/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : GaiaVariablesFactory.h
/// Package : BTagTools 
/// Author  : Dan Guest
/// Created : Sept 2013
///
/// DESCRIPTION:
///
/// Reads info from the jet, uses it to construct a JetFitterGenericTagInfo
/// object. 
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "JetTagTools/GaiaVariablesFactory.h"
#include "JetTagInfo/JetFitterGenericTagInfo.h"
#include "JetTagInfo/JetFitterTagInfo.h"

//#include "JetEvent/Jet.h"

namespace Analysis {


  GaiaVariablesFactory::GaiaVariablesFactory(const std::string& name,
								 const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_jetFitterInstance("JetFitterTagNN"),
    m_jetFitterCharmInstance("JetFitterCharm"), 
    m_gaiaInstance("Gaia")
  {
    declareProperty("JetFitterName",m_jetFitterInstance);
    declareProperty("JetFitterCharmName",m_jetFitterCharmInstance);
    declareProperty("GaiaName",m_gaiaInstance);

    declareInterface<IJetFitterVariablesFactory>(this);
  }  

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

GaiaVariablesFactory::~GaiaVariablesFactory() {}

StatusCode GaiaVariablesFactory::initialize() {
  ATH_MSG_INFO(" Initialization of GaiaVariablesFactory succesfull");
  return StatusCode::SUCCESS;
}

StatusCode GaiaVariablesFactory::finalize() {
  ATH_MSG_INFO(" Finalization of GaiaVariablesFactory succesfull");
  return StatusCode::SUCCESS;
}


/*  IJetFitterTagInfo* 
  GaiaVariablesFactory
  ::getITagInfoObject(const xAOD::Jet & myJet) const {
    

    //create new JetFitterTagInfo object
    JetFitterGenericTagInfo* generic_tag_info = new JetFitterGenericTagInfo
      (m_gaiaInstance);
    
    // JetFitterCharm variables 
    const JetFitterGenericTagInfo* jfc_info = myJet.tagInfo
      <JetFitterGenericTagInfo>(m_jetFitterCharmInstance); 
    if (jfc_info) { 
      add_jfc_info(jfc_info, generic_tag_info); 
    } else if (m_jetFitterCharmInstance.size() > 0) { 
      ATH_MSG_WARNING("no jetfittercharm instance " + 
		      m_jetFitterInstance + " found"); 
    }
    const IJetFitterTagInfo* cnn_info = myJet.tagInfo
      <JetFitterTagInfo>(m_jetFitterInstance);
    if (cnn_info) { 
      add_cnn_info(cnn_info, generic_tag_info); 
    } else if (m_jetFitterInstance.size() > 0) { 
      std::string keys; 
      std::vector<std::string> key_vec = myJet.infoKeys(); 
      for (std::vector<std::string>::const_iterator itr = key_vec.begin(); 
	   itr != key_vec.end(); itr++) { 
	keys.append(*itr + " "); 
      }
      ATH_MSG_WARNING("no jetfitter instance " + 
		      m_jetFitterInstance + " found: " + keys); 
    }
    return generic_tag_info;

  }
*/  
  void GaiaVariablesFactory
  ::add_jfc_info(const JetFitterGenericTagInfo* from, 
		 JetFitterGenericTagInfo* to) const { 

    typedef JetFitterGenericTagInfo::IntMap::const_iterator IntItr; 
    typedef JetFitterGenericTagInfo::DoubleMap::const_iterator DoubleItr; 
    for (IntItr in_itr = from->getIntsBegin(); 
	 in_itr != from->getIntsEnd(); in_itr++) { 
      to->setInt(in_itr->first, in_itr->second); 
    }
    for (DoubleItr db_itr = from->getDoublesBegin(); 
	 db_itr != from->getDoublesEnd(); db_itr++) { 
      to->setDouble(db_itr->first, db_itr->second); 
    }

  }
  void GaiaVariablesFactory
  ::add_cnn_info(const IJetFitterTagInfo* from, 
		 JetFitterGenericTagInfo* to) const { 
    to->setDouble("jfit_efrc",  from->energyFraction()); 
    to->setDouble("jfit_mass",  from->mass()); 
    to->setDouble("jfit_sig3d", from->significance3d()); 
    to->setInt("jfit_nvtx"    , from->nVTX()); 
    to->setInt("jfit_nvtx1t"  , from->nSingleTracks()); 
    to->setInt("jfit_ntrkAtVx", from->nTracksAtVtx()); 
  }
    

}//end Analysis namespace
