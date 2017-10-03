/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Liquid Argon detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloTTDescriptor.cxx,v 1.5 2006-02-15 09:05:19 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "CaloTTDetDescr/CaloTTDescriptor.h"

//FLG#include "CaloIdentifier/CaloLVL1_ID.h"

#include <iostream>
#include <iomanip>

CaloTTDescriptor::CaloTTDescriptor(float eta_min,   float eta_max,   float deta, 
				   float phi_min,   float phi_max,   float dphi,
				   int  sign_eta,   short n_lay)
    : 
    m_sign_eta	(sign_eta),
    m_eta_min	(eta_min),
    m_eta_max	(eta_max),
    m_deta	(deta),
    m_phi_min	(phi_min),
    m_phi_max	(phi_max),
    m_dphi	(dphi),
    m_nEta      ((short) ((eta_max - eta_min)/deta + 0.501)),
    m_nPhi      ((short) ((phi_max - phi_min)/dphi + 0.501)),
    m_nLay      (n_lay)
{
}

CaloTTDescriptor::~CaloTTDescriptor()
{}

void	
CaloTTDescriptor::print	() const
{
    std::cout << std::endl << " CaloTTDescriptor print: " 
	      << std::endl << std::endl;
    
    // Print out id
    m_id.show();
    
    std::cout << " Calo LVL1 Trigger Towers: " << std::endl;
    std::cout << "  eta min  eta max     deta   phi min   phi max      dphi   nLay"
	      << std::endl;
    std::cout << std::setiosflags(std::ios::fixed);
    std::cout << std::setw(9) << std::setprecision(4) << m_eta_min << " " 
	      << std::setw(9) << std::setprecision(4) << m_eta_max << " " 
	      << std::setw(9) << std::setprecision(4) << m_deta    << " " 
	      << std::setw(9) << std::setprecision(4) << m_phi_min << " " 
	      << std::setw(9) << std::setprecision(4) << m_phi_max << " " 
	      << std::setw(9) << std::setprecision(4) << m_dphi    << " "
	      << std::setw(9) << std::setprecision(4) << m_nLay    << " "
	      << std::endl;	    
    
    return;
}

    
void 
CaloTTDescriptor::set (float               eta_min,        
		       float               eta_max,
		       float               deta,
		       float               phi_min,
		       float               phi_max,
		       float               dphi,
		       int                 sign_eta,
		       short               n_lay)
{
  m_eta_min  = eta_min;
  m_eta_max  = eta_max;
  m_deta     = deta;
  m_phi_min  = phi_min;
  m_phi_max  = phi_max;
  m_dphi     = dphi;
  m_nEta     = (short) ((eta_max - eta_min)/deta + 0.501);
  m_nPhi     = (short) ((phi_max - phi_min)/dphi + 0.501);
  m_sign_eta = sign_eta;
  m_nLay = n_lay;
}


    








