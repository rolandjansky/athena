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
    AthAlgorithm (name, pSvcLocator),
    m_All(0),
    m_pass(0),
    m_EventCounter(0),
    m_PhotonTightness(0),
    m_ThinningSvc ("ThinningSvc", name),
    m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_gTrig(nullptr)
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

  m_All=0;
  m_pass=0;
  
  ATH_CHECK( m_ThinningSvc.retrieve() );
  ATH_CHECK(  m_trigDecisionTool.retrieve() );

  if ( m_PhotonType == "loose" ) {
     m_PhotonTightness=egammaPIDObs::PhotonLoose ;
  } else if ( m_PhotonType == "tight" ){
     m_PhotonTightness=egammaPIDObs::PhotonTight ;
  } else {
     m_PhotonTightness=egammaPIDObs::PhotonLoose ;
  }

  ATH_MSG_INFO( m_PhotonType << " photon selected : setting isem to " <<  m_PhotonTightness );      

  return StatusCode::SUCCESS;
}


StatusCode DPDPhotonFilter::finalize()
{
  ATH_MSG_DEBUG("DPDPhotonFilter PASSED "<<m_pass<<" FROM "<< m_All);
  return StatusCode::SUCCESS;
}


StatusCode DPDPhotonFilter::execute()
{  

  m_All++;
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
  const PhotonContainer* photTES=0;
  ATH_CHECK(  evtStore()->retrieve( photTES, m_PhotonCollectionName) );
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
  m_pass++;
} else {
  ATH_MSG_DEBUG("Filter Failed");
  setFilterPassed(false);
}
    
//  if (m_ThinningSvc->filter(*m_photTES, vbool) != StatusCode::SUCCESS) {
//    ATH_MSG_ERROR("Error in Thinning");
//  }

  return StatusCode::SUCCESS;
}
