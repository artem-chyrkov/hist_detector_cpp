#include "mixtureSeparation.h"
#include <limits>

std::size_t argmax(const std::vector<Float>& X)
{
    std::size_t res = 0;
    Float max = X[0];
    for (std::size_t i = 1; i < X.size(); ++i)
    {
        if (X[i] > max)
        {
            max = X[i];
            res = i;
        }
    }
    return res;
}

namespace MixtureSeparation
{

void algorithm2(const std::vector<Float>& hist,
                Float& m1, Float& m2, std::size_t& separateIndex)
{
    std::size_t N = hist.size();
    separateIndex = 3;
    m1 = m2 = 0.0;
    Float f = std::numeric_limits<Float>::infinity();
    std::size_t startIndex = separateIndex;
    for (std::size_t si = startIndex; si < N - startIndex; ++si)
    {
        std::vector<Float> histLeft(hist.begin(), hist.begin() + si);
        std::vector<Float> histRight(hist.begin() + si, hist.end());
        std::pair<Float, Float> m_D_left = calculateMeanAndDispersionI(histLeft);
        std::pair<Float, Float> m_D_right = calculateMeanAndDispersionI(histRight);
        Float fi = m_D_left.second + m_D_right.second;
        if (fi < f)
        {
            f = fi;
            Float mLeft = argmax(histLeft);
            Float mRight = argmax(histRight);
            separateIndex = si;
            m1 = mLeft;
            m2 = mRight + si;
        }
    }
}

}
