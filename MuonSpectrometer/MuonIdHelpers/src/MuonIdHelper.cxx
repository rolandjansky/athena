/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// -----------------------------------------
// ******************************************************************************

//<doc><file> $Id: MuonIdHelper.cxx,v 1.48 2007-11-25 16:33:41 ketevi Exp $
//<version>   $Name: not supported by cvs2svn $

// Includes
 
#include "MuonIdHelpers/MuonIdHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

inline void MuonIdHelper::create_mlog() const
{
  if(!m_Log) m_Log=new MsgStream(m_msgSvc, "MuonIdHelper");
}

const std::string MuonIdHelper::BAD_NAME = "UNKNOWN";

// Constructor

MuonIdHelper::MuonIdHelper(): m_station_region_index(0), m_module_hash_max(0),
  m_channel_hash_max(0), m_detectorElement_hash_max(0), m_Log(NULL) {

  m_MUON_INDEX           =  0;
  m_GROUP_INDEX           =  6500;
  m_NAME_INDEX            =  1;
  m_ETA_INDEX             =  2;
  m_PHI_INDEX             =  3;
  m_TECHNOLOGY_INDEX      =  4;
  m_MODULE_INDEX          =  5;
  m_DETECTORELEMENT_INDEX =  5;
  m_CHANNEL_INDEX         = 10; 
  m_dict                  =  0;
  m_msgSvc                =  0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MessageSvc", m_msgSvc);
  if (sc.isFailure()) std::cout << "Fail to locate Message Service" << std::endl;

}

// Destructor

MuonIdHelper::~MuonIdHelper()
{
  if(m_Log) delete m_Log;
}

int
MuonIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
  create_mlog();

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    (*m_Log) << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    (*m_Log) << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  // init base object
  if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

  // Register version of the MuonSpectrometer dictionary
  if (register_dict_tag(dict_mgr, "MuonSpectrometer")) return(1);

  m_dict = dict_mgr.find_dictionary ("MuonSpectrometer"); 
  if(!m_dict) {
    (*m_Log) << MSG::ERROR 
	     << " initialize_from_dict - cannot access MuonSpectrometer dictionary "
	     << endmsg;
    return 1;
  }

  // Initialize some of the field indices
  if(initLevelsFromDict()) return (1);

  //
  // Build multirange for the valid set of identifiers
  //

  // Find value for the field MuonSpectrometer
  int muonField = -1;
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
  if (atlasDict->get_label_value("subdet", "MuonSpectrometer", muonField)) {
    (*m_Log) << MSG::ERROR << "Could not get value for label 'MuonSpectrometer' of field 'subdet' in dictionary " 
	     << atlasDict->m_name
	     << endmsg;
    return (1);
  }

  // Build MultiRange down to "technology" for all (muon) regions
  ExpandedIdentifier region_id;
  region_id.add(muonField);
  Range prefix;
  MultiRange muon_range = m_dict->build_multirange(region_id, prefix, "technology");
  if (muon_range.size() > 0 ) {
    (*m_Log) << MSG::INFO << "MultiRange built successfully to Technology " 
	     << "MultiRange size is " << muon_range.size() << endmsg;
  } else {
    (*m_Log) << MSG::ERROR << "Muon MultiRange is empty" << endmsg;
  }

  return 0;
}

// From hash get Identifier
int 	
MuonIdHelper::get_id 	 (const IdentifierHash& hash_id,
			  Identifier& id,
			  const IdContext* context) const
{
  int result = 1;
  id.clear();

  size_t begin = (context) ? context->begin_index(): 0;
  // cannot get hash if end is 0:
  size_t end   = (context) ? context->end_index()  : 0; 
  if (0 == begin) { 
    // No hashes yet for ids with prefixes
    if (m_MODULE_INDEX == end) {
      if (hash_id < (unsigned int)(m_module_vec.end() - m_module_vec.begin())) {
	id = m_module_vec[hash_id];
	result = 0;
      }
    } else if (m_DETECTORELEMENT_INDEX == end) {
      if (hash_id < (unsigned int)(m_detectorElement_vec.end() - m_detectorElement_vec.begin())) {
	id = m_detectorElement_vec[hash_id];
	result = 0;
      }
    } else if (m_CHANNEL_INDEX == end) {
      if (hash_id < (unsigned int)(m_channel_vec.end() - m_channel_vec.begin())) {
	id = m_channel_vec[hash_id];
	result = 0;
      }
    }
  }
  return (result);
}

int MuonIdHelper::get_module_hash(const Identifier& id,
				  IdentifierHash& hash_id) const {

  // by binary search - overwritten in the derived classes
  IdContext context = module_context();
  int result = 1;
  hash_id = 0;
  size_t begin = context.begin_index();
  size_t end   = context.end_index(); 

  if (0 == begin) {
    // No hashes yet for ids with prefixes
    if (m_MODULE_INDEX == end) {
      id_vec_it it = std::lower_bound(m_module_vec.begin(), 
				      m_module_vec.end(), 
				      id);
	    	   
      if (it != m_module_vec.end()) {
	hash_id = it - m_module_vec.begin();
	result = 0;
      } 
      else 
      	{
	  create_mlog();
	  (*m_Log)<<MSG::WARNING<< "MuonIdHelper::get_hash(cid,hash,con):hash not OK for collection" << endmsg; 
	}
    }
  } 
  return result;
}

int MuonIdHelper::get_detectorElement_hash(const Identifier& id,
					   IdentifierHash& hash_id) const {
  // by binary search - overwritten in the derived classes
  int result = 1;
  hash_id = 0;
  IdContext context = detectorElement_context();
  size_t begin = context.begin_index();
  size_t end   = context.end_index();
  if ( 0 == begin ) {
    if (m_DETECTORELEMENT_INDEX == end) {
      id_vec_it it = std::lower_bound(m_detectorElement_vec.begin(), 
  				      m_detectorElement_vec.end(), 
				      id);
      if (it != m_detectorElement_vec.end()) {
	hash_id = it - m_detectorElement_vec.begin();
	result = 0;
      } 
      else 
      	{
	  create_mlog();
	  (*m_Log) << MSG::WARNING << "MuonIdHelper::get_hash(cid,hash,con):hash not OK for detector element" << endmsg; 
	}
    }
  }
  return result;
}

int MuonIdHelper::get_channel_hash(const Identifier& id,
				   IdentifierHash& hash_id) const {
  IdContext context = channel_context();
  return get_hash(id,hash_id,&context);
}

int
MuonIdHelper::get_hash 	(const Identifier& id, 
			 IdentifierHash& hash_id,
			 const IdContext* context) const
{

  // Get the hash code from either a vec. We convert to compact 
  // and call get_hash again. For the latter, we calculate the hash from the
  // Identifier.

  int result = 1;
  hash_id = 0;
  size_t begin = (context) ? context->begin_index(): 0;
  //    size_t end   = (context) ? context->end_index()  : 0; 
  if (0 == begin) {
    // No hashes yet for ids with prefixes
    result = get_hash_calc(id, hash_id, context);
  }
  return (result);
}

int 	
MuonIdHelper::get_expanded_id	(const Identifier& id,
				 ExpandedIdentifier& exp_id,
				 const IdContext* context) const {
  return (get_expanded_id_calc (id, exp_id, context));
}

int 	
MuonIdHelper::get_id 		(const ExpandedIdentifier& old_id,
				 Identifier& new_id) const {
  // Copy old_id into new_id and get compact if possible
  int result = 0;
  new_id.clear();

  ExpandedIdentifier dummy_id;
  IdContext context(dummy_id, 0, old_id.fields() - 1);
  Identifier compact_id;
  if (!get_compact_id(old_id, compact_id, &context)) {
    new_id = compact_id;
  }
  return (result);
}

// From compact, get ExpandedIdentifier
int
MuonIdHelper::get_expanded_id_calc (const Identifier& compact_id,
				    ExpandedIdentifier& id,
				    const IdContext* context) const
{

  int result = 1;
  id.clear();
  if (m_dict) {
    // some preconditions in the case that the dictionary existd
    size_t begin = (context) ? context->begin_index(): 0;
    size_t end   = (context) ? context->end_index()  : m_CHANNEL_INDEX;
    assert (end <= m_CHANNEL_INDEX);

    if (0 == end) {
      result = 0;
    }
    else if (0 == begin) {
      ExpandedIdentifier empty;
      result = m_dict->unpack(compact_id, 
			      empty,
			      end,
			      id); 
    }
    else {
      // Non-zero prefix - we assume that the prefix contains
      // the IdDet level
      result = m_dict->unpack(compact_id, 
			      context->prefix_id(),
			      end,
			      id); 
    }
       
  }
  return result;
}

int 	
MuonIdHelper::get_compact_id 	(const ExpandedIdentifier& id, 
				 Identifier& compact_id,
				 const IdContext* context) const
{
  // Get compact for all fields provided
  int result = 1;
  compact_id = (Identifier::value_type)0;
  int exp_id[10];
    
  for (size_t i=0; i< id.fields(); i++)
    exp_id[i] = id[i];

  if (m_dict && id.fields() > 0) {

    size_t begin = (context) ? context->begin_index(): 0;
    size_t end = (context) ? context->end_index() : id.fields() - 1;
    result = m_dict->pack32(exp_id, begin, end,m_GROUP_INDEX, compact_id);
    // 	result = m_dict->pack32(id, begin, end, compact_id);
  }
  return (result);
}

int 			
MuonIdHelper::get_hash_calc (const Identifier& compact_id, 
			     IdentifierHash& hash_id,
			     const IdContext* context) const
{
  // Get the hash code from vec (for wafers only).
  int result = 1;
  hash_id = 0;
  size_t begin = (context) ? context->begin_index(): 0;
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) {
    // No hashes yet for ids with prefixes
    if (m_MODULE_INDEX == end) {
      result = get_module_hash(compact_id, hash_id);
    }
    else if (m_DETECTORELEMENT_INDEX == end) {
      result = get_detectorElement_hash(compact_id, hash_id);
    }
    else if (m_CHANNEL_INDEX == end) {
      id_vec_it it = std::lower_bound(m_channel_vec.begin(), 
				      m_channel_vec.end(), 
				      compact_id);
	    	   
      if (it != m_channel_vec.end()) {
	hash_id = it - m_channel_vec.begin();
	result = 0;
      } 
      else 
	{
	  create_mlog();
	  (*m_Log) << MSG::WARNING << "MuonIdHelper::get_hash(cid,hash,con):hash not OK for channel" << endmsg;
	}
    }
  }
  return (result);
}

int MuonIdHelper::initLevelsFromDict() {
 
  create_mlog();

  if(!m_dict) {
    (*m_Log) << MSG::ERROR << " initLevelsFromDict - dictionary NOT initialized "
	     << endmsg;
    return (1);
  }
    
  // Find out which identifier field corresponds to each level. Use
  // names to find each field/leve.

  m_MUON_INDEX		= 999;
  m_NAME_INDEX		= 999;
  m_ETA_INDEX		        = 999;
  m_PHI_INDEX		        = 999;
  m_CHANNEL_INDEX		= 999;

  // Save index to stations for unpacking
  ExpandedIdentifier id(muon_exp()); 
  if (m_dict->find_region(id, m_station_region_index) ) {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find a muon station index: id, reg"  
	     << " " << (std::string) id << " " << m_station_region_index
	     << endmsg;
    return (1);
  }

  // Find a Muon region
  IdDictField* field = m_dict->find_field("subdet");
  if (field) {
    m_MUON_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'subdet' field " 	
	     << endmsg;
    return (1);
  }
  field = m_dict->find_field("stationName");
  if (field) {
    m_NAME_INDEX = field->m_index;

    if (m_stationNameVector.empty()) {
      // we only need to fill the vectors and sets once
      for (size_t i=0; i<field->get_label_number(); i++) {
	std::string name = field->get_label(i);
	int index = (int)field->get_label_value(name);
	// (*m_Log) << MSG::INFO << "stationName: labelName=" << name << " labelValue=" << index << endmsg;
	if ((int)m_stationNameVector.size() <= index) {
	  m_stationNameVector.resize(index+1);
	}
	m_stationNameVector[index] = name;
	
	// all chambers starting with B are in the barrel except the BEE chambers
	if ( 'B' == name[0] ) {
	  if( name[1] != 'E' ) m_isBarrel.insert(index);
	  else                 m_isSmall.insert(index);  // BEE is in the small sector
	  if ( 'G' == name[2] || 'F' == name[2] ) {
	    m_isSmall.insert(index);
	  }
	} else if ( 'F' == name[2] ) {
	  m_isForward.insert(index);
	}
	if ( 'S' == name[2] ) {
	  m_isSmall.insert(index);
	}
      }
    }
  
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'stationName' field " 	
	     << endmsg;
    return (1);
  }
  field = m_dict->find_field("stationEta");
  if (field) {
    m_ETA_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'stationEta' field " 	
	     << endmsg;
    return (1);
  }
  field = m_dict->find_field("stationPhi");
  if (field) {
    m_PHI_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'stationPhi' field " 	
	     << endmsg;
    return (1);
  }
  field = m_dict->find_field("technology");
  if (field) {
    m_TECHNOLOGY_INDEX = field->m_index;

    if (m_technologyNameVector.empty()) {
      for (size_t i=0; i<field->get_label_number(); i++) {
	std::string name = field->get_label(i);
	int index = (int)field->get_label_value(name);
	// (*m_Log) << MSG::INFO << "technology: labelName=" << name << " labelValue=" << index << endmsg;
	if ((int)m_technologyNameVector.size() <= index) {
	  m_technologyNameVector.resize(index+1);
	}
	m_technologyNameVector[index] = name;
      }
    }

  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'technology' field " 	
	     << endmsg;
    return (1);
  }
  m_MODULE_INDEX = m_TECHNOLOGY_INDEX;

  // Set the field implementations down to the technology
  const IdDictRegion& region = *m_dict->m_regions[m_station_region_index];
  m_muon_impl = region.m_implementation[m_MUON_INDEX]; 
  m_sta_impl  = region.m_implementation[m_NAME_INDEX]; 

  // m_stationNameField = m_dict->find_field ("stationName"); // Philipp
  // m_technologyField  = m_dict->find_field ("technology"); // Philipp
  return (0);
}

int MuonIdHelper::init_hashes(void) {

  create_mlog();

  //
  // create a vector(s) to retrieve the hashes for compact ids. For
  // the moment, we implement a hash for modules
  //

  // module hash
  m_module_hash_max = m_full_module_range.cardinality();
  (*m_Log) <<MSG::INFO << "The element hash max is " << (int) m_module_hash_max << endmsg;
  m_module_vec.resize(m_module_hash_max);
  IdContext context = module_context();
  unsigned int nids = 0;
  std::set<Identifier> ids;
  for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
    const Range& range = m_full_module_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first)
      {
	Identifier id;
	get_id((*first), id);
	if(!(ids.insert(id)).second) {
	  (*m_Log) << MSG::ERROR << "init_hashes "
		   << " Error: duplicated id for module id. nid " << (int) nids
		   << " compact id " << id << " id " << endmsg;
	  return (1);
	}
	nids++;
      }
  }
  if(ids.size() != m_module_hash_max) {
    (*m_Log) << MSG::ERROR << "init_hashes "
	     << " Error: set size NOT EQUAL to element hash max. size " << ids.size()
	     << " hash max " << m_module_hash_max 
	     << endmsg;
    return (1);
  }

  nids = 0;
  std::set<Identifier>::const_iterator first = ids.begin();
  std::set<Identifier>::const_iterator last  = ids.end();
  for (; first != last && nids < m_module_vec.size(); ++first) {
    m_module_vec[nids] = (*first);
    nids++;
  }
    
  return (0);
}

int MuonIdHelper::init_detectorElement_hashes(void) {

  create_mlog();

  //
  // create a vector(s) to retrieve the hashes for compact ids. For
  // the moment, we implement a hash for readout channels
  //

  // detector element hash
  m_detectorElement_hash_max = m_full_detectorElement_range.cardinality();
  (*m_Log) <<MSG::INFO << "The detector element hash max is " << (int) m_detectorElement_hash_max << endmsg;
  m_detectorElement_vec.resize(m_detectorElement_hash_max);
  IdContext context = detectorElement_context();
  unsigned int nids = 0;
  std::set<Identifier> ids;
  for (unsigned int i = 0; i < m_full_detectorElement_range.size(); ++i) {
    const Range& range = m_full_detectorElement_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first)
      {
	Identifier id;
	get_id((*first), id);
	/*
	  (*m_Log) << MSG::INFO
	  << "The id is " << (std::string) (*first)
	  << "from get_id = " << show_to_string(id,&context) 
	  << " hash = " << nids << endmsg;
	*/	
	if(!(ids.insert(id)).second) {
	  (*m_Log) << MSG::ERROR << "init_detectorElement_hashes "
		   << " Error: duplicated id for channel id. nid " << (int) nids
		   << " compact id " << id << " id " << endmsg;
	  return (1);
	}
	nids++;
      }
  }
  if(ids.size() != m_detectorElement_hash_max) {
    (*m_Log) << MSG::ERROR << "init_hashes "
	     << " Error: set size NOT EQUAL to hash max. size " << ids.size()
	     << " hash max " << m_detectorElement_hash_max 
	     << endmsg;
    return (1);
  }

  nids = 0;
  std::set<Identifier>::const_iterator first = ids.begin();
  std::set<Identifier>::const_iterator last  = ids.end();
  for (; first != last && nids < m_detectorElement_vec.size(); ++first) {
    m_detectorElement_vec[nids] = (*first);
    nids++;
  }

  return (0);
}

int MuonIdHelper::init_channel_hashes(void) {

  create_mlog();

  //
  // create a vector(s) to retrieve the hashes for compact ids. For
  // the moment, we implement a hash for readout channels
  //

  // readout channel hash
  m_channel_hash_max = m_full_channel_range.cardinality();
  (*m_Log) <<MSG::INFO << "The channel hash max is " << (int) m_channel_hash_max << endmsg;
  m_channel_vec.resize(m_channel_hash_max);
  IdContext context = channel_context();
  unsigned int nids = 0;
  std::set<Identifier> ids;
  for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
    const Range& range = m_full_channel_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first)
      {
	Identifier id;
	get_id((*first), id);
	
	  // (*m_Log) << MSG::INFO
	  // 	   << "The id is " << (std::string) (*first)
	  // 	   << "from get_id = " << print_to_string(id) 
	  // 	   << " hash = " << nids << endmsg;
		
	if(!(ids.insert(id)).second) {
	  (*m_Log) << MSG::ERROR << "init_channel_hashes "
		   << " Error: duplicated id for channel id. nid " << (int) nids
		   << " compact id " << id << " id " << endmsg;
	  return (1);
	}
	m_channel_vec[nids] = id;
	nids++;
      }
  }
  if(ids.size() != m_channel_hash_max) {
    (*m_Log) << MSG::ERROR << "init_hashes "
	     << " Error: set size NOT EQUAL to hash max. size " << ids.size()
	     << " hash max " << m_channel_hash_max 
	     << endmsg;
    return (1);
  }
  /*
    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last  = ids.end();
    for (; first != last && nids < m_channel_vec.size(); ++first) {
    m_channel_vec[nids] = (*first);
    nids++;
    }
  */
  return (0);
}

int MuonIdHelper::get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const
{
  unsigned short index = id;
  if (index < m_prev_phi_module_vec.size()) {
    if (m_prev_phi_module_vec[index] == NOT_VALID_HASH) return (1);
    prev = 	m_prev_phi_module_vec[index];
    return (0);
  }
  return (1);
}

int MuonIdHelper::get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const
{
  unsigned short index = id;
  if (index < m_next_phi_module_vec.size()) {
    if (m_next_phi_module_vec[index] == NOT_VALID_HASH) return (1);
    next = 	m_next_phi_module_vec[index];
    return (0);
  }
  return (1);
}

int MuonIdHelper::get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const
{
  unsigned short index = id;
  if (index < m_prev_eta_module_vec.size()) {
    if (m_prev_eta_module_vec[index] == NOT_VALID_HASH) return (1);
    prev = 	m_prev_eta_module_vec[index];
    return (0);
  }
  return (1);
}

int MuonIdHelper::get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const
{
  unsigned short index = id;
  if (index < m_next_eta_module_vec.size()) {
    if (m_next_eta_module_vec[index] == NOT_VALID_HASH) return (1);
    next = 	m_next_eta_module_vec[index];
    return (0);
  }
  return (1);
}

int MuonIdHelper::init_neighbors(void)
{
  //
  // create a vector(s) to retrieve the hashes for compact ids for
  // module neighbors.
  //
  create_mlog();
  (*m_Log) << MSG::VERBOSE << "MuonIdHelper::init_neighbors " << endmsg;

  m_prev_phi_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
  m_next_phi_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
  m_prev_eta_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
  m_next_eta_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);

  for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
    const Range& range = m_full_module_range[i];
    const Range::field& phi_field = range[m_PHI_INDEX];
    const Range::field& eta_field = range[m_ETA_INDEX];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& id = (*first);
      ExpandedIdentifier::element_type previous_phi;
      ExpandedIdentifier::element_type next_phi;
      ExpandedIdentifier::element_type previous_eta;
      ExpandedIdentifier::element_type next_eta;
      bool pphi = phi_field.get_previous(id[m_PHI_INDEX], previous_phi);
      bool nphi = phi_field.get_next    (id[m_PHI_INDEX], next_phi);
      bool peta = eta_field.get_previous(id[m_ETA_INDEX], previous_eta);
      bool neta = eta_field.get_next    (id[m_ETA_INDEX], next_eta);

      Identifier     compact_id;
      IdContext 	   wcontext = module_context();
	    
      // First get primary hash id
      IdentifierHash hash_id;
      if (!get_compact_id(id, compact_id, &wcontext)) {
	// forward to compact -> hash
	if (get_hash(compact_id, hash_id, &wcontext)) {
	  (*m_Log)<< MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get hash, exp/compact ";
	  //		    id.show();
	  (*m_Log)<< " " 
		  << std::hex << compact_id << std::dec << endmsg;
	  return (1);
	}
      }
      else {
	(*m_Log)<< MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get compact, exp/compact "<<endmsg;
	//		id.show();

	return (1);
      }

      // index for the subsequent arrays
      unsigned short index = hash_id;
      assert (hash_id < m_prev_phi_module_vec.size());
      assert (hash_id < m_next_phi_module_vec.size());
      assert (hash_id < m_prev_eta_module_vec.size());
      assert (hash_id < m_next_eta_module_vec.size());
	    
      if (pphi) {
	// Get previous phi hash id
	ExpandedIdentifier expId = id;
	expId[m_PHI_INDEX] = previous_phi;
	if (!get_compact_id(expId, compact_id, &wcontext)) {
	  // forward to compact -> hash
	  if (get_hash(compact_id, hash_id, &wcontext)) {
	    (*m_Log) << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get previous phi hash, exp/compact ";
	    //			id.show();
	    (*m_Log)<< " " 
		    << std::hex << compact_id << std::dec << endmsg;
	    return (1);
	  }
	}
	else {
	  (*m_Log)<< MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get previous phi compact, exp/compact "<<endmsg;
	  return (1);
	}
	m_prev_phi_module_vec[index] = hash_id;
      }
	    
      if (nphi) {
	// Get next phi hash id
	ExpandedIdentifier expId = id;
	expId[m_PHI_INDEX] = next_phi;
	if (!get_compact_id(expId, compact_id, &wcontext)) {
	  // forward to compact -> hash
	  if (get_hash(compact_id, hash_id, &wcontext)) {
	    (*m_Log) << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get next phi hash, exp/compact ";
	    //		id.show();
	    (*m_Log) << " " 
		     << std::hex << compact_id << std::dec << endmsg;
	    return (1);
	  }
	}
	else {
	  (*m_Log) << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get next phi compact, exp/compact "<<endmsg;
	  //		    id.show();
	  //		    std::cout << std::endl;
	  return (1);
	}
	m_next_phi_module_vec[index] = hash_id;
      }
	    
      if (peta) {
	// Get previous eta hash id
	ExpandedIdentifier expId = id;
	expId[m_ETA_INDEX] = previous_eta;
	if (!get_compact_id(expId, compact_id, &wcontext)) {
	  // forward to compact -> hash
	  if (get_hash(compact_id, hash_id, &wcontext)) {
	    (*m_Log) << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get previous eta hash, exp/compact ";
	    (*m_Log) << " " 
		     << std::hex << compact_id << std::dec << endmsg;
	    return (1);
	  }
	}
	else {
	  (*m_Log) << MSG::WARNING<< " MuonIdHelper::init_neighbors - unable to get previous eta compact, exp/compact "<<endmsg;
	  return (1);
	}
	m_prev_eta_module_vec[index] = hash_id;
      }
	    
      if (neta) {
	// Get next eta hash id
	ExpandedIdentifier expId = id;
	expId[m_ETA_INDEX] = next_eta;
	if (!get_compact_id(expId, compact_id, &wcontext)) {
	  // forward to compact -> hash
	  if (get_hash(compact_id, hash_id, &wcontext)) {
	    (*m_Log) << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get next eta hash, exp/compact ";
	    //id.show();
	    (*m_Log) << " " 
		     << std::hex << compact_id << std::dec << endmsg;
	    return (1);
	  }
	}
	else {
	  (*m_Log) << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get next eta compact, exp/compact "<<endmsg;
	  return (1);
	}
	m_next_eta_module_vec[index] = hash_id;
      }
    }
  }
  //std::cout << "Finish init_neighors() ..." << std::endl;
  return (0);
}

void MuonIdHelper::test_module_packing (void) const {

  create_mlog();

  if (m_dict) {	
    int nids = 0;
    IdContext context = module_context();
    const_id_iterator first = m_module_vec.begin();
    const_id_iterator last  = m_module_vec.end();
    for (; first != last; ++first, ++nids) {
      Identifier compact = (*first);
      ExpandedIdentifier id;
      if (get_expanded_id(compact, id, &context)) {
	(*m_Log) << MSG::ERROR << "test_module_packing: Unable to get expanded id. Compact id " 
		 << compact << endmsg;
	continue;
      }
      Identifier new_compact;
      if (get_compact_id(id, new_compact, &context)) {
	(*m_Log) << MSG::ERROR << "test_module_packing: Unable to get compact id. Expanded id " 
		 << (std::string)(id) << endmsg;
	continue;
      }
      if (compact != new_compact) {
	(*m_Log) << MSG::ERROR << "test_module_packing: new and old compacts not equal " 
		 << "New/old/expanded ids " << new_compact << " " << compact << " " 
		 << (std::string)(id) << endmsg;
	continue;
      }
    }

    (*m_Log) << MSG::INFO << "test_module_packing: Successful tested " << nids << " ids. " 
	     << endmsg;
  }
  else {
    (*m_Log) << MSG::INFO << "Unable to test module is packing - no dictionary has been defined. " 
	     << endmsg;
  }
}

void MuonIdHelper::test_id (const Identifier& id, const IdContext& context) const {

  create_mlog();

  Identifier compact = id;
  ExpandedIdentifier new_id;
  if (get_expanded_id(compact, new_id, &context)) {
    (*m_Log) << MSG::ERROR << "Unable to get expanded id. Compact id " << compact << endmsg;
    return;
  }
  Identifier new_compact;
  if (get_compact_id(new_id, new_compact, &context)) {
    (*m_Log) << MSG::ERROR << "Unable to get compact id. Expanded id " << show_to_string(id) 
	     << endmsg;
    return;
  }
  if (compact != new_compact) {
    (*m_Log) << MSG::ERROR << "new and old compacts not equal. New/old/expanded ids " 
	     << new_compact << " " << compact << " " << show_to_string(id) << endmsg;
    return;
  }
}

// Prepend the station ID

void MuonIdHelper::addStationID(Identifier& id, int stationName, int stationEta,
				int stationPhi, int technology) const {

  ExpandedIdentifier exp_id;
  IdContext muon_context(exp_id, 0, m_MUON_INDEX);
  if(get_expanded_id(id, exp_id, &muon_context)) {
    create_mlog();
    (*m_Log)<<MSG::ERROR<<" MUON_ID result is NOT ok. MUON id " 
	    << show_to_string(id) 
	    << " Fields not appended " << endmsg;
  } else {
    exp_id << stationName << stationEta << stationPhi << technology;
    get_id(exp_id, id);  // Fill output
  }
}

int MuonIdHelper::stationRegion(const Identifier& id) const {
  std::string name = stationNameString(stationName(id));

  if ('I' == name[1] || '4' == name[1] ) return 0;
  if ('E' == name[1] || '1' == name[1] ) return 1;
  if ('M' == name[1] || '2' == name[1] ) return 2;
  if ('O' == name[1] || '3' == name[1] ) return 3;
  if ( name == "CSS" || name == "CSL"  ) return 0;
  create_mlog();
  (*m_Log) << MSG::ERROR << " MuonId::stationRegion / id = " 
	   << show_to_string(id) 
	   << " stationnamestring = "
	   << name << endmsg;
  assert(0);
  return -1;
}

