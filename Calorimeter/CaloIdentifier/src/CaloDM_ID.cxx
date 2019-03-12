/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/CaloDM_ID.h"
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


using CxxUtils::strformat;


CaloDM_ID::CaloDM_ID(void) : 
    m_calodm_region_index(0) 
  , m_CALO_INDEX(999) 
  , m_DETZSIDE_INDEX(999)
  , m_DMAT_INDEX(999) 
  , m_SAMPLING_INDEX(999) 
  , m_REGION_INDEX(999) 
  , m_ETA_INDEX(999)
  , m_PHI_INDEX(999)
  , m_dict(0)
  , m_lar_zone_hash_max(0) 
  , m_lar_region_hash_max(0)
  , m_tile_zone_hash_max(0) 
  , m_tile_region_hash_max(0) 
 
{

}

CaloDM_ID:: ~CaloDM_ID(){}


int CaloDM_ID::eta_min(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext cntxt = region_context();
  if(!get_expanded_id(id, expId, &cntxt)) {
    int result = -999;
    const MultiRange * zoneRange = 0 ;
    if ( is_lar(id) ) {
      zoneRange = &(m_full_lar_zone_range) ;
    } else {
      assert ( is_tile(id) );
      zoneRange = &(m_full_tile_zone_range) ;
    }
    for (unsigned int i = 0; i < (*zoneRange).size(); ++i) {
      const Range& range = (*zoneRange)[i];
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

int CaloDM_ID::eta_max(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext cntxt = region_context();
  if(!get_expanded_id(id, expId, &cntxt)) {
    int result = -999;
    const MultiRange * zoneRange = 0 ;
    if ( is_lar(id) ) {
      zoneRange = &(m_full_lar_zone_range) ;
    } else {
      assert ( is_tile(id) );
      zoneRange = &(m_full_tile_zone_range) ;
    }
    for (unsigned int i = 0; i < (*zoneRange).size(); ++i) {
      const Range& range = (*zoneRange)[i];
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

int CaloDM_ID::phi_min(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext cntxt = region_context();
  if(!get_expanded_id(id, expId, &cntxt)) {
    int result = -999;
    const MultiRange * zoneRange = 0 ;
    if ( is_lar(id) ) {
      zoneRange = &(m_full_lar_zone_range) ;
    } else {
      assert ( is_tile(id) );
      zoneRange = &(m_full_tile_zone_range) ;
    }
    for (unsigned int i = 0; i < (*zoneRange).size(); ++i) {
      const Range& range = (*zoneRange)[i];
      if (range.match(expId)) {
	const Range::field& phi_field = range[m_PHI_INDEX];
	if (phi_field.has_minimum()) {
	  int phimin = phi_field.get_minimum();
	  if (-999 == result) {
	    result = phimin;
	  }
	  else {
	    if (phimin < result) result = phimin;
	  }
	}
      }
    }
    return (result);
  }
  return (-999);
}

int CaloDM_ID::phi_max(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext cntxt = zone_context();
  if(!get_expanded_id(id, expId, &cntxt)) {
    int result = -999;
    const MultiRange * zoneRange = 0 ;
    if ( is_lar(id) ) {
      zoneRange = &(m_full_lar_zone_range) ;
    } else {
      assert ( is_tile(id) );
      zoneRange = &(m_full_tile_zone_range) ;
    }
    for (unsigned int i = 0; i < (*zoneRange).size(); ++i) {
      const Range& range = (*zoneRange)[i];
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


/*===================================================================*/
int  CaloDM_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/*===================================================================*/
{
  // Msg Service
  MsgStream log(m_msgSvc, "CaloDM_ID" );
  std::string strg = "initialize_from_dictionary";
  if(m_msgSvc) {
    log << MSG::INFO << strg << endmsg;
  }
  else {
    std::cout << strg << std::endl;
  }
  std::stringstream strm;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    if(m_msgSvc)log << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if(m_msgSvc)log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

   // init base object
  if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

  // Register version of the Calorimeter dictionary
  if (register_dict_tag(dict_mgr, "Calorimeter")) return(1);

  m_dict = dict_mgr.find_dictionary ("Calorimeter"); 
  if(!m_dict) {
    if(m_msgSvc)
      {
	log << MSG::ERROR << " initialize_from_dict - cannot access Calorimeter dictionary " << endmsg;
      }
    else
      {
	std::cout << " CaloDM_ID::initialize_from_dict - cannot access Calorimeter dictionary "
		  << std::endl;
      }
    return 1;
  }
  
  // Initialize the field indices
  if(initLevelsFromDict()) return (1);

 
  // Find values for the calo, LArDM (neg) and TileDM (neg) fields
  int caloValue   = -1;
  if (m_dict->get_label_value("subdet", "Calorimeter", caloValue)) 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << "Could not get value for label 'Calorimeter' of field 'subdet' in dictionary " 
	      << m_dict->m_name << endmsg;
	}
      else
	{
	  std::cout << "Could not get value for label 'Calorimeter' of field 'subdet' in dictionary " 
		    << m_dict->m_name
		    << std::endl;
	}
    return (1);
  }

  int lardmCaloValue   = -1;
  // negative half
  if (m_dict->get_label_value("DetZside", "negative_DMLar_side", lardmCaloValue)) 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << "Could not get value for label 'negative_DMLar_side' of field 'DetZside' in dictionary " 
	      << m_dict->m_name << endmsg;
	}
      else
	{
	  std::cout << "Could not get value for label 'negative_DMLar_side' of field 'DetZside' in dictionary " 
		    << m_dict->m_name
		    << std::endl;
	}
      return (1);
    }

  int tiledmCaloValue   = -1;
  // negative half
  if (m_dict->get_label_value("DetZside", "negative_DMTile_side", tiledmCaloValue)) 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << "Could not get value for label 'negative_DMTile_side' of field 'DetZside' in dictionary " 
	      << m_dict->m_name
	      << endmsg;
	}
      else
	{
	  std::cout << "Could not get value for label 'negative_DMTile_side' of field 'DetZside' in dictionary " 
		    << m_dict->m_name
		    << std::endl;
	}
    return (1);
  }


  // Set up id for region and range prefix

  // NOTE: negative value is good enough to get multirange since the
  // regions are symmetric in +/-eta

  // LAr
  ExpandedIdentifier reg_id;
  reg_id.add(caloValue);
  reg_id.add(lardmCaloValue); 
  Range prefix;
  m_full_lar_region_range = m_dict->build_multirange(reg_id, prefix, "DMregion");
  m_full_lar_zone_range = m_dict->build_multirange(reg_id, prefix, "phivalue");
    
  // Tile
  ExpandedIdentifier reg_id2;
  reg_id2.add(caloValue);
  reg_id2.add(tiledmCaloValue); 
  Range prefix2;
  m_full_tile_region_range = m_dict->build_multirange(reg_id2, prefix2, "DMregion");
  m_full_tile_zone_range = m_dict->build_multirange(reg_id2, prefix2, "phivalue");


  if (!m_quiet) {
    if(m_msgSvc)
    {
      log << MSG::DEBUG << " initialize_from_dict : " << endmsg;
      log << MSG::DEBUG << " LAr zone range -> "  << (std::string)m_full_lar_zone_range << endmsg;
      log << MSG::DEBUG << " LAr region range -> "  << (std::string)m_full_lar_region_range << endmsg;
      log << MSG::DEBUG << " Tile zone range -> "  << (std::string)m_full_tile_zone_range << endmsg;
      log << MSG::DEBUG << " Tile region range -> "  << (std::string)m_full_tile_region_range << endmsg;
    }
    else
    {
      std::cout << " CaloDM_ID::initialize_from_dict : " 
		<< std::endl;
      std::cout << " LAr zone range -> "  << (std::string)m_full_lar_zone_range
		<< std::endl;
      std::cout << " LAr region range -> "  << (std::string)m_full_lar_region_range
		<< std::endl;
      std::cout << " Tile zone range -> "  << (std::string)m_full_tile_zone_range
		<< std::endl;
      std::cout << " Tile region range -> "  << (std::string)m_full_tile_region_range
		<< std::endl;
    }
  }
    

  // Setup the hash tables
  if(init_lar_hashes()) return (1);
  if(init_tile_hashes()) return (1);
  
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
    m_calodm_impl.bits() +
    m_dmat_impl.bits() +
    m_sampling_impl.bits() +
    m_region_impl.bits();
  IdDictFieldImplementation::size_type bits_offset = m_calodm_impl.bits_offset();
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
  

  // Set up vector as lookup table for lar hash calculation. 
  m_lar_hash_calcs.resize(size);

  for (unsigned int i = 0; i < m_lar_region_hash_max; ++i) {
    Identifier regId = lar_region_id(i) ;
    HashCalc hc;
    int etamin = eta_min(regId);
    if(etamin < 0) 
      {
	etamin = 0;
	if(m_msgSvc)
	  {
	    log << MSG::WARNING << " seting etamin to 0 because actual value not found for regId " << show_to_string(regId) << endmsg;
	  }
	else
	  {
	    std::cout << "WARNING !!! seting etamin to 0 because actual value not found for regId " << show_to_string(regId) << std::endl;
	  }
      }
    Identifier min = zone_id ( regId, etamin, 0);
    IdentifierHash min_hash = lar_zone_hash_binary_search(min);
    hc.m_hash   = min_hash;
    hc.m_etamin = etamin;
    hc.m_nphi   = phi_max(min)+1 ;
    m_lar_hash_calcs[m_pnz_reg_impl.unpack(min)] = hc;

    if (m_pnz_reg_impl.unpack(min) >= size) 
      {
	if( m_msgSvc)
	  {
	    log << MSG::WARNING << "min > " << size << " " 
		<< i << " "
		<< show_to_string(min) << " " 
		<< m_pnz_reg_impl.unpack(min) << " " 
		<< endmsg;
	  }
	else
	  {
	    std::cout << "min > " << size << " " 
		      << i << " "
		      << show_to_string(min) << " " 
		      << m_pnz_reg_impl.unpack(min) << " " 
		      << std::endl;
	  }
      }
  }

  // Check lar hash calculation
  for (unsigned int i = 0; i < m_lar_zone_hash_max; ++i) {
    Identifier id = lar_zone_id(i);
    if (lar_zone_hash(id) != i) 
      {
	if( m_msgSvc)
	  {
	    log << MSG::ERROR << "lar zone ranges, id, hash, i = " 
		<< show_to_string(id) << ", " 
		<< lar_zone_hash(id) << ", " 
		<< i
		<< endmsg;
	  }
	else
	  {
	    std::cout << " *****  Error ";
	    std::cout << "lar zone ranges, id, hash, i = " 
		      << show_to_string(id) << ", " 
		      << lar_zone_hash(id) << ", " 
		      << i
		      << std::endl;
	  }
      }
  }
  
  // Set up vector as lookup table for tile hash calculation. 
  m_tile_hash_calcs.resize(size);

  for (unsigned int i = 0; i < m_tile_region_hash_max; ++i) {
    Identifier regId = tile_region_id(i) ;
    HashCalc hc;
    int etamin = eta_min(regId);
    if(etamin < 0) 
      {
	etamin = 0;
	if( m_msgSvc)
	  {
	    log << MSG::WARNING << "seting etamin to 0 because actual value not found for regId " << show_to_string(regId) << endmsg;
	  }
	else
	  {
	    std::cout << "WARNING !!! seting etamin to 0 because actual value not found for regId " << show_to_string(regId) << std::endl;
	  }
      }
    Identifier min = zone_id ( regId, etamin, 0);
    IdentifierHash min_hash = tile_zone_hash_binary_search(min);
    hc.m_hash   = min_hash;
    hc.m_etamin = etamin;
    hc.m_nphi   = phi_max(min)+1 ;
    m_tile_hash_calcs[m_pnz_reg_impl.unpack(min)] = hc;

    if (m_pnz_reg_impl.unpack(min) >= size) 
      {
	if( m_msgSvc)
	  {
	    log << MSG::WARNING << "min > " << size << " " 
		<< i << " "
		<< show_to_string(min) << " " 
		<< m_pnz_reg_impl.unpack(min) << " " 
		<< endmsg;
	  }
	else
	  {
	    std::cout << "min > " << size << " " 
		      << i << " "
		      << show_to_string(min) << " " 
		      << m_pnz_reg_impl.unpack(min) << " " 
		      << std::endl;
	  }
      }
  }

  // Check tile hash calculation
  for (unsigned int i = 0; i < m_tile_zone_hash_max; ++i) {
    Identifier id = tile_zone_id(i);
    if (tile_zone_hash(id) != i) 
      {
	if( m_msgSvc)
	  {
	    log << MSG::ERROR << "tile zone ranges, id, hash, i = " 
		<< show_to_string(id) << ", " 
		<< tile_zone_hash(id) << ", " 
		<< i
		<< endmsg;
	  }
	else
	  {
	    std::cout << " *****  Error ";
	    std::cout << "tile zone ranges, id, hash, i = " 
		      << show_to_string(id) << ", " 
		      << tile_zone_hash(id) << ", " 
		      << i
		      << std::endl;
	  }
      }
  }
  return 0;

}

  int  CaloDM_ID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
    // We assume that the context is >= region
    exp_id.clear();
    exp_id << calo_field_value()
	   << pos_neg_z(id)
           << dmat(id)
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

IdContext       
CaloDM_ID::region_context             (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_REGION_INDEX));
}

IdContext       
CaloDM_ID::zone_context              (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_PHI_INDEX));
}


void CaloDM_ID::lar_region_id_checks (int pos_neg_z, int dmat, int sampling, int region)const
{
  // Fill expanded id
  ExpandedIdentifier id(calo_exp());
  id << pos_neg_z << dmat << sampling << region ;
  
  if (!m_full_lar_region_range.match(id)) { 
    std::string errorMessage = "CaloDM_ID::lar_region_id() result is not OK: ID, range = "
      + std::string(id) + " , " + (std::string)m_full_lar_zone_range;
    throw CaloID_Exception(errorMessage , 3);
  }
}

void CaloDM_ID::tile_region_id_checks (int pos_neg_z, int dmat, int sampling, int region)const
{
  // Fill expanded id
  ExpandedIdentifier id(calo_exp());
  id << pos_neg_z << dmat << sampling << region ;
  
  if (!m_full_tile_region_range.match(id)) { 
    std::string errorMessage = "CaloDM_ID::tile_region_id() result is not OK: ID, range = "
      + std::string(id) + " , " + (std::string)m_full_tile_zone_range;
    throw CaloID_Exception(errorMessage , 3);
  }
}



 void CaloDM_ID::lar_zone_id_checks   ( int pos_neg_z, int dmat, int sampling, int region,
					int eta,       int phi ) const
{  
  // Fill expanded id
  ExpandedIdentifier id(calo_exp());
  id << pos_neg_z << dmat << sampling << region << eta << phi;

  if (!m_full_lar_zone_range.match(id)) { 
    std::string errorMessage = "CaloDM_ID::lar_zone_id() result is not OK: ID, range = "
      + std::string(id) + " , " + (std::string)m_full_lar_zone_range;
    throw CaloID_Exception(errorMessage , 4);
  }
}

 void CaloDM_ID::tile_zone_id_checks   ( int pos_neg_z, int dmat, int sampling, int region,
					 int eta,       int phi ) const
{  
  // Fill expanded id
  ExpandedIdentifier id(calo_exp());
  id << pos_neg_z << dmat << sampling << region << eta << phi;

  if (!m_full_tile_zone_range.match(id)) { 
    std::string errorMessage = "CaloDM_ID::tile_zone_id() result is not OK: ID, range = "
      + std::string(id) + " , " + (std::string)m_full_tile_zone_range;
    throw CaloID_Exception(errorMessage , 4);
  }
}


void CaloDM_ID::zone_id_checks   ( const Identifier& regionId,
				   int eta,  int phi ) const
{
  // Fill expanded id
    ExpandedIdentifier id; 

    IdContext context = region_context();
    if (get_expanded_id(regionId, id, &context)) {
      std::string errorMessage = "CaloDM_ID::zone_id(regionId) result is not OK: ID= "
	                       + show_to_string(regionId) ;
      throw CaloID_Exception(errorMessage , 4);
    }

    id << eta << phi;

    if(is_lar(regionId)) {
      if (!m_full_lar_zone_range.match(id)) {
	std::string errorMessage = "CaloDM_ID::zone_id(regionId) result is not OK: ID, range = "
	  + std::string(id) + " , " + (std::string)m_full_lar_zone_range;
	throw CaloID_Exception(errorMessage , 4);
      }
    } else if(is_tile(regionId)) {
      if (!m_full_tile_zone_range.match(id)) {
	std::string errorMessage = "CaloDM_ID::zone_id(regionId) result is not OK: ID, range = "
	  + std::string(id) + " , " + (std::string)m_full_tile_zone_range;
	throw CaloID_Exception(errorMessage , 4);
      }
    }
}


/*=======================================*/
int   CaloDM_ID::initLevelsFromDict(void) 
/*=======================================*/
{
  // Msg Service
  MsgStream log(m_msgSvc, "CaloDM_ID" );
  std::string strg;
  std::stringstream strm;
  if(!m_dict) {
    if( m_msgSvc) 
      {
	log << MSG::ERROR << "initLevelsFromDict - dictionary NOT initialized " 
	    << endmsg;
      }
    else
      {
	std::cout << "CaloDM_ID::initLevelsFromDict - dictionary NOT initialized "
		  << std::endl ;
      }
    return (1);
  }

  // Find out which identifier field corresponds to each level.

  m_CALO_INDEX       = 999 ;
  m_DETZSIDE_INDEX   = 999 ;
  m_DMAT_INDEX       = 999 ;   
  m_SAMPLING_INDEX   = 999 ; 
  m_REGION_INDEX     = 999 ;
  m_ETA_INDEX        = 999 ;
  m_PHI_INDEX        = 999 ;

  // Save index to a LArDM region for unpacking - search with region name
  IdDictRegion* reg = m_dict->find_region("DM_4_1_0_0");
  if (reg) 
    {
      m_calodm_region_index = reg->m_index;
    }
  else 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << "initLevelsFromDict - unable to find lardm region " << endmsg;
	}
      else
	{
	  std::cout << "CaloDM_ID::initLevelsFromDict - unable to find lardm region "  
		    << std::endl;
	}
      return (1);
    }
  
  
  // Fing a CaloDM region
  IdDictField* field = m_dict->find_field("subdet") ;
  if (field) {
    m_CALO_INDEX = field->m_index ;
  }
  else 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'subdet' field " 
	      << endmsg;
	}
      else
	{
	  std::cout <<  "CaloDM_ID::initLevelsFromDict - unable to find 'subdet' field "
		    << std::endl ;
	}
      return (1);
    }
  
  field = m_dict->find_field("DetZside") ;
  if (field) 
    {
      m_DETZSIDE_INDEX = field->m_index ;
    }
  else 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'DetZside' field " 
	      << endmsg;
	}
      else
	{
	  std::cout <<  "CaloDM_ID::initLevelsFromDict - unable to find 'DetZside' field "
		    << std::endl ;
	}
      return (1);
    }
 
 field = m_dict->find_field("DMvalue") ;
  if (field) 
    {
      m_DMAT_INDEX = field->m_index ;
    }
  else 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'DMvalue' field " 
	      << endmsg;
	}
      else
	{
	  std::cout <<  "CaloDM_ID::initLevelsFromDict - unable to find 'DMvalue' field "
		    << std::endl ;
	}
      return (1);
    }
 

 field = m_dict->find_field("samplingvalue") ;
  if (field) 
    {
      m_SAMPLING_INDEX = field->m_index ;
    }
  else 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'samplingvalue' field " 
	      << endmsg;
	}
      else
	{
	  std::cout <<  "CaloDM_ID::initLevelsFromDict - unable to find 'samplingvalue' field "
		    << std::endl ;
	}
    return (1);
  }
  
  field = m_dict->find_field("DMregion") ;
  if (field) 
    {
      m_REGION_INDEX = field->m_index ;
    }
  else 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'DMregion' field " << endmsg;
	}
      else
	{
	  std::cout <<  "CaloDM_ID::initLevelsFromDict - unable to find 'DMregion' field "
		    << std::endl ;
	}
      return (1);
    }

  /* std::cout << "m_region= " << m_REGION_INDEX << std::endl; */

  field = m_dict->find_field("DMEta") ;
  if (field) {
    m_ETA_INDEX = field->m_index ;
  }
  else 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'DMEta' field " 
	      << endmsg;
	}
      else
	{
	  std::cout <<  "CaloDM_ID::initLevelsFromDict - unable to find 'DMEta' field "
		    << std::endl ;
	}
      return (1);
  }
 
  field = m_dict->find_field("phivalue") ;
  if (field) 
    {
      m_PHI_INDEX = field->m_index ;
    }
  else 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'phivalue' field " 
	      << endmsg;
	}
      else
	{
	  std::cout <<  "CaloDM_ID::initLevelsFromDict - unable to find 'phivalue' field "
		    << std::endl ;
	}
      return (1);
    }
 
 
  // Set the field implementations

  const IdDictRegion& region = *m_dict->m_regions[m_calodm_region_index];

  m_calo_impl     = region.m_implementation[m_CALO_INDEX]; 
  m_calodm_impl   = region.m_implementation[m_DETZSIDE_INDEX]; 
  m_dmat_impl     = region.m_implementation[m_DMAT_INDEX]; 
  m_sampling_impl = region.m_implementation[m_SAMPLING_INDEX]; 
  m_region_impl   = region.m_implementation[m_REGION_INDEX]; 
  m_eta_impl      = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl      = region.m_implementation[m_PHI_INDEX]; 

  if (!m_quiet) {
    if(m_msgSvc)
    {
      //      log << MSG::DEBUG << "CaloDM_ID::initLevelsFromDict - found levels " << endmsg ;
      //      log << MSG::DEBUG << "subdet         " << m_CALO_INDEX     << endmsg ;
      //      log << MSG::DEBUG << "detzside       " << m_DETZSIDE_INDEX   << endmsg ;
      //      log << MSG::DEBUG << "Dmat           " << m_DMAT_INDEX << endmsg ;
      //      log << MSG::DEBUG << "sampling       " << m_SAMPLING_INDEX << endmsg ;
      //      log << MSG::DEBUG << "region         " << m_REGION_INDEX   << endmsg ;
      //      log << MSG::DEBUG << "eta            " << m_ETA_INDEX      << endmsg ;
      //      log << MSG::DEBUG << "phi            " << m_PHI_INDEX      << endmsg ; 
      log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
      log << MSG::DEBUG << "calo      "  << m_calo_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "detzside  "  << m_calodm_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "dmat      "  << m_dmat_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "samp      "  << m_sampling_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "reg       "  << m_region_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "eta       "  << m_eta_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "phi       "  << m_phi_impl.show_to_string() << endmsg;
    }
    else
    { 
      //      std::cout << "CaloDM_ID::initLevelsFromDict - found levels " << std::endl ;
      //      std::cout << "subdet         " << m_CALO_INDEX     << std::endl ;
      //      std::cout << "detzside       " << m_DETZSIDE_INDEX   << std::endl ;
      //      std::cout << "Dmat           " << m_DMAT_INDEX << std::endl ;
      //      std::cout << "sampling       " << m_SAMPLING_INDEX << std::endl ;
      //      std::cout << "region         " << m_REGION_INDEX   << std::endl ;
      //      std::cout << "eta            " << m_ETA_INDEX      << std::endl ;
      //      std::cout << "phi            " << m_PHI_INDEX      << std::endl ; 
      std::cout << "decode index and bit fields for each level: " << std::endl;
      std::cout << "calo      "  << m_calo_impl.show_to_string() << std::endl;
      std::cout << "detzside  "  << m_calodm_impl.show_to_string() << std::endl;
      std::cout << "dmat      "  << m_dmat_impl.show_to_string() << std::endl;
      std::cout << "samp      "  << m_sampling_impl.show_to_string() << std::endl;
      std::cout << "reg       "  << m_region_impl.show_to_string() << std::endl;
      std::cout << "eta       "  << m_eta_impl.show_to_string() << std::endl;
      std::cout << "phi       "  << m_phi_impl.show_to_string() << std::endl;
    }
  }
  return(0) ;
}

/*=======================================*/
int   CaloDM_ID::init_lar_hashes(void) 
/*=======================================*/
{
  // Msg Service
  MsgStream log(m_msgSvc, "CaloDM_ID" );
  std::string strg;
  std::stringstream strm;

  // zone hash
  m_lar_zone_hash_max = m_full_lar_zone_range.cardinality();
  m_lar_zone_vec.resize(m_lar_zone_hash_max);
  unsigned int nids = 0;
  std::set<Identifier> ids;
  for (unsigned int i = 0; i < m_full_lar_zone_range.size(); ++i) {
    const Range& range = m_full_lar_zone_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      Identifier zon_id = zone_id   ( exp_id[m_DETZSIDE_INDEX],
				      exp_id[m_DMAT_INDEX], 
				      exp_id[m_SAMPLING_INDEX], 
				      exp_id[m_REGION_INDEX] ,
				      exp_id[m_ETA_INDEX] ,
				      exp_id[m_PHI_INDEX] ) ;
      if(!(ids.insert(zon_id)).second)
	{
	  if(m_msgSvc)
	    {
	      log << MSG::ERROR << " init_lar_hashes "
		  << " duplicated id for lardm id. nids= " << nids
		  << " compact Id  " << show_to_string(zon_id)
		  << endmsg; 
	    }
	  else
	    {
	      std::cout << " CaloDM_ID::init_lar_hashes "
			<< " Error: duplicated id for lardm id. nids= " << nids
			<< " compact Id  " ; 
	      (*first).show();
	      std::cout << std::endl;
	    }
	}
      nids++;
    }
  }
  if(ids.size() != m_lar_zone_hash_max) 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << " init_lar_hashes "
	      << " set size NOT EQUAL to hash max. size " << ids.size()
	      << " hash max " << m_lar_zone_hash_max << endmsg;
	}
      else
	{
	  std::cout << " CaloDM_ID::init_lar_hashes "
		    << " Error: set size NOT EQUAL to hash max. size " << ids.size()
		    << " hash max " << m_lar_zone_hash_max
		    << std::endl;
	}
      return (1);
    }

  nids=0;
  std::set<Identifier>::const_iterator first = ids.begin();
  std::set<Identifier>::const_iterator last  = ids.end();
  for (;first != last && nids < m_lar_zone_vec.size(); ++first) {
    m_lar_zone_vec[nids] = (*first) ;
    nids++;
  }

  // region hash
  m_lar_region_hash_max = m_full_lar_region_range.cardinality();
  m_lar_region_vec.resize(m_lar_region_hash_max);
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_lar_region_range.size(); ++i) {
    const Range& range = m_full_lar_region_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      //     std::cout << "exp_id = " << (std::string)exp_id << std::endl;
      Identifier reg_id = region_id   ( exp_id[m_DETZSIDE_INDEX],
					exp_id[m_DMAT_INDEX], 
					exp_id[m_SAMPLING_INDEX], 
					exp_id[m_REGION_INDEX] );
      if(!(ids.insert(reg_id)).second)
	{
	  if(m_msgSvc)
	    {
	      log << MSG::ERROR << " init_lar_hashes (regions) "
		  << " duplicated id for region id. nids= " << nids
		  << " compact Id  " << show_to_string(reg_id) 
		  << endmsg;
	    }
	  else
	    {
	      std::cout << " CaloDM_ID::init_lar_hashes (regions) "
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
  if(ids.size() != m_lar_region_hash_max) 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << " init_lar_hashes "
	      << " set size NOT EQUAL to region hash max. size " << ids.size()
	      << " region hash max " << m_lar_region_hash_max << endmsg;
	}
      else
	{
	  std::cout << " CaloDM_ID::init_lar_hashes "
		    << " Error: set size NOT EQUAL to region hash max. size " << ids.size()
		    << " region hash max " << m_lar_region_hash_max
		    << std::endl;
	}
    return (1);
  }

  nids=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nids < m_lar_region_vec.size(); ++first) {
    m_lar_region_vec[nids] = (*first) ;
    nids++;
  }

  return (0);
                   
}

/*=====================================*/
int   CaloDM_ID::init_tile_hashes(void) 
/*=====================================*/
{
  // Msg Service
  MsgStream log(m_msgSvc, "CaloDM_ID" );
  std::string strg;
  std::stringstream strm;

  // zone hash
  m_tile_zone_hash_max = m_full_tile_zone_range.cardinality();
  m_tile_zone_vec.resize(m_tile_zone_hash_max);
  unsigned int nids = 0;
  std::set<Identifier> ids;
  for (unsigned int i = 0; i < m_full_tile_zone_range.size(); ++i) {
    const Range& range = m_full_tile_zone_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      Identifier zon_id = zone_id   ( exp_id[m_DETZSIDE_INDEX],
				      exp_id[m_DMAT_INDEX], 
				      exp_id[m_SAMPLING_INDEX], 
				      exp_id[m_REGION_INDEX] ,
				      exp_id[m_ETA_INDEX] ,
				      exp_id[m_PHI_INDEX] ) ;
      /*
      std::cout << "detZside,dmat,samp,reg,eta,phi= " 
		<<exp_id[m_DETZSIDE_INDEX] << ", "
		<<exp_id[m_DMAT_INDEX] << ", "
		<<exp_id[m_SAMPLING_INDEX] << ", "
		<<exp_id[m_REGION_INDEX] << ", "
		<<exp_id[m_ETA_INDEX] << ", "
		<<exp_id[m_PHI_INDEX];
      std::cout << std::endl;
      std::cout << "tile zon_id= " << show_to_string(zon_id) << std::endl;
      */
      if(!(ids.insert(zon_id)).second)
	{
	  if(m_msgSvc)
	    {
	      log << MSG::ERROR << " init_tile_hashes "
		  << " duplicated id for tiledm id. nids= " << nids
		  << " compact Id  " << show_to_string(zon_id)
		  << endmsg;
	    }
	  else
	    {
	      std::cout << " CaloDM_ID::init_tile_hashes "
			<< " Error: duplicated id for tiledm id. nids= " << nids
			<< " compact Id  " ;
	      (*first).show();
	      std::cout << std::endl;
	    }
	}
      nids++;
    }
  }
  if(ids.size() != m_tile_zone_hash_max) 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << " init_tile_hashes "
	      << " set size NOT EQUAL to hash max. size " << ids.size()
	      << " hash max " << m_tile_zone_hash_max << endmsg;
	}
      else
	{
	  std::cout << " CaloDM_ID::init_tile_hashes "
		    << " Error: set size NOT EQUAL to hash max. size " << ids.size()
		    << " hash max " << m_tile_zone_hash_max
		    << std::endl;
	}
      return (1);
  }

  nids=0;
  std::set<Identifier>::const_iterator first = ids.begin();
  std::set<Identifier>::const_iterator last  = ids.end();
  for (;first != last && nids < m_tile_zone_vec.size(); ++first) {
    m_tile_zone_vec[nids] = (*first) ;
    nids++;
  }

  // region hash
  m_tile_region_hash_max = m_full_tile_region_range.cardinality();
  m_tile_region_vec.resize(m_tile_region_hash_max);
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_tile_region_range.size(); ++i) {
    const Range& range = m_full_tile_region_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      //     std::cout << "exp_id = " << (std::string)exp_id << std::endl;
      Identifier reg_id = region_id   ( exp_id[m_DETZSIDE_INDEX],
					exp_id[m_DMAT_INDEX], 
					exp_id[m_SAMPLING_INDEX], 
					exp_id[m_REGION_INDEX] );
      if(!(ids.insert(reg_id)).second)
	{
	  if(m_msgSvc)
	    {
	      log << MSG::ERROR << " init_tile_hashes (regions) "
		  << " duplicated id for region id. nids= " << nids
		  << " compact Id  " << show_to_string(reg_id) 
		  << endmsg;
	    }
	  else
	    {
	      std::cout << " CaloDM_ID::init_tile_hashes (regions) "
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
  if(ids.size() != m_tile_region_hash_max) 
    {
      if(m_msgSvc)
	{
	  log << MSG::ERROR << " init_tile_hashes "
	      << " set size NOT EQUAL to region hash max. size " << ids.size()
	      << " region hash max " << m_tile_region_hash_max << endmsg;
	}
      else
	{
	  std::cout << " CaloDM_ID::init_tile_hashes "
		    << " Error: set size NOT EQUAL to region hash max. size " << ids.size()
		    << " region hash max " << m_tile_region_hash_max
		    << std::endl;
	}
    return (1);
  }

  nids=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nids < m_tile_region_vec.size(); ++first) {
    m_tile_region_vec[nids] = (*first) ;
    nids++;
  }

  return (0);
                   
}


