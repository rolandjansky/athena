/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_ANALYSIS
#include "VertexAnalysisAlgorithm.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "TH1.h"
#include "GaudiKernel/ITHistSvc.h"

using std::cout;
using std::endl;

VertexAnalysisAlgorithm::VertexAnalysisAlgorithm( const std::string& name, ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ), m_prwTool("CP::PileupReweightingTool") {

  declareProperty("SGKey"       , m_sgKey = "PrimaryVertices");
  declareProperty("SGKey2"      , m_sgKey2 = "TruthVertices");
  declareProperty("DirName"     , m_dirName = "IDPerformanceMon");
  declareProperty("MaxLayoutEta", m_MaxLayoutEta = 3.0);
  declareProperty("MuCorrected" , m_muCorrected = -1.0);
  declareProperty("MuWeight"    , m_muWeight = 1.0);
   
  declareProperty( "PRWTool", m_prwTool , "The pileup reweighting tool");
  
}

StatusCode VertexAnalysisAlgorithm::initialize() {

   // Greet the user:
   ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
   ATH_MSG_DEBUG( "SGKey = " << m_sgKey );


   // Setup hist service
   ServiceHandle<ITHistSvc> histSvc("THistSvc",name()); 
   CHECK( histSvc.retrieve() );


   // Use the analyzer to get all the histos to register
   std::vector<std::pair<TH1*,std::string> > vhists = m_analyzer.bookHistograms(m_MaxLayoutEta);
   for(auto & hit : vhists ) {
     std::string streamPath = std::string("/VertexAnalysis/") + m_dirName + "/" + hit.second + "/" + hit.first->GetName();
     // Register it
     CHECK(histSvc->regHist(streamPath, hit.first ));
   }
   
   // Return gracefully:
   return StatusCode::SUCCESS;
}

StatusCode VertexAnalysisAlgorithm::execute() {
  
  // Retrieve the event info:
  const xAOD::EventInfo* eventInfo = 0;
  CHECK( evtStore()->retrieve( eventInfo) );
  
  // Retrieve the vertices:
  const xAOD::VertexContainer * vxContainer = 0;
  ATH_CHECK( evtStore()->retrieve( vxContainer, m_sgKey ) );

  // Retrieve the truth vertices:
  const xAOD::TruthVertexContainer * tvxContainer = 0;
  ATH_CHECK( evtStore()->retrieve( tvxContainer, m_sgKey2 ) );

  // Check if MC or Data
  bool isMC = false;
  if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
    isMC = true;
  }

  //m_muCorrected = m_prwTool->getCorrectedMu( *eventInfo );
  //m_muCorrected = m_prwTool->getCorrectedActualInteractionsPerCrossing( *eventInfo );

  if(isMC == true){
    m_muCorrected = eventInfo->averageInteractionsPerCrossing();
    m_muWeight = m_prwTool->getCombinedWeight( *eventInfo );
  }

  else if(isMC == false){
    m_muCorrected = m_prwTool->getCorrectedAverageInteractionsPerCrossing( *eventInfo );
  }

  //cout << "ActualInteractionsPerCrossing = " << eventInfo->actualInteractionsPerCrossing() << endl;
  //cout << "CombinedWeight = " << m_muCorrected << endl;

  if(m_muCorrected == -1.0){
    m_muCorrected = eventInfo->actualInteractionsPerCrossing();
  }
  
  // Run the analysis method
  m_analyzer.execute(*vxContainer, *tvxContainer, *eventInfo, m_muCorrected, m_muWeight);
  
  return StatusCode::SUCCESS;
}

#endif
