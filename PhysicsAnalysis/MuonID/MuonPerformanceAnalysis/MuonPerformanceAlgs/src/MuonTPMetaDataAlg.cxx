/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonPerformanceAlgs includes
#include "MuonTPMetaDataAlg.h"

#include <GaudiKernel/ITHistSvc.h>
#include <xAODCutFlow/CutBookkeeperContainer.h>

#include <EventBookkeeperMetaData/EventBookkeeperCollection.h>
#include <xAODEventInfo/EventInfo.h>
#include <EventInfo/EventStreamInfo.h>
#include <xAODMetaData/FileMetaData.h>

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
            m_containsCutBookKeeper(false),
            m_TotEvents(0),
            m_ProcEvents(0),
            m_RunNumber(0),
            m_mcChannelNumber(-1),
            m_SumOfWeights(0.),
            m_SumOfWeights2(0.),
            m_TotalLumiBlocks(),
            m_ProcessedLumiBlocks() {
    //declareProperty( "Property", m_nProperty ); //example property declaration
}

MuonTPMetaDataAlg::~MuonTPMetaDataAlg() {
}

StatusCode MuonTPMetaDataAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    m_MetaDataTree = new TTree("MetaDataTree", "MetaData tree of the MuonTPanalysis");
    m_MetaDataTree->Branch("TotalEvents", &m_TotEvents);
    m_MetaDataTree->Branch("ProcessedEvents", &m_ProcEvents);
    m_MetaDataTree->Branch("isData", &m_isData);
    m_MetaDataTree->Branch("isDerivedAOD", &m_isDerivedAOD);
    m_MetaDataTree->Branch("mcChannelNumber", &m_mcChannelNumber);
    m_MetaDataTree->Branch("runNumber", &m_RunNumber);
    m_MetaDataTree->Branch("TotalSumW", &m_SumOfWeights);
    m_MetaDataTree->Branch("TotalSumW2", &m_SumOfWeights2);
    m_MetaDataTree->Branch("TotalLumiBlocks", &m_TotalLumiBlocks);
    m_MetaDataTree->Branch("ProcessedLumiBlocks", &m_ProcessedLumiBlocks);
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
        m_ProcessedLumiBlocks.insert(evtInfo->lumiBlock());
    }
    FillTree(isData, runNumber, mcChannel);
    if (m_containsCutBookKeeper) return StatusCode::SUCCESS;
    if (isData) {
        ATH_MSG_DEBUG("Append lumiblock: " << evtInfo->lumiBlock());
        m_TotalLumiBlocks.insert(evtInfo->lumiBlock());
    }
    ++m_TotEvents;
    float weight = isData ? 1. : (*evtInfo->mcEventWeights().begin());
    m_SumOfWeights += weight;
    m_SumOfWeights2 += weight * weight;
    ATH_MSG_DEBUG("Total events: " << m_TotEvents << " ProcEvents: " << m_ProcEvents << " SumOfWeights: " << m_SumOfWeights << " SumOfWeights2: " << m_SumOfWeights2);

    return StatusCode::SUCCESS;
}
const xAOD::CutBookkeeper* MuonTPMetaDataAlg::RetrieveCutBookKeeper(const std::string &Stream){
    const xAOD::CutBookkeeper* all = 0;        
    if (inputMetaStore()->contains < xAOD::CutBookkeeperContainer > ("CutBookkeepers")) {
        const xAOD::CutBookkeeperContainer* bks = 0;
        if(!inputMetaStore()->retrieve(bks, "CutBookkeepers").isSuccess()){
            ATH_MSG_WARNING("Could not retrieve the CutBookKeeperContainer. Although it should be there");
            return all;
        }
        int maxCycle = -1; //need to find the max cycle where input stream is StreamAOD and the name is AllExecutedEvents
        for (auto cbk : *bks) {
            if (cbk->inputStream() == Stream && cbk->name() == "AllExecutedEvents" && cbk->cycle() > maxCycle) {
                maxCycle = cbk->cycle();
                all = cbk;
            }
        }
    }
    return all;
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
    const xAOD::FileMetaData* fmd = 0;
    if (inputMetaStore()->contains < xAOD::FileMetaData > ("FileMetaData")) ATH_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));
    if (!fmd) {
        ATH_MSG_WARNING("FileMetaData not found in input file, setting m_isDerivedAOD=false.");
        m_isDerivedAOD = false;
    }
    else {
        std::string dataType;
        if (!(fmd->value(xAOD::FileMetaData::MetaDataType::dataType, dataType))) {
            ATH_MSG_WARNING("MetaDataType::dataType not found in xAOD::FileMetaData, setting m_isDerivedAOD=false.");
            m_isDerivedAOD = false;
        }
        if (dataType.find("DAOD")!=std::string::npos) m_isDerivedAOD = true;
    }
    int mcChannelNumber = esi->getEventTypes().begin()->mc_channel_number();
    int runNumber = (*esi->getRunNumbers().begin());
    FillTree(isData, runNumber, mcChannelNumber);

    //Copy the information about the LumiBlocks
    for (const auto& Lumi : esi->getLumiBlockNumbers()) {
        m_TotalLumiBlocks.insert(Lumi);
    }
    const xAOD::CutBookkeeper* all = RetrieveCutBookKeeper("StreamAOD");
    if (!all){
        all = RetrieveCutBookKeeper("unknownStream");
        if (!all) m_containsCutBookKeeper = false;
        else m_containsCutBookKeeper = true;
    } else m_containsCutBookKeeper = true;
    if (m_containsCutBookKeeper) {
        m_TotEvents += all->nAcceptedEvents();
        m_SumOfWeights += all->sumOfEventWeights();
        m_SumOfWeights2 += all->sumOfEventWeightsSquared();
        ATH_MSG_INFO("CutBookKeeper: -- Total Events: " << all->nAcceptedEvents() << " SumOfWeights: " << all->sumOfEventWeights() << " SumOfWeights2: " << all->sumOfEventWeightsSquared());        
    } else ATH_MSG_WARNING("No CutBookKeeper information could be extracted");
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
        m_containsCutBookKeeper = false;
        m_ProcessedLumiBlocks.clear();
        m_TotalLumiBlocks.clear();
    }
}
