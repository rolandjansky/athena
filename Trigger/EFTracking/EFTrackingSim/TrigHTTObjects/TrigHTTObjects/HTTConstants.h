/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_CONSTANTS_H
#define TRIGHTTOBJECTS_CONSTANTS_H


#include <vector>
#define MTX_TOLERANCE 3e-16

namespace htt {

    // "Idealised" radius values for each of the detector barrel layers
    const std::vector< double > TARGET_R_1STAGE = { 290.516, 396.066, 558.552, 564.953, 758.321, 764.665, 996.384, 1002.72 };
    const std::vector< double > TARGET_R_2STAGE = { 33.3024, 99.1959, 159.543, 227.638, 290.516, 396.066, 402.463, 558.552, 564.953, 758.321, 764.665, 996.384, 1002.72 };

    const std::vector< double > QOVERPT_BINS = { -0.001, -0.0009, -0.00075, -0.0006, -0.00045, -0.0003, -0.00015, -0.000075, 0.000075, 0.00015, 0.0003, 0.00045, 0.0006, 0.00075, 0.0009, 0.001};

    // --- This is the current HTTCluster to HTTHit scaling factor --- //
    constexpr float scaleHitFactor = 2;
    constexpr float DEG_TO_RAD = 0.017453292519943295;

    static constexpr double A = 0.0003; // for Hough Transform

}

#endif // TRIGHTTOBJECTS_CONSTANTS_H
