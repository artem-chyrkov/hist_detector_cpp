#ifndef _HISTDETECTOR_H_
#define _HISTDETECTOR_H_

#include <vector>
#include "opencv.hpp"  // TODO refactor (is needed for cv::Mat only)
#include "CircularList.h"
#include "statistics.h"
#include "SlidingStripeRois.h"


typedef enum { SL_NON_SUSPICIOUS, SL_UNKNOWN, SL_PROBABLY_SUSPICIOUS, SL_SUSPICIOUS } SuspicionLevel;

struct Target
{
    Target(const Roi& _roi, SuspicionLevel _suspicionLevel, Float _m1, Float _m2, Float _D) :
        roi(_roi), suspicionLevel(_suspicionLevel), m1(_m1), m2(_m2), D(_D) {}
    Roi roi;
    SuspicionLevel suspicionLevel;
    Float m1, m2, D;
};


class HistDetector
{
public:
    typedef std::vector<std::pair<Float, Float> > PatchParams;

public:
    HistDetector(int _binsCount, Float _alpha_D4,
                 Float _k1_alpha, Float _k2_alpha, Float _q_alpha);
    void init(int ringListSize);
    void train(const PatchParams& patchParams);
    std::vector<Target> detect(const cv::Mat& frame, const std::vector<Roi>& rois);
    std::vector<Float> calculateHistogram(const cv::Mat& patchGray) const;

private:
    void clear();
    void recalculateThresholds();
    bool isFrequentMode(const Float mi);

public://private:
    int binsCount;
    int binWidth;
    Float alpha_D4;
    Float k1_alpha;
    Float k2_alpha;
    Float q_alpha;

    Float D1;
    Float delta_m;
    Float n_T;
    Float D4;

    std::vector<Float> hist_m_PT, binEdges_m_PT;

    CircularList<std::pair<Float, Float> > P_T;  // (m, D) of textures
    CircularList<Float> P_F;  // D of fragments
};

#endif // _HISTDETECTOR_H_
