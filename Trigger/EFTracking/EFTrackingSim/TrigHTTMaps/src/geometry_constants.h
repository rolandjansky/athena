// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef GEOMETRY_CONSTANTS_H
#define GEOMETRY_CONSTANTS_H

/**
 * @file geometry_constants.h
 * @author Will Kalderon
 * @date May 11, 2021
 * @brief file to define various geometry constants
 */

namespace geometry_constants
{
  // inner radii of endcap strips
  // from strip TDR (ATLAS-TDR-025) table 3.2 on page 29
  const std::vector<float> strip_endcap_innerRadii {
    384.5, 403.5, 427.5, 456.4,
    489.8, 507.9, 535.0, 559.1,
    575.6, 606.4,
    638.6, 670.8, 697.1, 723.3,
    756.9, 811.5,
    867.5, 907.6,
    967.8 // this is the outer radius of the outer row of the outer ring
    };
  
  const std::vector<int> strip_endcap_nRows = {4,4,2,4,2,2};

}

#endif // GEOMETRY_CONSTANTS_H
