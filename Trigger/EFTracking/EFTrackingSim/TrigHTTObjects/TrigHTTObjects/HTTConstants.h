/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_CONSTANTS_H
#define TRIGHTTOBJECTS_CONSTANTS_H


#include <vector>
#define MTX_TOLERANCE 3e-16

namespace htt {

    const std::vector< double > TARGET_R_1STAGE = { 290.516, 396.066, 558.552, 564.953, 758.321, 764.665, 996.384, 1002.72 };
    const std::vector< double > TARGET_R_2STAGE = { 33.3024, 99.1959, 159.543, 227.638, 290.516, 396.066, 402.463, 558.552, 564.953, 758.321, 764.665, 996.384, 1002.72 };

    // --- This is the current HTTCluster to HTTHit scaling factor --- //
    constexpr float scaleHitFactor = 2;
    constexpr float DEG_TO_RAD = 0.017453292519943295;
}

#endif // TRIGHTTOBJECTS_CONSTANTS_H
