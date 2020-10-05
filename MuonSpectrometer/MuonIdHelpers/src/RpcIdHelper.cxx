/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/RpcIdHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

RpcIdHelper::RpcIdHelper():
  MuonIdHelper("RpcIdHelper"),
  m_DOUBLETR_INDEX(0),
  m_DOUBLETZ_INDEX(0),
  m_DOUBLETPHI_INDEX(0),
  m_GASGAP_INDEX(0),
  m_MEASURESPHI_INDEX(0),
  m_gasGapMax(UINT_MAX) {}

// Initialize dictionary
int RpcIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
  int status = 0;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    (*m_Log) << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << "(Re)initialize" << endmsg;
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
  
  IdDictField* field = m_dict->find_field("doubletR");
  if (field) {
    m_DOUBLETR_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'doubletR' field " 	
	<< endmsg;
    status = 1;
  }

  field = m_dict->find_field("doubletZ");
  if (field) {
    m_DOUBLETZ_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'doubletZ' field " 	
	<< endmsg;
    status = 1;
  }

  field = m_dict->find_field("doubletPhi");
  if (field) {
    m_DOUBLETPHI_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'doubletPhi' field " 	
	<< endmsg;
    status = 1;
  }

  field = m_dict->find_field("rpcGasGap");
  if (field) {
    m_GASGAP_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'rpcGasGap' field " 	
	<< endmsg;
    status = 1;
  }

  field = m_dict->find_field("rpcMeasuresPhi");
  if (field) {
    m_MEASURESPHI_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'rpcMeasuresPhi' field " 	
	<< endmsg;
    status = 1;
  }

  field = m_dict->find_field("rpcStrip");
  if (field) {
    m_CHANNEL_INDEX = field->m_index;
  }
  else {
    (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'rpcStrip' field " 	
	<< endmsg;
    status = 1;
  }

  // reinitialze the module context
  m_MODULE_INDEX = m_DOUBLETR_INDEX;
  m_DETECTORELEMENT_INDEX = m_DOUBLETZ_INDEX;

  // save an index to the first region of rpc
  IdDictGroup* rpcGroup =  m_dict->find_group ("rpc");
  if(!rpcGroup) {
    (*m_Log) << MSG::ERROR << "Cannot find rpc group" << endmsg;
  } else {
    m_GROUP_INDEX =  rpcGroup->regions()[0]->m_index;
  }

  const IdDictRegion& region = *m_dict->m_regions[m_GROUP_INDEX];
  m_eta_impl  = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl  = region.m_implementation[m_PHI_INDEX]; 
  m_tec_impl  = region.m_implementation[m_TECHNOLOGY_INDEX]; 
  m_dbr_impl  = region.m_implementation[m_DOUBLETR_INDEX]; 
  m_dbz_impl  = region.m_implementation[m_DOUBLETZ_INDEX]; 
  m_dbp_impl  = region.m_implementation[m_DOUBLETPHI_INDEX]; 
  m_gap_impl  = region.m_implementation[m_GASGAP_INDEX]; 
  m_mea_impl  = region.m_implementation[m_MEASURESPHI_INDEX]; 
  m_str_impl  = region.m_implementation[m_CHANNEL_INDEX]; 

  (*m_Log) << MSG::DEBUG << " RPC decode index and bit fields for each level: "  << endmsg;
  (*m_Log) << MSG::DEBUG << " muon        "  << m_muon_impl.show_to_string() << endmsg;
  (*m_Log) << MSG::DEBUG << " station     "  << m_sta_impl.show_to_string()  << endmsg;
  (*m_Log) << MSG::DEBUG << " eta         "  << m_eta_impl.show_to_string()  << endmsg;
  (*m_Log) << MSG::DEBUG << " phi         "  << m_phi_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " technology  "  << m_tec_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " TR          "  << m_dbr_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " TZ          "  << m_dbz_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " TPHI        "  << m_dbp_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " gas gap     "  << m_gap_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " phi         "  << m_mea_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " strip       "  << m_str_impl.show_to_string()  << endmsg; 


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

  // Build MultiRange down to "doubletR" for all (muon) regions
  ExpandedIdentifier region_id;
  region_id.add(muonField);
  Range prefix;
  MultiRange muon_range = m_dict->build_multirange(region_id, prefix, "doubletR");
 
  if (muon_range.size() > 0 ) {
    (*m_Log) << MSG::INFO << "MultiRange built successfully to doubletR: " 
	<< "MultiRange size is " << muon_range.size() << endmsg;
  } else {
    (*m_Log) << MSG::ERROR << "Muon MultiRange is empty" << endmsg;
  }

  // Build MultiRange down to "detectorElement" for all mdt regions

  ExpandedIdentifier detectorElement_region;
  detectorElement_region.add(muonField);
  Range detectorElement_prefix;
  MultiRange muon_detectorElement_range = m_dict->build_multirange(detectorElement_region,
                                                                    detectorElement_prefix, "doubletPhi");
  if (muon_detectorElement_range.size() > 0 )
  {
     (*m_Log) << MSG::INFO << "MultiRange built successfully to detectorElement: " 
         << "DetectorElement MultiRange size is " << muon_detectorElement_range.size() << endmsg;
  }
  else
  {
     (*m_Log) << MSG::ERROR << "Muon RPC ReadoutElement MultiRange is empty" << endmsg;
  }

  // Build MultiRange down to "rpcStrip" for all rpc regions
  ExpandedIdentifier rpc_region;
  rpc_region.add(muonField);
  Range rpc_prefix;
  MultiRange muon_channel_range = m_dict->build_multirange(rpc_region, rpc_prefix, 
							     "rpcStrip");
 
  if (muon_channel_range.size() > 0 ) {
    (*m_Log) << MSG::INFO << "MultiRange built successfully to rpcStrip: " 
	<< "MultiRange size is " << muon_channel_range.size() << endmsg;
  } else {
    (*m_Log) << MSG::ERROR << "Muon RPC channel MultiRange is empty" << endmsg;
  }

  // build RPC module ranges
  // Find the regions that have a "RPC doubletR field" and save them
  int rpcField=-1; 
  status = m_dict->get_label_value("technology", "RPC", rpcField); 
 
  for (int i = 0; i < (int) muon_range.size(); ++i) {
    const Range& range = muon_range[i];
    if (range.fields() > m_TECHNOLOGY_INDEX) {
      const Range::field& field = range[m_TECHNOLOGY_INDEX];
      if ( field.match( (ExpandedIdentifier::element_type) rpcField ) ) {
	m_full_module_range.add(range);
	if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << "field size is " << (int) range.cardinality() 
	    << " field index = " << i << endmsg;
      }
    }
  }

  for (int j = 0; j < (int) muon_detectorElement_range.size(); ++j)
  {
      const Range& range = muon_detectorElement_range[j];
      if (range.fields() > m_TECHNOLOGY_INDEX)
      {
          const Range::field& field = range[m_TECHNOLOGY_INDEX];
          if ( field.match( (ExpandedIdentifier::element_type) rpcField ) )
          {
              m_full_detectorElement_range.add(range);
              if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG
                  << "detectorElement field size is " << (int) range.cardinality() 
                  << " field index = " << j << endmsg;
          }
      }
  }

  for (int j = 0; j < (int) muon_channel_range.size(); ++j) {
    const Range& range = muon_channel_range[j];
    if (range.fields() > m_TECHNOLOGY_INDEX) {
      const Range::field& field = range[m_TECHNOLOGY_INDEX];
      if ( field.match( (ExpandedIdentifier::element_type) rpcField ) ) {
	m_full_channel_range.add(range);
	if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << "channel field size is " << (int) range.cardinality() 
	    << " field index = " << j << endmsg;
      }
    }
  }

  // test to see that the multi range is not empty
  if (m_full_module_range.size() == 0) {
    (*m_Log) << MSG::ERROR << "RPC MultiRange ID is empty for modules" << endmsg;
    status = 1;
  } else {
    if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << " full module range size is " << m_full_module_range.size() << endmsg;
  }

    /// test to see that the detectorElement multi range is not empty
  if (m_full_detectorElement_range.size() == 0)
  {
      (*m_Log) << MSG::ERROR << "MDT MultiRange ID is empty for detector elements" << endmsg;
      status = 1;
  }

  // test to see that the multi range is not empty
  if (m_full_channel_range.size() == 0) {
    (*m_Log) << MSG::ERROR << "RPC MultiRange ID is empty for channels" << endmsg;
    status = 1;
  } else {
    if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << " full channel range size is " << m_full_channel_range.size() << endmsg;
  }

  // Setup the hash tables for RPC
  (*m_Log) << MSG::INFO << "Initializing RPC hash indices ... " << endmsg;
  status = init_hashes();
  status = init_detectorElement_hashes(); // doubletZ
  status = init_id_to_hashes();

  // Setup hash tables for finding neighbors
  (*m_Log) << MSG::INFO << "Initializing RPC hash indices for finding neighbors ... " << endmsg;
  status = init_neighbors();

  // retrieve the maximum number of gas gaps
  ExpandedIdentifier expId;
  IdContext gasGap_context(expId, 0, m_GASGAP_INDEX);
  for (const auto &id : m_detectorElement_vec) { // channel Identifiers not filled for RPCs, thus using detector element ones
  if(!get_expanded_id(id, expId, &gasGap_context)) {
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
        if (range.match(expId)) {
          const Range::field& gap_field = range[m_GASGAP_INDEX];
          if (gap_field.has_maximum()) {
            unsigned int max = (gap_field.get_maximum());
            if (m_gasGapMax == UINT_MAX) m_gasGapMax = max;
            else if (max > m_gasGapMax) m_gasGapMax = max;
          }
        }
      }
    }
  }
  if (m_gasGapMax == UINT_MAX) {
    (*m_Log) << MSG::ERROR << "No maximum number of RPC gas gaps was retrieved" << endmsg;
    status = 1;
  } else {
    if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << " Maximum number of RPC gas gaps is " << m_gasGapMax << endmsg;
  }
  m_init = true;
  return (status);
}

int RpcIdHelper::init_id_to_hashes() {
   unsigned int hash_max = this->module_hash_max();
   for (unsigned int i=0; i<hash_max; ++i) {
      Identifier id = m_module_vec[i];
      int station   = this->stationName(id);
      int eta       = this->stationEta(id) + 10; // for negative etas
      int phi       = this->stationPhi(id);
      int dR        = this->doubletR(id);
      m_module_hashes[station][eta-1][phi-1][dR-1] = i;
   }

   hash_max = this->detectorElement_hash_max();
   for (unsigned int i=0; i<hash_max; ++i) {
      Identifier id    = m_detectorElement_vec[i];
      int station      = this->stationName(id);
      std::string name = this->stationNameString(station);
      int eta          = this->stationEta(id) + 10; // for negative eta
      int phi          = this->stationPhi(id);
      int dR           = this->doubletR(id);
      int zIndex       = this->zIndex(id);
      m_detectorElement_hashes[station][eta-1][phi-1][dR-1][zIndex-1] = i;
   }
   return 0;
}

int RpcIdHelper::get_module_hash(const Identifier& id,
                                 IdentifierHash& hash_id) const {
  //Identifier moduleId = elementID(id);
  //IdContext context = module_context();
  //return get_hash(moduleId,hash_id,&context);
  int station   = this->stationName(id);
  int eta       = this->stationEta(id) + 10; // for negative etas
  int phi       = this->stationPhi(id);
  int dR        = this->doubletR(id);
  hash_id       = m_module_hashes[station][eta-1][phi-1][dR-1];
  return 0;
}

int RpcIdHelper::get_detectorElement_hash(const Identifier& id,
                                         IdentifierHash& hash_id) const {
  //Identifier detectorElementId = detectorElementID(id);
  //IdContext context = detectorElement_context();
  //return get_hash(detectorElementId,hash_id,&context);
  int station      = this->stationName(id);
  std::string name = this->stationNameString(station);
  int eta          = this->stationEta(id) + 10; // for negative eta
  int phi          = this->stationPhi(id);
  int dR           = this->doubletR(id);
  int zIndex       = this->zIndex(id);
  hash_id          = m_detectorElement_hashes[station][eta-1][phi-1][dR-1][zIndex-1];
  return 0;
}

void RpcIdHelper::idChannels (const Identifier& id, std::vector<Identifier>& vect) const {

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

// Access to min and max of level ranges

int RpcIdHelper::stationEtaMin(const Identifier& id) const {
 
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
}

int RpcIdHelper::stationEtaMax(const Identifier& id) const {

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
}

int RpcIdHelper::stationPhiMin(const Identifier& id) const {
  
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

 int RpcIdHelper::stationPhiMax(const Identifier& id) const {
  
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

 int RpcIdHelper::doubletRMin(const Identifier& id) const {
   
  ExpandedIdentifier expId;
  IdContext doubletR_context(expId, 0, m_DOUBLETR_INDEX);
  if(!get_expanded_id(id, expId, &doubletR_context)) {
    
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
      const Range& range = m_full_module_range[i];
      if (range.match(expId)) {
	const Range::field& r_field = range[m_DOUBLETR_INDEX];
	if (r_field.has_minimum()) {
	  return (r_field.get_minimum());
	}
      }
    }
  }
  // Failed to find the min
  return (999);
}

 int RpcIdHelper::doubletRMax(const Identifier& id) const {
  
  ExpandedIdentifier expId;
  IdContext doubletR_context(expId, 0, m_DOUBLETR_INDEX);
  if(!get_expanded_id(id, expId, &doubletR_context)) {
    
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
      const Range& range = m_full_module_range[i];
      if (range.match(expId)) {
	const Range::field& r_field = range[m_DOUBLETR_INDEX];
	  if (r_field.has_maximum()) {
	    return (r_field.get_maximum());
	  }
      }
    }
  }
  // Failed to find the max
  return (-999);
}

 int RpcIdHelper::doubletZMin(const Identifier& id) const {
  
  ExpandedIdentifier expId;
  IdContext doubletZ_context(expId, 0, m_DOUBLETZ_INDEX);
  if(!get_expanded_id(id, expId, &doubletZ_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& z_field = range[m_DOUBLETZ_INDEX];
	if (z_field.has_minimum()) {
	  return (z_field.get_minimum());
	}
      }
    }
  }
  // Failed to find the min
  return (999);
}

 int RpcIdHelper::doubletZMax(const Identifier& id) const {
   
  ExpandedIdentifier expId;
  IdContext doubletZ_context(expId, 0, m_DOUBLETZ_INDEX);
  if(!get_expanded_id(id, expId, &doubletZ_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& z_field = range[m_DOUBLETZ_INDEX];
	if (z_field.has_maximum()) {
	  return (z_field.get_maximum());
	}
      }
    }
  }
  // Failed to find the max
  return (-999);
}

 int RpcIdHelper::doubletPhiMin(const Identifier& id) const {

  ExpandedIdentifier expId;
  IdContext doubletPhi_context(expId, 0, m_DOUBLETPHI_INDEX);
  if(!get_expanded_id(id, expId, &doubletPhi_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& dphi_field = range[m_DOUBLETPHI_INDEX];
	if (dphi_field.has_minimum()) {
	  return (dphi_field.get_minimum());
	}
      }
    }
  }
  // Failed to find the min
  return (999);
}

 int RpcIdHelper::doubletPhiMax(const Identifier& id) const {
   
  ExpandedIdentifier expId;
  IdContext doubletPhi_context(expId, 0, m_DOUBLETPHI_INDEX);
  if(!get_expanded_id(id, expId, &doubletPhi_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& dphi_field = range[m_DOUBLETPHI_INDEX];
	if (dphi_field.has_maximum()) {
	  return (dphi_field.get_maximum());
	}
      }
    }
  }
  // Failed to find the max
  return (-999);
}

 int RpcIdHelper::gasGapMin(const Identifier& id) const {

  ExpandedIdentifier expId;
  IdContext gasGap_context(expId, 0, m_GASGAP_INDEX);
  if(!get_expanded_id(id, expId, &gasGap_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
	if (range.match(expId)) {
	  const Range::field& gas_field = range[m_GASGAP_INDEX];
	  if (gas_field.has_minimum()) {
	    return (gas_field.get_minimum());
	  }
	}
    }
  }
  // Failed to find the min
  return (999);
}

 int RpcIdHelper::gasGapMax(const Identifier& id) const {
 
  ExpandedIdentifier expId;
  IdContext gasGap_context(expId, 0, m_GASGAP_INDEX);
  if(!get_expanded_id(id, expId, &gasGap_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& gap_field = range[m_GASGAP_INDEX];
	if (gap_field.has_maximum()) {
	  return (gap_field.get_maximum());
	}
      }
    }
  }
  // Failed to find the max
  return (-999);
}

 int RpcIdHelper::measuresPhiMin(const Identifier& id) const {
 
  ExpandedIdentifier expId;
  IdContext measuresPhi_context(expId, 0, m_MEASURESPHI_INDEX);
  if(!get_expanded_id(id, expId, &measuresPhi_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& mphi_field = range[m_MEASURESPHI_INDEX];
	if (mphi_field.has_minimum()) {
	  return (mphi_field.get_minimum());
	}
      }
    }
  }
  // Failed to find the min
  return (999);
}

 int RpcIdHelper::measuresPhiMax(const Identifier& id) const {
 
  ExpandedIdentifier expId;
  IdContext measuresPhi_context(expId, 0, m_MEASURESPHI_INDEX);
  if(!get_expanded_id(id, expId, &measuresPhi_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& mphi_field = range[m_MEASURESPHI_INDEX];
	if (mphi_field.has_maximum()) {
	  return (mphi_field.get_maximum());
	}
      }
    }
  }
  // Failed to find the max
  return (-999);
}

 int RpcIdHelper::stripMin(const Identifier& id) const {

  ExpandedIdentifier expId;
  IdContext strip_context(expId, 0, m_CHANNEL_INDEX);
  if(!get_expanded_id(id, expId, &strip_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& strip_field = range[m_CHANNEL_INDEX];
	if (strip_field.has_minimum()) {
	  return (strip_field.get_minimum());
	}
      }
    }
  }
  // Failed to find the min
  return (999);
}

 int RpcIdHelper::stripMax(const Identifier& id) const {
  
  ExpandedIdentifier expId;
  IdContext strip_context(expId, 0, m_CHANNEL_INDEX);
  if(!get_expanded_id(id, expId, &strip_context)) {
    
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& strip_field = range[m_CHANNEL_INDEX];
	if (strip_field.has_maximum()) {
	  return (strip_field.get_maximum());
	}
      }
    }
  }
  // Failed to find the max
  return (-999);
}

// Public validation of levels

 bool RpcIdHelper::valid(const Identifier& id) const {

    if (! validElement(id)) return false;
 
    int dbz = doubletZ(id);
    if ((dbz < doubletZMin(id)) ||
        (dbz > doubletZMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid doubletZ=" << dbz
            << " doubletZMin=" << doubletZMin(id)
            << " doubletZMax=" << doubletZMax(id)
            << endmsg;
	return false;
    }

    int dbp = doubletPhi(id);
    if ((dbp < doubletPhiMin(id)) ||
        (dbp > doubletPhiMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid doubletPhi=" << dbp
            << " doubletPhiMin=" << doubletPhiMin(id)
            << " doubletPhiMax=" << doubletPhiMax(id)
            << endmsg;
	return false;
    }

    int gasG = gasGap(id);
    if ((gasG < gasGapMin(id)) ||
        (gasG > gasGapMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid gasGap=" << gasG
            << " gasGapMin=" << gasGapMin(id)
            << " gasGapMax=" << gasGapMax(id)
            << endmsg;
	return false;
    }

    int mPhi = measuresPhi(id);
    if ((mPhi < measuresPhiMin(id)) ||
        (mPhi > measuresPhiMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid measuresPhi=" << mPhi
            << " measuresPhiMin=" << measuresPhiMin(id)
            << " measuresPhiMax=" << measuresPhiMax(id)
            << endmsg;
	return false;
    }

    int str = strip(id);
    if ((str < stripMin(id)) ||
        (str > stripMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid strip=" << str
            << " stripMin=" << stripMin(id)
            << " stripMax=" << stripMax(id)
            << endmsg;
	return false;
    }
    return true;

}

 bool RpcIdHelper::validElement(const Identifier& id) const {

    int station = stationName(id);
    std::string name = stationNameString(station);

    if ('B' != name[0])
    {
        (*m_Log) << MSG::WARNING
            << "Invalid stationName=" << name
            << endmsg;
	return false;
    }

    int eta = stationEta(id);
    if (eta < stationEtaMin(id) ||
        eta > stationEtaMax(id)    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid stationEta=" << eta
            << " for stationName=" << name
            << " stationEtaMin=" << stationEtaMin(id)
            << " stationEtaMax=" << stationEtaMax(id)
            << endmsg;
	return false;
    }

    int phi = stationPhi(id);
    if ((phi < stationPhiMin(id)) ||
        (phi > stationPhiMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid stationPhi=" << phi
            << " for stationName=" << name
            << " stationPhiMin=" << stationPhiMin(id)
            << " stationPhiMax=" << stationPhiMax(id)
            << endmsg;
	return false;
    }

    int dbr = doubletR(id);
    if ((dbr < doubletRMin(id)) ||
        (dbr > doubletRMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid doubletR=" << dbr
            << " for stationName=" << name
            << " doubletRMin=" << doubletRMin(id)
            << " doubletRMax=" << doubletRMax(id)
            << endmsg;
	return false;
    }
    return true;
}

 bool RpcIdHelper::validPad(const Identifier& id) const {

    if (! validElement(id)) return false; 

    int dbz = doubletZ(id);
    if ((dbz < doubletZMin(id)) ||
        (dbz > doubletZMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid doubletZ=" << dbz
            << " doubletZMin=" << doubletZMin(id)
            << " doubletZMax=" << doubletZMax(id)
            << endmsg;
	return false;
    }

    int dbp = doubletPhi(id);
    if ((dbp < doubletPhiMin(id)) ||
        (dbp > doubletPhiMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid doubletPhi=" << dbp
            << " doubletPhiMin=" << doubletPhiMin(id)
            << " doubletPhiMax=" << doubletPhiMax(id)
            << endmsg;
	return false;
    }
    return true;

}

// Private validation of levels

bool RpcIdHelper::validElement(const Identifier& id, int stationName, int stationEta, 
				      int stationPhi, int doubletR, bool noPrint) const
{
    std::string name = stationNameString(stationName);

    if ('B' != name[0])
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid stationName=" << name
            << endmsg;
	return false;
    }
    if (stationEta < stationEtaMin(id) ||
        stationEta > stationEtaMax(id)    )
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid stationEta=" << stationEta
            << " for stationName=" << name
            << " stationEtaMin=" << stationEtaMin(id)
            << " stationEtaMax=" << stationEtaMax(id)
            << endmsg;
	return false;
    }
    if ((stationPhi < stationPhiMin(id)) ||
        (stationPhi > stationPhiMax(id))    )
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid stationPhi=" << stationPhi
            << " for stationName=" << name
            << " stationPhiMin=" << stationPhiMin(id)
            << " stationPhiMax=" << stationPhiMax(id)
            << endmsg;
	return false;
    }
    if ((doubletR < doubletRMin(id)) ||
        (doubletR > doubletRMax(id))    )
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid doubletR=" << doubletR
            << " for stationName=" << name
            << " doubletRMin=" << doubletRMin(id)
            << " doubletRMax=" << doubletRMax(id)
            << endmsg;
	return false;
    }
    return true;
}

// Check values down to detector element level

bool RpcIdHelper::validChannel(const Identifier& id, int stationName, int stationEta, 
				      int stationPhi, int doubletR, int doubletZ, 
				      int doubletPhi, int gasGap, int measuresPhi, 
				      int strip, bool noPrint) const
{
    if (! validElement(id, stationName, stationEta, stationPhi, doubletR, noPrint)) return false;

    if ((doubletZ < doubletZMin(id)) ||
        (doubletZ > doubletZMax(id))    )
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid doubletZ=" << doubletZ
            << " doubletZMin=" << doubletZMin(id)
            << " doubletZMax=" << doubletZMax(id)
            << endmsg;
	return false;
    }
    if ((doubletPhi < doubletPhiMin(id)) ||
        (doubletPhi > doubletPhiMax(id))    )
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid doubletPhi=" << doubletPhi
            << " doubletPhiMin=" << doubletPhiMin(id)
            << " doubletPhiMax=" << doubletPhiMax(id)
            << endmsg;
	return false;
    }
    if ((gasGap < gasGapMin(id)) ||
        (gasGap > gasGapMax(id))    )
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid gasGap=" << gasGap
            << " gasGapMin=" << gasGapMin(id)
            << " gasGapMax=" << gasGapMax(id)
            << endmsg;
	return false;
    }
    if ((measuresPhi < measuresPhiMin(id)) ||
        (measuresPhi > measuresPhiMax(id))    )
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid measuresPhi=" << measuresPhi
            << " measuresPhiMin=" << measuresPhiMin(id)
            << " measuresPhiMax=" << measuresPhiMax(id)
            << endmsg;
	return false;
    }
    if ((strip < stripMin(id)) ||
        (strip > stripMax(id))    )
    {
        if (!noPrint) (*m_Log) << MSG::WARNING
            << "Invalid strip=" << strip
            << " stripMin=" << stripMin(id)
            << " stripMax=" << stripMax(id)
            << endmsg;
	return false;
    }
    return true;
}

// Check values down to the pad

bool RpcIdHelper::validPad(const Identifier& id, int stationName, 
				  int stationEta, int stationPhi, 
				  int doubletR, int doubletZ, int doubletPhi) const
{
    if (! validElement(id, stationName, stationEta, stationPhi, doubletR)) return false;

    if ((doubletZ < doubletZMin(id)) ||
        (doubletZ > doubletZMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid doubletZ=" << doubletZ
            << " doubletZMin=" << doubletZMin(id)
            << " doubletZMax=" << doubletZMax(id)
            << endmsg;
	return false;
    }
    if ((doubletPhi < doubletPhiMin(id)) ||
        (doubletPhi > doubletPhiMax(id))    )
    {
        (*m_Log) << MSG::WARNING
            << "Invalid doubletPhi=" << doubletPhi
            << " doubletPhiMin=" << doubletPhiMin(id)
            << " doubletPhiMax=" << doubletPhiMax(id)
            << endmsg;
	return false;
    }
    return true;
}

int RpcIdHelper::init_detectorElement_hashes(void) {

    //
    // create a vector(s) to retrieve the hashes for compact ids. For
    // the moment, we implement a hash for detector channels
    //

    // detector element hash
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
            Identifier doubletZ_id   = this->doubletZID(id);
            int dZ                   = this->doubletZ(id);
	    int corrected_doubletZ   = this->zIndex(id);
            bool isInserted = false;
            if ( dZ == corrected_doubletZ ) {
               isInserted = ids.insert(doubletZ_id).second;
               if ( (!isInserted)  && m_Log->level()<=MSG::DEBUG)  (*m_Log) << MSG::DEBUG << "init_detectorElement_hashes " 
                                      << "Please check the dictionary for possible duplication for "
                                      << id << endmsg;
            } else {
              isInserted = ids.insert(id).second;
	      if ( !isInserted ) {
	         (*m_Log) << MSG::ERROR << "init_detectorElement_hashes "
		     << " Error: duplicated id for detector element id. nid " << (int) nids
                     << " doubletPhi ID " << id << endmsg; 
		 return (1);
              }
	    }
	    nids++;
	}
    }
    m_detectorElement_hash_max = ids.size();
    (*m_Log) <<MSG::INFO << "The detector element hash max is " << (int) m_detectorElement_hash_max << endmsg;
    m_detectorElement_vec.resize(m_detectorElement_hash_max);

    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last  = ids.end();
    for (; first != last && nids < m_detectorElement_vec.size(); ++first) {
	m_detectorElement_vec[nids] = (*first);
	nids++;
    }

    return (0);
}

