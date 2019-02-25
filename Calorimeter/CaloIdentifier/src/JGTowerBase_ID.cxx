/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/JGTowerBase_ID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include "CxxUtils/StrFormat.h"
#include "GaudiKernel/MsgStream.h"
#include <algorithm>
#include <string>
#include <set>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include "boost/foreach.hpp"

using CxxUtils::strformat;


JGTowerBase_ID::JGTowerBase_ID(void) : 
  m_jgtower_region_index(0) 
  , m_CALO_INDEX(999) 
  , m_DETZSIDE_INDEX(999)
  , m_SAMPLING_INDEX(999) 
  , m_REGION_INDEX(999) 
  , m_ETA_INDEX(999)
  , m_PHI_INDEX(999)
  , m_dict(0)
  , m_tower_hash_max(0) 
  , m_calo_region_hash_max(0) 

{

}

JGTowerBase_ID:: ~JGTowerBase_ID(){}



IdContext	
JGTowerBase_ID::region_context 		(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_REGION_INDEX));
}

IdContext	
JGTowerBase_ID::tower_context 		(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_PHI_INDEX));
}


int  JGTowerBase_ID::get_id  (const IdentifierHash& hash_id, Identifier& id, const IdContext* context) const
{
  MsgStream log(m_msgSvc, "JGTowerBase_ID" );
  std::stringstream strm;
  std::string strg;
  int result = 1;
  id.clear();

  size_t begin = (context) ? context->begin_index(): 0;
  // cannot get hash if end is 0:
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) { 
    if (m_REGION_INDEX == end) {
      if (hash_id < (unsigned int)(m_region_vec.end() - m_region_vec.begin())) 
	{
	  id = m_region_vec[hash_id];
	  result = 0;
	} 
      else 
	{
	  strm << hash_id;
	  strg = " hash_id out of range "+strm.str();
	  if(m_msgSvc)
	    {
	      log << MSG::ERROR << strg << endmsg;
	    }
	  else
	    {
	      std::cout << strg << std::endl;
	    }
	}
    }
    else if (m_PHI_INDEX == end) {
      if (hash_id < (unsigned int)(m_tower_vec.end() - m_tower_vec.begin())) {
	id = m_tower_vec[hash_id];
	result = 0;
      } 
      else 
	{
	  strm << hash_id;
	  strg = " hash_id out of range "+strm.str();
	  if(m_msgSvc)
	    {
	      log << MSG::ERROR << strg << endmsg;
	    }
	  else
	    {
	      std::cout << strg << std::endl;
	    }
	}
    }
  }
  return(result);
}

int  JGTowerBase_ID::get_hash  (const Identifier& id, IdentifierHash& hash_id, const IdContext* context) const
{
  hash_id = 0;
  int result = 1;

  size_t begin = (context) ? context->begin_index(): 0;
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) {
    if (m_REGION_INDEX  == end) {
      Identifier redId = region_id (id);
      std::vector<Identifier>::const_iterator it = std::lower_bound(m_region_vec.begin(),m_region_vec.end(),redId);
      if ( it != m_region_vec.end() ){
	hash_id = it - m_region_vec.begin();
	result = 0;
      }
    }
    else if (m_PHI_INDEX  == end) {

      //Identifier redId = tower_id (id);

      std::vector<Identifier>::const_iterator it = std::lower_bound(m_tower_vec.begin(),m_tower_vec.end(),id);
      if ( it != m_tower_vec.end() ){
	hash_id = it - m_tower_vec.begin();
	result = 0;
      }
    }

    else {
      std::string errorMessage =
        "Error in JGTowerBase_ID::get_hash, invalid context ";
      throw CaloID_Exception(errorMessage , 10);
    }
  }

  return (result);
}


IdentifierHash JGTowerBase_ID::calo_region_hash(const Identifier regId) const
{
  IdentifierHash regHash;
  IdContext regionContext = region_context();
  int sc = get_hash(regId, regHash, &regionContext);
  if (sc!=0) return NOT_VALID;
  return regHash;
}


int  JGTowerBase_ID::initialize_base_from_dictionary (const IdDictMgr& dict_mgr, const std::string& group_name, const std::string& t_pre)
{
  MsgStream log(m_msgSvc, "JGTowerBase_ID" );
  std::string strg = "initialize_from_dictionary";
  if(m_msgSvc) {
    log << MSG::INFO << strg << endmsg;
  }
  else {
    std::cout << strg << std::endl;
  }

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    if(m_msgSvc)log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if(m_msgSvc)log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  std::stringstream strm;
  std::stringstream strm1;
  std::stringstream strm2;
  std::string strg1;
  std::string strg2;
  
  // init base object
  if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

  // Register version of the Calorimeter dictionary
  if (register_dict_tag(dict_mgr, "Calorimeter")) return(1);

  m_dict = dict_mgr.find_dictionary ("Calorimeter"); 
  if(!m_dict) 
    {
      strg= " initialize_from_dict - cannot access Calorimeter dictionary ";
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else 
	{
	  std::cout << strg << std::endl;
	}
      return(1);
    }
  
  // Initialize the field indices
  if(initLevelsFromDict(t_pre)) return (1);

  // Find values for the calo and  JGTOWER (neg) fields
  int caloValue   = -1;
  if (m_dict->get_label_value("subdet", "Calorimeter", caloValue)) 
    {
      strm << m_dict->m_name;
      strg= "Could not get value for label 'Calorimeter' of field 'subdet' in dictionary "+strm.str(); 
      if(m_msgSvc)
	{
	  log << MSG::ERROR << strg << endmsg;
	}
      else
	{
	  std::cout << strg << std::endl;
	}
      return (1);
    }

  int jgtowerCaloValue   = -1;
  // negative half
  //    if (m_dict->get_label_value("DetZside", "negative_jgtower_side", jgtowerCaloValue)) 
  // positive half  FLG 12 Jul 07: negative side -> problem for test beam
  if (m_dict->get_label_value("DetZside", group_name, jgtowerCaloValue)) 
    {
      strm << m_dict->m_name;
      //	strg = " Could not get value for label 'negative_jgtower_side' of field 'DetZside in dictionary"+strm.str();
      strg = " Could not get value for label "+group_name+" of field 'DetZside in dictionary"+strm.str();
      if(m_msgSvc)
	{
	  log << MSG::ERROR << strg << endmsg;
	}
      else
	{
	  std::cout << strg << std::endl;
	}
      return (1);
    }

  // Set up id for region and range prefix
  // NOTE: negative value is good enough to get multirange since the
  // regions are symmetric in +/-eta
  // FLG Jul 07: EXCEPT FOR CTB !!!!!!!!!!

  ExpandedIdentifier reg_id;
  reg_id.add(caloValue);
  reg_id.add(jgtowerCaloValue); 
  Range prefix;
  m_full_reg_range = m_dict->build_multirange(reg_id, prefix, t_pre+"region");
  m_full_tower_range = m_dict->build_multirange(reg_id, prefix, t_pre+"phi");
  
  // Setup the hash tables
  if(init_hashes()) return (1);

  // initialize dictionary regions
  if (fill_vec_of_dict_regions ("Reg_"+t_pre+"ower")) return 1;

  // Setup hash tables for finding neighbors
  if(init_neighbors()) return (1);     

  strm1 << (std::string)m_full_tower_range; 
  strg  = " JGTowerBase_ID::initialize_from_dict : ";
  strg1 = " tower range -> "+strm1.str();
  if(m_msgSvc)
    {
      log << MSG::DEBUG << strg << endmsg;
      log << MSG::DEBUG << strg1 << endmsg;
      log << MSG::DEBUG << strg2 << endmsg;
    }
  else
    {
      std::cout << strg << std::endl;
      std::cout << strg1 << std::endl;
      std::cout << strg2 << std::endl;
    }

  //std::cout << " JGTowerBase_ID::initialize_from_dict : " 
  //        << std::endl;
  //std::cout << " tower range -> "  << (std::string)m_full_tower_range
  //        << std::endl;
  //std::cout << " layer range -> "  << (std::string)m_full_layer_range
  //        << std::endl;


  // Setup for hash calculation

  // Regions have uniform eta/phi granularity 
  // The lookup table only needs to contain the
  // hash offset for each region, the first eta index
  // and the number of phi cells.

  // The implementation requires:

  //   1) a lookup table for each region containing hash offset,
  //      etamin and nphi
  //   2) a decoder to access the "index" corresponding to the
  //      pnz/samp/reg fields. These fields use 6 bits, so the
  //      vector has a length of 64 for 16 regions.


  // Create decoder for fields pnz to region
  IdDictFieldImplementation::size_type bits = 
    m_jgtower_impl.bits() +
    m_sampling_impl.bits() +
    m_region_impl.bits();
  IdDictFieldImplementation::size_type bits_offset = m_jgtower_impl.bits_offset();
  m_pnz_reg_impl.set_bits(bits, bits_offset);
  int size = (1 << bits);

  //      std::cout << "pnz_reg        "  
  //  	    << m_pnz_reg_impl.show_to_string() << std::endl; 
  //      std::cout << "size " << size << std::endl;
    

  //    std::cout << "pnz_reg "  << m_pnz_reg_impl.decode_index()  << " " 
  //      << (std::string)m_pnz_reg_impl.ored_field()  << " " 
  //      << std::hex << m_pnz_reg_impl.mask() << " " 
  //      << m_pnz_reg_impl.zeroing_mask() << " " 
  //      << std::dec << m_pnz_reg_impl.shift()
  //      << " " << m_pnz_reg_impl.bits() << " " <<m_pnz_reg_impl.bits_offset()
  //      << std::endl;


  // Set up vector as lookup table for hash calculation. 
  m_hash_calcs.resize(size);

  for (unsigned int i = 0; i < m_calo_region_hash_max; ++i) {

    Identifier regId = region_id(i) ;

    HashCalc hc;
      
    int etamin = eta_min(regId);
    Identifier min = tower_id ( regId, etamin, 0);
    IdentifierHash min_hash = tower_hash_binary_search(min);
    hc.m_hash   = min_hash;
    hc.m_etamin = etamin;
    hc.m_nphi   = phi_max(min)+1 ;
    m_hash_calcs[m_pnz_reg_impl.unpack(min)] = hc;

    if (m_pnz_reg_impl.unpack(min) >= size) 
      {
	strm << size;
	strm1 << show_to_string(min);
	strm2 << m_pnz_reg_impl.unpack(min);
	strg = "Min > "+strm.str();
	strg1= " "+strm1.str();
	strg2= " "+strm2.str();
	if(m_msgSvc)
	  {
	    log << MSG::DEBUG << strg << endmsg;
	    log << MSG::DEBUG << strg1 << endmsg;
	    log << MSG::DEBUG << strg2 << endmsg;
	  }
	else
	  {
	    std::cout << strg << std::endl;
	    std::cout << strg1 << std::endl;
	    std::cout << strg2 << std::endl;
	  }	  
	//std::cout << "min > " << size << " " 
	//    << i << " "
	//	    << show_to_string(min) << " " 
	//	    << m_pnz_reg_impl.unpack(min) << " " 
	//	    << std::endl;
      }
  }

  // Check hash calculation
  for (unsigned int i = 0; i < m_tower_hash_max; ++i) {
    Identifier id = tower_id(i);
    if (tower_hash(id) != i) 
      {
	strm << show_to_string(id);
	strm1 << tower_hash(id);
	strm2 << i;
	strg = " *****  Error tower ranges, id, hash, i = "+strm.str();
	strg1= " , "+strm1.str();
	strg2= " , "+strm2.str();
	if(m_msgSvc)
	  {
	    log << MSG::ERROR << strg << endmsg;
	    log << MSG::ERROR << strg1 << endmsg;
	    log << MSG::ERROR << strg2 << endmsg;
	  }
	else
	  {
	    std::cout << strg << std::endl;
	    std::cout << strg1 << std::endl;
	    std::cout << strg2 << std::endl;
	  }
	    
	//std::cout << "tower ranges, id, hash, i = " 
	//	      << show_to_string(id) << ", " 
	//	      << tower_hash(id) << ", " 
	//	      << i
	//	      << std::endl;
      }
  }
    
  return 0;

}




int JGTowerBase_ID::eta_min(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext region_cntxt = region_context();
  if(!get_expanded_id(regId, expId, &region_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_tower_range.size(); ++i) {
      const Range& range = m_full_tower_range[i];
      if (range.match(expId)) {
	const Range::field& eta_field = range[m_ETA_INDEX];
	if (eta_field.has_minimum()) {
	  int etamin = eta_field.get_minimum();
	  if (-999 == result) {
	    result = etamin;
	  }
	  else {
	    if (etamin < result) result = etamin;
	  }
	}
      }
    }
    return (result);
  }
  return (-999);
}

int JGTowerBase_ID::eta_max(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext region_cntxt = region_context();
  if(!get_expanded_id(regId, expId, &region_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_tower_range.size(); ++i) {
      const Range& range = m_full_tower_range[i];
      if (range.match(expId)) {
	const Range::field& eta_field = range[m_ETA_INDEX];
	if (eta_field.has_maximum()) {
	  int etamax = eta_field.get_maximum();
	  if (result < etamax) result = etamax;
	}
      }
    }
    return (result);
  }
  return (-999);  // default
}

int JGTowerBase_ID::phi_max(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext region_cntxt = region_context();
  if(!get_expanded_id(regId, expId, &region_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_tower_range.size(); ++i) {
      const Range& range = m_full_tower_range[i];
      if (range.match(expId)) {
	const Range::field& phi_field = range[m_PHI_INDEX];
	if (phi_field.has_maximum()) {
	  int phimax = phi_field.get_maximum();
	  if (result < phimax) result = phimax;
	}
      }
    }
    return (result);
  }
  return (-999);  // default
}

float JGTowerBase_ID::etaGranularity(const Identifier regId) const
{
  IdentifierHash regHash = calo_region_hash(regId);
  if (regHash >= m_vecOfDictRegions.size()) return NOT_VALID;
  return m_vecOfDictRegions[regHash]->m_deta;
}

float JGTowerBase_ID::phiGranularity(const Identifier regId) const
{
  IdentifierHash regHash = calo_region_hash(regId);
  if (regHash >= m_vecOfDictRegions.size()) return NOT_VALID;
  return m_vecOfDictRegions[regHash]->m_dphi;
}

float JGTowerBase_ID::eta0(const Identifier regId) const
{
  IdentifierHash regHash = calo_region_hash(regId);
    if (regHash >= m_vecOfDictRegions.size()) return NOT_VALID;
  return m_vecOfDictRegions[regHash]->m_eta0;
}

float JGTowerBase_ID::phi0(const Identifier regId) const
{
  IdentifierHash regHash = calo_region_hash(regId);
  if (regHash >= m_vecOfDictRegions.size()) return NOT_VALID;
  return m_vecOfDictRegions[regHash]->m_phi0;
}

int             
JGTowerBase_ID::get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const
{
  unsigned short index = id;
  if (index < m_prev_phi_vec.size()) {
    if (m_prev_phi_vec[index] == NOT_VALID_HASH) return (1);
    prev =  m_prev_phi_vec[index];
    return (0);
  }
  return (1);
}

int             
JGTowerBase_ID::get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const
{
  unsigned short index = id;
  if (index < m_next_phi_vec.size()) {
    if (m_next_phi_vec[index] == NOT_VALID_HASH) return (1);
    next =  m_next_phi_vec[index];
    return (0);
  }
  return (1);
}

int             
JGTowerBase_ID::get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const
{
  unsigned short index = id;
  if (index < m_prev_eta_vec.size()) {
    if (m_prev_eta_vec[index] == NOT_VALID_HASH) return (1);
    prev =  m_prev_eta_vec[index];
    return (0);
  }
  return (1);
}

int
JGTowerBase_ID::get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const
{
  unsigned short index = id;
  if (index < m_next_eta_vec.size()) {
    if (m_next_eta_vec[index] == NOT_VALID_HASH) return (1);
    next =  m_next_eta_vec[index];
    return (0);
  }
  return (1);
}


int  JGTowerBase_ID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
  // We assume that the context is >= region
  exp_id.clear();
  exp_id << calo_field_value()
	 << pos_neg(id)
	 << sampling(id)
	 << region(id);
  if(context && context->end_index() >= m_ETA_INDEX) {
    exp_id << eta(id);
    if(context->end_index() >= m_PHI_INDEX) {
      exp_id << phi(id);
    }
  }
  return (0);
}
void JGTowerBase_ID::tower_id_checks   ( int pos_neg, int sampling, int region,
					 int eta,       int phi ) const
{  
  // Fill expanded id
  ExpandedIdentifier id(calo_exp());
  id << pos_neg << sampling <<
    region << eta << phi;

  if (!m_full_tower_range.match(id)) { 
    std::string errorMessage = "JGTowerBase_ID::tower_id() result is not OK: ID, range = "
      + std::string(id) + " , " + (std::string)m_full_tower_range;
    throw CaloID_Exception(errorMessage , 2);
  }
}

void JGTowerBase_ID::tower_id_checks   ( const Identifier regionId,
					 int eta,  int phi ) const
{
  // Fill expanded id
  ExpandedIdentifier id; 

  IdContext context = region_context();
  if (get_expanded_id(regionId, id, &context)) {
    std::string errorMessage = "JGTowerBase_ID::tower_id(regionId) result is not OK: ID= "
      + show_to_string(regionId) ;
    throw CaloID_Exception(errorMessage , 2);
  }

  id << eta << phi;

  if (!m_full_tower_range.match(id)) {
    std::string errorMessage = "JGTowerBase_ID::tower_id(regionId,field values) result is not OK: ID, range = "
      + std::string(id) + " , " + (std::string)m_full_tower_range;
    throw CaloID_Exception(errorMessage , 2);
  }
}

void JGTowerBase_ID::region_id_checks (int pos_neg, int sampling, int region)const 
{
  // Fill expanded id
  ExpandedIdentifier id(calo_exp());
  id << pos_neg << sampling << region ;

  if (!m_full_reg_range.match(id)) { 
    std::string errorMessage = "JGTowerBase_ID::region_id() result is not OK: ID, range = "
      + std::string(id) + " , " + (std::string)m_full_reg_range;
    throw CaloID_Exception(errorMessage , 2);
  }
}

int   JGTowerBase_ID::initLevelsFromDict(const std::string& t_pre) 
{
  MsgStream log(m_msgSvc, "JGTowerBase_ID" );
  std::stringstream strm;
  std::stringstream strm1;
  std::stringstream strm2;
  std::stringstream strm3;
  std::stringstream strm4;
  std::stringstream strm5;
  std::stringstream strm6;
  std::stringstream strm7;
  std::string strg;
  std::string strg1;
  std::string strg2;
  std::string strg3;
  std::string strg4;
  std::string strg5;
  std::string strg6;
  std::string strg7;
  if(!m_dict) 
    {
      strg= "initLevelsFromDict - dictionary NOT initialized ";
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else 
	{
	  std::cout << strg << std::endl;
	}
      return (1);
    }

  // Find out which identifier field corresponds to each level.

  m_CALO_INDEX       = 999 ;
  m_DETZSIDE_INDEX   = 999 ;
  m_SAMPLING_INDEX   = 999 ;
  m_REGION_INDEX     = 999 ;
  m_ETA_INDEX        = 999 ;
  m_PHI_INDEX        = 999 ;

  // Save index to a JGTOWER region for unpacking - search with region name
  IdDictRegion* reg = m_dict->find_region(t_pre+"ower_0");
  if (reg) 
    {
      m_jgtower_region_index = reg->m_index;
    }
  else 
    {
      strg = "initLevelsFromDict - unable to find jgtower region ";
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else 
	{
	  std::cout << strg << std::endl;
	}      
      return (1);
    }
  
  // Fing a JGTOWER region
  IdDictField* field = m_dict->find_field("subdet") ;
  if (field) {
    m_CALO_INDEX = field->m_index ;
  }
  else 
    {
      strg=  "initLevelsFromDict - unable to find 'subdet' field ";
      if(m_msgSvc) 
	{
	  log << MSG::ERROR << strg << endmsg;
	}
      else 
	{
	  std::cout << strg << std::endl;
	}	
      return (1);
    }
  
  field = m_dict->find_field("DetZside") ;
  if (field) {
    m_DETZSIDE_INDEX = field->m_index ;
  }
  else 
    {
      strg= "initLevelsFromDict - unable to find 'DetZside' field ";
      if(m_msgSvc) 
	{
	  log << MSG::ERROR << strg << endmsg;
	}
      else 
	{
	  std::cout << strg << std::endl;
	}	
      return (1);
    }

  field = m_dict->find_field(t_pre+"sampling") ;
  if (field) {
    m_SAMPLING_INDEX = field->m_index ;
  }
  else 
    {
      
      strg="initLevelsFromDict - unable to find '"+t_pre+"sampling' field ";
      if(m_msgSvc) 
	{
	  log << MSG::ERROR << strg << endmsg;
	}
      else 
	{
	  std::cout << strg << std::endl;
	}      
      return (1);
    }

  field = m_dict->find_field(t_pre+"region") ;
  if (field) {
    m_REGION_INDEX = field->m_index ;
  }
  else 
    {
      
      strg="initLevelsFromDict - unable to find 'region' field ";
      if(m_msgSvc) 
	{
	  log << MSG::ERROR << strg << endmsg;
	}
      else 
	{
	  std::cout << strg << std::endl;
	}      
      return (1);
    }

  /* std::cout << "m_region= " << m_REGION_INDEX << std::endl; */

  field = m_dict->find_field(t_pre+"eta") ;
  if (field) {
    m_ETA_INDEX = field->m_index ;
  }
  else 
    {
      strg= "initLevelsFromDict - unable to find 'eta' field ";
      if(m_msgSvc) 
	{
	  log << MSG::ERROR << strg << endmsg;
	}
      else 
	{
	  std::cout << strg << std::endl;
	}		
      return (1);
    }
  
  field = m_dict->find_field(t_pre+"phi") ;
  if (field) {
    m_PHI_INDEX = field->m_index ;
  }
  else 
    {
      
      strg= "initLevelsFromDict - unable to find 'phi' field ";
      if(m_msgSvc) 
	{
	  log << MSG::ERROR << strg << endmsg;
	}
      else 
	{
	  std::cout << strg << std::endl;
	}          
      return (1);
    }
  // Set the field implementations

  const IdDictRegion& region = *m_dict->m_regions[m_jgtower_region_index];

  m_calo_impl     = region.m_implementation[m_CALO_INDEX]; 
  m_jgtower_impl     = region.m_implementation[m_DETZSIDE_INDEX];
  m_sampling_impl = region.m_implementation[m_SAMPLING_INDEX]; 
  m_region_impl   = region.m_implementation[m_REGION_INDEX]; 
  m_eta_impl      = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl      = region.m_implementation[m_PHI_INDEX]; 

  strm1 << m_calo_impl.show_to_string();
  strm2 << m_jgtower_impl.show_to_string();
  strm3 << m_sampling_impl.show_to_string();
  strm4 << m_region_impl.show_to_string();
  strm5 << m_eta_impl.show_to_string();
  strm6 << m_phi_impl.show_to_string();
  strg = "decode index and bit fields for each level: ";
  strg1= "calo  "+strm1.str();
  strg2= "detzside  "+strm2.str();
  strg3= "sampling  "+strm3.str();
  strg4= "reg  "+strm4.str();
  strg5= "eta  "+strm5.str();
  strg6= "phi  "+strm6.str();
  if(m_msgSvc) 
    {
      log << MSG::DEBUG << strg << endmsg;
      log << MSG::DEBUG << strg1 << endmsg;
      log << MSG::DEBUG << strg2 << endmsg;
      log << MSG::DEBUG << strg3 << endmsg;
      log << MSG::DEBUG << strg4 << endmsg;
      log << MSG::DEBUG << strg5 << endmsg;
      log << MSG::DEBUG << strg6 << endmsg;
    }
  else 
    {
      std::cout << strg << std::endl;
      std::cout << strg1 << std::endl;
      std::cout << strg2 << std::endl;
      std::cout << strg3 << std::endl;
      std::cout << strg4 << std::endl;
      std::cout << strg5 << std::endl;
      std::cout << strg6 << std::endl;
    }
  
  return(0) ;
}


int   JGTowerBase_ID::init_hashes(void) 
{
  MsgStream log(m_msgSvc, "JGTowerBase_ID" );
  std::stringstream strm;
  std::stringstream strm1;
  std::stringstream strm2;
  std::string strg;
  std::string strg1;
  std::string strg2;
  // tower hash
  m_tower_hash_max = m_full_tower_range.cardinality();
  m_tower_vec.resize(m_tower_hash_max);
  unsigned int nids = 0;
  std::set<Identifier> ids;
  for (unsigned int i = 0; i < m_full_tower_range.size(); ++i) {
    const Range& range = m_full_tower_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      Identifier tow_id = tower_id   ( exp_id[m_DETZSIDE_INDEX],
				       exp_id[m_SAMPLING_INDEX], 
				       exp_id[m_REGION_INDEX] ,
				       exp_id[m_ETA_INDEX] ,
				       exp_id[m_PHI_INDEX] ) ;
      if(!(ids.insert(tow_id)).second)
	{
	  if(m_msgSvc)
	    {
	      log << MSG::ERROR << " init_hashes "
		  << " duplicated id for J/GTower id. nids= " << nids
		  << " compact Id  " << endmsg; 
	    }
	  else
	    {
	      std::cout << " JGTowerBase_ID::init_hashes "
	  		<< " Error: duplicated id for J/GTower id. nids= " << nids
	  		<< " compact Id " ;
	      (*first).show();
	      std::cout << " " << show_to_string(tow_id) << std::endl; 
	    }
	}
      nids++;
    }
  }
  if(ids.size() != m_tower_hash_max) 
    {
      if( m_msgSvc)
	{
	  log << MSG::ERROR << " init_hashes "
	      << " set size NOT EQUAL to hash max. size " << ids.size()
	      << " hash max " << m_tower_hash_max
	      << endmsg;
	}
      else
	{
	  std::cout << " JGTowerBase_ID::init_hashes "
		    << " Error: set size NOT EQUAL to hash max. size " << ids.size()
		    << " hash max " << m_tower_hash_max
		    << std::endl;
	}
      return (1);
    }

  nids=0;
  std::set<Identifier>::const_iterator first = ids.begin();
  std::set<Identifier>::const_iterator last  = ids.end();
  for (;first != last && nids < m_tower_vec.size(); ++first) {
    m_tower_vec[nids] = (*first) ;
    nids++;
  }
  // region hash
  m_calo_region_hash_max = m_full_reg_range.cardinality();
  m_region_vec.resize(m_calo_region_hash_max);
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_reg_range.size(); ++i) {
    const Range& range = m_full_reg_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      Identifier reg_id = region_id   ( exp_id[m_DETZSIDE_INDEX],
					exp_id[m_SAMPLING_INDEX], 
					exp_id[m_REGION_INDEX] );
      if(!(ids.insert(reg_id)).second)
	{
	  if(m_msgSvc)
	    {
	      log << MSG::ERROR << " JGTowerBase_ID::init_hashes "
		  << "  duplicated id for region id. nids= " << nids
		  << " compact Id  " << endmsg;
	    }
	  else
	    {
	      std::cout << " JGTowerBase_ID::init_hashes "
			<< " Error: duplicated id for region id. nids= " << nids
			<< " compact Id  " ;
	      (*first).show();
	      std::cout << " " << show_to_string(reg_id) << std::endl;
	      std::cout << std::endl;
	    }
	}
      nids++;
    }
  }
  if(ids.size() != m_calo_region_hash_max) 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << " JGTowerBase_ID::init_hashes "
	      << "  set size NOT EQUAL to region hash max. size " << ids.size()
	      << " region hash max " << m_calo_region_hash_max
	      << endmsg;
	}
      else
	{
	  std::cout << " JGTowerBase_ID::init_hashes "
		    << " Error: set size NOT EQUAL to region hash max. size " << ids.size()
		    << " region hash max " << m_calo_region_hash_max
		    << std::endl;
	}
      return (1);
    }
  nids=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nids < m_region_vec.size(); ++first) {
    m_region_vec[nids] = (*first) ;
    nids++;
  }

  return (0);                   
}



int   JGTowerBase_ID::init_neighbors(void) 
{
  MsgStream log(m_msgSvc, "JGTowerBase_ID" );
  //  std::cout << " JGTowerBase_ID::init_neighbors " << std::endl;
  //  std::cout << " m_tower_hash_max, NOT_VALID_HASH = " << m_tower_hash_max << " " << NOT_VALID_HASH << std::endl;

  m_prev_phi_vec.resize(m_tower_hash_max, NOT_VALID_HASH);
  m_next_phi_vec.resize(m_tower_hash_max, NOT_VALID_HASH);
  m_prev_eta_vec.resize(m_tower_hash_max, NOT_VALID_HASH);
  m_next_eta_vec.resize(m_tower_hash_max, NOT_VALID_HASH);
  for (unsigned int i = 0; i < m_full_tower_range.size(); ++i) {
    const Range& range = m_full_tower_range[i];
    const Range::field& eta_field = range[m_ETA_INDEX];
    const Range::field& phi_field = range[m_PHI_INDEX];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      ExpandedIdentifier::element_type previous_eta;
      ExpandedIdentifier::element_type next_eta;
      ExpandedIdentifier::element_type previous_phi;
      ExpandedIdentifier::element_type next_phi;
      bool peta = eta_field.get_previous(exp_id[m_ETA_INDEX], previous_eta);
      bool neta = eta_field.get_next    (exp_id[m_ETA_INDEX], next_eta);
      bool pphi = phi_field.get_previous(exp_id[m_PHI_INDEX], previous_phi);
      bool nphi = phi_field.get_next    (exp_id[m_PHI_INDEX], next_phi);
      
      IdContext tcontext = tower_context();
      
      // Get and save region id to speed things up
      Identifier reg_id = region_id   ( exp_id[m_DETZSIDE_INDEX],
					exp_id[m_SAMPLING_INDEX], 
					exp_id[m_REGION_INDEX] );
      
      // First get primary hash id
      IdentifierHash hash_id;
      Identifier id = tower_id (reg_id, 
				exp_id[m_ETA_INDEX],
				exp_id[m_PHI_INDEX]);
      if (get_hash(id, hash_id,&tcontext)) 
	{
	  if( m_msgSvc )
	    {
	      log << MSG::ERROR << " init_neighbors - unable to get hash, compact = " << endmsg;
	    }
	  else
	    {
	      std::cout << " JGTowerBase_ID::init_neighbors - unable to get hash, compact = ";
	      exp_id.show();
	      std::cout << std::endl;
	    }
	  return (1);
	}
      
      // index for the subsequent arrays
      unsigned short index = hash_id;
      assert (hash_id < m_prev_phi_vec.size());
      assert (hash_id < m_next_phi_vec.size());
      assert (hash_id < m_prev_eta_vec.size());
      assert (hash_id < m_next_eta_vec.size());

      if (pphi) {
	// Get previous phi hash id
	id = tower_id (reg_id, 
		       exp_id[m_ETA_INDEX],
		       previous_phi);
	// forward to compact -> hash
	if (get_hash(id, hash_id,&tcontext)) 
	  {
	    if( m_msgSvc )
	      {
		log << MSG::ERROR << " init_neighbors - unable to get previous phi hash, exp/compact " << endmsg;
	      }
	    else
	      {
		std::cout << " JGTowerBase_ID::init_neighbors - unable to get previous phi hash, exp/compact ";
		exp_id.show();
		std::cout << " " 
			  << std::endl;
	      }
	    return (1);
	  }
	m_prev_phi_vec[index] = hash_id;
      }

      if (nphi) {
	// Get next phi hash id
	id = tower_id (reg_id, 
		       exp_id[m_ETA_INDEX],
		       next_phi);
	// forward to compact -> hash
	if (get_hash(id, hash_id,&tcontext)) 
	  {
	    if(m_msgSvc)
	      {
		log << MSG::ERROR << " init_neighbors - unable to get next phi hash, exp/compact "<<endmsg;
	      }
	    else
	      {
		std::cout << " JGTowerBase_ID::init_neighbors - unable to get next phi hash, exp/compact ";
		exp_id.show();
		std::cout << " " 
			  << std::endl;
	      }
	    return (1);
	  }
	m_next_phi_vec[index] = hash_id;
      }
      if (peta) {
	// Get previous eta hash id
	id = tower_id (reg_id, 
		       previous_eta,
		       exp_id[m_PHI_INDEX]);
	// forward to compact -> hash
	if (get_hash(id, hash_id,&tcontext)) 
	  {
	    if( m_msgSvc )
	      {
		log << MSG::ERROR << " init_neighbors - unable to get previous eta hash, exp/compact "<< endmsg;
	      }
	    else
	      {
		std::cout << " JGTowerBase_ID::init_neighbors - unable to get previous eta hash, exp/compact ";
		exp_id.show();
		std::cout << " " 
			  << std::endl;
	      }
	    return (1);
	  }
	m_prev_eta_vec[index] = hash_id;
      }
      
      if (neta) {
	// Get next eta hash id
	id = tower_id (reg_id, 
		       next_eta,
		       exp_id[m_PHI_INDEX]);
	// forward to compact -> hash
	if (get_hash(id, hash_id,&tcontext)) 
	  {
	    if( m_msgSvc )
	      {
		log << MSG::ERROR << " init_neighbors - unable to get next eta hash, exp/compact ";
	      }
	    else
	      {
		std::cout << " JGTowerBase_ID::init_neighbors - unable to get next eta hash, exp/compact ";
		exp_id.show();
		std::cout << " " 
			  << std::endl;
	      }
	    return (1);
	  }
	m_next_eta_vec[index] = hash_id;
      } // end neta cond
    } // end loop on identifiers
  } // end loop on ranges
  return (0);
}

int
JGTowerBase_ID::fill_vec_of_dict_regions (const std::string& group_name /*= ""*/)
{
  m_vecOfDictRegions.clear();
  m_vecOfDictRegions.reserve (m_calo_region_hash_max);
  IdContext region_cntxt = region_context();
  ExpandedIdentifier expRegId;
  for (unsigned int i = 0; i < m_calo_region_hash_max; ++i) {
    Identifier id = region_id(i);
    if(!get_expanded_id(id, expRegId, &region_cntxt)) {
      m_vecOfDictRegions.push_back (m_dict->find_region(expRegId,group_name));
    }
  }
  //assert (m_vecOfDictRegions.size() == regions().hash_max());
  return 0;
}
