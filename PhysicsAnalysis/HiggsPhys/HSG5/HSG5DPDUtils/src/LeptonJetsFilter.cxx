/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"

#include "HSG5DPDUtils/LeptonJetsFilter.h"

#include "JetEvent/JetCollection.h"
#include "muonEvent/MuonContainer.h"
#include "NavFourMom/INav4MomLinkContainer.h"

namespace HSG5 {

//________________________________________________________________________
LeptonJetsFilter::LeptonJetsFilter( const std::string& name,
				    ISvcLocator* pSvcLocator ) 
  : AthFilterAlgorithm(name,pSvcLocator)
  , m_nProcessed(0)
  , m_nEventPassTrigger(0)
{
  declareProperty("SelectedJetContainerName",  m_selectedJetContainerName="",  "Name of output jet container from JetSelector");
  declareProperty("SelectedMuonContainerName", m_selectedMuonContainerName="", "Name of output muon container from MuonSelector");

  declareProperty("MinSumJetsAndMuon", m_minSumJetsAndMuon=0,   "Minimum number of jets and muons combined");
  declareProperty("MaxSumJetsAndMuon", m_maxSumJetsAndMuon=999, "Maximum number of jets and muons combined");

}

//________________________________________________________________________
LeptonJetsFilter::~LeptonJetsFilter()
{

}

//________________________________________________________________________
StatusCode LeptonJetsFilter::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;


  return sc;
}

//________________________________________________________________________
StatusCode LeptonJetsFilter::finalize()
{
  float effiPassTrigger = 100. * m_nEventPassTrigger / float(m_nProcessed);
  float effiErrPassTrigger = 100. * std::sqrt(m_nEventPassTrigger) / float(m_nProcessed);

  ATH_MSG_INFO("==> finalize "<<this->name()<<"...");
  ATH_MSG_INFO("***************************************************************");
  ATH_MSG_INFO("Cut-flow table of "<<this->name()<<" skimming algorithm");
  ATH_MSG_INFO("-------------");
  ATH_MSG_INFO(" Number of processed events: "<<m_nProcessed);
  std::stringstream effstr;
  effstr<<" Events passing selection:  "<<m_nEventPassTrigger<<" and resulting efficiency = ("
	<<std::setw(3)<<effiPassTrigger<<" +/- "<<effiErrPassTrigger<<")";
  ATH_MSG_INFO(effstr.str());

  ATH_MSG_INFO("***************************************************************");
   
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode LeptonJetsFilter::execute()
{
  ATH_MSG_DEBUG("Start executing lepton-jet filter");

  ++m_nProcessed;

  StatusCode sc = StatusCode::SUCCESS;

  int sumJetsAndMuon=0;
  
  // get number output jets from StoreGate
  const INav4MomLinkContainer* outputJetContainer=0;
  if (evtStore()->contains<INav4MomLinkContainer>(m_selectedJetContainerName)) {
    sc = evtStore()->retrieve(outputJetContainer, m_selectedJetContainerName);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("No collection with name "<<m_selectedJetContainerName<<" found in StoreGate");
    }  
    else {
      ATH_MSG_DEBUG("found jet container, adding "<<outputJetContainer->size()<<" to sum");
      sumJetsAndMuon+=outputJetContainer->size();
    }
  }
  else
    ATH_MSG_DEBUG("StoreGate does not contain collection with name "<<m_selectedJetContainerName);

  // get output muons from StoreGate
  const INav4MomLinkContainer* outputMuonContainer=0;
  if (evtStore()->contains<INav4MomLinkContainer>(m_selectedMuonContainerName)) {
    sc = evtStore()->retrieve(outputMuonContainer, m_selectedMuonContainerName);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("No collection with name "<<m_selectedMuonContainerName<<" found in StoreGate");
    }  
    else {
      ATH_MSG_DEBUG("found muon container, adding "<<outputMuonContainer->size()<<" to sum");
      sumJetsAndMuon+=outputMuonContainer->size();
    }
  }
  else 
    ATH_MSG_DEBUG("StoreGate does not contain collection with name "<<m_selectedMuonContainerName);


  // check if passed selection cuts
  ATH_MSG_DEBUG("sumJetsAndMuon="<<sumJetsAndMuon<<", min: "<<m_minSumJetsAndMuon
		<<", max: "<<m_maxSumJetsAndMuon);
  if (sumJetsAndMuon>=m_minSumJetsAndMuon &&
      sumJetsAndMuon<=m_maxSumJetsAndMuon) {
    ATH_MSG_DEBUG("passed lepton+jets filter");
    setFilterPassed(true);
    ++m_nEventPassTrigger;
  }
  else {
    ATH_MSG_DEBUG("failed lepton+jets filter");
    setFilterPassed(false);
  }

  return StatusCode::SUCCESS;
}

} // end namespace
