#include <algorithm>
//#include <iostream>
#include "statistics.h"

void getMinMax(const std::vector<Float>& X, Float& minValue, Float& maxValue)
{
    minValue = maxValue = X[0];
    for (std::size_t i = 1; i < X.size(); ++i)
    {
        if (X[i] < minValue) minValue = X[i];
        if (X[i] > maxValue) maxValue = X[i];
    }
}

void histogram(const std::vector<Float>& X, const std::size_t binsCount,
               std::vector<Float>& hist, std::vector<Float>& binEdges)
{
    hist.resize(binsCount);
    binEdges.resize(binsCount + 1);
    std::fill(hist.begin(), hist.end(), 0.0);
    Float min, max, binWidth;
    getMinMax(X, min, max);
    binWidth = (max - min) / binsCount;
    for (std::size_t i = 0; i < X.size(); ++i)
    {
        std::size_t binIndex = std::size_t((X[i] - min) / binWidth);
        if (binIndex == binsCount) --binIndex;
        hist[binIndex] += 1.0;
    }
    for (std::size_t i = 0; i <= binsCount; ++i) binEdges[i] = min + binWidth * i;
}


std::vector<Float> getBinCenters(const std::vector<Float>& binEdges)
{
    std::vector<Float> binCenters;
    binCenters.resize(binEdges.size() - 1);
    for (std::size_t i = 0; i < binCenters.size(); ++i)
    {
        binCenters[i] = (binEdges[i] + binEdges[i + 1]) / 2.0;
    }
    return binCenters;
}


std::size_t getBinIndex(Float value, const std::vector<Float>& binEdges)
{
    std::size_t N = binEdges.size();
    for (std::size_t i = 0; i < N - 1; ++i)
    {
        if ((binEdges[i] <= value) && (value < binEdges[i + 1])) return i;
    }
    return N - 1;
}


std::pair<Float, Float> calculateMeanAndDispersionI(const std::vector<Float>& hist)
{
    Float m(0.0), m2(0.0);
    Float H = 0.0;
    for (std::size_t i = 0; i < hist.size(); ++i)
    {
        H += hist[i];
        m += i * hist[i];
        m2 += i * i * hist[i];
    }
    m /= H;
    m2 /= H;
    Float D = m2 - m * m;
    return std::pair<Float, Float>(m, D);
}


std::pair<Float, Float> calculateMeanAndDispersion(const std::vector<Float>& hist,
                                                   const std::vector<Float>& binCenters)
{
    Float m(0.0), m2(0.0);
    Float H = 0.0;
    for (std::size_t i = 0; i < hist.size(); ++i)
    {
        H += hist[i];
        m += binCenters[i] * hist[i];
        m2 += binCenters[i] * binCenters[i] * hist[i];
    }
    m /= H;
    m2 /= H;
    Float D = m2 - m * m;
    return std::pair<Float, Float>(m, D);
}


Float percentile(const std::vector<Float>& X, const Float alpha)
{
    std::vector<Float> Y(X.begin(), X.end());
    std::sort(Y.begin(), Y.end());
    Float i = alpha * (Y.size() - 1);
    std::size_t iFloor = std::size_t(floor(i));
    std::size_t iCeil = std::size_t(ceil(i));
    if (iFloor == iCeil) return Y[iCeil];
    else
    {
        Float pFloor = iCeil - i;
        Float pCeil = i - iFloor;
        return pFloor * Y[iFloor] + pCeil * Y[iCeil];
    }
}
