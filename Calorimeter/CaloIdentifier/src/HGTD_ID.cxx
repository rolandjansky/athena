/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/src/HGTD_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Factor out code common between LArEM_ID and LArEM_SuperCell_ID.
 */

#include "CaloIdentifier/HGTD_ID.h"
#include "LArEM_region.h"
#include "IdDict/IdDictDefs.h"
#include "CxxUtils/StrFormat.h"

using CxxUtils::strformat;


HGTD_ID::HGTD_ID ()
  : CaloIDHelper ("HGTD_ID")
    //   m_slar (supercell ? 1 : 0)
{
  m_HGTD_REGION_INDEX = 9999;
  m_LAR_INDEX         = 9999;
  m_HGTD_INDEX        = 9999;
  m_BEC_INDEX         = 9999;
  m_SAMPLING_INDEX    = 9999;
  m_GRANULARITY_INDEX = 9999;
  m_X_INDEX_INDEX     = 9999;
  m_Y_INDEX_INDEX     = 9999;

  m_two_sym_sides = 1;
}


HGTD_ID::~HGTD_ID(void) 
{
}

int HGTD_ID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
  // We assume that the context is >= region
  exp_id.clear();
  exp_id << lar_field_value()
    //      	   << lar_hgtd_field_value()
    	 << -2
    //	 << 7
	 << barrel_ec(id)
	 << sampling(id)
	 << granularity(id);
  if(context && context->end_index() >= m_X_INDEX_INDEX) {
    exp_id << x_index(id);
    if(context->end_index() >= m_Y_INDEX_INDEX) {
      exp_id << y_index(id);
      //    	    if ( context->end_index() >= m_SLAR_INDEX) {
      //  exp_id << (unsigned)is_supercell(id);
      // }
    }
  }
  return (0);
}

int HGTD_ID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id) const
{
    // We assume that the context is >= region
    exp_id.clear();
    exp_id << lar_field_value()
      //      	   << lar_hgtd_field_value()
      //	   << -2
	   << 7
	   << barrel_ec(id)
	   << sampling(id)
	   << granularity(id)
	   << x_index(id)
	   << y_index(id);

    return (0);
}

int  HGTD_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/*=================================================================*/
{
  MsgStream log(m_msgSvc, "HGTD_ID" );

  std::string strg =  "initialize_from_dictionary";
  log << MSG::DEBUG << strg << endreq;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endreq;
    return (0);
  }
  else {
    if(m_msgSvc)log << MSG::DEBUG << "(Re)initialize" << endreq;
  }

  // init base object
  if(CaloIDHelper::initialize_base_from_dictionary(dict_mgr,
                                                   "LArCalorimeter"))
    return (1);
  
  // initialize dictionary version
  AtlasDetectorID::setDictVersion(dict_mgr, "LArCalorimeter");

  // Check whether dictionary has HGTD
  if (!dict()->find_group("hgtd")) {
    strg =  " initialize_from_dict - LArCalorimeter dictionary does NOT contain HGTD description. Unable to initialize HGTD_ID.";
    log << MSG::INFO << strg << endreq;
    return 0; // return OK
  }
      
  // Initialize the field indices
  if(initLevelsFromDict()) {
    strg =  " initialize_from_dict - cannot initialize HGTD part of LArCalorimeter dictionary ";
    if(m_msgSvc) {
      log << MSG::WARNING << strg << endreq;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);   
  }
  else {

    // Find value for the field LAr Calorimeter   
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
    int larField   = -1;
    if (atlasDict->get_label_value("subdet", "LArCalorimeter", larField)) {
      std::stringstream strm ;
      strm <<  atlasDict->m_name ;
      strg = "Could not get value for label 'LArCalorimeter' of field 'subdet' in dictionary " 
      + strm.str();
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endreq;
      }
      else {
	std::cout << strg << std::endl;
      }
      return (1);
    }


    // Find value for the field HGTD  - here the "part" has to be HGTD
    int larHgtdField   = -1;
    if (dict()->get_label_value("part", "HGTD", larHgtdField)) {
      std::stringstream strm ;
      strm <<  atlasDict->m_name ;
      strg = "Could not get value for label 'HGTD' of field 'part' in dictionary " 
      + strm.str();
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endreq;
      }
      else {
	std::cout << strg << std::endl;
      }
      return (1);
    }

    // Set up id for region and range prefix
    ExpandedIdentifier reg_id;
    reg_id.add(larField);
    reg_id.add(larHgtdField);
    Range prefix;	

    // (1) Connected channels
    std::string group_name = "hgtd";
    m_full_hgtd_range = dict()->build_multirange(reg_id, group_name, prefix);
    m_full_region_range = dict()->build_multirange(reg_id, group_name, prefix, "region");  

    // Setup the hash tables
    if(init_hashes()) return (1);

    // initilize m_two_sym_sides
    m_two_sym_sides = ( dictionaryVersion() == "fullAtlas" );

    /*
    if(m_do_neighbours) {
           if(init_neighbors(dict_mgr)) return (1);     
    }
    */
  }
   
  //  m_is_initialized = true;
  return 0;

}
int HGTD_ID::init_hashes(void){

  if (channels().init (*this, "channels",
                       m_full_hgtd_range, &HGTD_ID::channel_id,
                       m_Y_INDEX_INDEX))
    return 1;

  /* if (regions().init (*this, "granularity",
                      m_full_region_range, &HGTD_ID::region_id,
                      m_GRANULARITY_INDEX))
		      return 1;*/

  return (0);
}

int HGTD_ID::initLevelsFromDict(void) 
{
  MsgStream log(m_msgSvc, "HGTD_ID" );

  if(!dict()) {
    std::string strg = "initLevelsFromDict - dictionary NOT initialized ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endreq;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  // Find out which identifier field corresponds to each level.

  m_HGTD_REGION_INDEX = 9999;
  m_LAR_INDEX         = 9999;
  m_HGTD_INDEX        = 9999;
  m_BEC_INDEX         = 9999;
  m_SAMPLING_INDEX    = 9999;
  m_GRANULARITY_INDEX = 9999;
  m_X_INDEX_INDEX     = 9999;
  m_Y_INDEX_INDEX     = 9999;

  IdDictField* field = dict()->find_field("subdet") ;
  if (field) {
    m_LAR_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'subdet' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endreq;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("part") ;
  if (field) {
    m_HGTD_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'part' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endreq;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("barrel-endcap") ;
  if (field) {
    m_BEC_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'barrel-endcap' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endreq;
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
      log << MSG::ERROR << strg << endreq;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  
  field = dict()->find_field("granularity") ;
  if (field) {
    m_GRANULARITY_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'granularity' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endreq;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("x_index") ;
  if (field) {
    m_X_INDEX_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'x_index' field ";
    if(m_msgSvc) {
      log << MSG::WARNING << strg << endreq;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }
  
  
  field = dict()->find_field("y_index") ;
  if (field) {
    m_Y_INDEX_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'y_index' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endreq;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }
  
   // Set the field implementations


  IdDictDictionary::regions_const_it rit; 
  for (rit = dict()->m_regions.begin (); rit != dict()->m_regions.end (); ++rit) { 
    const IdDictRegion& region = *(*rit);
    if (region.m_group=="hgtd") {m_HGTD_REGION_INDEX = region.m_index; break;}
  }
  
  const IdDictRegion& region = *dict()->m_regions[m_HGTD_REGION_INDEX];

  m_lar_impl         = region.m_implementation[m_LAR_INDEX]; 
  m_hgtd_impl        = region.m_implementation[m_HGTD_INDEX]; 
  m_bec_impl         = region.m_implementation[m_BEC_INDEX]; 
  m_sampling_impl    = region.m_implementation[m_SAMPLING_INDEX]; 
  m_granularity_impl = region.m_implementation[m_GRANULARITY_INDEX]; 
  m_x_index_impl     = region.m_implementation[m_X_INDEX_INDEX]; 
  m_y_index_impl     = region.m_implementation[m_Y_INDEX_INDEX];

  if(m_msgSvc) {
    log << MSG::DEBUG << "decode index and bit fields for each level: " << endreq;
    log << MSG::DEBUG << "lar   "   << m_lar_impl.show_to_string()    << endreq;
    log << MSG::DEBUG << "hgtd  "   << m_hgtd_impl.show_to_string()   << endreq;
    log << MSG::DEBUG << "bec    "   << m_bec_impl.show_to_string()     << endreq;
    log << MSG::DEBUG << "sampling"  << m_sampling_impl.show_to_string()  << endreq;
    log << MSG::DEBUG << "granularity"  << m_granularity_impl.show_to_string()  << endreq;
    log << MSG::DEBUG << "x_index "   << m_x_index_impl.show_to_string()    << endreq;
    log << MSG::DEBUG << "y_index "   << m_y_index_impl.show_to_string()    << endreq;
  }
  else {
    std::cout << "decode index and bit fields for each level: " << endreq;
    std::cout << "lar   "   << m_lar_impl.show_to_string()    << endreq;
    std::cout << "hgtd  "   << m_hgtd_impl.show_to_string()   << endreq;
    std::cout << "bec    "   << m_bec_impl.show_to_string()     << endreq;
    std::cout << "sampling"  << m_sampling_impl.show_to_string()  << endreq;
    std::cout << "granularity"  << m_granularity_impl.show_to_string()  << endreq;
    std::cout << "x_index "   << m_x_index_impl.show_to_string()    << endreq;
    std::cout << "y_index "   << m_y_index_impl.show_to_string()    << endreq;
  }


  return(0) ;
}

void HGTD_ID::channel_id_checks   ( int barrel_ec, int sampling, int granularity, int xIndex, int yIndex ) const throw(LArID_Exception)
{  
    // Check that id is within allowed range
  
    // Fill expanded id
  ExpandedIdentifier expId(lar_hgtd_exp());
  expId << barrel_ec << sampling << granularity << xIndex << yIndex;
  
    if( expId.last_error () != ExpandedIdentifier::none) {
	std::string errorMessage = 
          "Error in HGTD_ID::channel_id_checks(field values), " + 
          strformat ("barrel_ec: %d , sampling: %d, granularity: %d , xIndex: %d , yIndex: %d ",
                     barrel_ec , sampling , granularity, xIndex, yIndex);
	throw LArID_Exception(errorMessage , 6);
    }

    if (!m_full_hgtd_range.match(expId)) { 
	std::string errorMessage = "HGTD_ID::channel_id_checks() result is not OK: ID, range = "
	    + std::string(expId) + " , " + (std::string)m_full_hgtd_range;
	throw LArID_Exception(errorMessage , 6);
    } 
}

