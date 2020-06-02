/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class  RungeKuttaUtils, (c) ATLAS Detector software
//  author Igor Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef RungeKuttaUtils_H
#define RungeKuttaUtils_H

#include "CxxUtils/restrict.h"
#include "GaudiKernel/AlgTool.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include <map>
#include <vector>

namespace Trk {

class ConeSurface;
class DiscSurface;
class PlaneSurface;
class CylinderSurface;
class StraightLineSurface;
class PatternTrackParameters;

/**
@namespace RungeKuttaUtils

Trk::RungeKuttaUtils is set algorithms for track parameters transformation
from local to global and global to local system coordinate and step
estimation to surface.

  AtaPlane    AtaStraightLine      AtaDisc       AtaCylinder      Perigee
     |               |               |               |              |
     |               |               |               |              |
     V               V               V               V              V
     -----------------------------------------------------------------
                                     |              Local->Global transformation
                                     V
                  Global position (Runge Kutta presentation)
                                     |
                                     V              Global->Local transformation
     ----------------------------------------------------------------
     |               |               |               |              |
     |               |               |               |              |
     V               V               V               V              V
 PlaneSurface StraightLineSurface DiscSurface CylinderSurface PerigeeSurface

For using Runge Kutta method we use global coordinate, direction,
inverse momentum and Jacobian of transformation. All this parameters we save
in array P[42].
                 /dL0    /dL1    /dPhi   /dThe   /dCM
X  ->P[0]  dX /   P[ 7]   P[14]   P[21]   P[28]   P[35]
Y  ->P[1]  dY /   P[ 8]   P[15]   P[22]   P[29]   P[36]
Z  ->P[2]  dZ /   P[ 9]   P[16]   P[23]   P[30]   P[37]
Ax ->P[3]  dAx/   P[10]   P[17]   P[24]   P[31]   P[38]
Ay ->P[4]  dAy/   P[11]   P[18]   P[25]   P[32]   P[39]
Az ->P[5]  dAz/   P[12]   P[19]   P[26]   P[33]   P[40]
CM ->P[6]  dCM/   P[13]   P[20]   P[27]   P[34]   P[41]

where
     in case local presentation

     L0  - first  local coordinate  (surface dependent)
     L1  - second local coordinate  (surface dependent)
     Phi - Azimuthal angle
     The - Polar     angle
     CM  - charge/momentum

     in case global presentation

     X   - global x-coordinate        = surface dependent
     Y   - global y-coordinate        = surface dependent
     Z   - global z-coordinate        = sutface dependent
     Ax  - direction cosine to x-axis = Sin(The)*Cos(Phi)
     Ay  - direction cosine to y-axis = Sin(The)*Sin(Phi)
     Az  - direction cosine to z-axis = Cos(The)
     CM  - charge/momentum            = local CM

@author Igor.Gavrilenko@cern.ch
**/

namespace RungeKuttaUtils
{

  /////////////////////////////////////////////////////////////////////////////////
  // Step estimators to surface
  /////////////////////////////////////////////////////////////////////////////////

  double stepEstimator(int, double*, const double*, bool&);
  double stepEstimatorToCone(double* ATH_RESTRICT, const double* ATH_RESTRICT, bool&);
  double stepEstimatorToPlane(const double* ATH_RESTRICT, const double* ATH_RESTRICT, bool&);
  double stepEstimatorToCylinder(double* ATH_RESTRICT, const double* ATH_RESTRICT, bool&);
  double stepEstimatorToStraightLine(const double* ATH_RESTRICT, const double* ATH_RESTRICT, bool&);

  /////////////////////////////////////////////////////////////////////////////////
  // Step estimators to surfaces
  /////////////////////////////////////////////////////////////////////////////////
  std::pair<double, int> stepEstimator(std::vector<std::pair<const Trk::Surface*, Trk::BoundaryCheck>>&,
                                       std::multimap<double, int>&,
                                       const double*,
                                       const double*,
                                       double,
                                       double,
                                       int Nv,
                                       bool&);

  int fillDistancesMap(std::vector<std::pair<const Trk::Surface*, Trk::BoundaryCheck>>&,
                       std::multimap<double, int>&,
                       const double* ATH_RESTRICT,
                       double,
                       const Trk::Surface*,
                       double* ATH_RESTRICT);

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from local to global system coordinates
  // for Trk::TrackParameters and Trk::NeutralParameters
  /////////////////////////////////////////////////////////////////////////////////

  bool transformLocalToGlobal(bool, const Trk::TrackParameters&, double*);

  bool transformLocalToGlobal(bool, const Trk::NeutralParameters&, double*);

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from local to global system coordinates
  // for Trk::PatternTrackParameters
  /////////////////////////////////////////////////////////////////////////////////

  bool transformLocalToGlobal(bool, const Trk::PatternTrackParameters&, double*);

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from global to local system coordinates
  /////////////////////////////////////////////////////////////////////////////////

  void transformGlobalToLocal(double* ATH_RESTRICT, double* ATH_RESTRICT);
  void transformGlobalToLocal(const Trk::Surface*,
                              bool,
                              double* ATH_RESTRICT,
                              double* ATH_RESTRICT,
                              double* ATH_RESTRICT);
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  // Covariance matrix production for Trk::TrackParameters
  /////////////////////////////////////////////////////////////////////////////////

  AmgSymMatrix(5)* newCovarianceMatrix(const double*, const AmgSymMatrix(5) &);

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from curvilinear to global system coordinates
  // covariance matrix only
  /////////////////////////////////////////////////////////////////////////////////

  void transformCurvilinearToGlobal(double* ATH_RESTRICT, double* ATH_RESTRICT);

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from global to curvilinear system coordinates
  // covariance matrix only
  /////////////////////////////////////////////////////////////////////////////////

  void transformGlobalToCurvilinear(bool, double* ATH_RESTRICT, double* ATH_RESTRICT, double* ATH_RESTRICT);

  /////////////////////////////////////////////////////////////////////////////////
  // Jacobian of transformations from curvilinear to local system coordinates
  /////////////////////////////////////////////////////////////////////////////////

  void jacobianTransformCurvilinearToLocal(const Trk::TrackParameters&, double*);

  void jacobianTransformCurvilinearToLocal(const Trk::PatternTrackParameters&, double*);

  void jacobianTransformCurvilinearToLocal(double* ATH_RESTRICT, const Trk::Surface*, double* ATH_RESTRICT);

  void jacobianTransformCurvilinearToDisc(double* ATH_RESTRICT, double* ATH_RESTRICT);
  void jacobianTransformCurvilinearToPlane(double* ATH_RESTRICT, double* ATH_RESTRICT);
  void jacobianTransformCurvilinearToCylinder(double* ATH_RESTRICT, double* ATH_RESTRICT);
  void jacobianTransformCurvilinearToStraightLine(const double* ATH_RESTRICT, double* ATH_RESTRICT);

  bool transformLocalToGlobal(bool, const Trk::Surface*, const double* ATH_RESTRICT, double* ATH_RESTRICT);
}
}

#endif // RungeKuttaUtils_H
