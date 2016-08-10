/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArIdentifier/LArElectrodeID.h"
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


LArElectrodeID::LArElectrodeID(void) : 
  m_larElectrodeRegion_index(999),
  m_larelectrode_index(999),
  m_atlas_index(999),
  m_configuration_index(999),
  m_detector_index(999),
  m_zside_index(999),
  m_module_index(999),
  m_hvphi_index(999),
  m_hveta_index(999),
  m_hvgap_index(999),
  m_electrode_index(999),
  m_dict(0),
  m_electrodeHashMax(0)
{

}

LArElectrodeID:: ~LArElectrodeID(){}


IdContext LArElectrodeID::electrodeContext(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_electrode_index));
}


//==========================================================================
int  LArElectrodeID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
//==========================================================================
{

  MsgStream log(m_msgSvc, "LArElectrodeID" );
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
    log << MSG::DEBUG << " => initialize_from_dictionary(dict_mgr) =" 
	<< AtlasDetectorID::initialize_from_dictionary(dict_mgr) 
	<< endmsg;
  }
  m_dict = dict_mgr.find_dictionary ("LArElectrode"); 
  log << MSG::DEBUG << " => defined m_dict from find_dictionary(LArElectrode) = " 
      << m_dict << endmsg;
  if(!m_dict) {
    strg = " initialize_from_dictionary - cannot access LArElectrode dictionary ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << "LArElectrodeID::" << strg << std::endl;
    }
    return 1;
  }
  
  // Register version of the dictionary used
  // ---------------------------------------
  if (register_dict_tag(dict_mgr, "LArElectrode")) return(1);
  log << MSG::DEBUG << "Register_dict_tag of LArElectrode is OK" << endmsg;

  // initialize dictionary version
  // -----------------------------
  AtlasDetectorID::setDictVersion(dict_mgr, "LArElectrode");
  log << MSG::DEBUG << "setDictVersion of LArElectrode is OK" << endmsg;


  // Initialize the field indices
  // =========================================================================
  if(initLevelsFromDict()) return (1); 

  /* Find value for the field Calorimeter */
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
  int larField   = -1;
  if (atlasDict->get_label_value("subdet", "LArElectrode", larField)) {
    std::stringstream strm;
    strm << atlasDict->m_name;
    strg= " Could not get value for label 'LArElectrode' of field 'subdet' in dictionary "+strm.str();
    if(m_msgSvc){
      log << MSG::ERROR << strg << endmsg;
    }
    else{
      std::cout << "LArElectrodeID:" << strg << std::endl;
    }
    return (1);
  }
  log << MSG::DEBUG << "[init_from_dictionary] > larField value = "<< larField << endmsg; 


  /* Find values for the field Detector */
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

  // Set up Expanded identifier for electrode range prefix
  // =========================================================
  ExpandedIdentifier reg_id;
  reg_id.add(larField);
  reg_id.add(configurationValue); 
  Range prefix;

  /*Full range for all lines */
  m_full_larelectrode_range=m_dict->build_multirange(reg_id, prefix);
  m_full_electrode_range = m_dict->build_multirange(reg_id, prefix, "electrode");
  log << MSG::DEBUG << "[initialize_from_dictionary] >  Electrode range -> "  
      << (std::string)m_full_electrode_range << endmsg;
  
  // Setup the hash tables
  // =========================================================
  if(init_hashes()) return (1);

  return 0;
}


//=====================================================================================
int LArElectrodeID::get_expanded_id  (const HWIdentifier& id, 
				      ExpandedIdentifier& exp_id, 
				      const IdContext* context) const
//=====================================================================================
{
  MsgStream log(m_msgSvc, "LArElectrodeID" );
  // We assume that the context is >= electrode
  exp_id.clear();
  exp_id << lar_field_value()
	 << s_larconfig_value
    	 << detector(id)
    	 << zside(id)
	 << module(id)
	 << hv_phi(id)
	 << hv_eta(id)
	 << gap(id);
  if(context && context->end_index() >= m_electrode_index) {
    exp_id << electrode(id);
  }
  return (0);
}



//=============================================================================
int LArElectrodeID::initLevelsFromDict(void) 
//=============================================================================
{
  MsgStream log(m_msgSvc, "LArElectrodeID" );
  log << MSG::DEBUG  << "[initLevelsFromDict] Entering routine... " << endmsg;

  if(!m_dict) {
    log << MSG::INFO  << "LArElectrodeID::initLevelsFromDict - dictionary NOT initialized "
              << endmsg ;
    return (1);
  }

  log << MSG::DEBUG  << "[initLevelsFromDict] m_dict OK ... " << endmsg;

  // Find out which identifier field corresponds to each level.
  // ========================================================================
  m_atlas_index          = 999;
  m_configuration_index  = 999;
  m_detector_index       = 999;
  m_zside_index          = 999;
  m_module_index         = 999;
  m_hvphi_index          = 999;
  m_hveta_index          = 999;
  m_hvgap_index          = 999;
  m_electrode_index      = 999;
  log << MSG::DEBUG  << "[initLevelsFromDict] data member initialization OK ... "  << endmsg;
  
  // Search with region name
  IdDictRegion* reg = m_dict->find_region("EM-BARREL-ALL");
  if (reg) {
      m_larElectrodeRegion_index = reg->m_index;}
  else {
    log << MSG::INFO  << "WARNING : [initLevelsFromDict] - unable to find 'EM-BARREL-ALL' region"  
	      << endmsg;
    return (0);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] region 'EM-BARREL-ALL' found OK ... "  << endmsg;

  // Find LArElectrode field 
  // ========================================================================
  IdDictField* field = m_dict->find_field("subdet") ;
  if (field) {
    m_atlas_index = field->m_index ;}
  else {
    log << MSG::INFO  
	<< "LArElectrodeID::initLevelsFromDict - unable to find 'subdet' field " << endmsg ;
    return (1);
  }
  log << MSG::DEBUG 
      << "[initLevelsFromDict] field 'LArElectrode' found OK "  << endmsg;

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

  // Find Detector field 
  // ========================================================================
  field = m_dict->find_field("detector") ;
  if (field) {
    m_detector_index = field->m_index ;}
  else {
    log << MSG::INFO  
	<< "LArElectrodeID::initLevelsFromDict - unable to find 'detector' field " << endmsg ;
    return (1);
  }
  log << MSG::DEBUG 
      << "[initLevelsFromDict] field 'detector' found OK "  << endmsg;


  // Look for Field 'zside'
  // ========================================================================
  field = m_dict->find_field("zside") ;
  if (field) {
    m_zside_index = field->m_index ;}
  else {
    log << MSG::INFO  <<  "LArElectrodeID::initLevelsFromDict - unable to find 'zside' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'zside' found OK "  << endmsg;


  // Look for Fields 'module'
  // ========================================================================
  field = m_dict->find_field("module") ;
  if (field) {
    m_module_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "LArElectrodeID::initLevelsFromDict - unable to find 'module' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'module' found OK "  << endmsg;

  // Look for Fields 'hvphi'
  // ========================================================================
  field = m_dict->find_field("hvphi") ;
  if (field) {
    m_hvphi_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "initLevelsFromDict - unable to find 'hvphi' field "<< endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'hvphi' found OK "  << endmsg;

  // Look for Fields 'hveta'
  // ========================================================================
  field = m_dict->find_field("hveta") ;
  if (field) {
    m_hveta_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "[initLevelsFromDict] - unable to find 'hveta' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'hveta' found OK "  << endmsg;

  // Look for Fields 'hvgap'
  // ========================================================================
  field = m_dict->find_field("hvgap") ;
  if (field) {
    m_hvgap_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "[initLevelsFromDict] - unable to find 'hvgap' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'hvgap' found OK "  << endmsg;

  
  // Look for Fields 'electrode'
  // ========================================================================
  field = m_dict->find_field("electrode") ;
  if (field) {
    m_electrode_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "[initLevelsFromDict] - unable to find 'electrode' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] field 'electrode' found OK "  << endmsg;


  // Set the field implementation
  // ========================================================================

  const IdDictRegion& region = *m_dict->m_regions[m_larElectrodeRegion_index];
  log << MSG::DEBUG  << "[initLevelsFromDict] Found levels: " << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > Atlas     : " << m_atlas_index  << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > Configurat: " << m_configuration_index  << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > detector  : " << m_detector_index  << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > zside     : " << m_zside_index     << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > hvphi     : " << m_hvphi_index     << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > hveta     : " << m_hveta_index     << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > hvgap     : " << m_hvgap_index     << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > electrode : " << m_electrode_index << endmsg ;
  m_atlas_impl      = region.m_implementation[m_atlas_index]; 
  m_configuration_impl= region.m_implementation[m_configuration_index]; 
  m_detector_impl   = region.m_implementation[m_detector_index]; 
  m_zside_impl      = region.m_implementation[m_zside_index]; 
  m_module_impl     = region.m_implementation[m_module_index];
  m_hvphi_impl      = region.m_implementation[m_hvphi_index]; 
  m_hveta_impl      = region.m_implementation[m_hveta_index]; 
  m_hvgap_impl      = region.m_implementation[m_hvgap_index];  
  m_electrode_impl  = region.m_implementation[m_electrode_index]; 
  
  log << MSG::DEBUG  << "[initLevelsFromDict] Decode index and bit fields for each level: " << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > Atlas       "  << m_atlas_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > larConfig  "  << m_configuration_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > detector    "  << m_detector_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > z-side      "  << m_zside_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > module      "  << m_module_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > hvphi       "  << m_hvphi_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > hveta       "  << m_hveta_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > hvgap       "  << m_hvgap_impl.show_to_string() << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > hv line     "  << m_electrode_impl.show_to_string()  << endmsg;


  return(0) ;
}




//=====================================================
int  LArElectrodeID::init_hashes(void) 
//=====================================================
{
  MsgStream log(m_msgSvc, "LArElectrodeID" );
  std::stringstream strm1;
  std::stringstream strm2;
  std::stringstream strm3;
  std::string strg1;
  std::string strg2;
  std::string strg3;

  // tower hash
  // -----------
  m_electrodeHashMax = m_full_larelectrode_range.cardinality();
  m_electrode_vec.resize(m_electrodeHashMax);
  unsigned int nids = 0;
  std::set<HWIdentifier> ids;
  for (unsigned int i = 0; i < m_full_larelectrode_range.size(); ++i) {
    const Range& range = m_full_larelectrode_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      HWIdentifier elec_id = ElectrodeId( 
					 exp_id[m_detector_index] ,
					 exp_id[m_zside_index] ,
					 exp_id[m_module_index] ,
					 exp_id[m_hvphi_index] ,
					 exp_id[m_hveta_index] ,
					 exp_id[m_hvgap_index] ,
					 exp_id[m_electrode_index]  
					 );
      if(!(ids.insert(elec_id)).second){
	strm1 << nids;
	strg1 = "[init_hashes] > duplicated id for channel nb = "+strm1.str();
	strm3 << show_to_string(elec_id);
	strg3 = " expanded Id= "+strm3.str();
	if(m_msgSvc){
	  log  << MSG::ERROR << strg1 << endmsg;
	  log  << MSG::ERROR << strg3 << endmsg;
	}
      }
      nids++;
    }
  }
  if(ids.size() != m_electrodeHashMax) {
    strm1 << ids.size();
    strm2 << m_electrodeHashMax;
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
  for (;first != last && nids < m_electrode_vec.size(); ++first) {
    m_electrode_vec[nids] = (*first) ;
    nids++;
  }
  log << MSG::DEBUG << "[init_hashes()] > Electrode_size= " << m_electrode_vec.size() << endmsg;
  return (0);                   
}



