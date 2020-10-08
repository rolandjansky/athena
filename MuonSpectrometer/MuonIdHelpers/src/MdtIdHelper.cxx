/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ==============================================================================
 * ATLAS Muon Identifier Helpers Package
 * -----------------------------------------
 * ==============================================================================
 */

#include "MuonIdHelpers/MdtIdHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

inline void MdtIdHelper::create_mlog() const
{
  if(!m_Log) m_Log=new MsgStream(m_msgSvc, "MdtIdHelper");
}


MdtIdHelper::MdtIdHelper() : MuonIdHelper(), m_TUBELAYER_INDEX(0), m_tubesMax(UINT_MAX) {}

/// initialize dictionary
int MdtIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
  create_mlog();

  int status = 0;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    (*m_Log) << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  /// init base object

  if (AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);
 
  // Register version of the MuonSpectrometer dictionary
  if (register_dict_tag(dict_mgr, "MuonSpectrometer")) return(1);

  m_dict = dict_mgr.find_dictionary ("MuonSpectrometer"); 
 
  if (!m_dict)
    {
      (*m_Log) << MSG::ERROR 
	       << " initialize_from_dict - cannot access MuonSpectrometer dictionary "
	       << endmsg;
      return 1;
    }



  /// Initialize some of the field indices

  if (initLevelsFromDict()) return (1);

  IdDictField* field = m_dict->find_field("multiLayer");
  if (field)
    {
      m_DETECTORELEMENT_INDEX = field->m_index;
    }
  else
    {
      (*m_Log) << MSG::ERROR
	       << "initLevelsFromDict - unable to find 'multiLayer' field " 	
	       << endmsg;
      status = 1;
    }

  field = m_dict->find_field("tubeLayer");
  if (field)
    {
      m_TUBELAYER_INDEX = field->m_index;
    }
  else
    {
      (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'tubeLayer' field " 	
	       << endmsg;
      status = 1;
    }

  field = m_dict->find_field("tube");
  if (field)
    {
      m_CHANNEL_INDEX = field->m_index;
    }
  else
    {
      (*m_Log) << MSG::ERROR << "initLevelsFromDict - unable to find 'tube' field " 	
	       << endmsg;
      status = 1;
    }

  // save an index to the first region of mdt

  IdDictGroup* mdtGroup =  m_dict->find_group ("mdt");
  if (!mdtGroup)
    {
      (*m_Log) << MSG::ERROR << "Cannot find mdt group" << endmsg;
    }
  else
    {
      m_GROUP_INDEX =  mdtGroup->regions()[0]->m_index;
    }

  const IdDictRegion& region = *m_dict->m_regions[m_GROUP_INDEX];
  m_eta_impl  = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl  = region.m_implementation[m_PHI_INDEX]; 
  m_tec_impl  = region.m_implementation[m_TECHNOLOGY_INDEX]; 
  m_mla_impl  = region.m_implementation[m_DETECTORELEMENT_INDEX]; 
  m_lay_impl  = region.m_implementation[m_TUBELAYER_INDEX]; 
  m_tub_impl  = region.m_implementation[m_CHANNEL_INDEX]; 

  (*m_Log) << MSG::DEBUG << " MDT decode index and bit fields for each level: "  << endmsg;
  (*m_Log) << MSG::DEBUG << " muon        "  << m_muon_impl.show_to_string() << endmsg;
  (*m_Log) << MSG::DEBUG << " station     "  << m_sta_impl.show_to_string()  << endmsg;
  (*m_Log) << MSG::DEBUG << " eta         "  << m_eta_impl.show_to_string()  << endmsg;
  (*m_Log) << MSG::DEBUG << " phi         "  << m_phi_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " technology  "  << m_tec_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " multilayer  "  << m_mla_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " layer       "  << m_lay_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " tube        "  << m_tub_impl.show_to_string()  << endmsg; 
 
  /**
   * Build multirange for the valid set of identifiers
   */

  /// Find value for the field MuonSpectrometer

  int muonField = -1;
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
  if (atlasDict->get_label_value("subdet", "MuonSpectrometer", muonField))
    {
      (*m_Log) << MSG::ERROR
	       << "Could not get value for label 'MuonSpectrometer' of field "
	       << "'subdet' in dictionary " << atlasDict->m_name
	       << endmsg;
      return (1);
    }

  // Build MultiRange down to "technology" for all (muon) regions

  ExpandedIdentifier region_id;
  region_id.add(muonField);
  Range prefix;
  MultiRange muon_range = m_dict->build_multirange(region_id, prefix, "technology");
  if (muon_range.size() > 0 )
    {
      (*m_Log) << MSG::INFO
	       << "MultiRange built successfully to Technology: " 
	       << "MultiRange size is " << muon_range.size() << endmsg;
    }
  else
    {
      (*m_Log) << MSG::ERROR << "Muon MultiRange is empty" << endmsg;
    }

  // Build MultiRange down to "detector element" for all mdt regions

  ExpandedIdentifier detectorElement_region;
  detectorElement_region.add(muonField);
  Range detectorElement_prefix;
  MultiRange muon_detectorElement_range = m_dict->build_multirange(detectorElement_region,  detectorElement_prefix, "multiLayer");
  if (muon_detectorElement_range.size() > 0 )
    {
      (*m_Log) << MSG::INFO << "MultiRange built successfully to detector element: " 
	       << "Multilayer MultiRange size is " << muon_detectorElement_range.size() << endmsg;
    }
  else
    {
      (*m_Log) << MSG::ERROR << "Muon MDT detector element MultiRange is empty" << endmsg;
    }
    
  // Build MultiRange down to "tube" for all mdt regions

  ExpandedIdentifier mdt_region;
  mdt_region.add(muonField);
  Range mdt_prefix;
  MultiRange muon_channel_range = m_dict->build_multirange(mdt_region,
							     mdt_prefix, "tube");
  if (muon_channel_range.size() > 0 )
    {
      (*m_Log) << MSG::INFO << "MultiRange built successfully to tube: " 
	       << "MultiRange size is " << muon_channel_range.size() << endmsg;
    }
  else
    {
      (*m_Log) << MSG::ERROR << "Muon MDT channel MultiRange is empty" << endmsg;
    }
    


  /**
   * Build MDT module ranges:
   *
   * Find the regions that have a "technology field" that matches the MDT
   * and save them
   */

  int mdtField=-1;
  status = m_dict->get_label_value("technology", "MDT", mdtField);

  for (int i = 0; i < (int) muon_range.size(); ++i)
    {
      const Range& range = muon_range[i];
      if (range.fields() > m_TECHNOLOGY_INDEX)
        {
	  const Range::field& field = range[m_TECHNOLOGY_INDEX];
	  if ( field.match( (ExpandedIdentifier::element_type) mdtField ) )
            {
	      m_full_module_range.add(range);
	      if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG
						       << "module field size is " << (int) range.cardinality() 
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
	  if ( field.match( (ExpandedIdentifier::element_type) mdtField ) )
            {
	      m_full_detectorElement_range.add(range);
	      if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG
						       << "detector element field size is " << (int) range.cardinality() 
						       << " field index = " << j << endmsg;
            }
        }
    }

  for (int k = 0; k < (int) muon_channel_range.size(); ++k)
    {
      const Range& range = muon_channel_range[k];
      if (range.fields() > m_TECHNOLOGY_INDEX)
        {
	  const Range::field& field = range[m_TECHNOLOGY_INDEX];
	  if ( field.match( (ExpandedIdentifier::element_type) mdtField ) )
            {
	      m_full_channel_range.add(range);
	      if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG
						       << "channel field size is " << (int) range.cardinality() 
						       << " field index = " << k << endmsg;
            }
        }
    }

  /// test to see that the module multi range is not empty

  if (m_full_module_range.size() == 0)
    {
      (*m_Log) << MSG::ERROR << "MDT MultiRange ID is empty for modules" << endmsg;
      status = 1;
    }

  /// test to see that the detector element multi range is not empty

  if (m_full_detectorElement_range.size() == 0)
    {
      (*m_Log) << MSG::ERROR << "MDT MultiRange ID is empty for detector elements" << endmsg;
      status = 1;
    }

  /// test to see that the tube multi range is not empty

  if (m_full_channel_range.size() == 0)
    {
      (*m_Log) << MSG::ERROR << "MDT MultiRange ID is empty for channels" << endmsg;
      status = 1;
    }

  // Setup the hash tables for MDT

  (*m_Log) << MSG::INFO << "Initializing MDT hash indices ... " << endmsg;
  status = init_hashes();
  status = init_detectorElement_hashes();
  status = init_id_to_hashes();

  // Setup hash tables for finding neighbors

  (*m_Log) << MSG::INFO << "Initializing MDT hash indices for finding neighbors ... " << endmsg;
  status = init_neighbors();

  // retrieve the maximum number of tubes in any chamber
  ExpandedIdentifier expId;
  IdContext channel_context(expId, 0, m_CHANNEL_INDEX);
  for (const auto &id : m_detectorElement_vec) {
    if (!get_expanded_id(id, expId, &channel_context)) {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
        const Range& range = m_full_channel_range[i];
        if (range.match(expId)) {
          const Range::field& channel_field = range[m_CHANNEL_INDEX];
          if (channel_field.has_maximum())  {
            unsigned int max = channel_field.get_maximum();
            if (m_tubesMax == UINT_MAX) m_tubesMax = max;
            else if (max > m_tubesMax) m_tubesMax = max;
          }
        }
      }
    }
  }
  if (m_tubesMax == UINT_MAX) {
    (*m_Log) << MSG::ERROR << "No maximum number of MDT tubes was retrieved" << endmsg;
    status = 1;
  } else {
    if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG << " Maximum number of MDT tubes is " << m_tubesMax << endmsg;
  }
  m_init = true;
  return (status);
}

int MdtIdHelper::init_id_to_hashes() {
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
}

int MdtIdHelper::get_module_hash(const Identifier& id,
                                 IdentifierHash& hash_id) const {
  //Identifier moduleId = elementID(id);
  //IdContext context = module_context();
  //return get_hash(moduleId,hash_id,&context);
  int station   = this->stationName(id);
  int eta       = this->stationEta(id) + 10; // for negative etas
  int phi       = this->stationPhi(id);
  hash_id       = m_module_hashes[station][eta-1][phi-1];
  return 0;  
}

int MdtIdHelper::get_detectorElement_hash(const Identifier& id,
                                          IdentifierHash& hash_id) const {
  //Identifier multilayerId = multilayerID(id);
  //IdContext context = multilayer_context();
  //return get_hash(multilayerId,hash_id,&context);
  int station    = this->stationName(id);
  int eta        = this->stationEta(id) + 10; // for negative eta
  int phi        = this->stationPhi(id);
  int multilayer = this->multilayer(id);
  hash_id        = m_detectorElement_hashes[station][eta-1][phi-1][multilayer-1];
  return 0;
}

Identifier MdtIdHelper::multilayerID(const Identifier& channelID) const {
  assert(is_mdt(channelID));
  Identifier result(channelID);
  m_lay_impl.reset(result);
  m_tub_impl.reset(result);
  return result;
}

Identifier MdtIdHelper::multilayerID(const Identifier& moduleID, int multilayer, bool check, bool* isValid) const {
  bool val = false;
  Identifier result(moduleID);
  m_mla_impl.pack (multilayer,result);
  if ( check ) {
    val = this->validElement(result);
    if ( isValid ) *isValid = val;
  }
  return result;
}

void MdtIdHelper::idChannels (const Identifier& id, std::vector<Identifier>& vect) const {

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

/// Access to min and max of level ranges

int MdtIdHelper::stationEtaMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext eta_context(expId, 0, m_ETA_INDEX);
  if (!get_expanded_id(id, expId, &eta_context))
    {
      int result = -999;
      for (unsigned int i = 0; i < m_full_module_range.size(); ++i)
	{
	  const Range& range = m_full_module_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& eta_field = range[m_ETA_INDEX];
	      if (eta_field.has_minimum())
		{
		  int etamin = eta_field.get_minimum();
		  if (-999 == result)
		    {
		      result = etamin;
		    }
		  else
		    {
		      if (etamin < result) result = etamin;
		    }
		}
	    }
	}
      return (result);
    }
  return (999);  /// default
}

int MdtIdHelper::stationEtaMax(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext eta_context(expId, 0, m_ETA_INDEX);
  if (!get_expanded_id(id, expId, &eta_context))
    {
      int result = -999;
      for (unsigned int i = 0; i < m_full_module_range.size(); ++i)
	{
	  const Range& range = m_full_module_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& eta_field = range[m_ETA_INDEX];
	      if (eta_field.has_maximum())
		{
		  int etamax = eta_field.get_maximum();
		  if (result < etamax) result = etamax;
		}
	    }
	}
      return (result);
    }
  return (-999); 
}

int MdtIdHelper::stationPhiMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext phi_context(expId, 0, m_PHI_INDEX);
  if (!get_expanded_id(id, expId, &phi_context))
    { 
      for (unsigned int i = 0; i < m_full_module_range.size(); ++i)
	{
	  const Range& range = m_full_module_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_PHI_INDEX];
	      if (phi_field.has_minimum())
		{
		  return (phi_field.get_minimum());
		}
	    }
	}
    }
  /// Failed to find the min
  return (999);
}

int MdtIdHelper::stationPhiMax(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext phi_context(expId, 0, m_PHI_INDEX);
  if (!get_expanded_id(id, expId, &phi_context))
    {
      for (unsigned int i = 0; i < m_full_module_range.size(); ++i)
	{
	  const Range& range = m_full_module_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_PHI_INDEX];
	      if (phi_field.has_maximum())
		{
		  return (phi_field.get_maximum());
		}
	    }
	}
    } 
  /// Failed to find the max
  return (-999);
}

int MdtIdHelper::numberOfMultilayers(const Identifier& id) const
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

int MdtIdHelper::multilayerMin(const Identifier& id) const
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

int MdtIdHelper::multilayerMax(const Identifier& id) const
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

int MdtIdHelper::tubeLayerMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext tubelayer_context(expId, 0, m_TUBELAYER_INDEX);
  if(!get_expanded_id(id, expId, &tubelayer_context))
    {
      int result = -999;
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& tubelayer_field = range[m_TUBELAYER_INDEX];
	      if (tubelayer_field.has_minimum())
		{
		  int tubelayermin = tubelayer_field.get_minimum();
		  if (-999 == result)
		    {
		      result = tubelayermin;
		    }
		  else
		    {
		      if (tubelayermin < result) result = tubelayermin;
		    }
		}
	    }
	}
      return (result);
    }
  return (999);  // default
}

int MdtIdHelper::tubeLayerMax(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext tubelayer_context(expId, 0, m_TUBELAYER_INDEX);
  if(!get_expanded_id(id, expId, &tubelayer_context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
      {
	const Range& range = m_full_channel_range[i];
	if (range.match(expId))
	  {
	    const Range::field& tubelayer_field = range[m_TUBELAYER_INDEX];
	    if (tubelayer_field.has_maximum())
	      {
		int tubelayermax = tubelayer_field.get_maximum();
		if (result < tubelayermax) result = tubelayermax;
	      }
	  }
      }
    return (result);
  }
  return (-999); 
}

int MdtIdHelper::tubeMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext channel_context(expId, 0, m_CHANNEL_INDEX);
  if (!get_expanded_id(id, expId, &channel_context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
      {
	const Range& range = m_full_channel_range[i];
	if (range.match(expId))
	  {
	    const Range::field& channel_field = range[m_CHANNEL_INDEX];
	    if (channel_field.has_minimum())
	      {
		int channelmin = channel_field.get_minimum();
		if (-999 == result)
		  {
		    result = channelmin;
		  }
		else
		  {
		    if (channelmin < result) result = channelmin;
		  }
	      }
	  }
      }
    return (result);
  }
  return (999); /// default
}

int MdtIdHelper::tubeMax(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext channel_context(expId, 0, m_CHANNEL_INDEX);
  if (!get_expanded_id(id, expId, &channel_context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
      {
	const Range& range = m_full_channel_range[i];
	if (range.match(expId))
	  {
	    const Range::field& channel_field = range[m_CHANNEL_INDEX];
	    if (channel_field.has_maximum())
	      {
		int channelmax = channel_field.get_maximum();
		if (result < channelmax) result = channelmax;
	      }
	  }
      }
    return (result);
  }
  return (-999); 
}

/// Public validation of levels

bool MdtIdHelper::valid(const Identifier& id) const
{
  create_mlog();

  if (! validElement(id)) return false;

  int mlayer     = multilayer(id);
  if ((mlayer < multilayerMin(id)) ||
      (mlayer > multilayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid multilayer=" << mlayer
	       << " multilayerMin=" << multilayerMin(id)
	       << " multilayerMax=" << multilayerMax(id)
	       << endmsg;
      return false;
    }

  int layer      = tubeLayer(id);
  if ((layer < tubeLayerMin(id)) ||
      (layer > tubeLayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid tubeLayer=" << layer
	       << " tubeLayerMin=" << tubeLayerMin(id)
	       << " tubeLayerMax=" << tubeLayerMax(id)
	       << endmsg;
      return false;
    }

  int tb         = tube(id);
  if ((tb < tubeMin(id)) ||
      (tb > tubeMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid tube=" << tb
	       << " tubeMin=" << tubeMin(id)
	       << " tubeMax=" << tubeMax(id)
	       << endmsg;
      return false;
    }
  return true;
   
}

bool MdtIdHelper::validElement(const Identifier& id) const
{
  create_mlog();

  int station        = stationName(id);
  std::string name   = stationNameString(station);

  if (('B' != name[0]) && ('E' != name[0]))
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid stationName=" << name
	       << endmsg;
      return false;
    }

  int eta            = stationEta(id);
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

  int phi            = stationPhi(id);
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
  return true;
}

/// Private validation of levels

bool MdtIdHelper::validElement(const Identifier& id, int stationName,
                               int stationEta, int stationPhi) const
{
  create_mlog();

  std::string name = stationNameString(stationName);

  if (('B' != name[0]) && ('E' != name[0]))
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid stationName=" << name
	       << endmsg;
      return false;
    }
  if (stationEta < stationEtaMin(id) ||
      stationEta > stationEtaMax(id)    )
    {
      (*m_Log) << MSG::WARNING
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
      (*m_Log) << MSG::WARNING
	       << "Invalid stationPhi=" << stationPhi
	       << " for stationName=" << name
	       << " stationPhiMin=" << stationPhiMin(id)
	       << " stationPhiMax=" << stationPhiMax(id)
	       << endmsg;
      return false;
    }
  return true;
}

bool MdtIdHelper::validChannel(const Identifier& id, int stationName, 
			       int stationEta, int stationPhi, 
			       int multilayer, int tubeLayer, int tube) const
{
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
  if ((tubeLayer < tubeLayerMin(id)) ||
      (tubeLayer > tubeLayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid tubeLayer=" << tubeLayer
	       << " tubeLayerMin=" << tubeLayerMin(id)
	       << " tubeLayerMax=" << tubeLayerMax(id)
	       << endmsg;
      return false;
    }
  if ((tube < tubeMin(id)) ||
      (tube > tubeMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid tube=" << tube
	       << " tubeMin=" << tubeMin(id)
	       << " tubeMax=" << tubeMax(id)
	       << endmsg;
      return false;
    }
  return true;
   
}

bool MdtIdHelper::barrelChamber(int stationName) const
{
  std::string name = stationNameString(stationName);
  return ('B' == name[0]);
}
