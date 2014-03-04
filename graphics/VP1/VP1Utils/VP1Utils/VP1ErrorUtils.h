/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1ErrorUtils                       //
//                                                            //
//  Description: Utilities for drawing errors for various     //
//               measurements                                 //
//                                                            //
//  Author: Troels Kofoed Jacobsen                            //
//  Initial version: July 2008                                //
//                                                            //
//  Update: Riccardo Maria BIANCHI Feb 2014                   //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1ERRORUTILS_H
#define VP1ERRORUTILS_H

//#include "CLHEP/Geometry/Point3D.h"
//#include "TrkParameters/MeasuredTrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"// EDM include(s):



typedef AmgSymMatrix(2) AmgCovMatrix;

class SoSeparator;
class SoGroup;
class SoLineSet;

namespace InDet { class PixelCluster; }
namespace Trk {
  class Surface;
  class CovarianceMatrix;
}

class VP1ErrorUtils
{
public:
  static void errorAtPlaneSurface( SoSeparator* errSimple,
                                   SoSeparator* errDetailed,
                                   const AmgSymMatrix(5)& tmpCovMat,
                                   const Trk::Surface* theSurface,
                                   const Amg::Vector3D& p1,
                                   const double& nStdDev=3.0,
                                   int numNodes = 12,
                                   const bool& moveToSurface = false,
                                   const bool& force1D=false,
                                   const bool& addTransform=true);

  static void errorAtStraightLineSurface( SoSeparator* errSimple, SoSeparator* errDetailed,
                                          const AmgSymMatrix(5)& tmpcovmat,
                                          const Amg::Vector2D& localPos,
                                          const Trk::Surface* theSurface,
                                          const double& nStdDev=3.0,
                                          const bool& drawZErrCircles=false,
                                          const bool& drawCylinder=false,
                                          int numNodes=12,
                                          const bool& force1D=false,
                                          const bool& addTransform=true);

  static void addRotatedErrorEllipse( SoGroup* gr,
                                      const AmgSymMatrix(5)& covmat,
                                      const double& sigmaScale,
                                      int numNodes=12,
                                      const double& translate=0.0,
                                      const bool& symmetric=false);
                                      
  static void addSimple1DError( SoGroup* gr,
                                const double& error,
                                const double& sigmaScale,
                                const double& length);
                                      
  static bool covMatrixIsSane( const AmgCovMatrix& covmat); //!< returns false if there is a problem detected with the cov matrix
  static bool covMatrixIsSane( const AmgSymMatrix(5)& covmat); //!< returns false if there is a problem detected with the cov matrix
};

#endif
