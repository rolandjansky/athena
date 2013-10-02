/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PerigeeFromVertexCreator.cxx,    (c) ATLAS Detector software  //
///////////////////////////////////////////////////////////////////

#include "TrkEventUtils/PerigeeFromVertexCreator.h"
#include <cmath>

double Trk::PerigeeFromVertexCreator::s_unitConversion = -0.3;
double Trk::PerigeeFromVertexCreator::s_magField       = 2.083;
              
Trk::Perigee Trk::PerigeeFromVertexCreator::createPerigee(Amg::Vector3D& vertex, 
                                                          Amg::Vector3D& momentum, 
                                                          double charge) const
{ 
  return createPerigee(vertex, momentum.phi(),momentum.theta(),charge/momentum.mag());
}

Trk::Perigee Trk::PerigeeFromVertexCreator::createPerigee(Amg::Vector3D& vertex,
                                                          double phi,
                                                          double theta,
                                                          double qOverP) const
{
  // calculate perigee
  double sinp = sin(phi);
  double cosp = sin(phi);
  double R = vertex[1]*cosp - vertex[0]*sinp;
  double Q = vertex[1]*sinp + vertex[0]*cosp;
  double rho = s_unitConversion*m_field*qOverP/sin(theta);
  double d0 = R + pow(Q,2.)*rho/2.;
  double z0 = vertex[2] - Q*(1-R*rho)/tan(theta);
  double phi0 = phi - Q*rho;;

  // check phi0 for [-Pi,+Pi) range
  while (phi0 > M_PI) phi0 -= M_PI;
  while (phi0 < -M_PI) phi0 += M_PI;

  return Trk::Perigee(d0,z0,phi0,theta,qOverP,PerigeeSurface());
}
             
Trk::Perigee* Trk::PerigeeFromVertexCreator::createNewPerigee(Amg::Vector3D& vertex, 
                                                              Amg::Vector3D& momentum, 
                                                               double charge) const
{
  return createNewPerigee(vertex, momentum.phi(),momentum.theta(),charge/momentum.mag());
}

Trk::Perigee* Trk::PerigeeFromVertexCreator::createNewPerigee(Amg::Vector3D& vertex,
                                                              double phi,
                                                              double theta,
                                                              double qOverP) const
{
  // calculate perigee
  double sinp = sin(phi);
  double cosp = sin(phi);
  double R = vertex[1]*cosp - vertex[0]*sinp;
  double Q = vertex[1]*sinp + vertex[0]*cosp;
  double rho = s_unitConversion*m_field*qOverP/sin(theta);
  double d0 = R + pow(Q,2.)*rho/2.;
  double z0 = vertex[2] - Q*(1-R*rho)/tan(theta);
  double phi0 = phi - Q*rho;;

  // check phi0 for [-Pi,+Pi) range
  while (phi0 > M_PI) phi0 -= M_PI;
  while (phi0 < -M_PI) phi0 += M_PI;

  return new Trk::Perigee(d0,z0,phi0,theta,qOverP,PerigeeSurface());
}

