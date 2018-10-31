#include <math.h>
#include "HistDetector.h"
#include "CircularList.inl"
#include "mixtureSeparation.h"

//float histRange[] = { 0.0f, 256.0f };
//const float* HISTOGRAM_RANGE[] = { histRange };
//int channels[] = { 0 };


std::vector<Float> extractD(const HistDetector::PatchParams& patchParams)
{
    std::vector<Float> D;
    D.resize(patchParams.size());
    for (std::size_t i = 0; i < patchParams.size(); ++i) D[i] = patchParams[i].second;
    return D;
}

void splitPT(const CircularList<std::pair<Float, Float> >& P_T,
             std::vector<Float>& m, std::vector<Float>& D)
{
    std::size_t N = P_T.getActualSize();
    m.resize(N);
    D.resize(N);
    for (std::size_t i = 0; i < N; ++i)
    {
        std::pair<Float, Float> mD = P_T.get()[i];
        m[i] = mD.first;
        D[i] = mD.second;
    }
}

template<class T>
T max(const std::vector<T>& X)
{
    T m = X[0];
    for (std::size_t i = 1; i < X.size(); ++i) if (X[i] > m) m = X[i];
    return m;
}

template<class T>
T mean(const std::vector<T>& X)
{
    T m(0);
    for (std::size_t i = 0; i < X.size(); ++i) m += X[i];
    m /= X.size();
    return m;
}


HistDetector::HistDetector(int _binsCount, Float _alpha_D4,
                           Float _k1_alpha, Float _k2_alpha, Float _q_alpha) :
    binsCount(_binsCount), binWidth(256 / _binsCount), alpha_D4(_alpha_D4),
    k1_alpha(_k1_alpha), k2_alpha(_k2_alpha), q_alpha(_q_alpha)
{
}


void HistDetector::init(int ringListSize)
{
    P_T.init(ringListSize);
    P_F.init(ringListSize);
    clear();
}


void HistDetector::train(const HistDetector::PatchParams& patchParams)
{
    clear();
    std::vector<Float> D = extractD(patchParams);
    std::size_t N = D.size();
    std::vector<Float> histD, binEdgesD;
    histogram(D, std::size_t(sqrt(N)) + 1, histD, binEdgesD);
    std::vector<Float> binCentersD = getBinCenters(binEdgesD);
    Float m1, m2;
    std::size_t si;
    MixtureSeparation::algorithm2(histD, m1, m2, si);
    Float mD = calculateMeanAndDispersion(std::vector<Float>(histD.begin(),
                                                             histD.begin() + si),
                                          std::vector<Float>(binCentersD.begin(),
                                                             binCentersD.begin() + si)).first;
    D1 = k1_alpha * mD;

    D4 = percentile(D, alpha_D4);

    for (std::size_t i = 0; i < patchParams.size(); ++i)
    {
        if (D[i] < D1)  // [0..D1)
        {
            P_T.add(patchParams[i]);
        }
        else if (D[i] < D4) // [D1..D4)
        {
            P_F.add(D[i]);
        }
    }

    recalculateThresholds();
}


std::vector<Target> HistDetector::detect(const cv::Mat& frame, const std::vector<Roi>& rois)
{
    std::vector<Target> targets;
    for (std::vector<Roi>::const_iterator roi = rois.begin(); roi != rois.end(); ++roi)
    {
        cv::Mat patchBgr = frame(cv::Rect(roi->x, roi->y, roi->w, roi->h)), patchGray;
        cv::cvtColor(patchBgr, patchGray, cv::COLOR_BGR2GRAY);
        std::vector<Float> hist = calculateHistogram(patchGray);
        std::pair<Float, Float> m_D = calculateMeanAndDispersionI(hist);
        Float m = m_D.first, D = m_D.second;

        if (D >= D4)  // [D4; +inf)
        {
            targets.push_back(Target(*roi, SL_PROBABLY_SUSPICIOUS, m, m, D));
            continue;
        }

        if (D < D1)  // [0; D1)
        {
            P_T.add(m_D);
            continue;
        }

        Float m1, m2;  std::size_t si;
        MixtureSeparation::algorithm2(hist, m1, m2, si);
        if (fabs(m2 - m1) <= k2_alpha * sqrt(D))  // |m1 - m2| < dm
        {
            P_F.add(D);
            targets.push_back(Target(*roi, SL_UNKNOWN, m1, m2, D));
        }
        else  // check each mode
        {
            if (isFrequentMode(m1) && isFrequentMode(m2))
            {
            }
            else
            {
                targets.push_back(Target(*roi, SL_SUSPICIOUS, m1, m2, D));
            }
        }
    }

    recalculateThresholds();
    return targets;
}


void HistDetector::clear()
{
    P_T.clear();
    P_F.clear();
    D1 = delta_m = n_T = D4 = 0.0;
}


void HistDetector::recalculateThresholds()
{
    std::vector<Float> m_PT, D_PT;
    splitPT(P_T, m_PT, D_PT);
    D1 = max(D_PT);

    D_PT.insert(D_PT.end(), P_F.get().begin(), P_F.get().begin() + P_F.getActualSize());
    D4 = max(D_PT);

    histogram(m_PT, std::size_t(sqrt(m_PT.size())) + 1, hist_m_PT, binEdges_m_PT);
    n_T = q_alpha * mean(hist_m_PT);
}

bool HistDetector::isFrequentMode(const Float mi)
{
    if ((mi <= binEdges_m_PT[0]) ||
        (mi >= binEdges_m_PT[binEdges_m_PT.size() - 1])) return false;
    std::size_t miBinIndex =getBinIndex(mi, binEdges_m_PT);
    Float miFrequency = hist_m_PT[miBinIndex];
    return miFrequency > n_T;
}


std::vector<Float> HistDetector::calculateHistogram(const cv::Mat& patchGray) const
{
    std::vector<Float> histVector;
    histVector.resize(binsCount);
    std::fill(histVector.begin(), histVector.end(), 0.0);
    for (int y = 0; y < patchGray.rows; ++y)
    {
        for (int x = 0; x < patchGray.cols; ++x)
        {
            histVector[std::size_t(patchGray.at<unsigned char>(x, y) / binWidth)] += 1.0;
        }
    }
    return histVector;
}
