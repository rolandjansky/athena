/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArIdentifier/LArHVLineID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include "GaudiKernel/MsgStream.h"
#include <algorithm>
#include <string>
#include <set>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <assert.h>


LArHVLineID::LArHVLineID(void) : 
  m_larhvRegion_index(999),
  m_atlas_index(999),
  m_configuration_index(999),
  m_partition_index(999),
  m_canline_index(999),
  m_cannode_index(999),
  m_hvline_index(999),
  m_dict(0),
  m_hvlineHashMax(0)
{

}

LArHVLineID:: ~LArHVLineID(){}


IdContext LArHVLineID::hvlineContext(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_hvline_index));
}

/*
IdContext LArHVLineID::canlineContext(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_canline_index));
}
*/

//==========================================================================
int  LArHVLineID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
//==========================================================================
{

  MsgStream log(m_msgSvc, "LArHVLineID" );
  std::string strg = " => initialize_from_dictionary()";
  if(m_msgSvc) {
    log << MSG::INFO << strg << endmsg;
  }
  else {
    std::cout << strg << std::endl;
  }
  
  // Check whether this helper should be reinitialized
  // -------------------------------------------------
  if (!reinitialize(dict_mgr)) {
    if(m_msgSvc)log << MSG::DEBUG 
		    << "Request to reinitialize not satisfied - tags have not changed" 
		    << endmsg;
    return (0);
  }
  else {
    log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }
  log << MSG::DEBUG << " => Initialization of dict_mgr done ! " << m_dict << endmsg;

  // init base object
  // ----------------
  if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)){
    return (1);
  }
  else{
    log << MSG::INFO << " => initialize_from_dictionary(dict_mgr) =" 
	<< AtlasDetectorID::initialize_from_dictionary(dict_mgr) 
	<< endmsg;
  }
  m_dict = dict_mgr.find_dictionary ("LArHighVoltage"); 

  if(!m_dict) 
    {
      strg = " initialize_from_dictionary - cannot access LArHighVoltage dictionary ";
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else 
	{
	  std::cout << "LArHVLineID::" << strg << std::endl;
	}
      return 1;
    }

  // Register version of the dictionary used
  // ---------------------------------------
  if (register_dict_tag(dict_mgr, "LArHighVoltage")) return(1);
  log << MSG::INFO << "Register_dict_tag of LArHighVoltage is OK" << endmsg;

  // initialize dictionary version
  // -----------------------------
  AtlasDetectorID::setDictVersion(dict_mgr, "LArHighVoltage");
  log << MSG::INFO << "setDictVersion of LArHighVoltage is OK" << endmsg;


  // Initialize the field indices
  // =========================================================================
  if(initLevelsFromDict()) return (1); 

  /* Find value for the field Calorimeter */
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
  int larHVValue   = -1;
  if (atlasDict->get_label_value("subdet", "LArHighVoltage", larHVValue)) {
    std::stringstream strm;
    strm << atlasDict->m_name;
    strg= " Could not get value for label 'LArHighVoltage' of field 'subdet' in dictionary "+strm.str();
    if(m_msgSvc){
      log << MSG::ERROR << strg << endmsg;
    }
    else{
      std::cout << "LArHVLineID:" << strg << std::endl;
    }
    return (1);
  }
  log << MSG::DEBUG << "[init_from_dictionary] > larHV value = "<< larHVValue << endmsg; 


  /* Find values for the field Configuration */
  int configurationValue   = 1;
  if (m_dict->get_label_value("configuration", "Atlas", configurationValue)) {
    std::stringstream strm;
    strm <<  m_dict->m_name;      
    strg = "WARNING : Could not get value for label 'configuration' in dictionary "+strm.str();
    if(m_msgSvc) {
      log << MSG::INFO << strg << endmsg;
    }
    else{
      std::cout << strg << std::endl;
    }
    return (0);
  }
  log << MSG::DEBUG << "[init_from_dictionary] > configurationValue = "
      << configurationValue << endmsg; 

  // Set up Expanded identifier for hvline range prefix
  // =========================================================
  ExpandedIdentifier reg_id;
  reg_id.add(larHVValue);
  reg_id.add(configurationValue); 
  Range prefix;

  /*Full range for all lines */
  m_full_atlas_highvoltage_range=m_dict->build_multirange(reg_id, prefix);
  m_full_hvline_range = m_dict->build_multirange(reg_id, prefix, "hvline");
  m_full_canline_range = m_dict->build_multirange(reg_id, prefix, "canline");
  log << MSG::INFO << "[initialize_from_dictionary] >  HV line range -> "  
      << (std::string)m_full_hvline_range << endmsg;
  
  // Setup the hash tables
  // =========================================================
  if(init_hashes()) return (1);

  return 0;
}


//=====================================================================================
int LArHVLineID::get_expanded_id  (const HWIdentifier& id, 
				   ExpandedIdentifier& exp_id, 
				   const IdContext* context) const
//=====================================================================================
{
  MsgStream log(m_msgSvc, "LArHVLineID" );
  // We assume that the context is >= hvline
  exp_id.clear();
  exp_id << lar_field_value()
	 << s_lar_atlas_value
    	 << partition(id)
    	 << can_line(id)
	 << can_node(id)
	 << hv_line(id);
  if(context && context->end_index() >= m_hvline_index) {
    exp_id << hv_line(id);
  }
  return (0);
}


//=============================================================================
int LArHVLineID::initLevelsFromDict(void) 
//=============================================================================
{
  MsgStream log(m_msgSvc, "LArHVLineID" );
  log << MSG::DEBUG  << "[initLevelsFromDict] Entering routine... " << endmsg;

  if(!m_dict) {
    log << MSG::INFO  << "LArHVLineID::initLevelsFromDict - dictionary NOT initialized "
              << endmsg ;
    return (1);
  }

  log << MSG::INFO  << "[initLevelsFromDict] m_dict OK ... " << endmsg;

  // Find out which identifier field corresponds to each level.
  // ========================================================================
  m_atlas_index               = 999;
  m_configuration_index       = 999;
  m_partition_index           = 999;
  m_canline_index             = 999;
  m_cannode_index             = 999;
  m_hvline_index              = 999;

  log << MSG::DEBUG  << "[initLevelsFromDict] data member initialization OK ... "  << endmsg;
  
  // Search with region name
  IdDictRegion* reg = m_dict->find_region("LArHV-HEC-A");
  if (reg) {
      m_larhvRegion_index = reg->m_index;}
  else {
    log << MSG::INFO  << "WARNING : LArHVLineID::initLevelsFromDict - unable to find 'barrel-region1' region"  
	      << endmsg;
    return (0);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] region 'LAr-HV-HEC-A' found OK ... "  << endmsg;

  // Find ATLAS field 
  // ========================================================================
  IdDictField* field = m_dict->find_field("subdet") ;
  if (field) {
    m_atlas_index = field->m_index ;}
  else {
    log << MSG::INFO  
	<< "LArHVLineID::initLevelsFromDict - unable to find 'subdet' field " << endmsg ;
    return (1);
  }
  log << MSG::DEBUG 
      << "[initLevelsFromDict] field 'LArHighVoltage' found OK "  << endmsg;

  // Find Configuration field 
  // ========================================================================
  field = m_dict->find_field("configuration") ;
  if (field) {
    m_configuration_index = field->m_index ;}
  else {
    log << MSG::INFO  
	<< "LArHVLineID::initLevelsFromDict - unable to find 'configuration' field " << endmsg ;
    return (1);
  }
  log << MSG::DEBUG 
      << "[initLevelsFromDict] field config=Atlas found OK "  << endmsg;

  // Look for Field 'partition'
  // ========================================================================
  field = m_dict->find_field("partition") ;
  if (field) {
    m_partition_index = field->m_index ;}
  else {
    log << MSG::INFO  <<  "LArHVLineID::initLevelsFromDict - unable to find 'partition' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'partition' found OK "  << endmsg;


  // Look for Field 'CAN LINE'
  // ========================================================================
  field = m_dict->find_field("canline") ;
  if (field) {
    m_canline_index = field->m_index ;}
  else {
    log << MSG::INFO  <<  "LArHVLineID::initLevelsFromDict - unable to find 'canline' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'canline' found OK "  << endmsg;


  // Look for Fields 'CAN NODE'
  // ========================================================================
  field = m_dict->find_field("cannode") ;
  if (field) {
    m_cannode_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "LArHVLineID::initLevelsFromDict - unable to find 'cannode' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'cannode' found OK "  << endmsg;

  
  // Look for Fields 'HV_line'
  // ========================================================================
  field = m_dict->find_field("hvline") ;
  if (field) {
    m_hvline_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "LArHVLineID::initLevelsFromDict - unable to find 'hvline' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'hvline' found OK "  << endmsg;


  // Set the field implementation
  // ========================================================================

  const IdDictRegion& region = *m_dict->m_regions[m_larhvRegion_index];
  log << MSG::DEBUG  << "[initLevelsFromDict] Found levels: " << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > larHV           " << m_atlas_index     << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > larConfiguration " << m_configuration_index     << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > CAN Node       " << m_cannode_index    << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > HV line        " << m_hvline_index      << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > partition      " << m_partition_index  << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > CAN line       " << m_canline_index  << endmsg ;


  log << MSG::DEBUG  << "[initLevelsFromDict] > ...fields implementation... " << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > ...implementation: m_larhvcalo_index " << endmsg;
  m_atlas_impl        = region.m_implementation[m_atlas_index]; 
  m_configuration_impl= region.m_implementation[m_configuration_index]; 
  m_partition_impl    = region.m_implementation[m_partition_index]; 
  m_canline_impl      = region.m_implementation[m_canline_index]; 
  m_cannode_impl      = region.m_implementation[m_cannode_index]; 
  m_hvline_impl       = region.m_implementation[m_hvline_index]; 
  
  log << MSG::DEBUG  << "[initLevelsFromDict] Decode index and bit fields for each level: " << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > larHV      "  << m_atlas_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > larConfig  "  << m_configuration_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > partition  "  << m_partition_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > can line   "  << m_canline_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > can node   "  << m_cannode_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > hv line    "  << m_hvline_impl.show_to_string()  << endmsg;


  return(0) ;
}




//=====================================================
int  LArHVLineID::init_hashes(void) 
//=====================================================
{
  MsgStream log(m_msgSvc, "LArHVLineID" );
  std::stringstream strm1;
  std::stringstream strm2;
  std::stringstream strm3;
  std::string strg1;
  std::string strg2;
  std::string strg3;

  // tower hash
  // -----------
  m_hvlineHashMax = m_full_atlas_highvoltage_range.cardinality();
  m_hvline_vec.resize(m_hvlineHashMax);
  unsigned int nids = 0;
  std::set<HWIdentifier> ids;
  for (unsigned int i = 0; i < m_full_atlas_highvoltage_range.size(); ++i) {
    const Range& range = m_full_atlas_highvoltage_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      HWIdentifier hv_id = HVLineId( 
				    exp_id[m_partition_index] ,
				    exp_id[m_canline_index] ,
				    exp_id[m_cannode_index] ,
				    exp_id[m_hvline_index]  
				    );
      if(!(ids.insert(hv_id)).second){
	strm1 << nids;
	strg1 = "[init_hashes] > duplicated id for channel nb = "+strm1.str();
	strm3 << show_to_string(hv_id);
	strg3 = " expanded Id= "+strm3.str();
	if(m_msgSvc){
	  log  << MSG::ERROR << strg1 << endmsg;
	  log  << MSG::ERROR << strg3 << endmsg;
	}
      }
      nids++;
    }
  }
  if(ids.size() != m_hvlineHashMax) {
    strm1 << ids.size();
    strm2 << m_hvlineHashMax;
    strg1 = "[init_hashes] >";
    strg2 = " set size NOT EQUAL to hash max. size "+strm1.str();
    strg3 = " hash max "+strm2.str();
    if(m_msgSvc)
      {
	log << MSG::ERROR << strg1 << endmsg;
	log << MSG::ERROR << strg2 << endmsg;
	log << MSG::ERROR << strg3 << endmsg;
      }
    else
      {
	std::cout << "LArOnlineID::" << strg1 << std::endl;
	std::cout << "Error" << strg2 << std::endl;
	std::cout << strg3 << std::endl;
      }
    return (1);
  }

  nids=0;
  std::set<HWIdentifier>::const_iterator first = ids.begin();
  std::set<HWIdentifier>::const_iterator last  = ids.end();
  for (;first != last && nids < m_hvline_vec.size(); ++first) {
    m_hvline_vec[nids] = (*first) ;
    nids++;
  }
  log << MSG::INFO << "[init_hashes()] > Hvline_size= " << m_hvline_vec.size() << endmsg;
  return (0);                   
}



