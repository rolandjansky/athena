/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "TrigJetMonitoring/HLTJetMonTool.h"

#include "TrigObjectMatching/TrigMatchTool.h"
#include "TrigObjectMatching/TrigMatchDefs.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "JetEvent/JetCollection.h"

#include "JetUtils/JetCaloQualityUtils.h"

#include "EventKernel/SignalStateHelper.h"

#include <iostream>
#include <math.h>

#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"

#include "boost/tokenizer.hpp"

// ------------------------------------------------------------------------------------

HLTJetMonTool::HLTJetMonTool(   
    const std::string & type, 
    const std::string & name, 
    const IInterface* parent) : IHLTMonTool(type, name, parent),
                                m_trigMatchTool("TrigMatchTool/TrigMatchTool"){

  //m_tdt( "Trig::TrigDecisionTool/TrigDecisionTool" )

  ATH_MSG_INFO( "in HLTJetMonTool::HLTJetMonTool() " );

  clearVectors();

  m_debuglevel = true;

  //declareProperty( "TrigDecisionTool",   m_tdt);

  declareProperty("TrigMatchTool",       m_trigMatchTool); 
  declareProperty("JetMonBase",          m_monBase = "/HLT/JetMon");

  declareProperty("DoL1Efficiency",      m_doL1TrigEff = true);
  declareProperty("DoOfflineJets",       m_doOFJets = true);
  declareProperty("DoHLTEfficiency",     m_doHLTTrigEff = true);

  declareProperty("L1xAODJetKey",        m_L1xAODJetKey = "LVL1JetRoIs");
  declareProperty("HLTJetKeys",          m_HLTJetKeys, "SG Keys to access HLT Jet Collections");
  declareProperty("OFJetKeys",           m_OFJetKeys, "SG Keys to access offline Jet Collections" );

  declareProperty("L1JetDir",            m_L1dir = "/L1CaloJet_Fex" );
  declareProperty("HLTJetDir",           m_HLTdir = "/HLTJetRec_Fex" );
  declareProperty("EfficiencyDir",       m_Effdir = "/JetTrigEff" );
  declareProperty("HLTJetDirPrefix",     m_HLTpfx = "HLT");
  declareProperty("OFJetDirPrefix",      m_OFpfx = "OFAlg" );
  
  declareProperty("monitoring_l1jet",	 m_monitoring_l1jet, "Primary L1 monitoring triggers");
  declareProperty("monitoring_jet",	 m_monitoring_jet, "Primary HLT monitoring triggers");
  //declareProperty("BasicL1Chains",     m_basicL1Trig, "L1 Chains for Basic Jet Trigger Histograms");
  //declareProperty("BasicHLTChains",    m_basicHLTTrig, "HLT Chains for Basic Jet Trigger Histograms");

  declareProperty("primary_l1jet",	 m_primary_l1jet, "Primary L1 triggers for Jet Efficiency");
  declareProperty("primary_jet",	 m_primary_jet,   "Primary HLT triggers for Jet Efficiency");
  declareProperty("hltContainers",	 m_hltContainers, "Primary HLT containers");
  //declareProperty("L1ItemNames",       m_L1Items, "L1 Items for Jet Trigger Efficiency");
  //declareProperty("HLTChainNames",     m_HLTChains, "HLT Chains for Jet Trigger Efficiency");
  
  declareProperty("L1EtThresGeV",        m_l1EtThres, "L1 ET Thresholds for L1 chains" );
  declareProperty("HLTEtThresGeV",       m_hltEtThres, "HLT ET Thresholds for EF chains" );
  declareProperty("OFEtThresGeV",        m_ofEtThres, "OF ET Thresholds for Efficiency" );

  // Jet Multiplicity bins
  declareProperty("NJetNBins",        m_njnbins );
  declareProperty("NJetBinLo",        m_njbinlo );
  declareProperty("NJetBinHi",        m_njbinhi );
  
  // Jet ET bins
  declareProperty("JetEtNBins",       m_jEtnbins );
  declareProperty("JetEtBinLo",       m_jEtbinlo );
  declareProperty("JetEtBinHi",       m_jEtbinhi );

  // Jet eta bins
  declareProperty("JetetaNBins",      m_jetanbins );
  declareProperty("JetetaBinLo",      m_jetabinlo );
  declareProperty("JetetaBinHi",      m_jetabinhi );
  
  // Jet phi bins
  declareProperty("JetphiNBins",      m_jphinbins );
  declareProperty("JetphiBinLo",      m_jphibinlo );
  declareProperty("JetphiBinHi",      m_jphibinhi );
  
  // Jet EMFrac bin 
  declareProperty("JetemfracNBins",   m_jemfracnbins );
  declareProperty("JetemfracBinLo",   m_jemfracbinlo );
  declareProperty("JetemfracBinHi",   m_jemfracbinhi );

  // Jet HECFrac bin
  declareProperty("JethecfracNBins",   m_jhecfracnbins );
  declareProperty("JethecfracBinLo",   m_jhecfracbinlo );
  declareProperty("JethecfracBinHi",   m_jhecfracbinhi );
  
  // Jet DeltaET bins
  declareProperty("JetDEtNBins",      m_jDEtnbins );
  declareProperty("JetDEtBinLo",      m_jDEtbinlo );
  declareProperty("JetDEtBinHi",      m_jDEtbinhi );
  
  // Jet DeltaEta/DeltaPhi bins
  declareProperty("JetDepNBins",      m_jDepnbins );
  declareProperty("JetDepBinLo",      m_jDepbinlo );
  declareProperty("JetDepBinHi",      m_jDepbinhi );
  
  declareProperty("L1EffNBinsEt",      m_l1nbinsEt );
  declareProperty("L1EffBinLoEtGeV",   m_l1binloEt );
  declareProperty("L1EffBinHiEtGeV",   m_l1binhiEt );
    
  declareProperty("HLTEffNBinsEt",      m_hltnbinsEt );
  declareProperty("HLTEffBinLoEtGeV",   m_hltbinloEt );
  declareProperty("HLTEffBinHiEtGeV",   m_hltbinhiEt );

  //declareProperty("JetChainsRegex",     m_chainsByRegexp);

  declareProperty("EMFractionCut",      m_emfracCut);

  declareProperty("DoOFJetSelectionForBasicHists",     m_doselOFBasicHists = false);
  declareProperty("DoOFJetSelectionForTrigEff",        m_doselOFJets = false);

  declareProperty("DoOFMinJetPtCut",      m_reqMinPtCut = true);
  declareProperty("OFMinJetPtGeV",        m_MinPtCut = 10.);

  declareProperty("DoOFAbsJetEtaCut",     m_reqEtaCut = true);
  declareProperty("OFAbsJetEta",          m_EtaCut = 3.2);
  
  declareProperty("DoOFMaxNJetCut",       m_reqMaxNJetCut = true);
  declareProperty("OFMaxNJet",            m_MaxNJet = -1);
  
  //declareProperty("DoOFJetEMFCut",      m_reqEMFracCut = true);
  //declareProperty("OFEMFracCut",        m_ofemfracCut);

  declareProperty("DeltaRCut",          m_deltaRCut = 0.2);

  // Use jet energy at this scale
  // JETEMSCALE (same as UNCALIBRATED), JETFINAL (same as CALIBRATED)
  declareProperty("DoOFSignalState", m_reqP4State = true);
  declareProperty("OFSignalState", m_p4State = "JETFINAL");

  declareProperty("DoOFJetQualityCut",     m_reqBadQCut = true);
  declareProperty("OFJetQuality",          m_JetQuality = "LooseBad" );

  /*
  // n90 => no. of constituents having 90% of jet energy
  declareProperty("DoOFMinN90JetCut", m_reqN90Cut = true);
  declareProperty("OFMinN90Jet", m_n90Cut = 5);

  // Out-of-time Calo Jet Energy (not to exceed X ns)
  declareProperty("DoOFMaxJetTimeCut", m_reqTimeCut = true);
  declareProperty("OFMaxTimens", m_jetTimens = 50);

  // Remove Jets with Bad Quality (fraction not to exceed X)
  declareProperty("OFMaxBadQFracJet",  m_badQFrac = 0.8);
  */

} // end HLTJetMonTool c'tor

// ------------------------------------------------------------------------------------
void HLTJetMonTool::clearVectors() {
  //m_ofemfracCut.clear();
  m_HLTJetC.clear();
  m_OFJetC.clear();

  m_monitoring_l1jet.clear();
  m_monitoring_jet.clear();
  m_primary_l1jet.clear();
  m_primary_jet.clear();
  
  m_basicL1Trig.clear();
  m_basicHLTTrig.clear();

  m_L1Items.clear();
  m_HLTChains.clear();
  m_hltContainers.clear();

  m_l1EtThres.clear();
  m_hltEtThres.clear();
  m_ofEtThres.clear();

  // Jet Multiplicity bins
  m_njnbins.clear();
  m_njbinlo.clear();
  m_njbinhi.clear();

  // Jet ET bins
  m_jEtnbins.clear();
  m_jEtbinlo.clear();
  m_jEtbinhi.clear();

  // Jet eta bins
  m_jetanbins.clear();
  m_jetabinlo.clear();
  m_jetabinhi.clear();

  // Jet phi bins
  m_jphinbins.clear();
  m_jphibinlo.clear();
  m_jphibinhi.clear();

  // Jet emfrac bins
  m_jemfracnbins.clear();
  m_jemfracbinlo.clear();
  m_jemfracbinhi.clear();

  // Jet hecfrac bins
  m_jhecfracnbins.clear();
  m_jhecfracbinlo.clear();
  m_jhecfracbinhi.clear();

  // Jet DeltaET bins
  m_jDEtnbins.clear();
  m_jDEtbinlo.clear();
  m_jDEtbinhi.clear();

  // Jet DeltaEta/DeltaPhi bins
  m_jDepnbins.clear();
  m_jDepbinlo.clear();
  m_jDepbinhi.clear();

  m_l1nbinsEt.clear();
  m_hltnbinsEt.clear();
  m_l1binloEt.clear();
  m_hltbinloEt.clear();
  m_l1binhiEt.clear();
  m_hltbinhiEt.clear();
}
// ------------------------------------------------------------------------------------

HLTJetMonTool::~HLTJetMonTool() {
  ATH_MSG_INFO( "in HLTJetMonTool::~HLTJetMonTool() " );
} // end ~HLTJetMonTool

// ------------------------------------------------------------------------------------

StatusCode HLTJetMonTool::init() {

  // init message stream
  m_log->setLevel(outputLevel());
  m_debuglevel = (m_log->level() <= MSG::DEBUG);

  ATH_MSG_INFO( "in HLTJetMonTool::init()" );

  // Monitoring trigger names specified through the congfiguration file or menu database, for menu-aware monitoring
  if(m_monitoring_l1jet.empty()) m_monitoring_l1jet.push_back("L1_J15");
  if(m_monitoring_jet.empty())   m_monitoring_jet.push_back("j60");
  if(m_primary_l1jet.empty())    m_primary_l1jet.push_back("L1_J15");
  if(m_primary_l1jet.empty())    m_monitoring_l1jet.push_back("j60");

  // Fill triggers for L1 basic plots. String pairs of the form ("MonGroup name","L1 Trigger Name")
  unsigned int k = 0; 
  for( ; k< m_monitoring_l1jet.size(); ++k) {
    ATH_MSG_INFO( "monitoring_l1jet : " << m_monitoring_l1jet[k].c_str() );
    m_basicL1Trig.insert(std::pair<std::string,std::string>(m_monitoring_l1jet[k].c_str(),m_monitoring_l1jet[k].c_str()));
  }
 
  // Fill trigger for HLT basic plots. String pairs of the form ("MonGroup name","HLT Filter Name")
  k = 0;
  for( ; k < m_monitoring_jet.size(); ++k) {
    ATH_MSG_INFO( "monitoring_jet : " << m_monitoring_jet[k].c_str() );
    m_basicHLTTrig.insert(std::pair<std::string,std::string>( m_monitoring_jet[k].c_str(), m_monitoring_jet[k].c_str()));
  }
  
  // L1 Triggers for efficiency plots. String pairs of the form ("MonGroup name","L1 Trigger Name")
  k = 0;
  for( ; k < m_primary_l1jet.size(); ++k) {
    ATH_MSG_INFO( "primary_l1jet : " << m_primary_l1jet[k].c_str() );
    m_L1Items.insert(std::pair<std::string,std::string>(m_primary_l1jet[k].c_str(),m_primary_l1jet[k].c_str()));
  }
 
  // HLT chains for efficiencyplots. String pairs of the form ("MonGroup name","HLT Filter Name")
  k = 0;
  for( ; k < m_primary_jet.size(); ++k) {
    ATH_MSG_INFO( "primary_jet : " << m_primary_jet[k].c_str() );
    m_HLTChains.insert(std::pair<std::string,std::string>(m_primary_jet[k].c_str(),m_primary_jet[k].c_str()));
  }

  // defaults for bins
  // Jet Multiplicity bins
  if(m_njnbins.empty()){m_njnbins.push_back(51);}
  if(m_njbinlo.empty()){m_njbinlo.push_back(-0.5);}
  if(m_njbinhi.empty()){m_njbinhi.push_back(50.5);}

  // Jet ET bins
  if(m_jEtnbins.empty()){m_jEtnbins.push_back(110);}
  if(m_jEtbinlo.empty()){m_jEtbinlo.push_back(1209.5);}
  if(m_jEtbinhi.empty()){m_jEtbinhi.push_back(-0.5);}

  // Jet eta bins
  if(m_jetanbins.empty()){m_jetanbins.push_back(84);}
  if(m_jetabinlo.empty()){m_jetabinlo.push_back(-6.0);}
  if(m_jetabinhi.empty()){m_jetabinhi.push_back(6.0);}

  // Jet phi bins
  if(m_jphinbins.empty()){m_jphinbins.push_back(35);}
  if(m_jphibinlo.empty()){m_jphibinlo.push_back(-1.115 * M_PI);}
  if(m_jphibinhi.empty()){m_jphibinhi.push_back(1.115 * M_PI);}

  // Jet emfrac bins
  if(m_jemfracnbins.empty()){m_jemfracnbins.push_back(52);}
  if(m_jemfracbinlo.empty()){m_jemfracbinlo.push_back(-0.02);}
  if(m_jemfracbinhi.empty()){m_jemfracbinhi.push_back(1.02);}

  //Jet hecfrac bins
  if(m_jhecfracnbins.empty()){m_jhecfracnbins.push_back(52);}
  if(m_jhecfracbinlo.empty()){m_jhecfracbinlo.push_back(-0.02);}
  if(m_jhecfracbinhi.empty()){m_jhecfracbinhi.push_back(1.02);}

  // Jet DeltaET bins
  if(m_jDEtnbins.empty()){m_jDEtnbins.push_back(50);}
  if(m_jDEtbinlo.empty()){m_jDEtbinlo.push_back(-100.0);}
  if(m_jDEtbinhi.empty()){m_jDEtbinhi.push_back(100.0);}

  // Jet DeltaEta/DeltaPhi bins
  if(m_jDepnbins.empty()){m_jDepnbins.push_back(12);}
  if(m_jDepbinlo.empty()){m_jDepbinlo.push_back(-6.0);}
  if(m_jDepbinhi.empty()){m_jDepbinhi.push_back(6.0);}

  // defaults for HLT jet keys
  if(m_HLTJetKeys.empty()) { 
    m_HLTJetKeys.insert(std::pair<std::string,std::string>("a4tcemsubjesFS","HLT_xAOD__JetContainer_a4tcemsubjesFS"));
  }

  // defaults for offline jet keys
  if(m_OFJetKeys.empty()) { 
    m_OFJetKeys.insert(std::pair<std::string,std::string>("AntiKt4EMTopoJets","AntiKt4EMTopoJets"));
  }

  // defaults for L1 items used in efficiencies
  if(m_L1Items.empty()) {
    m_L1Items.insert(std::pair<std::string,std::string>("L1_J15","L1_J15"));
    m_L1Items.insert(std::pair<std::string,std::string>("L1_J100","L1_J100"));
    
    // defaults for L1 ET Thresholds
    m_l1EtThres.insert(std::pair<std::string,double>("L1_J15",8.));
    m_l1EtThres.insert(std::pair<std::string,double>("L1_J100",20.));
    m_ofEtThres.insert(std::pair<std::string,double>("L1_J15",30.));
    m_ofEtThres.insert(std::pair<std::string,double>("L1_J100",75.));

    // bins, binlo, binhi
    m_l1nbinsEt.clear();
    m_l1nbinsEt.push_back(25); 
    m_l1nbinsEt.push_back(40); 
    m_l1binloEt.clear();
    m_l1binloEt.push_back(40.);
    m_l1binloEt.push_back(60.);
    m_l1binhiEt.clear();
    m_l1binhiEt.push_back(140.);
    m_l1binhiEt.push_back(220.);
    
  }

  /*if(m_reqEMFracCut) {
    if(m_ofemfracCut.empty() || m_ofemfracCut.size() > 2) {
      m_ofemfracCut.clear();
      m_ofemfracCut.push_back(0.2);
      m_ofemfracCut.push_back(0.95);
    }
  }*/

  // defaults for HLT chains used in efficiencies
  if(m_HLTChains.empty()) {
    m_HLTChains.insert(std::pair<std::string,std::string>("j60","j60"));
    m_HLTChains.insert(std::pair<std::string,std::string>("j360","j360"));
    
    // defaults for EF ET Thresholds
    m_hltEtThres.insert(std::pair<std::string,double>("j60",8.));
    m_hltEtThres.insert(std::pair<std::string,double>("j360",20.));
    m_ofEtThres.insert(std::pair<std::string,double>("j60",30.));
    m_ofEtThres.insert(std::pair<std::string,double>("j360",95.));
    
    // bins, binlo, binhi
    m_hltnbinsEt.clear();
    m_hltnbinsEt.push_back(60); 
    m_hltnbinsEt.push_back(45); 
    m_hltbinloEt.clear();
    m_hltbinloEt.push_back(100.);
    m_hltbinloEt.push_back(180.);
    m_hltbinhiEt.clear();
    m_hltbinhiEt.push_back(250.);
    m_hltbinhiEt.push_back(360.);
  }

  if (m_debuglevel) {
    unsigned int ich = 0;
    for(JetSigIter it = m_L1Items.begin(); it != m_L1Items.end(); ++it, ich++) {
      ATH_MSG_DEBUG( (*it).first << "\tThr: " 
          << m_l1EtThres[(*it).first] << "\t\tEtBins(" << m_l1nbinsEt[ich] << "," << m_l1binloEt[ich] << "," << m_l1binhiEt[ich] << ")" );
    }
    ich = 0;
    for(JetSigIter it = m_HLTChains.begin(); it != m_HLTChains.end(); ++it, ich++) {
      ATH_MSG_DEBUG( (*it).first << "\tThr: " 
          << m_hltEtThres[(*it).first] << "\t\tEtBins(" << m_hltnbinsEt[ich] << "," << m_hltbinloEt[ich] << "," << m_hltbinhiEt[ich] << ")" );
    }
  }

  // initialize jet category
  // jets in this category are rejected
  if(m_JetQuality == "MediumBad" )      m_JetCategory = MediumBad;
  else if(m_JetQuality == "TightBad" )  m_JetCategory = TightBad;
  else                                  m_JetCategory = LooseBad;

  /*
  //load the tdt
  StatusCode sc1 = StatusCode::SUCCESS;
  if( !m_tdt.empty() ) {
    sc1 = m_tdt.retrieve();
    if( !sc1.isSuccess() ) {
      ATH_MSG_WARNING( "Unable to retrieve the TrigDecisionTool" );
      return sc1;
    }
    ATH_MSG_INFO( "Retrieved the TrigDecisionTool" );
  }
  else {
    ATH_MSG_ERROR( "Could not retrive the TrigDecisionTool as it was not specified!" );
    return StatusCode::FAILURE;
  }
*/
  // load the matching tool
  StatusCode sc = StatusCode::SUCCESS;
  if( !m_trigMatchTool.empty() ) {
    sc = m_trigMatchTool.retrieve();
    if( !sc.isSuccess() ) {
      ATH_MSG_WARNING( "Unable to retrieve the TrigMatchTool" );
      return sc;
    }
    ATH_MSG_INFO( "Retrieved the TrigMatchTool" );
  }
  else {
    ATH_MSG_ERROR( "Could not retrive the TrigMatchTool as it was not specified!" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

} // end init

// ------------------------------------------------------------------------------------

StatusCode HLTJetMonTool::book( ) {

  if (m_debuglevel) 
    ATH_MSG_INFO( "in HLTJetMonTool::book()" );

  const unsigned int l1v_num  = m_L1Items.size();
  const unsigned int hltv_num = m_HLTChains.size();


  // nbins Et defaults
  if(m_l1nbinsEt.size() != l1v_num) {
    m_l1nbinsEt.clear();
    for(unsigned int k = 0; k < l1v_num; k++) 
      m_l1nbinsEt.push_back(m_jEtnbins[0]);
  }

  if(m_hltnbinsEt.size() != hltv_num) {
    m_hltnbinsEt.clear();
    for(unsigned int k = 0; k < hltv_num; k++) 
      m_hltnbinsEt.push_back(m_jEtnbins[0]);
  }

  // binlo Et defaults
  if(m_l1binloEt.size() != l1v_num) {
    m_l1binloEt.clear();
    for(unsigned int k = 0; k < l1v_num; k++) 
      m_l1binloEt.push_back(m_jEtbinlo[0]);
  }

  if(m_hltbinloEt.size() != hltv_num) {
    m_hltbinloEt.clear();
    for(unsigned int k = 0; k < hltv_num; k++) 
      m_hltbinloEt.push_back(m_jEtbinlo[0]);
  }

  // binhi Et defaults
  if(m_l1binhiEt.size() != l1v_num) {
    m_l1binhiEt.clear();
    for(unsigned int k = 0; k < l1v_num; k++) 
      m_l1binhiEt.push_back(m_jEtbinhi[0]);
  }

  if(m_hltbinhiEt.size() != hltv_num) {
    m_hltbinhiEt.clear();
    for(unsigned int k = 0; k < hltv_num; k++) 
      m_hltbinhiEt.push_back(m_jEtbinhi[0]);
  }


  // std::vector<int> m_njnbins, m_jEtnbins, m_jetanbins, m_jphinbins, m_jDEtnbins, m_jDepnbins;
  for(unsigned int k = 0; k < m_njnbins.size(); k++) {
    float thebinSize = m_njnbins[k] > 0 ? fabs(m_njbinhi[k]-m_njbinlo[k]) / m_njnbins[k] : -1.;
    m_njperbin.push_back(thebinSize);
  }

  for(unsigned int k = 0; k < m_jEtnbins.size(); k++) {
    float thebinSize = m_jEtnbins[k] > 0 ? fabs(m_jEtbinhi[k]-m_jEtbinlo[k]) / m_jEtnbins[k] : -1.;
    m_jEtperbin.push_back(thebinSize);
  }
  for(unsigned int k = 0; k < m_jetanbins.size(); k++) {
    float thebinSize = m_jetanbins[k] > 0 ? fabs(m_jetabinhi[k]-m_jetabinlo[k]) / m_jetanbins[k] : -1.;
    m_jetaperbin.push_back(thebinSize);
  }
  for(unsigned int k = 0; k < m_jphinbins.size(); k++) {
    float thebinSize = m_jphinbins[k] > 0 ? fabs(m_jphibinhi[k]-m_jphibinlo[k]) / m_jphinbins[k] : -1.;
    m_jphiperbin.push_back(thebinSize);
  }
  for(unsigned int k = 0; k < m_jemfracnbins.size(); k++) {
    float thebinSize = m_jemfracnbins[k] > 0 ? fabs(m_jemfracbinhi[k]-m_jemfracbinlo[k]) / m_jemfracnbins[k] : -1.;
    m_jemfracperbin.push_back(thebinSize);
  }
  for(unsigned int k = 0; k < m_jhecfracnbins.size(); k++) {
    float thebinSize = m_jhecfracnbins[k] > 0 ? fabs(m_jhecfracbinhi[k]-m_jhecfracbinlo[k]) / m_jhecfracnbins[k] : -1.;
    m_jhecfracperbin.push_back(thebinSize);
  }
  for(unsigned int k = 0; k < m_jDEtnbins.size(); k++) {
    float thebinSize = m_jDEtnbins[k] > 0 ? fabs(m_jDEtbinhi[k]-m_jDEtbinlo[k]) / m_jDEtnbins[k] : -1.;
    m_jDEtperbin.push_back(thebinSize);
  }
  for(unsigned int k = 0; k < m_jDepnbins.size(); k++) {
    float thebinSize = m_jDepnbins[k] > 0 ? fabs(m_jDepbinhi[k]-m_jDepbinlo[k]) / m_jDepnbins[k] : -1.;
    m_jDepperbin.push_back(thebinSize);
  }

  // book jet histograms
  bookJetHists();

  // set properties
  TH1 *h(0);
  for (std::map<std::string, TH1 *>::iterator i = m_hist.begin(); i != m_hist.end(); ++i) {
    h = i->second;
    if(h) setHistProperties(h);
  } // for

  for (std::map<std::string, TH2 *>::iterator i = m_hist2.begin(); i != m_hist2.end(); ++i) {
    h = (TH1 *) i->second;
    if(h) setHistProperties(h);
  } // for

  return StatusCode::SUCCESS;

} // end book

// ------------------------------------------------------------------------------------


void HLTJetMonTool::bookJetHists() {

  // Define levels and choose to plot additional histograms
  // Levels = "L1", "HLT" + offline
  std::vector<std::string> levels;
  std::vector<std::string> bookvars;
  std::string varlist;

  ///////////////////////////////////////////////////////////////////////////////
  // bookBasicHists - 
  // L1 histograms in m_L1dir
  levels.push_back("L1");
  varlist = "n;et;eta;phi;phi_vs_eta;e_vs_eta;e_vs_phi;phi_vs_eta_lar";

  // create path to L1 and HLT dirs
  std::string L1dir = m_monBase + m_L1dir;
  std::string HLTdir = m_monBase + m_HLTdir;
  addMonGroup (new MonGroup(this, L1dir.c_str(), run));
  addMonGroup (new MonGroup(this, HLTdir.c_str(), run));

  // add mongroup to list
  m_monGroups["L1"]  = L1dir;
  m_monGroups["HLT"] = HLTdir;

  // create a new mongroup for L1 and make it current
  setCurrentMonGroup(L1dir);

  // book L1 histograms
  int nvar = basicKineVar(varlist,bookvars);
  if(nvar==0) ATH_MSG_INFO("Error in bookKineVars - variable list not tokenized!");
  bookBasicHists(levels,bookvars);
  
  ///////////////////////////////////////////////////////////////////////////////
  // bookBasicHists - 
  // HLT histograms in m_HLTdir
  setCurrentMonGroup(HLTdir);
  levels.clear(); levels.push_back("HLT");

  unsigned int k = 0; 
  for(JetSigIter hj= m_HLTJetKeys.begin(); hj != m_HLTJetKeys.end(); ++hj, k++) {
    // book histograms for each HLT jet container 
    varlist = "n;et;eta;phi;emfrac;hecfrac;phi_vs_eta;e_vs_eta;e_vs_phi;phi_vs_eta_lar";
    nvar = basicKineVar(varlist,bookvars);
    if(nvar==0) ATH_MSG_INFO("Error in bookKineVars - variable list not tokenized!");

    std::string HLTpfx = Form("%s%s",m_HLTpfx.c_str(),((*hj).first).c_str());
    levels.clear();
    levels.push_back(HLTpfx); 

    // create path to HLT dir
    std::string HLTdir = m_monBase + "/HLT/" + (*hj).first;

    // add mongroup to list
    std::string HLTpfx_short = Form("%s%d",m_HLTpfx.c_str(),k);
    m_monGroups[HLTpfx_short] = HLTdir;  

    // create a new mongroup for HLT and make it current
    addMonGroup (new MonGroup(this, HLTdir.c_str(), run));
    setCurrentMonGroup(HLTdir); 

    // book HLT histograms
    bookBasicHists(levels,bookvars);
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // fillBasicL1forChain - "roidesc_eta;roidesc_phi;roidesc_phi_vs_eta;unmatched_eta"; // all with L1 prefix
  //                     - "et;eta;phi;phi_vs_eta;"; // all with L1 prefix
  //

  // L1 Chains
  varlist="et;eta;phi;phi_vs_eta;";
  nvar = basicKineVar(varlist,bookvars);
  if(nvar==0) ATH_MSG_INFO("Error in bookKineVars - variable list not tokenized!");
  levels.clear(); levels.push_back("L1");
  for(JetSigIter k= m_basicL1Trig.begin(); k != m_basicL1Trig.end(); ++k ) {
    const std::string theDir = L1dir + "/" + (*k).first;
    ATH_MSG_INFO("Booking histograms for " << theDir);
    m_monGroups[(*k).first] = theDir;
    addMonGroup (new MonGroup(this, theDir,run));
    setCurrentMonGroup(theDir);
    bookBasicHists(levels,bookvars);
  }

  // L1 only
  varlist="roidesc_eta;roidesc_phi;roidesc_phi_vs_eta;eta_unmatched;";  // L1 prefix
  nvar = basicKineVar(varlist,bookvars);
  if(nvar==0) ATH_MSG_INFO("Error in bookKineVars - variable list not tokenized!");
  levels.clear(); levels.push_back("L1");
  for(JetSigIter k= m_basicL1Trig.begin(); k != m_basicL1Trig.end(); ++k ) {
    setCurrentMonGroup(m_monGroups[(*k).first]);
    bookBasicHists(levels,bookvars);
  }
    
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // fillBasicHLTforChain - "l1et_vs_hltet;l1eta_vs_hlteta;l1phi_vs_hltphi;"
  //                     - "et;eta;phi;phi_vs_eta;et_EffRelL2;eta_EffRelL2;phi_EffRelL2" // with HLT prefix

  // HLT Chains
  // HLT basic histograms
  varlist="et;eta;phi;phi_vs_eta;emfrac;hecfrac";
  nvar = basicKineVar(varlist,bookvars);
  levels.clear(); levels.push_back("HLT"); levels.push_back("L1");
  for(JetSigIter k= m_basicHLTTrig.begin(); k != m_basicHLTTrig.end(); ++k ) {
    const std::string theDir = HLTdir + "/" + (*k).first;
    m_monGroups[(*k).first] = theDir;
    ATH_MSG_INFO("Booking histograms for " << theDir);
    addMonGroup (new MonGroup(this, theDir, run));
    setCurrentMonGroup(theDir);
    bookBasicHists(levels,bookvars);
  }
  
  // L1 only
  varlist="roidesc_eta;roidesc_phi;roidesc_phi_vs_eta;eta_unmatched;";  // L1 prefix
  nvar = basicKineVar(varlist,bookvars);
  levels.clear(); levels.push_back("L1");
  for(JetSigIter k= m_basicHLTTrig.begin(); k != m_basicHLTTrig.end(); ++k ) {
    setCurrentMonGroup(m_monGroups[(*k).first]);
    bookBasicHists(levels,bookvars);
  } 
  // correlation/relative efficiency histograms for HLT chains
  varlist =  "l1et_vs_hltet;l1eta_vs_hlteta;l1phi_vs_hltphi;";
  nvar = basicKineVar(varlist,bookvars);
  levels.clear(); levels.push_back("HLT");
  for(JetSigIter k= m_basicHLTTrig.begin(); k != m_basicHLTTrig.end(); ++k ) {
    setCurrentMonGroup(m_monGroups[(*k).first]);
    bookBasicHists(levels,bookvars);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Offline Jets - 
  // OF m_OFpfx for each offline jet container
  // OF histograms will be in their respective folder by container names
  // Example: if m_OFpfx = "OF_" and m_OFJetKeys[] = {"AntiKt4LCTopoJets", "AntiKt10EMTopoJets"}
  // then dirs will be {"Rec_AntiKt4LCTopoJets", "Rec_AntiKt10EMTopoJets"}

  if(m_doOFJets) {
    k = 0; // FIXME
    for(JetSigIter ofj= m_OFJetKeys.begin(); ofj != m_OFJetKeys.end(); ++ofj, k++) {
      // book histograms for each offline jet container 
      varlist = "n;et;eta;phi;emfrac;hecfrac;phi_vs_eta;e_vs_eta;e_vs_phi;phi_vs_eta_lar";
      nvar = basicKineVar(varlist,bookvars);
      if(nvar==0) ATH_MSG_INFO("Error in bookKineVars - variable list not tokenized!");

      //std::string OFpfx = Form("%s%s",m_OFpfx.c_str(),m_OFJetKeys[k].c_str());
      std::string OFpfx = Form("%s%s",m_OFpfx.c_str(),((*ofj).first).c_str());
      levels.clear();
      levels.push_back(OFpfx); 

      // create path to OF dir
      std::string OFdir = m_monBase + "/OF/" + (*ofj).first;

      // add mongroup to list
      std::string OFpfx_short = Form("%s%d",m_OFpfx.c_str(),k);
      m_monGroups[OFpfx_short] = OFdir;  

      // create a new mongroup for OF and make it current
      addMonGroup (new MonGroup(this, OFdir.c_str(), run));
      setCurrentMonGroup(OFdir); 

      // book OF histograms
      bookBasicHists(levels,bookvars);

      // Efficiency plots -
      varlist = "et;eta;phi;phi_vs_eta";
      nvar = basicKineVar(varlist,bookvars);
      if(nvar==0) ATH_MSG_INFO("Error in bookKineVars - variable list not tokenized!");

      setCurrentMonGroup(OFdir); 
      // create path to Efficiency dir
      std::string Effdir = OFdir + m_Effdir;
      std::string Effdir_short = Form("%sEff",OFpfx_short.c_str());
      m_monGroups[Effdir_short] = Effdir;

      // create a new mongroup for OF efficiency and make it current
      addMonGroup (new MonGroup(this, Effdir.c_str(), run));
      setCurrentMonGroup(Effdir);

      // L1
      if(m_doL1TrigEff) {
	bookOfflineHists(m_L1Items, (*ofj).second );
      }

      // HLT
      if(m_doHLTTrigEff) {
	bookOfflineHists(m_HLTChains, (*ofj).second );
      }

    } // end for JetSigIter
  } // end if(m)doOFJets)

  if(m_debuglevel) {
    std::ostringstream mongrps;
    std::map<std::string, std::string>::iterator it = m_monGroups.begin();
    for(; it != m_monGroups.end(); it++ )
      mongrps << Form("\n\t%10s <==> %10s",((*it).first).c_str(), ((*it).second).c_str());
    ATH_MSG_DEBUG( mongrps.str() );
  }
  // -------------------------------------------------------------------- //

} // end bookJetHists

// ------------------------------------------------------------------------------------

void HLTJetMonTool::bookOfflineHists(JetSigtype& item, const std::string& ofjet) {

  if(item.size() == 0 ) return;
  JetSigIter it0 = item.begin();
  std::string itPfx = (*it0).first, itlvl = "NO";
  if(itPfx.size() > 2) {
    itlvl = itPfx.substr(0,2);
  }

  // item
  unsigned int k = 0;
  for(JetSigIter it = item.begin(); it != item.end(); ++it, k++) {

    int nbinsEt = (  (itlvl == "L1")  ? m_l1nbinsEt[k] : 
                  (  (itlvl == "HLT") ? m_hltnbinsEt[k] : m_jEtnbins[0] ));

    float binloEt = ( (itlvl == "L1")  ? m_l1binloEt[k] : 
                    ( (itlvl == "HLT") ? m_hltbinloEt[k] : m_jEtbinlo[0] ));

    float binhiEt = (  (itlvl == "L1")  ? m_l1binhiEt[k] : 
                    (  (itlvl == "HLT") ? m_hltbinhiEt[k] : m_jEtbinhi[0] ));

    TString trigItem = (*it).second;
    TString trigName = (*it).first;

    // eff vs. pt
    TString htitle = Form("%s Efficiency w.r.t %s vs. p_{T}; p_{T}^{jet} [GeV]; Efficiency",trigItem.Data(), ofjet.c_str());
    TString hname = Form("%s_Eff_vs_pt_num",trigName.Data());

    addHistogram(new TH1F(hname, htitle,nbinsEt,binloEt,binhiEt));
    ATH_MSG_DEBUG("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );

    hname = Form("%s_Eff_vs_pt_den",trigName.Data());
    addHistogram(new TH1F(hname, htitle,nbinsEt,binloEt,binhiEt));

    // eff vs. eta
    htitle = Form("%s Efficiency w.r.t %s vs. #eta; #eta^{jet}; Efficiency",trigItem.Data(), ofjet.c_str());
    hname = Form("%s_Eff_vs_eta_num",trigName.Data());

    addHistogram(new TH1F(hname, htitle,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));

    hname = Form("%s_Eff_vs_eta_den",trigName.Data());
    addHistogram(new TH1F(hname, htitle,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));

    // eff vs. phi
    htitle = Form("%s Efficiency w.r.t %s vs. #varphi; #varphi^{jet} [rad]; Efficiency",trigItem.Data(), ofjet.c_str());
    hname = Form("%s_Eff_vs_phi_num",trigName.Data());

    addHistogram(new TH1F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));

    hname = Form("%s_Eff_vs_phi_den",trigName.Data());
    addHistogram(new TH1F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));

  } // end for

} // end bookTrigEffHist

/*
// ------------------------------------------------------------------------------------

void HLTJetMonTool::bookCorrHists(const std::string& level2, const std::string& level1,  const std::string& ofjAlg ) {

  //if(k >= m_OFJetKeys.size()) return;

  std::string OFalg = ofjAlg; //m_OFJetKeys[k];
  //std::string OFalg_short = Form("%s%d",m_OFpfx.c_str(),k);

  TString htitle = "histogram title"; 
  TString hname = "hist2d_name"; 

  bool m_notOF = (level1 == "L1") && (level2 == "HLT");
  std::string lvlUp = "Offline", lvlUpt = "Offline hist", lvlDown = level1;

  if(m_notOF) {
    lvlUp = level2; 
    lvlUpt = level2;
  } else {
    //lvlUp = OFalg_short;
    lvlUp = m_OFpfx;
    lvlUpt = OFalg;
  }

  // DeltaEta vs Eta ---------------------
  htitle = Form("#Delta#eta(%s, %s) vs. %s #eta ; %s #eta; #Delta#eta",lvlUpt.c_str(), lvlDown.c_str(), lvlUpt.c_str(), lvlUpt.c_str()) ;
  hname = Form("%s%sdeta_vs_%seta",lvlUp.c_str(), lvlDown.c_str(), lvlUp.c_str());
  addHistogram(new TH2F(hname, htitle,m_jDepnbins[0],m_jDepbinlo[0],m_jDepbinhi[0],m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));  

  // DeltaPhi vs Phi ---------------------
  htitle = Form("#Delta#varphi(%s, %s) vs. %s #varphi ; %s #varphi [rad]; #Delta#varphi [rad]",lvlUpt.c_str(), lvlDown.c_str(), lvlUpt.c_str(), lvlUpt.c_str()) ;
  hname = Form("%s%sdphi_vs_%sphi",lvlUp.c_str(), lvlDown.c_str(), lvlUp.c_str());
  addHistogram(new TH2F(hname, htitle,m_jDepnbins[0],m_jDepbinlo[0],m_jDepbinhi[0],m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));  

  // DeltaEt vs Et  ---------------------
  htitle = Form("#DeltaE_{T}(%s, %s) vs. %s E_{T} ; %s E_{T} [GeV]; #DeltaE_{T} [GeV]",lvlUpt.c_str(), lvlDown.c_str(), lvlUpt.c_str(), lvlUpt.c_str());

  hname = Form("%s%sdEt_vs_%sEt",lvlUp.c_str(), lvlDown.c_str(), lvlUp.c_str());
  addHistogram(new TH2F(hname, htitle,m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0],m_jDEtnbins[0],m_jDEtbinlo[0],m_jDEtbinhi[0]));  

  // Et vs Et ---------------------
  htitle = Form("Correlation: %s E_{T} vs. %s E_{T}; %s E_{T} [GeV]; %s E_{T} [GeV]",lvlUpt.c_str(), lvlDown.c_str(),lvlDown.c_str(), lvlUpt.c_str());
  hname = Form("%s_vs_%s_Et",lvlUp.c_str(), lvlDown.c_str());
  addHistogram(new TH2F(hname, htitle,m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0],m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0]));

  // Phi vs Phi ---------------------
  htitle = Form("Correlation: %s #varphi vs. %s #varphi; %s #varphi [rad]; %s #varphi [rad]",lvlUpt.c_str(), lvlDown.c_str(),lvlDown.c_str(), lvlUpt.c_str());
  hname = Form("%s_vs_%s_phi",lvlUp.c_str(), lvlDown.c_str());
  addHistogram(new TH2F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0],m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));

  // Eta vs Eta ---------------------
  htitle = Form("Correlation: %s #eta vs. %s #eta; %s #eta; %s #eta",lvlUpt.c_str(), lvlDown.c_str(),lvlDown.c_str(), lvlUpt.c_str());
  hname = Form("%s_vs_%s_eta",lvlUp.c_str(), lvlDown.c_str());
  addHistogram(new TH2F(hname, htitle,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0],m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));


} // end bookCorrHist
*/

// ------------------------------------------------------------------------------------

void HLTJetMonTool::bookBasicHists(std::vector<std::string>& level, std::vector<std::string>& bookvars) {
  ATH_MSG_DEBUG("bookBasicHists for level: " << level );
  for( unsigned int i = 0; i < level.size(); i++ ) {
    int nbins_eta = m_jetanbins[0];
    double binlo_eta = m_jetabinlo[0];
    double binhi_eta = m_jetabinhi[0];
    TString lvl = level[i].c_str();
    TString title = level[i].c_str();
    if(lvl.Contains(m_HLTpfx)) {
      lvl = m_HLTpfx;
      title = title.ReplaceAll(m_HLTpfx,"");
    }
    if(lvl.Contains(m_OFpfx)) {
      lvl = m_OFpfx;
      title = title.ReplaceAll(m_OFpfx,"");
    }
    for(VectIter var = bookvars.begin(); var != bookvars.end(); ++var) {

      TString htitle, hname; 

      // jet multiplicity
      if(*var == "n") {
        htitle = Form("%s jet multiplicity; num of jets; entries/%1.f jet",title.Data(),m_njperbin[0]);
        hname = Form("%sJet_n",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_njnbins[0],m_njbinlo[0],m_njbinhi[0]));
      }

      // jet ET
      if(*var == "et") {
        htitle = Form("%s transverse energy; E_{T} [GeV]; entries/%3.1f GeV",title.Data(),m_jEtperbin[0]);
        hname = Form("%sJet_Et",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0]));
      }

      // jet et Efficiency v ET wrt L1
      if(*var == "et_EffRelL1") {
        htitle = Form("%s Efficiency vs. E_{T} relative to L1; E_{T} [GeV]; entries/%3.1f GeV",title.Data(),m_jEtperbin[0]);
        hname = Form("%sJet_Et_EffRelL1",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0]));
      }

      // jet eta
      if(*var == "eta") {
        htitle = Form("%s pseudorapidity; #eta; entries/%3.1f",title.Data(),m_jetaperbin[0]);
        hname = Form("%sJet_eta",lvl.Data());
        addHistogram(new TH1F(hname, htitle,nbins_eta,binlo_eta,binhi_eta));
      }
     
      // jet eta Efficiency v ET wrt L1
      if(*var == "eta_EffRelL1") {
        htitle = Form("%s Efficiency vs. #eta relative to L1; #eta ; entries/%3.1f GeV",title.Data(),m_jetaperbin[0]);
        hname = Form("%sJet_eta_EffRelL1",lvl.Data());
        addHistogram(new TH1F(hname, htitle,nbins_eta,binlo_eta,binhi_eta));
      }

      // jet phi
      if(*var == "phi") {
        htitle = Form("%s azimuth; #varphi [rad]; entries/%3.1f rad",title.Data(),m_jphiperbin[0]);
        hname = Form("%sJet_phi",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));
      }
      
      // jet phi Efficiency v ET wrt L1
      if(*var == "phi_EffRelL1") {
        htitle = Form("%s Efficiency vs. #varphi relative to L1; #varphi [rad]; entries/%3.1f rad",title.Data(),m_jphiperbin[0]);
        hname = Form("%sJet_phi_EffRelL1",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));
      }

      // jet phi (y) vs. eta (x)
      if(*var == "phi_vs_eta") {
        htitle = Form("%s #varphi vs. #eta; #eta; #varphi [rad];",title.Data());
        hname = Form("%sJet_phi_vs_eta",lvl.Data());
        addHistogram(new TH2F(hname, htitle, nbins_eta,binlo_eta,binhi_eta, m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));
      }

      // jet emfrac
      if(*var == "emfrac") {
        htitle = Form("%s emfrac; a.u.; entries/%3.1f",title.Data(),m_jemfracperbin[0]);
        hname = Form("%sJet_emfrac",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jemfracnbins[0],m_jemfracbinlo[0],m_jemfracbinhi[0]));
      }

      // jet hecfrac
      if(*var == "hecfrac") {
        htitle = Form("%s hecfrac; a.u.; entries/%3.1f",title.Data(),m_jhecfracperbin[0]);
        hname = Form("%sJet_hecfrac",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jhecfracnbins[0],m_jhecfracbinlo[0],m_jhecfracbinhi[0]));
      }
      
      // roi descriptor eta
      if(*var == "roidesc_eta") {
        htitle = Form("%s pseudorapidity of center of RoI at origin; #eta; entries/%3.1f",title.Data(),m_jetaperbin[0]);
        hname = Form("%sJet_roidesc_eta",lvl.Data());
        addHistogram(new TH1F(hname, htitle,nbins_eta,binlo_eta,binhi_eta));
      }
      
      // unmatched roi eta
      if(*var == "eta_unmatched") {
        htitle = Form("%s pseudorapidity of unmatched L1 RoI; #eta; entries/%3.1f",title.Data(),m_jetaperbin[0]);
        hname = Form("%sJet_unmatched_eta",lvl.Data());
        addHistogram(new TH1F(hname, htitle,nbins_eta,binlo_eta,binhi_eta));
      }

      // roi descriptor  phi
      if(*var == "roidesc_phi") {
        htitle = Form("%s azimuth of center of RoI at origin; #varphi [rad]; entries/%3.1f rad",title.Data(),m_jphiperbin[0]);
        hname = Form("%sJet_roidesc_phi",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));
      }

      // roi descriptor phi (y) vs. eta (x)
      if(*var == "roidesc_phi_vs_eta") {
        htitle = Form("%s #varphi vs. #eta of center of RoI at origin; #eta; #varphi [rad];",title.Data());
        hname = Form("%sJet_roidesc_phi_vs_eta",lvl.Data());
        addHistogram(new TH2F(hname, htitle, nbins_eta,binlo_eta,binhi_eta, m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));
      }

      // jet eta (y) vs. energy (x)
      if(*var == "e_vs_eta") {
        htitle = Form("%s energy vs. #eta; #eta; energy [GeV];",title.Data());
        hname = Form("%sJet_E_vs_eta",lvl.Data());
        addHistogram(new TH2F(hname, htitle, nbins_eta,binlo_eta,binhi_eta, m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0]));
      }

      // jet phi (y) vs. energy (x)
      if(*var == "e_vs_phi") {
        htitle = Form("%s energy vs. #varphi; #varphi [rad]; energy [GeV];",title.Data());
        hname = Form("%sJet_E_vs_phi",lvl.Data());
        addHistogram(new TH2F(hname, htitle, m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0], m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0]));
      }

      // jet phi (y) vs. eta (x)
      if(*var == "phi_vs_eta_lar") {
        htitle = Form("%s #varphi vs. #eta EM Fraction > %3.1f %%; #eta; #varphi [rad];",title.Data(), m_emfracCut*100);
        hname = Form("%sJet_phi_vs_eta_LAr",lvl.Data());
        addHistogram(new TH2F(hname, htitle, nbins_eta,binlo_eta,binhi_eta, m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));
      }
      
      // jet l1et (y) vs. hltet (x)
      if(*var == "l1et_vs_hltet") {
        htitle = "L1 vs. HLT transverse energy; HLT E_{T} [GeV]; L1 E_{T} [GeV];";
        hname = "Jet_l1et_vs_hltet";
        addHistogram(new TH2F(hname, htitle, m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0], m_jEtnbins[0],m_jEtbinlo[0],m_jEtbinhi[0]));
      }

      // jet l1eta (y) vs. hlteta (x)
      if(*var == "l1eta_vs_hlteta") {
        htitle = "L1 vs. HLT pseudorapidity; HLT #eta; L1 #eta;";
        hname = "Jet_l1eta_vs_hlteta";
        addHistogram(new TH2F(hname, htitle, nbins_eta,binlo_eta,binhi_eta, nbins_eta,binlo_eta,binhi_eta));
      }

      // jet l1phi (y) vs. hltphi (x)
      if(*var == "l1phi_vs_hltphi") {
        htitle = "L1 vs. HLT azimuth; HLT #varphi [rad]; L1 #varphi [rad];";
        hname = "Jet_l1phi_vs_hltphi";
        addHistogram(new TH2F(hname, htitle, m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0], m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));
      }
      
    } // for chain

  } // end for

} // end bookJetHist

// ------------------------------------------------------------------------------------

void HLTJetMonTool::setHistProperties(TH1* h) {

  if (m_debuglevel) 
    ATH_MSG_DEBUG( "in HLTJetMonTool::setHistProperties() " );
  if(!h) return;
  h->SetFillColor(42);
  h->SetTitleSize(0.037,"X");
  h->SetTitleSize(0.037,"Y");
  h->SetLabelSize(0.033,"X");
  h->SetLabelSize(0.033,"Y");

} // end setHistProperties

// ------------------------------------------------------------------------------------

StatusCode HLTJetMonTool::retrieveContainers() {

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve xAOD L1 ROI 
  m_L1JetRoIC = 0;
  sc = m_storeGate->retrieve(m_L1JetRoIC, m_L1xAODJetKey);
  if(sc.isFailure() || !m_L1JetRoIC) {
    ATH_MSG_DEBUG ("Could not retrieve LVL1JetRoIs with key \"" << m_L1xAODJetKey << "\" from TDS"  );
  }
  else {
    ATH_MSG_INFO(" Retrieved LVL1JetROIs with key \"" << m_L1xAODJetKey << "\" from TDS" );
  }
  
  // retrieve HLT jets
  // clear before retrieving containers
  m_HLTJetC.clear();
  for(JetSigIter hltj=m_HLTJetKeys.begin(); hltj != m_HLTJetKeys.end(); ++hltj ) {
    const xAOD::JetContainer *hltcoll = 0;
    sc = m_storeGate->retrieve(hltcoll, (*hltj).second);
    if(sc.isFailure() || !hltcoll) {
      ATH_MSG_DEBUG ("Could not retrieve JetCollection with key \"" << (*hltj).second << "\" from TDS"  );
    }
    else {
      ATH_MSG_INFO ("FOUND JetCollection with key \"" << (*hltj).second << "\" from TDS"  );
    }
    m_HLTJetC.push_back(hltcoll);
  } // end for

  if(!m_doOFJets) return sc;
  // retrieve OF Jets 
  // clear before retrieving containers
  m_OFJetC.clear();
  for(JetSigIter ofj= m_OFJetKeys.begin(); ofj != m_OFJetKeys.end(); ++ofj ) {
    const xAOD::JetContainer *jetcoll = 0;
    sc = m_storeGate->retrieve(jetcoll, (*ofj).second /*m_OFJetKeys[k]*/);
    if(sc.isFailure() || !jetcoll) {
      ATH_MSG_DEBUG ("Could not retrieve JetCollection with key \"" << (*ofj).second << "\" from TDS"  );
    }
    else {
      ATH_MSG_INFO ("FOUND JetCollection with key \"" << (*ofj).second << "\" from TDS"  );
    }
    m_OFJetC.push_back(jetcoll);
  } // end for

  return sc;

} // end retrieveContainers

// ------------------------------------------------------------------------------------

StatusCode HLTJetMonTool::fill() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG( "in HLTJetMonTool::fill()" );

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve containers
  sc = retrieveContainers();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "HLTJetMonTool::retrieveContainers() failed" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO ( "HLTJetMonTool::retrieveContainers() returned success" );

  // fill histograms
  sc = fillJetHists();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "HLTJetMonTool::fillJetHists() failed" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO ( "HLTJetMonTool::fillJetHists() returned success" );

  return StatusCode::SUCCESS;

} // end fill()

// ------------------------------------------------------------------------------------

StatusCode HLTJetMonTool::fillJetHists() {

  StatusCode sc = StatusCode::SUCCESS;
  sc = fillBasicHists();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "HLTJetMonTool::fillBasicHists() failed" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO ( "HLTJetMonTool::fillBasicHists() returned success" );

  // fill offline jet and trigger eff hists
  if(m_doOFJets) {
    sc = fillOfflineHists();
    if (sc.isFailure()) {
      ATH_MSG_WARNING ( "HLTJetMonTool::fillOfflineHists() failed" );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_INFO ( "HLTJetMonTool::fillOfflineHists() returned success" );

  }

  return StatusCode::SUCCESS;

} // end fillJetHists

// ------------------------------------------------------------------------------------

StatusCode HLTJetMonTool::fillBasicHists() {

  TH1 *h(0);
  TH2 *h2(0);

  // Get Jet RoI
  // L1 begin filling basic histograms
  ATH_MSG_INFO ("Filling L1 Jets");
  if(m_L1JetRoIC) {
    setCurrentMonGroup(m_monGroups["L1"]);
    if(m_debuglevel)
      ATH_MSG_DEBUG( "Mon group set to " << m_monGroups["L1"] );
    unsigned int L1Roi_num = 0;

    xAOD::JetRoIContainer::const_iterator it_L1 = m_L1JetRoIC->begin();
    xAOD::JetRoIContainer::const_iterator it_e_L1 = m_L1JetRoIC->end();
    for ( ; it_L1 != it_e_L1; it_L1++) {
      L1Roi_num++;
      double et = ( (*it_L1)->et4x4())/CLHEP::GeV;
      if(et < 1.e-3) et = 0.;
      double eta = (*it_L1)->eta();
      double ene = et * cosh(eta);
      double phi = (*it_L1)->phi();
      if(m_debuglevel)
        ATH_MSG_DEBUG( "et =  " << et <<  "\teta = " << eta << "\tene = " << ene );

      if((h = hist("L1Jet_Et"))) { 
        if(m_debuglevel)
          ATH_MSG_DEBUG( "found L1Jet_Et" ); 
        h->Fill(et); 
      }
      if((h = hist("L1Jet_eta")))  h->Fill(eta);
      if((h = hist("L1Jet_phi")))  h->Fill(phi);
      if((h2 = hist2("L1Jet_E_vs_eta")))  h2->Fill(eta,ene);
      if((h2 = hist2("L1Jet_E_vs_phi")))  h2->Fill(phi,ene);
      if((h2 = hist2("L1Jet_phi_vs_eta")))  h2->Fill(eta,phi);
    } // end for it_L1

    if((h = hist("L1Jet_n"))) {
      if (m_debuglevel)
         ATH_MSG_DEBUG( " Number of L1JetROI's " << L1Roi_num );  
      h->Fill(L1Roi_num);
    }

    // begin fill L1  per chain -- will be filled in L1/EF by using ancestor methods 
      for(JetSigIter l1 = m_basicL1Trig.begin(); l1 != m_basicL1Trig.end(); ++l1) {
      setCurrentMonGroup(m_monGroups[(*l1).first]);
      ATH_MSG_INFO("Calling fillBasicL1forChain(" << (*l1).second << ", " << m_l1EtThres[(*l1).second] << ")" );
      fillBasicL1forChain((*l1).second, m_l1EtThres[(*l1).second]);     
      } // end fill L1 per chain
   
  } // end if m_L1RoiC

  // fill HLT jets
  ATH_MSG_INFO ("Filling HLT Jets");
  unsigned int Nelem = m_HLTJetC.size();
  ATH_MSG_DEBUG ("m_HLTJetC size = " << Nelem );
  for(unsigned int k = 0; k < Nelem; k++ ) {
    const xAOD::JetContainer *jetcoll = 0;
    jetcoll = m_HLTJetC[k];

    if(jetcoll) {
      std::string lvl = m_HLTpfx; 
      std::string mgrp = m_monGroups[Form("%s%d",m_HLTpfx.c_str(),k)];
	  
      if(m_debuglevel)ATH_MSG_DEBUG( "level set to " << lvl <<  " and mongroup set to " << mgrp );
      setCurrentMonGroup(mgrp);
    
      unsigned int n_HLTJet = 0;
      for(const auto & thisjet : *jetcoll) {
        n_HLTJet++;
        if(m_debuglevel) {
	      //checks jet variables
	      ATH_MSG_INFO( "REGTEST Looking at jet " << n_HLTJet);
	      ATH_MSG_INFO( "REGTEST    pt: " << thisjet->pt() );
	      ATH_MSG_INFO( "REGTEST    eta: " << thisjet->eta() );
	      ATH_MSG_INFO( "REGTEST    phi: " << thisjet->phi() );
	      ATH_MSG_INFO( "REGTEST    m: " << thisjet->m() );
	      ATH_MSG_INFO( "REGTEST    e: " << thisjet->e() );
	      //  ATH_MSG_INFO( "REGTEST    emfrac: " << thisjet->getAttribute<float>(xAOD::JetAttribute::EMFrac) ); 
	      //  ATH_MSG_INFO( "REGTEST    hecfrac: " << thisjet->getAttribute<float>(xAOD::JetAttribute::HECFrac) ); 
	      ATH_MSG_INFO( "REGTEST    px: " << thisjet->px() );
	      ATH_MSG_INFO( "REGTEST    py: " << thisjet->py() );
	      ATH_MSG_INFO( "REGTEST    pz: " << thisjet->pz() );
	      ATH_MSG_INFO( "REGTEST    type: " << thisjet->type() );
	      ATH_MSG_INFO( "REGTEST    algorithm (kt: 0, cam: 1, antikt: 2, ...): " << thisjet->getAlgorithmType() );
	      ATH_MSG_INFO( "REGTEST    size parameter: " << thisjet->getSizeParameter() );
	      ATH_MSG_INFO( "REGTEST    input (LCTopo: 0, EMTopo: 1, ...): " << thisjet->getInputType() );
	      ATH_MSG_INFO( "REGTEST    constituents signal state (uncalibrated: 0, calibrated: 1): " << thisjet->getConstituentsSignalState() );
	      ATH_MSG_INFO( "REGTEST    number of constituents: " << thisjet->numConstituents() );      
	    }

        // for basic hists, don't cut eta/pt
        //come back to this - LS//if(m_doselOFBasicHists) if(!selectJet(jet)) continue;

        double e = (thisjet->e())/CLHEP::GeV;
        double et = 0., epsilon = 1.e-3;
        if(thisjet->pt() > epsilon) et = (thisjet->pt())/CLHEP::GeV;
        if(et < epsilon) et = 0;
        double eta = thisjet->eta();
        double phi = thisjet->phi();
 	double  emfrac  = thisjet->getAttribute<float>(xAOD::JetAttribute::EMFrac); 
	double  hecfrac = thisjet->getAttribute<float>(xAOD::JetAttribute::HECFrac); 
        if(m_debuglevel) ATH_MSG_DEBUG( lvl << " et =  " << et <<  "\teta = " << eta << "\temfrac = " << emfrac <<"\thecfrac");

        if((h = hist( Form("%sJet_Et",lvl.c_str()))))  h->Fill(et);
        if((h = hist(Form("%sJet_eta",lvl.c_str()))))  h->Fill(eta);
        if((h = hist(Form("%sJet_phi",lvl.c_str()))))  h->Fill(phi);
        if((h = hist(Form("%sJet_emfrac",lvl.c_str()))))  h->Fill(emfrac);
	if((h = hist(Form("%sJet_hecfrac",lvl.c_str()))))  h->Fill(hecfrac);
        if((h2 = hist2(Form("%sJet_phi_vs_eta",lvl.c_str()))))  h2->Fill(eta,phi);
        if((h2 = hist2(Form("%sJet_E_vs_eta",lvl.c_str()))))  h2->Fill(eta,e);
        if((h2 = hist2(Form("%sJet_E_vs_phi",lvl.c_str()))))  h2->Fill(phi,e);

        // note: if this histogram turns out to be empty, it means:
        // emfraction is always 0 because the energies for different 
        // samplings are not filled at HLT
        if (emfrac > m_emfracCut) {
          if((h2 = hist2(Form("%sJet_phi_vs_eta_LAr",lvl.c_str()))))  h2->Fill(eta,phi);
        }
        // restore signal state
        // done automatically by sigstateH ??
      } // end for thisjet
      if((h = hist(Form("%sJet_n",lvl.c_str()))))  h->Fill(n_HLTJet);
    } // end if jetcoll
  } // end for k [HLT jet containers]

  // fill per chain -- begin
  for(JetSigIter hltit = m_HLTChains.begin(); hltit != m_HLTChains.end(); ++hltit) {
    setCurrentMonGroup(m_monGroups[(*hltit).first]);
    ATH_MSG_INFO("Calling fillBasicHLTforChain(" << (*hltit).second << ", " << m_hltEtThres[(*hltit).second] << ")" );
    fillBasicHLTforChain( (*hltit).second, m_hltEtThres[(*hltit).second], m_hltContainers[(*hltit).second]);
  } // fill per chain -- end
 
  
  // fill offline jets in one loop
  // offline jets are in (0, 1, ..., N-1)th elements 
  if(m_doOFJets) {
    ATH_MSG_DEBUG ("Filling OF Jets");
    unsigned int Nelem = m_OFJetC.size();
    ATH_MSG_DEBUG ("m_OFJetC size = " << Nelem );
    for(unsigned int k = 0; k < Nelem; k++ ) {
      const xAOD::JetContainer *jetcoll = 0;
      jetcoll = m_OFJetC[k];
      if(jetcoll) {
	  std::string lvl = m_OFpfx; 
	  std::string mgrp = m_monGroups[Form("%s%d",m_OFpfx.c_str(),k)];
	  if(m_debuglevel) ATH_MSG_DEBUG( "level set to " << lvl <<  " and mongroup set to " << mgrp );
	  setCurrentMonGroup(mgrp);
          unsigned int n_OFJet = 0;
	  for(const auto & thisjet : *jetcoll) {
            n_OFJet++;
            if(!thisjet) continue;

            // for basic hists, don't cut eta/pt
            //come back to this - LS//if(m_doselOFBasicHists) if(!selectJet(jet)) continue;

            double e = (thisjet->e())/CLHEP::GeV;
            double et = 0., epsilon = 1.e-3;
            if(thisjet->pt() > epsilon) et = (thisjet->pt())/CLHEP::GeV;
            if(et < epsilon) et = 0;
            ATH_MSG_VERBOSE( lvl << " thisjet->pt() =  " << et );
            double eta = thisjet->eta();
            double phi = thisjet->phi();
	    //float emfrac = 0.0;
 	    double  emfrac =  thisjet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
	    double  hecfrac = thisjet->getAttribute<float>(xAOD::JetAttribute::HECFrac);
            if(m_debuglevel) ATH_MSG_DEBUG( lvl << " et =  " << et <<  "\teta = " << eta << "\temfrac = " << emfrac <<"\thecfrac");

            if((h = hist( Form("%sJet_Et",lvl.c_str()))))  h->Fill(et);
            if((h = hist(Form("%sJet_eta",lvl.c_str()))))  h->Fill(eta);
            if((h = hist(Form("%sJet_phi",lvl.c_str()))))  h->Fill(phi);
            if((h = hist(Form("%sJet_emfrac",lvl.c_str()))))  h->Fill(emfrac);
	    if((h = hist(Form("%sJet_hecfrac",lvl.c_str()))))  h->Fill(hecfrac);
            if((h2 = hist2(Form("%sJet_phi_vs_eta",lvl.c_str()))))  h2->Fill(eta,phi);
            if((h2 = hist2(Form("%sJet_E_vs_eta",lvl.c_str()))))  h2->Fill(eta,e);
            if((h2 = hist2(Form("%sJet_E_vs_phi",lvl.c_str()))))  h2->Fill(phi,e);

            // note: if this histogram turns out to be empty, it means:
            // emfraction is alwasy 0 because the energies for different 
            // samplings are not filled at HLT
            if (emfrac > m_emfracCut) {
              if((h2 = hist2(Form("%sJet_phi_vs_eta_LAr",lvl.c_str()))))  h2->Fill(eta,phi);
            }
            // restore signal state
            // done automatically by sigstateH
	  } // end for thisjet
	  if((h = hist(Form("%sJet_n",lvl.c_str()))))  h->Fill(n_OFJet);
	  setCurrentMonGroup(mgrp);
      } // if jetcoll
    } // end for k
  } // if(m_doOFJets)

  return StatusCode::SUCCESS;

} // end fillBasicHists

// ------------------------------------------------------------------------------------
/*
bool HLTJetMonTool::evtSelTriggersPassed() {

  if(!m_doEvtSel) return true;
  std::vector<std::string>::const_iterator it = m_sampSelTriggers.begin(), itE = m_sampSelTriggers.end();
  for ( ; it != itE; it++ ) {
    //getTDT()->isPassed(name, TrigDefs::eventAccepted)
    if (getTDT()->isPassed(*it, TrigDefs::eventAccepted)) {
      return true;
    }
  }
  return false;

} // end evtSelTriggersPassed 
*/

// ------------------------------------------------------------------------------------
void HLTJetMonTool::fillBasicHLTforChain( const std::string& theChain, double thrHLT, const std::string& theContainer ) {
  

  TH1 *h(0); 
  TH2 *h2(0); 
  
  
  ATH_MSG_INFO("fillBasicHLTforChain CHAIN: " << Form("HLT_%s",theChain.c_str()) << " passed TDT: " << getTDT()->isPassed(Form("HLT_%s",theChain.c_str())));
  // ATH_MSG_INFO( "HLT Chains: " << getTDT()->getListOfTriggers( "HLT_.*" ));
  
  if (getTDT()->isPassed(Form("HLT_%s",theChain.c_str()))) {
    
    ATH_MSG_DEBUG("TDT Passed....");

    Trig::FeatureContainer f = getTDT()->features(Form("HLT_%s",theChain.c_str()));  //, TrigDefs::alsoDeactivateTEs);
    // Trig::FeatureContainer f = getTDT()->features("HLT_.*");
    Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin());
    Trig::FeatureContainer::combination_const_iterator combEnd(f.getCombinations().end());
    if ( comb==combEnd ) {
      ATH_MSG_INFO("no features for chain");
    }
    for( ; comb!=combEnd ; ++comb) {
      const std::vector<Trig::Feature<xAOD::JetContainer> > theHLTTrigF = (getTDT()->features(Form("HLT_%s",theChain.c_str()))).getCombinations().begin()->get<xAOD::JetContainer>(theContainer.c_str());       ATH_MSG_INFO("features size: "<<theHLTTrigF.size());
    }
    
     auto cg = getTDT()->getChainGroup(Form("HLT_%s",theChain.c_str()));
     auto fc = cg->features();

     ATH_MSG_INFO("Features Container : "<< &fc);

     auto JetFeatureContainers = fc.get<xAOD::JetContainer>(theContainer.c_str());
     for(auto jcont : JetFeatureContainers) {

       ATH_MSG_INFO("Test Loop....");

       for (auto j : *jcont.cptr()) {
	 //double e = (j->e())/CLHEP::GeV;
	 double et = 0., epsilon = 1.e-3;
	 if(j->pt() > epsilon) et = (j->pt())/CLHEP::GeV;
	 if(et < epsilon) et = 0;
	 bool hlt_thr_pass = ( et > thrHLT );
	 if(hlt_thr_pass) {
	   double eta = j->eta();
	   double phi = j->phi();
	   
	   if((h = hist("HLTJet_Et")))            h->Fill(et);
	   if((h = hist("HLTJet_eta")))           h->Fill(eta);
	   if((h = hist("HLTJet_phi")))           h->Fill(phi);
	   if((h2 = hist2("HLTJet_phi_vs_eta")))  h2->Fill(eta,phi);  

	 }// if hlt threshold
       }// loop over jet container
     }// loop over features container
  }// if chain passed
}//EoF


    /*
       
//for(std::vector<Trig::Feature<xAOD::JetContainer> >::const_iterator HLTjIt =  theHLTTrigF.begin();
  //                                                                 HLTjIt != theHLTTrigF.end(); ++HLTjIt ) {

    //  ATH_MSG_INFO("Looping over features....");
     // if(!HLTjIt->cptr()) continue;
     // const JetCollection *HLTJet = HLTjIt->cptr();
     // if(!HLTJet) continue;
     // for(JetCollection::const_iterator eit = HLTJet->begin(); eit != HLTJet->end(); ++eit ) {
       // ATH_MSG_INFO("Looping over HLTJet.....");
//	const Jet* jet = *eit;
  //      if(!jet) continue;
    //    double e = (jet->e())/CLHEP::GeV;
    //    double eta = jet->eta();
    //    double phi = jet->phi();
     //   double et = e/cosh(eta);
     //   if(et < 1.e-3) et = 0;
     //   bool hlt_thr_pass = ( et > thrHLT );
    //    if(hlt_thr_pass) {
    //      if((h = hist("HLTJet_Et")))   h->Fill(et);
    //      if((h = hist("HLTJet_eta")))  h->Fill(eta);
    //      if((h = hist("HLTJet_phi")))  h->Fill(phi);
    //      if((h2 = hist2("HLTJet_phi_vs_eta")))  h2->Fill(eta,phi);
      //  }

 Remove until ancestor method is fixed
        // get ancestor (L1RoI) and fill L1 histograms corresponding to this chain
        Trig::Feature<Jet_ROI> jRoI =  getTDT()->ancestor(<Jet_ROI>(*eit),""); // FIXME DAMMIT !!! //
        if ( !jRoI.empty() ) {
          if(!jRoI.cptr()) continue;
          const Jet_ROI *L1_Roi = jRoI.cptr();
          if(!L1_Roi) continue;
          double l1et = (L1_Roi->getET4x4())/CLHEP::GeV;
          if(l1et < 1.e-3) l1et = 0;
          double l1eta = L1_Roi->getEta();
          double l1phi = L1_Roi->getPhi();
          double dr_l2 = delta_r(l1eta,l1phi,eta,phi);
          bool id_match_found = (dr_l2 < m_deltaRCut);
          if(id_match_found && hlt_thr_pass) {
            if((h2 = hist2("Jet_l1et_vs_hltet")))   h2->Fill(et,l1et);
            if((h2 = hist2("Jet_l1eta_vs_hlteta")))  h2->Fill(eta,l1eta);
            if((h2 = hist2("Jet_l1phi_vs_hltphi")))  h2->Fill(phi,l1phi);
          } // if l1match
        } // if jRoI
*/
   //   } // for HLTjet
  //  } // for feature






// ------------------------------------------------------------------------------------

void HLTJetMonTool::fillBasicL1forChain(const std::string& theChain, double thrEt ) {

  if( !m_L1JetRoIC ) return; // TEMPORARY - Should issue a warning  

  TH1 *h(0); 
  TH2 *h2(0); 
  
  ATH_MSG_INFO("fillBasicL1forChain: ITEM: " << theChain.c_str() << " passed TDT: "<<getTDT()->isPassed(theChain.c_str()));
  
  if (getTDT()->isPassed(theChain.c_str())){
    
    Trig::FeatureContainer chainFeatures = getTDT()->features(theChain.c_str());
    for(Trig::FeatureContainer::combination_const_iterator chIt = chainFeatures.getCombinations().begin(); 
        chIt != chainFeatures.getCombinations().end(); ++chIt ) {
      
      std::vector< Trig::Feature<TrigRoiDescriptor> > theRoIdesc = chIt->get<TrigRoiDescriptor>("initialRoI");
      for(std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator combIt = theRoIdesc.begin(); 
          combIt!= theRoIdesc.end(); ++combIt){
	

	ATH_MSG_INFO("Test Loop....");

        if(!(combIt->cptr())) continue;
        if((h = hist("L1Jet_roidesc_eta")))  h->Fill(combIt->cptr()->eta());
        if((h = hist("L1Jet_roidesc_phi")))  h->Fill(combIt->cptr()->phi());
        if((h2 = hist2("L1Jet_roidesc_phi_vs_eta")))  h2->Fill(combIt->cptr()->eta(), combIt->cptr()->phi());
	
        unsigned int id =   combIt->cptr()->roiWord();
        bool id_match_found = false;
        xAOD::JetRoIContainer::const_iterator it_L1 = m_L1JetRoIC->begin();
        xAOD::JetRoIContainer::const_iterator it_e_L1 = m_L1JetRoIC->end();
	
        for (; it_L1 != it_e_L1; ++it_L1) {
          double et = ((*it_L1)->et4x4())/CLHEP::GeV;
          if(et < 1.e-3) et = 0;
          double eta = (*it_L1)->eta();
          double phi = (*it_L1)->phi();
          if(m_debuglevel) {
            const Jet_ROI::thresholds_type thrVec = (*it_L1)->thrNames();
            const Jet_ROI::thr_value_type thrVal = (*it_L1)->thrValues();
            double et6 = ((*it_L1)->et6x6())/CLHEP::GeV;
            double et8 = ((*it_L1)->et8x8())/CLHEP::GeV;
            std::ostringstream thrv; 
            unsigned int kk = 0;
            for(Jet_ROI::thresholds_type::const_iterator l1i = thrVec.begin(); l1i != thrVec.end(); ++l1i,kk++ ) {
              thrv << *l1i << " " << thrVal[kk] << ", " ;
            }
            ATH_MSG_DEBUG( "CHAIN: " << theChain << " " << et << " GeV " << eta << " " 
			   << phi << " rad " << et6 << " " << et8 << " " << thrv.str());
          }
          if(id == (*it_L1)->roiWord() ) {
            id_match_found = true;
            bool l1_thr_pass = (et > thrEt);
            ATH_MSG_DEBUG("CHAIN: " << theChain << " " << et << "\tthreshold = " << thrEt << "\tpass = " << l1_thr_pass);
            if(l1_thr_pass) {
              if((h = hist("L1Jet_Et")))  h->Fill(et);
              if((h = hist("L1Jet_eta")))  h->Fill(eta);
              if((h = hist("L1Jet_phi")))  h->Fill(phi);
              if((h2 = hist2("L1Jet_phi_vs_eta")))  h2->Fill(eta,phi);
            }
          }
        }
        double unmatched_eta = id_match_found ? -999. : combIt->cptr()->eta();
        if((h = hist("L1Jet_unmatched_eta")))  h->Fill(unmatched_eta);
      } // for combIt
    } // l2It
  } // L2 chain isPassed
} //EoF
// ------------------------------------------------------------------------------------

bool HLTJetMonTool::isChainActive(const std::string& theChain ) {
  bool isactive = true;
  if( std::abs( getTDT()->getPrescale( theChain ) + 1.0 ) < 0.001 )
    //if( std::abs( m_tdt->getPrescale( theChain ) + 1.0 ) < 0.001 )
    isactive = false;
  return isactive;

} // end isChainActive

// ------------------------------------------------------------------------------------

bool HLTJetMonTool::selectJet(const Jet *jet) {

  //if(!m_doselOFJets) return true; 

  // pt / eta
  // don't cut on eta here since we have FJ signatures to contend with:
  // if(m_reqEtaCut)       if (fabs(jet->eta()) >= m_EtaCut )              return false;
  if(m_reqMinPtCut)     if (jet->pt()/CLHEP::GeV <= m_MinPtCut )               return false;


  /* jet quality
  double jetemf = JetCaloHelper::jetEMFraction(jet);
  if(m_reqN90Cut)   
    if (jet->getMoment("n90") <= m_n90Cut && jetemf < m_ofemfracCut.at(0) )          return false;
  if(m_reqBadQCut)  
    if (jet->getMoment("LArQuality") > m_badQFrac && jetemf > m_ofemfracCut.at(1) )        return false;

  // timing
  if(m_reqTimeCut)  if (fabs(jet->getMoment("Timing")) >= m_jetTimens)     return false;
  */
  if(m_reqBadQCut) {
//    int SamplingMax = CaloSampling::Unknown;
// Temporary fix - must replace JetCaloHelper --- LS
//    double emf      = JetCaloHelper::jetEMFraction(jet);       
    double emf = 0.0;
// Temporary fix - must replace JetCaloHelper --- LS
//    double hecf     = JetCaloQualityUtils::hecF(jet);
    double hecf = 0.0;
    double larq     = jet->getMoment("LArQuality");
    double hecq     = jet->getMoment("HECQuality");
    double time     = jet->getMoment("Timing");
    double sumpttrk = jet->getMoment("sumPtTrk");
    double pt       = jet->pt();
    double eta      = jet->eta(P4SignalState::JETEMSCALE);
// Temporary fix - must replace JetCaloHelper --- LS
//    double fmax     = JetCaloQualityUtils::fracSamplingMax(jet, SamplingMax);
    double fmax = 0.0;
    double negE     = jet->getMoment("NegativeE");

    if(isBadJet(m_JetCategory, emf, hecf, larq, hecq, time, sumpttrk, eta, pt, fmax, negE)) return false;
  }
  
  return true;

} // end selectJet

// ------------------------------------------------------------------------------------
bool HLTJetMonTool::isBadJet(JetCategorytype Category, double emf, double hecf, double larq, double hecq, double time,
    double sumpttrk, double eta, double pt, double fmax, double negE) {

  double chf=sumpttrk/pt;

  if(fabs(time)>10)                                return true;
  if(emf<0.05 && chf<0.05 && fabs(eta)<2)          return true;
  if(emf<0.05 && fabs(eta)>=2)                     return true;
  if(fmax>0.99 && fabs(eta)<2)                     return true;

  if(Category==MediumBad || Category==TightBad)  {
    if(emf<0.05 && chf<0.1  && fabs(eta)<2)        return true;  
    if(emf>0.95 && chf<0.05 && fabs(eta)<2)        return true;
  }

  if(Category==TightBad)  {
    if(emf<0.1 && chf<0.2  && fabs(eta)<2)         return true;
    if(emf<0.1 && fabs(eta)>=2 )                   return true;    
    if(emf>0.9 && chf<0.02 && fabs(eta)<2)         return true;
  }

  if(fabs(negE/1000.)>60)                          return true;
  if(hecf>0.5 && fabs(hecq)>0.5)                   return true;  

  if(Category==MediumBad || Category==TightBad) {
    if(hecf>1-fabs(hecq))                          return true;
  }

  if(emf>0.95 && fabs(larq)>0.8 && fabs(eta)<2.8)  return true;

  if(Category==MediumBad || Category==TightBad) {
    if(emf>0.9 && fabs(larq)>0.8 && fabs(eta)<2.8) return true;
  }

  if(Category==TightBad) {
    if(fabs(larq)>0.95)                            return true;
    if(emf>0.98 && fabs(larq)>0.05)                return true;
  }

  return false;

}

// ------------------------------------------------------------------------------------

StatusCode HLTJetMonTool::fillOfflineHists() {

  TH1 *h(0);
  //TH2 *h2(0);
  // fill offline jets in one loop
  unsigned int Nelem = m_OFJetC.size();
  for(unsigned int k = 0; k < Nelem; k++ ) {
    const xAOD::JetContainer *jetcoll = m_OFJetC[k];
    if(jetcoll) {
      std::string malg = Form("%s%d",m_OFpfx.c_str(),k);
      std::string mgrp = m_monGroups[malg];
      std::string mgrp_eff = m_monGroups[Form("%s%dEff",m_OFpfx.c_str(),k)];
      ATH_MSG_INFO("In fillOfflineHists: mgrp = " << mgrp << " while mgrp_eff = " << mgrp_eff);

      //JetCollection::const_iterator it = jetcoll->begin();
      //JetCollection::const_iterator ite = jetcoll->end();
      std::vector<ChainMatch> mFoundL1, mFoundHLT; 
      std::vector<ChainMatch>::iterator mIt;
      JetSigIter lIt;

      mFoundL1.clear(); mFoundHLT.clear();
      unsigned int njmax = jetcoll->size();
      if(!njmax) continue;
      if(m_doselOFJets) {
        if(m_reqMaxNJetCut)  if(m_MaxNJet > 0) njmax = m_MaxNJet;
      }
      for(const auto & jet : *jetcoll) {
      //for (unsigned int numj = 0; (numj < njmax); numj++ ) {
        //const Jet* jet = jetcoll->at(numj);
        if(!jet) continue;

        // COME BACK TO THIS -- LS //
        //SignalStateHelper sigstateH(jet);
        //if(m_reqP4State) { 
        //  if( m_p4State == "JETEMSCALE"     ) sigstateH.setSignalState(P4SignalState::JETEMSCALE);
        //  if( m_p4State == "JETFINAL"       ) sigstateH.setSignalState(P4SignalState::JETFINAL);
        //  if( m_p4State == "CALIBRATED"     ) sigstateH.setSignalState(P4SignalState::CALIBRATED);
        //  if( m_p4State == "UNCALIBRATED"   ) sigstateH.setSignalState(P4SignalState::UNCALIBRATED);
        //}

        //if(m_doselOFJets) if(!selectJet(jet)) {
        //  // if jet fails selection, restore signal state
        //    sigstateH.releaseObject();
        //    continue;
        //}

        bool l1pass  = false; // = passedChain(jet,mFoundL1,"L1");
        bool hltpass = false; // = passedChain(jet,mFoundHLT,"HLT");

        bool passed_eta_central = !m_reqEtaCut;
        bool passed_eta_forward = !m_reqEtaCut;

        if(m_reqEtaCut) {
          passed_eta_forward = fabs(jet->eta()) >= m_EtaCut;
          passed_eta_central = !passed_eta_forward;
        }

        bool passL1thr, passHLTthr;

        // set mongroup to fill efficiency histograms
        setCurrentMonGroup(mgrp_eff);
        for(lIt = m_L1Items.begin(); lIt != m_L1Items.end(); ++lIt) {
          std::string itemName = (*lIt).first;
          bool passed_eta_cut = !m_reqEtaCut;
          double thrGeVItem = m_ofEtThres[itemName];
          passL1thr = (jet->pt()/CLHEP::GeV > thrGeVItem);
          ATH_MSG_DEBUG("L1ITEM: " << itemName << "\t: thr = " << thrGeVItem << "\tpass = " << passL1thr);
          if(m_reqEtaCut) {
            TString fjsig = itemName;
            if(fjsig.Contains("FJ")) passed_eta_cut = passed_eta_forward;
            else passed_eta_cut = passed_eta_central;
          }
          // if L1 eff required
          if(m_doL1TrigEff) {
	     if(getTDT()->isPassed((*lIt).second) && passed_eta_cut) {

	    //  if(m_tdt->isPassed((*lIt).second) && passed_eta_cut) {
	    // fill denominator for all L1 items
              if(passL1thr) {
                //if((h = hist(Form("%s_Eff_vs_Et_den",itemName.c_str())))) h->Fill(jet->et()/CLHEP::GeV);
                if((h = hist(Form("%s_Eff_vs_pt_den",itemName.c_str())))) h->Fill(jet->pt()/CLHEP::GeV);
                if((h = hist(Form("%s_Eff_vs_eta_den",itemName.c_str())))) h->Fill(jet->eta());
                if((h = hist(Form("%s_Eff_vs_phi_den",itemName.c_str())))) h->Fill(jet->phi());
              }
            } // if tdt
          } // if trigeff
        } // for
        // if any L1 items passed, fill numerator
        if((l1pass = passedChain(jet,mFoundL1,"L1"))) {
          ATH_MSG_DEBUG( "offline jet passed some L1 items" );
          for(mIt = mFoundL1.begin(); mIt != mFoundL1.end(); ++mIt) {
            std::string itemName = (*mIt).m_cname;
            bool pass = (*mIt).m_chpass;
            // fill num L1 eff
            if(m_doL1TrigEff) {
              bool passed_eta_cut = !m_reqEtaCut;
              if(m_reqEtaCut) {
                TString fjsig = itemName;
                if(fjsig.Contains("FJ")) passed_eta_cut = passed_eta_forward;
                else passed_eta_cut = passed_eta_central;
              }

	       if(pass && getTDT()->isPassed((*mIt).m_ctrigname) && passed_eta_cut) {
	      // if(pass && m_tdt->isPassed((*mIt).m_ctrigname) && passed_eta_cut) {
		if(passL1thr) {
                  //if((h = hist(Form("%s_Eff_vs_Et_num",itemName.c_str())))) h->Fill(jet->et()/CLHEP::GeV);
                  if((h = hist(Form("%s_Eff_vs_pt_num",itemName.c_str())))) h->Fill(jet->pt()/CLHEP::GeV);
                  if((h = hist(Form("%s_Eff_vs_eta_num",itemName.c_str())))) h->Fill(jet->eta());
                  if((h = hist(Form("%s_Eff_vs_phi_num",itemName.c_str())))) h->Fill(jet->phi());
                }
              } // if this L1 item passed
            } // if trigeff  
          } // for found L1 items
        } // if any L1 items passed


        for(lIt = m_HLTChains.begin(); lIt != m_HLTChains.end(); ++lIt) {
          std::string chainName = (*lIt).first;
          bool passed_eta_cut = !m_reqEtaCut;
          double thrGeVItem = m_ofEtThres[(*lIt).second];
          passHLTthr = (jet->pt()/CLHEP::GeV > thrGeVItem);
          ATH_MSG_DEBUG("HLTCHAIN: " << chainName << "\t: " << (*lIt).second << "\t thr = " << thrGeVItem << "\tpass = " << passHLTthr);
          if(m_reqEtaCut) {
            TString fjsig = chainName;
            if(fjsig.Contains("fj")) passed_eta_cut = passed_eta_forward;
            else passed_eta_cut = passed_eta_central;
          }
          // fill denominator for all HLT chains
          // if HLT eff required
          if(m_doHLTTrigEff) {

	    if(getTDT()->isPassed((*lIt).second) && passed_eta_cut) {
	    // if(m_tdt->isPassed((*lIt).second) && passed_eta_cut) {             
	      if(passHLTthr) {
                ATH_MSG_DEBUG("CHAIN: " << chainName << "ofjet_et = " << jet->pt()/CLHEP::GeV << "\tthr= " << m_ofEtThres[(*lIt).second]);
                //if((h = hist(Form("%s_Eff_vs_Et_den",chainName.c_str())))) h->Fill(jet->et()/CLHEP::GeV);
                if((h = hist(Form("%s_Eff_vs_pt_den",chainName.c_str())))) h->Fill(jet->pt()/CLHEP::GeV);
                if((h = hist(Form("%s_Eff_vs_eta_den",chainName.c_str())))) h->Fill(jet->eta());
                if((h = hist(Form("%s_Eff_vs_phi_den",chainName.c_str())))) h->Fill(jet->phi());
              }
            }
          }
        }
        // if any HLT chains passed, fill numerator
        if((hltpass = passedChain(jet,mFoundHLT,"HLT"))) {
          ATH_MSG_DEBUG( "offline jet passed some HLT chains" );
          for(mIt = mFoundHLT.begin(); mIt != mFoundHLT.end(); ++mIt) {
            std::string chainName = (*mIt).m_cname;
            bool pass = (*mIt).m_chpass;
            bool passed_eta_cut = !m_reqEtaCut;
            if(m_reqEtaCut) {
              TString fjsig = chainName;
              if(fjsig.Contains("fj")) passed_eta_cut = passed_eta_forward;
              else passed_eta_cut = passed_eta_central;
            }
            // fill num HLT eff
            if(m_doHLTTrigEff) {
	      if(pass && getTDT()->isPassed((*mIt).m_ctrigname) && passed_eta_cut) {

	      // if(pass && m_tdt->isPassed((*mIt).m_ctrigname) && passed_eta_cut) {  
		if(passHLTthr) {
                  //if((h = hist(Form("%s_Eff_vs_Et_num",chainName.c_str())))) h->Fill(jet->et()/CLHEP::GeV);
                  if((h = hist(Form("%s_Eff_vs_pt_num",chainName.c_str())))) h->Fill(jet->pt()/CLHEP::GeV);
                  if((h = hist(Form("%s_Eff_vs_eta_num",chainName.c_str())))) h->Fill(jet->eta());
                  if((h = hist(Form("%s_Eff_vs_phi_num",chainName.c_str())))) h->Fill(jet->phi());
                }
              }
            } // if this HLT chain passed
          } // for found HLT chains
        } // if any HLT chains passed

        // set mongroup to fill correlation histograms
        setCurrentMonGroup(mgrp);

        // restore signal state
        //done automatically by sigstateH

      } // for loop over jet
    } // if jetcoll
  } // for k (loop over all ref coll's)

  return StatusCode::SUCCESS;

} // end fillOfflineHists

// ------------------------------------------------------------------------------------

bool HLTJetMonTool::passedChain( const xAOD::Jet *jet, std::vector<ChainMatch>& mFound, const std::string& level) {

  if(!mFound.empty()) mFound.clear();
  JetSigIter chIt, chItB, chItE;
  const TrigMatch::TrigJetL1  *matchL1 = 0;
  const TrigMatch::TrigJetEF  *matchEF = 0;

  chItB = ( (level == "L1") ? m_L1Items.begin()  : 
            (level == "HLT") ? m_HLTChains.begin() : m_L1Items.end() );

  chItE = ( (level == "L1") ? m_L1Items.end()  : 
            (level == "HLT") ? m_HLTChains.end() : m_L1Items.end() );

  // loop over all items(or chains)
  bool passAny = false;
  for(chIt = chItB; chIt != chItE; chIt++ ) {
    matchL1  = 0;  
    matchEF  = 0;
    ChainMatch obj;
    obj.m_cname = (*chIt).first;
    obj.m_ctrigname = (*chIt).second;
    if(level == "L1") {
      matchL1 = m_trigMatchTool->matchToTriggerObject<TrigMatch::TrigJetL1>(jet, (*chIt).second, m_deltaRCut );
      if(matchL1) {
        obj.m_chpass = true;
        passAny = true;
        double et = matchL1->et();
        if(et < 1.e-3) et = 0;
        obj.m_eta = matchL1->eta(); obj.m_phi = matchL1->phi(); obj.m_pt = matchL1->pt(); obj.m_et = et;
        //mFound.push_back(obj);
      }
      ATH_MSG_DEBUG( "matching offline jet to L1 for item " << (*chIt).second << " match = " << matchL1 ); 
    } else if (level == "HLT") {
      matchEF = m_trigMatchTool->matchToTriggerObject<TrigMatch::TrigJetEF>(jet, (*chIt).second, m_deltaRCut );
      if(matchEF) {
        obj.m_chpass = true;
        passAny = true;
        double et = matchEF->et();
        obj.m_eta = matchEF->eta(); obj.m_phi = matchEF->phi(); obj.m_pt = matchEF->pt(); obj.m_et = et;
        // mFound.push_back(obj);
      }
      ATH_MSG_DEBUG( "matching offline jet to HLT for chain " << (*chIt).second << " match = " << matchEF ); 
    } 
    mFound.push_back(obj);
  } // end for
  return passAny; 

} // end passChain

// ------------------------------------------------------------------------------------

StatusCode HLTJetMonTool::proc( ) {

  if (m_debuglevel) 
    ATH_MSG_INFO( "in HLTJetMonTool::proc()" );

// LS 24 Jan 2014 - Interval checking now managed in ManagedMonitorTool so do nothing.
//  if(isEndOfEventsBlock){}
//  if(isEndOfLumiBlock){}
//  if(isEndOfRun) {
//  }

  return StatusCode::SUCCESS;
} // end proc

// ------------------------------------------------------------------------------------

// calculate phi1-phi2 keeping value between -pi and pi
double HLTJetMonTool::signed_delta_phi(double phi1, double phi2) {
  double phia = phi1;
  if(phi1 > M_PI) phia = phi1 - 2*M_PI;
  double phib = phi2;
  if(phi2 > M_PI) phib = phi2 - 2*M_PI;
  double dphi = phia - phib;
  if(dphi > M_PI) dphi -= 2*M_PI;
  if(dphi < -1.*M_PI) dphi += 2*M_PI;
  return dphi;
}

// ------------------------------------------------------------------------------------
double HLTJetMonTool::delta_r(double eta1, double phi1, double eta2, double phi2) {
  double dphi = signed_delta_phi(phi1,phi2);
  return sqrt( (eta1-eta2)*(eta1-eta2) + dphi*dphi);
}

// ------------------------------------------------------------------------------------

int HLTJetMonTool::basicKineVar(const std::string& hist, std::vector<std::string>& m_kinevars) {

  typedef  boost::tokenizer<boost::char_separator<char> > Tokenizer;
  int retval = -1;
  m_kinevars.clear();
  const char* separator=";";
  Tokenizer tokComp(hist, boost::char_separator<char>(separator));
  for(Tokenizer::const_iterator vars = tokComp.begin(); vars != tokComp.end(); ++vars) m_kinevars.push_back(*vars);
  retval = (int)m_kinevars.size();
  return retval;
}
// ------------------------------------------------------------------------------------
