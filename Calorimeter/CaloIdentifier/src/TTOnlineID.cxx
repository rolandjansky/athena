/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/TTOnlineID.h"
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


TTOnlineID::TTOnlineID(void) :
  m_calo_index(999),
  m_detzside_index(999),
  m_l1online_regions_index(999),
  m_crate_index(999),
  m_module_index(999),
  m_submodule_index(999),
  m_channel_index(999),
  m_dict(0),
  m_crateHashMax(0),
  m_moduleHashMax(0),
  m_submoduleHashMax(0),
  m_channelHashMax(0),
  m_iDetZSideFieldValue(-1)
{

}

TTOnlineID:: ~TTOnlineID(){}


IdContext TTOnlineID::crateContext(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_crate_index));
}

IdContext TTOnlineID::moduleContext(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_module_index));
}

IdContext TTOnlineID::submoduleContext(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_submodule_index));
}

IdContext TTOnlineID::channelContext(void) const
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_channel_index));
}



//==========================================================================
int  TTOnlineID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
//==========================================================================
{

  MsgStream log(m_msgSvc, "TTOnlineID" );
  std::string strg = "initialize_from_dictionary";
  if(m_msgSvc) {
    log << MSG::INFO << strg << endmsg;
  }
  else {
    std::cout << strg << std::endl;
  }

  // Check whether this helper should be reinitialized
  // -------------------------------------------------
  if (!reinitialize(dict_mgr)) {
    if(m_msgSvc)log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed"
		    << endmsg;
    return (0);
  }
  else {
    log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  // init base object
  // ----------------
  if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)){
    return (1);
  }

  m_dict = dict_mgr.find_dictionary("Calorimeter");

  log << MSG::DEBUG << " => defined m_dict from find_dictionary(Calorimeter) = " << m_dict << endmsg;
  if(!m_dict)
    {
      strg = " initialize_from_dictionary - cannot access Calorimeter dictionary";
      if(m_msgSvc) {
	//log << MSG::ERROR << strg << endmsg;
      }
      else
	{
	  std::cout << "TTOnlineID::" << strg << std::endl;
	}
      return 1;
    }

  // Register version of the dictionary used
  // ---------------------------------------
  if (register_dict_tag(dict_mgr, "Calorimeter")) return(1);

  // initialize dictionary version
  // -----------------------------
  AtlasDetectorID::setDictVersion(dict_mgr, "Calorimeter");


  // Initialize the field indices
  // --------------------------------------
  if(initLevelsFromDict()) return (1);

  /* Find value for the field Calorimeter */
  // ---------------------------------------
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS");
  int caloValue   = -1;
  if (atlasDict->get_label_value("subdet", "Calorimeter", caloValue)) {
    std::stringstream strm;
    strm << atlasDict->m_name;
    strg= " Could not get value for label 'Calorimeter' of field 'subdet' in dictionary "+strm.str();
    if(m_msgSvc){
      log << MSG::ERROR << strg << endmsg;
    }
    else{
      std::cout << "TTOnlineID:" << strg << std::endl;
    }
    return (1);
  }
  log << MSG::DEBUG << "[init_from_dictionary] > caloValue = "<< caloValue << endmsg;

  /* Find values for the fake field DetZside */
  // --------------------------------------

  // retrieve the calo tag from the DB
  std::string tag = m_dict->dict_tag();
  bool oldTag = ( tag.size()==0 || tag == "CaloIdentifier-DC3-05" || tag == "CaloIdentifier-LVL1-01");
  log << MSG::DEBUG << "Calorimeter dict. DB tag= " << tag << endmsg;

  //int detzsideValue   = -1;
  //if (m_dict->get_label_value("DetZside", "no_side", detzsideValue)) {
  if (m_dict->get_label_value("DetZside", "no_side", m_iDetZSideFieldValue)) {
    if (!oldTag) {
      std::stringstream strm;
      strm <<  m_dict->m_name;
      strg = "WARNING : Could not get value for label 'DetZside' in dictionary "+strm.str();
      if(m_msgSvc) {
	log << MSG::INFO << strg << endmsg;
      }
      else{
	std::cout << strg << std::endl;
      }
    }
    return (0);
  }
  log << MSG::DEBUG << "[init_from_dictionary] > detzsideValue = "<< detzside_field_value() << endmsg;


  // Set up id for tower and layer's range prefix
  // ---------------------------------------------
  //   Note: negative value is good enough to get multirange since the
  //   regions are symmetric in +/-eta

  ExpandedIdentifier reg_id;
  reg_id.add(caloValue);
  reg_id.add(detzside_field_value());

  Range prefix;
  m_full_crate_range     = m_dict->build_multirange(reg_id, prefix, "crate");
  m_full_module_range    = m_dict->build_multirange(reg_id, prefix, "module");
  m_full_submodule_range = m_dict->build_multirange(reg_id, prefix, "submodule");
  m_full_channel_range   = m_dict->build_multirange(reg_id, prefix, "channel");

  log << MSG::DEBUG << "[initialize_from_dictionary] >  Crate range -> "
      << (std::string)m_full_crate_range << endmsg;
  log << MSG::DEBUG << "[initialize_from_dictionary] >  Module range -> "
      << (std::string)m_full_module_range << endmsg;
  log << MSG::DEBUG << "[initialize_from_dictionary] >  SubModule range -> "
      << (std::string)m_full_submodule_range << endmsg;
  log << MSG::DEBUG << "[initialize_from_dictionary] >  Channel range -> "
      << (std::string)m_full_channel_range << endmsg;


  // Setup the hash tables
  // --------------------------------------
  if(init_hashes()) return (1);
  return 0;
}


//=====================================================================================
int TTOnlineID::get_expanded_id  (const HWIdentifier& id, ExpandedIdentifier& exp_id,
				     const IdContext* context) const
//=====================================================================================
{
  MsgStream log(m_msgSvc, "TTOnlineID" );
  // We assume that the context is >= region
  exp_id.clear();
  exp_id << calo_field_value();
  exp_id << detzside_field_value();
  if(context && context->end_index() >= m_crate_index) {
    exp_id << crate(id);
    if(context->end_index() >= m_module_index) {
      exp_id << module(id);
      if(context->end_index() >= m_submodule_index) {
				exp_id << submodule(id);
				if(context->end_index() >= m_channel_index)
					exp_id << channel(id);
      }
    }
  }
  return (0);
}


//=====================================================================================
void TTOnlineID::channel_Id_checks   ( int crate, int module, int submodule, int channel  ) const
//=====================================================================================
{
  // Fill expanded id
  ExpandedIdentifier id(calo_exp());
  id << detzside_field_value() << crate << module << submodule << channel;
  if (!m_full_channel_range.match(id)) {
    std::string errorMessage = "[TTOnlineID]::channnelId() result is not OK: ID, range = "
      + std::string(id) + " , " + (std::string)m_full_channel_range;
    throw CaloID_Exception(errorMessage , 2);
  }
}

/* ================================================================*/
void TTOnlineID::channel_Id_checks   ( const HWIdentifier submoduleId, int channel ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
    ExpandedIdentifier expId;
    IdContext context = submoduleContext();
    if (get_expanded_id(submoduleId, expId, &context)) {
	std::string errorMessage = "TTOnlineID::channel_Id_checks(submoduleId) result is not OK: ID = "
	    + show_to_string(submoduleId) ;
	throw CaloID_Exception(errorMessage , 99);
    }

    expId << channel;
    if (!m_full_channel_range.match(expId)) {
	std::string errorMessage = "TTOnlineID::channel_Id_checks(submoduleId) result is not OK: ID, range = "
	    + std::string(expId) + " , " + (std::string)m_full_channel_range;
	throw CaloID_Exception(errorMessage , 99);
    }
}

/* ================================================================*/
void TTOnlineID::channel_Id_checks   ( const HWIdentifier moduleId, int submodule, int channel ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
    ExpandedIdentifier expId;
    IdContext context = moduleContext();
    if (get_expanded_id(moduleId, expId, &context)) {
	std::string errorMessage = "TTOnlineID::channel_Id_checks(moduleId) result is not OK: ID = "
	    + show_to_string(moduleId) ;
	throw CaloID_Exception(errorMessage , 99);
    }

    expId << submodule << channel;
    if (!m_full_channel_range.match(expId)) {
	std::string errorMessage = "TTOnlineID::channel_Id_checks(submoduleId) result is not OK: ID, range = "
	    + std::string(expId) + " , " + (std::string)m_full_channel_range;
	throw CaloID_Exception(errorMessage , 99);
    }
}

/* ================================================================*/
void TTOnlineID::channel_Id_checks   ( const HWIdentifier crateId, int module, int submodule, int channel ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
    ExpandedIdentifier expId;
    IdContext context = crateContext();
    if (get_expanded_id(crateId, expId, &context)) {
	std::string errorMessage = "TTOnlineID::channel_Id_checks(crateId) result is not OK: ID = "
	    + show_to_string(crateId) ;
	throw CaloID_Exception(errorMessage , 99);
    }

    expId << module << submodule << channel;
    if (!m_full_channel_range.match(expId)) {
	std::string errorMessage = "TTOnlineID::channel_Id_checks(crateId) result is not OK: ID, range = "
	    + std::string(expId) + " , " + (std::string)m_full_channel_range;
	throw CaloID_Exception(errorMessage , 99);
    }
}

/* ================================================================*/
void TTOnlineID::submodule_Id_checks( int crate, int module, int submodule ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(calo_exp());
  expId << detzside_field_value() << crate <<  module << submodule;

  if (!m_full_submodule_range.match(expId)) { std::string errorMessage = "TTOnlineID::submodule_Id_checks() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_submodule_range;
    throw CaloID_Exception(errorMessage , 99);
  }
}

/* ================================================================*/
void TTOnlineID::module_Id_checks( int crate, int module ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(calo_exp());
  expId << detzside_field_value() << crate << module;

  if (!m_full_module_range.match(expId)) { std::string errorMessage = "TTOnlineID::module_Id_checks() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_module_range;
    throw CaloID_Exception(errorMessage , 99);
  }
}

/* ================================================================*/
void TTOnlineID::crate_Id_checks( int crate ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(calo_exp());
  expId << detzside_field_value() << crate;

  if (!m_full_crate_range.match(expId)) { std::string errorMessage = "TTOnlineID::crate_Id_checks() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_crate_range;
    throw CaloID_Exception(errorMessage , 99);
  }
}


//=========================================================
int TTOnlineID::initLevelsFromDict(void)
//=========================================================
{
  MsgStream log(m_msgSvc, "TTOnlineID" );
  log << MSG::DEBUG  << "[initLevelsFromDict] (0) Entering routine... " << endmsg;

  if(!m_dict) {
    log << MSG::INFO  << "TTOnlineID::initLevelsFromDict - dictionary NOT initialized "
              << endmsg ;
    return (1);
  }

  log << MSG::DEBUG  << "[initLevelsFromDict] (1) m_dict OK ... " << endmsg;

  // Find out which identifier field corresponds to each level.
  // ----------------------------------------------------------
  m_calo_index                = 999 ;
  m_detzside_index            = 999 ;
  m_crate_index               = 999 ;
  m_module_index              = 999 ;
  m_submodule_index           = 999 ;
  m_channel_index             = 999 ;

  log << MSG::DEBUG  << "[initLevelsFromDict] (2) data member initialization OK ... "  << endmsg;

  // retrieve the calo tag from the DB
  std::string tag = m_dict->dict_tag();
  bool oldTag = ( tag.size()==0 || tag == "CaloIdentifier-DC3-05" || tag == "CaloIdentifier-LVL1-01");
  log << MSG::DEBUG << "Calorimeter dict. DB tag= " << tag << endmsg;

  // Save index to a Online LVL1 region for unpacking - search with region name
  IdDictRegion* reg = m_dict->find_region("PPM_Crate-00");
  if (reg) {
      m_l1online_regions_index = reg->m_index;}
  else {
    if(!oldTag) log << MSG::INFO  << "WARNING : TTOnlineID::initLevelsFromDict - unable to find 'PPM_Crate-00' region" << endmsg;
    return (0);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] (3) region 'PPM_Crate-00' found OK ... " << m_l1online_regions_index << endmsg;

  // Look for Fields...
  // ------------------
  IdDictField* field = m_dict->find_field("subdet") ;
  if (field) {
    m_calo_index = field->m_index;}
  else {
    log << MSG::INFO  <<  "TTOnlineID::initLevelsFromDict - unable to find 'Calo' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] (4) field 'Calo' found OK "  << endmsg;


  field = m_dict->find_field("DetZside") ;
  if (field) {
    m_detzside_index = field->m_index ;}
  else {
    log << MSG::INFO  <<  "TTOnlineID::initLevelsFromDict - unable to find 'DetZside' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] (5) field 'DetZside' found OK "  << endmsg;


  field = m_dict->find_field("crate") ;
  if (field) {
    m_crate_index = field->m_index ;}
  else {
    log << MSG::INFO  <<  "TTOnlineID::initLevelsFromDict - unable to find 'crate' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] (6) field 'crate' found OK "  << endmsg;

  field = m_dict->find_field("module") ;
  if (field) {
    m_module_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "TTOnlineID::initLevelsFromDict - unable to find 'module' field "
              << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] (7) field 'module' found OK "  << endmsg;

  field = m_dict->find_field("submodule") ;
  if (field) {
    m_submodule_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "TTOnlineID::initLevelsFromDict - unable to find 'submodule' field " << endmsg ;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] (8) field 'submodule' found OK "  << endmsg;

  field = m_dict->find_field("channel") ;
  if (field) {
    m_channel_index = field->m_index ;
  }
  else {
    log << MSG::INFO  <<  "TTOnlineID::initLevelsFromDict - unable to find 'channel' field " << endmsg;
    return (1);
  }
  log << MSG::DEBUG  << "[initLevelsFromDict] (9) field 'channel' found OK "  << endmsg;


  // Set the field implementation
  // ------------------------------
  const IdDictRegion& region = *m_dict->m_regions[m_l1online_regions_index];
  log << MSG::DEBUG  << "[initLevelsFromDict] (10) found levels: " << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > subdet         " << m_calo_index           << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > detzside       " << m_detzside_index  << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > crate          " << m_crate_index          << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > module         " << m_module_index         << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > submodule      " << m_submodule_index      << endmsg ;
  log << MSG::DEBUG  << "[initLevelsFromDict] > channel        " << m_channel_index        << endmsg ;

  log << MSG::DEBUG  << "[initLevelsFromDict] > ...fields implementation... " << endmsg;
  log << MSG::DEBUG  << "[initLevelsFromDict] > ...implementation: m_calo_index " << endmsg;
  m_calo_impl          = region.m_implementation[m_calo_index];

  log << MSG::DEBUG  << "[initLevelsFromDict] > ...implementation: m_det_side_index " << endmsg;
  m_calo_detzside_impl = region.m_implementation[m_detzside_index];

  log << MSG::DEBUG  << "[initLevelsFromDict] > ...implementation: m_crate_index " << endmsg;
  m_crate_impl     = region.m_implementation[m_crate_index];

  log << MSG::DEBUG  << "[initLevelsFromDict] > ...implementation: m_module_index " << endmsg;
  m_module_impl     = region.m_implementation[m_module_index];
  log << MSG::DEBUG  << "[initLevelsFromDict] > ...implementation: m_submodule_index " << endmsg;
  m_submodule_impl   = region.m_implementation[m_submodule_index];
  log << MSG::DEBUG  << "[initLevelsFromDict] > ...implementation: m_channel_index " << endmsg;
  m_channel_impl   = region.m_implementation[m_channel_index];

  if (!m_quiet) {
    log << MSG::DEBUG  << "[initLevelsFromDict] (11) decode index and bit fields for each level: " << endmsg;
    log << MSG::DEBUG  << "[initLevelsFromDict] > calo          " << m_calo_impl.show_to_string() << endmsg;
    log << MSG::DEBUG  << "[initLevelsFromDict] > detzside  "  << m_calo_detzside_impl.show_to_string() << endmsg;
    log << MSG::DEBUG  << "[initLevelsFromDict] > crate         " << m_crate_impl.show_to_string()  << endmsg;
    log << MSG::DEBUG  << "[initLevelsFromDict] > module        " << m_module_impl.show_to_string() << endmsg;
    log << MSG::DEBUG  << "[initLevelsFromDict] > submodule     " << m_submodule_impl.show_to_string()  << endmsg;
    log << MSG::DEBUG  << "[initLevelsFromDict] > channel       " << m_channel_impl.show_to_string()  << endmsg;
  }

  return(0) ;
}


//=====================================================
int  TTOnlineID::init_hashes(void)
//=====================================================
{
  MsgStream log(m_msgSvc, "TTOnlineID" );
  std::stringstream strm1;
  std::stringstream strm2;
  std::stringstream strm3;
  std::string strg1;
  std::string strg2;
  std::string strg3;

  // Crate hash
  // -----------
	{
	  m_crateHashMax = m_full_crate_range.cardinality();
	  m_crate_vec.resize(m_crateHashMax);
	  unsigned int nids = 0;
	  std::set<HWIdentifier> ids;
	  for (unsigned int i = 0; i < m_full_crate_range.size(); ++i) {
	    const Range& range = m_full_crate_range[i];
	    Range::const_identifier_factory first = range.factory_begin();
	    Range::const_identifier_factory last  = range.factory_end();
	    for (; first != last; ++first) {
	      const ExpandedIdentifier& exp_id = (*first);
	      HWIdentifier cra_id = crateId( exp_id[m_crate_index] ) ;

	      if(!(ids.insert(cra_id)).second){
		strm1 << nids;
		strg1 = "[init_hashes] > duplicated id for channel nb = "+strm1.str();
		strm3 << show_to_string(cra_id);
		strg3 = " expanded Id= "+strm3.str();
		if(m_msgSvc){
		  log  << MSG::ERROR << strg1 << endmsg;
		  log  << MSG::ERROR << strg3 << endmsg;
		}
		else{
		  std::cout << "TTOnlineID::Error" << strg1 << std::endl;
		  std::cout << strg3 << std::endl;
		}
	      }
	      nids++;
	    }
	  }
	  if(ids.size() != m_crateHashMax) {
	    strm1 << ids.size();
	    strm2 << m_crateHashMax;
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
		std::cout << "TTOnlineID::" << strg1 << std::endl;
		std::cout << "Error" << strg2 << std::endl;
		std::cout << strg3 << std::endl;
	      }
	    return (1);
	  }

	  nids=0;
	  std::set<HWIdentifier>::const_iterator first = ids.begin();
	  std::set<HWIdentifier>::const_iterator last  = ids.end();
	  for (;first != last && nids < m_crate_vec.size(); ++first) {
	    m_crate_vec[nids] = (*first) ;
	    nids++;
	  }
	  log << MSG::DEBUG << "[init_hashes()] > Crate_size= " << m_crate_vec.size() << endmsg;
	}

  // Module hash
  // -----------
	{
	  m_moduleHashMax = m_full_module_range.cardinality();
	  m_module_vec.resize(m_moduleHashMax);
	  unsigned int nids = 0;
	  std::set<HWIdentifier> ids;
	  for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
	    const Range& range = m_full_module_range[i];
	    Range::const_identifier_factory first = range.factory_begin();
	    Range::const_identifier_factory last  = range.factory_end();
	    for (; first != last; ++first) {
	      const ExpandedIdentifier& exp_id = (*first);
	      HWIdentifier mod_id = moduleId(exp_id[m_crate_index], exp_id[m_module_index] ) ;

	      if(!(ids.insert(mod_id)).second){
		strm1 << nids;
		strg1 = "[init_hashes] > duplicated id for channel nb = "+strm1.str();
		strm3 << show_to_string(mod_id);
		strg3 = " expanded Id= "+strm3.str();
		if(m_msgSvc){
		  log  << MSG::ERROR << strg1 << endmsg;
		  log  << MSG::ERROR << strg3 << endmsg;
		}
		else{
		  std::cout << "TTOnlineID::Error" << strg1 << std::endl;
		  std::cout << strg3 << std::endl;
		}
	      }
	      nids++;
	    }
	  }
	  if(ids.size() != m_moduleHashMax) {
	    strm1 << ids.size();
	    strm2 << m_moduleHashMax;
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
		std::cout << "TTOnlineID::" << strg1 << std::endl;
		std::cout << "Error" << strg2 << std::endl;
		std::cout << strg3 << std::endl;
	      }
	    return (1);
	  }

	  nids=0;
	  std::set<HWIdentifier>::const_iterator first = ids.begin();
	  std::set<HWIdentifier>::const_iterator last  = ids.end();
	  for (;first != last && nids < m_module_vec.size(); ++first) {
	    m_module_vec[nids] = (*first) ;
	    nids++;
	  }
	  log << MSG::DEBUG << "[init_hashes()] > Module_size= " << m_module_vec.size() << endmsg;
	}

  // SubModule hash
  // -----------
	{
	  m_submoduleHashMax = m_full_submodule_range.cardinality();
	  m_submodule_vec.resize(m_submoduleHashMax);
	  unsigned int nids = 0;
	  std::set<HWIdentifier> ids;
	  for (unsigned int i = 0; i < m_full_submodule_range.size(); ++i) {
	    const Range& range = m_full_submodule_range[i];
	    Range::const_identifier_factory first = range.factory_begin();
	    Range::const_identifier_factory last  = range.factory_end();
	    for (; first != last; ++first) {
	      const ExpandedIdentifier& exp_id = (*first);
	      HWIdentifier submod_id = submoduleId(exp_id[m_crate_index], exp_id[m_module_index], exp_id[m_submodule_index] ) ;

	      if(!(ids.insert(submod_id)).second){
		strm1 << nids;
		strg1 = "[init_hashes] > duplicated id for channel nb = "+strm1.str();
		strm3 << show_to_string(submod_id);
		strg3 = " expanded Id= "+strm3.str();
		if(m_msgSvc){
		  log  << MSG::ERROR << strg1 << endmsg;
		  log  << MSG::ERROR << strg3 << endmsg;
		}
		else{
		  std::cout << "TTOnlineID::Error" << strg1 << std::endl;
		  //std::cout << strg2 << std::endl;
		  std::cout << strg3 << std::endl;
		}
	      }
	      nids++;
	    }
	  }
	  if(ids.size() != m_submoduleHashMax) {
	    strm1 << ids.size();
	    strm2 << m_submoduleHashMax;
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
		std::cout << "TTOnlineID::" << strg1 << std::endl;
		std::cout << "Error" << strg2 << std::endl;
		std::cout << strg3 << std::endl;
	      }
	    return (1);
	  }

	  nids=0;
	  std::set<HWIdentifier>::const_iterator first = ids.begin();
	  std::set<HWIdentifier>::const_iterator last  = ids.end();
	  for (;first != last && nids < m_submodule_vec.size(); ++first) {
	    m_submodule_vec[nids] = (*first) ;
	    nids++;
	  }
	  log << MSG::DEBUG << "[init_hashes()] > Submodule_size= " << m_submodule_vec.size() << endmsg;
	}

  // Channel hash
  // -----------
	{
	  m_channelHashMax = m_full_channel_range.cardinality();
	  m_channel_vec.resize(m_channelHashMax);
	  unsigned int nids = 0;
	  std::set<HWIdentifier> ids;
	  for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
	    const Range& range = m_full_channel_range[i];
	    Range::const_identifier_factory first = range.factory_begin();
	    Range::const_identifier_factory last  = range.factory_end();
	    for (; first != last; ++first) {
	      const ExpandedIdentifier& exp_id = (*first);
	      HWIdentifier chan_id = channelId(exp_id[m_crate_index], exp_id[m_module_index], exp_id[m_submodule_index], exp_id[m_channel_index]) ;

	      if(!(ids.insert(chan_id)).second){
		strm1 << nids;
		strg1 = "[init_hashes] > duplicated id for channel nb = "+strm1.str();
		strm3 << show_to_string(chan_id);
		strg3 = " expanded Id= "+strm3.str();
		if(m_msgSvc){
		  log  << MSG::ERROR << strg1 << endmsg;
		  log  << MSG::ERROR << strg3 << endmsg;
		}
		else{
		  std::cout << "TTOnlineID::Error" << strg1 << std::endl;
		  std::cout << strg3 << std::endl;
		}
	      }
	      nids++;
	    }
	  }
	  if(ids.size() != m_channelHashMax) {
	    strm1 << ids.size();
	    strm2 << m_channelHashMax;
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
		std::cout << "TTOnlineID::" << strg1 << std::endl;
		std::cout << "Error" << strg2 << std::endl;
		std::cout << strg3 << std::endl;
	      }
	    return (1);
	  }

	  nids=0;
	  std::set<HWIdentifier>::const_iterator first = ids.begin();
	  std::set<HWIdentifier>::const_iterator last  = ids.end();
	  for (;first != last && nids < m_channel_vec.size(); ++first) {
	    m_channel_vec[nids] = (*first) ;
	    nids++;
	  }
	  log << MSG::DEBUG << "[init_hashes()] > Channel_size= " << m_channel_vec.size() << endmsg;
	}
  return (0);
}



