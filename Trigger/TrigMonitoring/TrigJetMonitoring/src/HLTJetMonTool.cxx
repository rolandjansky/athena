/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "TrigJetMonitoring/HLTJetMonTool.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "GaudiKernel/SystemOfUnits.h"

//#include "TrigCaloEvent/TrigT2JetContainer.h"
//#include "JetEvent/JetCollection.h"

//#include "JetUtils/JetCaloQualityUtils.h"

//#include "EventKernel/SignalStateHelper.h"

#include <iostream>
#include <math.h>
#include <algorithm> 

#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TProfile.h"
#include "TEfficiency.h"

#include "boost/tokenizer.hpp"

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

HLTJetMonTool::HLTJetMonTool(   
    const std::string & type, 
    const std::string & name, 
    const IInterface* parent) : IHLTMonTool(type, name, parent)
{

 
  ATH_MSG_INFO( "in HLTJetMonTool::HLTJetMonTool() " );

  clearVectors();

  //declareProperty("TrigMatchTool",       m_trigMatchTool); 
  declareProperty("JetMonBase",          m_monBase = "/HLT/JetMon");

  declareProperty("DoL1Efficiency",      m_doL1TrigEff = true);
  declareProperty("DoOfflineJets",       m_doOFJets = true);
  declareProperty("DoHLTEfficiency",     m_doHLTTrigEff = true);
  //  declareProperty("DoLumiWeight",        m_doLumiWeight = true);

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

  declareProperty("L1DijetChains",       m_l1_DijetChains,"L1 for Dijet Histos");
  declareProperty("HLTDijetChains",      m_hlt_DijetChains,"HLT for Dijet Histos");
  declareProperty("OFDijetChains",       m_of_DijetChains,"OF for Dijet Histos");

  declareProperty("primary_l1jet",	 m_primary_l1jet, "Primary L1 triggers for Jet Efficiency");
  declareProperty("primary_jet",	 m_primary_jet,   "Primary HLT triggers for Jet Efficiency");
  declareProperty("hltContainers",	 m_hltContainers, "Primary HLT containers");
  //declareProperty("L1ItemNames",       m_L1Items, "L1 Items for Jet Trigger Efficiency");
  //declareProperty("HLTChainNames",     m_HLTChains, "HLT Chains for Jet Trigger Efficiency");
  
  declareProperty("L1EtThresGeV",        m_l1EtThres, "L1 ET Thresholds for L1 chains" );
  declareProperty("HLTEtThresGeV",       m_hltEtThres, "HLT ET Thresholds for EF chains" );
  declareProperty("OFEtThresGeV",        m_ofEtThres, "OF ET Thresholds for Efficiency" );

  declareProperty("HLTEtaHighThres",     m_hltEtaHighThres, "HLT Eta High Threshold for HLT eta chains" );
  declareProperty("HLTEtaLowThres",      m_hltEtaLowThres,  "HLT Eta Low Threshold for HLT eta chains" );
  declareProperty("L1EtaHighThres",      m_l1EtaHighThres,  "L1 Eta High Threshold for L1 eta chains" );
  declareProperty("L1EtaLowThres",       m_l1EtaLowThres,   "L1 Eta Low Threshold for L1 eta chains" );

  declareProperty("HLTJetn",             m_hltJetn,         "HLT jet number for efficiency calculation" );

  //Collision mode flags
  declareProperty("isPP",             m_isPP,         "collision mode flag" );
  declareProperty("isHI",             m_isHI,         "collision mode flag" );
  declareProperty("isCosmic",         m_isCosmic,     "collision mode flag" );
  declareProperty("isMC",             m_isMC,         "collision mode flag" );
  //declareProperty("isPPb",            m_isPPb,        "collision mode flag" );

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

  // Jet JVT bins
  declareProperty("JetJVTNBins",       m_jJVTnbins );
  declareProperty("JetJVTBinLo",       m_jJVTbinlo );
  declareProperty("JetJVTBinHi",       m_jJVTbinhi );

  // Jet SumPtTrk bins
  declareProperty("JetSumPtTrk500NBins", m_jSumPtTrk500nbins );
  declareProperty("JetSumPtTrk500BinLo", m_jSumPtTrk500binlo );
  declareProperty("JetSumPtTrk500BinHi", m_jSumPtTrk500binhi );

  //declareProperty("JetChainsRegex",     m_chainsByRegexp);

  declareProperty("EMFractionCut",      m_emfracCut = 0.9);

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
  declareProperty("JetCategory",           m_JetCategory = "LooseBad"  );
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

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void HLTJetMonTool::clearVectors() {
  //m_ofemfracCut.clear();
  m_HLTJetC.clear();
  m_OFJetC.clear();

  m_monitoring_l1jet.clear();
  m_monitoring_jet.clear();
  m_primary_l1jet.clear();
  m_primary_jet.clear();

  m_l1_DijetChains.clear();
  m_hlt_DijetChains.clear();
  m_of_DijetChains.clear();  

  m_basicL1Trig.clear();
  m_basicHLTTrig.clear();

  m_L1Items.clear();
  m_HLTChains.clear();
  m_hltContainers.clear();

  m_l1EtThres.clear();
  m_hltEtThres.clear();
  m_ofEtThres.clear();

  m_hltEtaHighThres.clear();
  m_hltEtaLowThres.clear();

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
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

HLTJetMonTool::~HLTJetMonTool() {
  ATH_MSG_INFO( "in HLTJetMonTool::~HLTJetMonTool() " );
} // end ~HLTJetMonTool

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

StatusCode HLTJetMonTool::init() {

  ATH_MSG_INFO( "in HLTJetMonTool::init()" );

   //Luminosity information

  m_lumi_weight=1.;
  m_lumiBlock=-1;

  // Monitoring trigger names specified through the congfiguration file or menu database, for menu-aware monitoring
  if(m_monitoring_l1jet.empty()) m_monitoring_l1jet.push_back("L1_J15");
  if(m_monitoring_jet.empty())   m_monitoring_jet.push_back("j60");
  if(m_primary_l1jet.empty())    m_primary_l1jet.push_back("L1_J15");
  // if(m_primary_l1jet.empty())    m_monitoring_l1jet.push_back("j60");

 
  // Fill triggers for L1 basic plots. String pairs of the form ("MonGroup name","L1 Trigger Name")
  unsigned int k = 0; 
  for( ; k< m_monitoring_l1jet.size(); ++k) {
    ATH_MSG_DEBUG( "monitoring_l1jet : " << m_monitoring_l1jet[k].c_str() );

    //  std::vector<std::string> selectChains = getTDT()->getListOfTriggers(m_monitoring_l1jet[k].c_str()); //this will configure the chain only if the chain is in the TDT
    // if (selectChains.size()){
    m_basicL1Trig.insert(std::pair<std::string,std::string>(m_monitoring_l1jet[k].c_str(),m_monitoring_l1jet[k].c_str()));
  }
  // }
 
  // Fill trigger for HLT basic plots. String pairs of the form ("MonGroup name","HLT Filter Name")
  k = 0;
  for( ; k < m_monitoring_jet.size(); ++k) {
    ATH_MSG_DEBUG( "monitoring_jet : " << m_monitoring_jet[k].c_str() );
    m_basicHLTTrig.insert(std::pair<std::string,std::string>( m_monitoring_jet[k].c_str(), m_monitoring_jet[k].c_str()));
  }
  
  // L1 Triggers for efficiency plots. String pairs of the form ("MonGroup name","L1 Trigger Name")
  k = 0;
  for( ; k < m_primary_l1jet.size(); ++k) {
    ATH_MSG_DEBUG( "primary_l1jet : " << m_primary_l1jet[k].c_str() );
    m_L1Items.insert(std::pair<std::string,std::string>(m_primary_l1jet[k].c_str(),m_primary_l1jet[k].c_str()));
  }
 
  // HLT chains for efficiencyplots. String pairs of the form ("MonGroup name","HLT Filter Name")
  k = 0;
  for( ; k < m_primary_jet.size(); ++k) {
    ATH_MSG_DEBUG( "primary_jet : " << m_primary_jet[k].c_str() );
    m_HLTChains.insert(std::pair<std::string,std::string>(m_primary_jet[k].c_str(),m_primary_jet[k].c_str()));
  }

  // L1 Chains for DiJet histos
  k = 0;
  for ( ; k < m_l1_DijetChains.size(); ++k){
    ATH_MSG_DEBUG( "L1 DiJet chains : "<< m_l1_DijetChains[k].c_str() );
  }
  // OF Chains for DiJet histos
  k = 0;
  for ( ; k < m_of_DijetChains.size(); ++k){
    ATH_MSG_DEBUG( "OF DiJet chains : "<< m_of_DijetChains[k].c_str() );
  }

  k = 0;
  for ( ; k < m_hlt_DijetChains.size(); ++k){
    ATH_MSG_DEBUG( "HLT DiJet chains : "<< m_hlt_DijetChains[k].c_str() );
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

    m_hltEtaHighThres.insert(std::pair<std::string,double>("j60",3.2));
    m_hltEtaHighThres.insert(std::pair<std::string,double>("j360",3.2));
    m_hltEtaLowThres.insert(std::pair<std::string,double>("j60",0.));
    m_hltEtaLowThres.insert(std::pair<std::string,double>("j360",0.));

   
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

  if (msgLvl(MSG::DEBUG)) {
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
  if(m_JetQuality == "MediumBad" )      m_JetCategory = "MediumBad";
  else if(m_JetQuality == "TightBad" )  m_JetCategory = "TightBad";
  else                                  m_JetCategory = "LooseBad";

  
  return StatusCode::SUCCESS;

} // end init

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

StatusCode HLTJetMonTool::book( ) {

  ATH_MSG_DEBUG( "in HLTJetMonTool::book()" );

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
  for(unsigned int k = 0; k < m_jJVTnbins.size(); k++) {
    float thebinSize = m_jJVTnbins[k] > 0 ? fabs(m_jJVTbinhi[k]-m_jJVTbinlo[k]) / m_jJVTnbins[k] : -1.;
    m_jJVTperbin.push_back(thebinSize);
  }
  for(unsigned int k = 0; k < m_jSumPtTrk500nbins.size(); k++) {
    float thebinSize = m_jSumPtTrk500nbins[k] > 0 ? fabs(m_jSumPtTrk500binhi[k]-m_jSumPtTrk500binlo[k]) / m_jSumPtTrk500nbins[k] : -1.;
    m_jSumPtTrk500perbin.push_back(thebinSize);
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
  TH2 *h2(0);

  for (std::map<std::string, TH1 *>::iterator i = m_hist.begin(); i != m_hist.end(); ++i) {
    h = i->second;
    if(h) setHistProperties(h);
  } // for

  for (std::map<std::string, TH2 *>::iterator i = m_hist2.begin(); i != m_hist2.end(); ++i) {
    h2 = (TH2 *) i->second;
    if(h2) set2DHistProperties(h2);
  } // for

  return StatusCode::SUCCESS;

} // end book

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void HLTJetMonTool::bookJetHists() {

  // Define levels and choose to plot additional histograms
  // Levels = "L1", "HLT" + offline
  std::vector<std::string> levels;
  std::vector<std::string> bookvars;
  std::string varlist;

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //:: bookBasicHists - 
  //:: L1 histograms in m_L1dir
  levels.push_back("L1");
  varlist = "n;et;eta;phi;phi_vs_eta;e_vs_eta;e_vs_phi;phi_vs_eta_lar;sigma_vs_lb;";

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
  
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //:: bookBasicHists - 
  //:: HLT histograms in m_HLTdir
  setCurrentMonGroup(HLTdir);
  levels.clear(); levels.push_back("HLT");

  unsigned int k = 0; 
  for(JetSigIter hj= m_HLTJetKeys.begin(); hj != m_HLTJetKeys.end(); ++hj, k++) {
    // book histograms for each HLT jet container 
    varlist = "n;et;high_et;eta;phi;emfrac;hecfrac;jvt;sumpttrk500;phi_vs_eta;e_vs_eta;e_vs_phi;phi_vs_eta_lar;sigma_vs_lb;";
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


  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //::  fillBasicL1forChain - "roidesc_eta;roidesc_phi;roidesc_phi_vs_eta;unmatched_eta"; // all with L1 prefix
  //::                      - "et;eta;phi;phi_vs_eta;"; // all with L1 prefix
  //:: 

  // L1 Chains
  varlist="et;eta;phi;phi_vs_eta;e_vs_eta;e_vs_phi;sigma_vs_lb;n;";
  nvar = basicKineVar(varlist,bookvars);
  if(nvar==0) ATH_MSG_INFO("Error in bookKineVars - variable list not tokenized!");
  levels.clear(); levels.push_back("L1");
  for(JetSigIter k= m_basicL1Trig.begin(); k != m_basicL1Trig.end(); ++k ) {
    const std::string theDir = L1dir + "/" + (*k).first;
    ATH_MSG_DEBUG("Booking histograms for " << theDir);
    m_monGroups[(*k).first] = theDir;
    addMonGroup (new MonGroup(this, theDir,run));
    setCurrentMonGroup(theDir);
    bookBasicHists(levels,bookvars);
  }

  // L1 only
  varlist="roidesc_eta;roidesc_phi;roidesc_phi_vs_eta;";  // L1 prefix
  nvar = basicKineVar(varlist,bookvars);
  if(nvar==0) ATH_MSG_INFO("Error in bookKineVars - variable list not tokenized!");
  levels.clear(); levels.push_back("L1");
  for(JetSigIter k= m_basicL1Trig.begin(); k != m_basicL1Trig.end(); ++k ) {
    setCurrentMonGroup(m_monGroups[(*k).first]);
    bookBasicHists(levels,bookvars);
  }
    
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //:: fillBasicHLTforChain - "l1et_vs_hltet;l1eta_vs_hlteta;l1phi_vs_hltphi;"
  //::                     - "et;eta;phi;phi_vs_eta;et_EffRelL2;eta_EffRelL2;phi_EffRelL2" // with HLT prefix

  // HLT Chains
  // HLT basic histograms
  varlist="et;leading_et;high_et;eta;phi;emfrac;hecfrac;jvt;sumpttrk500;phi_vs_eta;e_vs_eta;e_vs_phi;sigma_vs_lb;n;";
  nvar = basicKineVar(varlist,bookvars);
  levels.clear(); levels.push_back("HLT"); /*levels.push_back("L1");*/
  for(JetSigIter k= m_basicHLTTrig.begin(); k != m_basicHLTTrig.end(); ++k ) {
    const std::string theDir = HLTdir + "/" + (*k).first;
    m_monGroups[(*k).first] = theDir;
    ATH_MSG_DEBUG("Booking histograms for " << theDir);
    addMonGroup (new MonGroup(this, theDir, run));
    setCurrentMonGroup(theDir);
    bookBasicHists(levels,bookvars);
  }
 
  /* 
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
  */

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //:: Offline Jets - 
  //:: OF m_OFpfx for each offline jet container
  //:: OF histograms will be in their respective folder by container names
  //:: Example: if m_OFpfx = "OF_" and m_OFJetKeys[] = {"AntiKt4LCTopoJets", "AntiKt10EMTopoJets"}
  //:: then dirs will be {"Rec_AntiKt4LCTopoJets", "Rec_AntiKt10EMTopoJets"}

  if(m_doOFJets) {
    k = 0; // FIXME
    for(JetSigIter ofj= m_OFJetKeys.begin(); ofj != m_OFJetKeys.end(); ++ofj, k++) {
      // book histograms for each offline jet container 
      varlist = "n;et;eta;phi;emfrac;hecfrac;jvt;sumpttrk500;phi_vs_eta;e_vs_eta;e_vs_phi;phi_vs_eta_lar;sigma_vs_lb;";
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


  //::::::::::::::::::::::::::::::::::::::::BOOK DIJET HISTOS::::::::::::::::::::::::::::::::::::::::::::::::

  for(JetSigIter l1= m_basicL1Trig.begin(); l1 != m_basicL1Trig.end(); ++l1 ) {
    for (unsigned int k=0; k < m_l1_DijetChains.size(); ++k){
      
      if (m_l1_DijetChains[k] == (*l1).first){ //book L1 vs L1
	setCurrentMonGroup(m_monGroups[(*l1).first]);
	bookDijetHistos((*l1).first,(*l1).first);
      }
    }

    for(JetSigIter ofj= m_OFJetKeys.begin(); ofj != m_OFJetKeys.end(); ++ofj) { //book L1 vs OF
      for (unsigned int k=0; k < m_l1_DijetChains.size(); ++k){
	for (unsigned int j=0; j < m_of_DijetChains.size(); ++j){

	  if (m_l1_DijetChains[k] == (*l1).first && m_of_DijetChains[j] == (*ofj).second){
	    setCurrentMonGroup(m_monGroups[(*l1).first]);
	    bookDijetHistos((*l1).first,(*ofj).second);
	  }
	}
      }
    } //loop OF

    for(JetSigIter hj= m_basicHLTTrig.begin(); hj != m_basicHLTTrig.end(); ++hj) { //book L1 vs HLT
      for (unsigned int k=0; k < m_l1_DijetChains.size(); ++k){
	for (unsigned int j=0; j < m_hlt_DijetChains.size(); ++j){
	  
	  if (m_l1_DijetChains[k] == (*l1).first && m_hlt_DijetChains[j] == (*hj).first){
	    setCurrentMonGroup(m_monGroups[(*l1).first]);
	    bookDijetHistos((*l1).first,(*hj).first);
	  }
	}
      }
    } //loop HLT
  } //loop L1


for(JetSigIter hj= m_basicHLTTrig.begin(); hj != m_basicHLTTrig.end(); ++hj) {
    for (unsigned int k=0; k < m_hlt_DijetChains.size(); ++k){
      
      if (m_hlt_DijetChains[k] == (*hj).first){ //book HLT vs HLT
	setCurrentMonGroup(m_monGroups[(*hj).first]);
	bookDijetHistos((*hj).first,(*hj).first);
      }
    }

    for(JetSigIter ofj= m_OFJetKeys.begin(); ofj != m_OFJetKeys.end(); ++ofj) { //book HLT vs OF
      for (unsigned int k=0; k < m_hlt_DijetChains.size(); ++k){
	for (unsigned int j=0; j < m_of_DijetChains.size(); ++j){

	  if (m_hlt_DijetChains[k] == (*hj).first && m_of_DijetChains[j] == (*ofj).second){
	    setCurrentMonGroup(m_monGroups[(*hj).first]);
	    bookDijetHistos((*hj).first,(*ofj).second);
	  }
	}
      }
    } //loop OF
  } //loop L1


  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


  if(msgLvl(MSG::DEBUG)) {
    std::ostringstream mongrps;
    std::map<std::string, std::string>::iterator it = m_monGroups.begin();
    for(; it != m_monGroups.end(); it++ )
      mongrps << Form("\n\t%10s <==> %10s",((*it).first).c_str(), ((*it).second).c_str());
    ATH_MSG_DEBUG( mongrps.str() );
  }

} // end bookJetHists

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void HLTJetMonTool::bookDijetHistos(const std::string& trigjet, const std::string& ofjet) {


  int   nbinsEt = 150;
  float binloEt = 0.;
  float binhiEt = 1500.;

  int   nbinsEtOF = 500;
  float binloEtOF = 0.;
  float binhiEtOF = 5000.;



  if (trigjet==ofjet){
 
    //L1/HLT lead Et vs. OF lead Et
    TString htitle = Form("%s E_{T} w.r.t %s E_{T}; Leading E_{T}^{jet} [GeV]; Subleading E_{T}^{jet} [GeV]",trigjet.c_str(), ofjet.c_str());
    TString hname = Form("%s_leadEt_vs_%s_subleadEt",trigjet.c_str(), ofjet.c_str());
    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,nbinsEt,binloEt,binhiEt));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );
    
  }else{
    
    //L1/HLT lead Et vs. OF lead Et
    TString htitle = Form("%s E_{T} w.r.t %s E_{T}; Leading Trigger E_{T}^{jet} [GeV]; Leading OF  E_{T}^{jet} [GeV]",trigjet.c_str(), ofjet.c_str());
    TString hname = Form("%s_leadEt_vs_%s_leadEt",trigjet.c_str(), ofjet.c_str());
    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,nbinsEtOF,binloEtOF,binhiEtOF));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );
    
    //L1/HLT lead Et vs. OF sublead Et
    htitle = Form("%s E_{T} w.r.t %s E_{T}; Leading Trigger E_{T}^{jet} [GeV]; Subleading OF  E_{T}^{jet} [GeV]",trigjet.c_str(), ofjet.c_str());
    hname = Form("%s_leadEt_vs_%s_subleadEt",trigjet.c_str(), ofjet.c_str());
    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,nbinsEtOF,binloEtOF,binhiEtOF));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );
    
    //L1/HLT sublead Et vs. OF lead Et
    htitle = Form("%s E_{T} w.r.t %s E_{T}; Subleading Trigger E_{T}^{jet} [GeV]; Leading OF  E_{T}^{jet} [GeV]",trigjet.c_str(), ofjet.c_str());
    hname = Form("%s_subleadEt_vs_%s_leadEt",trigjet.c_str(), ofjet.c_str());
    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,nbinsEtOF,binloEtOF,binhiEtOF));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );
    
    //L1/HLT sublead Et vs. OF sublead Et
    htitle = Form("%s E_{T} w.r.t %s E_{T}; Subleading Trigger  E_{T}^{jet} [GeV]; Subleading OF  E_{T}^{jet} [GeV]",trigjet.c_str(), ofjet.c_str());
    hname = Form("%s_subleadEt_vs_%s_subleadEt",trigjet.c_str(), ofjet.c_str());
    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,nbinsEtOF,binloEtOF,binhiEtOF));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );
  }
}//EoF

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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

    int nbinsEt = (  (itlvl == "L1")  ? m_l1nbinsEt[k]*4 : 
                  (  (itlvl == "HLT") ? m_hltnbinsEt[k]*4 : m_jEtnbins[0]*4 ));

    float binloEt = ( (itlvl == "L1")  ? m_l1binloEt[k] : 
                    ( (itlvl == "HLT") ? m_hltbinloEt[k] : m_jEtbinlo[0] ));

    float binhiEt = (  (itlvl == "L1")  ? m_l1binhiEt[k] : 
                    (  (itlvl == "HLT") ? m_hltbinhiEt[k] : m_jEtbinhi[0] ));

    TString trigItem = (*it).second;
    TString trigName = (*it).first;
   


    // if (itlvl=="HLT"){
      TString ContainerName=m_hltContainers[(*it).second];
      //}else{
      //  TString ContainerName="";
      //}


      //::::::::::::::::::::::::::::: 1D HISTOS :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // eff vs. pt
    TString htitle = Form("%s : %s Efficiency w.r.t %s vs. p_{T}; p_{T}^{jet} [GeV]; Efficiency", ContainerName.Data(), trigItem.Data(), ofjet.c_str());
    TString hname = Form("%s_Eff_vs_pt_num",trigName.Data());

    addHistogram(new TH1F(hname, htitle,nbinsEt,binloEt,binhiEt));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );

    hname = Form("%s_Eff_vs_pt_den",trigName.Data());
    addHistogram(new TH1F(hname, htitle,nbinsEt,binloEt,binhiEt));

    hname = Form("%s_Eff_vs_pt",trigName.Data());
    addHistogram(new TH1F(hname, htitle,nbinsEt,binloEt,binhiEt));
  
    hname = Form("%s_EffPt",trigName.Data());
    addProfile(new TProfile(hname, htitle,nbinsEt,binloEt,binhiEt));

    /*
    // eff vs. Et
    htitle = Form("%s : %s Efficiency w.r.t %s vs. E_{T}; E_{T}^{jet} [GeV]; Efficiency", ContainerName.Data(), trigItem.Data(), ofjet.c_str());
    hname = Form("%s_Eff_vs_Et_num",trigName.Data());

    addHistogram(new TH1F(hname, htitle,nbinsEt,binloEt,binhiEt));
    ATH_MSG_DEBUG("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );

    hname = Form("%s_Eff_vs_Et_den",trigName.Data());
    addHistogram(new TH1F(hname, htitle,nbinsEt,binloEt,binhiEt));
    */

    // eff vs. eta
    htitle = Form("%s : %s Efficiency w.r.t %s vs. #eta; #eta^{jet}; Efficiency",ContainerName.Data(),trigItem.Data(), ofjet.c_str());
    hname = Form("%s_Eff_vs_eta_num",trigName.Data());

    addHistogram(new TH1F(hname, htitle,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));

    hname = Form("%s_Eff_vs_eta_den",trigName.Data());
    addHistogram(new TH1F(hname, htitle,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));

    hname = Form("%s_Eff_vs_eta",trigName.Data());
    addHistogram(new TH1F(hname, htitle,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));

    hname = Form("%s_EffEta",trigName.Data());
    addProfile(new TProfile(hname, htitle,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));

    // eff vs. phi
    htitle = Form("%s : %s Efficiency w.r.t %s vs. #varphi; #varphi^{jet} [rad]; Efficiency",ContainerName.Data(),trigItem.Data(), ofjet.c_str());
    hname = Form("%s_Eff_vs_phi_num",trigName.Data());

    addHistogram(new TH1F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));

    hname = Form("%s_Eff_vs_phi_den",trigName.Data());
    addHistogram(new TH1F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));

    hname = Form("%s_Eff_vs_phi",trigName.Data());
    addHistogram(new TH1F(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));

    hname = Form("%s_EffPhi",trigName.Data());
    addProfile(new TProfile(hname, htitle,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));


    //Unmatched jets pt
    htitle = Form("%s : %s Unmatched jets p_{T} w.r.t %s; Trigger p_{T}^{jet} [GeV]; Number of unmatched jets", ContainerName.Data(), trigItem.Data(), ofjet.c_str());
    hname = Form("%s_unmatched_pt",trigName.Data());

    addHistogram(new TH1F(hname, htitle,nbinsEt,binloEt,binhiEt));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );


    //Resolution
    
    htitle = Form("%s : %s Resolution w.r.t %s; (Trigger p_{T}^{jet}-OF p_{T}^{jet})/OF p_{T}^{jet} ;", ContainerName.Data(), trigItem.Data(), ofjet.c_str());
    hname = Form("%s_Resolution_pt",trigName.Data());

    addHistogram(new TH1F(hname, htitle,100,-1,1));
    

    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


    //::::::::::::::::::::::::::::::: 2D HISTOS :::::::::::::::::::::::::::::::::::::::::::::::::::

    // eff vs. pt vs. eta
    htitle = Form("%s : %s Efficiency w.r.t %s vs. p_{T} vs. #eta; p_{T}^{jet} [GeV]; #eta^{jet}", ContainerName.Data(), trigItem.Data(), ofjet.c_str());
    hname = Form("%s_Eff_vs_pt_vs_eta_num",trigName.Data());

    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );

    hname = Form("%s_Eff_vs_pt_vs_eta_den",trigName.Data());
    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,m_jetanbins[0],m_jetabinlo[0],m_jetabinhi[0]));

    // eff vs. pt vs. phi
    htitle = Form("%s : %s Efficiency w.r.t %s vs. p_{T} vs. #varphi; p_{T}^{jet} [GeV]; #varphi^{jet}", ContainerName.Data(), trigItem.Data(), ofjet.c_str());
    hname = Form("%s_Eff_vs_pt_vs_phi_num",trigName.Data());

    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );

    hname = Form("%s_Eff_vs_pt_vs_phi_den",trigName.Data());
    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,m_jphinbins[0],m_jphibinlo[0],m_jphibinhi[0]));


    //Trig. vs. OF pt
    htitle = Form("%s : %s p_{T} w.r.t %s p_{T}; Leading OF p_{T}^{jet} [GeV]; Leading Trigger p_{T}^{jet}", ContainerName.Data(), trigItem.Data(), ofjet.c_str());
    hname = Form("%s_pt_vs_OF_pt",trigName.Data());

    addHistogram(new TH2F(hname, htitle,nbinsEt,binloEt,binhiEt,nbinsEt,binloEt,binhiEt));
    ATH_MSG_VERBOSE("Booked " << hname << "(" << nbinsEt << "," << binloEt << "," << binhiEt << ")" );


    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  } // end for

} // end bookTrigEffHist


// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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

      // jet High ET
      if(*var == "high_et") {
        htitle = Form("%s transverse energy; E_{T} [GeV]; entries/%3.1f GeV",title.Data(),m_jEtperbin[0]*2);
        hname = Form("%sJet_HighEt",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jEtnbins[0]*2,m_jEtbinlo[0],m_jEtbinhi[0]*2));
      }

      // leading jet ET
      if(*var == "leading_et") {
        htitle = Form("%s leading jet transverse energy; E_{T} [GeV]; entries/%3.1f GeV",title.Data(),m_jEtperbin[0]);
        hname = Form("%sJet_Leading_Et",lvl.Data());
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
      
      // jet jvt
      if(*var == "jvt") {
        htitle = Form("%s jvt; a.u.; entries/%3.1f",title.Data(),m_jJVTperbin[0]);
        hname = Form("%sJet_JVT",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jJVTnbins[0],m_jJVTbinlo[0],m_jJVTbinhi[0]));
      }

      // jet sumpttrk500
      if(*var == "sumpttrk500") {
        htitle = Form("%s_sumpttrk50;0 a.u.; entries/%3.1f",title.Data(),m_jSumPtTrk500perbin[0]);
        hname = Form("%sJet_SumPtTrk500",lvl.Data());
        addHistogram(new TH1F(hname, htitle,m_jSumPtTrk500nbins[0],m_jSumPtTrk500binlo[0],m_jSumPtTrk500binhi[0]));
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

      //::::::::::::::::::::::: LUMINOSITY HISTOS :::::::::::::::::::::::::::::::::::::::::::::::::
 
      if(*var == "sigma_vs_lb") {
	htitle = Form("%s #sigma vs. LumiBlock; N. LumiBlock; N/L",title.Data());
	hname = Form("%sSigma_vs_LB",lvl.Data());
	addHistogram(new TH1F(hname, htitle,10000,0,10000));
      }

      //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      
    } // for chain

  } // end for

} // end bookJetHist

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void HLTJetMonTool::setHistProperties(TH1* h) {

  if(!h) return;
  h->SetFillColor(42);
  h->SetTitleSize(0.037,"X");
  h->SetTitleSize(0.037,"Y");
  h->SetLabelSize(0.033,"X");
  h->SetLabelSize(0.033,"Y");
  h->Sumw2();
  h->SetOption("hist");

} // end setHistProperties

void HLTJetMonTool::set2DHistProperties(TH2* h) {

  if(!h) return;
  h->SetOption("CONT0");

} // end set2DHistProperties

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

StatusCode HLTJetMonTool::retrieveContainers() {

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve xAOD L1 ROI 
  m_L1JetRoIC = 0;
  sc = evtStore()->retrieve(m_L1JetRoIC, m_L1xAODJetKey);
  if(sc.isFailure() || !m_L1JetRoIC) {
    ATH_MSG_INFO ("Could not retrieve LVL1JetRoIs with key \"" << m_L1xAODJetKey << "\" from TDS"  );
  }
  else {
    ATH_MSG_DEBUG(" Retrieved LVL1JetROIs with key \"" << m_L1xAODJetKey << "\" from TDS" );
  }
  
  // retrieve HLT jets
  // clear before retrieving containers
  m_HLTJetC.clear();
  for(JetSigIter hltj=m_HLTJetKeys.begin(); hltj != m_HLTJetKeys.end(); ++hltj ) {
    const xAOD::JetContainer *hltcoll = 0;
    sc = evtStore()->retrieve(hltcoll, (*hltj).second);
    if(sc.isFailure() || !hltcoll) {
      ATH_MSG_INFO ("Could not retrieve JetCollection with key \"" << (*hltj).second << "\" from TDS"  );
    }
    else {
      ATH_MSG_DEBUG("FOUND JetCollection with key \"" << (*hltj).second << "\" from TDS"  );
    }
    m_HLTJetC.push_back(hltcoll);
  } // end for

  if(!m_doOFJets) return sc;
  // retrieve OF Jets 
  // clear before retrieving containers
  m_OFJetC.clear();
  for(JetSigIter ofj= m_OFJetKeys.begin(); ofj != m_OFJetKeys.end(); ++ofj ) {
    const xAOD::JetContainer *jetcoll = 0;
    sc = evtStore()->retrieve(jetcoll, (*ofj).second /*m_OFJetKeys[k]*/);
    if(sc.isFailure() || !jetcoll) {
      ATH_MSG_INFO ("Could not retrieve JetCollection with key \"" << (*ofj).second << "\" from TDS"  );
    }
    else {
      ATH_MSG_DEBUG("FOUND JetCollection with key \"" << (*ofj).second << "\" from TDS"  );
    }
    m_OFJetC.push_back(jetcoll);
  } // end for

  // Override SC failure on retrieve, because we may want to run on files
  // without every single chain activated!?
  return StatusCode::SUCCESS;

} // end retrieveContainers

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int HLTJetMonTool::retrieveLumiBlock(){
  
  const xAOD::EventInfo* eventInfo = 0; 
  
  StatusCode sc=evtStore()->retrieve(eventInfo,"EventInfo"); 
  if (sc.isFailure() || 0 == eventInfo ){
    ATH_MSG_ERROR("Could not retrieve EventInfo. LumiBlock set to -1");
    return -1;
  }
   
  return eventInfo->lumiBlock();
}



StatusCode HLTJetMonTool::fill() {

  ATH_MSG_DEBUG( "in HLTJetMonTool::fill()" );

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve containers
  sc = retrieveContainers();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "HLTJetMonTool::retrieveContainers() failed" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( "HLTJetMonTool::retrieveContainers() returned success" );

  // fill histograms
  sc = fillJetHists();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "HLTJetMonTool::fillJetHists() failed" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( "HLTJetMonTool::fillJetHists() returned success" );

  return StatusCode::SUCCESS;

} // end fill()

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

StatusCode HLTJetMonTool::fillJetHists() {

  
  // if (lbLumiWeight()>0 && m_doLumiWeight){//get weight from luminosity of the corresponding BCID
  //  m_lumi_weight=1/lbLumiWeight();
  // }
  
  m_lumiBlock=retrieveLumiBlock(); //get lb number
  m_v_lbn.push_back(m_lumiBlock);

  //  ATH_MSG_INFO("lbLumiWeight() = "<<lbLumiWeight()<<" m_lumi_weight = "<<m_lumi_weight<<" LumiBlock = "<<m_lumiBlock<<" first v_lb = "<<m_v_lbn.front()<<" last m_v_lbn = "<<m_v_lbn.back());

  StatusCode sc = StatusCode::SUCCESS;
  sc = fillBasicHists();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "HLTJetMonTool::fillBasicHists() failed" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( "HLTJetMonTool::fillBasicHists() returned success" );

  // fill offline jet and trigger eff hists
 
  if(m_doOFJets) {
    sc = fillOfflineHists();
    if (sc.isFailure()) {
      ATH_MSG_WARNING ( "HLTJetMonTool::fillOfflineHists() failed" );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG ( "HLTJetMonTool::fillOfflineHists() returned success" );    
  }
  
// fill dijet monitoring hists
    sc = fillDiJetHists();
    if (sc.isFailure()) {
      ATH_MSG_WARNING ( "HLTJetMonTool::fillDiJetHists() failed" );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG ( "HLTJetMonTool::fillDiJetHists() returned success" );    

  return StatusCode::SUCCESS;

} // end fillJetHists

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

StatusCode HLTJetMonTool::fillBasicHists() {

  TH1 *h(0);
  TH2 *h2(0);

  // Get Jet RoI
  // L1 begin filling basic histograms
  ATH_MSG_DEBUG ("Filling L1 Jets");

  if(m_L1JetRoIC) {
    setCurrentMonGroup(m_monGroups["L1"]);
    ATH_MSG_DEBUG( "Mon group set to " << m_monGroups["L1"] );
    unsigned int L1Roi_num = 0;

    xAOD::JetRoIContainer::const_iterator it_L1 = m_L1JetRoIC->begin();
    xAOD::JetRoIContainer::const_iterator it_e_L1 = m_L1JetRoIC->end();
    for ( ; it_L1 != it_e_L1; it_L1++) {
      L1Roi_num++;
      double et = ( (*it_L1)->et4x4())/Gaudi::Units::GeV;
      if(et < 1.e-3) et = 0.;
      double eta = (*it_L1)->eta();
      double ene = et * cosh(eta);
      double phi = (*it_L1)->phi();
      
      ATH_MSG_DEBUG( "et =  " << et <<  "\teta = " << eta << "\tene = " << ene << "\tphi = "<<phi );

      if((h = hist("L1Jet_Et"))) { 
        ATH_MSG_DEBUG( "found L1Jet_Et" ); 
        h->Fill(et,m_lumi_weight); 
      }
      if((h  = hist("L1Jet_eta")))          h->Fill(eta,m_lumi_weight);
      if((h  = hist("L1Jet_phi")))          h->Fill(phi,m_lumi_weight);

      if((h  = hist("L1Sigma_vs_LB"))){      
	//	h->GetXaxis()->SetRangeUser(*std::min_element(m_v_lbn.begin(),m_v_lbn.end())-1,*std::max_element(m_v_lbn.begin(),m_v_lbn.end())+1);
	h->Fill(m_lumiBlock,m_lumi_weight);
      }

      if((h2 = hist2("L1Jet_E_vs_eta")))    h2->Fill(eta,ene,m_lumi_weight);
      if((h2 = hist2("L1Jet_E_vs_phi")))    h2->Fill(phi,ene,m_lumi_weight);
      if((h2 = hist2("L1Jet_phi_vs_eta")))  h2->Fill(eta,phi,m_lumi_weight);
    } // end for it_L1

    if((h = hist("L1Jet_n"))) {
      ATH_MSG_DEBUG( " Number of L1JetROI's " << L1Roi_num );  
      h->Fill(L1Roi_num,m_lumi_weight);
    }

    // begin fill L1  per chain -- will be filled in L1/EF by using ancestor methods 

    m_v_L1jet.clear();
    m_v_L1index.clear();
    m_v_L1index.push_back(0);
    m_n_index=0;    

    for(JetSigIter l1 = m_basicL1Trig.begin(); l1 != m_basicL1Trig.end(); ++l1) {
      setCurrentMonGroup(m_monGroups[(*l1).first]);
      ATH_MSG_DEBUG("Calling fillBasicL1forChain(" << (*l1).second << ", " << m_l1EtThres[(*l1).second] << ")" );
      fillBasicL1forChain((*l1).second, m_l1EtThres[(*l1).second]);     
    } // end fill L1 per chain
    
  } // end if m_L1RoiC

  // fill HLT jets
  ATH_MSG_DEBUG ("Filling HLT Jets");

  unsigned int Nelem = m_HLTJetC.size();
  ATH_MSG_DEBUG ("m_HLTJetC size = " << Nelem );
  for(unsigned int k = 0; k < Nelem; k++ ) {
    const xAOD::JetContainer *jetcoll = 0;
    jetcoll = m_HLTJetC[k];

    if(jetcoll) {
      std::string lvl = m_HLTpfx; 
      std::string mgrp = m_monGroups[Form("%s%d",m_HLTpfx.c_str(),k)];
      
      ATH_MSG_DEBUG( "level set to " << lvl <<  " and mongroup set to " << mgrp );
      setCurrentMonGroup(mgrp);
      
      if((h  = hist(Form("%sSigma_vs_LB",lvl.c_str())))){    
	//	h->GetXaxis()->SetRangeUser(*std::min_element(m_v_lbn.begin(),m_v_lbn.end())-1,*std::max_element(m_v_lbn.begin(),m_v_lbn.end())+1);
	h->Fill(m_lumiBlock,m_lumi_weight);
      }
      
      unsigned int n_HLTJet = 0;
      for(const auto & thisjet : *jetcoll) {
        n_HLTJet++;
	
      	if(/*m_debuglevel*/1) {
	  //checks jet variables
	  ATH_MSG_VERBOSE( "REGTEST Looking at jet " << n_HLTJet);
	  ATH_MSG_VERBOSE( "REGTEST    pt: " << thisjet->pt() );
	  ATH_MSG_VERBOSE( "REGTEST    eta: " << thisjet->eta() );
	  ATH_MSG_VERBOSE( "REGTEST    phi: " << thisjet->phi() );
	  ATH_MSG_VERBOSE( "REGTEST    m: " << thisjet->m() );
	  ATH_MSG_VERBOSE( "REGTEST    e: " << thisjet->e() );
	  ATH_MSG_VERBOSE( "REGTEST    px: " << thisjet->px() );
	  ATH_MSG_VERBOSE( "REGTEST    py: " << thisjet->py() );
	  ATH_MSG_VERBOSE( "REGTEST    pz: " << thisjet->pz() );
	  ATH_MSG_VERBOSE( "REGTEST    type: " << thisjet->type() );
	  ATH_MSG_VERBOSE( "REGTEST    algorithm (kt: 0, cam: 1, antikt: 2, ...): " << thisjet->getAlgorithmType() );
	  ATH_MSG_VERBOSE( "REGTEST    size parameter: " << thisjet->getSizeParameter() );
	  ATH_MSG_VERBOSE( "REGTEST    input (LCTopo: 0, EMTopo: 1, ...): " << thisjet->getInputType() );
	  ATH_MSG_VERBOSE( "REGTEST    constituents signal state (uncalibrated: 0, calibrated: 1): " << thisjet->getConstituentsSignalState() );
	  ATH_MSG_VERBOSE( "REGTEST    number of constituents: " << thisjet->numConstituents() );      
	}
	
        // for basic hists, don't cut eta/pt
        //come back to this - LS//if(m_doselOFBasicHists) if(!selectJet(jet)) continue;
	
        double e = (thisjet->e())/Gaudi::Units::GeV;
        double et = 0., epsilon = 1.e-3;
        if(thisjet->pt() > epsilon) et = (thisjet->pt())/Gaudi::Units::GeV;
        if(et < epsilon) et = 0;
        double eta = thisjet->eta();
        double phi = thisjet->phi();

	double  emfrac  =1;
	double  hecfrac =1;
	if (m_isPP || m_isCosmic || m_isMC){
	  emfrac  = thisjet->getAttribute<float>(xAOD::JetAttribute::EMFrac); 
	  hecfrac = thisjet->getAttribute<float>(xAOD::JetAttribute::HECFrac); 
	  ATH_MSG_VERBOSE( "REGTEST    emfrac: " << emfrac ); 
	  ATH_MSG_VERBOSE( "REGTEST    hecfrac: " << hecfrac ); 
	}

	double  jvt  = -0.1;
	double  sumpttrk500 = 0;
	if (thisjet->isAvailable<float>("Jvt")){
	  jvt = thisjet->getAttribute<float>("Jvt"); 
 	  sumpttrk500 = thisjet->getAttribute<std::vector<float> >("SumPtTrkPt500")[0]/Gaudi::Units::GeV;
	  ATH_MSG_VERBOSE( "REGTEST    JVT: " << jvt ); 
	  ATH_MSG_VERBOSE( "REGTEST    SumPtTrk500: " << sumpttrk500 ); 
	}

        if(msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG( lvl << " et =  " << et <<  "\teta = " << eta << "\temfrac = " << emfrac <<"\thecfrac");

        if((h  = hist(Form("%sJet_Et",lvl.c_str()))))           h->Fill(et,m_lumi_weight);
	if((h  = hist(Form("%sJet_HighEt",lvl.c_str()))))       h->Fill(et,m_lumi_weight);
        if((h  = hist(Form("%sJet_eta",lvl.c_str()))))          h->Fill(eta,m_lumi_weight);
        if((h  = hist(Form("%sJet_phi",lvl.c_str()))))          h->Fill(phi,m_lumi_weight);
        if((h  = hist(Form("%sJet_emfrac",lvl.c_str()))))       h->Fill(emfrac,m_lumi_weight);
	if((h  = hist(Form("%sJet_hecfrac",lvl.c_str()))))      h->Fill(hecfrac,m_lumi_weight);
        if((h  = hist(Form("%sJet_JVT",lvl.c_str()))))          h->Fill(jvt,m_lumi_weight);
	if((h  = hist(Form("%sJet_SumPtTrk500",lvl.c_str()))))  h->Fill(sumpttrk500,m_lumi_weight);
        if((h2 = hist2(Form("%sJet_phi_vs_eta",lvl.c_str()))))  h2->Fill(eta,phi,m_lumi_weight);
        if((h2 = hist2(Form("%sJet_E_vs_eta",lvl.c_str()))))    h2->Fill(eta,e,m_lumi_weight);
        if((h2 = hist2(Form("%sJet_E_vs_phi",lvl.c_str()))))    h2->Fill(phi,e,m_lumi_weight);
	
        // note: if this histogram turns out to be empty, it means:
        // emfraction is always 0 because the energies for different 
        // samplings are not filled at HLT
        if (emfrac > m_emfracCut) {
          if((h2 = hist2(Form("%sJet_phi_vs_eta_LAr",lvl.c_str()))))  h2->Fill(eta,phi,m_lumi_weight);
        }
        // restore signal state
        // done automatically by sigstateH ??
      } // end for thisjet
      
      if((h = hist(Form("%sJet_n",lvl.c_str()))))  h->Fill(n_HLTJet,m_lumi_weight);
    
    } // end if jetcoll
  } // end for k [HLT jet containers]

  // fill HLT per chain -- begin

  m_v_HLTjet.clear();
  m_v_HLTindex.clear();
  m_v_HLTindex.push_back(0);
  m_n_index=0;

  for(JetSigIter hltit = m_HLTChains.begin(); hltit != m_HLTChains.end(); ++hltit) {
    setCurrentMonGroup(m_monGroups[(*hltit).first]);
    ATH_MSG_DEBUG("Calling fillBasicHLTforChain(" << (*hltit).second << ", " << m_hltEtThres[(*hltit).second] << ")" );
    fillBasicHLTforChain( (*hltit).second, m_hltEtThres[(*hltit).second], m_hltContainers[(*hltit).second]);
  } // fill per chain -- end
 
  
  // fill offline jets in one loop
  // offline jets are in (0, 1, ..., N-1)th elements 

  m_v_OFjet.clear();
  m_v_OFindex.clear();
  m_n_index=0;
  m_v_OFindex.push_back(m_n_index);
  TLorentzVector v_thisjet;

  if(m_doOFJets) {
    ATH_MSG_DEBUG ("Filling OF Jets");
    unsigned int Nelem = m_OFJetC.size();
    for(unsigned int k = 0; k < Nelem; k++ ) {
      const xAOD::JetContainer *jetcoll = 0;
      jetcoll = m_OFJetC[k];
      if(jetcoll) {
	  std::string lvl = m_OFpfx; 
	  std::string mgrp = m_monGroups[Form("%s%d",m_OFpfx.c_str(),k)];
	  ATH_MSG_DEBUG( "level set to " << lvl <<  " and mongroup set to " << mgrp );
	  setCurrentMonGroup(mgrp);
          unsigned int n_OFJet = 0;

	    if((h  = hist(Form("%sSigma_vs_LB",lvl.c_str())))){      
	       h->GetXaxis()->SetRangeUser(*std::min_element(m_v_lbn.begin(),m_v_lbn.end())-1,*std::max_element(m_v_lbn.begin(),m_v_lbn.end())+1);
	       h->Fill(m_lumiBlock,m_lumi_weight);
	    }


	  for(const auto & thisjet : *jetcoll) {
            n_OFJet++;

            if(!thisjet) continue;

            // for basic hists, don't cut eta/pt
            //come back to this - LS//if(m_doselOFBasicHists) if(!selectJet(jet)) continue;

            double e = (thisjet->e())/Gaudi::Units::GeV;
            double et = 0., epsilon = 1.e-3;
            if(thisjet->pt() > epsilon) et = (thisjet->pt())/Gaudi::Units::GeV;
            if(et < epsilon) et = 0;
            ATH_MSG_DEBUG( lvl << " thisjet->pt() =  " << et );
            double  eta     = thisjet->eta();
            double  phi     = thisjet->phi();
	    double  emfrac  =1;
	    double  hecfrac =1;
	    if (m_isPP || m_isCosmic || m_isMC){
	      emfrac  = thisjet->getAttribute<float>(xAOD::JetAttribute::EMFrac); 
	      hecfrac = thisjet->getAttribute<float>(xAOD::JetAttribute::HECFrac); 
	    }

	    double  jvt  = -0.1;
	    double  sumpttrk500 = 0;
	    if (thisjet->isAvailable<float>("Jvt")){
	      jvt = thisjet->getAttribute<float>("Jvt"); 
	      sumpttrk500 = thisjet->getAttribute<std::vector<float> >("SumPtTrkPt500")[0]/Gaudi::Units::GeV;
	    }

            if(msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG( lvl << " et =  " << et <<  "\teta = " << eta << "\temfrac = " << emfrac <<"\thecfrac");


	    v_thisjet.SetPtEtaPhiE(thisjet->pt()/Gaudi::Units::GeV,eta,phi,e);
	    m_v_OFjet.push_back(v_thisjet);
	    m_n_index++;

            if((h  = hist(Form("%sJet_Et",lvl.c_str()))))           h->Fill(et,m_lumi_weight);
            if((h  = hist(Form("%sJet_eta",lvl.c_str()))))          h->Fill(eta,m_lumi_weight);
            if((h  = hist(Form("%sJet_phi",lvl.c_str()))))          h->Fill(phi,m_lumi_weight);
            if((h  = hist(Form("%sJet_emfrac",lvl.c_str()))))       h->Fill(emfrac,m_lumi_weight);
	    if((h  = hist(Form("%sJet_hecfrac",lvl.c_str()))))      h->Fill(hecfrac,m_lumi_weight);
	    if((h  = hist(Form("%sJet_JVT",lvl.c_str()))))          h->Fill(jvt,m_lumi_weight);
	    if((h  = hist(Form("%sJet_SumPtTrk500",lvl.c_str()))))  h->Fill(sumpttrk500,m_lumi_weight);
            if((h2 = hist2(Form("%sJet_phi_vs_eta",lvl.c_str()))))  h2->Fill(eta,phi,m_lumi_weight);
            if((h2 = hist2(Form("%sJet_E_vs_eta",lvl.c_str()))))    h2->Fill(eta,e,m_lumi_weight);
            if((h2 = hist2(Form("%sJet_E_vs_phi",lvl.c_str()))))    h2->Fill(phi,e,m_lumi_weight);

            // note: if this histogram turns out to be empty, it means:
            // emfraction is alwasy 0 because the energies for different 
            // samplings are not filled at HLT
            if (emfrac > m_emfracCut) {
              if((h2 = hist2(Form("%sJet_phi_vs_eta_LAr",lvl.c_str()))))  h2->Fill(eta,phi,m_lumi_weight);
            }
            // restore signal state
            // done automatically by sigstateH
	  } // end for thisjet
	  if((h = hist(Form("%sJet_n",lvl.c_str()))))  h->Fill(n_OFJet,m_lumi_weight);
	  setCurrentMonGroup(mgrp);
      } // if jetcoll
      m_v_OFindex.push_back(m_n_index);
    } // end for k
  } // if(m_doOFJets)

  return StatusCode::SUCCESS;

} // end fillBasicHists

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void HLTJetMonTool::fillBasicHLTforChain( const std::string& theChain, double thrHLT, const std::string& /*theContainer*/ ) {
  

  TH1 *h(0); 
  TH2 *h2(0); 

  TLorentzVector v_thisjet;

  double count=0;
  
  ATH_MSG_DEBUG("fillBasicHLTforChain CHAIN: " << Form("HLT_%s",theChain.c_str()) << " passed TDT: " << getTDT()->isPassed(Form("HLT_%s",theChain.c_str())));
  
  if (getTDT()->isPassed(Form("HLT_%s",theChain.c_str()))) {

    if((h  = hist("HLTSigma_vs_LB"))){
      h->Fill(m_lumiBlock,m_lumi_weight);
    }

    auto cg = getTDT()->getChainGroup(Form("HLT_%s",theChain.c_str())); //get features
    auto fc = cg->features();

     auto JetFeatureContainers = fc.get<xAOD::JetContainer>(); //get features container
     
     for(auto jcont : JetFeatureContainers) {

       // ATH_MSG_INFO("Loop Over Features");

       for (auto j : *jcont.cptr()) {

	 double e = (j->e())/Gaudi::Units::GeV;
	 double et = 0., epsilon = 1.e-3;

	 if(j->pt() > epsilon) et = (j->pt())/Gaudi::Units::GeV;

	 ATH_MSG_DEBUG("jet et = "<<et);


	 if(et < epsilon) et = 0;
	 bool hlt_thr_pass = ( et > thrHLT );
	 if(hlt_thr_pass) {
	   double eta     = j->eta();
	   double phi     = j->phi();
	   double  emfrac  =1;
	   double  hecfrac =1;
	   if (m_isPP || m_isCosmic || m_isMC){
	      emfrac  = j->getAttribute<float>(xAOD::JetAttribute::EMFrac); 
	      hecfrac = j->getAttribute<float>(xAOD::JetAttribute::HECFrac); 
	    }

	   v_thisjet.SetPtEtaPhiE(j->pt()/Gaudi::Units::GeV,j->eta(), j->phi(),j->e()/Gaudi::Units::GeV);
	   m_v_HLTjet.push_back(v_thisjet);
           m_n_index++;
		   
	   if((h  = hist("HLTJet_Et")))            h->Fill(et,      m_lumi_weight);
	   if((h  = hist("HLTJet_HighEt")))        h->Fill(et,      m_lumi_weight);
	   if((h  = hist("HLTJet_eta")))           h->Fill(eta,     m_lumi_weight);
	   if((h  = hist("HLTJet_phi")))           h->Fill(phi,     m_lumi_weight);
	   if((h  = hist("HLTJet_emfrac")))        h->Fill(emfrac,  m_lumi_weight);
	   if((h  = hist("HLTJet_hecfrac")))       h->Fill(hecfrac, m_lumi_weight);

	   if (count==0){
	     if((h  = hist("HLTJet_Leading_Et")))            h->Fill(et,      m_lumi_weight);
	   }
	   
	   if((h2 = hist2("HLTJet_phi_vs_eta")))   h2->Fill(eta,phi,m_lumi_weight);  
	   if((h2 = hist2("HLTJet_E_vs_eta")))     h2->Fill(eta,e,m_lumi_weight); 
	   if((h2 = hist2("HLTJet_E_vs_phi")))     h2->Fill(phi,e,m_lumi_weight); 
	   
	 }// if hlt threshold

	 count++;

       }// loop over jet container
     }// loop over features container

     if((h  = hist("HLTJet_n")))            h->Fill(count,      m_lumi_weight);
     
  }// if chain passed

   m_v_HLTindex.push_back(m_n_index);

}//EoF


// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void HLTJetMonTool::fillBasicL1forChain(const std::string& theChain, double thrEt ) {

  if( !m_L1JetRoIC ) return; // TEMPORARY - Should issue a warning  

  TH1 *h(0); 
  TH2 *h2(0);

  double count=0;

  TLorentzVector v_thisjet; //create a 4vector for the single jets in the event

  

  if (getTDT()->isPassed(theChain.c_str())){

    

    if((h  = hist("L1Sigma_vs_LB"))){
      //   h->GetXaxis()->SetRangeUser(*std::min_element(m_v_lbn.begin(),m_v_lbn.end())-1,*std::max_element(m_v_lbn.begin(),m_v_lbn.end())+1);
      h->Fill(m_lumiBlock,m_lumi_weight);
    }
    
    Trig::FeatureContainer chainFeatures = getTDT()->features(theChain.c_str());
    for(Trig::FeatureContainer::combination_const_iterator chIt = chainFeatures.getCombinations().begin(); 
        chIt != chainFeatures.getCombinations().end(); ++chIt ) {
      
      ATH_MSG_DEBUG("loop features");

      std::vector< Trig::Feature<TrigRoiDescriptor> > theRoIdesc = chIt->get<TrigRoiDescriptor>("initialRoI");
      for(std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator combIt = theRoIdesc.begin(); 
          combIt!= theRoIdesc.end(); ++combIt){

	ATH_MSG_DEBUG("loop roidesc");	

        if(!(combIt->cptr())) continue;
        if((h = hist("L1Jet_roidesc_eta")))  h->Fill(combIt->cptr()->eta(),m_lumi_weight);
        if((h = hist("L1Jet_roidesc_phi")))  h->Fill(combIt->cptr()->phi(),m_lumi_weight);
        if((h2 = hist2("L1Jet_roidesc_phi_vs_eta")))  h2->Fill(combIt->cptr()->eta(), combIt->cptr()->phi(),m_lumi_weight);
	
        unsigned int id =   combIt->cptr()->roiWord();
	//  bool id_match_found = false;
        xAOD::JetRoIContainer::const_iterator it_L1 = m_L1JetRoIC->begin();
        xAOD::JetRoIContainer::const_iterator it_e_L1 = m_L1JetRoIC->end();
	
        for (; it_L1 != it_e_L1; ++it_L1) {

          double et = ((*it_L1)->et4x4())/Gaudi::Units::GeV;
	  if(et < 1.e-3) et = 0;
          double eta = (*it_L1)->eta();
          double phi = (*it_L1)->phi();
	  double ene = et * cosh(eta);


          if(msgLvl(MSG::DEBUG)) {
            const Jet_ROI::thresholds_type thrVec = (*it_L1)->thrNames();
            const Jet_ROI::thr_value_type thrVal = (*it_L1)->thrValues();
            double et6 = ((*it_L1)->et6x6())/Gaudi::Units::GeV;
            double et8 = ((*it_L1)->et8x8())/Gaudi::Units::GeV;
            std::ostringstream thrv; 
            unsigned int kk = 0;
            for(Jet_ROI::thresholds_type::const_iterator l1i = thrVec.begin(); l1i != thrVec.end(); ++l1i,kk++ ) {
              thrv << *l1i << " " << thrVal[kk] << ", " ;
            }
            ATH_MSG_DEBUG( "CHAIN: " << theChain << " " << et << " GeV " << eta << " " 
			   << phi << " rad " << et6 << " " << et8 << " " << thrv.str());
          }
          if(id == (*it_L1)->roiWord() ) {
	    // id_match_found = true;
            bool l1_thr_pass = (et > thrEt);
            ATH_MSG_DEBUG("CHAIN: " << theChain << " " << et << "\tthreshold = " << thrEt << "\tpass = " << l1_thr_pass);
            if(l1_thr_pass) {
	      
	      v_thisjet.SetPtEtaPhiE(et,eta,phi,ene);
	      m_v_L1jet.push_back(v_thisjet);
	      m_n_index++;
	      count++;

              if((h  = hist("L1Jet_Et")))           h->Fill(et,m_lumi_weight);
              if((h  = hist("L1Jet_eta")))          h->Fill(eta,m_lumi_weight);
              if((h  = hist("L1Jet_phi")))          h->Fill(phi,m_lumi_weight); 
              if((h2 = hist2("L1Jet_phi_vs_eta")))  h2->Fill(eta,phi,m_lumi_weight);
	      if((h2 = hist2("L1Jet_E_vs_eta")))    h2->Fill(eta,ene,m_lumi_weight);
	      if((h2 = hist2("L1Jet_E_vs_phi")))    h2->Fill(phi,ene,m_lumi_weight);
            }
          }
        }
	// double unmatched_eta = id_match_found ? -999. : combIt->cptr()->eta();
	// if((h = hist("L1Jet_unmatched_eta")))  h->Fill(unmatched_eta,m_lumi_weight);
      } // for combIt
    } // l2It

    if((h  = hist("L1Jet_n")))           h->Fill(count,m_lumi_weight);
    
  } // L2 chain isPassed

  m_v_L1index.push_back(m_n_index);

} //EoF
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//:::::::::::::::::::::::::::::::::FILL DIJET HISTS::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

StatusCode HLTJetMonTool::fillDiJetHists() {

  TH2* h2(0);

  int count=0;
  int count1=0;
  int count2=0;
  int count3=0;
  int count4=0;
  int count5=0;
  int index_lead=0;
  double ptlead=-999;
  double ptsublead=-999;
  bool l1lead=false;
  bool l1sublead=false;
  bool oflead=false;
  bool ofsublead=false;
  bool hltlead=false;
  bool hltsublead=false;
  TLorentzVector v_jet;
  TLorentzVector v_leadjet;
  TLorentzVector v_subleadjet;
  TLorentzVector v_ofleadjet;
  TLorentzVector v_ofsubleadjet;
  TLorentzVector v_hltleadjet;
  TLorentzVector v_hltsubleadjet;


  //Fill L1 Correlation


 //:::::::::::::::::::::::SELECT L1 JETS:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

 
 for(JetSigIter l1 = m_basicL1Trig.begin(); l1 != m_basicL1Trig.end(); ++l1) { 
   std::string theChain=(*l1).second;  
   ptlead=-999;
   ptsublead=-999;
   index_lead=0;
   l1lead=false;
   l1sublead=false;
   ATH_MSG_DEBUG("for "<<m_v_L1index[count]<<" to "<<m_v_L1index[count+1]);
   
   for (int i=m_v_L1index[count]; i<m_v_L1index[count+1]; ++i) {
     v_jet=m_v_L1jet[i];
     ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
     
     if (v_jet.Pt() >= ptlead){ //select leading
       v_leadjet=v_jet;
       ptlead=v_jet.Pt();
       index_lead=i;
       l1lead=true;
       ATH_MSG_DEBUG("is leading");
     }
   }
   
   count++;
   
   for (int i=m_v_L1index[count1]; i<m_v_L1index[count1+1]; ++i) { //select subleading
     v_jet=m_v_L1jet[i];
     ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
     
     if (v_jet.Pt()>=ptsublead && i != index_lead){
       v_subleadjet=v_jet;
       ptsublead=v_jet.Pt();
       l1sublead=true;
       ATH_MSG_DEBUG("is subleading");
     }
   }
   
   count1++;
   
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   
     
   //::::::::::::::::::FILL L1 vs L1 HISTO::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   
   
    for (unsigned int k=0; k < m_l1_DijetChains.size(); ++k){
      
      ATH_MSG_DEBUG("L1 Chain "<<theChain.c_str()<<" Dijet Chain "<<m_l1_DijetChains[k].c_str());
      
      if (theChain == m_l1_DijetChains[k]){
	ATH_MSG_DEBUG("pass");
	setCurrentMonGroup(m_monGroups[(*l1).first]);
	
	if (l1lead && l1sublead){
	  if((h2 = hist2(Form("%s_leadEt_vs_%s_subleadEt",theChain.c_str(),theChain.c_str())))) h2->Fill(v_leadjet.Et(),v_subleadjet.Et(),m_lumi_weight);
	  
	}
      }   
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



    //:::::::::::::::::::::::SELECT OF JETS::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    
    count2=0;
    count3=0;

    
    for(JetSigIter ofj= m_OFJetKeys.begin(); ofj != m_OFJetKeys.end(); ++ofj) {
      ptlead=-999;
      ptsublead=-999;
      index_lead=0;
      oflead=false;
      ofsublead=false;	
      ATH_MSG_DEBUG("for "<<m_v_OFindex[count]<<" to "<<m_v_OFindex[count+1]);
      
      for (int i=m_v_OFindex[count2]; i<m_v_OFindex[count2+1]; ++i) {
	v_jet=m_v_OFjet[i];
	ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
	
	if (v_jet.Pt() >= ptlead){ //select leading
	  v_ofleadjet=v_jet;
	  ptlead=v_jet.Pt();
	  index_lead=i;
	  oflead=true;
	  ATH_MSG_DEBUG("is leading");
	}
      }
      
      count2++;
      
      for (int i=m_v_OFindex[count3]; i<m_v_OFindex[count3+1]; ++i) { //select subleading
	v_jet=m_v_OFjet[i];
	ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
	
	if (v_jet.Pt()>=ptsublead && i != index_lead){
	  v_ofsubleadjet=v_jet;
	  ptsublead=v_jet.Pt();
	  ofsublead=true;
	  ATH_MSG_DEBUG("is subleading");
	}
      }

      //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      
      count3++;
    
      //::::::::::::::::::FILL L1 vs OF HISTO::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


    for (unsigned int k=0; k < m_l1_DijetChains.size(); ++k){      
      for (unsigned int j=0; j < m_of_DijetChains.size(); ++j){
	ATH_MSG_DEBUG("L1 Chain "<<theChain.c_str()<<" Dijet Chain "<<m_l1_DijetChains[k].c_str());
      
	if (theChain == m_l1_DijetChains[k] && (*ofj).second == m_of_DijetChains[j]){
	  ATH_MSG_DEBUG("pass");
	  setCurrentMonGroup(m_monGroups[(*l1).first]);
	  
	  if (l1lead && l1sublead && oflead && ofsublead){
	    if((h2 = hist2(Form("%s_subleadEt_vs_%s_subleadEt",theChain.c_str(),((*ofj).second).c_str()))))    h2->Fill(v_subleadjet.Et(),v_ofsubleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_leadEt_vs_%s_leadEt",theChain.c_str(),((*ofj).second).c_str()))))          h2->Fill(v_leadjet.Et(),v_ofleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_leadEt_vs_%s_subleadEt",theChain.c_str(),((*ofj).second).c_str()))))       h2->Fill(v_leadjet.Et(),v_ofsubleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_subleadEt_vs_%s_leadEt",theChain.c_str(),((*ofj).second).c_str()))))       h2->Fill(v_subleadjet.Et(),v_ofleadjet.Et(),m_lumi_weight);	    
	  }
	}
      } 
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    } //end OF JetCol loop
    
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    
       

    //:::::::::::::::::::::::SELECT HLT JETS::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    
    count4=0;
    count5=0;

    
    for(JetSigIter hj = m_basicHLTTrig.begin(); hj != m_basicHLTTrig.end(); ++hj) {
      ptlead=-999;
      ptsublead=-999;
      index_lead=0;
      hltlead=false;
      hltsublead=false;	
      ATH_MSG_DEBUG("for "<<m_v_HLTindex[count4]<<" to "<<m_v_HLTindex[count4+1]);
      
      for (int i=m_v_HLTindex[count4]; i<m_v_HLTindex[count4+1]; ++i) {
	v_jet=m_v_HLTjet[i];
	ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
	
	if (v_jet.Pt() >= ptlead){ //select leading
	  v_hltleadjet=v_jet;
	  ptlead=v_jet.Pt();
	  index_lead=i;
	  hltlead=true;
	  ATH_MSG_DEBUG("is leading");
	}
      }
      
      count4++;
      
      for (int i=m_v_HLTindex[count5]; i<m_v_HLTindex[count5+1]; ++i) { //select subleading
	v_jet=m_v_HLTjet[i];
	ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
	
	if (v_jet.Pt()>=ptsublead && i != index_lead){
	  v_hltsubleadjet=v_jet;
	  ptsublead=v_jet.Pt();
	  hltsublead=true;
	  ATH_MSG_DEBUG("is subleading");
	}
      }

      //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      
      count5++;
    
      //::::::::::::::::::FILL L1 vs HLT HISTO::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


    for (unsigned int k=0; k < m_l1_DijetChains.size(); ++k){      
      for (unsigned int j=0; j < m_hlt_DijetChains.size(); ++j){
	ATH_MSG_DEBUG("L1 Chain "<<theChain.c_str()<<" Dijet Chain "<<m_l1_DijetChains[k].c_str());
      
	if (theChain == m_l1_DijetChains[k] && (*hj).first == m_hlt_DijetChains[j]){
	  ATH_MSG_DEBUG("pass");
	  setCurrentMonGroup(m_monGroups[(*l1).first]);
	  
	  if (l1lead && l1sublead && hltlead && hltsublead){
	    if((h2 = hist2(Form("%s_subleadEt_vs_%s_subleadEt",theChain.c_str(),((*hj).first).c_str()))))    h2->Fill(v_subleadjet.Et(),v_hltsubleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_leadEt_vs_%s_leadEt",theChain.c_str(),((*hj).first).c_str()))))          h2->Fill(v_leadjet.Et(),v_hltleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_leadEt_vs_%s_subleadEt",theChain.c_str(),((*hj).first).c_str()))))       h2->Fill(v_leadjet.Et(),v_hltsubleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_subleadEt_vs_%s_leadEt",theChain.c_str(),((*hj).first).c_str()))))       h2->Fill(v_subleadjet.Et(),v_hltleadjet.Et(),m_lumi_weight);	    
	  }
	}
      } 
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    } //end HLT JetCol loop
    
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 

 } // for L1 Chain
 
 //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



 //Now Fill HLT correlation


 //:::::::::::::::::::::::SELECT HLT JETS:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

 count=0;
 count1=0;
 
 for(JetSigIter hj = m_basicHLTTrig.begin(); hj != m_basicHLTTrig.end(); ++hj) { 
   std::string theChain=(*hj).second;  
   ptlead=-999;
   ptsublead=-999;
   index_lead=0;
   hltlead=false;
   hltsublead=false;
   ATH_MSG_DEBUG("for "<<m_v_HLTindex[count]<<" to "<<m_v_HLTindex[count+1]);
   
   for (int i=m_v_HLTindex[count]; i<m_v_HLTindex[count+1]; ++i) {
     v_jet=m_v_HLTjet[i];
     ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
     
     if (v_jet.Pt() >= ptlead){ //select leading
       v_leadjet=v_jet;
       ptlead=v_jet.Pt();
       index_lead=i;
       hltlead=true;
       ATH_MSG_DEBUG("is leading");
     }
   }
   
   count++;
   
   for (int i=m_v_HLTindex[count1]; i<m_v_HLTindex[count1+1]; ++i) { //select subleading
     v_jet=m_v_HLTjet[i];
     ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
     
     if (v_jet.Pt()>=ptsublead && i != index_lead){
       v_subleadjet=v_jet;
       ptsublead=v_jet.Pt();
       hltsublead=true;
       ATH_MSG_DEBUG("is subleading");
     }
   }
   
   count1++;
   
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   
     
   //::::::::::::::::::FILL HLT vs HLT HISTO::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   
   
    for (unsigned int k=0; k < m_hlt_DijetChains.size(); ++k){
      
      ATH_MSG_DEBUG("HLT Chain "<<theChain.c_str()<<" Dijet Chain "<<m_hlt_DijetChains[k].c_str());
      
      if (theChain == m_hlt_DijetChains[k]){
	ATH_MSG_DEBUG("pass");
	setCurrentMonGroup(m_monGroups[(*hj).first]);
	
	if (hltlead && hltsublead){
	  if((h2 = hist2(Form("%s_leadEt_vs_%s_subleadEt",theChain.c_str(),theChain.c_str())))) h2->Fill(v_leadjet.Et(),v_subleadjet.Et(),m_lumi_weight);
	  
	}
      }   
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



    //:::::::::::::::::::::::SELECT OF JETS::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    
    count2=0;
    count3=0;

    
    for(JetSigIter ofj= m_OFJetKeys.begin(); ofj != m_OFJetKeys.end(); ++ofj) {
      ptlead=-999;
      ptsublead=-999;
      index_lead=0;
      oflead=false;
      ofsublead=false;	
      ATH_MSG_DEBUG("for "<<m_v_OFindex[count]<<" to "<<m_v_OFindex[count+1]);
      
      for (int i=m_v_OFindex[count2]; i<m_v_OFindex[count2+1]; ++i) {
	v_jet=m_v_OFjet[i];
	ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
	
	if (v_jet.Pt() >= ptlead){ //select leading
	  v_ofleadjet=v_jet;
	  ptlead=v_jet.Pt();
	  index_lead=i;
	  oflead=true;
	  ATH_MSG_DEBUG("is leading");
	}
      }
      
      count2++;
      
      for (int i=m_v_OFindex[count3]; i<m_v_OFindex[count3+1]; ++i) { //select subleading
	v_jet=m_v_OFjet[i];
	ATH_MSG_DEBUG("jet n "<<i+1<<" pt = "<<v_jet.Pt());
	
	if (v_jet.Pt()>=ptsublead && i != index_lead){
	  v_ofsubleadjet=v_jet;
	  ptsublead=v_jet.Pt();
	  ofsublead=true;
	  ATH_MSG_DEBUG("is subleading");
	}
      }

      //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      
      count3++;
    
      //::::::::::::::::::FILL HLT vs OF HISTO::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


    for (unsigned int k=0; k < m_hlt_DijetChains.size(); ++k){      
      for (unsigned int j=0; j < m_of_DijetChains.size(); ++j){
	ATH_MSG_DEBUG("HLT Chain "<<theChain.c_str()<<" Dijet Chain "<<m_hlt_DijetChains[k].c_str());
      
	if (theChain == m_hlt_DijetChains[k] && (*ofj).second == m_of_DijetChains[j]){
	  ATH_MSG_DEBUG("pass");
	  setCurrentMonGroup(m_monGroups[(*hj).first]);
	  
	  if (hltlead && hltsublead && oflead && ofsublead){
	    if((h2 = hist2(Form("%s_subleadEt_vs_%s_subleadEt",theChain.c_str(),((*ofj).second).c_str()))))    h2->Fill(v_subleadjet.Et(),v_ofsubleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_leadEt_vs_%s_leadEt",theChain.c_str(),((*ofj).second).c_str()))))          h2->Fill(v_leadjet.Et(),v_ofleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_leadEt_vs_%s_subleadEt",theChain.c_str(),((*ofj).second).c_str()))))       h2->Fill(v_leadjet.Et(),v_ofsubleadjet.Et(),m_lumi_weight);
	    if((h2 = hist2(Form("%s_subleadEt_vs_%s_leadEt",theChain.c_str(),((*ofj).second).c_str()))))       h2->Fill(v_subleadjet.Et(),v_ofleadjet.Et(),m_lumi_weight);	    
	  }
	}
      } 
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    } //end OF JetCol loop
 } // end HLT loop

 
 return StatusCode::SUCCESS;
 
} //EoF

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


bool HLTJetMonTool::isChainActive(const std::string& theChain ) {
  bool isactive = true;
  if( std::abs( getTDT()->getPrescale( theChain ) + 1.0 ) < 0.001 )
    //if( std::abs( m_tdt->getPrescale( theChain ) + 1.0 ) < 0.001 )
    isactive = false;
  return isactive;

} // end isChainActive

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#if 0
bool HLTJetMonTool::selectJet(const Jet *jet) {

  //if(!m_doselOFJets) return true; 

  // pt / eta
  // don't cut on eta here since we have FJ signatures to contend with:
  // if(m_reqEtaCut)       if (fabs(jet->eta()) >= m_EtaCut )              return false;
  if(m_reqMinPtCut)     if (jet->pt()/Gaudi::Units::GeV <= m_MinPtCut )               return false;


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
#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::..

bool HLTJetMonTool::isBadJet(const std::string &Category, double emf, double hecf, double larq, double hecq, double time,
    double sumpttrk, double eta, double pt, double fmax, double negE) {

  double chf=sumpttrk/pt;

  if(fabs(time)>10)                                return true;
  if(emf<0.05 && chf<0.05 && fabs(eta)<2)          return true;
  if(emf<0.05 && fabs(eta)>=2)                     return true;
  if(fmax>0.99 && fabs(eta)<2)                     return true;

  if(Category=="MediumBad" || Category=="TightBad")  {
    if(emf<0.05 && chf<0.1  && fabs(eta)<2)        return true;  
    if(emf>0.95 && chf<0.05 && fabs(eta)<2)        return true;
  }

  if(Category=="TightBad")  {
    if(emf<0.1 && chf<0.2  && fabs(eta)<2)         return true;
    if(emf<0.1 && fabs(eta)>=2 )                   return true;    
    if(emf>0.9 && chf<0.02 && fabs(eta)<2)         return true;
  }

  if(fabs(negE/1000.)>60)                          return true;
  if(hecf>0.5 && fabs(hecq)>0.5)                   return true;  

  if(Category=="MediumBad" || Category=="TightBad") {
    if(hecf>1-fabs(hecq))                          return true;
  }

  if(emf>0.95 && fabs(larq)>0.8 && fabs(eta)<2.8)  return true;

  if(Category=="MediumBad" || Category=="TightBad") {
    if(emf>0.9 && fabs(larq)>0.8 && fabs(eta)<2.8) return true;
  }

  if(Category=="TightBad") {
    if(fabs(larq)>0.95)                            return true;
    if(emf>0.98 && fabs(larq)>0.05)                return true;
  }

  return false;

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

StatusCode HLTJetMonTool::fillOfflineHists() {

  ATH_MSG_DEBUG("Filling Efficiency Plots");

  TH1      *h(0);
  TH2      *h2(0);
  TProfile *p(0);

  // fill offline jets in one loop
  unsigned int Nelem = m_OFJetC.size();
  for(unsigned int k = 0; k < Nelem; k++ ) {
    const xAOD::JetContainer *jetcoll = m_OFJetC[k];
    if(jetcoll) {
      std::string malg = Form("%s%d",m_OFpfx.c_str(),k);
      std::string mgrp = m_monGroups[malg];
      std::string mgrp_eff = m_monGroups[Form("%s%dEff",m_OFpfx.c_str(),k)];
     
      ATH_MSG_DEBUG("Offline Jet Collection : "<<mgrp);
      
      std::vector<std::string> mFoundL1, mFoundHLT, mUnmatchedL1, mUnmatchedHLT ; 
      std::vector<TLorentzVector> mFoundL1Jets, mFoundHLTJets,mUnmatchedL1Jets, mUnmatchedHLTJets;
      std::vector<std::string>::iterator mIt;
      JetSigIter lIt;

      TLorentzVector v_trigjet_tmp;

      for(const auto & jet : *jetcoll) {

	ATH_MSG_DEBUG("Offline Jet Collection Loop");

	mFoundL1.clear();
	mFoundHLT.clear();
	mFoundL1Jets.clear();
	mFoundHLTJets.clear();

	mUnmatchedL1.clear();
	mUnmatchedHLT.clear();
	mUnmatchedL1Jets.clear();
	mUnmatchedHLTJets.clear();


        bool l1pass  = false;
        bool hltpass = false;

	// bool passL1thr, passHLTthr;

	int count=0;

        // set mongroup to fill efficiency histograms
	
	setCurrentMonGroup(mgrp_eff);


	//:::::::::::::::::::::::::: OF vs L1 ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


	ATH_MSG_DEBUG("Filling Denominator Histograms for L1");
	
	for(lIt = m_L1Items.begin(); lIt != m_L1Items.end(); ++lIt) {
	  std::string itemName = (*lIt).first;          
	  double thrGeVItem = m_ofEtThres[itemName];
	  double EtaHighThres=m_l1EtaHighThres[(*lIt).second];
	  double EtaLowThres =m_l1EtaLowThres[(*lIt).second];

	  //passL1thr = (jet->pt()/CLHEP::GeV > thrGeVItem);
	  
	  ATH_MSG_DEBUG(" L1 Chain= "<<itemName<<" OF Jet pt= "<<jet->pt()/Gaudi::Units::GeV<<" OF Jet eta= "<<jet->eta()<<" Threshold= "<<thrGeVItem<<" TDT= "<<getTDT()->isPassed(itemName)<<" Eta Low "<<EtaLowThres<<" Eta High "<<EtaHighThres);
	  
	  // if L1 eff required
	  if(m_doL1TrigEff) {
	    if (isLeadingJet(jet,jetcoll,EtaLowThres,EtaHighThres,1)){ //OF leading jet in the same eta region of HLTJet
	      
	      ATH_MSG_DEBUG(" OF Jet passed requirements->Filling the denominator");
	      
	      if((h = hist(Form("%s_Eff_vs_pt_den",itemName.c_str()))))          h->Fill(jet->pt()/Gaudi::Units::GeV);
	      if((h = hist(Form("%s_Eff_vs_eta_den",itemName.c_str()))))         h->Fill(jet->eta());
	      if((h = hist(Form("%s_Eff_vs_phi_den",itemName.c_str()))))         h->Fill(jet->phi());
	      if((h2 = hist2(Form("%s_Eff_vs_pt_vs_eta_den",itemName.c_str())))) h2->Fill(jet->pt()/Gaudi::Units::GeV,jet->eta());
	      if((h2 = hist2(Form("%s_Eff_vs_pt_vs_phi_den",itemName.c_str())))) h2->Fill(jet->pt()/Gaudi::Units::GeV,jet->phi());
	      
	    }  
	  } //if trigeff
	}//Loop over Chain
	
	ATH_MSG_DEBUG("Filling Numerator Histograms for L1");
	
	  // if any L1 items passed, fill numerator
	if((l1pass = passedChainTest(jet,mFoundL1,mFoundL1Jets,mUnmatchedL1,mUnmatchedL1Jets,"L1"))) {
	  
          for(mIt = mFoundL1.begin(); mIt != mFoundL1.end(); ++mIt) {
	    
	    v_trigjet_tmp=mFoundL1Jets[count];
            std::string itemName = (*mIt).c_str();
	    double EtaHighThres=m_l1EtaHighThres[(*mIt).c_str()];
	    double EtaLowThres =m_l1EtaLowThres[(*mIt).c_str()];
	    // fill num L1 eff
	    
	    
	    ATH_MSG_DEBUG("Chain Matched= "<<itemName<<" TDT= "<<getTDT()->isPassed(itemName));
	    
            if(m_doL1TrigEff) {
	      if (isLeadingJet(jet,jetcoll,EtaLowThres,EtaHighThres,1)){ //OF leading jet in the same eta region of HLTJet
		if(getTDT()->isPassed(itemName)) {
		  
		  ATH_MSG_DEBUG(" OF Jet passed requirements->Filling the numerator");	 	
		  
		  if((h = hist(Form("%s_Eff_vs_pt_num",itemName.c_str()))))          h->Fill(jet->pt()/Gaudi::Units::GeV); //fill numerator histogram		 
		  if((h = hist(Form("%s_Eff_vs_eta_num",itemName.c_str()))))         h->Fill(jet->eta());
		  if((h = hist(Form("%s_Eff_vs_phi_num",itemName.c_str()))))         h->Fill(jet->phi());
		  if((h2 = hist2(Form("%s_Eff_vs_pt_vs_eta_num",itemName.c_str())))) h2->Fill(jet->pt()/Gaudi::Units::GeV,jet->eta());
		  if((h2 = hist2(Form("%s_Eff_vs_pt_vs_phi_num",itemName.c_str())))) h2->Fill(jet->pt()/Gaudi::Units::GeV,jet->phi());
		  if((h2 = hist2(Form("%s_pt_vs_OF_pt",itemName.c_str()))))          h2->Fill(jet->pt()/Gaudi::Units::GeV,v_trigjet_tmp.Pt()/Gaudi::Units::GeV);

		  if((p = profile(Form("%s_EffPt",itemName.c_str()))))  p->Fill(jet->pt()/Gaudi::Units::GeV,1.0,1.0); //fill profile with 1
		  if((p = profile(Form("%s_EffEta",itemName.c_str())))) p->Fill(jet->eta(),1.0,1.0);
		  if((p = profile(Form("%s_EffPhi",itemName.c_str())))) p->Fill(jet->phi(),1.0,1.0);
		  
		  double res=(v_trigjet_tmp.Pt()-jet->pt())/(jet->pt());
		  if((h = hist(Form("%s_Resolution_pt",itemName.c_str())))) h->Fill(res);
		  
		} if (!getTDT()->isPassed(itemName)){
		  
		  if((p = profile(Form("%s_EffPt",itemName.c_str()))))  p->Fill(jet->pt()/Gaudi::Units::GeV,0.0,1.0); //fill profile with 0
		  if((p = profile(Form("%s_EffEta",itemName.c_str())))) p->Fill(jet->eta(),0.0,1.0);
		  if((p = profile(Form("%s_EffPhi",itemName.c_str())))) p->Fill(jet->phi(),0.0,1.0);

		}
	      } // if this L1 item passed
	    } // if trigeff  
	    
	    count=count+1;
	    
	  } // for found L1 items
	} // if any L1 items passed
 

	count=0;

	for(mIt = mUnmatchedL1.begin(); mIt != mUnmatchedL1.end(); ++mIt) { //Filling Unmatched Jets
	  
	  v_trigjet_tmp=mUnmatchedL1Jets[count];
	  std::string itemName = (*mIt).c_str();
	  
	  ATH_MSG_DEBUG("Chain Unmatched= "<<itemName<<" TDT= "<<getTDT()->isPassed(itemName));
	  	 
	  if(m_doL1TrigEff) {
	    if(getTDT()->isPassed(itemName) /*&& passHLTthr*/) {
	      
	      if((h = hist(Form("%s_unmatched_pt",itemName.c_str())))) h->Fill(v_trigjet_tmp.Pt()/Gaudi::Units::GeV);   
	          
	    }//TDT
	  } // doTrigEff
	  
	  count=count+1;
	}//loop over unmatched jets


	  count=0;

	//::::::::::::::::::::::::::: OF vs HLT ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	  ATH_MSG_DEBUG("Filling Denominator Histograms for HLT");

	  for(lIt = m_HLTChains.begin(); lIt != m_HLTChains.end(); ++lIt) {
          std::string chainName = (*lIt).first;
	  double EtaHighThres   = m_hltEtaHighThres[(*lIt).second];
	  double EtaLowThres    = m_hltEtaLowThres[(*lIt).second];
	  int NJet              = m_hltJetn[(*lIt).second];

          //passHLTthr = (jet->pt()/CLHEP::GeV > thrGeVItem)
	  

          // if HLT eff required
          if(m_doHLTTrigEff) {
	    if (isLeadingJet(jet,jetcoll,EtaLowThres,EtaHighThres,NJet)){ //OF nth leading jet in the same eta region of HLTJet	    
	    
	      if((h = hist(Form("%s_Eff_vs_pt_den",chainName.c_str()))))          h->Fill(jet->pt()/Gaudi::Units::GeV);
	      if((h = hist(Form("%s_Eff_vs_eta_den",chainName.c_str()))))         h->Fill(jet->eta());
	      if((h = hist(Form("%s_Eff_vs_phi_den",chainName.c_str()))))         h->Fill(jet->phi());
	      if((h2 = hist2(Form("%s_Eff_vs_pt_vs_eta_den",chainName.c_str())))) h2->Fill(jet->pt()/Gaudi::Units::GeV,jet->eta());
	      if((h2 = hist2(Form("%s_Eff_vs_pt_vs_phi_den",chainName.c_str())))) h2->Fill(jet->pt()/Gaudi::Units::GeV,jet->phi());
	    
	    } //isLeading
          }//doTrigEff
      
        }//Loop over Chain

	ATH_MSG_DEBUG("Filling Numerator Histograms for HLT");

        // if any HLT chains passed, fill numerator
        if((hltpass = passedChainTest(jet,mFoundHLT,mFoundHLTJets,mUnmatchedHLT,mUnmatchedHLTJets,"HLT"))) {
         
          for(mIt = mFoundHLT.begin(); mIt != mFoundHLT.end(); ++mIt) {
	    
	    v_trigjet_tmp=mFoundHLTJets[count];
	    std::string chainName = (*mIt).c_str();
	    double EtaHighThres   = m_hltEtaHighThres[(*mIt).c_str()];
	    double EtaLowThres    = m_hltEtaLowThres[(*mIt).c_str()];
	    int NJet              = m_hltJetn[(*mIt).c_str()];
	  
            // fill num HLT eff
            if(m_doHLTTrigEff) {
	      if (isLeadingJet(jet,jetcoll,EtaLowThres,EtaHighThres,NJet)){ //OF leading jet in the same eta region of HLTJet	  
		if(getTDT()->isPassed(Form("HLT_%s",chainName.c_str()))) {
		  
		  if((h = hist(Form("%s_Eff_vs_pt_num",chainName.c_str()))))          h->Fill(jet->pt()/Gaudi::Units::GeV);
		  if((h = hist(Form("%s_Eff_vs_eta_num",chainName.c_str()))))         h->Fill(jet->eta());
		  if((h = hist(Form("%s_Eff_vs_phi_num",chainName.c_str()))))         h->Fill(jet->phi());
		  if((h2 = hist2(Form("%s_Eff_vs_pt_vs_eta_num",chainName.c_str())))) h2->Fill(jet->pt()/Gaudi::Units::GeV,jet->eta());
		  if((h2 = hist2(Form("%s_Eff_vs_pt_vs_phi_num",chainName.c_str())))) h2->Fill(jet->pt()/Gaudi::Units::GeV,jet->phi());
		  if((h2 = hist2(Form("%s_pt_vs_OF_pt",chainName.c_str()))))          h2->Fill(jet->pt()/Gaudi::Units::GeV,v_trigjet_tmp.Pt()/Gaudi::Units::GeV);

		  if((p = profile(Form("%s_EffPt",chainName.c_str()))))  p->Fill(jet->pt()/Gaudi::Units::GeV,1.0,1.0); //fill profile with 1
		  if((p = profile(Form("%s_EffEta",chainName.c_str())))) p->Fill(jet->eta(),1.0,1.0);
		  if((p = profile(Form("%s_EffPhi",chainName.c_str())))) p->Fill(jet->phi(),1.0,1.0);
		  
		  double res=(v_trigjet_tmp.Pt()-jet->pt())/(jet->pt());
		  if((h = hist(Form("%s_Resolution_pt",chainName.c_str())))) h->Fill(res);
		  
		} if (! getTDT()->isPassed(Form("HLT_%s",chainName.c_str()))){
		  if((p = profile(Form("%s_EffPt",chainName.c_str()))))  p->Fill(jet->pt()/Gaudi::Units::GeV,0.0,1.0); //fill profile with 0
		  if((p = profile(Form("%s_EffEta",chainName.c_str())))) p->Fill(jet->eta(),0.0,1.0);
		  if((p = profile(Form("%s_EffPhi",chainName.c_str())))) p->Fill(jet->phi(),0.0,1.0);
		}	 
              }//TDT
            } // doTrigEff
	 
	    count=count+1;

          } // Loop over Chain
        } // if any HLT chains passed

	count=0;

	for(mIt = mUnmatchedHLT.begin(); mIt != mUnmatchedHLT.end(); ++mIt) { //Filling Unmatched Jets
	  
	  v_trigjet_tmp=mUnmatchedHLTJets[count];
	  std::string chainName = (*mIt).c_str();
	  
	  ATH_MSG_DEBUG("Chain Unmatched= "<<chainName<<" TDT= "<<getTDT()->isPassed(Form("HLT_%s",chainName.c_str())));
	  
	  // fill num HLT eff
	  if(m_doHLTTrigEff) {
	    if(getTDT()->isPassed(Form("HLT_%s",chainName.c_str())) /*&& passHLTthr*/) {
	      	      
	      if((h = hist(Form("%s_unmatched_pt",chainName.c_str())))) h->Fill(v_trigjet_tmp.Pt()/Gaudi::Units::GeV);  
	      
	    }//TDT
	  } // doTrigEff
	
	  count=count+1;
	}//loop over unmatched jets


	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	
      } // for loop over OF jet
    } // if jetcoll
  } // for k (loop over all ref coll's)

  return StatusCode::SUCCESS;

} // end fillOfflineHists

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool HLTJetMonTool::passedChainTest( const xAOD::Jet *jet, std::vector<std::string>& mFound,  std::vector<TLorentzVector>& mFoundJets, std::vector<std::string>& mUnmatched, std::vector<TLorentzVector>& mUnmatchedJets,const std::string& level) {

  ATH_MSG_DEBUG("Start Matching");

 JetSigIter chIt, chItB, chItE;

  chItB = ( (level == "L1") ? m_L1Items.begin()  : 
            (level == "HLT") ? m_HLTChains.begin() : m_L1Items.end() );

  chItE = ( (level == "L1") ? m_L1Items.end()  : 
            (level == "HLT") ? m_HLTChains.end() : m_L1Items.end() );

  bool MatchDR = false;
  bool passAny = false;
  double ChainThreshold = 0;

  for(chIt = chItB; chIt != chItE; chIt++ ) {
    
    MatchDR  = 0; 
   
    std::string ChainName =  (*chIt).first;
    std::string ContainerName=m_hltContainers[(*chIt).second];

    if (level=="HLT"){
      ChainThreshold=m_hltEtThres[(*chIt).second];
    }else if (level=="L1"){
      ChainThreshold=m_l1EtThres[(*chIt).second];
    }

    ATH_MSG_DEBUG("Chain= "<<ChainName<<" ContainerName= "<<ContainerName<<" Threshold= "<<ChainThreshold);  


   TLorentzVector v_trigjet_matched = DeltaRMatching(jet, ChainName , ContainerName, level, ChainThreshold, m_deltaRCut, MatchDR);
    

   if(MatchDR) {      
     // passAny = true;
     // mFound.push_back(ChainName);
     // mFoundJets.push_back(v_trigjet_matched);
   }else{
     mUnmatched.push_back(ChainName);
     mUnmatchedJets.push_back(v_trigjet_matched);
    }

   mFound.push_back(ChainName);
   mFoundJets.push_back(v_trigjet_matched);
       
  }//Loop over Items
  
  passAny=true;

  ATH_MSG_DEBUG("Passed MAtching= "<<passAny);

  return passAny; 
  
}//EOF

bool HLTJetMonTool::isLeadingJet(const xAOD::Jet *jet, const xAOD::JetContainer *jetcoll, double EtaLow, double EtaHigh, int Jetn){

  double njets=0;
  double nleading=0;
  bool   found_jetn=false;
  std::vector<double> v_ofjets;

  for(const auto & j : *jetcoll) {
    if (fabs(j->eta())>=EtaLow && fabs(j->eta())<=EtaHigh){
     
      v_ofjets.push_back(j->pt());
      
    }
  }
  

  if (v_ofjets.size()>=static_cast<size_t>(Jetn)) {
    for(unsigned int i=0; i<v_ofjets.size(); ++i) {
      njets=njets+1;
      if (Jetn==1 && jet->pt() >= v_ofjets[i] && fabs(jet->eta())>=EtaLow && fabs(jet->eta())<=EtaHigh){ //Select leading if jetn=1
        nleading=nleading+1;
      } else if (Jetn>1 && jet->pt()==v_ofjets[Jetn-1] ){ // select nth Jet in case jetn != 1
        found_jetn=true;
      }
    }
  }
  
  v_ofjets.clear();

  if (found_jetn){
    njets=99;
    nleading=99;
  }
  
  if (njets==nleading && njets>0){
    ATH_MSG_DEBUG("is leading");
    return true;
  }else{
    return false; 
  } 
}

TLorentzVector HLTJetMonTool::DeltaRMatching(const xAOD::Jet *jet, const std::string &ChainName, const std::string &/*ContainerName*/, const std::string& level, double thrHLT, float DRCut,bool& Pass){

  double DRmin=99;
  double Ptmin=-99;
  unsigned int    count=1;

  ATH_MSG_DEBUG("DeltaR Matching");

  TLorentzVector v_OF;
  v_OF.SetPtEtaPhiE(jet->pt(),jet->eta(), jet->phi(),jet->e());
  TLorentzVector v_best_match;
  
  if (level=="L1"){

 ATH_MSG_DEBUG("Chain "<<ChainName.c_str());

    Trig::FeatureContainer chainFeatures = getTDT()->features(ChainName.c_str());
    for(Trig::FeatureContainer::combination_const_iterator chIt = chainFeatures.getCombinations().begin(); 
	chIt != chainFeatures.getCombinations().end(); ++chIt ) {
      
      std::vector< Trig::Feature<TrigRoiDescriptor> > theRoIdesc = chIt->get<TrigRoiDescriptor>("initialRoI");
      for(std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator combIt = theRoIdesc.begin(); 
	  combIt!= theRoIdesc.end(); ++combIt){
	
	if(!(combIt->cptr())) continue;	 
	xAOD::JetRoIContainer::const_iterator it_L1 = m_L1JetRoIC->begin();
	xAOD::JetRoIContainer::const_iterator it_e_L1 = m_L1JetRoIC->end();
	
	for (; it_L1 != it_e_L1; ++it_L1) {

	  double et = ( (*it_L1)->et4x4());
	  if(et < 1.e-3) et = 0.;
	  double eta = (*it_L1)->eta();
	  double ene = et * cosh(eta);
	  double phi = (*it_L1)->phi(); 

	  TLorentzVector v_L1;	
	  v_L1.SetPtEtaPhiE(et,eta,phi,ene);
	  
	  double DR=v_OF.DeltaR(v_L1);
	  //double DR=delta_r((*it_L1)->eta(),(*it_L1)->phi(),jet->eta(),jet->phi());
	  
	  ATH_MSG_DEBUG("Jet pt= "<<et<<" Jet eta= "<<eta<< "Et thresh = "<<thrHLT);
	      
	  if (DR<DRmin){ //select best matching jet
	   
	    DRmin=DR;
	    //  v_best_match=v_L1;	   

	  }

	  if (fabs(v_L1.Eta())>m_l1EtaLowThres[ChainName.c_str()] && fabs(v_L1.Eta())<m_l1EtaHighThres[ChainName.c_str()] && v_L1.Pt()>Ptmin){ //select leading jet in the chosen eta range
	    Ptmin= v_L1.Pt();
	    v_best_match=v_L1;	  
 
	    ATH_MSG_DEBUG("is leading");

	  } 
	}	   
      } // for combIt
    } // l2It  
  } //if level is L1


  else if (level=="HLT"){

    ATH_MSG_DEBUG("Chain "<<ChainName.c_str());
 
    auto cg = getTDT()->getChainGroup(Form("HLT_%s",ChainName.c_str())); //get features
    auto fc = cg->features();
    auto JetFeatureContainers = fc.get<xAOD::JetContainer>(); //get features container
    
    for(auto jcont : JetFeatureContainers) {
      
      for (auto j : *jcont.cptr()) {
		
	double et = 0., epsilon = 1.e-3;
	if(j->pt() > epsilon) et = (j->pt())/Gaudi::Units::GeV;
	if(et < epsilon) et = 0;

	TLorentzVector v_HLT;
	v_HLT.SetPtEtaPhiE(j->pt(),j->eta(), j->phi(),j->e());
	
	double DR=v_OF.DeltaR(v_HLT);
	//double DR=delta_r(j->eta(),j->phi(),jet->eta(),jet->phi());
	


	ATH_MSG_DEBUG("Jet pt= "<<j->pt()<<" Jet eta= "<<j->eta()<<" Eta Low= "<<m_hltEtaLowThres[ChainName.c_str()]<<" Eta High= "<<m_hltEtaHighThres[ChainName.c_str()]);      


	if (DR<DRmin){ //select best matching jet 
	  DRmin=DR;
	  // v_best_match=v_HLT;
	}

	if (fabs(v_HLT.Eta())>m_hltEtaLowThres[ChainName.c_str()] && fabs(v_HLT.Eta())<m_hltEtaHighThres[ChainName.c_str()] && v_HLT.Pt()>Ptmin && m_hltJetn[ChainName.c_str()]==1){ //select leading jet in the chosen eta range
	  Ptmin= v_HLT.Pt();
	  v_best_match=v_HLT;	  
	  ATH_MSG_DEBUG("is leading");

	}
        else if (fabs(v_HLT.Eta())>m_hltEtaLowThres[ChainName.c_str()] &&
                 fabs(v_HLT.Eta())<m_hltEtaHighThres[ChainName.c_str()] &&
                 m_hltJetn[ChainName.c_str()]==count &&
                 m_hltJetn[ChainName.c_str()] != 1) { // select nth jet in the chosen eta range
	  v_best_match=v_HLT;	  
	  ATH_MSG_DEBUG("is leading");
	}
	
       	count=count+1;
	
      }// loop over jet container
    }// loop over features container
  }// if level is HLT

  if (DRmin<DRCut){
    Pass=true;
    ATH_MSG_DEBUG("The Match is found with DRmin = "<<DRmin);
  }else{
    Pass=false;
    ATH_MSG_DEBUG("The Match is not found");
  }

  return v_best_match;

}//EOF


StatusCode HLTJetMonTool::proc( ) {

  // if (m_debuglevel) 
    ATH_MSG_INFO( "in HLTJetMonTool::proc()" );

// LS 24 Jan 2014 - Interval checking now managed in ManagedMonitorTool so do nothing.
//  if(isEndOfEventsBlock){}
//  if(isEndOfLumiBlock){}
    
    if (endOfRunFlag()){

    TH1      *h(0);
    TH1      *hnum(0);
    TH1      *hden(0);

    JetSigIter lIt;
    
    unsigned int Nelem = m_OFJetC.size();

    for(unsigned int k = 0; k < Nelem; k++ ) { //OF Chains
      const xAOD::JetContainer *jetcoll = m_OFJetC[k];
      if(jetcoll) {
	std::string lvl = m_OFpfx; 
	std::string mgrp = m_monGroups[Form("%s%d",m_OFpfx.c_str(),k)];
	setCurrentMonGroup(mgrp);
	
	if((h  = hist(Form("%sSigma_vs_LB",lvl.c_str())))){      
	  if (m_v_lbn.size()>0) h->GetXaxis()->SetRangeUser((*std::min_element(m_v_lbn.begin(),m_v_lbn.end()))-1,(*std::max_element(m_v_lbn.begin(),m_v_lbn.end()))+1);
	}

 
	//::::::::::::::::::::::::::::::::FILL TURN-ON CURVES::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	std::string malg = Form("%s%d",m_OFpfx.c_str(),k);
	mgrp = m_monGroups[malg];
	std::string mgrp_eff = m_monGroups[Form("%s%dEff",m_OFpfx.c_str(),k)];

	setCurrentMonGroup(mgrp_eff);


	ATH_MSG_DEBUG("mon group "<<mgrp_eff.c_str());

		
	for(lIt = m_L1Items.begin(); lIt != m_L1Items.end(); ++lIt) {
	  std::string L1itemName = (*lIt).first;       

	  
	  if((h = hist(Form("%s_Eff_vs_pt", L1itemName.c_str()))))  {
	    if((hnum=hist(Form("%s_Eff_vs_pt_num",L1itemName.c_str())))){
	      if((hden=hist(Form("%s_Eff_vs_pt_den",L1itemName.c_str())))){
		h->Divide(hnum,hden,1,1,"B");
		h->SetMarkerColor(kBlack);
		h->SetLineColor(kBlack);
		h->SetMarkerSize(0.9);
		h->SetMarkerStyle(20);
		h->SetOption("err");
	      }
	    }
	  }
	  if((h = hist(Form("%s_Eff_vs_eta",L1itemName.c_str())))) {
	    if((hnum=hist(Form("%s_Eff_vs_eta_num",L1itemName.c_str())))){
	      if((hden=hist(Form("%s_Eff_vs_eta_den",L1itemName.c_str())))){
		h->Divide(hnum,hden,1,1,"B");
		h->SetMarkerColor(kBlack);
		h->SetLineColor(kBlack);
		h->SetMarkerSize(0.9);
		h->SetMarkerStyle(20);
		h->SetOption("err");
	      }
	    }
	  }
	  if((h = hist(Form("%s_Eff_vs_phi",L1itemName.c_str())))) {
	    if((hnum=hist(Form("%s_Eff_vs_phi_num",L1itemName.c_str())))){
	      if((hden=hist(Form("%s_Eff_vs_phi_den",L1itemName.c_str())))){
		h->Divide(hnum,hden,1,1,"B");
		h->SetMarkerColor(kBlack);
		h->SetLineColor(kBlack);
		h->SetMarkerSize(0.9);
		h->SetMarkerStyle(20);
		h->SetOption("err");
	      }	  
	    }
	  }
	} //End Loop over L1
	
	for(lIt = m_HLTChains.begin(); lIt != m_HLTChains.end(); ++lIt) {
	  std::string HLTchainName = (*lIt).first;
	  

	  ATH_MSG_DEBUG("HLT Chain "<<HLTchainName.c_str());


	  if((h = hist(Form("%s_Eff_vs_pt", HLTchainName.c_str()))))  {
	    if((hnum=hist(Form("%s_Eff_vs_pt_num",HLTchainName.c_str())))){
	      if((hden=hist(Form("%s_Eff_vs_pt_den",HLTchainName.c_str())))){
		h->Divide(hnum,hden,1,1,"B");
		h->SetMarkerColor(kBlack);
		h->SetLineColor(kBlack);
		h->SetMarkerSize(0.9);
		h->SetMarkerStyle(20);
		h->SetOption("err");
	      }
	    }
	  }
	  if((h = hist(Form("%s_Eff_vs_eta",HLTchainName.c_str())))) {
	    if((hnum=hist(Form("%s_Eff_vs_eta_num",HLTchainName.c_str())))){
	      if((hden=hist(Form("%s_Eff_vs_eta_den",HLTchainName.c_str())))){
		h->Divide(hnum,hden,1,1,"B");
		h->SetMarkerColor(kBlack);
		h->SetLineColor(kBlack);
		h->SetMarkerSize(0.9);
		h->SetMarkerStyle(20);
		h->SetOption("err");
	      }
	    }
	  }
	  if((h = hist(Form("%s_Eff_vs_phi",HLTchainName.c_str())))) {
	    if((hnum=hist(Form("%s_Eff_vs_phi_num",HLTchainName.c_str())))){
	      if((hden=hist(Form("%s_Eff_vs_phi_den",HLTchainName.c_str())))){
		h->Divide(hnum,hden,1,1,"B");
		h->SetMarkerColor(kBlack);
		h->SetLineColor(kBlack);
		h->SetMarkerSize(0.9);
		h->SetMarkerStyle(20);
		h->SetOption("err");
	      }		
	    }
	  }
	} //End Loop over HLT

	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	
      }//if jetcoll
    }//loop over jetcoll


    //::::::::::::::::::::::::::::::::::::::SET HISTOGRAM PROPERTIES:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

 


    setCurrentMonGroup(m_monGroups["L1"]);
 
    if((h = hist("L1Sigma_vs_LB"))) { //basic L1
           if (m_v_lbn.size()>0) h->GetXaxis()->SetRangeUser((*std::min_element(m_v_lbn.begin(),m_v_lbn.end()))-1,(*std::max_element(m_v_lbn.begin(),m_v_lbn.end()))+1);
     }

    for(JetSigIter l1 = m_basicL1Trig.begin(); l1 != m_basicL1Trig.end(); ++l1) { //L1 Chain
      setCurrentMonGroup(m_monGroups[(*l1).first]);
      if((h  = hist("L1Sigma_vs_LB"))){
	if (m_v_lbn.size()>0) h->GetXaxis()->SetRangeUser((*std::min_element(m_v_lbn.begin(),m_v_lbn.end()))-1,(*std::max_element(m_v_lbn.begin(),m_v_lbn.end()))+1);
      }     
    }

    unsigned int NHLTelem = m_HLTJetC.size();//basic HLT
    
    for(unsigned int k = 0; k < NHLTelem; k++ ) {
      const xAOD::JetContainer *jetcoll = 0;
      jetcoll = m_HLTJetC[k];
      
      if(jetcoll) {
	std::string lvl = m_HLTpfx; 
	std::string mgrp = m_monGroups[Form("%s%d",m_HLTpfx.c_str(),k)];
	setCurrentMonGroup(mgrp);
	
	if((h  = hist(Form("%sSigma_vs_LB",lvl.c_str())))){    
	  if (m_v_lbn.size()>0) h->GetXaxis()->SetRangeUser((*std::min_element(m_v_lbn.begin(),m_v_lbn.end()))-1,(*std::max_element(m_v_lbn.begin(),m_v_lbn.end()))+1);
	}
      }
    }

    for(JetSigIter hltit = m_HLTChains.begin(); hltit != m_HLTChains.end(); ++hltit) { // HLT Chain
      setCurrentMonGroup(m_monGroups[(*hltit).first]);
        
      if((h  = hist("HLTSigma_vs_LB"))){
	if (m_v_lbn.size()>0) h->GetXaxis()->SetRangeUser((*std::min_element(m_v_lbn.begin(),m_v_lbn.end()))-1,(*std::max_element(m_v_lbn.begin(),m_v_lbn.end()))+1);
      } 
    } 
    
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    
    
    } //if endOfRun
    
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

int HLTJetMonTool::basicKineVar(const std::string& hist, std::vector<std::string>& kinevars) {

  typedef  boost::tokenizer<boost::char_separator<char> > Tokenizer;
  int retval = -1;
  kinevars.clear();
  const char* separator=";";
  Tokenizer tokComp(hist, boost::char_separator<char>(separator));
  for(Tokenizer::const_iterator vars = tokComp.begin(); vars != tokComp.end(); ++vars) kinevars.push_back(*vars);
  retval = (int)kinevars.size();
  return retval;
}
// ------------------------------------------------------------------------------------
