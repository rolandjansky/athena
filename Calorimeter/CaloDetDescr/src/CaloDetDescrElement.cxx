/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloDetDescr/CaloDetDescrElement.h"

#include <iostream>
#include <iomanip>
#include <cmath>

CaloDetDescrElement::CaloDetDescrElement(const IdentifierHash subcaloHash,
                                         const IdentifierHash onl1,
                                         const IdentifierHash onl2,
                                         const CaloDetDescriptor* descriptor)
  : m_descriptor(descriptor)
  , m_caloHash(descriptor
                 ? IdentifierHash(subcaloHash + descriptor->caloCellMin())
                 : subcaloHash)
  , m_eta(0.)
  , m_phi(0.)
  , m_sinTh(0.)
  , m_cosTh(0.)
  , m_deta(0.)
  , m_dphi(0.)
  , m_volume(0.)
  , m_sinPhi(0.)
  , m_cosPhi(0.)
  , m_r(0.)
  , m_eta_raw(0.)
  , m_phi_raw(0.)
  , m_r_raw(0.)
  , m_dr(0.)
  , m_x(0.)
  , m_y(0.)
  , m_z(0.)
  , m_x_raw(0.)
  , m_y_raw(0.)
  , m_z_raw(0.)
  , m_dx(0.)
  , m_dy(0.)
  , m_dz(0.)
  , m_onl1(onl1)
  , m_onl2(onl2)
{
  m_sample=CaloCell_ID::Unknown;
  if(is_tile()) {
    // Tile
    m_sample = (CaloCell_ID::CaloSample)(
      m_descriptor->getSampling() +
      m_descriptor->get_calo_helper()->tile_idHelper()->sampling(identify()));
  } else if (m_descriptor) {
    // LAr
    m_sample=m_descriptor->getSampling();
  } else{
    // MBTS
    m_sample=CaloCell_ID::Unknown;
  }
}

CaloDetDescrElement::~CaloDetDescrElement () 
{
}

Identifier
CaloDetDescrElement::identify() const
{
  if(m_descriptor){
    return (m_descriptor->get_calo_helper())->cell_id(calo_hash());
  }
  else{ // MBTS
    return (customID());
  }
}

void 			
CaloDetDescrElement::print() const
{    
    std::cout << std::endl << " CaloDetDescrElement print: " 
	      << std::endl << std::endl;
    
    std::cout << "Eta        = " << m_eta << std::endl;
    std::cout << "Phi        = " << m_phi << std::endl;
    std::cout << "r          = " << m_r << std::endl;
}

int CaloDetDescrElement::getLayer() const
{
  return 0;
}

bool CaloDetDescrElement::is_lar_em() const
{ 
  if(m_descriptor){
    return m_descriptor->is_lar_em(); 
  }
  return false;
} 

bool CaloDetDescrElement::is_lar_em_barrel() const 
{ 
  if(m_descriptor){
    return m_descriptor->is_lar_em_barrel(); 
  }
  return false;
} 

bool CaloDetDescrElement::is_lar_em_endcap() const 
{ 
  if(m_descriptor){
    return m_descriptor->is_lar_em_endcap(); 
  }
  return false;
} 

bool CaloDetDescrElement::is_lar_em_endcap_inner() const 
{ 
  if(m_descriptor){
    return m_descriptor->is_lar_em_endcap_inner(); 
  }
  return false;
} 

bool CaloDetDescrElement::is_lar_em_endcap_outer() const 
{ 
  if(m_descriptor){
    return m_descriptor->is_lar_em_endcap_outer(); 
  }
  return false;
} 

bool CaloDetDescrElement::is_lar_hec() const
{ 
  if(m_descriptor){
    return m_descriptor->is_lar_hec(); 
  }
  return false;
} 

bool CaloDetDescrElement::is_lar_fcal() const 
{ 
  if(m_descriptor){
    return m_descriptor->is_lar_fcal(); 
  }
  return false;
}

void
CaloDetDescrElement::set_online(const IdentifierHash onl1,
                                const IdentifierHash onl2)
{
  m_onl1 = onl1;
  m_onl2 = onl2;
}

void CaloDetDescrElement::propagateRaw()
{
  m_x = m_x_raw;
  m_y = m_y_raw;
  m_z = m_z_raw;

  m_eta = m_eta_raw;
  m_phi = m_phi_raw;
  m_r   = m_r_raw;
}

Identifier CaloDetDescrElement::customID() const
{
  return Identifier();
}
