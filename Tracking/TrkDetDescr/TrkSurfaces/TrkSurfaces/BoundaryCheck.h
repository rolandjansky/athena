/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BoundaryCheck.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_BOUNDARYCHECK_H
#define TRKSURFACES_BOUNDARYCHECK_H

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
namespace Trk {

/**
 @class BoundaryCheck

 The BoundaryCheck class allows to steer the way surface
 boundaries are used for inside/outside checks of
 parameters.

 These checks are performed in the local 2D frame of the
 surface and can either be:
 - inside/outside with and without tolerance
 - inside/outside according to a given chi2 value

     It also provides all the necessary tools for the individual implementations in the different SurfaceBounds classes.

 @author Andreas.Salzburger@cern.ch & Roland.Jansky@cern.ch
 */

// maint struct for comparing the extent of arbitrary convex polygons relative to prefixed axes
struct KDOP
{
  float min;
  // Minimum distance (from origin) along axes
  float max;
  // Maximum distance (from origin) along axes
};

// struct needed for FastSinCos method (see below)
struct sincosCache
{
  double sinC;
  double cosC;
};

class BoundaryCheck
{
  // saves us a lot of function calls in the EllipseToPoly method
  static constexpr double s_cos22 = 0.923879532511286756128183189396788286822416625863642486115097;
  static constexpr double s_cos45 = 0.707106781186547524400844362104849039284835937688474036588339;
  static constexpr double s_cos67 = 0.382683432365089771728459984030398866761344562485627041433800;

public:
  enum BoundaryCheckType
  {
    absolute = 0, //!< absolute check including tolerances
    chi2corr = 1  //!< relative (chi2 based) with full correlations
  };

  bool checkLoc1; //!< check local 1 coordinate
  bool checkLoc2; //!< check local 2 coordinate

  double toleranceLoc1; //!< absolute tolerance in local 1 coordinate
  double toleranceLoc2; //!< absolute tolerance in local 2 coordinate

  int nSigmas;                 //!< allowed sigmas for chi2 boundary check
  AmgSymMatrix(2) lCovariance; //!< local covariance matrix

  BoundaryCheckType bcType;

  /** Constructor for single boolean behavious */
  BoundaryCheck(bool sCheck)
    : checkLoc1(sCheck)
    , checkLoc2(sCheck)
    , toleranceLoc1(0.)
    , toleranceLoc2(0.)
    , nSigmas(-1)
    , lCovariance(AmgSymMatrix(2)::Identity())
    , bcType(absolute)
  {}

  /** Constructor for tolerance based check */
  BoundaryCheck(bool chkL1, bool chkL2, double tloc1 = 0., double tloc2 = 0.)
    : checkLoc1(chkL1)
    , checkLoc2(chkL2)
    , toleranceLoc1(tloc1)
    , toleranceLoc2(tloc2)
    , nSigmas(-1)
    , lCovariance(AmgSymMatrix(2)::Identity())
    , bcType(absolute)
  {}

  /** Constructor for chi2 based check */
  BoundaryCheck(const AmgSymMatrix(2) & lCov, int nsig = 1, bool chkL1 = true, bool chkL2 = true)
    : checkLoc1(chkL1)
    , checkLoc2(chkL2)
    , toleranceLoc1(0.)
    , toleranceLoc2(0.)
    , nSigmas(nsig)
    , lCovariance(lCov)
    , bcType(chi2corr)
  {}

  /** Conversion operator to bool */
  operator bool() const { return (checkLoc1 || checkLoc2); }

  /** Each Bounds has a method inside, which checks if a LocalPosition is inside the bounds.
            Inside can be called without/with boundary check */
  void ComputeKDOP(const std::vector<Amg::Vector2D> &v, const std::vector<Amg::Vector2D> &KDOPAxes, 
                   std::vector<KDOP>& kdop) const;

  std::vector<Amg::Vector2D> EllipseToPoly(int resolution = 3) const;

  bool TestKDOPKDOP(const std::vector<KDOP>& a, const std::vector<KDOP>& b) const;

  double FastArcTan(double x) const;

  sincosCache FastSinCos(double x) const;
};

}

#include "TrkSurfaces/BoundaryCheck.icc"
#endif // TRKSURFACES_BOUNDARYCHECK_H
