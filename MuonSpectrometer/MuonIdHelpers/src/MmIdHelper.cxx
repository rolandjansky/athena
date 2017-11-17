/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ==============================================================================
 * ATLAS Muon Identifier Helpers Package
 * -----------------------------------------
 * Copyright (C) 2003 by ATLAS Collaboration
 * ==============================================================================
 */

// Includes
#include "MuonIdHelpers/MmIdHelper.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

/*******************************************************************************/ 
inline void MmIdHelper::create_mlog() const {
  if(!m_Log) m_Log=new MsgStream(m_msgSvc, "MmIdHelper");
} 
/*******************************************************************************/ 
// Constructor/Destructor
MmIdHelper::MmIdHelper() : MuonIdHelper() {
    m_GASGAP_INDEX = 6;
}
/*******************************************************************************/ 
// Destructor
MmIdHelper::~MmIdHelper() {
  // m_Log deleted in base class.
}
/*******************************************************************************/ 
// Initialize dictionary
int MmIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr) {
  create_mlog();

  int status = 0;
  
  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    (*m_Log) << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  } else {
    if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << "(Re)initialize " << endmsg;
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

  int index = technologyIndex("MM");
  if (index == -1) {
    (*m_Log) << MSG::DEBUG << "initLevelsFromDict - there are no MM entries in the dictionary! "  << endmsg;
    return 0;
  }

  IdDictField* field = m_dict->find_field("mmMultilayer");
  if (field) {
    m_DETECTORELEMENT_INDEX = field->m_index;
  } else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'mmMultilayer' field "  << endmsg;
    status = 1;
  }

  field = m_dict->find_field("mmGasGap");
  if (field) {
    m_GASGAP_INDEX = field->m_index;
  } else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'mmGasGap' field " << endmsg;
    status = 1;
  }

  field = m_dict->find_field("mmChannel");
  if (field) {
    m_CHANNEL_INDEX = field->m_index;
  } else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find mmChannel' field " 	<< endmsg;
    status = 1;
  }

  // reinitialize the module ndex
  //// m_DETECTORELEMENT_INDEX = m_MODULE_INDEX;

  // save an index to the first region of MM
  IdDictGroup* mmGroup =  m_dict->find_group ("mm");
  if(!mmGroup) {
    (*m_Log) << MSG::ERROR << "Cannot find mm group" << endmsg;
  } else {
    m_GROUP_INDEX =  mmGroup->regions()[0]->m_index;
  }

  const IdDictRegion& region = *m_dict->m_regions[m_GROUP_INDEX];
  m_eta_impl   = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl   = region.m_implementation[m_PHI_INDEX]; 
  m_tec_impl   = region.m_implementation[m_TECHNOLOGY_INDEX]; 
  m_mplet_impl = region.m_implementation[m_DETECTORELEMENT_INDEX]; 
  m_gap_impl   = region.m_implementation[m_GASGAP_INDEX]; 
  m_cha_impl   = region.m_implementation[m_CHANNEL_INDEX]; 

  (*m_Log) << MSG::DEBUG << " MicroMegas decode index and bit fields for each level: "  << endmsg;
  (*m_Log) << MSG::DEBUG << " muon        "  << m_muon_impl.show_to_string()  << endmsg;
  (*m_Log) << MSG::DEBUG << " station     "  << m_sta_impl.show_to_string()   << endmsg;
  (*m_Log) << MSG::DEBUG << " eta         "  << m_eta_impl.show_to_string()   << endmsg;
  (*m_Log) << MSG::DEBUG << " phi         "  << m_phi_impl.show_to_string()   << endmsg; 
  (*m_Log) << MSG::DEBUG << " technology  "  << m_tec_impl.show_to_string()   << endmsg; 
  (*m_Log) << MSG::DEBUG << " multilayer   "  << m_mplet_impl.show_to_string() << endmsg; 
  (*m_Log) << MSG::DEBUG << " gasgap      "  << m_gap_impl.show_to_string()   << endmsg; 
  (*m_Log) << MSG::DEBUG << " channel     "  << m_cha_impl.show_to_string()   << endmsg; 
 
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
    (*m_Log) << MSG::INFO << "MultiRange built successfully to Technology: " 
	     << "MultiRange size is " << muon_range.size() << endmsg;
  } else {
    (*m_Log) << MSG::ERROR << "Muon MultiRange is empty" << endmsg;
  }

  // Build MultiRange down to "detector element" for all mdt regions
  ExpandedIdentifier detectorElement_region;
  detectorElement_region.add(muonField);
  Range detectorElement_prefix;
  MultiRange muon_detectorElement_range = m_dict->build_multirange(detectorElement_region, detectorElement_prefix, "mmMultilayer");
  if (muon_detectorElement_range.size() > 0 ) {
    (*m_Log) << MSG::INFO << "MultiRange built successfully to detector element: " << "Multilayer MultiRange size is " << muon_detectorElement_range.size() << endmsg;
  } else {
    (*m_Log) << MSG::ERROR << "Muon MicroMegas detector element MultiRange is empty" << endmsg;
  }

  // Build MultiRange down to "channel" for all MM regions
  ExpandedIdentifier mm_region;
  mm_region.add(muonField);
  Range mm_prefix;
  MultiRange muon_channel_range = m_dict->build_multirange(mm_region, mm_prefix, "mmChannel");
  if (muon_channel_range.size() > 0 ) {
    (*m_Log) << MSG::INFO << "MultiRange built successfully to channel: " << "MultiRange size is " << muon_channel_range.size() << endmsg;
  } else {
    (*m_Log) << MSG::ERROR << "Muon MultiRange is empty for channels" << endmsg;
  }

  // build MicroMegas module ranges
  // Find the regions that have a "technology field" that matches the MM and save them
  int mmField=-1;
  status = m_dict->get_label_value("technology", "MM", mmField); 

  for (int i = 0; i < (int) muon_range.size(); ++i) {
    const Range& range = muon_range[i];
    if (range.fields() > m_TECHNOLOGY_INDEX) {
      const Range::field& field = range[m_TECHNOLOGY_INDEX];
      if ( field.match( (ExpandedIdentifier::element_type) mmField ) ) {
	m_full_module_range.add(range);
	if (m_Log->level()<=MSG::DEBUG)(*m_Log) << MSG::DEBUG << "field size is " << (int) range.cardinality() << " field index = " << i << endmsg;
      }
    }
  }

  for (int j = 0; j < (int) muon_detectorElement_range.size(); ++j) {
    const Range& range = muon_detectorElement_range[j];
    if (range.fields() > m_TECHNOLOGY_INDEX) {
      const Range::field& field = range[m_TECHNOLOGY_INDEX];
      if ( field.match( (ExpandedIdentifier::element_type) mmField ) ) {
	m_full_detectorElement_range.add(range);
	if (m_Log->level()<=MSG::DEBUG)(*m_Log) << MSG::DEBUG << "detector element field size is " << (int) range.cardinality() << " field index = " << j << endmsg;
      }
    }
  }

  for (int j = 0; j < (int) muon_channel_range.size(); ++j) {
    const Range& range = muon_channel_range[j];
    if (range.fields() > m_TECHNOLOGY_INDEX) {
      const Range::field& field = range[m_TECHNOLOGY_INDEX];
      if ( field.match( (ExpandedIdentifier::element_type) mmField ) ) {
	m_full_channel_range.add(range);
	if (m_Log->level()<=MSG::DEBUG)(*m_Log) << MSG::DEBUG << "channel field size is " << (int) range.cardinality() << " field index = " << j << endmsg;
      }
    }
  }

  // test to see that the multi range is not empty
  if (m_full_module_range.size() == 0) {
    (*m_Log) << MSG::ERROR << "MicroMegas MultiRange ID is empty for modules" << endmsg;
    status = 1;
  }

  // test to see that the detector element multi range is not empty
  if (m_full_detectorElement_range.size() == 0) {
    (*m_Log) << MSG::ERROR << "MicroMegas MultiRange ID is empty for detector elements" << endmsg;
    status = 1;
  }

  // test to see that the multi range is not empty
  if (m_full_channel_range.size() == 0) {
    (*m_Log) << MSG::ERROR << "MicroMegas MultiRange ID is empty for channels" << endmsg;
    status = 1;
  }

  // Setup the hash tables for MicroMegas
  (*m_Log) << MSG::INFO << "Initializing MicroMegas hash indices ... " << endmsg;
  status = init_hashes();
  status = init_detectorElement_hashes(); // same as module hash
  status = init_id_to_hashes();
  
  /*
  //comment out this bit to test the identifiers 
  
  status = init_channel_hashes();
  std::cout << " looping over identifiers " << std::endl;
  const_id_iterator it = channel_begin();
  const_id_iterator it_end =  channel_end();
  for( ;it!=it_end;++it ){
  if( !is_mm(*it) )  (*m_Log) << MSG::DEBUG << "BadStgc: not MM    " <<  print_to_string(*it) << endmsg;
  if( !valid(*it) )  (*m_Log) << MSG::DEBUG << "BadStgc: not valid " <<  print_to_string(*it) << endmsg;
  }
  */

  // Setup hash tables for finding neighbors
  (*m_Log) << MSG::INFO << "Initializing MicroMegas hash indices for finding neighbors ... " << endmsg;
  status = init_neighbors();

  return (status);
}   //end MmIdHelper::initialize_from_dictionary
/*******************************************************************************/ 
int MmIdHelper::init_id_to_hashes() {
  unsigned int hash_max = this->module_hash_max();
  for (unsigned int i=0; i<hash_max; ++i) {
    Identifier id = m_module_vec[i];
    int station   = this->stationName(id);
    int eta       = this->stationEta(id) + 10; // for negative etas
    int phi       = this->stationPhi(id);
    m_module_hashes[station][eta-1][phi-1] = i;
  }
  
  hash_max = this->detectorElement_hash_max();
  for (unsigned int i=0; i<hash_max; ++i) {
    Identifier id  = m_detectorElement_vec[i];
    int station    = this->stationName(id);
    int eta        = this->stationEta(id) + 10; // for negative eta
    int phi        = this->stationPhi(id);
    int multilayer = this->multilayer(id);
    m_detectorElement_hashes[station][eta-1][phi-1][multilayer-1] = i;
  }
  
  return 0;
}  //end MmIdHelper::init_id_to_hashes()
/*******************************************************************************/ 
int MmIdHelper::getModule_hash(const Identifier& id, IdentifierHash& hash_id) const {
  //Identifier moduleId = elementID(id);
  //IdContext context = module_context();
  //return get_hash(moduleId,hash_id,&context);
  int station   = this->stationName(id);
  int eta       = this->stationEta(id) + 10; // for negative etas
  int phi       = this->stationPhi(id);
  hash_id       = m_module_hashes[station][eta-1][phi-1];
  return 0;
}  //end MmIdHelper::getModule_hash
/*******************************************************************************/ 
int MmIdHelper::getDetectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const {
  int station    = this->stationName(id);
  int eta        = this->stationEta(id) + 10; // for negative eta
  int phi        = this->stationPhi(id);
  int multilayer = this->multilayer(id);
  hash_id        = m_detectorElement_hashes[station][eta-1][phi-1][multilayer-1];
  return 0;
  // return this->getModule_hash(id, hash_id);
}
/*******************************************************************************/ 
Identifier MmIdHelper::multilayerID(const Identifier& channelID) const {
  assert(is_mm(channelID));
  Identifier result(channelID);
  //m_mplet_impl.reset(result);
  m_gap_impl.reset(result);
  m_cha_impl.reset(result);
  return result;
}
/*******************************************************************************/ 
Identifier MmIdHelper::multilayerID(const Identifier& moduleID, int multilayer, bool check, bool* isValid) const {
  bool val = false;
  Identifier result(moduleID);
  m_mplet_impl.pack (multilayer,result);
  if ( check ) {
    val = this->validElement(result);
    if ( isValid ) *isValid = val;
  }
  return result;
}
/*******************************************************************************/ 
void MmIdHelper::idChannels (const Identifier& id, std::vector<Identifier>& vect) const {
  vect.clear();
  Identifier parent = parentID(id);
  for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
    const Range& range = m_full_channel_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      Identifier child;
      get_id( (*first), child );
      if (parentID(child) == parent) vect.push_back(child);
    }
  }
}
/*******************************************************************************/ 
int MmIdHelper::stationEtaMin(const Identifier& id) const {
  ExpandedIdentifier expId;
  IdContext eta_context(expId, 0, m_ETA_INDEX);
  if(!get_expanded_id(id, expId, &eta_context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
      const Range& range = m_full_module_range[i];
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
  return (999);  // default
}  //end MmIdHelper::stationEtaMin
/*******************************************************************************/ 
int MmIdHelper::stationEtaMax(const Identifier& id) const {
  ExpandedIdentifier expId;
  IdContext eta_context(expId, 0, m_ETA_INDEX);
  if(!get_expanded_id(id, expId, &eta_context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
      const Range& range = m_full_module_range[i];
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
  return (-999); 
}  //end MmIdHelper::stationEtaMax
/*******************************************************************************/ 
int MmIdHelper::stationPhiMin(const Identifier& id) const {
  ExpandedIdentifier expId;
  IdContext phi_context(expId, 0, m_PHI_INDEX);

  if(!get_expanded_id(id, expId, &phi_context)) {
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
      const Range& range = m_full_module_range[i];
      if (range.match(expId)) {
	const Range::field& phi_field = range[m_PHI_INDEX];
	if (phi_field.has_minimum()) {
	  return (phi_field.get_minimum());
	}
      }
    }
  }
  // Failed to find the min
  return (999);
}
/*******************************************************************************/ 
int MmIdHelper::stationPhiMax(const Identifier& id) const {
  ExpandedIdentifier expId;
  IdContext phi_context(expId, 0, m_PHI_INDEX);

  if(!get_expanded_id(id, expId, &phi_context)) {
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
      const Range& range = m_full_module_range[i];
      if (range.match(expId)) {
	const Range::field& phi_field = range[m_PHI_INDEX];
	if (phi_field.has_maximum()) {
	  return (phi_field.get_maximum());
	}
      }
    }
  } 
  // Failed to find the max
  return (-999);
}
/*******************************************************************************/ 
int MmIdHelper::numberOfMultilayers(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext context = technology_context();
  if (!get_expanded_id(id, expId, &context))
    {
      int result = -999;
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& multilayer_field = range[m_DETECTORELEMENT_INDEX];
	      if (multilayer_field.has_maximum())
		{
		  int multilayermax = multilayer_field.get_maximum();
		  if (result < multilayermax) result = multilayermax;
		}
	    }
	}
      return (result);
    }
  return (-999); 

}
/*******************************************************************************/ 
int MmIdHelper::multilayerMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext multilayer_context(expId, 0, m_DETECTORELEMENT_INDEX);
  if (!get_expanded_id(id, expId, &multilayer_context))
    {
      int result = -999;
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& multilayer_field = range[m_DETECTORELEMENT_INDEX];
	      if (multilayer_field.has_minimum())
		{
		  int multilayermin = multilayer_field.get_minimum();
		  if (-999 == result)
		    {
		      result = multilayermin;
		    }
		  else
		    {
		      if (multilayermin < result) result = multilayermin;
		    }
		}
	    } 
	}
      return (result);
    }
  return (999);  /// default
}
/*******************************************************************************/ 
int MmIdHelper::multilayerMax(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext multilayer_context(expId, 0, m_DETECTORELEMENT_INDEX);
  if (!get_expanded_id(id, expId, &multilayer_context))
    {
      int result = -999;
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& multilayer_field = range[m_DETECTORELEMENT_INDEX];
	      if (multilayer_field.has_maximum())
		{
		  int multilayermax = multilayer_field.get_maximum();
		  if (result < multilayermax) result = multilayermax;
		}
	    }
	}
      return (result);
    }
  return (-999); 
}
/*******************************************************************************/ 
int MmIdHelper::gasGapMin(const Identifier& id) const {
  ExpandedIdentifier expId;
  IdContext gasgap_context(expId, 0, m_GASGAP_INDEX);
  if(!get_expanded_id(id, expId, &gasgap_context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& gasgap_field = range[m_GASGAP_INDEX];
	if (gasgap_field.has_minimum()) {
	  int gasgapmin = gasgap_field.get_minimum();
	  if (-999 == result) {
	    result = gasgapmin;
	  } else {
	    if (gasgapmin < result) result = gasgapmin;
	  }
	}
      }
    }
    return (result);
  }
  return (999); 
}
/*******************************************************************************/ 
int MmIdHelper::gasGapMax(const Identifier& id) const {
  ExpandedIdentifier expId;
  IdContext gasgap_context(expId, 0, m_GASGAP_INDEX);
  if(!get_expanded_id(id, expId, &gasgap_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& gasgap_field = range[m_GASGAP_INDEX];
	if (gasgap_field.has_maximum()) {
	  return (gasgap_field.get_maximum());
	}
      }
    }
  }
  // Failed to find the max
  return (-999);
}
/*******************************************************************************/ 
int MmIdHelper::channelMin(const Identifier& id) const {
  ExpandedIdentifier expId;
  IdContext channel_context(expId, 0, m_CHANNEL_INDEX);
  if(!get_expanded_id(id, expId, &channel_context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& channel_field = range[m_CHANNEL_INDEX];
	if (channel_field.has_minimum()) {
	  int channelmin = channel_field.get_minimum();
	  if (-999 == result) {
	    result = channelmin;
	  } else {
	    if (channelmin < result) result = channelmin;
	  }
	}
      }
    }
    return (result);
  }
  return (999); 
}
/*******************************************************************************/ 
int MmIdHelper::channelMax(const Identifier& id) const {
  ExpandedIdentifier expId;
  IdContext channel_context(expId, 0, m_CHANNEL_INDEX);
  if(!get_expanded_id(id, expId, &channel_context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& channel_field = range[m_CHANNEL_INDEX];
	if (channel_field.has_maximum()) {
	  int channelmax = channel_field.get_maximum();
	  if (result < channelmax) result = channelmax;
	}
      }
    }
    return (result);
  }
  return (-999); 
}
/*******************************************************************************/ 
// validation of levels
bool MmIdHelper::valid(const Identifier& id) const {
  create_mlog();
  if( !validElement(id) ) return false;

  int mplet     = multilayer(id);
  if ((mplet < multilayerMin(id)) ||
      (mplet > multilayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid multilayer=" << mplet
	       << " multilayerMin=" << multilayerMin(id)
	       << " multilayerMax=" << multilayerMax(id)
	       << endmsg;
      return false;
    }

  //  int station  = stationName(id);
  int gasG = gasGap(id);
  if (gasG  < gasGapMin(id) || gasG  > gasGapMax(id)) {
    (*m_Log) << MSG::WARNING
	     << "Invalid gasGap=" << gasG
	     << " gasGapMin=" << gasGapMin(id)
	     << " gasGapMax=" << gasGapMax(id)
	     << endmsg;
    return false;
  }


  int element = channel(id);
  if (element < channelMin(id) || element > channelMax(id) ) {
    (*m_Log) << MSG::WARNING
	     << "Invalid channel=" << element
	     << " channelMin=" << channelMin(id)
	     << " channelMax=" << channelMax(id)
	     << endmsg;
    return false;
  }
  return true;
}  //end MmIdHelper::valid
/*******************************************************************************/ 
bool MmIdHelper::validElement(const Identifier& id) const {
  create_mlog();
  
  int station = stationName(id);
  std::string name = stationNameString(station);

  if ('M' != name[0]) {
    (*m_Log) << MSG::WARNING
	     << "Invalid stationName=" << name
	     << endmsg;
    return false;
  }

  int eta = stationEta(id);
  if (eta < stationEtaMin(id) || eta > stationEtaMax(id) ) {
    (*m_Log) << MSG::WARNING
	     << "Invalid stationEta=" << eta
	     << " for stationName=" << name
	     << " stationIndex=" << station
	     << " stationEtaMin=" << stationEtaMin(id)
	     << " stationEtaMax=" << stationEtaMax(id)
	     << endmsg;
    return false;
  }
  
  int phi = stationPhi(id);
  if (phi < stationPhiMin(id) || phi > stationPhiMax(id) ) {
    (*m_Log) << MSG::WARNING
	     << "Invalid stationPhi=" << phi
	     << " for stationName=" << name
	     << " stationIndex=" << station
	     << " stationPhiMin=" << stationPhiMin(id)
	     << " stationPhiMax=" << stationPhiMax(id)
	     << endmsg;
    return false;
  }
  return true;

}  //end MmIdHelper::validElement
/*******************************************************************************/ 
// Private validation of levels
bool MmIdHelper::validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const {
  create_mlog();

  std::string name = stationNameString(stationName);

  if ('M' != name[0]) {
    (*m_Log) << MSG::WARNING
	     << "Invalid stationName=" << name
	     << endmsg;
    return false;
  }
  if (stationEta < stationEtaMin(id) || stationEta > stationEtaMax(id) ) {
    (*m_Log) << MSG::WARNING
	     << "Invalid stationEta=" << stationEta
	     << " for stationName=" << name
	     << " stationIndex=" << stationName
	     << " stationEtaMin=" << stationEtaMin(id)
	     << " stationEtaMax=" << stationEtaMax(id)
	     << endmsg;
    return false;
  }
  if (stationPhi < stationPhiMin(id) || stationPhi > stationPhiMax(id) ) {
    (*m_Log) << MSG::WARNING
	     << "Invalid stationPhi=" << stationPhi
	     << " for stationName=" << name
	     << " stationIndex=" << stationName
	     << " stationPhiMin=" << stationPhiMin(id)
	     << " stationPhiMax=" << stationPhiMax(id)
	     << endmsg;
    return false;
  }
  return true;
}  //end MmIdHelper::validElement
/*******************************************************************************/ 
// Check values down to readout channel level
bool MmIdHelper::validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channel) const {
  create_mlog();

  if (! validElement(id, stationName, stationEta, stationPhi)) return false;


  if ((multilayer < multilayerMin(id)) ||
      (multilayer > multilayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid multilayer=" << multilayer
	       << " multilayerMin=" << multilayerMin(id)
	       << " multilayerMax=" << multilayerMax(id)
	       << endmsg;
      return false;
    }


  if (gasGap  < gasGapMin(id) || gasGap  > gasGapMax(id)) {
    (*m_Log) << MSG::WARNING
	     << "Invalid gasGap=" << gasGap
	     << " gasGapMin=" << gasGapMin(id)
	     << " gasGapMax=" << gasGapMax(id) 
	     << endmsg;
    return false;
  }
  if (channel < channelMin(id) || channel > channelMax(id) ) {
    (*m_Log) << MSG::WARNING
	     << "Invalid channel=" << channel
	     << " channelMin=" << channelMin(id)
	     << " channelMax=" << channelMax(id)
	     << endmsg;
    return false;
  }
  return true;
}  //end MmIdHelper::validChannel
/*******************************************************************************/ 
