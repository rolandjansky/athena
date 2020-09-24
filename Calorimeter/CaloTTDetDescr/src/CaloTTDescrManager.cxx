/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calo description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloTTDescrManager.cxx,v 1.8 2006-07-21 12:39:14 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescrRegion.h"
#include "CaloTTDetDescr/CaloTTDescriptor.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include <iostream>



CaloTTDescrManager::CaloTTDescrManager() 
    :
    m_is_initialized(false),
    m_calo_lvl1_id(nullptr)
{
}

CaloTTDescrManager::~CaloTTDescrManager() 
{
//  Remove regions and descriptors
    clear();
}

CaloTTDescrRegion*			
CaloTTDescrManager::find_calo_region	(const Identifier& region) const
{
  calo_region_map::const_iterator p = m_calo_region_map.find(region);
  if(p != m_calo_region_map.end()) {
    return p->second;
  } else {
    // no CaloTTDescrRegion corresponding to Identifier
    // this is expected as some Identifiers have no real counter part)
    return(nullptr);
  }
   
}

CaloTTDescrManager::calo_region_const_iterator
CaloTTDescrManager::calo_region_begin	() const
{
    return m_calo_region_vec.begin();
}

CaloTTDescrManager::calo_region_const_iterator
CaloTTDescrManager::calo_region_end	() const
{
    return m_calo_region_vec.end();
}

CaloTTDescrManager::calo_region_vec_size
CaloTTDescrManager::calo_region_size	() const
{
    return m_calo_region_vec.size();
}

CaloTTDescrManager::calo_descr_const_iterator
CaloTTDescrManager::calo_descriptors_begin () const
{
    return m_calo_descr_vec.begin();
}

CaloTTDescrManager::calo_descr_const_iterator
CaloTTDescrManager::calo_descriptors_end	() const
{
    return m_calo_descr_vec.end();
}

CaloTTDescrManager::calo_descr_size
CaloTTDescrManager::calo_descriptors_size () const
{
    return m_calo_descr_vec.size();
}

const CaloLVL1_ID*CaloTTDescrManager::get_lvl1_id() const
{
    return (m_calo_lvl1_id);
}

    

void 				
CaloTTDescrManager::initialize()
{
  // initialization actually done in CaloTTMgrDetDescrCnv
  m_is_initialized = true;
}

bool CaloTTDescrManager::is_initialized()
{
  return m_is_initialized;
}


void
CaloTTDescrManager::add        	(CaloTTDescrRegion* region)
{
    m_calo_region_vec.push_back(region);
    //    m_calo_region_map[region->identify()] = region;    
    m_calo_region_map.insert(std::pair<Identifier, CaloTTDescrRegion*>((region->identify()),region));
}

void
CaloTTDescrManager::add         (CaloTTDescriptor*	descriptor)
{
    m_calo_descr_vec.push_back(descriptor);
}

void			
CaloTTDescrManager::set_helper	(const CaloLVL1_ID* id_helper)
{
    m_calo_lvl1_id = id_helper;
}


void
CaloTTDescrManager::print	() const
{
  std::cout << "CaloTTDescrManager       : " << std::endl;
  std::cout << "   Number of Calo TT Regions : " << m_calo_region_vec.size() << std::endl;
  std::cout << "   Number of Calo descriptors   : " << m_calo_descr_vec.size() << std::endl;

  calo_region_const_iterator	first = calo_region_begin();
  calo_region_const_iterator	last  = calo_region_end();
      
  for (; first != last; ++first) {
    (*first)->print();
  }

}

void CaloTTDescrManager::clear()
{
//  Remove regions and descriptors
    calo_region_const_iterator first = m_calo_region_vec.begin();
    calo_region_const_iterator last  = m_calo_region_vec.end();
    for (; first != last; ++first) delete (*first);
    m_calo_region_vec.clear();

    calo_descr_const_iterator first2 = m_calo_descr_vec.begin();
    calo_descr_const_iterator last2  = m_calo_descr_vec.end();
    for (; first2 != last2; ++first2) delete (*first2);
    m_calo_descr_vec.clear();

    m_is_initialized = false;
}



