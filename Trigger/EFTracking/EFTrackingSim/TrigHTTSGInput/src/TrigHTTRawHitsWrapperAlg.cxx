/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"

#include "TrigHTTRawHitsWrapperAlg.h"


TrigHTTRawHitsWrapperAlg::TrigHTTRawHitsWrapperAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{}


StatusCode TrigHTTRawHitsWrapperAlg::initialize()
{
  ATH_MSG_DEBUG("TrigHTTRawHitsWrapperAlg::initialize()");


  ATH_CHECK(m_hitInputTool.retrieve());
  ATH_MSG_DEBUG("Setting HTT_SGHitInput tool");

  ATH_MSG_INFO("Creating output file: " << m_outpath);
  m_outfile = TFile::Open(m_outpath.value().c_str(), "recreate");

  m_eventHeader = new HTTEventInputHeader();

  ATH_MSG_DEBUG("instantiaiting tree");
  m_EventTree = new TTree("HTTEventTree", "data");

  ATH_MSG_DEBUG("Setting branch");
  m_EventTree->Branch("HTTEventInputHeader",
    "HTTEventInputHeader",
    &m_eventHeader, 6400, 1);

  ATH_MSG_DEBUG("Finished initialize");

  return StatusCode::SUCCESS;
}


StatusCode TrigHTTRawHitsWrapperAlg::execute() {
  ATH_MSG_DEBUG("Running on event ");

  ATH_CHECK(m_eventHeader != nullptr);
  ATH_CHECK(m_hitInputTool->readData(m_eventHeader, Gaudi::Hive::currentContext()));
  m_EventTree->Fill();
  return StatusCode::SUCCESS;

}


StatusCode TrigHTTRawHitsWrapperAlg::finalize()
{
  // close the output files, but check that it exists (for athenaMP)
  if (m_outfile) {
    m_outfile->Write();
    m_outfile->Close();
  }
  delete m_eventHeader;
  return StatusCode::SUCCESS;
}
