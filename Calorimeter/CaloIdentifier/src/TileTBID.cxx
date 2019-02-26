/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Access to Tile Calorimeter TB data
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileTBID.cxx,v 1.1 2009-03-30 11:19:28 tsulaia Exp $
//<version>	$Name: not supported by cvs2svn $

#include "CaloIdentifier/TileTBID.h"
#include "IdDict/IdDictDefs.h"
#include "Identifier/IdentifierHash.h"
#include "CxxUtils/StrFormat.h"
#include "GaudiKernel/MsgStream.h"

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <assert.h>

using CxxUtils::strformat;


TileTBID::TileTBID( void ) 
  : m_tile_region_index(0)
    , m_SYSTEM_INDEX(999)
    , m_SECTION_INDEX(999)
    , m_TYPE_INDEX(999)
    , m_MODULE_INDEX(999)
    , m_CHANNEL_INDEX(999)
    , m_dict(0)
    , m_type_hash_max(0)
    , m_module_hash_max(0)
    , m_channel_hash_max(0)
{
}

TileTBID::~TileTBID(void) 
{
}

//
// TileTBID methods
//
std::string
TileTBID::to_string( const Identifier & id, int level ) const
{
  IdContext context;
  switch (level) {
    case 1: case -3:
    case 2: case -2: context = type_context(); break;
    case 3: case -1: context = module_context(); break;
    case 4: case  0: context = channel_context(); break;
  default:
    return "TileTBID::to_string - unknown context";
  }    
      
  ExpandedIdentifier exp_id;
  if ( get_expanded_id(id,exp_id,&context) ) {
    return "TileTBID::to_string - can not expand ID";
  } else {
    return (std::string)(exp_id);
  }  
}

//
// get methods
//
int
TileTBID::get_field ( const Identifier & id, size_type index ) const
{
  int field = 0;
  if(!m_dict->unpack(id, 0, index, m_tile_region_index, field)) {
    return field;
  } else {
    return 0;
  }
}

//
// check methods
//
bool               
TileTBID::is_tiletb (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( section(id) == TileTBID::TILE_TESTBEAM );
    
    return result;
}

//
// TileCal TB ID
//
Identifier
TileTBID::tiletb_id     ()      const
{
    Identifier compactID(m_base_tile_type);
    return compactID;
}

//
// Build type & module id
//
Identifier
TileTBID::type_id       ( int type )      const
{
#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << TileTBID::TILE_TESTBEAM << type;

	if (!m_full_type_range.match(expId)) { 
	    std::string errorMessage = "TileTBID::type_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_type_range;
	    throw TileID_Exception(errorMessage , 2);
	}
    }
#endif

    Identifier compactID(m_base_tile_type);
    m_type_impl.pack	(type,compactID);

    return (compactID);
}

Identifier
TileTBID::module_id       ( int type, int module )       const
{
#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << TileTBID::TILE_TESTBEAM << type << module;

	if (!m_full_module_range.match(expId)) { 
	    std::string errorMessage = "TileTBID::module_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_type_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }
#endif

    Identifier compactID(m_base_tile_type);
    m_type_impl.pack	(type,compactID);
    m_module_impl.pack	(module,compactID);

    return (compactID);
}

//
// Build channel id
//
Identifier
TileTBID::channel_id ( int type, int module, int channel ) const
{
#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << TileTBID::TILE_TESTBEAM << type << module << channel;

	if (!m_full_channel_range.match(expId)) { 
	    std::string errorMessage = "TileTBID::channel_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_type_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }
#endif

    Identifier compactID(m_base_tile_type);
    m_type_impl.pack	(type,compactID);
    m_module_impl.pack	(module,compactID);
    m_channel_impl.pack	(channel,compactID);

    return (compactID);
}

Identifier
TileTBID::module_id     ( const Identifier & any_id )   const
{
  Identifier compactId(any_id);
  m_channel_impl.reset(compactId);
  
  return (compactId);
}

Identifier
TileTBID::channel_id    ( const Identifier & module_id, 
                          int channel )                     const
{
    Identifier compactId(module_id);
    m_channel_impl.pack(channel,compactId);

#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId;

	IdContext context = module_context();
	if (get_expanded_id(compactId, expId, &context)) {
	    std::string errorMessage = "TileTBID::channel_id(module_id,channel) result is not OK: ID = "
		+ compactId.getString() ;
	    throw TileID_Exception(errorMessage , 1);

	    //      region_id.show();
	}

	expId << channel;

	if (!m_full_channel_range.match(expId)) { 
	    std::string errorMessage = "TileTBID::channel_id() result is not OK: ID, range = "
		+ (std::string)expId + " , " + (std::string)m_full_type_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }
#endif

    return compactId;
}

bool TileTBID::module_id  ( const Identifier& type_id,
                          int module, Identifier& module_id ) 
{
  ExpandedIdentifier id; 
  IdContext context = type_context();
  get_expanded_id(type_id, id, &context);
  id << module;
  if (!m_full_module_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!m_dict->pack32(id,0, id.fields() - 1,compactId)) {
    module_id = compactId;   
  }
  return true;
}

bool TileTBID::channel_id       ( const Identifier& type_id,
                          int module, int channel, Identifier& channel_id ) 
{
  ExpandedIdentifier id; 
  IdContext context = type_context();
  get_expanded_id(type_id, id, &context);
  id << module << channel;
  if (!m_full_channel_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!m_dict->pack32(id,0, id.fields() - 1,compactId)) {
    channel_id = compactId;   
  }
  return true;
}

bool TileTBID::channel_id       ( const Identifier& module_id,
                                  int channel, Identifier& channel_id ) 
{
  ExpandedIdentifier id; 
  IdContext context = module_context();
  get_expanded_id(module_id, id, &context);
  id << channel;
  if (!m_full_channel_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!m_dict->pack32(id,0, id.fields() - 1,compactId)) {
    channel_id = compactId;   
  }
  return true;
}

IdContext	
TileTBID::type_context  (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_TYPE_INDEX));
}

IdContext	
TileTBID::module_context  (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_MODULE_INDEX));
}

IdContext	
TileTBID::channel_context     (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_CHANNEL_INDEX));
}

int  TileTBID::get_id     (const IdentifierHash& hash_id, Identifier& id, const IdContext* context ) const
{
  int result = 1;
  id.clear();

  size_t begin = (context) ? context->begin_index(): 0;
  // cannot get hash if end is 0:
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) {

    if ( m_CHANNEL_INDEX == end ) {
      if (hash_id < (unsigned int)(m_channel_vec.end() - m_channel_vec.begin())) {
	id = m_channel_vec[hash_id];
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "TileTBID" );
	log << MSG::ERROR << "get_id: channel hash_id is out of range " << hash_id
            << " => " << m_channel_vec.size() << endmsg;
      }
    } else if ( m_MODULE_INDEX == end ) {
      if (hash_id < (unsigned int)(m_module_vec.end() - m_module_vec.begin())) {
	id = m_module_vec[hash_id];
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "TileTBID" );
	log << MSG::ERROR << "get_id: module hash_id is out of range "
            << hash_id << " => " << m_module_vec.size() << endmsg;
      }
    } else if ( m_TYPE_INDEX == end ) {
      if (hash_id < (unsigned int)(m_type_vec.end() - m_type_vec.begin())) {
	id = m_type_vec[hash_id];
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "TileTBID" );
	log << MSG::ERROR << "get_id: TYPE hash_id is out of range " << hash_id
            << " => " << m_type_vec.size() << endmsg;
      }
    }
  }
  return(result);
}


int  TileTBID::get_hash  (const Identifier& id, IdentifierHash& hash_id, const IdContext* context ) const
{
    
  hash_id = 0;
  int result = 1;

  size_t begin = (context) ? context->begin_index(): 0;
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) { 
    if ( m_CHANNEL_INDEX == end ) {
      std::vector<Identifier>::const_iterator it = std::lower_bound(m_channel_vec.begin(),m_channel_vec.end(),id);
      if ( it != m_channel_vec.end() ){
	hash_id = it - m_channel_vec.begin();
	result = 0;
      }
    } else if ( m_MODULE_INDEX == end ) {
      std::vector<Identifier>::const_iterator it = std::lower_bound(m_module_vec.begin(),m_module_vec.end(),id);
      if ( it != m_module_vec.end() ){
	hash_id = it - m_module_vec.begin();
	result = 0;
      }
    } else if ( m_TYPE_INDEX == end ) {
      std::vector<Identifier>::const_iterator it = std::lower_bound(m_type_vec.begin(),m_type_vec.end(),id);
      if ( it != m_type_vec.end() ){
	hash_id = it - m_type_vec.begin();
	result = 0;
      }
    }    
  }

  return (result);
}

int  TileTBID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
{
  MsgStream log(m_msgSvc, "TileTBID" );
  log << MSG::INFO << "initialize_from_dictionary " << endmsg;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  log << MSG::DEBUG << "calling base initialize_from_dictionary" << endmsg;

  // init base object
  if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

  // Register version of the TileCalo dictionary
  if (register_dict_tag(dict_mgr, "TileCalorimeter")) return(1);

  m_dict = dict_mgr.find_dictionary ("TileCalorimeter"); 
  if(!m_dict) {
    log << MSG::ERROR << "cannot access TileCalorimeter dictionary " << endmsg;
    return 1;
  }
  
  // Initialize the field indices
  if(initLevelsFromDict()) return (1);


  // Find value for the field Tile Calorimeter   
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
  int tileField   = -1;
  if (atlasDict->get_label_value("subdet", "TileCalorimeter", tileField)) {
    log << MSG::ERROR << "Could not get value for label 'TileCalorimeter' of field 'subdet' in dictionary " 
        << atlasDict->m_name
        << endmsg;
    return (1);
  }

  // Find value for the field Tile Testbeam
  int tiletbField   = -1;
  if (m_dict->get_label_value("section", "Testbeam", tiletbField)) {
    log << MSG::ERROR << "Could not get value for label 'Testbeam' of field 'section' in dictionary " 
        << m_dict->m_name
        << endmsg;
    return (1);
  }

  /*
  log << MSG::DEBUG << "TileTB_ID::initialize_from_dict - found field values: TileTB " 
      << TileTBField
      << endmsg;
  */

  // Set up id for region and range prefix
  ExpandedIdentifier reg_id;
  reg_id.add(tileField);
  reg_id.add(tiletbField);
  Range prefix;

  m_full_channel_range= m_dict->build_multirange(reg_id, prefix, "tbchannel");
  m_full_module_range = m_dict->build_multirange(reg_id, prefix, "tbmodule");
  m_full_type_range   = m_dict->build_multirange(reg_id, prefix, "type");

  if (!m_quiet) {
    log << MSG::DEBUG << "initialize_from_dict : "                                 << endmsg;
    log << MSG::DEBUG << " type range -> "    << (std::string)m_full_type_range    << endmsg;
    log << MSG::DEBUG << " module range -> "  << (std::string)m_full_module_range  << endmsg;
    log << MSG::DEBUG << " channel range -> " << (std::string)m_full_channel_range << endmsg;
  }

  // Setup the hash tables
  if(init_hashes()) return (1);

  return 0;

}

int TileTBID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
  int result = 1;

  size_t begin = (context) ? context->begin_index() : 0;
  size_t end   = (context) ? context->end_index()   : m_CHANNEL_INDEX;
  assert (end <= m_CHANNEL_INDEX);

  if (0 == end) {
    result = 0;
  }
  else if ( 0 == begin) {
    ExpandedIdentifier empty;
    result = m_dict->unpack(id, empty, end, exp_id);
  }
  else {
    result = m_dict->unpack(id, context->prefix_id(), end, exp_id);
  }
  return result;
}

int TileTBID::initLevelsFromDict(void) 
{
  MsgStream log(m_msgSvc, "TileTBID" );

  if(!m_dict) {
    log << MSG::ERROR << "initLevelsFromDict - dictionary NOT initialized "
        << endmsg ;
    return (1);
  }

  // Find out which identifier field corresponds to each level.

  m_SYSTEM_INDEX        = 999;
  m_SECTION_INDEX       = 999;
  m_TYPE_INDEX          = 999;
  m_MODULE_INDEX        = 999;
  m_CHANNEL_INDEX       = 999;

  // Save index to a Tile region for unpacking
  ExpandedIdentifier expId(tile_exp());
  if (m_dict->find_region(expId,m_tile_region_index)){
    log << MSG::ERROR << "initLevelsFromDict - unable to find tile region index: expId, reg "  
        << (std::string)expId << " " << m_tile_region_index
        << endmsg;
    return (1);
  }

  // Fing a Tile region
  IdDictField* field = m_dict->find_field("subdet") ;
  if (field) {
    m_SYSTEM_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'subdet' field "
        << endmsg ;
    return (1);
  }

  field = m_dict->find_field("section") ;
  if (field) {
    m_SECTION_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'section' field "
        << endmsg ;
    return (1);
  }

  field = m_dict->find_field("type") ;
  if (field) {
    m_TYPE_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'type' field "
        << endmsg ;
    return (1);
  }

  field = m_dict->find_field("tbmodule") ;
  if (field) {
    m_MODULE_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'tbmodule' field "
        << endmsg ;
    return (1);
  }

  field = m_dict->find_field("tbchannel") ;
  if (field) {
    m_CHANNEL_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'tbchannel' field "
        << endmsg ;
    return (1);
  }

   /* Set the field implementations */
  const IdDictRegion& region = *m_dict->m_regions[m_tile_region_index];

  m_system_impl  = region.m_implementation[m_SYSTEM_INDEX]; 
  m_section_impl = region.m_implementation[m_SECTION_INDEX]; 
  m_type_impl    = region.m_implementation[m_TYPE_INDEX]; 
  m_module_impl  = region.m_implementation[m_MODULE_INDEX]; 
  m_channel_impl = region.m_implementation[m_CHANNEL_INDEX]; 

  m_base_tile_type=Identifier (0);
  m_system_impl.pack  (tile_field_value(),m_base_tile_type);
  m_section_impl.pack (TileTBID::TILE_TESTBEAM,m_base_tile_type);

  if (!m_quiet) {
    log << MSG::DEBUG << "initLevelsFromDict decode index and bit fields for each level: "              << endmsg ;
    log << MSG::DEBUG << " system   [" << m_SYSTEM_INDEX   << "]  " << m_system_impl.show_to_string()   << endmsg ;
    log << MSG::DEBUG << " section  [" << m_SECTION_INDEX  << "]  " << m_section_impl.show_to_string()  << endmsg ;
    log << MSG::DEBUG << " type     [" << m_TYPE_INDEX     << "]  " << m_type_impl.show_to_string()     << endmsg ;
    log << MSG::DEBUG << " module   [" << m_MODULE_INDEX   << "]  " << m_module_impl.show_to_string()   << endmsg ;
    log << MSG::DEBUG << " channel  [" << m_CHANNEL_INDEX  << "]  " << m_channel_impl.show_to_string()  << endmsg ;
  }

  return(0) ;
}

int TileTBID::init_hashes(void) 
{
  MsgStream log(m_msgSvc, "TileTBID" );

  // type hash
  unsigned int nids = 0;
  std::set<Identifier> ids;
  for (unsigned int i = 0; i < m_full_type_range.size(); ++i) {
    const Range& range = m_full_type_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      Identifier id = type_id (exp_id[m_TYPE_INDEX]);
      if(!(ids.insert(id)).second){
	log << MSG::ERROR << "init_hashes "
            << " Error: duplicated id for type id. nids= " << nids
            << " compact Id  " << show_to_string(id)
            << endmsg;
      }
      nids++;
    }
  }
  m_type_hash_max = m_full_type_range.cardinality();
  if ( fill_vec(ids, m_type_hash_max, m_type_vec) ) return (1);

  // module hash
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
    const Range& range = m_full_module_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      Identifier id = module_id (exp_id[m_TYPE_INDEX], 
                                 exp_id[m_MODULE_INDEX]);
      if(!(ids.insert(id)).second){
	log << MSG::ERROR << "init_hashes "
            << " Error: duplicated id for module id. nids= " << nids
            << " compact Id  " << show_to_string(id)
            << endmsg;
      }
      nids++;
    }
  }
  m_module_hash_max = m_full_module_range.cardinality();
  if ( fill_vec(ids, m_module_hash_max, m_module_vec) ) return (1);

  // channel hash
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
    const Range& range = m_full_channel_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      Identifier id = channel_id    (exp_id[m_TYPE_INDEX], 
                                 exp_id[m_MODULE_INDEX], 
                                 exp_id[m_CHANNEL_INDEX]);
      if(!(ids.insert(id)).second){
	log << MSG::ERROR << "init_hashes "
            << " Error: duplicated id for channel id. nids= " << nids
            << " compact Id  " << show_to_string(id)
            << endmsg;
      }
      nids++;
    }
  }
  m_channel_hash_max = m_full_channel_range.cardinality();
  if ( fill_vec(ids, m_channel_hash_max, m_channel_vec) ) return (1);

  return (0);
}

int TileTBID::fill_vec  (std::set<Identifier> & ids, 
                         unsigned int hash_max, std::vector<Identifier> & vec)
{

  if(ids.size() != hash_max) {

    MsgStream log(m_msgSvc, "TileTBID" );
    log << MSG::ERROR << "fill_vec "
        << " Error: set size NOT EQUAL to hash max. size " << ids.size()
        << " hash max " << hash_max
        << endmsg;

    return (1);
  }

  vec.resize(hash_max);

  std::set<Identifier>::const_iterator first = ids.begin();
  std::set<Identifier>::const_iterator last  = ids.end();

  int index = 0;
  for (;first != last; ++first) {
    vec[index++] = (*first);
  }

  return (0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

