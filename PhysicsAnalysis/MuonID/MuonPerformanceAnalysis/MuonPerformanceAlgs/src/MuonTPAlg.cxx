/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPAlg.cxx 

#include "MuonTPAlg.h"
#include "GaudiKernel/Property.h"
#include "MuonTPTools/IMuonTPTool.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlots.h"
#include "xAODMuon/MuonContainer.h"
#include "GaudiKernel/ITHistSvc.h"
#include "xAODEventInfo/EventInfo.h"
using std::string;

//**********************************************************************

MuonTPAlg::MuonTPAlg(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator ),
  m_histSvc("THistSvc", name),
  m_tptools(),
  m_is_TruthOnData(false)
{
  declareProperty("MuonTPTools",        m_tptools);
  declareProperty("TagContainerName",   m_tagContainerName   = "Muons");
  declareProperty("ProbeContainerName", m_probeContainerName = "InDetTrackParticles");
  declareProperty("MatchContainerName", m_matchContainerName = "Muons");
  declareProperty("TopLevelFolderName", m_topLevelOutputFolderName = "UndefinedTagAndProbe");
}

//**********************************************************************

MuonTPAlg::~MuonTPAlg() { }

//**********************************************************************

StatusCode MuonTPAlg::initialize() {
  
  ATH_CHECK(m_histSvc.retrieve());
  ATH_CHECK(m_tptools.retrieve());
  
  // register histograms
  for(auto tool : m_tptools) {
    std::vector<HistData> histData = tool->retrieveBookedHistograms();

    for(auto hd : histData) {
      
      std::string histPath = hd.second+"/"+hd.first->GetName();
      m_histSvc->regHist("/MUONTP/"+m_topLevelOutputFolderName+"/"+histPath, hd.first).ignore(); //or check the statuscode
    }

    std::vector<std::pair <TTree*, std::string> > treeData = tool->retrieveBookedTrees();
    for(auto hd : treeData) {

      std::string treePath = hd.second+"/"+hd.first->GetName();
      m_histSvc->regTree("/MUONTP/"+m_topLevelOutputFolderName+"/"+treePath, hd.first).ignore(); //or check the statuscode
      ATH_MSG_DEBUG(std::string(" registered tree ")+"/MUONTP/"+treePath);
    }
    std::vector<std::pair <TGraph*, std::string> > graphData = tool->retrieveBookedGraphs();
    for(auto hd : graphData) {

      std::string graphPath = hd.second+"/"+hd.first->GetName();
      m_histSvc->regGraph("/MUONTP/"+m_topLevelOutputFolderName+"/"+graphPath, hd.first).ignore(); //or check the statuscode
      ATH_MSG_DEBUG(std::string(" registered graph ")+"/MUONTP/"+graphPath);
    }
  }
  
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode MuonTPAlg::finalize() {

	for(auto tool : m_tptools) {
	  tool->UpdateEfficiencies();
	}
	return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode MuonTPAlg::execute() {  


  // if this is truth-based, do not run on data.
  if (m_is_TruthOnData) return StatusCode::SUCCESS;


    // retrieve Event Info
  const xAOD::EventInfo* info = 0;
  if (evtStore()->retrieve(info).isFailure()){
      ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  }

  // retrieve tag (muon) container
  const xAOD::MuonContainer* tagContainer = 0;
  if(evtStore()->retrieve(tagContainer, m_tagContainerName).isFailure()) {
      if (!info->eventType(xAOD::EventInfo::IS_SIMULATION) && m_tagContainerName.find("Truth")!=std::string::npos){
          ATH_MSG_WARNING("Detected that the TP Alg is run with truth tags on a data sample. Will not produce any results!");
          m_is_TruthOnData=true;
          return StatusCode::SUCCESS;
      }
      else {
          ATH_MSG_FATAL( "Unable to retrieve " << m_tagContainerName );
          return StatusCode::FAILURE;
      }
  }
  
  // retrieve probe container
  const xAOD::IParticleContainer* probeContainer = 0;
  if(evtStore()->retrieve(probeContainer, m_probeContainerName).isFailure()) {
      if (!info->eventType(xAOD::EventInfo::IS_SIMULATION) && m_probeContainerName.find("Truth")!=std::string::npos){
          ATH_MSG_WARNING("Detected that the TP Alg is run with truth probes on a data sample. Will not produce any results!");
          m_is_TruthOnData=true;
          return StatusCode::SUCCESS;
      }
      else {
          ATH_MSG_FATAL( "Unable to retrieve " << m_probeContainerName );
          return StatusCode::FAILURE;
      }
  }

  // retrieve match container  
  const xAOD::IParticleContainer* matchContainer = 0;
  if(evtStore()->retrieve(matchContainer, m_matchContainerName).isFailure()) {
      if (!info->eventType(xAOD::EventInfo::IS_SIMULATION) && m_matchContainerName.find("Truth")!=std::string::npos){
          ATH_MSG_WARNING("Detected that the TP Alg is run with truth probes on a data sample. Will not produce any results!");
          m_is_TruthOnData=true;
          return StatusCode::SUCCESS;
      }
      else {
          ATH_MSG_FATAL( "Unable to retrieve " << m_matchContainerName );
          return StatusCode::FAILURE;
      }
  }
  
    // call the TP tool 
  for(auto tool : m_tptools) {
    tool->runTagAndProbe(tagContainer, probeContainer, matchContainer);
  }
  
  return StatusCode::SUCCESS;
}

//**********************************************************************
