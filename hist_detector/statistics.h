#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include <vector>

#define Float double

void histogram(const std::vector<Float>& X, const std::size_t binsCount,
               std::vector<Float>& hist, std::vector<Float>& binEdges);

std::vector<Float> getBinCenters(const std::vector<Float>& binEdges);
std::size_t getBinIndex(Float value, const std::vector<Float>& binEdges);

std::pair<Float, Float> calculateMeanAndDispersionI(const std::vector<Float>& hist);

std::pair<Float, Float> calculateMeanAndDispersion(const std::vector<Float>& hist,
                                                   const std::vector<Float>& binCenters);

Float percentile(const std::vector<Float>& X, const Float alpha);

#endif // _STATISTICS_H_
