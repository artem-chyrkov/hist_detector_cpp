#ifndef _MIXTURESEPARATION_H_
#define _MIXTURESEPARATION_H_

#include <vector>
#include "statistics.h"

namespace MixtureSeparation
{

void algorithm2(const std::vector<Float>& hist,
                Float& m1, Float& m2, std::size_t& separateIndex);

}

#endif // _MIXTURESEPARATION_H_
