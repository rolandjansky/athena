/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DetElementsRoadUtils_xk_h
#define TRT_DetElementsRoadUtils_xk_h

#include "TRT_ReadoutGeometry/TRT_BaseElement.h"

///////////////////////////////////////////////////////////////////
// Output parameters: P[ 0]  - radius          centre of wafer
//                    P[ 1]  - z               centre of wafer
//                    P[ 2]  - azimuthal angle centra of wifer
//                    P[ 3]  - min. distance to surface
//                    P[ 4]  - azimythal angle normal to surface
//                    P[ 5]  - sin(P[4])
//                    P[ 6]  - cos(P[4])
//                    P[ 7]  - sin(Polar angle)
//                    P[ 8]  - cos(Polar abgle)
//                    P[ 9]  - min. radius   
//                    P[10]  - max. radius   
//                    P[11]  - min. z
//                    P[12]  - max  z  
//                    P[13]  - min. azimythal angle  
//                    P[14]  - max. azimuthal angle
//                    P[15]  - tilt angle
//                    P[16]  - min. dist to surface in XY-plane
//                    P[17]  - Tilt angle
//                    P[18]  - first  local coordinate of wafer
//                    P[19]  - second local coordinate of wafer 
//               P[20],P[21] - vector to -F  boundary 
//               P[22],P[23] - vector to +ZR boundary 
//               P[24],P[25] - vector to +F  boundary 
//               P[26],P[27] - vector to -ZR boundary 
//                    P[28]  - thikness of the detector element
//                    P[29]  - Ax of Phi exis
//                    P[30]  - Ay of Phi exis
//                    P[31]  - Ax of Eta exis
//                    P[32]  - Ay of Eta exis
///////////////////////////////////////////////////////////////////

namespace InDet {
  namespace TRT_DetElementsRoadUtils_xk {
    void detElementInformation(const InDetDD::TRT_BaseElement& Si, double* P);
  }
}

#endif // TRT_DetElementsRoadUtils_xk_h
