/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/src/LArEM_Base_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Factor out code common between LArEM_ID and LArEM_SuperCell_ID.
 */

#include "CaloIdentifier/LArEM_Base_ID.h"
#include "LArEM_region.h"
#include "IdDict/IdDictDefs.h"
#include "CxxUtils/StrFormat.h"

using CxxUtils::strformat;


LArEM_Base_ID::LArEM_Base_ID (const std::string& name, bool supercell)
  : CaloIDHelper (name),
    m_slar (supercell ? 1 : 0)
{
  m_REGION_INDEX = 999;
  m_ETA_INDEX = 999;
  m_PHI_INDEX = 999;
  m_SLAR_INDEX = 999;
  m_LAR_INDEX = 999;
  m_EM_INDEX = 999;
  m_BEC_INDEX = 999;
  m_SAMPLING_INDEX = 999;
  m_em_region_index = 0;
  m_two_sym_sides = 1;
}


LArEM_Base_ID::~LArEM_Base_ID(void) 
{
  std::vector<LArEM_region*>::iterator first = m_vecOfRegions.begin();
  std::vector<LArEM_region*>::iterator last  = m_vecOfRegions.end();
  for (; first != last; ++first) delete (*first);
}

bool LArEM_Base_ID::is_supercell (const Identifier id)const
{
  return(m_slar_impl.unpack(id)) != 0;
}


int LArEM_Base_ID::eta_min(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext context = region_context();
  if(!get_expanded_id(regId, expId, &context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_em_range.size(); ++i) {
      const Range& range = m_full_em_range[i];
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

int LArEM_Base_ID::eta_max(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext context = region_context();
  if(!get_expanded_id(regId, expId, &context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_em_range.size(); ++i) {
      const Range& range = m_full_em_range[i];
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

int LArEM_Base_ID::phi_min_init(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext region_cntxt = region_context();
  if(!get_expanded_id(regId, expId, &region_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_em_range.size(); ++i) {
      const Range& range = m_full_em_range[i];
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

int LArEM_Base_ID::phi_max(const Identifier regId) const
{
  ExpandedIdentifier expId;
  IdContext context = region_context();
  if(!get_expanded_id(regId, expId, &context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_em_range.size(); ++i) {
      const Range& range = m_full_em_range[i];
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

void LArEM_Base_ID::region_id_checks   ( int barrel_ec, int sampling, int region ) const
{
  // Check that id is within allowed range

  // Fill expanded id
  ExpandedIdentifier expId(lar_em_exp());
  expId << barrel_ec << sampling << region ;

  if (!m_full_region_range.match(expId)) { 
    std::string errorMessage = "LArEM_Base_ID::region_id_checks() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_region_range;
    throw LArID_Exception(errorMessage , 5);
  }
}


void LArEM_Base_ID::channel_id_checks   ( int barrel_ec, int sampling, int region,
				     int eta,       int phi ) const
{  
    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier expId(lar_em_exp());
    expId << barrel_ec << sampling << region << eta << phi << m_slar ;

    if (!m_full_em_range.match(expId)) { 
	std::string errorMessage = "LArEM_Base_ID::channel_id_checks() result is not OK: ID, range = "
	    + std::string(expId) + " , " + (std::string)m_full_em_range;
	throw LArID_Exception(errorMessage , 6);
    }
}

void LArEM_Base_ID::channel_id_checks   ( const Identifier regionId,
                                          int eta,       int phi ) const
{
  // Check that id is within allowed range

  // Fill expanded id
    ExpandedIdentifier expId; 

    IdContext context = region_context();
    if (get_expanded_id(regionId, expId, &context)) {
	std::string errorMessage = "LArEM_Base_ID::channel_id_checks(regId) result is not OK: ID = "
	    + show_to_string(regionId) ;
	throw LArID_Exception(errorMessage , 6);
    }

    expId << eta << phi << m_slar;

    if (!m_full_em_range.match(expId)) { 
	std::string errorMessage = "LArEM_Base_ID::channel_id_checks(regId) result is not OK: ID, range = "
	    + std::string(expId) + " , " + (std::string)m_full_em_range;
	throw LArID_Exception(errorMessage , 6);
    }
}




int  LArEM_Base_ID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
    // We assume that the context is >= region
    exp_id.clear();
    exp_id << lar_field_value()
      	   << lar_em_field_value()
	   << barrel_ec(id)
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

int LArEM_Base_ID::initLevelsFromDict (const std::string& group_name)
{
  MsgStream log(m_msgSvc, "LArEM_Base_ID" );
  if(!dict()) {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - dictionary NOT initialized "
	  << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - dictionary NOT initialized "
		<< std::endl ;
    }
    return (1);
  }

  // Find out which identifier field corresponds to each level.

  m_LAR_INDEX        = 999 ;
  m_EM_INDEX         = 999 ;
  m_BEC_INDEX        = 999 ;
  m_SAMPLING_INDEX   = 999 ;
  m_REGION_INDEX     = 999 ;
  m_ETA_INDEX        = 999 ;
  m_PHI_INDEX        = 999 ;
  m_SLAR_INDEX       = 999 ;

  // Find a EM region
  IdDictField* field = dict()->find_field("subdet") ;
  if (field) {
    m_LAR_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'subdet' field "
	  << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'subdet' field "
		<< std::endl ;
    }
    return (1);
  }

  field = dict()->find_field("part") ;
  if (field) {
    m_EM_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'part' field "
	  << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'part' field "
		<< std::endl ;
    }
    return (1);
  }

  field = dict()->find_field("barrel-endcap") ;
  if (field) {
    m_BEC_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'barrel-endcap' field "
	  << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'barrel-endcap' field "
		<< std::endl ;
    }
    return (1);
  }

  field = dict()->find_field("sampling") ;
  if (field) {
    m_SAMPLING_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'sampling' field "
	  << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'sampling' field "
		<< std::endl ;
    }
    return (1);
  }

  field = dict()->find_field("region") ;
  if (field) {
    m_REGION_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'region' field "
	  << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'region' field "
		<< std::endl ;
    }
    return (1);
  }

  field = dict()->find_field("eta") ;
  if (field) {
    m_ETA_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'eta' field "
	  << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'eta' field "
		<< std::endl ;
    }
    return (1);
  }

  field = dict()->find_field("phi") ;
  if (field) {
    m_PHI_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'phi' field "
	  << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'phi' field "
		<< std::endl ;
    }
    return (1);
  }

  field = dict()->find_field("is-slar") ;
  if (field) {
    m_SLAR_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'is-slar' field "
          << endmsg;
    }
    else {
      std::cout << "LArEM_Base_ID::initLevelsFromDict - unable to find 'is-slar' field "
                << std::endl ;
    }
    return (1);
  }

  // Set the field implementations

  IdDictGroup* group = dict()->find_group(group_name);
  if ( !group ){
    log << MSG::ERROR << "initLevelsFromDict - cannot find " << group_name 
        << " group' field " << endmsg;
  }
  else {
	m_em_region_index = group->regions()[0]->m_index;
  }
  const IdDictRegion& region = *dict()->m_regions[m_em_region_index];

  m_lar_impl      = region.m_implementation[m_LAR_INDEX]; 
  m_em_impl       = region.m_implementation[m_EM_INDEX]; 
  m_bec_impl      = region.m_implementation[m_BEC_INDEX]; 
  m_sampling_impl = region.m_implementation[m_SAMPLING_INDEX]; 
  m_region_impl   = region.m_implementation[m_REGION_INDEX]; 
  m_eta_impl      = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl      = region.m_implementation[m_PHI_INDEX]; 
  m_slar_impl     = region.m_implementation[m_SLAR_INDEX]; 

  if (!m_quiet) {
    if(m_msgSvc) {
      log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
      log << MSG::DEBUG << "lar  "  << m_lar_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "em   "   << m_em_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "bec  "  << m_bec_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "samp " << m_sampling_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "reg  "  << m_region_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "eta  "  << m_eta_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "phi  "  << m_phi_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "is-slar  "  << m_slar_impl.show_to_string() << endmsg;
    }
    else {
      std::cout << "decode index and bit fields for each level: " << std::endl;
      std::cout << "lar  "  << m_lar_impl.show_to_string() << std::endl;
      std::cout << "em   "   << m_em_impl.show_to_string() << std::endl;
      std::cout << "bec  "  << m_bec_impl.show_to_string() << std::endl;
      std::cout << "samp " << m_sampling_impl.show_to_string() << std::endl;
      std::cout << "reg  "  << m_region_impl.show_to_string() << std::endl;
      std::cout << "eta  "  << m_eta_impl.show_to_string() << std::endl;
      std::cout << "phi  "  << m_phi_impl.show_to_string() << std::endl;
      std::cout << "is-slar  "  << m_slar_impl.show_to_string() << std::endl;
    }
  }
  

  return(0) ;
}


int         LArEM_Base_ID::init_hashes(void) 
{
  if (channels().init (*this, "channels",
                       m_full_em_range, &LArEM_Base_ID::channel_id,
                       m_SLAR_INDEX))
    return 1;
  if (regions().init (*this, "regions",
                      m_full_region_range, &LArEM_Base_ID::region_id,
                      m_REGION_INDEX))
    return 1;

  return (0);
}


int
LArEM_Base_ID::initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
                                                const std::string& group_name)
/*=================================================================*/
{
  MsgStream log(m_msgSvc, "LArEM_Base_ID" );

  log << MSG::DEBUG << "initialize_base_from_dictionary" << endmsg;

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
  if (initLevelsFromDict(group_name)) return (1);

  // Find value for the field LAr Calorimeter   
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
    int larField   = -1;
    if (atlasDict->get_label_value("subdet", "LArCalorimeter", larField)) {
      if(m_msgSvc) {
	log << MSG::ERROR << "Could not get value for label 'LArCalorimeter' of field 'subdet' in dictionary " 
	    << atlasDict->m_name
	    << endmsg;
      }
      else {
        std::cout << "Could not get value for label 'LArCalorimeter' of field 'subdet' in dictionary " 
                  << atlasDict->m_name
                  << std::endl;
      }
      return (1);
    }

  // Find value for the field LArEM 
    int larEmField   = -1;
    if (dict()->get_label_value("part", "LArEM", larEmField)) {
      if(m_msgSvc) {
	log << MSG::ERROR << "Could not get value for label 'LArEM' of field 'part' in dictionary " 
	    << dict()->m_name
	    << endmsg;
      }
      else {
        std::cout << "Could not get value for label 'LArEM' of field 'part' in dictionary " 
                  << dict()->m_name
                  << std::endl;
      }
      return (1);
    }

    // Set up id for region and range prefix
    ExpandedIdentifier reg_id;
    reg_id.add(larField);
    reg_id.add(larEmField);
    Range prefix;	

    m_full_em_range = dict()->build_multirange(reg_id, group_name, prefix);
    m_full_region_range = dict()->build_multirange(reg_id, group_name, prefix, "region");  

    if (!m_quiet) {
      if(m_msgSvc) {
        log << MSG::DEBUG << " initialize_from_dict : " 
            << endmsg;
        log << MSG::DEBUG << " region range -> "  << (std::string)m_full_region_range
            << endmsg;
        log << MSG::DEBUG << " channel range -> "  << (std::string)m_full_em_range
            << endmsg;
      }
      else {
        std::cout << " LArEM_Base_ID::initialize_from_dict : " 
                  << std::endl;
        std::cout << " region range -> "  << (std::string)m_full_region_range
                  << std::endl;
        std::cout << " channel range -> "  << (std::string)m_full_em_range
                  << std::endl;
      }
    }
   
    // initialize m_two_sym_sides
    m_two_sym_sides = ( dictionaryVersion() == "fullAtlas" );


    // Setup the hash tables
    if(init_hashes()) return (1);

    // initialize dictionary regions
    if (fill_vec_of_dict_regions (group_name)) return 1;

    // added nov 2005 to speed up CaloNoiseTool initialisation
    m_vecOfPhiMin.resize(regions().hash_max());
    for (unsigned int i = 0; i < regions().hash_max(); ++i) {
      Identifier regId = region_id(i); 
      m_vecOfPhiMin[i] = phi_min_init(regId);
    }


    // Setup for hash calculation

    // We use the structure of the region definitions in the
    // dictionary to optimize. Specifically, each dict region
    // corresponds to two regions of uniform eta/phi granularity (+/-
    // in barrel/endcap). The lookup table only needs to contain the
    // hash offset for each region and the number of phi cells.

    // The implementation requires:

    //   1) a lookup table for each region containing hash offset and
    //      nphi
    //   2) a decoder to access the "index" corresponding to the
    //      bec/samp/reg fields. These fields use 8 bits, so the
    //      vector has a length of 256 for only 36 regions.


    // Create decoder for fields bec to region
    IdDictFieldImplementation::size_type bits = 
	m_bec_impl.bits() +
	m_sampling_impl.bits() +
	m_region_impl.bits();
    IdDictFieldImplementation::size_type bits_offset = m_bec_impl.bits_offset();
    m_bec_reg_impl.set_bits(bits, bits_offset);

    //    std::cout << "bec_reg "  << m_bec_reg_impl.decode_index()  << " " 
    //      << (std::string)m_bec_reg_impl.ored_field()  << " " 
    //      << std::hex << m_bec_reg_impl.mask() << " " 
    //      << m_bec_reg_impl.zeroing_mask() << " " 
    //      << std::dec << m_bec_reg_impl.shift()
    //      << " " << m_bec_reg_impl.bits() << " " <<m_bec_reg_impl.bits_offset()
    //      << std::endl;


    // Set up vector as lookup table for hash calculation. 
    m_hash_calcs.resize(256);
    for (unsigned int i = 0; i < m_full_em_range.size(); ++i) {
	const Range* range = &m_full_em_range[i];
	ExpandedIdentifier exp_id = range->minimum (); 
	HashCalc hc;

	Identifier min = channel_id (exp_id[m_BEC_INDEX],
				     exp_id[m_SAMPLING_INDEX], 
				     exp_id[m_REGION_INDEX],
				     exp_id[m_ETA_INDEX],
				     exp_id[m_PHI_INDEX]);
	IdentifierHash min_neg = channel_hash_binary_search(min);
	hc.m_hash    = min_neg;
	Identifier minReg = region_id(min) ;
	hc.m_etamin  = eta_min(minReg);
	hc.m_phimin  = phi_min(minReg);
	hc.m_nphi    = (*range)[m_PHI_INDEX].get_indices();
	m_hash_calcs[m_bec_reg_impl.unpack(min)] = hc;

	// Flip sign for bec if 2 symmetric sides (ATLAS case, not TB)
	if(m_two_sym_sides) {
	  Identifier min1 = channel_id (-exp_id[m_BEC_INDEX],
					exp_id[m_SAMPLING_INDEX], 
					exp_id[m_REGION_INDEX],
					exp_id[m_ETA_INDEX],
					exp_id[m_PHI_INDEX]);
	  IdentifierHash min_pos = channel_hash_binary_search(min1);
	  hc.m_hash  = min_pos;
	  m_hash_calcs[m_bec_reg_impl.unpack(min1)] = hc;
	}


	if (m_bec_reg_impl.unpack(min) > 255) {
	  if(m_msgSvc) {
	    log << MSG::WARNING << "min > 255 " 
		<< i << " "
		<< show_to_string(min) << " " 
		<< m_bec_reg_impl.unpack(min) << " " 
		<< endmsg;
	  }
	  else {
	    std::cout << "min > 255 " 
		      << i << " "
		      << show_to_string(min) << " " 
		      << m_bec_reg_impl.unpack(min) << " " 
		      << std::endl;
	  }
	}
    }
    
//   Some detailed printout
//  	ExpandedIdentifier exp_id = range->maximum (); 
// 	Identifier max = channel_id (exp_id[m_BEC_INDEX],
//  				     exp_id[m_SAMPLING_INDEX], 
//  				     exp_id[m_REGION_INDEX],
//  				     exp_id[m_ETA_INDEX],
//  				     exp_id[m_PHI_INDEX]);
//  	Identifier max0 = channel_id (-exp_id[m_BEC_INDEX],
//  				      exp_id[m_SAMPLING_INDEX], 
//  				      exp_id[m_REGION_INDEX],
//  				      exp_id[m_ETA_INDEX],
//  				      exp_id[m_PHI_INDEX]);
//  	std::cout << "channel ranges, id, hash " 
//  		  << i << " "
//  		  << show_to_string(min) << " " 
//  		  << show_to_string(max0) << " " 
//  		  << show_to_string(min1) << " " 
//  		  << show_to_string(max) << " " 
//  		  << channel_hash_binary_search(min)  << " " 
//  		  << channel_hash_binary_search(min1)  << " " 
//  		  << min_pos  << " " 
//  		  << range->cardinality() << " " 
//  		  << std::endl;
//      }

    // Check hash calculation
    for (unsigned int i = 0; i < channels().hash_max(); ++i) {
      Identifier id = channel_id(i);
      if (channel_hash(id) != i) {
	if(m_msgSvc) {
	  log << MSG::ERROR << "channel ranges, id, hash " 
	      << show_to_string(id) << " " 
	      << channel_hash(id) << " " 
	      << i
	      << endmsg;
	}
	else {
	  std::cout << " *****  Error ";
	  std::cout << "channel ranges, id, hash " 
		    << show_to_string(id) << " " 
		    << channel_hash(id) << " " 
		    << i
		    << std::endl;
	}
      }
    }
    
    // Setup hash tables for finding neighbors (at the end of initialisation, 
    // to benefit from optimization
    if(m_do_neighbours) {
      if(init_neighbors()) return (1);     
    }

    return 0;
}


int   LArEM_Base_ID::get_neighbours(const IdentifierHash id, const LArNeighbours::neighbourOption& option, 
			       std::vector<IdentifierHash>& neighbourList) const
{
  int result = 1; 

  neighbourList.clear();
  
  if(!m_do_neighbours) {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArEM_Base_ID" );
      log << MSG::WARNING << "neighbours not initialized !!! returning empty list" << endmsg;
    }
    else {
      std::cout << " WARNING: neighbours not initialized !!! returning empty list " << std::endl;
    }
    return result;
  }

  if(id >= channel_hash_max()) {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArEM_Base_ID" );
      log << MSG::WARNING << "neighbours requested for non-existing channel -- id/max " << id << "/"
          << channel_hash_max() << endmsg;
    }
    else {
      std::cout << " neighbours requested for non-existing channel -- id/max " << id << "/"
                << channel_hash_max() << std::endl;
    }
    return result;
  }

  const short int maxNeighb=22;
  IdentifierHash neighbList[maxNeighb];
  int neighbourIndex = 0;

  // cell index
  unsigned int index=id;

  //
  // .... find in which region is the cell   
  //
  auto itr = std::upper_bound(m_cells.begin(), m_cells.end(), index);
  unsigned short int regionN = (itr - m_cells.begin()) - 1;
  assert (regionN < m_vecOfRegions.size());
  // get pointer to this region
  LArEM_region* emRegion = m_vecOfRegions[regionN];
  // retrieve characteristic numbers for this region
  short int nPhi = emRegion->phiN();
  float gPhi = emRegion->phiGranularity();
  unsigned int minHash = emRegion->hashMin();
  unsigned int maxHash = emRegion->hashMax();

  bool corners2DOnly  = ( (option & LArNeighbours::all2D) 
			  == LArNeighbours::corners2D );
  //
  // .... previous neighbour in phi
  //
  IdentifierHash prevNeighbInPhi=NOT_VALID_HASH;
  if( (option & LArNeighbours::prevInPhi) 
      || corners2DOnly ){
    if(!get_prevInPhi(emRegion, index, nPhi, minHash, neighbourIndex, neighbList)){
      // always 1 and only 1 neighbour in phi in ATLAS, but 0 or 1 neighbour in phi in TB
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
      ||  corners2DOnly ){
    if(!get_nextInPhi(emRegion, index, nPhi, minHash, neighbourIndex, neighbList)){
      // always 1 and only 1 neighbour in phi in ATLAS, but 0 or 1 neighbour in phi in TB
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
    get_prevInEta(emRegion, index, nPhi, gPhi, minHash, neighbourIndex, neighbList, nPrevBiggerCell);
  }

  //
  // ....next neighbours in eta
  //
  unsigned int nNextBiggerCell=NOT_VALID_HASH;
  if( (option & LArNeighbours::nextInEta) ){
    get_nextInEta(emRegion, index, nPhi, gPhi, maxHash, neighbourIndex, neighbList, nNextBiggerCell);
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
      get_prevInEta(emRegion, index1, nPhi, gPhi, minHash, neighbourIndex, neighbList, nPrevBiggerCell);
      if ( neighbourIndex > oldNeighbourIndex+1 ) {
	neighbList[oldNeighbourIndex] = neighbList[neighbourIndex-1];
	neighbourIndex = oldNeighbourIndex+1;
      }
      oldNeighbourIndex = neighbourIndex;
      get_nextInEta(emRegion, index1, nPhi, gPhi, maxHash, neighbourIndex, neighbList, nNextBiggerCell);
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
      get_prevInEta(emRegion, index2, nPhi, gPhi, minHash, neighbourIndex, neighbList, nPrevBiggerCell);
      if ( neighbourIndex > oldNeighbourIndex+1 ) {
	neighbourIndex = oldNeighbourIndex+1;
      }
      oldNeighbourIndex = neighbourIndex;
      get_nextInEta(emRegion, index2, nPhi, gPhi, maxHash, neighbourIndex, neighbList, nNextBiggerCell);
      if ( neighbourIndex > oldNeighbourIndex+1 ) {
	neighbourIndex = oldNeighbourIndex+1;
      }
    }
  }

  //
  // .... neighbours in sampling (common code)
  // EM caracteristics = granularity changes in both eta and phi + partial overlap of samplings
  //
  if( (option & LArNeighbours::upAndDown) ) {
    // granularity of the initial region
    double gEta = (double)(emRegion->etaGranularity());
    // initial eta
    int nEta =  int( (index-minHash) / nPhi);
    double absEta = (double)(emRegion->etaMin()) + (double)(nEta * gEta);

    // previous neighbours in sampling
    if( (option & LArNeighbours::prevInSamp) ){
      get_prevInSamp(emRegion, index, nPhi, minHash, gEta, gPhi, absEta, neighbourIndex, neighbList);
    }

    // next neighbours in sampling
    if( (option & LArNeighbours::nextInSamp) ){
      get_nextInSamp(emRegion, index, nPhi, minHash, gEta, gPhi, absEta, neighbourIndex, neighbList);
    }
  }

  //
  // .... neighbours across subdet
  //
  if( (option & (LArNeighbours::prevSubDet | LArNeighbours::nextSubDet)) ){
    // granularity of the initial region
    double gEta = (double)(emRegion->etaGranularity());
    // initial eta
    int nEta =  int( (index-minHash) / nPhi);
    double absEta = (double)(emRegion->etaMin()) + (double)(nEta * gEta);

    // prev neighbours in subdet
    if( (option & LArNeighbours::prevSubDet) ){ 
      get_prevInSubdet(emRegion, index, nPhi, minHash, gEta, gPhi, absEta, neighbourIndex, neighbList);
    }
    // next neighbours in subdet
    if( (option & LArNeighbours::nextSubDet) ){
      get_nextInSubdet(emRegion, index, nPhi, minHash, gEta, gPhi, absEta, neighbourIndex, neighbList);
    }
  }


  neighbourList.resize(neighbourIndex);
  if (neighbourIndex > 0) {
    if (neighbourIndex <= maxNeighb) {
      std::copy (&neighbList[0], &neighbList[neighbourIndex],  neighbourList.begin());
      result = 0 ;
    } else {
      if(m_msgSvc) {
        MsgStream log(m_msgSvc, "LArEM_Base_ID" );
	log << MSG::WARNING << " more than 22 neighbours for this cell, NONE will be retained" << endmsg;
      }
      else {
	std::cout << "WARNING: more than 22 neighbours for this cell, NONE will be retained " << neighbourIndex << std::endl;
      }
    } 
  }

  return result;
}


int   LArEM_Base_ID::get_prevInPhi(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
			       int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 
  unsigned int nIndex = index-1;
  if(!emRegion->isPhiMin(index)) {
    if( ((index-minHash)%(nPhi)) == 0 ) nIndex=index+nPhi-1;
    IdentifierHash nHash = nIndex;
    neighbList[neighbourIndex] = nHash;
    neighbourIndex++;
    result = 0;
  }
  return result;
}

int   LArEM_Base_ID::get_nextInPhi(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi,  const unsigned int& minHash, 
			       int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 
  if(!emRegion->isPhiMax(index)) {
    unsigned int nIndex = index+1;
    if( ((index-minHash+1)%(nPhi)) == 0 ) nIndex=index-nPhi+1;
    IdentifierHash nHash = nIndex;
    neighbList[neighbourIndex] = nHash;
    neighbourIndex++;
    result = 0;
  }
  return result;
}

int   LArEM_Base_ID::get_prevInEta(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const float& gPhi, 
			       const unsigned int& minHash, 
			       int& neighbourIndex, IdentifierHash* neighbList, unsigned int& nBiggerCell) const
{
  int result = 1; 

  unsigned int nIndex = 0;
  IdentifierHash nHash = 0;

  if( emRegion->isEtaMin(index)){
    // eta == etaMin -> go to previous region in eta
    short int nPrevEtaRegion = emRegion->prevEtaRegion();
    // no neighbour if no previous region in eta
    if( nPrevEtaRegion != NOT_VALID_REGION ) {
      LArEM_region* prevEmRegion = m_vecOfRegions[nPrevEtaRegion];
      if(emRegion->isFirstBarrelRegion()) {
	// special case: barrel middle
	unsigned int minHashMinus = prevEmRegion->hashMin();
	nIndex = minHashMinus + index-minHash ;
	nHash = nIndex;
	neighbList[neighbourIndex] = nHash;
	neighbourIndex++;
	result = 0;
      }
      else {
	short int nPhiMinus = prevEmRegion->phiN();
	float gPhiMinus= prevEmRegion->phiGranularity();
	unsigned int maxHashMinus = prevEmRegion->hashMax();
	float phiMargin = 0.25*std::min(gPhi,gPhiMinus);
	float rPhi = (index-minHash)*gPhi+emRegion->phiMin();
	int nPhiMinusFirst = int(floor((rPhi     -prevEmRegion->phiMin())
				       /gPhiMinus+phiMargin))
	  +maxHashMinus-nPhiMinus;
	int nPhiMinusNext  = int(floor((rPhi+gPhi-prevEmRegion->phiMin())
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

int   LArEM_Base_ID::get_nextInEta(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const float& gPhi, 
			       const unsigned int& maxHash, 
			       int& neighbourIndex, IdentifierHash* neighbList, unsigned int& nBiggerCell) const
{
  int result = 1; 

  unsigned int nIndex = 0;
  IdentifierHash nHash = 0;

  if( emRegion->isEtaMax(index)){
    // eta == etaMax -> go to next region in eta
    short int nNextEtaRegion = emRegion->nextEtaRegion();
    // no neighbour if no next region in eta
    if( nNextEtaRegion != NOT_VALID_REGION ) {
      LArEM_region* nextEmRegion = m_vecOfRegions[nNextEtaRegion];
      float gPhiPlus= nextEmRegion->phiGranularity();
      unsigned int minHashPlus = nextEmRegion->hashMin();
      float phiMargin = 0.25*std::min(gPhi,gPhiPlus);
      float rPhi = (index+nPhi-maxHash)*gPhi+emRegion->phiMin();
      int nPhiPlusFirst = int(floor((rPhi     -nextEmRegion->phiMin())
				    /gPhiPlus+phiMargin))+minHashPlus;
      int nPhiPlusNext  = int(floor((rPhi+gPhi-nextEmRegion->phiMin())
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

int   LArEM_Base_ID::get_prevInSamp(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
				const double& gEta, const float& gPhi, const double& absEta, 
				int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 

  // neighbours' indices
  unsigned int nIndex=0;
  // neighbours' hash
  IdentifierHash nHash=0;

  // previous region in sampling
  const std::vector<short int>& prevSampRegion= emRegion->prevSamplingRegion();
  int nPrevSampReg = prevSampRegion.size();
  if(nPrevSampReg > 0) {
    for(int ireg=0; ireg<nPrevSampReg; ireg++) {
      LArEM_region* prevEmRegion = m_vecOfRegions[prevSampRegion[ireg]];
      // eta granularity of previous region in sampling
      double gEtaMinus= (double)(prevEmRegion->etaGranularity());
      // starting eta of prev region
      double minEtaMinus =  (double)(prevEmRegion->etaMin());
      double maxEtaMinus =  (double)(prevEmRegion->etaMax());
      double margin = 0.25*std::min(gEta,gEtaMinus);
      if((minEtaMinus < absEta+gEta-margin) && (absEta+margin < maxEtaMinus)) {
	
	// phi granularity of previous region in sampling
	float gPhiMinus= prevEmRegion->phiGranularity();
	// max phi of previous region in sampling
	short int nPhiMinus = prevEmRegion->phiN();
	// first hash of previous region in sampling
	unsigned int minHashMinus = prevEmRegion->hashMin(); 
	float phiMargin = 0.25*std::min(gPhi,gPhiMinus);
	// phi 'coordinate' in initial region
	float rPhi = ((index-minHash)%nPhi)*gPhi+emRegion->phiMin();
	int nPhiMinusFirst = int(floor((rPhi     -prevEmRegion->phiMin())
				       /gPhiMinus+phiMargin));
	int nPhiMinusNext  = int(floor((rPhi+gPhi-prevEmRegion->phiMin())
				       /gPhiMinus+phiMargin));
	if ( nPhiMinusNext == nPhiMinusFirst ) nPhiMinusNext++;
		
	// eta 'coordinate' in initial region
	double fEtaMinus = (absEta-minEtaMinus) / gEtaMinus + margin ;
	// eta 'coordinate' in initial region + 1
	double fEtaMaxMinus = (absEta+gEta-minEtaMinus) / gEtaMinus + margin ;
	int nEtaMinus = int(fEtaMinus);
	int nEtaMaxMinus = int(fEtaMaxMinus);
	if ( nEtaMaxMinus == nEtaMinus ) nEtaMaxMinus++;
	for(int i=nEtaMinus; i<nEtaMaxMinus; i++) {
	  for (int j=nPhiMinusFirst; j<nPhiMinusNext;j++) {
	    nIndex = minHashMinus + i * nPhiMinus + j;
	    if( (nIndex >= prevEmRegion->hashMin()) && (nIndex < prevEmRegion->hashMax()) ) {
	      nHash = nIndex;
	      neighbList[neighbourIndex] = nHash;
	      neighbourIndex++;
	      result = 0;
	    }
	  }
	} 
      } // end eta condition
    } // loop on ireg
  }
  return result;
}

int   LArEM_Base_ID::get_nextInSamp(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
				const double& gEta, const float& gPhi, const double& absEta,
				int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 

  // neighbours' indices
  unsigned int nIndex=0;
  // neighbours' hash
  IdentifierHash nHash=0;

  const std::vector<short int>& nextSampRegion= emRegion->nextSamplingRegion();
  int nNextSampReg = nextSampRegion.size();
  if(nNextSampReg > 0) {
    for(int ireg=0; ireg<nNextSampReg; ireg++) {
      LArEM_region* nextEmRegion = m_vecOfRegions[nextSampRegion[ireg]];
      double gEtaPlus = (double)(nextEmRegion->etaGranularity());
      float gPhiPlus= nextEmRegion->phiGranularity();
      double minEtaPlus = (double)(nextEmRegion->etaMin());
      double maxEtaPlus = (double)(nextEmRegion->etaMax());
      double margin = 0.25*std::min(gEta,gEtaPlus);
      if((minEtaPlus < absEta+gEta-margin) && (absEta+margin < maxEtaPlus)) {
	
	short int nPhiPlus = nextEmRegion->phiN();
	unsigned int minHashPlus = nextEmRegion->hashMin(); 
	float phiMargin = 0.25*std::min(gPhi,gPhiPlus);
	// phi 'coordinate' in initial region
	float rPhi = ((index-minHash)%nPhi)*gPhi+emRegion->phiMin();
	int nPhiPlusFirst = int(floor((rPhi     -nextEmRegion->phiMin())
				      /gPhiPlus+phiMargin));
	int nPhiPlusNext  = int(floor((rPhi+gPhi-nextEmRegion->phiMin())
				      /gPhiPlus+phiMargin));
	if ( nPhiPlusNext == nPhiPlusFirst ) nPhiPlusNext++;

	double fEtaPlus = (absEta-minEtaPlus) / gEtaPlus + margin ;
	// eta 'coordinate' in initial region + 1
	double fEtaMaxPlus = (absEta+gEta-minEtaPlus) / gEtaPlus + margin ;
	int nEtaPlus = int(fEtaPlus) ;
	int nEtaMaxPlus = int(fEtaMaxPlus) ;
	if (nEtaMaxPlus == nEtaPlus) nEtaMaxPlus++;

	for(int i=nEtaPlus; i<nEtaMaxPlus; i++){
	  for(int j=nPhiPlusFirst; j<nPhiPlusNext; j++){
	    nIndex = minHashPlus + i * nPhiPlus + j;
	    if( (nIndex >= nextEmRegion->hashMin()) && (nIndex < nextEmRegion->hashMax()) ) {
	      nHash = nIndex;
	      neighbList[neighbourIndex] = nHash;
	      neighbourIndex++;
	      result = 0;
	    }
	  }
	}
      } // end eta condition
    } // end loop on regions
  }
  return result;
}


int   LArEM_Base_ID::get_prevInSubdet(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
				const double& gEta, const float& gPhi, const double& absEta, 
				int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 

  // neighbours' indices
  unsigned int nIndex=0;
  // neighbours' hash
  IdentifierHash nHash=0;

  // previous region in sampling
  const std::vector<short int>& prevSubdetRegion= emRegion->prevSubdetRegion();
  int nPrevSubdetReg = prevSubdetRegion.size();
  if(nPrevSubdetReg > 0) {
    for(int ireg=0; ireg<nPrevSubdetReg; ireg++) {
      LArEM_region* prevEmRegion = m_vecOfRegions[prevSubdetRegion[ireg]];
      // eta granularity of previous region in sampling
      double gEtaMinus= (double)(prevEmRegion->etaGranularity());
      // starting eta of prev region
      double minEtaMinus =  (double)(prevEmRegion->etaMin());
      double maxEtaMinus =  (double)(prevEmRegion->etaMax());
      double margin = 0.25*std::min(gEta,gEtaMinus);
      if((minEtaMinus < absEta+gEta-margin) && (absEta+margin < maxEtaMinus)) {
	
	// phi granularity of previous region in sampling
	float gPhiMinus= prevEmRegion->phiGranularity();
	// max phi of previous region in sampling
	short int nPhiMinus = prevEmRegion->phiN();
	// first hash of previous region in sampling
	unsigned int minHashMinus = prevEmRegion->hashMin(); 
	float phiMargin = 0.25*std::min(gPhi,gPhiMinus);
	// phi 'coordinate' in initial region
	float rPhi = ((index-minHash)%nPhi)*gPhi+emRegion->phiMin();
	int nPhiMinusFirst = int(floor((rPhi     -prevEmRegion->phiMin())
				       /gPhiMinus+phiMargin));
	int nPhiMinusNext  = int(floor((rPhi+gPhi-prevEmRegion->phiMin())
				       /gPhiMinus+phiMargin));
	if ( nPhiMinusNext == nPhiMinusFirst ) nPhiMinusNext++;
	// eta 'coordinate' in initial region
	double fEtaMinus = (absEta-minEtaMinus) / gEtaMinus + margin ;
	// eta 'coordinate' in initial region + 1
	double fEtaMaxMinus = (absEta+gEta-minEtaMinus) / gEtaMinus + margin ;
	int nEtaMinus = int(fEtaMinus);
	int nEtaMaxMinus = int(fEtaMaxMinus);
	if ( nEtaMaxMinus == nEtaMinus ) nEtaMaxMinus++;

	for(int i=nEtaMinus; i<nEtaMaxMinus; i++) {
	  for (int j=nPhiMinusFirst; j<nPhiMinusNext;j++) {
	    nIndex = minHashMinus + i * nPhiMinus + j;
	    if( (nIndex >= prevEmRegion->hashMin()) && (nIndex < prevEmRegion->hashMax()) ) {
	      nHash = nIndex;
	      neighbList[neighbourIndex] = nHash;
	      neighbourIndex++;
	      result = 0;
	    }
	  }
	} 
      } // end eta condition
    } // loop on ireg
  }
  return result;
}

int   LArEM_Base_ID::get_nextInSubdet(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
				const double& gEta, const float& gPhi, const double& absEta,
				int& neighbourIndex, IdentifierHash* neighbList) const
{
  int result = 1; 

  // neighbours' indices
  unsigned int nIndex=0;
  // neighbours' hash
  IdentifierHash nHash=0;

  const std::vector<short int>& nextSubdetRegion= emRegion->nextSubdetRegion();
  int nNextSubdetReg = nextSubdetRegion.size();
  if(nNextSubdetReg > 0) {
    for(int ireg=0; ireg<nNextSubdetReg; ireg++) {
      LArEM_region* nextEmRegion = m_vecOfRegions[nextSubdetRegion[ireg]];
      double gEtaPlus = (double)(nextEmRegion->etaGranularity());
      float gPhiPlus= nextEmRegion->phiGranularity();
      double minEtaPlus = (double)(nextEmRegion->etaMin());
      double maxEtaPlus = (double)(nextEmRegion->etaMax());
      double margin = 0.25*std::min(gEta,gEtaPlus);
      if((minEtaPlus < absEta+gEta-margin) && (absEta+margin < maxEtaPlus)) {
	
	short int nPhiPlus = nextEmRegion->phiN();
	unsigned int minHashPlus = nextEmRegion->hashMin(); 
	float phiMargin = 0.25*std::min(gPhi,gPhiPlus);
	// phi 'coordinate' in initial region
	float rPhi = ((index-minHash)%nPhi)*gPhi+emRegion->phiMin();
	int nPhiPlusFirst = int(floor((rPhi     -nextEmRegion->phiMin())
				      /gPhiPlus+phiMargin));
	int nPhiPlusNext  = int(floor((rPhi+gPhi-nextEmRegion->phiMin())
				      /gPhiPlus+phiMargin));
	if ( nPhiPlusNext == nPhiPlusFirst ) nPhiPlusNext++;

	double fEtaPlus = (absEta-minEtaPlus) / gEtaPlus + margin ;
	// eta 'coordinate' in initial region + 1
	double fEtaMaxPlus = (absEta+gEta-minEtaPlus) / gEtaPlus + margin ;
	int nEtaPlus = int(fEtaPlus) ;
	int nEtaMaxPlus = int(fEtaMaxPlus) ;
	if (nEtaMaxPlus == nEtaPlus) nEtaMaxPlus++;

	for(int i=nEtaPlus; i<nEtaMaxPlus; i++) {
	  for(int j=nPhiPlusFirst; j<nPhiPlusNext; j++) {
	    nIndex = minHashPlus + i * nPhiPlus + j;
	    if( (nIndex >= nextEmRegion->hashMin()) && (nIndex < nextEmRegion->hashMax()) ) {
	      nHash = nIndex;
	      neighbList[neighbourIndex] = nHash;
	      neighbourIndex++;
	      result = 0;
	    }
	  }
	}
      } // end eta condition
    } // end loop on regions
  }
  return result;
}


int         LArEM_Base_ID::init_neighbors(void) 
{
  MsgStream log(m_msgSvc, "LArEM_Base_ID" );
  if(m_msgSvc) {
    log << MSG::DEBUG << "init_neighbors" << endmsg;
  }
  else {
    std::cout << " LArEM_Base_ID::init_neighbors " << std::endl;
  }

  const std::vector<const IdDictRegion*>& vecOfDictRegions = dictRegions();

  //
  // ..... loop on regions -> store vector of LArEM_region*
  //
  m_vecOfRegions.resize(region_hash_max());
  short int reg=0;
  std::vector<Identifier>::const_iterator debut=reg_begin() ;
  std::vector<Identifier>::const_iterator fin  =reg_end() ;
  for (; debut != fin; ++debut) {
      const Identifier& regId = (*debut);

      bool phiWrapAround = (dictionaryVersion() == "fullAtlas" );
      bool isBarrelMiddle = false;

      //
      // ..... translate regId to chanId and get hash
      //

      Identifier id ;
      unsigned int index0 = NOT_VALID_HASH;
      int etaMin = eta_min(regId);
      int phiMin = phi_min(regId);
      if(etaMin >= 0 && phiMin >= 0) {
	try{
	  id = channel_id (regId, eta_min(regId), phi_min(regId) );  
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
	IdentifierHash hashId = channel_hash (id) ;
	index0=hashId;
      }
      else {
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
      float deta = etaGranularity(reg);
      // starting phi 
      float phi0 = this->phi0(reg);
      // phi granularity
      float dphi =  phiGranularity(reg);

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
      short int regForPrevEta=NOT_VALID_REGION;
      IdDictRegion* prevEtaDicReg =  vecOfDictRegions[reg]->m_prev_abs_eta;
      short int regForNextEta=NOT_VALID_REGION;
      IdDictRegion* nextEtaDicReg =  vecOfDictRegions[reg]->m_next_abs_eta;
      for(unsigned int ireg=ireg0;ireg<ireg1;ireg++){
	if(vecOfDictRegions[ireg] == prevEtaDicReg) regForPrevEta = ireg;
	if(vecOfDictRegions[ireg] == nextEtaDicReg) regForNextEta = ireg;
      }
      // .... special case of barrel middle
      if(twoSymSides()) {
	if(eta0 < 0.1 ) {
	  isBarrelMiddle = true;
	  if(barrel_ec(regId) < 0) {
	    regForPrevEta = reg+6;
	  } else {
	    regForPrevEta = reg-6;
	  }
	}
      }

      //
      // .... compute prev/next regions in sampling  
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
      // .... compute prev/next regions in subdet  
      //
      std::vector<short int> regForPrevSubdet;
      std::vector<IdDictRegion*> prevSubdetDicReg =  vecOfDictRegions[reg]->m_prev_subdet;
      for(unsigned int isubdet=0;isubdet<prevSubdetDicReg.size();isubdet++){
	for(unsigned int ireg=ireg0;ireg<ireg1;ireg++){
	  if(vecOfDictRegions[ireg] == prevSubdetDicReg[isubdet]) regForPrevSubdet.push_back(ireg);
	}
      }

      std::vector<short int> regForNextSubdet;
      std::vector<IdDictRegion*> nextSubdetDicReg =  vecOfDictRegions[reg]->m_next_subdet;
      for(unsigned int isubdet=0;isubdet<nextSubdetDicReg.size();isubdet++){
	for(unsigned int ireg=ireg0;ireg<ireg1;ireg++){
	  if(vecOfDictRegions[ireg] == nextSubdetDicReg[isubdet]) regForNextSubdet.push_back(ireg);
	}
      }

      //
      // ....now ready to build region object
      //

      LArEM_region * emRegion = new LArEM_region(index0,deltaEta,nPhi,eta0,deta,phi0,dphi,phiWrapAround,isBarrelMiddle,
						 regForPrevEta,regForNextEta,
						 regForPrevSamp,regForNextSamp, 
						 regForPrevSubdet,regForNextSubdet);
      // save in a vector for further use in get_neighbours method
      // optimized nov 2005
      //      m_vecOfRegions.push_back(emRegion);
      m_vecOfRegions[reg]=emRegion;
      reg++;
  } // end of loop on regions

  //
  // ..... loop on channels 
  //
      // optimized nov 2005
  m_cells.reserve(64);
  unsigned int i=0;
  reg=0;
  Identifier lastRegId;
  for (Identifier chanId : em_range()) {
      const Identifier& regId = region_id(chanId);
      if(regId != lastRegId) {
	m_cells.push_back(i);
	reg++;
      }
      ++i;

      if(m_do_checks) {
	// for cross check only
	IdentifierHash hashReg = region_hash (regId) ;
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

      lastRegId=regId;
  }

  return (0);
}



