// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *
 * New file for VP1HEPVis
 *
 * taken from: http://fossies.org/dox/osc_vis_source_16.11.6/HEPVis_2include_2HEPVis_2SbRotation_8h_source.html
 *
 * R.M. Bianchi <rbianchi@cern.ch>
 *
 * 12.12.2012
 *
 *
 */

//---

#ifndef HEPVis_SbRotation_h
#define HEPVis_SbRotation_h

// Code taken from CoinGL/SbRotation but by using doubles instead of floats.
// It is used by SbPolyhedron::Transform.

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbVec4d.h>

class SbVec3d;

namespace HEPVis {

class SbRotation {
public:
  SbRotation();
  SbRotation(const SbVec3d& axis,double radians);
  SbRotation(double a11, double a12,double a13, double a14,
             double a21, double a22,double a23, double a24,
             double a31, double a32,double a33, double a34,
             double a41, double a42,double a43, double a44);
public:
  //SbRotation& operator*=(const double s);
  void multVec(const SbVec3d& src,SbVec3d& dst) const;
private:
  SbVec4d m_quat;
};

}

#endif
