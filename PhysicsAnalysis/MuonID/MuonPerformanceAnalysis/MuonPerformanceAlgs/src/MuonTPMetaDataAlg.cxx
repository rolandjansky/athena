/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonPerformanceAlgs includes
#include "MuonTPMetaDataAlg.h"

#include <GaudiKernel/ITHistSvc.h>
#include <xAODCutFlow/CutBookkeeper.h>

#include <EventBookkeeperMetaData/EventBookkeeperCollection.h>
#include <xAODCutFlow/CutBookkeeperContainer.h>
#include <xAODEventInfo/EventInfo.h>
#include <EventInfo/EventStreamInfo.h>

//uncomment the line below to use the HistSvc for outputting trees and histograms
//#include "GaudiKernel/ITHistSvc.h"
#include <TTree.h>
//#include "TH1D.h
MuonTPMetaDataAlg::MuonTPMetaDataAlg(const std::string& name, ISvcLocator* pSvcLocator) :
            AthAnalysisAlgorithm(name, pSvcLocator),
            m_histSvc("THistSvc", name),
            m_MetaDataTree(NULL),
            m_isData(false),
            m_isDerivedAOD(false),
            m_TotEvents(0),
            m_ProcEvents(0),
            m_RunNumber(0),
            m_mcChannelNumber(-1),
            m_SumOfWeights(0.),
            m_SumOfWeights2(0.),
            m_TotalLumi(),
            m_ProcessedLumi() {
    //declareProperty( "Property", m_nProperty ); //example property declaration
}

MuonTPMetaDataAlg::~MuonTPMetaDataAlg() {
}

StatusCode MuonTPMetaDataAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    m_MetaDataTree = new TTree("MetaDataTree", "Metadata tree of the MuonTPanalysis");
    m_MetaDataTree->Branch("TotalEvents", &m_TotEvents);
    m_MetaDataTree->Branch("ProcessedEvents", &m_ProcEvents);
    m_MetaDataTree->Branch("isData", &m_isData);
    m_MetaDataTree->Branch("mcChannelNumber", &m_mcChannelNumber);
    m_MetaDataTree->Branch("runNumber", &m_RunNumber);
    m_MetaDataTree->Branch("SumOfWeights", &m_SumOfWeights);
    m_MetaDataTree->Branch("SumOfWeights2", &m_SumOfWeights2);
    m_MetaDataTree->Branch("TotalLumiBlockIds", &m_TotalLumi);
    m_MetaDataTree->Branch("ProcessedLumiBlockIds", &m_ProcessedLumi);
    CHECK(m_histSvc->regTree("/MUONTP/MetaDataTree", m_MetaDataTree));

    return StatusCode::SUCCESS;
}

StatusCode MuonTPMetaDataAlg::finalize() {
    ATH_MSG_INFO("Finalizing " << name() << "...");
    m_MetaDataTree->Fill();
    return StatusCode::SUCCESS;
}

StatusCode MuonTPMetaDataAlg::execute() {
    ATH_MSG_DEBUG("Executing " << name() << "...");
    setFilterPassed(true);
    const xAOD::EventInfo* evtInfo = 0;
    CHECK(evtStore()->retrieve(evtInfo, "EventInfo"));
    ++m_ProcEvents;
    bool isData = !evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
    int mcChannel = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ? evtInfo->mcChannelNumber() : 0.;
    int runNumber = evtInfo->runNumber();
    if (isData) {
        ATH_MSG_DEBUG("Append lumiblock: " << evtInfo->lumiBlock());
        m_ProcessedLumi.insert(evtInfo->lumiBlock());
    }
    FillTree(isData, runNumber, mcChannel);
    if (m_isDerivedAOD) return StatusCode::SUCCESS;
    if (isData) {
        ATH_MSG_DEBUG("Append lumiblock: " << evtInfo->lumiBlock());
        m_TotalLumi.insert(evtInfo->lumiBlock());
    }
    ++m_TotEvents;
    float weight = isData ? 1. : (*evtInfo->mcEventWeights().begin());
    m_SumOfWeights += weight;
    m_SumOfWeights2 += weight * weight;
    ATH_MSG_DEBUG("Total events: " << m_TotEvents << " ProcEvents: " << m_ProcEvents << " SumOfWeights: " << m_SumOfWeights << " SumOfWeights2: " << m_SumOfWeights2);

    return StatusCode::SUCCESS;
}

StatusCode MuonTPMetaDataAlg::beginInputFile() {
    //
    //This method is called at the start of each input file, even if
    //the input file contains no events. Accumulate metadata information here
    //
    const EventStreamInfo* esi = 0;
    CHECK(inputMetaStore()->retrieve(esi));
    if (esi->getEventTypes().size() > 1) {
        ATH_MSG_WARNING("There seem to be more event types than one");
    }
    bool isData = !esi->getEventTypes().begin()->test(EventType::IS_SIMULATION);
    int mcChannelNumber = esi->getEventTypes().begin()->mc_channel_number();
    int runNumber = (*esi->getRunNumbers().begin());
    FillTree(isData, runNumber, mcChannelNumber);

    //Copy the information about the LumiBlocks
    for (const auto& Lumi : esi->getLumiBlockNumbers()) {
        m_TotalLumi.insert(Lumi);
    }
    //Check the file for the CutBookKeeeper and fill it with the sum of weights
    if (inputMetaStore()->contains < xAOD::CutBookkeeperContainer > ("CutBookkeepers")) {
        const xAOD::CutBookkeeperContainer* bks = 0;
        CHECK(inputMetaStore()->retrieve(bks, "CutBookkeepers"));
        const xAOD::CutBookkeeper* all = 0;
        int maxCycle = -1; //need to find the max cycle where input stream is StreamAOD and the name is AllExecutedEvents
        for (auto cbk : *bks) {
            if (cbk->inputStream() == "StreamAOD" && cbk->name() == "AllExecutedEvents" && cbk->cycle() > maxCycle) {
                maxCycle = cbk->cycle();
                all = cbk;
            }
        }
        if (!all){
            ATH_MSG_WARNING("CutBookKeeeperContainer found but no CutBookeper element named 'StreamAOD'");
            for (auto cbk : *bks) {
                if (cbk->inputStream() == "unknownStream" && cbk->name() == "AllExecutedEvents" && cbk->cycle() > maxCycle) {
                    maxCycle = cbk->cycle();
                    all = cbk;
                }
            }
        }
        if (!all){
            ATH_MSG_ERROR("No CutBookKeeper information found. Although the container is there. Please check input");
            m_isDerivedAOD=false;
            return StatusCode::FAILURE;
        }
        m_TotEvents += all->nAcceptedEvents();
        m_SumOfWeights += all->sumOfEventWeights();
        m_SumOfWeights2 += all->sumOfEventWeightsSquared();
        m_isDerivedAOD = true;
        ATH_MSG_INFO("CutBookKeeper: -- Total Events: " << all->nAcceptedEvents() << " SumOfWeights: " << all->sumOfEventWeights() << " SumOfWeights2: " << all->sumOfEventWeightsSquared());
    } else m_isDerivedAOD = false;
    return StatusCode::SUCCESS;
}
void MuonTPMetaDataAlg::FillTree(bool isData, unsigned int runNumber, int mcChannelNumber) {
    if (isData != m_isData || (m_isData && m_RunNumber != runNumber) || (!m_isData && m_mcChannelNumber != mcChannelNumber)) {
        if (m_mcChannelNumber > -1) m_MetaDataTree->Fill();
        m_mcChannelNumber = mcChannelNumber;
        m_RunNumber = runNumber;
        m_isData = isData;
        m_TotEvents = 0;
        m_ProcEvents = 0;
        m_SumOfWeights = 0.;
        m_SumOfWeights2 = 0.;
        m_isDerivedAOD = false;
        m_ProcessedLumi.clear();
        m_TotalLumi.clear();
    }
}
