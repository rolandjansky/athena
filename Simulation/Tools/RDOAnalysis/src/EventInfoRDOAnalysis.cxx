/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoRDOAnalysis.h"

#include <TH1.h>
#include <TTree.h>


EventInfoRDOAnalysis::EventInfoRDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode EventInfoRDOAnalysis::initialize()
{
  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_eventInfoKey.initialize() );

  // Grab Ntuple and histogramming service for tree
  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(m_ntupleName.value().c_str(), "EventInfoRDOAnalysis");
  ATH_CHECK(m_thistSvc->regTree(m_ntuplePath.value() + m_ntupleName.value(), m_tree));
  if (m_tree) {
    m_tree->Branch("runNumber", &m_runNumber);
    m_tree->Branch("eventNumber", &m_eventNumber);
    m_tree->Branch("lumiBlock", &m_lumiBlock);
    m_tree->Branch("bcid", &m_bcid);

    m_tree->Branch("actualInteractionsPerCrossing", &m_actualInteractionsPerCrossing);
    m_tree->Branch("averageInteractionsPerCrossing", &m_averageInteractionsPerCrossing);

    m_tree->Branch("beamPosX", &m_beamPosX);
    m_tree->Branch("beamPosY", &m_beamPosY);
    m_tree->Branch("beamPosZ", &m_beamPosZ);
    m_tree->Branch("beamPosSigmaX", &m_beamPosSigmaX);
    m_tree->Branch("beamPosSigmaY", &m_beamPosSigmaY);
    m_tree->Branch("beamPosSigmaZ", &m_beamPosSigmaZ);
  } else {
    ATH_MSG_ERROR("No tree found!");
    return StatusCode::FAILURE;
  }

  // HISTOGRAMS
  m_h_actualInteractionsPerCrossing = new TH1F("h_actualInteractionsPerCrossing", "actualInteractionsPerCrossing", 100, 0, 100);
  m_h_actualInteractionsPerCrossing->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_actualInteractionsPerCrossing->GetName(), m_h_actualInteractionsPerCrossing));

  m_h_averageInteractionsPerCrossing = new TH1F("h_averageInteractionsPerCrossing", "averageInteractionsPerCrossing", 100, 0, 100);
  m_h_averageInteractionsPerCrossing->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_averageInteractionsPerCrossing->GetName(), m_h_averageInteractionsPerCrossing));

  return StatusCode::SUCCESS;
}

StatusCode EventInfoRDOAnalysis::execute()
{
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  if (!eventInfo.isValid()) {
    return StatusCode::FAILURE;
  }

  m_runNumber = eventInfo->runNumber();
  m_eventNumber = eventInfo->eventNumber();
  m_lumiBlock = eventInfo->lumiBlock();
  m_bcid = eventInfo->bcid();

  m_actualInteractionsPerCrossing = eventInfo->actualInteractionsPerCrossing();
  m_averageInteractionsPerCrossing = eventInfo->averageInteractionsPerCrossing();
  m_h_actualInteractionsPerCrossing->Fill(m_actualInteractionsPerCrossing);
  m_h_averageInteractionsPerCrossing->Fill(m_averageInteractionsPerCrossing);

  m_beamPosX = eventInfo->beamPosX();
  m_beamPosY = eventInfo->beamPosY();
  m_beamPosZ = eventInfo->beamPosZ();

  m_beamPosSigmaX = eventInfo->beamPosSigmaX();
  m_beamPosSigmaY = eventInfo->beamPosSigmaY();
  m_beamPosSigmaZ = eventInfo->beamPosSigmaZ();

  if (m_tree) {
    m_tree->Fill();
  }

  return StatusCode::SUCCESS;
}
