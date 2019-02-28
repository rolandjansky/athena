/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Access to Tile Calorimeter raw data
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileHWID.cxx,v 1.8 2008-02-15 18:16:33 solodkov Exp $
//<version>	$Name: not supported by cvs2svn $

#include "TileIdentifier/TileHWID.h"
#include "IdDict/IdDictDefs.h"
#include "Identifier/IdentifierHash.h"

#include "GaudiKernel/MsgStream.h"

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <assert.h>


TileHWID::TileHWID( void ) 
  : m_tile_region_index(0)
    , m_SYSTEM_INDEX(999)
    , m_SECTION_INDEX(999)
    , m_ROS_INDEX(999)
    , m_DRAWER_INDEX(999)
    , m_CHANNEL_INDEX(999)
    , m_ADC_INDEX(999)
    , m_dict(0)
    , m_ros_hash_max(0)
    , m_drawer_hash_max(0)
    , m_channel_hash_max(0)
    , m_adc_hash_max(0)
{
}

TileHWID::~TileHWID(void) 
{
}

//
// TileHWID methods
//
std::string
TileHWID::to_string( const HWIdentifier & id, int level ) const
{
  IdContext context;
  switch (level) {
    case 1: case -4:
    case 2: case -3: context = ros_context(); break;
    case 3: case -2: context = drawer_context(); break;
    case 4: case -1: context = channel_context(); break;
    case 5: case  0: context = adc_context(); break;
  default:
    return "TileHWID::to_string - unknown context";
  }    
      
  ExpandedIdentifier exp_id;
  if ( get_expanded_id(id,exp_id,&context) ) {
    return "TileHWID::to_string - can not expand ID";
  } else {
    return (std::string)(exp_id);
  }  
}

//
// get methods
//
int
TileHWID::get_field ( const HWIdentifier & id, size_type index ) const
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
TileHWID::is_tilehw (const HWIdentifier& id) const
{
    bool result = is_tile(id)
      && ( section(id) == TileHWID::TILE_ONLINE );
    
    return result;
}

bool
TileHWID::is_low_gain (const HWIdentifier& id) const
{
    bool result = is_tilehw(id)
      && ( adc(id) == TileHWID::LOWGAIN );

    return result;
}

bool
TileHWID::is_high_gain (const HWIdentifier& id) const
{
    bool result = is_tilehw(id)
      && ( adc(id) == TileHWID::HIGHGAIN );

    return result;
}

//
// TileCal HW ID
//
HWIdentifier
TileHWID::tilehw_id     ()      const
{
        
    HWIdentifier compactID(m_base_tile_ros);
    return compactID;
}

//
// Build ros & drawer id
//
HWIdentifier
TileHWID::ros_id       ( int ros )      const
{

    HWIdentifier compactID(m_base_tile_ros);
    m_ros_impl.pack	(ros,compactID);

#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << TileHWID::TILE_ONLINE << ros;

	if (!m_full_ros_range.match(expId)) { 
	    std::string errorMessage = "TileHWID::ros_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_ros_range;
	    TileID_Exception except(errorMessage , 2);
	    throw except ;   
	}
    }
#endif

    return (compactID);
}

HWIdentifier
TileHWID::drawer_id       ( int ros, int drawer )       const
{

    HWIdentifier compactID(m_base_tile_ros);
    m_ros_impl.pack	(ros,compactID);
    m_drawer_impl.pack	(drawer,compactID);

#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << TileHWID::TILE_ONLINE << ros << drawer;

	if (!m_full_drawer_range.match(expId)) { 
	    std::string errorMessage = "TileHWID::drawer_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_ros_range;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   
	}
    }
#endif

    return (compactID);
}

HWIdentifier
TileHWID::drawer_id ( int frag ) const
{
    int ros = (frag >> 8) & 0xFF;
    int drawer = frag & 0xFF;
  
    return drawer_id (ros, drawer);
}

//
// Build channel & adc id
//
HWIdentifier
TileHWID::channel_id ( int ros, int drawer, int channel ) const
{

    HWIdentifier compactID(m_base_tile_ros);
    m_ros_impl.pack	(ros,compactID);
    m_drawer_impl.pack	(drawer,compactID);
    m_channel_impl.pack	(channel,compactID);

#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << TileHWID::TILE_ONLINE << ros << drawer << channel;

	if (!m_full_channel_range.match(expId)) { 
	    std::string errorMessage = "TileHWID::channel_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_ros_range;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   
	}
    }
#endif

    return (compactID);
}

HWIdentifier
TileHWID::adc_id ( int ros, int drawer, int channel, int adc ) const
{
        
    HWIdentifier compactID(m_base_tile_ros);
    m_ros_impl.pack	(ros,compactID);
    m_drawer_impl.pack	(drawer,compactID);
    m_channel_impl.pack	(channel,compactID);
    m_adc_impl.pack	(adc,compactID);

#ifdef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << TileHWID::TILE_ONLINE << ros << drawer << channel << adc;

	if (!m_full_adc_range.match(expId)) { 
	    std::string errorMessage = "TileHWID::adc_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_ros_range;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   
	}
    }
#endif

    return (compactID);
}

HWIdentifier
TileHWID::drawer_id     ( const HWIdentifier & any_id )   const
{
  HWIdentifier compactId(any_id);
  m_channel_impl.reset(compactId);
  m_adc_impl.reset(compactId);
  
  return (compactId);
}

HWIdentifier
TileHWID::channel_id     ( const HWIdentifier & any_id )   const
{
  HWIdentifier compactId(any_id);
  m_adc_impl.reset(compactId);
  return (compactId);
}

HWIdentifier
TileHWID::channel_id    ( const HWIdentifier & drawer_id, 
                          int channel )                     const
{
        
    HWIdentifier compactId(drawer_id);
    m_channel_impl.pack(channel,compactId);

#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId;

	IdContext context = drawer_context();
	if (get_expanded_id(compactId, expId, &context)) {
	    std::string errorMessage = "TileHWID::channel_id(drawer_id,channel) result is not OK: ID = "
		+ compactId.getString() ;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   

	    //      region_id.show();
	}

	expId << channel;

	if (!m_full_channel_range.match(expId)) { 
	    std::string errorMessage = "TileHWID::channel_id() result is not OK: ID, range = "
		+ (std::string)expId + " , " + (std::string)m_full_ros_range;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   
	}
    }
#endif

    return compactId;
}

HWIdentifier
TileHWID::adc_id        ( const HWIdentifier & channel_id,
                          int adc )                     const
{
        
    HWIdentifier compactId(channel_id);
    m_adc_impl.pack(adc,compactId);

#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId;

	IdContext context = channel_context();
	if (get_expanded_id(compactId, expId, &context)) {
	    std::string errorMessage = "TileHWID::adc_id(channel_id,adc) result is not OK: ID = "
		+ compactId.getString() ;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   

	    //      region_id.show();
	}

	expId << adc;

	if (!m_full_adc_range.match(expId)) { 
	    std::string errorMessage = "TileHWID::adc_id() result is not OK: ID, range = "
		+ (std::string)expId + " , " + (std::string)m_full_ros_range;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   
	}
    }
#endif

    return compactId;
}

HWIdentifier
TileHWID::adc_id        ( const HWIdentifier & drawer_id,
                          int channel, int adc )                const
{
        
    HWIdentifier compactId(drawer_id);
    m_channel_impl.pack(channel,compactId);
    m_adc_impl.pack(adc,compactId);

#ifndef NDEBUG
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId;

	IdContext context = drawer_context();
	if (get_expanded_id(compactId, expId, &context)) {
	    std::string errorMessage = "TileHWID::adc_id(drawer_id,channel,adc) result is not OK: ID = "
		+ compactId.getString() ;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   

	    //      region_id.show();
	}

	expId << channel << adc;

	if (!m_full_adc_range.match(expId)) { 
	    std::string errorMessage = "TileHWID::adc_id() result is not OK: ID, range = "
		+ (std::string)expId + " , " + (std::string)m_full_ros_range;
	    TileID_Exception except(errorMessage , 1);
	    throw except ;   
	}
    }
#endif

    return compactId;
}

bool TileHWID::drawer_id  ( const HWIdentifier& ros_id,
                          int drawer, HWIdentifier& drawer_id ) 
{
  ExpandedIdentifier id; 
  IdContext context = ros_context();
  get_expanded_id(ros_id, id, &context);
  id << drawer;
  if (!m_full_drawer_range.match(id)) { 
    return false;
  }

  drawer_id=HWIdentifier(ros_id);
  m_drawer_impl.pack(drawer,drawer_id);
  return true;
}

bool TileHWID::channel_id       ( const HWIdentifier& ros_id,
                          int drawer, int channel, HWIdentifier& channel_id ) 
{
  ExpandedIdentifier id; 
  IdContext context = ros_context();
  get_expanded_id(ros_id, id, &context);
  id << drawer << channel;
  if (!m_full_channel_range.match(id)) { 
    return false;
  }

  channel_id=HWIdentifier(ros_id);
  m_drawer_impl.pack(drawer,channel_id);
  m_channel_impl.pack(channel,channel_id);
  return true;
}

bool TileHWID::channel_id       ( const HWIdentifier& drawer_id,
                                  int channel, HWIdentifier& channel_id ) 
{
  ExpandedIdentifier id; 
  IdContext context = drawer_context();
  get_expanded_id(drawer_id, id, &context);
  id << channel;
  if (!m_full_channel_range.match(id)) { 
    return false;
  }

  channel_id=HWIdentifier(drawer_id);
  m_channel_impl.pack(channel,channel_id);
  return true;
}

bool TileHWID::adc_id     ( const HWIdentifier& channel_id,
                          int adc, HWIdentifier& adc_id ) 
{
  ExpandedIdentifier id; 
  IdContext context = channel_context();
  get_expanded_id(channel_id, id, &context);
  id << adc;
  if (!m_full_adc_range.match(id)) { 
    return false;
  }

  adc_id=HWIdentifier(channel_id);
  m_adc_impl.pack(adc,adc_id);
  return true;
}

IdContext	
TileHWID::ros_context  (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_ROS_INDEX));
}

IdContext	
TileHWID::drawer_context  (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_DRAWER_INDEX));
}

IdContext	
TileHWID::channel_context     (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_CHANNEL_INDEX));
}

IdContext	
TileHWID::adc_context     (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_ADC_INDEX));
}

int  TileHWID::get_id     (const IdentifierHash& hash_id, HWIdentifier& id, const IdContext* context ) const
{
  int result = 1;
  id.clear();

  size_t begin = (context) ? context->begin_index(): 0;
  // cannot get hash if end is 0:
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) {

    if ( m_ADC_INDEX == end ) {
      if (hash_id < (unsigned int)(m_adc_vec.end() - m_adc_vec.begin())) {
	id = m_adc_vec[hash_id];
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "TileHWID" );
	log << MSG::ERROR << "get_id: hwadc hash_id out of range " << hash_id
            << " => " << m_adc_vec.size() << endmsg;
      }
    } else if ( m_CHANNEL_INDEX == end ) {
      if (hash_id < (unsigned int)(m_channel_vec.end() - m_channel_vec.begin())) {
	id = m_channel_vec[hash_id];
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "TileHWID" );
	log << MSG::ERROR << "get_id: channel hash_id is out of range "
            << hash_id << " => " << m_channel_vec.size() << endmsg;
      }
    } else if ( m_DRAWER_INDEX == end ) {
      if (hash_id < (unsigned int)(m_drawer_vec.end() - m_drawer_vec.begin())) {
	id = m_drawer_vec[hash_id];
	result = 0;
      } else {
        MsgStream log(m_msgSvc, "TileHWID" );
	log << MSG::ERROR << "get_id: drawer hash_id is out of range " << hash_id
            << " => " << m_drawer_vec.size() << endmsg;
      }
    } else if ( m_ROS_INDEX == end ) {
      if (hash_id < (unsigned int)(m_ros_vec.end() - m_ros_vec.begin())) {
	id = m_ros_vec[hash_id];
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "TileHWID" );
	log << MSG::ERROR << "get_id: ROS hash_id is out of range " << hash_id
            << " => " << m_ros_vec.size() << endmsg;
      }
    }
  }
  return(result);
}

// for fast retrieval do not perform error checking
IdentifierHash  TileHWID::get_hash  (const HWIdentifier& id) const
{
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_adc_vec.begin(),m_adc_vec.end(),id);
  return (IdentifierHash)(it - m_adc_vec.begin());
}

int  TileHWID::get_hash  (const HWIdentifier& id, IdentifierHash& hash_id, const IdContext* context ) const
{
    
  hash_id = 0;
  int result = 1;

  size_t begin = (context) ? context->begin_index(): 0;
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) { 
    if ( m_ADC_INDEX == end ) {
      std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_adc_vec.begin(),m_adc_vec.end(),id);
      if ( it != m_adc_vec.end() ){
	hash_id = it - m_adc_vec.begin();
	result = 0;
      }
    } else if ( m_CHANNEL_INDEX == end ) {
      std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_channel_vec.begin(),m_channel_vec.end(),id);
      if ( it != m_channel_vec.end() ){
	hash_id = it - m_channel_vec.begin();
	result = 0;
      }
    } else if ( m_DRAWER_INDEX == end ) {
      std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_drawer_vec.begin(),m_drawer_vec.end(),id);
      if ( it != m_drawer_vec.end() ){
	hash_id = it - m_drawer_vec.begin();
	result = 0;
      }
    } else if ( m_ROS_INDEX == end ) {
      std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_ros_vec.begin(),m_ros_vec.end(),id);
      if ( it != m_ros_vec.end() ){
	hash_id = it - m_ros_vec.begin();
	result = 0;
      }
    }
  }

  return (result);
}

IdentifierHash  TileHWID::get_channel_hash(const HWIdentifier& id) const
{
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_channel_vec.begin(),m_channel_vec.end(),id);
  if ( it != m_channel_vec.end() ){
    return (it - m_channel_vec.begin());
  } else {
    return NOT_VALID_HASH;
  }
}


int  TileHWID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
{
  MsgStream log(m_msgSvc, "TileHWID" );
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
    log << MSG::ERROR << "cannot access TileCalorimeter dictionary" << endmsg;
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

  // Find value for the field Tile Online
  int tilehwField   = -1;
  if (m_dict->get_label_value("section", "Online", tilehwField)) {
    log << MSG::ERROR << "Could not get value for label 'Online' of field 'section' in dictionary " 
        << m_dict->m_name
        << endmsg;
    return (1);
  }

  /*
  log << MSG::DEBUG << "initialize_from_dict - found field values: TileHW " 
      << TileHWField
      << endmsg;
  */

  // Set up id for region and range prefix
  ExpandedIdentifier reg_id;
  reg_id.add(tileField);
  reg_id.add(tilehwField);
  Range prefix;

  m_full_adc_range    = m_dict->build_multirange(reg_id, prefix, "gain");
  m_full_channel_range= m_dict->build_multirange(reg_id, prefix, "channel");
  m_full_drawer_range = m_dict->build_multirange(reg_id, prefix, "drawer");
  m_full_ros_range    = m_dict->build_multirange(reg_id, prefix, "ros");

  log << MSG::DEBUG << "initialize_from_dict : "                                 << endmsg;
  log << MSG::DEBUG << " ros range -> "     << (std::string)m_full_ros_range     << endmsg;
  log << MSG::DEBUG << " drawer range -> "  << (std::string)m_full_drawer_range  << endmsg;
  log << MSG::DEBUG << " channel range -> " << (std::string)m_full_channel_range << endmsg;
  log << MSG::DEBUG << " adc range -> "     << (std::string)m_full_adc_range     << endmsg;

  // Setup the hash tables
  if(init_hashes()) return (1);

  return 0;

}

int TileHWID::get_expanded_id  (const HWIdentifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
  int result = 1;

  size_t begin = (context) ? context->begin_index() : 0;
  size_t end   = (context) ? context->end_index()   : m_ADC_INDEX;
  assert (end <= m_ADC_INDEX);

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

int TileHWID::initLevelsFromDict(void) 
{
  MsgStream log(m_msgSvc, "TileHWID" );

  if(!m_dict) {
    log << MSG::ERROR << "initLevelsFromDict - dictionary NOT initialized "
        << endmsg ;
    return (1);
  }

  // Find out which identifier field corresponds to each level.

  m_SYSTEM_INDEX        = 999;
  m_SECTION_INDEX       = 999;
  m_ROS_INDEX           = 999;
  m_DRAWER_INDEX        = 999;
  m_CHANNEL_INDEX       = 999;
  m_ADC_INDEX           = 999;

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

  field = m_dict->find_field("ros") ;
  if (field) {
    m_ROS_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'ros' field "
        << endmsg ;
    return (1);
  }

  field = m_dict->find_field("drawer") ;
  if (field) {
    m_DRAWER_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'drawer' field "
        << endmsg ;
    return (1);
  }

  field = m_dict->find_field("channel") ;
  if (field) {
    m_CHANNEL_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'channel' field "
        << endmsg ;
    return (1);
  }

  field = m_dict->find_field("gain") ;
  if (field) {
    m_ADC_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'gain' field "
        << endmsg ;
    return (1);
  }

   /* Set the field implementations */
  const IdDictRegion& region = *m_dict->m_regions[m_tile_region_index];

  m_system_impl  = region.m_implementation[m_SYSTEM_INDEX]; 
  m_section_impl = region.m_implementation[m_SECTION_INDEX]; 
  m_ros_impl     = region.m_implementation[m_ROS_INDEX]; 
  m_drawer_impl  = region.m_implementation[m_DRAWER_INDEX]; 
  m_channel_impl = region.m_implementation[m_CHANNEL_INDEX]; 
  m_adc_impl     = region.m_implementation[m_ADC_INDEX]; 

  m_base_tile_ros=HWIdentifier (0);
  m_system_impl.pack  (tile_field_value(),m_base_tile_ros);
  m_section_impl.pack (TileHWID::TILE_ONLINE,m_base_tile_ros);

  log << MSG::DEBUG << "initLevelsFromDict decode index and bit fields for each level: "              << endmsg ;
  log << MSG::DEBUG << " system   [" << m_SYSTEM_INDEX   << "]  " << m_system_impl.show_to_string()   << endmsg ;
  log << MSG::DEBUG << " section  [" << m_SECTION_INDEX  << "]  " << m_section_impl.show_to_string()  << endmsg ;
  log << MSG::DEBUG << " ros      [" << m_ROS_INDEX      << "]  " << m_ros_impl.show_to_string()      << endmsg ;
  log << MSG::DEBUG << " drawer   [" << m_DRAWER_INDEX   << "]  " << m_drawer_impl.show_to_string()   << endmsg ;
  log << MSG::DEBUG << " channel  [" << m_CHANNEL_INDEX  << "]  " << m_channel_impl.show_to_string()  << endmsg ;
  log << MSG::DEBUG << " adc      [" << m_ADC_INDEX      << "]  " << m_adc_impl.show_to_string()      << endmsg ;

  return(0) ;
}

int TileHWID::init_hashes(void) 
{
  MsgStream log(m_msgSvc, "TileHWID" );

  // ros hash
  unsigned int nids = 0;
  std::set<HWIdentifier> ids;
  for (unsigned int i = 0; i < m_full_ros_range.size(); ++i) {
    const Range& range = m_full_ros_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      HWIdentifier id = ros_id (exp_id[m_ROS_INDEX]);
      if(!(ids.insert(id)).second){
	log << MSG::ERROR << "init_hashes "
            << " Error: duplicated id for ros id. nids= " << nids
            << " compact Id  " << show_to_string(id)
            << endmsg;
      }
      nids++;
    }
  }
  m_ros_hash_max = m_full_ros_range.cardinality();
  if ( fill_vec(ids, m_ros_hash_max, m_ros_vec) ) return (1);

  // drawer hash
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_drawer_range.size(); ++i) {
    const Range& range = m_full_drawer_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      HWIdentifier id = drawer_id (exp_id[m_ROS_INDEX], 
                                 exp_id[m_DRAWER_INDEX]);
      if(!(ids.insert(id)).second){
	log << MSG::ERROR << "init_hashes "
            << " Error: duplicated id for drawer id. nids= " << nids
            << " compact Id  " << show_to_string(id)
            << endmsg;
      }
      nids++;
    }
  }
  m_drawer_hash_max = m_full_drawer_range.cardinality();
  if ( fill_vec(ids, m_drawer_hash_max, m_drawer_vec) ) return (1);

  // channel hash
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
    const Range& range = m_full_channel_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      HWIdentifier id = channel_id    (exp_id[m_ROS_INDEX], 
                                 exp_id[m_DRAWER_INDEX], 
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

  // adc hash
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_adc_range.size(); ++i) {
    const Range& range = m_full_adc_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      HWIdentifier id = adc_id    (exp_id[m_ROS_INDEX], 
                                 exp_id[m_DRAWER_INDEX], 
                                 exp_id[m_CHANNEL_INDEX],
                                 exp_id[m_ADC_INDEX]);
      if(!(ids.insert(id)).second){
	log << MSG::ERROR << "init_hashes "
            << " Error: duplicated id for adc id. nids= " << nids
            << " compact Id  " << show_to_string(id)
            << endmsg;
      }
      nids++;
    }
  }
  m_adc_hash_max = m_full_adc_range.cardinality();
  if ( fill_vec(ids, m_adc_hash_max, m_adc_vec) ) return (1);
/*
  for(std::vector<HWIdentifier>::const_iterator it=m_adc_vec.begin()
			;it!=m_adc_vec.end();++it){
	std::vector<HWIdentifier>::const_iterator i
		=std::lower_bound(vec.begin(),vec.end(),it);
	m_adc_vec[m_adc_impl.unpack(*it)]
	
  }
*/

  return (0);
}

int TileHWID::fill_vec  (std::set<HWIdentifier> & ids, 
                         unsigned int hash_max, std::vector<HWIdentifier> & vec)
{

  if(ids.size() != hash_max) {

    MsgStream log(m_msgSvc, "TileHWID" );
    log << MSG::ERROR << "fill_vec "
        << " Error: set size NOT EQUAL to hash max. size " << ids.size()
        << " hash max " << hash_max
        << endmsg;

    return (1);
  }

  vec.resize(hash_max);

  std::set<HWIdentifier>::const_iterator first = ids.begin();
  std::set<HWIdentifier>::const_iterator last  = ids.end();

  int index = 0;
  for (;first != last; ++first) {
    vec[index++] = (*first);
  }
  

  return (0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

