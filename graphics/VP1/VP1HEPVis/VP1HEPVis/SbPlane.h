// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *
 * New file for VP1HEPVis
 *
 * taken from: http://fossies.org/dox/osc_vis_source_16.11.6/HEPVis_2include_2HEPVis_2SbPlane_8h_source.html
 *
 * R.M. Bianchi <rbianchi@cern.ch>
 *
 * 12.12.2012
 *
 *===================
 * VP1 Customization:
 *
 *  - look into the code for comments "// VP1 change"
 *
 */

//---


#ifndef HEPVis_SbPlane_h
#define HEPVis_SbPlane_h

/*
 * An SbPlane class that uses doubles instead of floats.
 * Used by the BooleanProcessor.
 */

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbVec3d.h>

namespace HEPVis {

class SbPlane {
public:
  SbPlane();
  SbPlane(const SbVec3d& normal,double D);
  SbPlane(const SbVec3d& normal,const SbVec3d& point);
  // To follow CLHEP.
  // Constructor from four numbers - creates plane a*x+b*y+c*z+d=0.
  //SbPlane(double a,double b,double c,double d);
  double getDistance(const SbVec3d& point) const;
  const SbVec3d& getNormal() const;
  double getDistanceFromOrigin() const;
public:
  double distance(const SbVec3d& point) const; //CLHEP compatibility
private:
  SbVec3d m_normal;
  double m_distance;
};

}

#endif
