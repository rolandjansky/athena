/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPTool.cxx
#include "MuonTPTools/MuonTPTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "xAODEventInfo/EventInfo.h"

//**********************************************************************

MuonTPTool::MuonTPTool(std::string myname)
: AsgTool(myname),
  m_muonTPSelectionTools(),
  m_muonTPEfficiencyTools()
{
    declareProperty("MuonTPSelectionTools",  m_muonTPSelectionTools);
    declareProperty("MuonTPEfficiencyTools", m_muonTPEfficiencyTools);
    declareProperty("PlottingTools", m_TPPlots);
    declareProperty("EfficiencyFlag",        m_efficiencyFlag = "JPsiTP");

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

    // Create histograms
    for (auto tpPlot: m_TPPlots){
        ATH_CHECK(tpPlot->RegisterPlots(m_muonTPSelectionTools, m_muonTPEfficiencyTools));
    }

    return StatusCode::SUCCESS;
}


//**********************************************************************

void MuonTPTool::runTagAndProbe(const xAOD::MuonContainer* tags, const xAOD::IParticleContainer* probes, const xAOD::IParticleContainer* matches) const
{
    ATH_MSG_DEBUG("Number of candidate tags    : " << tags->size() );
    ATH_MSG_DEBUG("Number of candidate probes  : " << probes->size() );
    ATH_MSG_DEBUG("Number of candidate matches : " << matches->size() );

    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info).isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
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
        ProbeContainer* probeCont = tpSelTool->selectProbes(tags, probes);

        if (probeCont->size() > 0){
            for (auto plots: m_TPPlots){
                plots->fillCutFlow("at least 1 Probe",weight,tpSelTool);
            }
        }
        // Loop on TP efficiencies for each selection
        for(auto tpEffTool : m_muonTPEfficiencyTools) {

            // Match probes
            tpEffTool->matchProbes(probeCont, matches);

            // Fill efficiency histograms
            for(auto probe : *probeCont) {

                for (auto plots: m_TPPlots){
                    plots->fill(*probe, tpSelTool,tpEffTool);
                }
            }
        }

        // Clean up memory
        for (ProbeContainer::iterator p = probeCont->begin(); p != probeCont->end(); p++) delete *p;
        probeCont->clear();
        delete probeCont;
    }

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

void MuonTPTool::UpdateEfficiencies(){
    for (auto plots: m_TPPlots){
        plots->CalcEff();
    }
}

