/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "TrigSteerMonitor/TrigChainMoni.h"
#include "TrigSteering/TrigSteer.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/ResultBuilder.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigMonitorBase/TrigLockedHist.h"
#include <EventInfo/EventID.h>

#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/StoreGateSvc.h"

#include <vector>
#include <algorithm>
#include <TH1I.h>
#include <TAxis.h>

#include "boost/foreach.hpp"
#define foreach BOOST_FOREACH


TrigChainMoni::TrigChainMoni(const std::string & type, const std::string & name,
			     const IInterface* parent)
  :  TrigMonitorToolBase(type, name, parent),
     m_log(0),
     m_logLvl(0),
     m_histoPathshift(""),
     m_parentAlg(0),
     m_chainAcceptanceHist(0),
     m_chainAcceptancePSHist(0),
     m_chainAcceptancePTHist(0),
     m_activeChainsHist(0),
     m_trigLvl(""),
     m_useLBHistos(true)
 {
   //declareInterface<IMonitorToolBase>(this);

  declareProperty("HistoPathshift", m_histoPathshift = "/EXPERT/TrigSteering");
  //  declareProperty("ReserveLumiHistos", m_reserveLumiHistos = 1 );
  declareProperty("useLBHistos",m_useLBHistos=true);

}

TrigChainMoni::~TrigChainMoni()
{
}

StatusCode TrigChainMoni::finalize()
{ 
   if ( TrigMonitorToolBase::finalize().isFailure() ) {
    if(m_logLvl <= MSG::ERROR ) (*m_log) << MSG::ERROR  << " Unable to finalize base class !"
	     << endreq;
    return StatusCode::FAILURE;
  }
  delete m_log; m_log = 0;
  return StatusCode::SUCCESS;
}

StatusCode TrigChainMoni::endRun()
{ 
  
  return StatusCode::SUCCESS;
}

StatusCode TrigChainMoni::initialize()
{

  m_log = new MsgStream(msgSvc(), name());
  m_logLvl = m_log->level();

  if ( TrigMonitorToolBase::initialize().isFailure() ) {
    if(m_logLvl <= MSG::ERROR ) (*m_log) << MSG::ERROR  << " Unable to initialize base class !"
	     << endreq;
    return StatusCode::FAILURE;
  }
  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());

  if ( !m_parentAlg ) {
    if(m_logLvl <= MSG::ERROR ) (*m_log) << MSG::ERROR  << " Unable to cast the parent algorithm to HLT::TrigSteer !"
	     << endreq;
    return StatusCode::FAILURE;
  }

  if(m_logLvl <= MSG::INFO)(*m_log) << MSG::INFO << "Finished initialize() of TrigChainMoni"
				    << endreq;
  

  m_trigLvl = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ? "EF" : "HLT" ;
  
  StatusCode sc = service("StoreGateSvc", m_storeGate); 
  if(sc.isFailure()) {
    if(m_logLvl <= MSG::FATAL ) (*m_log) << MSG::FATAL << "Unable to get pointer to StoreGate Service"  << endreq;
    return sc;
  }
   
   return StatusCode::SUCCESS;
}

StatusCode TrigChainMoni::bookHists()
{
  if ( bookHistograms( false, false, true ).isFailure() ) {
    if(m_logLvl <= MSG::ERROR ) (*m_log) << MSG::ERROR << "Failure"  << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigChainMoni::bookHistograms( bool/* isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/ )
{

  TrigMonGroup expertHistograms( this, m_parentAlg->name(), expert);
  
  //Reset private members
  m_chainAcceptanceHist=0;
  m_chainAcceptancePSHist=0;
  m_chainAcceptancePTHist=0;
  m_activeChainsHist=0;
  
  //book
  std::vector<const HLT::SteeringChain*> cc = m_parentAlg->getConfiguredChains();
  unsigned int totalNChains = cc.size();

  std::map<std::string, unsigned int> chainname;

  /*
    MM, 29.7.11: I changed the way the binmap is made as well as the
    way the names get into the labels to address Savannah bug 79938
    which reported differences in histograms between xml and db
    setups, apparently because the bin ordering had changed. A map is
    used for the chainname array since the map is automatically sorted
    on key (alphabetically since no comparator is explicitly defined).
  */

  foreach(const HLT::SteeringChain* chain, cc) {
    chainname[chain->getChainName()] = chain->getChainCounter();
  }
  
  binnr = 0;
  for( std::map<std::string, unsigned int>::const_iterator it = chainname.begin(); it != chainname.end();
       it++) {
    m_binmap[ it->second ] = binnr++;
  }
  
  //run summary  histos
  if(m_logLvl <= MSG::DEBUG) (*m_log)<<MSG::DEBUG<<" now book runsummary histos for: " << m_parentAlg->name() <<endreq;

  // Histo: chain Acceptance
  std::string tmpstring_title  = "Raw acceptance of chains in " + m_trigLvl;
  std::string tmpstring_name  = "ChainAcceptance";
  m_chainAcceptanceHist=new TH1I(tmpstring_name.c_str(), tmpstring_title.c_str(), totalNChains,0.5,totalNChains+0.5);
  
  if ( expertHistograms.regHist(m_chainAcceptanceHist).isFailure()) {
    if(m_logLvl <= MSG::WARNING ) (*m_log) << MSG::WARNING  << "Can't book "
					   << m_histoPathshift+ m_chainAcceptanceHist->GetName() << endreq;
  }
  
  // Histo: chain Acceptance after PS
  tmpstring_title = "Raw acceptance of chains after Prescale in " + m_trigLvl;
  tmpstring_name  = "ChainAcceptancePS";
  m_chainAcceptancePSHist =new TH1I(tmpstring_name.c_str(),tmpstring_title.c_str(),totalNChains,0.5,totalNChains+0.5);
  
  if ( expertHistograms.regHist(m_chainAcceptancePSHist).isFailure()) {
    if(m_logLvl <= MSG::WARNING ) (*m_log) << MSG::WARNING  << "Can't book "
	     << m_histoPathshift+ m_chainAcceptancePSHist->GetName() << endreq;
  }
  
  // Histo: chain Acceptance after PT
  tmpstring_title  = "Final acceptance of chains (after Prescale and PassThrough) in " + m_trigLvl;
  tmpstring_name  = "ChainAcceptancePT";
  m_chainAcceptancePTHist =new TH1I(tmpstring_name.c_str(),tmpstring_title.c_str(),totalNChains,0.5,totalNChains+0.5);
  
  if ( expertHistograms.regHist(m_chainAcceptancePTHist).isFailure()) {
    if(m_logLvl <= MSG::WARNING ) (*m_log) << MSG::WARNING  << "Can't book "
	     << m_histoPathshift+ m_chainAcceptancePSHist->GetName() << endreq;
  }

  // Histo: active chains
  tmpstring_title  = "Number of active chains and sucessful events in " + m_trigLvl;
  tmpstring_name  = "NumberOfActiveChainsPerEvent";
  m_activeChainsHist = new TH1I(tmpstring_name.c_str(),tmpstring_title.c_str(),totalNChains,0.5,totalNChains+0.5);
  
  if ( expertHistograms.regHist(m_activeChainsHist).isFailure()) {
    if(m_logLvl <= MSG::WARNING ) (*m_log) << MSG::WARNING  << "Can't book "
	     << m_histoPathshift+ m_activeChainsHist->GetName() << endreq;
  }


  // Histo: rerun chains
  tmpstring_title  = "Number of run chains in decision evaluation " + m_trigLvl;
  tmpstring_name  = "NumberOfRunChainsPerEvent";
  m_runChainsHist = new TH1I(tmpstring_name.c_str(),tmpstring_title.c_str(),totalNChains,0.5,totalNChains+0.5);
  
  if ( expertHistograms.regHist(m_runChainsHist).isFailure()) {
    if(m_logLvl <= MSG::WARNING ) (*m_log) << MSG::WARNING  << "Can't book "
	     << m_histoPathshift+ m_runChainsHist->GetName() << endreq;
  }



  // Histo: rerun chains
  tmpstring_title  = "Number of rerun chains " + m_trigLvl;
  tmpstring_name  = "NumberOfRerunChains";
  m_rerunChainsHist = new TH1I(tmpstring_name.c_str(),tmpstring_title.c_str(),totalNChains,0.5,totalNChains+0.5);
  
  if ( expertHistograms.regHist(m_rerunChainsHist).isFailure()) {
    if(m_logLvl <= MSG::WARNING ) (*m_log) << MSG::WARNING  << "Can't book "
	     << m_histoPathshift+ m_rerunChainsHist->GetName() << endreq;
  }
  
  unsigned int tmpbin = 1;
  for( std::map<std::string, unsigned int>::const_iterator it = chainname.begin(); it != chainname.end();
       it++, tmpbin++) {
    const std::string &chName = it->first;
    m_chainAcceptanceHist   ->GetXaxis()->SetBinLabel(tmpbin,chName.c_str());
    m_chainAcceptancePSHist ->GetXaxis()->SetBinLabel(tmpbin,chName.c_str());
    m_chainAcceptancePTHist ->GetXaxis()->SetBinLabel(tmpbin,chName.c_str());
    m_activeChainsHist      ->GetXaxis()->SetBinLabel(tmpbin,chName.c_str());
    m_runChainsHist         ->GetXaxis()->SetBinLabel(tmpbin,chName.c_str());
    m_rerunChainsHist       ->GetXaxis()->SetBinLabel(tmpbin,chName.c_str());
  }
  
  // m_useLBHistos = true; // MM debugging

  return StatusCode::SUCCESS;
}








StatusCode TrigChainMoni::fillHists()
{
  m_useLBHistos = true; // MM debug

  //get lumiBlockNumber


  if( !m_chainAcceptanceHist|| 
      !m_chainAcceptancePSHist|| 
      !m_chainAcceptancePTHist|| 
      !m_activeChainsHist ||
      !m_rerunChainsHist ){
    if(m_logLvl <= MSG::WARNING) (*m_log)<<MSG::WARNING<<" pointers to histograms not ok, dont Fill ! "<<endreq;
    return StatusCode::FAILURE;  
  }
  

  //fill histos 

  const std::vector<const HLT::SteeringChain*>& activeChains = m_parentAlg->getActiveChains();

  for (std::vector<const HLT::SteeringChain*>::const_iterator chain = activeChains.begin();
       chain != activeChains.end(); ++chain) {
    Int_t chaincounter = (Int_t)(*chain)->getChainCounter();

    const HLT::SteeringChain* ch = *chain;

    // fill the raw histogram
    // m_binmap runs from 0 ... totalNChains -1 but binning starts with 1
    // for the first chain --> fill m_binmap[chaincounter]+1 !

    int bin = m_binmap[chaincounter]+1;

    m_activeChainsHist->Fill(bin);

    if ( (ch->runInFirstPass() || ch->isPrescaled()) && !ch->runInSecondPass() ) 
      m_runChainsHist->Fill(bin);

    if ( ch->runInSecondPass()) {
      m_rerunChainsHist->Fill(bin);
      continue;
    }    
    if ( ch->chainPassedRaw() ) {
      m_chainAcceptanceHist->Fill(bin);
    }
    if ( !ch->isPrescaled() ) {
      // fill the prescaled histogram
      m_chainAcceptancePSHist->Fill(bin);
    }
    if ( ch->chainPassed()  ) {
      // fill the pass through histogram
      m_chainAcceptancePTHist->Fill(bin);
    }

  }
  return StatusCode::SUCCESS;
}

StatusCode TrigChainMoni::finalHists()
{
  //print statements for validation
  if( !m_chainAcceptanceHist || 
      !m_chainAcceptancePSHist || 
      !m_chainAcceptancePTHist || 
      !m_activeChainsHist ||
      !m_rerunChainsHist ){
    if(m_logLvl <= MSG::WARNING) (*m_log)<<MSG::WARNING<<" pointers to histograms not ok, dont Fill ! "<<endreq;
    return StatusCode::FAILURE;  
  }
    
  std::string   tmpstring = "REGTEST Print Statistics:  Number of accepted events per chain :  raw and after prescale, pass through ";
  if (m_logLvl <= MSG::INFO)(*m_log)<<MSG::INFO<<tmpstring<<endreq;
  // sort histograms by labels (LabelsOption("a") fixes order of bins to be sorted alphabetically 
  lock_histogram_operation<TH1I>(m_chainAcceptanceHist)->GetXaxis()->LabelsOption("a");
  lock_histogram_operation<TH1I>(m_chainAcceptancePSHist)->GetXaxis()->LabelsOption("a");
  lock_histogram_operation<TH1I>(m_chainAcceptancePTHist)->GetXaxis()->LabelsOption("a");
  lock_histogram_operation<TH1I>(m_activeChainsHist)->GetXaxis()->LabelsOption("a");
  lock_histogram_operation<TH1I>(m_runChainsHist)->GetXaxis()->LabelsOption("a");
  lock_histogram_operation<TH1I>(m_rerunChainsHist)->GetXaxis()->LabelsOption("a");
  
  for(int i=1;i<=m_chainAcceptanceHist->GetNbinsX();i++){    
    /*
      (*m_log)<<MSG::INFO<<" REGTEST chain "<<m_chainAcceptanceHist_runsummary-> GetXaxis()->GetBinLabel(i)<<" accepted events= "<<
      m_chainAcceptanceHist_runsummary->GetBinContent(i) <<" ( PS: "<<
      m_chainAcceptancePSHist_runsummary->GetBinContent(i) <<" , PT: "<<
      m_chainAcceptancePTHist_runsummary->GetBinContent(i) <<")"<<endreq;
    */
    
    if (m_logLvl <= MSG::INFO)
      (*m_log)<<MSG::INFO<<" REGTEST events accepted by chain: " 
	      << std::setw(35) << std::left << m_chainAcceptanceHist->GetXaxis()->GetBinLabel(i) 
	      <<"  active: " << std::setw(5) << std::fixed <<  int(m_activeChainsHist->GetBinContent(i))
	      <<"  run :" << std::setw(5) << std::fixed << int(m_runChainsHist->GetBinContent(i)) 
	      <<"  PS: "<< std::setw(5) << std::fixed << int(m_chainAcceptancePSHist->GetBinContent(i)) 
	      <<"  raw: " << std::setw(5) << std::fixed <<  int(m_chainAcceptanceHist->GetBinContent(i))
	      <<"  accepted after PS and PT: "<< std::setw(5) << std::fixed << int(m_chainAcceptancePTHist->GetBinContent(i)) 
	      <<"  rerun: " << std::setw(5) << std::fixed <<  int(m_rerunChainsHist->GetBinContent(i))  <<endreq;        
  }
  return StatusCode::SUCCESS;
}
