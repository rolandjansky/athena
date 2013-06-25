/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * VP1HEPVis
 * SbPlane.cxx
 *
 *  Created on: Dec 13, 2012
 *      Author: rbianchi <Riccardo.Maria.Bianchi@cern.ch>
 *
 */


/*
 *
 * New file for HEPVis
 *
 * taken from: http://fossies.org/dox/osc_vis_source_16.11.6/HEPVis_2include_2HEPVis_2SbPlane_8cxx_source.html
 *
 * R.M. Bianchi <rbianchi@cern.ch>
 *
 * 12.12.2012
 *
 *===================
 * VP1 Customization:
 *
 *  - look into the code for comments "// VP1 change [...] //---"
 *
 */

//---


// this :
// VP1 change
//#include <HEPVis/SbPlane.h>
#include <VP1HEPVis/SbPlane.h>
//---


HEPVis::SbPlane::SbPlane(void) //private to avoid problems.
//NOTE : CoinGL/SbPlane() does not initialize the fields !
:m_normal(0,0,1) //CLHEP initialize with x-y plane (normal to z and distance 0)
,m_distance(0)
{}
HEPVis::SbPlane::SbPlane(const SbVec3d& a_normal,double a_D) {
  m_normal = a_normal;
  m_normal.normalize();
  m_distance = a_D;
  //NOTE : equation of the plan is then : n[0]*x+n[1]*y+n[2]*z-distance = 0
}
HEPVis::SbPlane::SbPlane(const SbVec3d& a_normal,const SbVec3d& a_point) {
  //FIXME if(normalref.sqrLength()==0) //throw
  m_normal = a_normal;
  m_normal.normalize();
  m_distance = m_normal.dot(a_point);
}

/*
HEPVis::SbPlane::SbPlane(double a,double b,double c,double d) {
  // To follow CLHEP.
  // Constructor from four numbers - creates plane a*x+b*y+c*z+d=0.
  normal.setValue(a,b,c);
  normal.normalize();
  distance = -d; //NOTE : yes, it is a minus.
}
*/
double HEPVis::SbPlane::getDistance(const SbVec3d& a_point) const {
  return a_point.dot(m_normal) - m_distance;
}
double HEPVis::SbPlane::distance(const SbVec3d& a_point) const {
  return getDistance(a_point);
}
const SbVec3d& HEPVis::SbPlane::getNormal() const { return m_normal;}
double HEPVis::SbPlane::getDistanceFromOrigin(void) const { return m_distance;}


