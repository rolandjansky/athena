/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calorimeter LVL1 TT description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloTTDescrRegion.cxx,v 1.6 2006-07-21 12:39:15 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "CaloTTDetDescr/CaloTTDescrRegion.h"
#include <iostream>

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>


Identifier
CaloTTDescrRegion::identify() const
{
    return m_id;
}

void 			
CaloTTDescrRegion::set_spherical(    double     eta, 
				     double     phi, 
				     double     rho)
{
  m_eta = eta ; 
  m_phi= phi;
  m_rho = rho;

  // from this recompute cartesian coordinates 
  m_x = m_rho*cos(m_phi);
  m_y = m_rho*sin(m_phi);
  m_z = m_rho*sinh(m_eta);

 }

void 			
CaloTTDescrRegion::set_spherical_size( double     deta,
				       double     dphi,
				       double     drho )
{
  m_deta = deta; 
  m_dphi = dphi;
  m_drho = drho;  

  // from this recompute size in cartesian... to be completed !!!

  double dx = 0.;
  double dy = 0.;
  double dz = 0.;
  if(m_rho < 1600) {
    dz = m_drho * fabs(sinh(m_eta)) + m_deta * m_rho * cosh(m_eta);
  } else {
    dz = m_drho * fabs(sinh(m_eta));
  }

  m_dx = fabs(dx);
  m_dy = fabs(dy);
  m_dz = fabs(dz);

 }

void 			
CaloTTDescrRegion::set_cylindric(    double     eta, 
				     double     phi, 
				     double     z)
{
  m_eta = eta ; 
  m_phi = phi;
  m_z   = z;

  // from this recompute complementary set of coordinates 
  m_rho = m_z/sinh(m_eta);
  m_x   = m_rho*cos(m_phi);
  m_y   = m_rho*sin(m_phi);

 }

void 			
CaloTTDescrRegion::set_cylindric_size( double     deta,
				       double     dphi,
				       double     dz )
{
  m_deta = deta; 
  m_dphi = dphi;
  m_dz   = dz;  

  // from this recompute size in complementary set of coord. ... to be completed !!!

  double dx   = 0.;
  double dy   = 0.;
  double drho = (-m_z*cosh(m_eta)/sinh(m_eta)*m_deta
                 +m_dz) / sinh(m_eta);

  m_dx   = fabs(dx);
  m_dy   = fabs(dy);
  m_drho = fabs(drho);
      
 }

void 			
CaloTTDescrRegion::set_cartesian ( double     x, 
				   double     y, 
				   double     z )
{
  m_x       = x;
  m_y       = y;
  m_z       = z;

  // from this recompute (semi)spherical coordinates 
  m_rho = sqrt ( m_x*m_x+m_y*m_y) ;
  double big_r = sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
  m_eta = 0.5*log((big_r+m_z)/(big_r-m_z));
  m_phi = atan2(m_y,m_x);
}

void 			
CaloTTDescrRegion::set_cartesian_size ( double     dx, 
					double     dy,
					double     dz )
{
  m_dx       = dx;
  m_dy       = dy;
  m_dz       = dz;
  
  // from this recompute size in (semi)spherical... TBD !!!
  m_deta = 0.;
  m_dphi = 0.;
  m_drho = 0.;
}

void 			
CaloTTDescrRegion::print		() const
{

    std::cout << std::endl << " CaloTTDescrRegion print: " 
	      << std::endl << std::endl;
    
    m_id.show();
    m_descriptor->print();
}

