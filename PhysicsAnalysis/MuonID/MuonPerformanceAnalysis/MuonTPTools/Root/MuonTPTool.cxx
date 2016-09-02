/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPTool.cxx
#include "MuonTPTools/MuonTPTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "xAODEventInfo/EventInfo.h"
//#include "GaudiKernel/ITHistSvc.h"

//**********************************************************************

MuonTPTool::MuonTPTool(std::string myname)
  : AsgTool(myname),
    //     m_histSvc("THistSvc", myname),
    m_muonTPSelectionTools(),
    m_muonTPEfficiencyTools(),
    m_inefficient_evts(0),
    m_evt(-1),
    m_run(-1),
    m_centeta(false)
{
  declareProperty("MuonTPSelectionTools",  m_muonTPSelectionTools);
  declareProperty("MuonTPEfficiencyTools", m_muonTPEfficiencyTools);
  declareProperty("PlottingTools",         m_TPPlots);
  declareProperty("TreeTools",             m_TPTrees);
  declareProperty("EfficiencyFlag",        m_efficiencyFlag = "JPsiTP");
  declareProperty("DumpInefficientEvents", m_dump_inefficient = false);

}

MuonTPTool::~MuonTPTool()
{
}


//**********************************************************************


StatusCode MuonTPTool::initialize()
{
  // Retrieve tools
  ATH_CHECK(m_muonTPSelectionTools.retrieve());
  ATH_CHECK(m_muonTPEfficiencyTools.retrieve());
  ATH_CHECK(m_TPPlots.retrieve());
  ATH_CHECK(m_TPTrees.retrieve());
  //ATH_CHECK(m_histSvc.retrieve());

  // ATH_MSG_INFO("==== TP Tools Sequence ====");
  // for (auto tpSel : m_muonTPSelectionTools) {
  //   ATH_MSG_INFO(" TP Selection : "<<tpSel->name());
  //   for (auto tpEff : m_muonTPEfficiencyTools)
  //     ATH_MSG_INFO("   TP Efficiency : "<<tpEff->name());
  //   for (auto tpPlot : m_TPPlots)
  //     ATH_MSG_INFO("   TP Plots : "<<tpPlot->name());
  // }
  // for (auto tpTree : m_TPTrees)
  //   ATH_MSG_INFO("TP Tree : "<<tpTree->name());
  // ATH_MSG_INFO("==== END of TP Tools Sequence ====");


  if (m_dump_inefficient){
    m_evt=-1;
    m_run=-1;
    m_centeta=false;
    m_inefficient_evts = new TTree((std::string("InefficientEvents")+m_efficiencyFlag).c_str(),"Inefficient Events");
    m_inefficient_evts->Branch("IsCentralEta",&m_centeta);
    m_inefficient_evts->Branch("Event",&m_evt);
    m_inefficient_evts->Branch("Run",&m_run);
  }
  // Create histograms
  for (auto tpPlot: m_TPPlots){
    ATH_CHECK(tpPlot->RegisterPlots(m_muonTPSelectionTools, m_muonTPEfficiencyTools));
  }
  for (auto tpTree: m_TPTrees){
    ATH_CHECK(tpTree->RegisterTrees(m_muonTPSelectionTools, m_muonTPEfficiencyTools));
  }

  return StatusCode::SUCCESS;
}


//**********************************************************************

void MuonTPTool::runTagAndProbe(const xAOD::MuonContainer* tags, const xAOD::IParticleContainer* probes, const xAOD::IParticleContainer* matches) 
{
	ATH_MSG_DEBUG("runTagAndProbe ");
  ATH_MSG_DEBUG("Number of candidate tags    : " << tags->size() );
  ATH_MSG_DEBUG("Number of candidate probes  : " << probes->size() );
  ATH_MSG_DEBUG("Number of candidate matches : " << matches->size() );

  const xAOD::EventInfo* info = 0;
  ATH_MSG_DEBUG("Picking up the EI ");
  ATH_MSG_DEBUG(""<<evtStore());
  if (evtStore()->retrieve(info, "EventInfo").isFailure()){
    ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  }

  bool ineff = false;
  if (m_dump_inefficient){
    m_centeta = true;
    m_run = info->runNumber();
    m_evt = info->eventNumber();
  }

  static bool isMC = info->eventType(xAOD::EventInfo::IS_SIMULATION);
  //     static double channel = (isMC ? info->mcChannelNumber() : info->runNumber());
  double weight = (isMC ? info->mcEventWeight() : 1.00);
  // Loop on TP selections
  for (auto tpSelTool : m_muonTPSelectionTools) {

    for (auto plots: m_TPPlots){
      plots->fillCutFlow("Processed Events",1,tpSelTool);
      plots->fillCutFlow("Processed Events (Weight)",weight,tpSelTool);
    }

    // Select probes
    ATH_MSG_DEBUG("TP Selection "<<tpSelTool->name());
    ProbeContainer* probeCont = tpSelTool->selectProbes(tags, probes);
    ATH_MSG_DEBUG("TP Selection OK");

    if (probeCont->size() > 0){
      for (auto plots: m_TPPlots){
	plots->fillCutFlow("at least 1 Probe",weight,tpSelTool);
      }
    }

    for(auto probe : *probeCont) {
      probe->HasSomeTrigger(false);
      probe->HasSomeTrigger_HLT(false);
    }

    // Loop on TP efficiencies for each selection
    for(auto tpEffTool : m_muonTPEfficiencyTools){
      // do not run 'double' variations
      if (!tpEffTool->isNominal() && !tpSelTool->isNominal()) continue;

      // Match probes
      ATH_MSG_DEBUG("Matching for "<<tpEffTool->name());
      tpEffTool->matchProbes(probeCont, matches);
      ATH_MSG_DEBUG("Matching OK");

      // Fill efficiency histograms
      for(auto probe : *probeCont)
	{
	  if (m_dump_inefficient && !probe->isMatched()){
	    ineff = true;
	    if (fabs(probe->eta()) > 0.1) m_centeta = false;
	  }
	  for (auto trees: m_TPTrees){
	    trees->updateMatch(*probe, tpEffTool);
	  }
	  for (auto plots: m_TPPlots){
	    plots->fill(*probe, tpSelTool,tpEffTool);
	  }
	}
    }

    for (auto trees: m_TPTrees){
    	ATH_MSG_DEBUG("Filling trees for "<<trees->name());
      for(auto probe : *probeCont) {
	trees->fill(*probe, tpSelTool);
      }
      ATH_MSG_DEBUG("Filling trees OK");
      trees->ForgetKnownProbes();
    }

    // Clean up memory
    for (ProbeContainer::iterator p = probeCont->begin(); p != probeCont->end(); p++) delete *p;
    probeCont->clear();
    delete probeCont;
  }

  if (m_dump_inefficient && ineff){
    m_inefficient_evts->Fill();
  }
  ATH_MSG_DEBUG("DONE ");
  return;
}
//**********************************************************************

std::vector<HistData> MuonTPTool::retrieveBookedHistograms()
{
  std::vector<HistData> histData;
  std::vector<HistData> histData_aux;
  for (auto plots: m_TPPlots){
    histData_aux = plots->retrieveBookedHistograms();
    histData.insert(histData.end(),histData_aux.begin(),histData_aux.end());
  }

  return histData;
}
std::vector<std::pair < TTree*, std::string> > MuonTPTool::retrieveBookedTrees()
{
  std::vector<std::pair < TTree*, std::string> > histData;
  std::vector<std::pair < TTree*, std::string> > histData_aux;
  for (auto trees: m_TPTrees){
    histData_aux = trees->retrieveBookedTrees();
    histData.insert(histData.end(),histData_aux.begin(),histData_aux.end());
  }
  if (m_dump_inefficient){
    histData.push_back(std::make_pair(m_inefficient_evts, ""));
  }

  return histData;
}
std::vector<std::pair < TGraph*, std::string> > MuonTPTool::retrieveBookedGraphs()
{
  std::vector<std::pair < TGraph*, std::string> > histData;
  std::vector<std::pair < TGraph*, std::string> > histData_aux;
  for (auto plots: m_TPPlots){
    histData_aux = plots->retrieveBookedGraphs();
    histData.insert(histData.end(),histData_aux.begin(),histData_aux.end());
  }

  return histData;
}

void MuonTPTool::UpdateEfficiencies(){
  for (auto plots: m_TPPlots){
    plots->CalcEff();
  }
}

