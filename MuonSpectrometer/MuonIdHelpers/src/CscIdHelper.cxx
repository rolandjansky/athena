/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ==============================================================================
 * ATLAS Muon Identifier Helpers Package
 * -----------------------------------------
 * ==============================================================================
 */

//<doc><file> $Id: CscIdHelper.cxx,v 1.44 2009-01-20 22:44:13 kblack Exp $
//<version>   $Name: not supported by cvs2svn $

/// Includes

#include "MuonIdHelpers/CscIdHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

inline void CscIdHelper::create_mlog() const
{
  if(!m_Log) m_Log=new MsgStream(m_msgSvc, "CscIdHelper");
}


/// Constructor/Destructor

CscIdHelper::CscIdHelper() : MuonIdHelper(), m_CHAMBERLAYER_INDEX(0),
  m_WIRELAYER_INDEX(0), m_MEASURESPHI_INDEX(0), m_etaStripMax(0), m_phiStripMax(0) {}

/// Destructor

CscIdHelper::~CscIdHelper()
{
  // m_Log deleted in base class.
}



/// Initialize dictionary

int CscIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr)
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

  int index = technologyIndex("CSC");
  if (index == -1) {
    (*m_Log) << MSG::DEBUG << "initLevelsFromDict - there are no CSC entries in the dictionary! "  << endmsg;
    return 0;
  }

  IdDictField* field = m_dict->find_field("chamberLayer");
  if (field)
    {
      m_CHAMBERLAYER_INDEX = field->m_index;
    }
  else
    {
      (*m_Log) << MSG::ERROR
	       << "initLevelsFromDict - unable to find 'chamberLayer' field " 	
	       << endmsg;
      status = 1;
    }

  field = m_dict->find_field("wireLayer");
  if (field)
    {
      m_WIRELAYER_INDEX = field->m_index;
    }
  else
    {
      (*m_Log) << MSG::ERROR
	       << "initLevelsFromDict - unable to find 'wireLayer' field " 	
	       << endmsg;
      status = 1;
    }

  field = m_dict->find_field("cscMeasuresPhi");
  if (field)
    {
      m_MEASURESPHI_INDEX = field->m_index;
    }
  else
    {
      (*m_Log) << MSG::ERROR
	       << "initLevelsFromDict - unable to find 'cscMeasuresPhi' field " 	
	       << endmsg;
      status = 1;
    }

  field = m_dict->find_field("cscStrip");
  if (field)
    {
      m_CHANNEL_INDEX = field->m_index;
    }
  else
    {
      (*m_Log) << MSG::ERROR
	       << "initLevelsFromDict - unable to find 'cscStrip' field " 	
	       << endmsg;
      status = 1;
    }

  // initialize the multilayer index
  m_DETECTORELEMENT_INDEX = m_CHAMBERLAYER_INDEX;
    
  /// save an index to the first region of csc

  IdDictGroup* cscGroup =  m_dict->find_group("csc");
  if(!cscGroup)
    {
      (*m_Log) << MSG::ERROR << "Cannot find csc group" << endmsg;
    }
  else
    {
      m_GROUP_INDEX = cscGroup->regions()[0]->m_index;
    }

  const IdDictRegion& region = *m_dict->m_regions[m_GROUP_INDEX];
  m_eta_impl  = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl  = region.m_implementation[m_PHI_INDEX]; 
  m_tec_impl  = region.m_implementation[m_TECHNOLOGY_INDEX]; 
  m_cla_impl  = region.m_implementation[m_CHAMBERLAYER_INDEX]; 
  m_lay_impl  = region.m_implementation[m_WIRELAYER_INDEX]; 
  m_mea_impl  = region.m_implementation[m_MEASURESPHI_INDEX]; 
  m_str_impl  = region.m_implementation[m_CHANNEL_INDEX]; 

  (*m_Log) << MSG::DEBUG << " CSC decode index and bit fields for each level: "  << endmsg;
  (*m_Log) << MSG::DEBUG << " muon        "  << m_muon_impl.show_to_string() << endmsg;
  (*m_Log) << MSG::DEBUG << " station     "  << m_sta_impl.show_to_string()  << endmsg;
  (*m_Log) << MSG::DEBUG << " eta         "  << m_eta_impl.show_to_string()  << endmsg;
  (*m_Log) << MSG::DEBUG << " phi         "  << m_phi_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " technology  "  << m_tec_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " cham layer  "  << m_cla_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " layer       "  << m_lay_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " phi         "  << m_mea_impl.show_to_string()  << endmsg; 
  (*m_Log) << MSG::DEBUG << " strip       "  << m_str_impl.show_to_string()  << endmsg; 

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

  /// Build MultiRange down to "technology" for all (muon) regions

  ExpandedIdentifier region_id;
  region_id.add(muonField);
  Range prefix;
  MultiRange muon_range = m_dict->build_multirange(region_id,
						     prefix,
						     "technology");
  if (muon_range.size() > 0 )
    {
      (*m_Log) << MSG::INFO << "MultiRange built successfully to Technology: " 
	       << "MultiRange size is " << muon_range.size() << endmsg;
    }
  else
    {
      (*m_Log) << MSG::ERROR << "Muon MultiRange is empty for modules" << endmsg;
    }

  // Build MultiRange down to "detector element" for all mdt regions

  ExpandedIdentifier detectorElement_region;
  detectorElement_region.add(muonField);
  Range detectorElement_prefix;
  MultiRange muon_detectorElement_range = m_dict->build_multirange(detectorElement_region,
								     detectorElement_prefix, "chamberLayer");
  if (muon_detectorElement_range.size() > 0 )
    {
      (*m_Log) << MSG::INFO << "MultiRange built successfully to detector element: "
	       << "Multilayer MultiRange size is " << muon_detectorElement_range.size() << endmsg;
    }
  else
    {
      (*m_Log) << MSG::ERROR << "Muon CSC detector element MultiRange is empty" << endmsg;
    }

  /// Build MultiRange down to "cscStrip" for all CSC regions

  ExpandedIdentifier csc_region;
  csc_region.add(muonField);
  Range csc_prefix;
  MultiRange muon_channel_range = m_dict->build_multirange(csc_region,
							     csc_prefix, 
							     "cscStrip");
  if (muon_channel_range.size() > 0 )
    {
      (*m_Log) << MSG::INFO << "MultiRange built successfully to cscStrip: " 
	       << "MultiRange size is " << muon_channel_range.size() << endmsg;
    }
  else
    {
      (*m_Log) << MSG::ERROR << "Muon MultiRange is empty for channels" << endmsg;
    }

  /**
   * Build CSC module ranges:
   *
   * Find the regions that have a "technology field" that matches the MDT
   * and save them
   */

  int cscField=-1;
  status = m_dict->get_label_value("technology", "CSC", cscField); 

  for (int i = 0; i < (int) muon_range.size(); ++i)
    {
      const Range& range = muon_range[i];
      if (range.fields() > m_TECHNOLOGY_INDEX)
        {
	  const Range::field& field = range[m_TECHNOLOGY_INDEX];
	  if ( field.match( (ExpandedIdentifier::element_type) cscField ) )
            {
	      m_full_module_range.add(range);
	      if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG
						       << "field size is " << (int) range.cardinality() 
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
	  if ( field.match( (ExpandedIdentifier::element_type) cscField ) )
            {
	      m_full_detectorElement_range.add(range);
	      if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG
						       << "detector element field size is " << (int) range.cardinality()
						       << " field index = " << j << endmsg;
            }
        }
    }

  for (int j = 0; j < (int) muon_channel_range.size(); ++j)
    {
      const Range& range = muon_channel_range[j];
      if (range.fields() > m_TECHNOLOGY_INDEX) {
	const Range::field& field = range[m_TECHNOLOGY_INDEX];
	if ( field.match( (ExpandedIdentifier::element_type) cscField ) )
	  {
	    m_full_channel_range.add(range);
	    if (m_Log->level()<=MSG::DEBUG) (*m_Log) << MSG::DEBUG
						     << "channel field size is " << (int) range.cardinality() 
						     << " field index = " << j << endmsg;
	  }
      }
    }

  /// test to see that the multi range is not empty

  if (m_full_module_range.size() == 0)
    {
      (*m_Log) << MSG::ERROR
	       << "CSC MultiRange ID is empty for modules" << endmsg;
      status = 1;
    }

  /// test to see that the detector element multi range is not empty

  if (m_full_detectorElement_range.size() == 0)
    {
      (*m_Log) << MSG::ERROR << "CSC MultiRange ID is empty for detector elements" << endmsg;
      status = 1;
    }

  /// test to see that the multi range is not empty

  if (m_full_channel_range.size() == 0)
    {
      (*m_Log) << MSG::ERROR
	       << "CSC MultiRange ID is empty for channels" << endmsg;
      status = 1;
    }

  /// Setup the hash tables for CSC

  (*m_Log) << MSG::INFO
	   << "Initializing CSC hash indices ... " << endmsg;
  status = init_hashes();
  status = init_detectorElement_hashes(); // for chamber layer - a chamber
  status = init_channel_hashes();
  status = strip_hash_offsets();
  status = init_id_to_hashes();

  /// Setup hash tables for finding neighbors

  (*m_Log) << MSG::INFO
	   << "Initializing CSC hash indices for finding neighbors ... " << endmsg;
  status = init_neighbors();

  return (status);
}

int CscIdHelper::init_id_to_hashes() {
  unsigned int hash_max = this->module_hash_max();
  for (unsigned int i=0; i<hash_max; ++i) {
    Identifier id = m_module_vec[i];
    int station   = this->stationName(id);
    int eta       = this->stationEta(id) + 2; // for negative etas
    int phi       = this->stationPhi(id);
    m_module_hashes[station][eta-1][phi-1] = i;
  }

  hash_max = this->detectorElement_hash_max();
  for (unsigned int i=0; i<hash_max; ++i) {
    Identifier id  = m_detectorElement_vec[i];
    int station    = this->stationName(id);
    int eta        = this->stationEta(id) + 2; // for negative eta
    int phi        = this->stationPhi(id);
    int multilayer = this->chamberLayer(id);
    m_detectorElement_hashes[station][eta-1][phi-1][multilayer-1] = i;
  }

  return 0;
}

int CscIdHelper::get_module_hash(const Identifier& id,
                                 IdentifierHash& hash_id) const {
  //Identifier moduleId = elementID(id);
  //IdContext context = module_context();
  //return get_hash(moduleId,hash_id,&context);
  int station   = this->stationName(id);
  int eta       = this->stationEta(id) + 2; // for negative etas
  int phi       = this->stationPhi(id);
  hash_id       = m_module_hashes[station][eta-1][phi-1];
  return 0;
}

int CscIdHelper::get_detectorElement_hash(const Identifier& id,
                                          IdentifierHash& hash_id) const {
  //Identifier multilayerId = multilayerID(id);
  //IdContext context = multilayer_context();
  //return get_hash(multilayerId,hash_id,&context);
  int station    = this->stationName(id);
  int eta        = this->stationEta(id) + 2; // for negative eta
  int phi        = this->stationPhi(id);
  int multilayer = this->chamberLayer(id);
  hash_id        = m_detectorElement_hashes[station][eta-1][phi-1][multilayer-1];
  return 0;
}

int CscIdHelper::get_channel_hash(const Identifier& id, IdentifierHash& hash_id) const {
  const IdContext context=this->channel_context();
  return get_hash_calc(id,hash_id,&context);
}

void CscIdHelper::idChannels (const Identifier& id, std::vector<Identifier>& vect) const {

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

int CscIdHelper::stationEtaMin(const Identifier& id) const
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

int CscIdHelper::stationEtaMax(const Identifier& id) const
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

int CscIdHelper::stationPhiMin(const Identifier& id) const
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

int CscIdHelper::stationPhiMax(const Identifier& id) const
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

int CscIdHelper::chamberLayerMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext chamberLayer_context(expId, 0, m_CHAMBERLAYER_INDEX);
  if (!get_expanded_id(id, expId, &chamberLayer_context))
    {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_CHAMBERLAYER_INDEX];
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

int CscIdHelper::chamberLayerMax(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext chamberLayer_context(expId, 0, m_CHAMBERLAYER_INDEX);
  if (!get_expanded_id(id, expId, &chamberLayer_context))
    {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_CHAMBERLAYER_INDEX];
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

int CscIdHelper::wireLayerMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext wireLayer_context(expId, 0, m_WIRELAYER_INDEX);
  if (!get_expanded_id(id, expId, &wireLayer_context))
    {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_WIRELAYER_INDEX];
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

int CscIdHelper::wireLayerMax(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext chamberLayer_context(expId, 0, m_WIRELAYER_INDEX);
  if (!get_expanded_id(id, expId, &chamberLayer_context))
    {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_WIRELAYER_INDEX];
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

int CscIdHelper::measuresPhiMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext measuresPhi_context(expId, 0, m_MEASURESPHI_INDEX);
  if (!get_expanded_id(id, expId, &measuresPhi_context))
    {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_MEASURESPHI_INDEX];
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

int CscIdHelper::measuresPhiMax(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext measuresPhi_context(expId, 0, m_MEASURESPHI_INDEX);
  if (!get_expanded_id(id, expId, &measuresPhi_context))
    {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_MEASURESPHI_INDEX];
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

int CscIdHelper::stripMin(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext strip_context = channel_context();
  if (!get_expanded_id(id, expId, &strip_context))
    {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_CHANNEL_INDEX];
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

int CscIdHelper::stripMax(const Identifier& id) const
{
  if (measuresPhi(id)){ 
    if (m_phiStripMax) return m_phiStripMax; 
  } else { 
    if (m_etaStripMax) return m_etaStripMax; 
  } 
  
  // Okay, so now need to fill values.
 
  ExpandedIdentifier expId;
  IdContext strip_context = channel_context();
  if (!get_expanded_id(id, expId, &strip_context))
    {
      for (unsigned int i = 0; i < m_full_channel_range.size(); ++i)
	{
	  const Range& range = m_full_channel_range[i];
	  if (range.match(expId))
	    {
	      const Range::field& phi_field = range[m_CHANNEL_INDEX];
	      if (phi_field.has_maximum())
		{  
		  if (measuresPhi(id)){
		    m_phiStripMax=phi_field.get_maximum(); return m_phiStripMax;
		  } else {
		    m_etaStripMax=phi_field.get_maximum(); return m_etaStripMax;
		  } 
		}
	    }
	}
    }
  /// Failed to find the max
  return (-999);
}

/// Public validation of levels

bool CscIdHelper::valid(const Identifier& id) const
{
  create_mlog();

  if (! validElement(id)) return false;

  int cLayer  = chamberLayer(id);
  if ((cLayer < chamberLayerMin(id)) ||
      (cLayer > chamberLayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid chamberLayer=" << cLayer
	       << " chamberLayerMin=" << chamberLayerMin(id)
	       << " chamberLayerMax=" << chamberLayerMax(id)
	       << endmsg;
      return false;
    }

  int wLayer  = wireLayer(id);
  if ((wLayer < wireLayerMin(id)) ||
      (wLayer > wireLayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid wireLayer=" << wLayer
	       << " wireLayerMin=" << wireLayerMin(id)
	       << " wireLayerMax=" << wireLayerMax(id)
	       << endmsg;
      return false;
    }

  int mPhi    = measuresPhi(id);
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

  int channel = strip(id);
  if ((channel > stripMax(id)) ||
      (channel < stripMin(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid strip=" << channel
	       << " stripMin=" << stripMin(id)
	       << " stripMax=" << stripMax(id)
	       << endmsg;
      return false;
    }
  return true;
 
}

bool CscIdHelper::validElement(const Identifier& id) const
{
  create_mlog();

  int station = stationName(id);
  std::string name = stationNameString(station);
  if ('C' != name[0])
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid stationName=" << name
	       << endmsg;
      return false;
    }

  int eta = stationEta(id);
  if ((eta < stationEtaMin(id)) ||
      (eta > stationEtaMax(id)) ||
      (0 == eta)                 )
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
  return true;
 
}

/// Private validation of levels

bool CscIdHelper::validElement(const Identifier& id, int stationName,
			       int stationEta, int stationPhi) const
{
  create_mlog();
      
  std::string name = stationNameString(stationName);

  if ('C' != name[0])
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid stationName=" << name
	       << endmsg;
      return false;
    }
  if ((stationEta < stationEtaMin(id)) ||
      (stationEta > stationEtaMax(id)) ||
      (0 == stationEta)                 )
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

bool CscIdHelper::validChannel(const Identifier& id, int stationName, int stationEta, 
			       int stationPhi,int chamberLayer, int wireLayer, 
			       int measuresPhi, int strip) const
{
  create_mlog();

  if (! validElement(id, stationName, stationEta, stationPhi)) return false;

  if ((chamberLayer < chamberLayerMin(id)) ||
      (chamberLayer > chamberLayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid chamberLayer=" << chamberLayer
	       << " chamberLayerMin=" << chamberLayerMin(id)
	       << " chamberLayerMax=" << chamberLayerMax(id)
	       << endmsg;
      return false;
    }
  if ((wireLayer < wireLayerMin(id)) ||
      (wireLayer > wireLayerMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid wireLayer=" << wireLayer
	       << " wireLayerMin=" << wireLayerMin(id)
	       << " wireLayerMax=" << wireLayerMax(id)
	       << endmsg;
      return false;
    }
  if ((measuresPhi < measuresPhiMin(id)) ||
      (measuresPhi > measuresPhiMax(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid measuresPhi=" << measuresPhi
	       << " measuresPhiMin=" << measuresPhiMin(id)
	       << " measuresPhiMax=" << measuresPhiMax(id)
	       << endmsg;
      return false;
    }
  if ((strip > stripMax(id)) ||
      (strip < stripMin(id))    )
    {
      (*m_Log) << MSG::WARNING
	       << "Invalid strip=" << strip
	       << " stripMin=" << stripMin(id)
	       << " stripMax=" << stripMax(id)
	       << endmsg;
      return false;
    }
  return true;
}

// Create hash from compact
int CscIdHelper::get_hash_calc   (const Identifier& compact_id,
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
      result = this->get_module_hash(compact_id, hash_id);
    }
    else if (m_DETECTORELEMENT_INDEX == end) {
      result = this->get_detectorElement_hash(compact_id, hash_id);
    }
    else if (m_CHANNEL_INDEX == end) {
      int stationIndex         = stationName(compact_id);
      int eta                  = this->stationEta(compact_id);
      int phi                  = this->stationPhi(compact_id);
      int chamberLayer         = this->chamberLayer(compact_id);
      int wireLayer            = this->wireLayer(compact_id);
      int orientation          = this->measuresPhi(compact_id);
      int stripNumber          = this->strip(compact_id);
      int etaIndex = -1;
      (eta == -1) ? etaIndex = 1 : etaIndex = 2;
      int chamberType = -1;
      isSmall(stationIndex) ? chamberType = 0 : chamberType = 1;
      int maxStrip = this->stripMax(compact_id);
      int offset = m_hashOffset[chamberType][orientation];
      //std::cout<<"station: "<<stationIndex<<", z: "<<eta<<", phi: "<<phi<<", chamber: "<<chamberLayer<<", wire: "<<wireLayer<<", mphi: "<<orientation<<", type: "<<chamberType<<", strip: "<<stripNumber<<", max: "<<maxStrip<<", offset: "<<offset<<std::endl;
 
      hash_id = offset + (stripNumber-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)+8*maxStrip*(phi-1)+64*maxStrip*(etaIndex-1);

      result = 0;
    }
  }
  return result;
}

// calculate the hash offset
int CscIdHelper::strip_hash_offsets()
{
  m_hashOffset[0][0] = 0;
  std::string version = m_dict->m_version;

  if ( version == "H8 2004") {
    m_hashOffset[0][1] = 1536;
    m_hashOffset[1][0] = m_hashOffset[0][0];
    m_hashOffset[1][1] = m_hashOffset[0][1];    
  }
  else if ( version == "CSC Cosmic") {
    m_hashOffset[0][1] = 3072;
    m_hashOffset[1][0] = m_hashOffset[0][0];
    m_hashOffset[1][1] = m_hashOffset[0][1];
  }
  else if ( version == "P.03" || version == "H8 2003" || version == "H8 2002" || version =="M2.8" ) {
    m_hashOffset[0][1] = 27392;
    m_hashOffset[1][0] = m_hashOffset[0][1]+3584;
    m_hashOffset[1][1] = m_hashOffset[0][1]+m_hashOffset[1][0];    
  } 
  else {
    m_hashOffset[0][1] = 24576;
    m_hashOffset[1][0] = m_hashOffset[0][1]+6144;
    m_hashOffset[1][1] = m_hashOffset[0][1]+m_hashOffset[1][0];
  }
  return 0;
}
