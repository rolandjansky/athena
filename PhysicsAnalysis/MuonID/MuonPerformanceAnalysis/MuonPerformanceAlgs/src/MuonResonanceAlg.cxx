/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonResonanceAlg.cxx 

#include "MuonResonanceAlg.h"
#include "GaudiKernel/Property.h"
#include "MuonResonanceTools/IMuonResonanceTool.h"
//#include "MuonPerformanceHistUtils/MuonResonanceEfficiencyPlots.h"
#include "xAODMuon/MuonContainer.h"
#include "GaudiKernel/ITHistSvc.h"
#include "xAODEventInfo/EventInfo.h"

using std::string;

//**********************************************************************

MuonResonanceAlg::MuonResonanceAlg(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator ),
  m_histSvc("THistSvc", name),
  m_restools()
{
  declareProperty("MuonResonanceTools",        m_restools);
  declareProperty("MuonContainerName",   m_muonContainerName   = "Muons");
}

//**********************************************************************

MuonResonanceAlg::~MuonResonanceAlg() { }

//**********************************************************************

StatusCode MuonResonanceAlg::initialize() {
  
  ATH_CHECK(m_histSvc.retrieve());
  ATH_CHECK(m_restools.retrieve());
  
  // register histograms
  for(auto tool : m_restools) {
    std::vector<HistData> histData = tool->retrieveBookedHistograms();
    for(auto hd : histData) {
      std::string histPath = hd.second+"/"+hd.first->GetName();
      m_histSvc->regHist("/MUONRES/"+histPath, hd.first).ignore(); //or check the statuscode
    }
   }
  
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode MuonResonanceAlg::finalize() {

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode MuonResonanceAlg::execute() {  

  const xAOD::EventInfo* eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
  if (!eventInfo){
    ATH_MSG_ERROR("Could not retrieve EventInfo. Returning 1");
    return StatusCode::FAILURE;
  }
  bool isMC = eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ); 
  ATH_MSG_DEBUG("isSimulation =  " << isMC ); 


  // Retrieve tag (muon) container
  const xAOD::MuonContainer* muonContainer = 0;
  if(evtStore()->retrieve(muonContainer, m_muonContainerName).isFailure()) {
    ATH_MSG_FATAL( "Unable to retrieve " << m_muonContainerName );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "in ResAlg execute" );

  
  // call the Resonance tool 
  ATH_MSG_DEBUG( "size of m_tools "<<m_restools.size() );


  for(auto tool : m_restools) {

    ATH_MSG_DEBUG( "calling tool ");

    tool->findResonance(muonContainer, isMC);
    ATH_MSG_DEBUG( "called tool ");
  }
  
  return StatusCode::SUCCESS;
}

//**********************************************************************
