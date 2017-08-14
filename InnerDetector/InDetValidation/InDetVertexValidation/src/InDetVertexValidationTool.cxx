/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
// InDetVertexValidation includes
#include "InDetVertexValidation/InDetVertexValidationTool.h"
//#include "InDetVertexValidation/InDetVertexPlots.h"

#include "GaudiKernel/ITHistSvc.h"

#include <vector>
#include "xAODEventInfo/EventInfo.h"

#include <limits>
#include <cmath> //to get std::isnan()

using std::pair;
using std::make_pair;

using std::cout;
using std::endl;

//Parametrized constructor
InDetVertexValidationTool::InDetVertexValidationTool(const std::string & type, const std::string & name, const IInterface* parent):
  ManagedMonitorToolBase(type, name, parent)
{
  //declareProperty( "Property", m_nProperty ); //example property declaration
  declareProperty("EventInfoContainerName"        , m_eventInfoContainerName = "EventInfo");
  declareProperty("VertexContainerName"           , m_vertexContainerName = "PrimaryVertices");
  declareProperty("TruthVertexContainerName"      , m_truthvertexContainerName = "TruthVertices");
  declareProperty("TrackContainerName"            , m_trackContainerName = "InDetTrackParticles");
  declareProperty("truthEventContainerName"       , m_trutheventContainerName = "TruthEvents");
  declareProperty("truthPileupEventContainerName" , m_trutheventPUContainerName = "TruthPileupEvents");
  //declareProperty("TrackSelectionTool"          , m_trackSelectionTool);
  //declareProperty("SubFolder"                   , m_folder);
  declareProperty("DirName"                       , m_dirName = "IDPerformanceMon");
  declareProperty("MaxLayoutEta"                  , m_MaxLayoutEta = 3.0);
  declareProperty("MuCorrected"                   , m_muCorrected = -1.0);
  declareProperty("MuWeight"                      , m_muWeight = 1.0);
  
  //declareProperty("PRWTool"                     , m_prwTool);
  declareProperty("TightTrackVertexAssoTool"      , m_tighttrackvertexassoTool);
  declareProperty("LooseTrackVertexAssoTool"      , m_loosetrackvertexassoTool);
  //declareProperty("MCTruthClassifier"             , m_truthClassifier);
}

InDetVertexValidationTool::~InDetVertexValidationTool(){
}

/*StatusCode InDetVertexValidationTool::initialize(){
    ATH_MSG_DEBUG ("Initializing " << name() << "...");
    //ATH_CHECK(ManagedMonitorToolBase::initialize());

    //m_monPlots = std::move(std::unique_ptr<InDetVertexPlots> (new InDetVertexPlots(0,m_dirName+m_folder)));

    return StatusCode::SUCCESS;
    }*/

StatusCode InDetVertexValidationTool::fillHistograms(){
  ATH_MSG_DEBUG ("Filling hists " << name() << "...");

  // Retrieve the tracks:
  const xAOD::TrackParticleContainer* ptrack = 0;
  ptrack = getContainer<xAOD::TrackParticleContainer>(m_trackContainerName);
  
  // Retrieve the vertices:
  const xAOD::VertexContainer* pvertex = 0;
  pvertex = getContainer<xAOD::VertexContainer>(m_vertexContainerName);

  xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_loose;
  xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_tight;

  /*if (pvertex) {
    m_monPlots->fill(*pvertex);
    } else {
    ATH_MSG_WARNING("Cannot open " << m_vertexContainerName << " vertex container. Skipping vertexing plots.");
    }*/
  
  // Retrieve the event info:  
  const xAOD::EventInfo* pei = getContainer<xAOD::EventInfo>(m_eventInfoContainerName);
  
  if (pei) {

    if(pei->eventType(xAOD::EventInfo::IS_SIMULATION) == true){
      CHECK(m_loosetrackvertexassoTool.retrieve());
      CHECK(m_tighttrackvertexassoTool.retrieve());

      trkvxassoUniqueMap_loose = m_loosetrackvertexassoTool->getUniqueMatchMap(*ptrack, *pvertex);
      trkvxassoUniqueMap_tight = m_tighttrackvertexassoTool->getUniqueMatchMap(*ptrack, *pvertex);
    }

    // Retrieve the truth vertices:
    const xAOD::TruthVertexContainer* tvertex = 0;
    if(pei->eventType(xAOD::EventInfo::IS_SIMULATION) == true){tvertex = getContainer<xAOD::TruthVertexContainer>(m_truthvertexContainerName);}
    
    // Retrieve the truth events:
    const xAOD::TruthEventContainer* tevent = 0;
    bool Truthevents_available = false;
    if(pei->eventType(xAOD::EventInfo::IS_SIMULATION) == true){
      if(evtStore()->contains<xAOD::TruthEventBaseContainer>(m_trutheventContainerName)){
	tevent = getContainer<xAOD::TruthEventContainer>(m_trutheventContainerName);
	Truthevents_available = true;
      }
    }
    
    // Retrieve the truth pileup events:
    const xAOD::TruthPileupEventContainer* tPUevent = 0;
    bool TruthPUevents_available = false;
    if(pei->eventType(xAOD::EventInfo::IS_SIMULATION) == true){
      if(evtStore()->contains<xAOD::TruthEventBaseContainer>(m_trutheventPUContainerName)){
	tPUevent = getContainer<xAOD::TruthPileupEventContainer>(m_trutheventPUContainerName);
	TruthPUevents_available = true;
      }
    }

    //m_muCorrected = m_prwTool->getCorrectedMu( *pei );
    //m_muCorrected = m_prwTool->getCorrectedActualInteractionsPerCrossing( *pei );
    
    // prwTool not enabled to the moment, set m_muCorrected to -1
    m_muCorrected = -1.0;

    if(m_muCorrected == -1.0){
      m_muCorrected = pei->actualInteractionsPerCrossing();
    }

    if(pvertex) {
      // Run the analysis method
      analyzer.execute(*pvertex, *tvertex, *pei, m_muCorrected, m_muWeight, *ptrack, *tevent, *tPUevent, trkvxassoUniqueMap_loose, trkvxassoUniqueMap_tight, Truthevents_available, TruthPUevents_available);
    }
  } else {
    ATH_MSG_WARNING("Cannot open " << m_eventInfoContainerName << " EventInfo container. Skipping vertexing plots using EventInfo.");      
  }
  
  return StatusCode::SUCCESS;
}

StatusCode InDetVertexValidationTool::bookHistograms(){
  ATH_MSG_INFO ("Booking hists " << name() << "...");
  
  //m_monPlots->setDetailLevel(100); //DEBUG, enable expert histograms
  //m_monPlots->initialize();
  //std::vector<HistData> hists = m_monPlots->retrieveBookedHistograms();
  
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  CHECK( histSvc.retrieve() );

  std::vector<std::pair<TH1*,std::string> > vhists = analyzer.bookHistograms(m_MaxLayoutEta);

  //for (auto hist : hists){
  for(auto & hit : vhists ){
    //ATH_CHECK(regHist(hist.first,hist.second,all)); //??

    std::string streamPath = m_dirName + "/" + hit.second;
    ATH_CHECK(regHist(hit.first,streamPath,all));
  } 
  
  return StatusCode::SUCCESS;
}


StatusCode InDetVertexValidationTool::procHistograms() {
  ATH_MSG_INFO ("Finalising hists " << name() << "...");
  
  if (endOfRun){
    //m_monPlots->finalize();
    analyzer.procHistograms();
  }
  
  ATH_MSG_INFO ("Successfully finalized hists");
  
  return StatusCode::SUCCESS;
}
#endif
