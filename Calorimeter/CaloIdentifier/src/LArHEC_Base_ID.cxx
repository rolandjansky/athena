/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/src/LArHEC_Base_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Factor out code common between LArHEC_ID and LArHEC_SuperCell_ID.
 */


#include "CaloIdentifier/LArHEC_Base_ID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include "LArHEC_region.h"
#include "CxxUtils/StrFormat.h"

#include "GaudiKernel/MsgStream.h"

#include <string>
#include <set>
#include <iostream>
#include <math.h>

using CxxUtils::strformat;


LArHEC_Base_ID::LArHEC_Base_ID (const std::string& name, bool supercell) :
    CaloIDHelper (name)
  , m_slar (supercell ? 1 : 0)
  , m_hec_region_index(0) 
  , m_LAR_INDEX(999) 
  , m_HEC_INDEX(999) 
  , m_POSNEG_INDEX(999) 
  , m_SAMPLING_INDEX(999) 
  , m_REGION_INDEX(999) 
  , m_ETA_INDEX(999)
  , m_PHI_INDEX(999)
  , m_SLAR_INDEX(999)
  , m_two_sym_sides(1)
{
}

LArHEC_Base_ID::~LArHEC_Base_ID(void) 
{
  std::vector<LArHEC_region*>::iterator first = m_vecOfRegions.begin();
  std::vector<LArHEC_region*>::iterator last  = m_vecOfRegions.end();
  for (; first != last; ++first) delete (*first);
}

Identifier LArHEC_Base_ID::channel_id  ( int pos_neg, int sampling, 
                                         int eta,     int phi ) const
{
    // must calculate region number and shift eta 
    int region=999;
    if ( 0 <= eta && eta < 10 ) { region  = 0;}
    else if ( 10 <= eta && eta < 14 ) { region  = 1; eta -= 10;}

    return (channel_id (pos_neg, sampling, region, eta, phi));
}


Identifier LArHEC_Base_ID::channel_id  ( int pos_neg, int sampling, int sector, int region, 
				    int eta,     int phi_sector ) const
{
    int phi = ( region == 0 ? sector*2 + phi_sector : sector ); 
    return (channel_id (pos_neg, sampling, region, eta, phi ));
}

int LArHEC_Base_ID::eta_min(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext region_cntxt = region_context();
  if(!get_expanded_id(regId, expId, &region_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
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

int LArHEC_Base_ID::eta_max(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext region_cntxt = region_context();
  if(!get_expanded_id(regId, expId, &region_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
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

int LArHEC_Base_ID::phi_min_init(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext region_cntxt = region_context();
  if(!get_expanded_id(regId, expId, &region_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
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
  return (-999);  // default
}

int LArHEC_Base_ID::phi_max(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext region_cntxt = region_context();
  if(!get_expanded_id(regId, expId, &region_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
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

int  LArHEC_Base_ID::initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
                                                       const std::string& group_name)
/*===================================================================*/
{
  MsgStream log(m_msgSvc, "LArHEC_Base_ID" );
  
  std::string strg = "initialize_base_from_dictionary";
  log << MSG::DEBUG << strg << endmsg;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if(m_msgSvc)log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  // init base object
  if(CaloIDHelper::initialize_base_from_dictionary(dict_mgr,
                                                   "LArCalorimeter"))
    return (1);
  
  // initialize dictionary version
  AtlasDetectorID::setDictVersion(dict_mgr, "LArCalorimeter");

  // Initialize the field indices
  if(initLevelsFromDict(group_name)) {
    strg =  " initialize_base_from_dict - cannot initialize HEC part of LArCalorimeter dictionary ";
    if(m_msgSvc) {
      log << MSG::WARNING << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    //    return (1);   // to allow TB dictionary (no HEC in H8)
  }
  else  {
    // Find value for the field LAr Calorimeter   
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
    int larField   = -1;
    if (atlasDict->get_label_value("subdet", "LArCalorimeter", larField)) {
      std::stringstream strm ;
      strm <<  atlasDict->m_name ;
      strg = "Could not get value for label 'LArCalorimeter' of field 'subdet' in dictionary " 
      + strm.str();
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else {
	std::cout << strg << std::endl;
      }
      return (1);
    }

    // Find value for the field LArHEC
    int larHecField   = -1;
    if (dict()->get_label_value("part", "LArHEC", larHecField)) {
      std::stringstream strm ;
      strm <<  atlasDict->m_name ;
      strg = "Could not get value for label 'LArHEC' of field 'part' in dictionary " 
      + strm.str();
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else {
	std::cout << strg << std::endl;
      }
      return (1);
    }
    
    // Set up id for region and range prefix
      ExpandedIdentifier exp_region_id;
      exp_region_id.add(larField);
      exp_region_id.add(larHecField);
      Range prefix;
      m_full_channel_range = dict()->build_multirange(exp_region_id, group_name, prefix);
      m_full_region_range = dict()->build_multirange(exp_region_id, group_name, prefix, "region");  

      if (!m_quiet) {
        std::string strg0 = "initialize_from_dict : " ;
        std::string strg1 = " channel range -> " + (std::string)m_full_channel_range;
        std::string strg2 = " region range -> "  + (std::string)m_full_region_range;
        if(m_msgSvc) {
          log << MSG::DEBUG << strg0 << endmsg;
          log << MSG::DEBUG << strg1 << endmsg;
          log << MSG::DEBUG << strg2 << endmsg;
        }
        else {
          std::cout << strg0 << std::endl;
          std::cout << strg1 << std::endl;
          std::cout << strg2 << std::endl;
        }
      }


    // initilize m_two_sym_sides
    m_two_sym_sides = ( dictionaryVersion() == "fullAtlas" );

    // Setup the hash tables
    if(init_hashes()) return (1);

    // initialize dictionary regions
    if (fill_vec_of_dict_regions (group_name)) return 1;
    
    // added nov 2005 to speed up CaloNoiseTool initialisation
    m_vecOfPhiMin.resize(region_hash_max());
    for (unsigned int i = 0; i < region_hash_max(); ++i) {
      Identifier regId = region_id(i); 
      m_vecOfPhiMin[i] = phi_min_init(regId);
    }

    // Setup for hash calculation
    // The regions have uniform eta/phi granularity.
    // The lookup table only needs to contain the
    // hash offset for each region, the first eta index
    // and the number of phi cells.

    // The implementation requires:

    //   1) a lookup table for each region containing hash offset,
    //      etamin and nphi
    //   2) a decoder to access the "index" corresponding to the
    //      pn/samp/reg fields. These fields use 4 bits, so the
    //      vector has a length of 16 for 16 regions.

    // Create decoder for fields pn to region
    IdDictFieldImplementation::size_type bits = 
	m_pn_impl.bits() +
	m_sampling_impl.bits() +
	m_region_impl.bits();
    IdDictFieldImplementation::size_type bits_offset = m_pn_impl.bits_offset();
    m_pn_reg_impl.set_bits(bits, bits_offset);
      
    //        std::cout << "pn_reg "  << m_pn_reg_impl.decode_index()  << " " 
    //    << (std::string)m_pn_reg_impl.ored_field()  << " " 
    //    << std::hex << m_pn_reg_impl.mask() << " " 
    //    << m_pn_reg_impl.zeroing_mask() << " " 
    //    << std::dec << m_pn_reg_impl.shift()
    //    << " " << m_pn_reg_impl.bits() << " " <<m_pn_reg_impl.bits_offset()
    //    << std::endl;
    
    
    // Set up vector as lookup table for hash calculation. 
    m_hash_calcs.resize(16);
      
    for (unsigned int i = 0; i < region_hash_max(); ++i) {
	
      Identifier regId = region_id(i) ;
      HashCalc hc;
	
      int etamin = eta_min(regId);
      if(etamin < 0) {
	etamin = 0;
	std::string strg = "setting etamin to 0 because actual value not found for regId " 
	  + show_to_string(regId);
	if(m_msgSvc) {
	  log << MSG::WARNING << strg << endmsg;
	}
	else {
	  std::cout << strg << std::endl;
	}
      }
      int phimin = phi_min(regId);
      if(phimin < 0) {
	phimin = 0;
	std::string strg = "setting phimin to 0 because actual value not found for regId " 
	  + show_to_string(regId);
	if(m_msgSvc) {
	  log << MSG::WARNING << strg << endmsg;
	}
	else {
	  std::cout << strg << std::endl;
	}
      }
      Identifier min = channel_id ( regId, etamin, phimin);
      IdentifierHash min_hash = channel_hash_binary_search(min);
      hc.m_hash   = min_hash;
      hc.m_etamin = etamin;
      hc.m_phimin = phimin;
      hc.m_nphi   = phi_max(min)-phimin+1 ;
      m_hash_calcs[m_pn_reg_impl.unpack(min)] = hc;
	
      if (m_pn_reg_impl.unpack(min) > 15) {
	std::stringstream strm ;
	strm << i;
	strm << m_pn_reg_impl.unpack(min);
	std::string strg = "min > 15 " 
	  + strm.str() + show_to_string(min) ;
	if(m_msgSvc) {
	  log << MSG::ERROR << strg << endmsg;
	}
	else {
	  std::cout << strg << std::endl;
	}
      }
    }
    
    // Check hash calculation
    for (unsigned int i = 0; i < channel_hash_max(); ++i) {
      Identifier id = channel_id(i);
      if (channel_hash(id) != i) {
	std::stringstream strm ;
	strm << channel_hash(id);
	strm << i;
	std::string strg = "channel ranges, id, hash, i = " 
	  + show_to_string(id) + strm.str();
	if(m_msgSvc) {
	  log << MSG::ERROR << strg << endmsg;
	}
	else {
	  std::cout << strg << std::endl;
	}
      }
    }
  }

  // Setup hash tables for finding neighbors
  if(m_do_neighbours)  {
    if(init_neighbors()) return (1);     
  }

  return 0;
}



void LArHEC_Base_ID::region_id_checks   ( int pos_neg, int sampling, int region ) const
{
	
  // Check that id is within allowed range
  // Fill expanded id (initially with 4/2/0/0/0/0/0)
  ExpandedIdentifier expId(lar_hec_exp());
  expId << pos_neg << sampling << region ;
      
  if (!m_full_region_range.match(expId)) { 
    std::string errorMessage = "LArHEC_Base_ID::region_id() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_region_range;
    throw LArID_Exception(errorMessage , 7);
  }

}

void LArHEC_Base_ID::channel_id_checks   ( int pos_neg, int sampling, int region,
                                           int eta,     int phi ) const
{  
	
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(lar_hec_exp());
  expId << pos_neg << sampling << region << eta << phi << m_slar ;

  if (!m_full_channel_range.match(expId)) { 
    std::string errorMessage = "LArHEC_Base_ID::channel_id() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_channel_range;
    throw LArID_Exception(errorMessage , 8);
  }
}

void LArHEC_Base_ID::channel_id_checks   ( const Identifier regionId,
                                           int eta,       int phi ) const
{
	
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId; 

  IdContext context = region_context();
  if (get_expanded_id(regionId, expId, &context)) {
    std::string errorMessage = "LArHEC_Base_ID::channel_id(regId) result is not OK: ID = "
      + show_to_string(regionId) ;
    throw LArID_Exception(errorMessage , 8);
  }

  expId << eta << phi << m_slar;
      
  if (!m_full_channel_range.match(expId)) { 
    std::string errorMessage = "LArHEC_Base_ID::channel_id(regId) result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_channel_range;
    throw LArID_Exception(errorMessage , 8);
  }
}

int  LArHEC_Base_ID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
    // We assume that the context is >= region
    exp_id.clear();
    exp_id << lar_field_value()
      	   << lar_hec_field_value()
	   << pos_neg(id)
	   << sampling(id)
	   << region(id);
    if(context && context->end_index() >= m_ETA_INDEX) {
	exp_id << eta(id);
	if(context->end_index() >= m_PHI_INDEX) {
	    exp_id << phi(id);
    	    if ( context->end_index() >= m_SLAR_INDEX) {
              exp_id << (unsigned)is_supercell(id);
            }
	}
    }
    return (0);
}

int LArHEC_Base_ID::initLevelsFromDict (const std::string& /*group_name*/) 
{
  MsgStream log(m_msgSvc, "LArHEC_Base_ID" );

  if(!dict()) {
    std::string strg = "initLevelsFromDict - dictionary NOT initialized ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  // Find out which identifier field corresponds to each level.

  m_LAR_INDEX        = 999 ;
  m_HEC_INDEX        = 999 ;
  m_POSNEG_INDEX     = 999 ;
  m_SAMPLING_INDEX   = 999 ;
  m_REGION_INDEX     = 999 ;
  m_ETA_INDEX        = 999 ;
  m_PHI_INDEX        = 999 ;
  m_SLAR_INDEX       = 999 ;

  // Save index to a HEC region for unpacking
  ExpandedIdentifier expId(lar_hec_exp());
  if (dict()->find_region(expId,m_hec_region_index)){
    std::stringstream strm ;
    strm <<  m_hec_region_index ;
    std::string strg = "initLevelsFromDict - unable to find hec region index: id, reg "  
      +  (std::string)expId + strm.str();
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  IdDictField* field = dict()->find_field("subdet") ;
  if (field) {
    m_LAR_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'subdet' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("part") ;
  if (field) {
    m_HEC_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'part' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("barrel-endcap") ;
  if (field) {
    m_POSNEG_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'barrel-endcap' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("sampling") ;
  if (field) {
    m_SAMPLING_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'sampling' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("region") ;
  if (field) {
    m_REGION_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'region' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("eta") ;
  if (field) {
    m_ETA_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'eta' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("phi") ;
  if (field) {
    m_PHI_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'phi' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("is-slar-hec") ;
  if (field) {
    m_SLAR_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'is-slar-hec' field "
          << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'is-slar-hec' field "
                << std::endl ;
    }
    return (1);
  }


  // Set the field implementations

  const IdDictRegion& region = *dict()->m_regions[m_hec_region_index];

  /*
  std::cout << "LArHEC_Base_ID::initLevelsFromDict - found levels " << std::endl ;
  std::cout << "lar            " << m_LAR_INDEX      << std::endl ;
  std::cout << "part           " << m_HEC_INDEX      << std::endl ;
  std::cout << "pos-neg        " << m_POSNEG_INDEX   << std::endl ;
  std::cout << "sampling       " << m_SAMPLING_INDEX << std::endl ;
  std::cout << "region         " << m_REGION_INDEX   << std::endl ;
  std::cout << "eta            " << m_ETA_INDEX      << std::endl ;
  std::cout << "phi            " << m_PHI_INDEX      << std::endl ;
  std::cout << "slar           " << m_SLAR_INDEX     << std::endl ;
  */

  m_lar_impl      = region.m_implementation[m_LAR_INDEX]; 
  m_hec_impl      = region.m_implementation[m_HEC_INDEX]; 
  m_pn_impl       = region.m_implementation[m_POSNEG_INDEX]; 
  m_sampling_impl = region.m_implementation[m_SAMPLING_INDEX]; 
  m_region_impl   = region.m_implementation[m_REGION_INDEX]; 
  m_eta_impl      = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl      = region.m_implementation[m_PHI_INDEX]; 
  m_slar_impl     = region.m_implementation[m_SLAR_INDEX]; 


  if (!m_quiet) {
    if(m_msgSvc) {
      log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
      log << MSG::DEBUG << "lar  "  << m_lar_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "hec  "  << m_hec_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "pn   "  << m_pn_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "samp "  << m_sampling_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "reg  "  << m_region_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "eta  "  << m_eta_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "phi  "  << m_phi_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "is-slar  " << m_slar_impl.show_to_string() << endmsg;
    } 
    else {
      std::cout << "decode index and bit fields for each level: " << std::endl;
      std::cout << "lar  "  << m_lar_impl.show_to_string() << std::endl;
      std::cout << "hec  "  << m_hec_impl.show_to_string() << std::endl;
      std::cout << "pn   "  << m_pn_impl.show_to_string() << std::endl;
      std::cout << "samp "  << m_sampling_impl.show_to_string() << std::endl;
      std::cout << "reg  "  << m_region_impl.show_to_string() << std::endl;
      std::cout << "eta  "  << m_eta_impl.show_to_string() << std::endl;
      std::cout << "phi  "  << m_phi_impl.show_to_string() << std::endl;
      std::cout << "is-slar  " << m_slar_impl.show_to_string() << std::endl;
    }
  }
    
  return(0) ;
}

int LArHEC_Base_ID::init_hashes() 
{
  if (channels().init (*this, "channels",
                       m_full_channel_range, &LArHEC_Base_ID::channel_id,
                       m_SLAR_INDEX))
    return 1;
  if (regions().init (*this, "regions",
                      m_full_region_range, &LArHEC_Base_ID::region_id,
                      m_REGION_INDEX))
    return 1;

  return (0);
}

int   LArHEC_Base_ID::get_neighbours(const IdentifierHash id, const LArNeighbours::neighbourOption& option, std::vector<IdentifierHash>& neighbourList) const
{
  int result = 1; 

  neighbourList.clear();

  if(!m_do_neighbours) {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArHEC_Base_ID" );
      log << MSG::WARNING << "neighbours not initialized !!! returning empty list" << endmsg;
    }
    else {
      std::cout << " LArHEC_Base_ID: neighbours not initialized !!! returning empty list " << std::endl;
    }
    return result;
  }

  if(id>=channel_hash_max()) {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArHEC_Base_ID" );
      log << MSG::WARNING << "neighbours requested for  non-existing channel -- id/max " << id << "/"
          << channel_hash_max() << endmsg;
    }
    else {
      std::cout << " neighbours requested for non-existing channel -- id/max " << id << "/"
                << channel_hash_max() << std::endl;
    }
    return result;
  }

  const short int maxNeighb=20;
  IdentifierHash neighbList[maxNeighb];
  int neighbourIndex = 0;

  // cell index
  unsigned int index=id;

  //
  // .... find in which region is the cell   
  //
  short int regionN=m_vecOfCellInfo[index];
  // get pointer to this region
  LArHEC_region* hecRegion = m_vecOfRegions[regionN];
  // retrieve characteristic numbers for this region
  short int nPhi = hecRegion->phiN();
  float gPhi = hecRegion->phiGranularity();
  unsigned int minHash = hecRegion->hashMin();
  unsigned int maxHash = hecRegion->hashMax();

  bool corners2DOnly  = ( (option & LArNeighbours::all2D) 
			  == LArNeighbours::corners2D );
  //
  // .... previous neighbour in phi
  //
  IdentifierHash prevNeighbInPhi=NOT_VALID_HASH;
  if( (option & LArNeighbours::prevInPhi) 
      || corners2DOnly ){
    if(!get_prevInPhi(hecRegion, index, nPhi, minHash, neighbourIndex, neighbList)){
      prevNeighbInPhi=neighbList[neighbourIndex-1];
      if( corners2DOnly ){
	neighbourIndex--;
      }
    }
  }

  //
  // ....next neighbour in phi
  //
  IdentifierHash nextNeighbInPhi=NOT_VALID_HASH;
  if( (option & LArNeighbours::nextInPhi)
      || corners2DOnly ){
    if(!get_nextInPhi(hecRegion, index, nPhi, minHash, neighbourIndex, neighbList)){
      nextNeighbInPhi=neighbList[neighbourIndex-1];
      if( corners2DOnly ){
	neighbourIndex--;
      }
    }
  }

  //
  // ....previous neighbours in eta
  //
  unsigned int nPrevBiggerCell=NOT_VALID_HASH;
  if( (option & LArNeighbours::prevInEta) ){
    get_prevInEta(hecRegion, index, nPhi, gPhi, minHash, neighbourIndex, neighbList, nPrevBiggerCell);
  }

  //
  // ....next neighbours in eta
  //
  unsigned int nNextBiggerCell=NOT_VALID_HASH;
  if( (option & LArNeighbours::nextInEta) ){
    get_nextInEta(hecRegion, index, nPhi, gPhi, maxHash, neighbourIndex, neighbList, nNextBiggerCell);
  }

  //
  // ....corners in the same sampling
  //
  if( (option & LArNeighbours::corners2D) ){
    if(prevNeighbInPhi != NOT_VALID_HASH){
      unsigned int index1=prevNeighbInPhi;
      int oldNeighbourIndex = neighbourIndex;
      // allow only 1 corner cell in order to avoid the problem of 
      // non-mutual neighbourness
      // since the cells come ordered in phi it should be the last cell for
      // prevNeighbInPhi as starting points
      get_prevInEta(hecRegion, index1, nPhi, gPhi, minHash, neighbourIndex, neighbList, nPrevBiggerCell);
      if ( neighbourIndex > oldNeighbourIndex+1 ) {
	neighbList[oldNeighbourIndex] = neighbList[neighbourIndex-1];
	neighbourIndex = oldNeighbourIndex+1;
      }
      oldNeighbourIndex = neighbourIndex;
      get_nextInEta(hecRegion, index1, nPhi, gPhi, maxHash, neighbourIndex, neighbList, nNextBiggerCell);
      if ( neighbourIndex > oldNeighbourIndex+1 ) {
	neighbList[oldNeighbourIndex] = neighbList[neighbourIndex-1];
	neighbourIndex = oldNeighbourIndex+1;
      }
    }

    if(nextNeighbInPhi != NOT_VALID_HASH){
      unsigned int index2=nextNeighbInPhi;
      int oldNeighbourIndex = neighbourIndex;
      // allow only 1 corner cell in order to avoid the problem of 
      // non-mutual neighbourness
      // since the cells come ordered in phi it should be the 1st cell for
      // nextNeighbInPhi 
      get_prevInEta(hecRegion, index2, nPhi, gPhi, minHash, neighbourIndex, neighbList, nPrevBiggerCell);
      if ( neighbourIndex > oldNeighbourIndex+1 ) {
	neighbourIndex = oldNeighbourIndex+1;
      }
      oldNeighbourIndex = neighbourIndex;
      get_nextInEta(hecRegion, index2, nPhi, gPhi, maxHash, neighbourIndex, neighbList, nNextBiggerCell);
      if ( neighbourIndex > oldNeighbourIndex+1 ) {
	neighbourIndex = oldNeighbourIndex+1;
      }
    }
  }

  //
  // .... neighbours in sampling (common code)
  // HEC caracteristics = no granularity change, partial overlap of samplings
  //
  if( (option & LArNeighbours::upAndDown) ){
    // initial eta granularity
    float granEta = hecRegion->etaGranularity();
    // initial eta
    int nEta =  int( (index-minHash) / nPhi);
    double absEta = hecRegion->etaMin() + nEta*granEta;

    // previous neighbours in sampling
    if( (option & LArNeighbours::prevInSamp) ){
      get_prevInSamp(hecRegion, index, nPhi, minHash, absEta, neighbourIndex, neighbList);
    } // end option

    // next neighbours in sampling
    if( (option & LArNeighbours::nextInSamp) ){
      get_nextInSamp(hecRegion, index, nPhi, minHash, absEta, neighbourIndex, neighbList);
    }
  }

  neighbourList.resize(neighbourIndex);
  if (neighbourIndex <= maxNeighb) {
    std::copy (&neighbList[0], &neighbList[neighbourIndex],  neighbourList.begin());
    result = 0 ;
  } else {
    std::stringstream strm ;
    strm << neighbourIndex ;
    std::string strg = "more than 20 neighbours for this cell, NONE will be retained " 
      + strm.str();
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArHEC_Base_ID" );
      log << MSG::WARNING << strg << endmsg;
    }
    else {
      std::cout << "WARNING: " << strg << std::endl;
    }
  } 
  return result;

}

int   LArHEC_Base_ID::get_prevInPhi(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
			       int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 
  if(!hecRegion->isPhiMin(index)) {
    unsigned int nIndex = index-1;
    if( ((index-minHash)%(nPhi)) == 0 ) nIndex=index+nPhi-1;
    IdentifierHash nHash = nIndex;
    neighbList[neighbourIndex] = nHash;
    neighbourIndex++;
    result = 0;
  }
  return result;
}

int   LArHEC_Base_ID::get_nextInPhi(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
			       int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 
  if(!hecRegion->isPhiMax(index)) {
    unsigned int nIndex = index+1;
    if( ((index-minHash+1)%(nPhi)) == 0 ) nIndex=index-nPhi+1;
    IdentifierHash nHash = nIndex;
    neighbList[neighbourIndex] = nHash;
    neighbourIndex++;
    result = 0;
  }
  return result;
}

int   LArHEC_Base_ID::get_prevInEta(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const float& gPhi, const unsigned int& minHash, 
				int& neighbourIndex, IdentifierHash* neighbList, unsigned int& nBiggerCell) const
{
  int result = 1; 
  unsigned int nIndex = 0;
  IdentifierHash nHash = 0;

  if( hecRegion->isEtaMin(index)){
    // eta == etaMin -> go to previous region in eta
    short int nPrevEtaRegion = hecRegion->prevEtaRegion();
    // no neighbour if no previous region in eta
    if( nPrevEtaRegion != NOT_VALID_HEC_REGION ) {
      LArHEC_region* prevHecRegion = m_vecOfRegions[nPrevEtaRegion];
      short int nPhiMinus = prevHecRegion->phiN();
      float gPhiMinus= prevHecRegion->phiGranularity();
      unsigned int maxHashMinus = prevHecRegion->hashMax();
      float phiMargin = 0.25*std::min(gPhi,gPhiMinus);
      float rPhi = (index-minHash)*gPhi+hecRegion->phiMin();
      int nPhiMinusFirst = int(floor((rPhi     -prevHecRegion->phiMin())
				     /gPhiMinus+phiMargin))
	+maxHashMinus-nPhiMinus;
      int nPhiMinusNext  = int(floor((rPhi+gPhi-prevHecRegion->phiMin())
				     /gPhiMinus+phiMargin))
	+maxHashMinus-nPhiMinus;
      if ( nPhiMinusNext == nPhiMinusFirst ) nPhiMinusNext++;
		
      for(int i=nPhiMinusFirst; i<nPhiMinusNext; i++){
	nIndex = i ;
	if(nIndex != nBiggerCell) {
	  nHash = nIndex;
	  neighbList[neighbourIndex] = nHash;
	  neighbourIndex++;
	  result = 0;
	}
	// to avoid duplicated cells in corners
	if(gPhi < gPhiMinus && nBiggerCell == NOT_VALID_HASH) nBiggerCell=nIndex;
      }
    }
  }
  else {
    // stay in same region (1 neighbour)
    nIndex = index - nPhi;
    nHash = nIndex;
    neighbList[neighbourIndex] = nHash;
    neighbourIndex++;
    result = 0;
  }
  return result;
}

int   LArHEC_Base_ID::get_nextInEta(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const float& gPhi, 
				const unsigned int& maxHash, 
				int& neighbourIndex, IdentifierHash* neighbList, unsigned int& nBiggerCell) const
{
  int result = 1; 
  unsigned int nIndex = 0;
  IdentifierHash nHash = 0;

  if( hecRegion->isEtaMax(index)){
    // eta == etaMax -> go to next region in eta
    short int nNextEtaRegion = hecRegion->nextEtaRegion();
    // no neighbour if no next region in eta
    if( nNextEtaRegion != NOT_VALID_HEC_REGION ) {
      LArHEC_region* nextHecRegion = m_vecOfRegions[nNextEtaRegion];
      float gPhiPlus= nextHecRegion->phiGranularity();
      unsigned int minHashPlus = nextHecRegion->hashMin();
      float phiMargin = 0.25*std::min(gPhi,gPhiPlus);
      float rPhi = (index+nPhi-maxHash)*gPhi+hecRegion->phiMin();
      int nPhiPlusFirst = int(floor((rPhi     -nextHecRegion->phiMin())
				    /gPhiPlus+phiMargin))+minHashPlus;
      int nPhiPlusNext  = int(floor((rPhi+gPhi-nextHecRegion->phiMin())
				    /gPhiPlus+phiMargin))+minHashPlus;
      if ( nPhiPlusNext == nPhiPlusFirst ) nPhiPlusNext++;

      for(int i=nPhiPlusFirst; i<nPhiPlusNext; i++){
	nIndex = i ;
	if(nIndex != nBiggerCell) {
	  nHash = nIndex;
	  neighbList[neighbourIndex] = nHash;
	  neighbourIndex++;
	  result = 0;
	}
	// to avoid duplicated cells in corners
	if(gPhi < gPhiPlus && nBiggerCell == NOT_VALID_HASH) nBiggerCell=nIndex;
      }
    }
  }
  else {
    // stay in same region (1 neighbour)
    nIndex = index + nPhi;
    nHash = nIndex;
    neighbList[neighbourIndex] = nHash;
    neighbourIndex++;
    result = 0;
  }
  return result;
}

int   LArHEC_Base_ID::get_prevInSamp(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
				 const double& absEta, 
				 int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 
  // neighbours' indices
  unsigned int nIndex=0;
  // neighbours' hash
  IdentifierHash nHash=0;

  // previous region in sampling
  const std::vector<short int>& prevSampRegion=hecRegion->prevSamplingRegion();
  int nPrevSampReg = prevSampRegion.size();
  if(nPrevSampReg > 0) {
    float gEta = hecRegion->etaGranularity();
    float gPhi = hecRegion->phiGranularity();
    for(int ireg=0; ireg<nPrevSampReg; ireg++) {
      LArHEC_region* prevHecRegion = m_vecOfRegions[prevSampRegion[ireg]];
      float minEtaMinus = prevHecRegion->etaMin();
      float maxEtaMinus = prevHecRegion->etaMax();
      // eta granularity of previous region
      float granEtaMinus = prevHecRegion->etaGranularity();
      float margin = 0.25*std::min(gEta,granEtaMinus);
      if((minEtaMinus < absEta+gEta-margin) && (absEta+margin < maxEtaMinus)) {
	
	// max phi of previous region in sampling
	short int nPhiMinus = prevHecRegion->phiN();
	// first hash of previous region in sampling
	unsigned int minHashMinus = prevHecRegion->hashMin(); 
	float gPhiMinus = prevHecRegion->phiGranularity();
	float phiMargin = 0.25*std::min(gPhi,gPhiMinus);
	// phi 'coordinate' in initial region
	float rPhi = ((index-minHash)%nPhi)*gPhi+hecRegion->phiMin();
	int nPhiMinusFirst = int(floor((rPhi     -prevHecRegion->phiMin())
				       /gPhiMinus+phiMargin));
	int nPhiMinusNext  = int(floor((rPhi+gPhi-prevHecRegion->phiMin())
				       /gPhiMinus+phiMargin));
	if ( nPhiMinusNext == nPhiMinusFirst ) nPhiMinusNext++;
	// eta coordinate in initial region
	double fEtaMinus = (absEta-minEtaMinus) / granEtaMinus + margin;
	short int nEtaMinus = int(fEtaMinus) ;
	for(int i=nPhiMinusFirst; i<nPhiMinusNext; i++){
	  nIndex = minHashMinus + nEtaMinus * nPhiMinus + i;
	  if( (nIndex >= prevHecRegion->hashMin()) && (nIndex < prevHecRegion->hashMax()) ) {
	    nHash = nIndex;
	    neighbList[neighbourIndex] = nHash;
	    neighbourIndex++;
	    result = 0;
	  }
	}
      } // end eta condition
    } // end loop on ireg
  } // end if(nPrevSampReg>0)
  return result;
}

int   LArHEC_Base_ID::get_nextInSamp(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
				 const double& absEta, 
				 int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 
  // neighbours' indices
  unsigned int nIndex=0;
  // neighbours' hash
  IdentifierHash nHash=0;

  const std::vector<short int>& nextSampRegion=hecRegion->nextSamplingRegion();
  int nNextSampReg = nextSampRegion.size();
  if(nNextSampReg > 0) {
    float gEta = hecRegion->etaGranularity();
    float gPhi = hecRegion->phiGranularity();
    for(int ireg=0; ireg<nNextSampReg; ireg++) {
      LArHEC_region* nextHecRegion = m_vecOfRegions[nextSampRegion[ireg]];
      float granEtaPlus = nextHecRegion->etaGranularity();
      float minEtaPlus = nextHecRegion->etaMin();
      float maxEtaPlus = nextHecRegion->etaMax();
      float margin = 0.25*std::min(gEta,granEtaPlus);
      if((minEtaPlus < absEta+gEta-margin) && (absEta+margin < maxEtaPlus)) {

	short int nPhiPlus = nextHecRegion->phiN();
	unsigned int minHashPlus = nextHecRegion->hashMin(); 
	float gPhiPlus = nextHecRegion->phiGranularity();
	float phiMargin = 0.25*std::min(gPhi,gPhiPlus);
	// phi 'coordinate' in initial region
	float rPhi = ((index-minHash)%nPhi)*gPhi+hecRegion->phiMin();
	int nPhiPlusFirst = int(floor((rPhi     -nextHecRegion->phiMin())
				      /gPhiPlus+phiMargin));
	int nPhiPlusNext  = int(floor((rPhi+gPhi-nextHecRegion->phiMin())
				      /gPhiPlus+phiMargin));
	if ( nPhiPlusNext == nPhiPlusFirst ) nPhiPlusNext++;

	double fEtaPlus = (absEta-minEtaPlus) / granEtaPlus + margin ;
	int nEtaPlus = int(fEtaPlus) ;
	for(int i=nPhiPlusFirst; i<nPhiPlusNext; i++){
	  nIndex = minHashPlus + nEtaPlus * nPhiPlus + i;
	  if( (nIndex >= nextHecRegion->hashMin()) && (nIndex < nextHecRegion->hashMax()) ) {
	    nHash = nIndex;
	    neighbList[neighbourIndex] = nHash;
	    neighbourIndex++;
	    result = 0;
	  }
	}
      }
    }
  }
  return result;
}


int LArHEC_Base_ID::init_neighbors(void) 
{
  MsgStream log(m_msgSvc, "LArHEC_Base_ID" );

  const std::vector<const IdDictRegion*>& vecOfDictRegions = dictRegions();

  if (!m_quiet) {
    if(m_msgSvc) {
      log << MSG::DEBUG << "init_neighbors" << endmsg;
    }
    else {
      std::cout << " LArHEC_Base_ID::init_neighbors " << std::endl;
    }
  }

  //
  // ..... loop on HEC regions -> store vector of LArHEC_region*
  //
  short int reg=0;
  std::vector<Identifier>::const_iterator debut=reg_begin() ;
  std::vector<Identifier>::const_iterator fin  =reg_end() ;

  for (; debut != fin; ++debut) 
    {
      const Identifier& regId = (*debut);
      bool fullSym = (dictionaryVersion() == "fullAtlas" );
      //
      // ..... translate regId to chanId and get hash
      //
      Identifier id ;
      unsigned int index0 = NOT_VALID_HASH;
      int etaMin = eta_min(regId);
      int phiMin = phi_min(regId);
      if(etaMin >= 0 && phiMin >= 0) 
	{
	  try{
	    id = channel_id (regId, etaMin, phiMin );
	  }
	  catch(LArID_Exception & except){
	  if(m_msgSvc) {
	    log << MSG::ERROR << " LArId exception " 
		<< (std::string)except
		<< endmsg;
	  }
	  else {
	    std::cout 
	      <<  " LArId exception " 
	      << (std::string)except
	      << std::endl;
	  }
	  }
	  IdentifierHash 	hashId = channel_hash(id) ;
	  index0=hashId;
	}
      else 
	{
	  if(m_msgSvc) {
	    log << MSG::WARNING << " could not find non negative etaMin and phiMin for region " 
		<< show_to_string(regId)
		<< endmsg;
	  }
	  else {
	    std::cout << "WARNING !! could not find non negative etaMin and phiMin for region " 
		      << show_to_string(regId)
		      << std::endl;
	  }
	  index0 = 0;
	}


      short int deltaEta = eta_max(regId) - eta_min(regId) + 1 ;
      short int nPhi = phi_max(regId) - phi_min(regId) + 1 ;
      // starting eta 
      float eta0 = this->eta0(reg);
      // eta granularity
      float deta = this->etaGranularity(reg);
      // starting phi 
      float phi0 = this->phi0(reg);
      // phi granularity
      float dphi = this->phiGranularity(reg);

      // full range of regions
      unsigned int ireg0=0;
      unsigned int ireg1=region_hash_max();
      //  if 2 symetric sides, in which side is the current reg.
      if(twoSymSides()) {
	if(reg < (short int)region_hash_max()/2) {
	  ireg0=0;
	  ireg1=region_hash_max()/2;
	} else {
	  ireg0=region_hash_max()/2;
	  ireg1=region_hash_max();
	}
      }

      //
      // .... compute prev/next regions in eta 
      //
      short int regForPrevEta=NOT_VALID_HEC_REGION;
      IdDictRegion* prevEtaDicReg =  vecOfDictRegions[reg]->m_prev_abs_eta;
      short int regForNextEta=NOT_VALID_HEC_REGION;
      IdDictRegion* nextEtaDicReg =  vecOfDictRegions[reg]->m_next_abs_eta;
      for(unsigned int ireg=ireg0;ireg<ireg1;ireg++){
	if(vecOfDictRegions[ireg] == prevEtaDicReg) regForPrevEta = ireg;
	if(vecOfDictRegions[ireg] == nextEtaDicReg) regForNextEta = ireg;
      }

      //
      // .... compute prev/next regions in sampling 
      //                                            
      //
      std::vector<short int> regForPrevSamp;
      std::vector<IdDictRegion*> prevSampDicReg =  vecOfDictRegions[reg]->m_prev_samp;
      for(unsigned int isam=0;isam<prevSampDicReg.size();isam++){
	for(unsigned int ireg=ireg0;ireg<ireg1;ireg++){
	  if(vecOfDictRegions[ireg] == prevSampDicReg[isam]) regForPrevSamp.push_back(ireg);
	}
      }

      std::vector<short int> regForNextSamp;
      std::vector<IdDictRegion*> nextSampDicReg =  vecOfDictRegions[reg]->m_next_samp;
      for(unsigned int isam=0;isam<nextSampDicReg.size();isam++){
	for(unsigned int ireg=ireg0;ireg<ireg1;ireg++){
	  if(vecOfDictRegions[ireg] == nextSampDicReg[isam]) regForNextSamp.push_back(ireg);
	}
      }

      //
      // ....now ready to build region object
      //

      LArHEC_region * hecRegion = new LArHEC_region(index0,deltaEta,nPhi,eta0,deta,phi0,dphi,fullSym,
						    regForPrevEta,regForNextEta,
						    regForPrevSamp,regForNextSamp);
      // save in a vector for further use in get_neighbours method
      m_vecOfRegions.push_back(hecRegion);
      reg++;
  } // end of loop on regions

  //
  // ..... loop on channels 
  //
  reg=0;
  debut = hec_begin();
  fin   = hec_end();
  if( debut != fin ){
  Identifier lastRegId=region_id(*debut);
  for (; debut != fin; ++debut) {
      const Identifier& chanId = (*debut);
      const Identifier& regId = region_id(chanId);
      if(regId != lastRegId) reg++;

      if(m_do_checks) {
	// for cross check only
	IdentifierHash hashReg = region_hash(regId);
	if ((short int)hashReg != reg) {
	  if(m_msgSvc) {
	    log << MSG::ERROR << " init_neighbors: problem reg, hashReg = " << reg << " " << hashReg 
		<< endmsg;
	  }
	  else {
	    std::cout << " init_neighbors: problem reg, hashReg = " << reg << " " << hashReg 
		      << std::endl;
	  }
	}
      }

      // save region of the cell in a vector for further use in get_neighbours method
      m_vecOfCellInfo.push_back(reg);

      lastRegId=regId;
  }
  }
  return (0);
}


