/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes
#include <algorithm>
#include <iostream>

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthenaKernel/IThinningSvc.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
//Collections 
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/egamma.h"
//Actual Class 

#include "TrigDecisionTool/ChainGroup.h"
#include "DPDUtils/DPDPhotonFilter.h"


/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////// //////////////////////////////////////////////// 

/// Constructors
////////////////

DPDPhotonFilter::DPDPhotonFilter( const std::string& name,ISvcLocator* pSvcLocator ) : 
    AthAlgorithm (name, pSvcLocator), mLog( msgSvc(),name ), m_storeGate(0),
    m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  //
  // Property declaration
  // 
  declareProperty("PhotonCollectionName",m_PhotonCollectionName="PhotonAODCollection");
  declareProperty("PhotType",m_PhotonType);
  declareProperty("PhotEtCut",m_PhotonEtCut=10000);
  declareProperty("PhotEtaCut", m_PhotonEtaCut=2.5);
  declareProperty("TriggerChain", m_TriggerChain);
  declareProperty("TriggerSignatures",m_TriggerSignatures);
  declareProperty("TrigDecisionTool", m_trigDecisionTool,"Tool to access TrigDecision");
  declareProperty("UseTriggerSelection", m_UseTriggerSelection=false);      
}

/// Destructor
///////////////
DPDPhotonFilter::~DPDPhotonFilter()
{

}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode DPDPhotonFilter::initialize()
{

  All=0;
  pass=0;
  StatusCode  sc;
  
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
      return sc;
    }
  
  if ( !evtStore().retrieve().isSuccess() ) 
    {
      ATH_MSG_ERROR ("Unable to retrieve evtStore");
      return StatusCode::FAILURE;
    }
 
  sc = service("ThinningSvc", m_ThinningSvc);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("ThinningSvc not found");
    return sc;
  }
   
  ATH_MSG_INFO("... seeking TrigDecisionTool");
  sc = m_trigDecisionTool.retrieve();
  if ( StatusCode::SUCCESS != sc ||  m_trigDecisionTool == 0 ) {
     ATH_MSG_ERROR("Can't get handle on TrigDecsiionTool");
     return StatusCode::FAILURE;
  };

  ATH_MSG_INFO("Setting photon tightness ");

  if ( m_PhotonType == "loose" ) {
     m_PhotonTightness=egammaPID::PhotonLoose ;
  } else if ( m_PhotonType == "tight" ){
     m_PhotonTightness=egammaPID::PhotonTight ;
  } else {
     m_PhotonTightness=egammaPID::PhotonLoose ;
  }

  ATH_MSG_INFO( m_PhotonType << " photon selected : setting isem to " <<  m_PhotonTightness );      

  return StatusCode::SUCCESS;
}


StatusCode DPDPhotonFilter::finalize()
{
  ATH_MSG_DEBUG("DPDPhotonFilter PASSED "<<pass<<" FROM "<< All);
  return StatusCode::SUCCESS;
}


StatusCode DPDPhotonFilter::execute()
{  

  All++;
  StatusCode sc = StatusCode::SUCCESS;
  unsigned int photonCounter=0;
  std::vector<bool> vbool;

if (m_UseTriggerSelection){

///////////////////////////////////////////////////////////////////////////////
//////////////// Selection based on Trigger signatures
///////////////////////////////////////////////////////////////////////////////

  std::string TagMsg="TriggerSection::";
 
  m_gTrig      = m_trigDecisionTool->getChainGroup(m_TriggerChain);
  
  ATH_MSG_DEBUG("EF: "<<m_gTrig->isPassed());
  ATH_MSG_DEBUG("EF: "<<m_gTrig->getListOfTriggers());

  if ( m_gTrig->isPassed() ) {
        photonCounter++;
        vbool.push_back(true);
  }else{
	vbool.push_back(false);
  }


 }else{

///////////////////////////////////////////////////////////////////////////////
//////////////// Selection based on offline photon
///////////////////////////////////////////////////////////////////////////////
  
  /// read the AOD photon container from persistecy storage
  photTES=0;
  sc = evtStore()->retrieve( photTES, m_PhotonCollectionName);
  if( sc.isFailure()  ||  !photTES ) {
     ATH_MSG_ERROR("No AOD photon container found in TDS");
     return sc;
  }
  ATH_MSG_DEBUG("PhotonContainer successfully retrieved");
  ATH_MSG_DEBUG("Applying DPDPhotonFilter .... ");

  for(unsigned int i=0;i<photTES->size();++i) {

      ATH_MSG_DEBUG("Found a photon -->>>"<<photTES->at(i)->isem()<<"  "<<photTES->at(i)->et()<<" and  "<<fabs(photTES->at(i)->eta() )) ;

      if ( (photTES->at(i)->author(egammaParameters::AuthorPhoton) || (photTES->at(i)->author(egammaParameters::AuthorRConv) ) ) 
	  &&photTES->at(i)->et()>m_PhotonEtCut && fabs(photTES->at(i)->eta() ) < m_PhotonEtaCut
          &&photTES->at(i)->isem(m_PhotonTightness)==0) {
		  vbool.push_back(true);
		  photonCounter++;
      }else{
	  vbool.push_back(false);
      }
   }

  }

  if(photonCounter>0) {
      ATH_MSG_DEBUG("Filter Passed");
      setFilterPassed(true);
      pass++;
  } else {
      ATH_MSG_DEBUG("Filter Failed");
      setFilterPassed(false);
  }
    
//  if (m_ThinningSvc->filter(*photTES, vbool) != StatusCode::SUCCESS) {
//    ATH_MSG_ERROR("Error in Thinning");
//  }

  return StatusCode::SUCCESS;
}
