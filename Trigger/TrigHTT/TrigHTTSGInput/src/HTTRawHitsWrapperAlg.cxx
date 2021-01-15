/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTSGInput/HTTRawHitsWrapperAlg.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"

#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"


HTTRawHitsWrapperAlg::HTTRawHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_hitInputTool("HTT_SGToRawHitsTool/HTT_SGToRawHitsTool"),
  m_eventHeader(nullptr)
{
  declareProperty("InputTool",  m_hitInputTool);
  declareProperty("OutFileName",      m_outpath= "httsim_smartwrapper.root", "output path");
}


StatusCode HTTRawHitsWrapperAlg::initialize()
{
  ATH_MSG_DEBUG ( "HTTRawHitsWrapperAlg::initialize()");
  
  
  ATH_CHECK(m_hitInputTool.retrieve());
  ATH_MSG_DEBUG ("Setting HTT_SGHitInput tool");
  
  ATH_MSG_INFO ("Creating output file: "  << m_outpath);
  m_outfile = TFile::Open(m_outpath.value().c_str(),"recreate");
  
  m_eventHeader = new HTTEventInputHeader();

  ATH_MSG_DEBUG ("instantiaiting tree");
  m_EventTree = new TTree("HTTEventTree","data");

  ATH_MSG_DEBUG ( "Setting branch");
  m_EventTree->Branch("HTTEventInputHeader",
		      "HTTEventInputHeader",
		      &m_eventHeader, 6400, 1);
  
  ATH_MSG_DEBUG ( "Finished initialize");
  
  return StatusCode::SUCCESS;
}


StatusCode HTTRawHitsWrapperAlg::execute() {
  ATH_MSG_DEBUG ("Running on event ");   

  if (m_eventHeader != nullptr) {
    if (m_hitInputTool->readData(m_eventHeader) != StatusCode::SUCCESS){
      ATH_MSG_FATAL ("Cannot retrieve data from the Tool");
      return StatusCode::FAILURE;
    }    
    m_EventTree->Fill();
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_FATAL("Hit Input Tool was null");
    return StatusCode::FAILURE;
  }
}


StatusCode HTTRawHitsWrapperAlg::finalize()
{

  // close the output files, but check that it exists (for athenaMP)
  if (m_outfile) {
    m_outfile->Write();
    m_outfile->Close();
  }
  return StatusCode::SUCCESS;
}
