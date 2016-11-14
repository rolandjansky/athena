/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ThreadGaudi.h"


#include "TrigSteerMoni.h"

#include "TrigSteering/TrigSteer.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigInterfaces/AlgoConfig.h"

#include <vector>
#include <algorithm>
#include <TH1F.h>
#include <TH2F.h>


TrigSteerMoni::TrigSteerMoni(const std::string & type, const std::string & name,
			     const IInterface* parent)
  :  MonitorToolBase(type, name, parent)
{
    declareInterface<IMonitorToolBase>(this);
    declareProperty("HistoPath", m_histoPath = "/EXPERT/TrigSteering");
}


TrigSteerMoni::~TrigSteerMoni()
{
}

StatusCode TrigSteerMoni::initialize()
{
    m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
    if ( !m_parentAlg ) {
      msg() << MSG::ERROR << " Unable to cast the parent algorithm to HLT::TrigSteer !" << endmsg;
      return StatusCode::FAILURE;
    }

    msg() << MSG::INFO << "Finished initialize() of TrigSteerMoni" << endmsg;

    m_trigLvl = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ? "EF" : "HLT" ;


    return StatusCode::SUCCESS;
}



StatusCode TrigSteerMoni::bookHists()
{

  // service where to register stuff
  ITHistSvc *histSvc;
  if (!service("THistSvc", histSvc).isSuccess()) {
    msg() << MSG::ERROR << "Unable to locate THistSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<const HLT::SteeringChain*> configuredChains = m_parentAlg->getConfiguredChains();
  unsigned int totalNChains = configuredChains.size();

  unsigned int maxSteps = 0;
  for (const HLT::SteeringChain* chain : configuredChains ) {
     maxSteps = chain->getSignatures().size() > maxSteps ? chain->getSignatures().size() : maxSteps;
  }


  m_chainAcceptanceHist = new TH1F("ChainAcceptance",
                                   std::string("Raw acceptance of "+m_trigLvl+" chains").c_str(),
                                   totalNChains, 0.5, totalNChains+0.5);
  m_chainAcceptanceHist->GetXaxis()->SetTitle("Chain Counter");

  
  m_chainAcceptancePSHist = new TH1F("ChainAcceptancePS",
                                     std::string("Acceptance of "+m_trigLvl+" chains after Prescale").c_str(),
                                     totalNChains, 0.5, totalNChains+0.5);
  m_chainAcceptancePSHist->GetXaxis()->SetTitle("Chain Counter");


  m_chainAcceptancePTHist = new TH1F("ChainAcceptancePT",
                                     std::string("Final acceptance of "+m_trigLvl+" chains (after Prescale and PassThrough)").c_str(),
                                     totalNChains, 0.5, totalNChains+0.5);
  m_chainAcceptancePTHist->GetXaxis()->SetTitle("Chain Counter");


  //GF start
  m_signatureAcceptanceHist = new TH2F("SignatureAcceptance",
                                       std::string("Raw acceptance of fullfilled "+m_trigLvl+" signatures").c_str(),
                                       totalNChains, 0.5, totalNChains+0.5, maxSteps, 0.5, maxSteps+0.5);
  m_signatureAcceptanceHist->GetXaxis()->SetTitle("Chain counter");
  m_signatureAcceptanceHist->GetYaxis()->SetTitle("Signature (Chain step)");


  //GF end


  // book
  // 1D Histograms
  if ( histSvc->regHist(m_histoPath + m_chainAcceptanceHist->GetName(),
                        m_chainAcceptanceHist).isFailure())
     msg() << MSG::WARNING << "Can't book "
              << m_histoPath + m_chainAcceptanceHist->GetName() << endmsg;
  
  
  if ( histSvc->regHist(m_histoPath + m_chainAcceptancePSHist->GetName(),
                        m_chainAcceptancePSHist).isFailure())
     msg() << MSG::WARNING << "Can't book "
              << m_histoPath + m_chainAcceptancePSHist->GetName() << endmsg;
  
  if ( histSvc->regHist(m_histoPath + m_chainAcceptancePTHist->GetName(),
                        m_chainAcceptancePTHist).isFailure())
     msg() << MSG::WARNING << "Can't book "
              << m_histoPath + m_chainAcceptancePTHist->GetName() << endmsg;
  
  // 2D Histograms
  if ( histSvc->regHist(m_histoPath + m_signatureAcceptanceHist->GetName(),
                        m_signatureAcceptanceHist).isFailure())
     msg() << MSG::WARNING << "Can't book "
              << m_histoPath + m_signatureAcceptanceHist->GetName() << endmsg;
  

  return StatusCode::SUCCESS;
}

StatusCode TrigSteerMoni::fillHists()
{

  ++m_totalEvts;

  const std::vector<const HLT::SteeringChain*>& activeChains = m_parentAlg->getActiveChains();

  //  msg() << MSG::INFO << "Found " << activeChains.size() << " active Chains."
  //	    << endmsg;
  for (std::vector<const HLT::SteeringChain*>::const_iterator chain = activeChains.begin();
       chain != activeChains.end(); ++chain) {
    // msg() << MSG::INFO << "Chain name=" << (*it)->getChainId() << endmsg;
    Int_t bin = (Int_t)(*chain)->getChainCounter();
    float newAccept = (m_totalEvts - 1) * m_chainAcceptanceHist->GetBinContent( bin );
    float newAcceptPS = (m_totalEvts - 1) * m_chainAcceptancePSHist->GetBinContent( bin );
    float newAcceptPT = (m_totalEvts - 1) * m_chainAcceptancePTHist->GetBinContent( bin );

    if ( (*chain)->chainPassedRaw() ) ++newAccept;
    if ( ((*chain)->chainPassedRaw() && !(*chain)->isPrescaled()) ) ++newAcceptPS;
    if ( (*chain)->chainPassed()  ) ++newAcceptPT;

    m_chainAcceptanceHist->SetBinContent( bin, newAccept/m_totalEvts);
    m_chainAcceptancePSHist->SetBinContent( bin, newAcceptPS/m_totalEvts);
    m_chainAcceptancePTHist->SetBinContent( bin, newAcceptPT/m_totalEvts);


    for (unsigned int step = 0; step < (*chain)->getSignatures().size(); ++step) {
      float newSigAcc   = (m_totalEvts - 1) * m_signatureAcceptanceHist->GetBinContent(bin, step+1);
      if ( (int) step < (*chain)->getChainStep() ) ++newSigAcc;
      m_signatureAcceptanceHist->SetBinContent(bin, step+1, newSigAcc/m_totalEvts);
    }



  }
  return StatusCode::SUCCESS;
}

