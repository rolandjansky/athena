/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// METMonTool.cxx
// AUTHORS: Michele Consonni
// 20110202: Jet cleaning added by Isabel Pedraza and Michele Consonni
// **********************************************************************


#include "MissingETMonitoring/METMonTool.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODEventInfo/EventInfo.h"

#include "xAODMissingET/MissingET.h" 
#include "xAODMissingET/MissingETContainer.h" 
#include "xAODMissingET/MissingETComposition.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TAxis.h"

#include <cmath>
#include <sstream>

// using xAOD::MissingETContainer;
// using xAOD::MissingETComposition;
// using xAOD::MissingET;

// *********************************************************************
// Public Methods
// *********************************************************************


METMonTool::METMonTool(const std::string& type, const std::string& name, const IInterface* parent)
  :ManagedMonitorToolBase(type, name, parent),
   m_suffix(""),
   m_metFinKey("MET_RefFinal"),
   m_metCalKey("MET_LocHadTopo"),
   m_metRegKey(""),
   m_jetColKey("AntiKt4LCTopoJets"),
   m_eleColKey("Electrons"),
   m_muoColKey("Muons"),
   m_met_cut_80(false),
   m_etabin(100),
   m_phibin(100),
   m_etbin(800),
   m_met_cut(0.0),
   m_etrange(400.),
   m_etrangeSumFactor(10.),
   m_doFillNegativeSumEt(false),
   m_tos(1.25),
   m_truncatedMean(200.),
   m_doJetcleaning(false),
   m_badJets(false),
   m_maxNumContainerWarnings(10)
{
  // fill vector with Missing Et Sources
  m_metKeys.push_back("MET_Base");
  m_metKeys.push_back("MET_Topo");
  m_metKeys.push_back("MET_Track");
  // m_metKeys.push_back("MET_PFlow");

  m_ContainerWarnings_Muon = 0;
  m_ContainerWarnings_Ele = 0;
  m_ContainerWarnings_Jet = 0;

  // fill vector with caloIndices equal to CaloIndex enum in MissingEtCalo
  m_calIndices.push_back(0);
  m_calIndices.push_back(1);
  m_calIndices.push_back(2);
  m_calIndices.push_back(3);
  m_calIndices.push_back(4);
  m_calIndices.push_back(5);
  m_calIndices.push_back(6);
  m_calStrings.push_back("PEMB");
  m_calStrings.push_back("EMB");
  m_calStrings.push_back("PEME");// PEMEC -> PEME
  m_calStrings.push_back("EME");
  m_calStrings.push_back("TILE");
  m_calStrings.push_back("HEC");
  m_calStrings.push_back("FCAL");
  //float etCalFactors[] =  { 0.2, 0.5, 0.2, 0.5, 0.5, 0.5, 0.5 };
  float etCalFactors[] = { 0.2, 0.6, 0.1, 0.5, 0.5, 0.4, 0.3 };
  m_etrangeCalFactors.assign(etCalFactors, etCalFactors + 7);

  // fill vector with regionIndices equal to RegionIndex enum in MissingEtRegions
  m_regIndices.push_back(0);
  m_regIndices.push_back(1);
  m_regIndices.push_back(2);
  m_regStrings.push_back("Central");
  m_regStrings.push_back("EndCap");
  m_regStrings.push_back("Forward");

  //    m_met_cut_80 = false;

  float etRegFactors[] = { 0.1, 0.1, 0.1 };
  m_etrangeRegFactors.assign(etRegFactors, etRegFactors + 3);

  // declare properties
  declareProperty("NameSuffix", m_suffix);

  declareProperty("metKeys", m_metKeys);
  declareProperty("metFinKey", m_metFinKey);
  declareProperty("metCalKey", m_metCalKey);
  declareProperty("metRegKey", m_metRegKey);
  declareProperty("jetColKey", m_jetColKey);
  declareProperty("eleColKey", m_eleColKey);
  declareProperty("muoColKey", m_muoColKey);

  declareProperty("doMetCut80", m_met_cut_80);

  declareProperty("nEtBins", m_etbin);
  declareProperty("nEtaBins", m_etabin);
  declareProperty("nPhiBins", m_phibin);
  declareProperty("metCut", m_met_cut);

  declareProperty("EtRange", m_etrange);
  declareProperty("SumEtRangeFactor", m_etrangeSumFactor);
  declareProperty("EtRangeCalFactors", m_etrangeCalFactors);
  declareProperty("EtRangeRegFactors", m_etrangeRegFactors);

  declareProperty("FillNegativeSumEt", m_doFillNegativeSumEt);
  declareProperty("YaxisTitleOffset", m_tos);


  declareProperty("doJetcleaning", m_doJetcleaning); //name used in python to overwrite
  declareProperty("badJets", m_badJets);
  declareProperty("maxNumContainerWarnings", m_maxNumContainerWarnings);
  declareProperty("JetSelectorTool", m_selTool);

  m_Pi = 3.141592653589793238462;
}


StatusCode METMonTool::initialize()
{

  CHECK( m_genTool.retrieve() );
  //msg_info// ATH_MSG_INFO("METMonTool::143");
  //msg_info// ATH_MSG_INFO("METMonTool::143");
  //msg_info// ATH_MSG_INFO("METMonTool::143");

  //resize vector with number of WARNINGs already displayed for retrieval of a certain container
  m_ContainerWarnings_metKeys.resize(m_metKeys.size(), 0);

  if (!service("THistSvc", m_thistSvc).isSuccess()) {
    //	m_log << MSG::ERROR << "Unable to locate THistSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_badJets || m_doJetcleaning)   CHECK(m_selTool.retrieve());
  return ManagedMonitorToolBase::initialize();
}

METMonTool::~METMonTool()
{}


// *********************************************************************
// Book Histograms
// *********************************************************************


StatusCode METMonTool::bookHistograms()
{
  //msg_info// ATH_MSG_INFO("METMonTool::165");
  //msg_info// ATH_MSG_INFO("METMonTool::165");
  //msg_info// ATH_MSG_INFO("METMonTool::165");
  return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("in bookHistograms()");//msg(MSG::DEBUG) << "in bookHistograms()" << endmsg;

  // If m_metFinKey is not empty then append it to m_metKeys
  if (m_metFinKey != "")
    {
      std::vector<std::string>::iterator it = m_metKeys.begin();
      // Remove m_metFinKey from m_metKeys if it already exists
      for (; it != m_metKeys.end();)
        {
	  if ((*it) == m_metFinKey) it = m_metKeys.erase(it);
	  else it++;
        }
      m_metKeys.push_back(m_metFinKey);
    }

  m_ContainerWarnings_metKeys.resize(m_metKeys.size(), 0);

  // ...and debug
  // if ( msgLvl(MSG::DEBUG) ) {
  ATH_MSG_DEBUG("Using the following keys:");
  ATH_MSG_DEBUG("metCalKey = " << m_metCalKey);
  ATH_MSG_DEBUG("metRegKey = " << m_metRegKey);
  ATH_MSG_DEBUG("metKeys   = ");
  std::vector<std::string>::iterator it = m_metKeys.begin();
  for (; it != m_metKeys.end(); it++)
    {
      ATH_MSG_DEBUG((*it));
    }
  ATH_MSG_DEBUG("");
  // }

  // Check consistency between m_etrangeCalFactors and m_calIndices
  if (m_etrangeCalFactors.size() < m_calIndices.size())
    {
      ATH_MSG_DEBUG("Vector of Et ranges for calorimeter subsystems is incomplete: filling with 1's");
      for (unsigned int i = m_etrangeCalFactors.size(); i < m_calIndices.size(); i++)
        {
	  m_etrangeCalFactors.push_back(1.);
        }
    }

  // Check consistency between m_etrangeRegFactors and m_regIndices
  if (m_etrangeRegFactors.size() < m_regIndices.size())
    {
      ATH_MSG_DEBUG("Vector of Et ranges for regions is incomplete: filling with 1's");
      for (unsigned int i = m_etrangeRegFactors.size(); i < m_regIndices.size(); i++)
        {
	  m_etrangeRegFactors.push_back(1.);
        }
    }

  std::string suffix = "";
  if (m_suffix != "")
    {
      suffix += "_" + m_suffix;
    }

  //MonGroup met_sources(this, "MET/Sources" + suffix, run, ATTRIB_MANAGED);
  MonGroup met_sources(this, "MET/Sources" + suffix, (m_environment == AthenaMonManager::online) ? run : lumiBlock);
  MonGroup met_calos(this, "MET/Calos" + suffix, (m_environment == AthenaMonManager::online) ? run : lumiBlock);//, run, ATTRIB_MANAGED);
  MonGroup met_regions(this, "MET/Regions" + suffix, (m_environment == AthenaMonManager::online) ? run : lumiBlock);//, run, ATTRIB_MANAGED);
  MonGroup met_jets(this, "MET/Jets" + suffix, (m_environment == AthenaMonManager::online) ? run : lumiBlock);//, run, ATTRIB_MANAGED);
  MonGroup met_electrons(this, "MET/Electrons" + suffix, (m_environment == AthenaMonManager::online) ? run : lumiBlock);//, run, ATTRIB_MANAGED);
  MonGroup met_muons(this, "MET/Muons" + suffix, (m_environment == AthenaMonManager::online) ? run : lumiBlock);//, run, ATTRIB_MANAGED);
  MonGroup met_summary(this, "MET/Summary" + suffix, (m_environment == AthenaMonManager::online) ? run : lumiBlock);//, run, ATTRIB_MANAGED);

  // Book histograms

  if (m_environment == AthenaMonManager::online)
    {
      // book histograms that are only made in the online environment...
    }
  if (m_dataType == AthenaMonManager::cosmics)
    {
      // book histograms that are only relevant for cosmics data...
    }

  unsigned int nSources = m_metKeys.size();
  for (unsigned int i = 0; i < nSources; i++)
    {
      if (m_metFinKey != "" && i == nSources - 1)
        {
	  bookSourcesHistograms(m_metKeys[i], met_summary, true).ignore();
	  if (m_jetColKey != "") bookProfileHistograms(m_metKeys[i], "Jet", met_jets, &m_iJet).ignore();
	  if (m_eleColKey != "") bookProfileHistograms(m_metKeys[i], "Ele", met_electrons, &m_iEle).ignore();
	  if (m_muoColKey != "") bookProfileHistograms(m_metKeys[i], "Muo", met_muons, &m_iMuo).ignore();
        }
      else bookSourcesHistograms(m_metKeys[i], met_sources, false).ignore();
    }

  if (m_metCalKey != "") bookCalosHistograms(met_calos).ignore();
  if (m_metRegKey != "") bookRegionsHistograms(met_regions).ignore();

  bookSummaryHistograms(met_summary).ignore();

  ATH_MSG_DEBUG("bookHistograms() ended");
    
  return StatusCode::SUCCESS;
}


StatusCode METMonTool::clearHistograms()
{
  //msg_info// ATH_MSG_INFO("METMonTool::259");
  //msg_info// ATH_MSG_INFO("METMonTool::259");
  //msg_info// ATH_MSG_INFO("METMonTool::259");
  return StatusCode::SUCCESS;     // TB, interesting, why do we need to clear them? Is this memory management only?
  ATH_MSG_DEBUG("in clearHistograms()");

  // Sources
  m_et.clear();
  m_ex.clear();
  m_ey.clear();
  m_phi.clear();
  m_sumet.clear();

  // Profiles
  m_metVsEta.clear();
  //m_metParaVsEta.clear();
  //m_metPerpVsEta.clear();
  m_dphiVsEta.clear();
  m_metVsPhi.clear();
  //m_metParaVsPhi.clear();
  //m_metPerpVsPhi.clear();
  m_dphiVsPhi.clear();
  m_metVsEtaPhi.clear();

  // Calorimeters
  m_etCal.clear();
  m_exCal.clear();
  m_eyCal.clear();
  m_phiCal.clear();
  m_sumetCal.clear();

  // Regions
  m_etReg.clear();
  m_exReg.clear();
  m_eyReg.clear();
  m_phiReg.clear();
  m_sumetReg.clear();
    
  return StatusCode::SUCCESS;
}


StatusCode METMonTool::bookSourcesHistograms(std::string& metName, MonGroup& met_mongroup, bool doProfiles)
{
  //msg_info// ATH_MSG_INFO("METMonTool::303");
  //msg_info// ATH_MSG_INFO("METMonTool::303");
  //msg_info// ATH_MSG_INFO("METMonTool::303");
  return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("in bookSourcesHistograms(" << metName.c_str() << ")");
    
  std::ostringstream hName;
  std::ostringstream hTitle;
  std::ostringstream hxTitle;
  std::ostringstream hyTitle;
  TH1* h = 0;

  // ***************************** //
  // Missing ET Sources Monitoring //
  // ***************************** //

  // Set histogram ranges
  float etmin = 0., etmax = m_etrange;
  float sumetmin = 0., sumetmax = m_etrange*m_etrangeSumFactor;
  if (m_doFillNegativeSumEt) sumetmin = -sumetmax;

  if ((m_suffix.find("EF_xe20_noMu") != std::string::npos))
    {
      etmax = m_etrange*0.5;
      sumetmax = m_etrange*0.5*m_etrangeSumFactor;
      m_truncatedMean = 0.5*m_truncatedMean;
    }
  // Et Distributions
  hName << "et_" << metName;
  hTitle << "Et Distribution (" << metName << ")";
  hxTitle << "MET Et (GeV)";
  hyTitle << "Events";
  h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, etmin, etmax);
  h->GetXaxis()->SetTitle(hxTitle.str().c_str());
  h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(h).ignore();
  m_et.push_back(h);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  // Ex Distributions
  hName << "ex_" << metName;
  hTitle << "Ex Distribution (" << metName << ")";
  hxTitle << "MET Etx (GeV)";
  hyTitle << "Events";
  h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, -etmax, etmax);
  h->GetXaxis()->SetTitle(hxTitle.str().c_str());
  h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(h).ignore();
  m_ex.push_back(h);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  // Ey Distributions
  hName << "ey_" << metName;
  hTitle << "Ey Distribution (" << metName << ")";
  hxTitle << "MET Ety (GeV)";
  hyTitle << "Events";
  h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, -etmax, etmax);
  h->GetXaxis()->SetTitle(hxTitle.str().c_str());
  h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(h).ignore();
  m_ey.push_back(h);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  // Phi Distributions
  hName << "phi_" << metName;
  hTitle << "Phi Distribution (" << metName << ")";
  hxTitle << "MET Phi (radian)";
  hyTitle << "Events";
  h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_phibin, -m_Pi, +m_Pi);
  h->GetXaxis()->SetTitle(hxTitle.str().c_str());
  h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(h).ignore();
  m_phi.push_back(h);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  // SumEt Distributions
  hName << "sumet_" << metName;
  hTitle << "SumEt Distribution (" << metName << ")";
  hxTitle << "SumEt (GeV)";
  hyTitle << "Events";
  if (metName == "MET_Cryo" || metName == "MET_MuonBoy" || metName == "MET_Cryo" || metName == "MET_CellOut_em")
    sumetmax = m_etrange*m_etrangeSumFactor*0.25;
  h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, sumetmin, sumetmax);
  h->GetXaxis()->SetTitle(hxTitle.str().c_str());
  h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(h).ignore();
  m_sumet.push_back(h);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");

  if (doProfiles)
    {
      // MissingET Vs SumEt
      hName << "metVsSumEt_" << metName;
      hTitle << "MET Vs SumEt Distribution (" << metName << ")";
      hxTitle << "SumEt (GeV)";
      hyTitle << "MET Et (GeV)";
      m_metVsSumEt = new TProfile(hName.str().c_str(), hTitle.str().c_str(), m_etbin, sumetmin, sumetmax);
      m_metVsSumEt->GetXaxis()->SetTitle(hxTitle.str().c_str());
      m_metVsSumEt->GetYaxis()->SetTitle(hyTitle.str().c_str()); m_metVsSumEt->GetYaxis()->SetTitleOffset(m_tos);
      met_mongroup.regHist(m_metVsSumEt).ignore();
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");

      // MissingET Vs Phi
      hName << "metVsMetPhi_" << metName;
      hTitle << "MET Vs MetPhi Distribution (" << metName << ")";
      hxTitle << "MET Phi (radian)";
      hyTitle << "MET Et (GeV)";
      m_metVsMetPhi = new TProfile(hName.str().c_str(), hTitle.str().c_str(), m_phibin, -m_Pi, +m_Pi);
      m_metVsMetPhi->GetXaxis()->SetTitle(hxTitle.str().c_str());
      m_metVsMetPhi->GetYaxis()->SetTitle(hyTitle.str().c_str()); m_metVsMetPhi->GetYaxis()->SetTitleOffset(m_tos);
      met_mongroup.regHist(m_metVsMetPhi).ignore();
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
    }
    
  return StatusCode::SUCCESS;
}


StatusCode METMonTool::bookCalosHistograms(MonGroup& met_calos)
{
  //msg_info// ATH_MSG_INFO("METMonTool::431");
  //msg_info// ATH_MSG_INFO("METMonTool::431");
  //msg_info// ATH_MSG_INFO("METMonTool::431");
  return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("in bookCalosHistograms()");

  std::ostringstream hName;
  std::ostringstream hTitle;
  std::ostringstream hxTitle;
  std::ostringstream hyTitle;
  TH1* h = 0;

  // ********************************** //
  // Missing ET Calorimeters Monitoring //
  // ********************************** //

  for (unsigned int i = 0; i < m_calIndices.size(); i++)
    {
      // Set histogram ranges
      float etmin = 0., etmax = m_etrange*m_etrangeCalFactors[i];
      float sumetmin = 0., sumetmax = m_etrange*m_etrangeSumFactor*m_etrangeCalFactors[i];
      if (m_doFillNegativeSumEt) sumetmin = -1.*sumetmax;

      // Et Distributions
      hName << "et_" << m_calStrings[i];
      hTitle << "Et Distribution (" << m_calStrings[i] << ")";
      hxTitle << "MET Et (GeV)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, etmin, etmax);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_calos.regHist(h).ignore();
      m_etCal.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
      // Ex Distributions
      hName << "ex_" << m_calStrings[i];
      hTitle << "Ex Distribution (" << m_calStrings[i] << ")";
      hxTitle << "MET Etx (GeV)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, -etmax, etmax);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_calos.regHist(h).ignore();
      m_exCal.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
      // Ey Distributions
      hName << "ey_" << m_calStrings[i];
      hTitle << "Ey Distribution (" << m_calStrings[i] << ")";
      hxTitle << "MET Ety (GeV)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, -etmax, etmax);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_calos.regHist(h).ignore();
      m_eyCal.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
      // Phi Distributions
      hName << "phi_" << m_calStrings[i];
      hTitle << "Phi Distribution (" << m_calStrings[i] << ")";
      hxTitle << "MET Phi (radian)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_phibin, -m_Pi, +m_Pi);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_calos.regHist(h).ignore();
      m_phiCal.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
      // SumEt Distributions
      hName << "sumet_" << m_calStrings[i];
      hTitle << "SumEt Distribution (" << m_calStrings[i] << ")";
      hxTitle << "SumEt (GeV)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, sumetmin, sumetmax);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_calos.regHist(h).ignore();
      m_sumetCal.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
    }
  
  return StatusCode::SUCCESS;
}


StatusCode METMonTool::bookRegionsHistograms(MonGroup& met_regions)
{
  //msg_info// ATH_MSG_INFO("METMonTool::526");
  //msg_info// ATH_MSG_INFO("METMonTool::526");
  //msg_info// ATH_MSG_INFO("METMonTool::526");
  return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("in bookRegionsHistograms()");

  std::ostringstream hName;
  std::ostringstream hTitle;
  std::ostringstream hxTitle;
  std::ostringstream hyTitle;
  TH1* h = 0;

  // ***************************** //
  // Missing ET Regions Monitoring //
  // ***************************** //

  for (unsigned int i = 0; i < m_regIndices.size(); i++)
    {
      // Set histogram ranges
      float etmin = 0., etmax = m_etrange*m_etrangeRegFactors[i];
      float sumetmin = 0., sumetmax = m_etrange*m_etrangeSumFactor*m_etrangeRegFactors[i];
      if (m_doFillNegativeSumEt) sumetmin = -1.*sumetmax;

      // Ex Distributions
      hName << "et_" << m_regStrings[i];
      hTitle << "Et Distribution (" << m_regStrings[i] << ")";
      hxTitle << "MET Et (GeV)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, etmin, etmax);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_regions.regHist(h).ignore();
      m_etReg.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
      // Ex Distributions
      hName << "ex_" << m_regStrings[i];
      hTitle << "Ex Distribution (" << m_regStrings[i] << ")";
      hxTitle << "MET Etx (GeV)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, -etmax, etmax);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_regions.regHist(h).ignore();
      m_exReg.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
      // Ey Distributions
      hName << "ey_" << m_regStrings[i];
      hTitle << "Ey Distribution (" << m_regStrings[i] << ")";
      hxTitle << "MET Ety (GeV)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, -etmax, etmax);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_regions.regHist(h).ignore();
      m_eyReg.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
      // Phi Distributions
      hName << "phi_" << m_regStrings[i];
      hTitle << "Phi Distribution (" << m_regStrings[i] << ")";
      hxTitle << "MET Phi (radian)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_phibin, -m_Pi, +m_Pi);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_regions.regHist(h).ignore();
      m_phiReg.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
      // SumEt Distributions
      hName << "sumet_" << m_regStrings[i];
      hTitle << "SumEt Distribution (" << m_regStrings[i] << ")";
      hxTitle << "SumEt (GeV)";
      hyTitle << "Events";
      h = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_etbin, sumetmin, sumetmax);
      h->GetXaxis()->SetTitle(hxTitle.str().c_str());
      h->GetYaxis()->SetTitle(hyTitle.str().c_str()); h->GetYaxis()->SetTitleOffset(m_tos);
      met_regions.regHist(h).ignore();
      m_sumetReg.push_back(h);
      hName.str("");
      hTitle.str("");
      hxTitle.str(""); hyTitle.str("");
    }
  
  return StatusCode::SUCCESS;
}


StatusCode METMonTool::bookSummaryHistograms(MonGroup& met_summary)
{
  //msg_info// ATH_MSG_INFO("METMonTool::621");
  //msg_info// ATH_MSG_INFO("METMonTool::621");
  //msg_info// ATH_MSG_INFO("METMonTool::621");
  return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("in bookSummaryHistograms()");

  std::ostringstream exTitle;
  std::ostringstream eyTitle;
  std::ostringstream phiTitle;

  exTitle << "MET <etx> (GeV)";
  eyTitle << "MET <ety> (GeV)";
  phiTitle << "MET <phi> (radian)";

  // ************************************ //
  // Missing Ex, Ey, Phi Means Monitoring //
  // ************************************ //

  unsigned int nSources = m_metKeys.size();
  unsigned int nCalos = m_calIndices.size();
  unsigned int nRegions = m_regIndices.size();

  if (nSources > 1)
    {
      m_exMean = new TProfile("exMean", "Means of the Ex Sources", nSources, 0., nSources);
      m_eyMean = new TProfile("eyMean", "Means of the Ey Sources", nSources, 0., nSources);
      m_phiMean = new TProfile("phiMean", "Means of the Phi Sources", nSources, 0., nSources);
      m_exMean->GetYaxis()->SetTitle(exTitle.str().c_str()); m_exMean->GetYaxis()->SetTitleOffset(m_tos);
      m_eyMean->GetYaxis()->SetTitle(eyTitle.str().c_str()); m_eyMean->GetYaxis()->SetTitleOffset(m_tos);
      m_phiMean->GetYaxis()->SetTitle(phiTitle.str().c_str()); m_phiMean->GetYaxis()->SetTitleOffset(m_tos);
      for (unsigned int i = 0; i < nSources; i++)
        {
	  m_exMean->GetXaxis()->SetBinLabel(i + 1, m_metKeys[i].c_str());
	  m_eyMean->GetXaxis()->SetBinLabel(i + 1, m_metKeys[i].c_str());
	  m_phiMean->GetXaxis()->SetBinLabel(i + 1, m_metKeys[i].c_str());
        }
      met_summary.regHist(m_exMean).ignore();
      met_summary.regHist(m_eyMean).ignore();
      met_summary.regHist(m_phiMean).ignore();
    }

  if (m_metCalKey != "" && nCalos > 1)
    {
      m_exCalMean = new TProfile("exCalMean", "Means of the Ex Calorimeters", nCalos, 0., nCalos);
      m_eyCalMean = new TProfile("eyCalMean", "Means of the Ey Calorimeters", nCalos, 0., nCalos);
      m_phiCalMean = new TProfile("phiCalMean", "Means of the Phi Calorimeters", nCalos, 0., nCalos);
      m_exCalMean->GetYaxis()->SetTitle(exTitle.str().c_str()); m_exCalMean->GetYaxis()->SetTitleOffset(m_tos);
      m_eyCalMean->GetYaxis()->SetTitle(eyTitle.str().c_str()); m_eyCalMean->GetYaxis()->SetTitleOffset(m_tos);
      m_phiCalMean->GetYaxis()->SetTitle(phiTitle.str().c_str()); m_phiCalMean->GetYaxis()->SetTitleOffset(m_tos);
      for (unsigned int i = 0; i < nCalos; i++)
        {
	  m_exCalMean->GetXaxis()->SetBinLabel(i + 1, m_calStrings[i].c_str());
	  m_eyCalMean->GetXaxis()->SetBinLabel(i + 1, m_calStrings[i].c_str());
	  m_phiCalMean->GetXaxis()->SetBinLabel(i + 1, m_calStrings[i].c_str());
        }
      met_summary.regHist(m_exCalMean).ignore();
      met_summary.regHist(m_eyCalMean).ignore();
      met_summary.regHist(m_phiCalMean).ignore();
    }

  if (m_metRegKey != "" && nRegions > 1)
    {
      m_exRegMean = new TProfile("exRegMean", "Means of the Ex Regions", nRegions, 0., nRegions);
      m_eyRegMean = new TProfile("eyRegMean", "Means of the Ey Regions", nRegions, 0., nRegions);
      m_phiRegMean = new TProfile("phiRegMean", "Means of the Phi Regions", nRegions, 0., nRegions);
      m_exRegMean->GetYaxis()->SetTitle(exTitle.str().c_str()); m_exRegMean->GetYaxis()->SetTitleOffset(m_tos);
      m_eyRegMean->GetYaxis()->SetTitle(eyTitle.str().c_str()); m_eyRegMean->GetYaxis()->SetTitleOffset(m_tos);
      m_phiRegMean->GetYaxis()->SetTitle(phiTitle.str().c_str()); m_phiRegMean->GetYaxis()->SetTitleOffset(m_tos);
      for (unsigned int i = 0; i < nRegions; i++)
        {
	  m_exRegMean->GetXaxis()->SetBinLabel(i + 1, m_regStrings[i].c_str());
	  m_eyRegMean->GetXaxis()->SetBinLabel(i + 1, m_regStrings[i].c_str());
	  m_phiRegMean->GetXaxis()->SetBinLabel(i + 1, m_regStrings[i].c_str());
        }
      met_summary.regHist(m_exRegMean).ignore();
      met_summary.regHist(m_eyRegMean).ignore();
      met_summary.regHist(m_phiRegMean).ignore();
    }

  return StatusCode::SUCCESS;
}


StatusCode METMonTool::bookProfileHistograms(std::string& metName, const char* objName, MonGroup& met_mongroup, int* index)
{
  //msg_info// ATH_MSG_INFO("METMonTool::700");
  //msg_info// ATH_MSG_INFO("METMonTool::700");
  //msg_info// ATH_MSG_INFO("METMonTool::700");
  return StatusCode::SUCCESS;

  ATH_MSG_DEBUG("in bookProfileHistograms(" << metName.c_str() << ", " << objName << ")");

  // ************************************ //
  // Missing Et Correlations to Particles //
  // ************************************ //

  std::ostringstream hName;
  std::ostringstream hTitle;
  std::ostringstream hxTitle;
  std::ostringstream hyTitle;
  TProfile* hp = 0;
  TProfile2D* hp2 = 0;

  // MissingET Vs Eta
  hName << "metVs" << objName << "Eta_" << metName.c_str();
  hTitle << "MET Vs " << objName << " Eta Distribution (" << metName.c_str() << ")";
  hxTitle << objName << " Eta";
  hyTitle << "MET Et (GeV)";
  hp = new TProfile(hName.str().c_str(), hTitle.str().c_str(), m_etabin, -5., +5.);
  hp->GetXaxis()->SetTitle(hxTitle.str().c_str());
  hp->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(hp).ignore();
  m_metVsEta.push_back(hp);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  /* // MissingET Parallel Vs Eta
     hName << "metParaVs" << objName << "Eta_" << metName.c_str();
     hTitle << "MET Parallel Vs " << objName << " Eta Distribution (" << metName.c_str() << ")";
     hxTitle << objName << " Eta";
     hyTitle << "MET Parallel (GeV)";
     hp = new TProfile( hName.str().c_str(), hTitle.str().c_str(), m_etabin, -5., +5. );
     hp->GetXaxis()->SetTitle(hxTitle.str().c_str());
     hp->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp->GetYaxis()->SetTitleOffset(m_tos);
     met_mongroup.regHist( hp ).ignore();
     //m_metParaVsEta.push_back( hp );
     hName.str("");
     hTitle.str("");
     hxTitle.str(""); hyTitle.str("");
     // MissingET Perpendicular Vs Eta
     hName << "metPerpVs" << objName << "Eta_" << metName.c_str();
     hTitle << "MET Perpendicular Vs " << objName << " Eta Distribution (" << metName.c_str() << ")";
     hxTitle << objName << " Eta";
     hyTitle << "MET Perpendicular (GeV)";
     hp = new TProfile( hName.str().c_str(), hTitle.str().c_str(), m_etabin, -5., +5. );
     hp->GetXaxis()->SetTitle(hxTitle.str().c_str());
     hp->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp->GetYaxis()->SetTitleOffset(m_tos);
     met_mongroup.regHist( hp ).ignore();
     m_metPerpVsEta.push_back( hp );
     hName.str("");
     hTitle.str("");
     hxTitle.str(""); hyTitle.str("");*/
  // DeltaPhi Vs Eta
  hName << "dphiVs" << objName << "Eta_" << metName.c_str();
  hTitle << "DeltaPhi Vs " << objName << " Eta Distribution (" << metName.c_str() << ")";
  hxTitle << objName << " Eta";
  hyTitle << "deltaPhi (MET, " << objName << ")";
  hp = new TProfile(hName.str().c_str(), hTitle.str().c_str(), m_etabin, -5., +5.);
  hp->GetXaxis()->SetTitle(hxTitle.str().c_str());
  hp->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(hp).ignore();
  m_dphiVsEta.push_back(hp);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  // MissingET Vs Phi
  hName << "metVs" << objName << "Phi_" << metName.c_str();
  hTitle << "MET Vs " << objName << " Phi Distribution (" << metName.c_str() << ")";
  hxTitle << objName << " Phi (radian)";
  hyTitle << "MET Et (GeV)";
  hp = new TProfile(hName.str().c_str(), hTitle.str().c_str(), m_phibin, -m_Pi, +m_Pi);
  hp->GetXaxis()->SetTitle(hxTitle.str().c_str());
  hp->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(hp).ignore();
  m_metVsPhi.push_back(hp);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  /*
  // MissingET Parallel Vs Phi
  hName << "metParaVs" << objName << "Phi_" << metName.c_str();
  hTitle << "MET Parallel Vs " << objName << " Phi Distribution (" << metName.c_str() << ")";
  hxTitle << objName << " Phi (radian)";
  hyTitle << "MET Parallel (GeV)";
  hp = new TProfile( hName.str().c_str(), hTitle.str().c_str(), m_phibin, -m_Pi, +m_Pi );
  hp->GetXaxis()->SetTitle(hxTitle.str().c_str());
  hp->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist( hp ).ignore();
  m_metParaVsPhi.push_back( hp );
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  // MissingET Perpendicular Vs Phi
  hName << "metPerpVs" << objName << "Phi_" << metName.c_str();
  hTitle << "MET Perpendicular Vs " << objName << " Phi Distribution (" << metName.c_str() << ")";
  hxTitle << objName << " Phi (radian)";
  hyTitle << "MET Perpendicular (GeV)";
  hp = new TProfile( hName.str().c_str(), hTitle.str().c_str(), m_phibin, -m_Pi, +m_Pi );
  hp->GetXaxis()->SetTitle(hxTitle.str().c_str());
  hp->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist( hp ).ignore();
  m_metPerpVsPhi.push_back( hp );
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  */
  //DeltaPhi Vs Phi
  hName << "dphiVs" << objName << "Phi_" << metName.c_str();
  hTitle << "DeltaPhi Vs " << objName << " Phi Distribution (" << metName.c_str() << ")";
  hxTitle << objName << " Phi (radian)";
  hyTitle << "deltaPhi (MET, " << objName << ")";
  hp = new TProfile(hName.str().c_str(), hTitle.str().c_str(), m_phibin, -m_Pi, +m_Pi);
  hp->GetXaxis()->SetTitle(hxTitle.str().c_str());
  hp->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(hp).ignore();
  m_dphiVsPhi.push_back(hp);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");
  // MissingET Vs Eta Phi
  hName << "metVs" << objName << "EtaPhi_" << metName.c_str();
  hTitle << "MET Vs " << objName << " EtaPhi Distribution (" << metName.c_str() << ")";
  hxTitle << objName << " Eta";
  hyTitle << objName << " Phi (radian)";
  hp2 = new TProfile2D(hName.str().c_str(), hTitle.str().c_str(), m_etabin, -5., +5., m_phibin, -m_Pi, +m_Pi);
  hp2->GetXaxis()->SetTitle(hxTitle.str().c_str());
  hp2->GetYaxis()->SetTitle(hyTitle.str().c_str()); hp2->GetYaxis()->SetTitleOffset(m_tos);
  met_mongroup.regHist(hp2).ignore();
  m_metVsEtaPhi.push_back(hp2);
  hName.str("");
  hTitle.str("");
  hxTitle.str(""); hyTitle.str("");

  (*index) = m_metVsEtaPhi.size() - 1;

  return StatusCode::SUCCESS;
}


// *********************************************************************
// Fill Histograms
// Fill Histograms
// Fill Histograms
// *********************************************************************

StatusCode METMonTool::fillHistograms()
{
  //msg_info// ATH_MSG_INFO("METMonTool::851");
  //msg_info// ATH_MSG_INFO("METMonTool::851");
  //msg_info// ATH_MSG_INFO("METMonTool::851");

  ATH_MSG_DEBUG("in fillHistograms()");

  const xAOD::EventInfo* thisEventInfo = 0;
  StatusCode sc(evtStore()->retrieve(thisEventInfo));

  if (sc != StatusCode::SUCCESS)
    ATH_MSG_DEBUG("No EventInfo object found! Can't access LAr event info status!");
  else
    {
      if (thisEventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error)
        {
	  return StatusCode::SUCCESS;
        }
    }


  fillSourcesHistograms().ignore();
  if (m_metCalKey != "") fillCalosHistograms().ignore();
  if (m_metRegKey != "") fillRegionsHistograms().ignore();

  ATH_MSG_DEBUG("fillHistograms() ended");
  return StatusCode::SUCCESS;
}


StatusCode METMonTool::fillSourcesHistograms()
{

  // MET > 80 cut
  /*
    const xAOD::MissingETContainer* xMissEt80 = 0;

    if (m_met_cut > 0) {
    m_met_cut_80 = true;
    }

    if (m_met_cut_80) {
    if (evtStore()->contains<xAOD::MissingETContainer>("MET_Reference_AntiKt4LCTopo")) {
    ATH_CHECK(evtStore()->retrieve(xMissEt80, "MET_Reference_AntiKt4LCTopo"));

    // ATH_MSG_WARNING("MONTOOL DOING 80 GeV Cut!!");

    if (!xMissEt80) {
    ATH_MSG_DEBUG("Unable to retrieve MissingETContainer: " << "MET_Reference_AntiKt4LCTopo");
    }
    else {
    float sumet = (*xMissEt80)["FinalClus"]->met() / CLHEP::GeV;
    if (sumet < 80.0) return StatusCode::SUCCESS;
    }
    }
    }
  */



  //msg_info// ATH_MSG_INFO("METMonTool::880");
  //msg_info// ATH_MSG_INFO("METMonTool::880");
  //msg_info// ATH_MSG_INFO("METMonTool::880");

  ATH_MSG_DEBUG("in fillSourcesHistograms()");

  //const xAOD::Jet* xjet = 0;
  const xAOD::JetContainer* xJetCollection = 0;
  if (m_jetColKey != "")
    {
      ATH_CHECK(evtStore()->retrieve(xJetCollection, "AntiKt4LCTopoJets"));
      if (!xJetCollection)
        {
	  ATH_MSG_WARNING("Unable to retrieve JetContainer: " << "AntiKt4LCTopoJets");
	  //return StatusCode::FAILURE;
        }
      else
        {
	  // Get Hardest Jet
	  // Assume sorted collection
	  if (!(xJetCollection->size() > 0) && m_badJets)
            {
	      return StatusCode::SUCCESS;
            }

	  if (xJetCollection->size() > 0)
            {
	      xAOD::JetContainer::const_iterator jetItr = xJetCollection->begin();


	      if (m_badJets)
                {
		  int counterbadjets = 0;

		  xAOD::JetContainer::const_iterator jetItrE = xJetCollection->end();

		  for (; jetItr != jetItrE; ++jetItr)
                    {

		      const xAOD::Jet* xjet = *jetItr;
		      if( m_selTool->keep(*xjet) ) continue; //jetsel tool
		      counterbadjets++;
		
                    }
		  if (counterbadjets == 0)
                    {
		      return StatusCode::SUCCESS;
                    }
                }
            }
	  else
            {
	      ATH_MSG_DEBUG("Empty jet collection");
            }
        }
    }

  const xAOD::ElectronContainer* xElectrons = 0; // evtStore()->retrieve< const xAOD::ElectronContainer >("ElectronCollection");
  const xAOD::Electron* xhEle = 0;

  if (m_eleColKey != "")
    {
      ATH_CHECK(evtStore()->retrieve(xElectrons, "Electrons"));
      if (!xElectrons)
        {
	  ATH_MSG_WARNING("Unable to retrieve ElectronContainer: " << "Electrons");
        }
      else
        {
	  // Get Hardest Electron
	  // Assume sorted collection
	  if (xElectrons->size() > 0)
            {
	      auto eleItr = xElectrons->begin();
                xhEle = *eleItr;
		  }
	  else ATH_MSG_DEBUG("Empty electron collection");
        }
    }

  const xAOD::MuonContainer* xMuons = 0; // evtStore()->retrieve< const xAOD::ElectronContainer >("ElectronCollection");
  const xAOD::Muon* xhMuon = 0;

  if (m_muoColKey != "")
    {
      ATH_CHECK(evtStore()->retrieve(xMuons, "Muons"));
      if (!xMuons)
        {
	  ATH_MSG_WARNING("Unable to retrieve muon collection: " << "Muons");
	  m_ContainerWarnings_Muon++;
        }
      else
        {
	  // Get Hardest Electron
	  // Assume sorted collection
	  if (xMuons->size() > 0)
            {
	      auto muonItr = xMuons->begin();
                xhMuon = *muonItr;
		  }
	  else ATH_MSG_DEBUG("Empty muon collection");
        }
    }

  // *************************** //
  // Retrieve Missing Et Sources //
  // *************************** //

  bool doSummary = (m_metKeys.size() > 1);
  // const MissingET *missET;

  if (m_met_cut_80) {
    if (evtStore()->contains<xAOD::MissingETContainer>("MET_Reference_AntiKt4LCTopo")) {
      const xAOD::MissingETContainer* xMissEt_forCut = 0;
      ATH_CHECK(evtStore()->retrieve(xMissEt_forCut, "MET_Reference_AntiKt4LCTopo"));
      float et_RefFinal = (*xMissEt_forCut)["FinalClus"]->met() / CLHEP::GeV;
      if (et_RefFinal < m_met_cut) return StatusCode::SUCCESS;
    }
  }
    
  if (m_doJetcleaning && !m_badJets)
    {
      if (xJetCollection->size() > 0){
	xAOD::JetContainer::const_iterator jetItrE = xJetCollection->end();
	  
	xAOD::JetContainer::const_iterator jetItr = xJetCollection->begin();
	for (; jetItr != jetItrE; ++jetItr)
	  {
	      
	    const xAOD::Jet* xjet = *jetItr;
	    bool isgoodjet =  m_selTool->keep(*xjet);
	    if(! isgoodjet )  return StatusCode::SUCCESS;
	  }
      }
    }
    


  for (unsigned int i = 0; i < m_metKeys.size(); i++)
    {

      std::string xaod_key = "";
      std::string xaod_subkey = "";
      if (m_metKeys[i].compare("MET_RefFinal") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "FinalClus"; // add track
        }
      else if (m_metKeys[i].compare("MET_RefEle") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "RefEle";
        }
      else if (m_metKeys[i].compare("MET_RefGamma") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "RefGamma";
        }
      else if (m_metKeys[i].compare("MET_RefTau") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "RefTau";
        }

      else if (m_metKeys[i].compare("MET_SoftClus") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "SoftClus";
        }

      else if (m_metKeys[i].compare("MET_SoftTrk") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "FinalTrk";
        }

      else if (m_metKeys[i].compare("MET_PVSoftTrk") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "PVSoftTrk";
        }

      else if (m_metKeys[i].compare("MET_RefJet_JVFCut") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "RefJet"; // RefJet_JVFCut
        }

      else if (m_metKeys[i].compare("MET_RefJet") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "RefJet";
        }
      else if (m_metKeys[i].compare("MET_Muon") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4LCTopo";
	  xaod_subkey = "Muons";
        }
      else if (m_metKeys[i].compare("MET_PFlow_RefFinal") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "FinalClus"; // add track
        }
      else if (m_metKeys[i].compare("MET_PFlow_RefEle") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "RefEle";
        }
      else if (m_metKeys[i].compare("MET_PFlow_RefGamma") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "RefGamma";
        }
      else if (m_metKeys[i].compare("MET_PFlow_RefTau") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "RefTau";
        }

      else if (m_metKeys[i].compare("MET_PFlow_SoftClus") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "SoftClus";
        }

      else if (m_metKeys[i].compare("MET_PFlow_SoftTrk") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "FinalTrk";
        }

      else if (m_metKeys[i].compare("MET_PFlow_PVSoftTrk") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "PVSoftTrk";
        }

      else if (m_metKeys[i].compare("MET_PFlow_RefJet_JVFCut") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "RefJet"; // RefJet_JVFCut
        }

      else if (m_metKeys[i].compare("MET_PFlow_RefJet") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "RefJet";
        }
      else if (m_metKeys[i].compare("MET_PFlow_Muon") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "Muons";
        }
      else if (m_metKeys[i].compare("MET_LocHadTopo") == 0
	       || m_metKeys[i].compare("MET_LocalHadTopo") == 0)
        {
	  xaod_key = "MET_LocHadTopo";
	  xaod_subkey = "LocHadTopo";
        }
      else if (m_metKeys[i].compare("MET_Topo") == 0)
        {
	  xaod_key = "MET_EMTopo";
	  xaod_subkey = "EMTopo";
        }
      else if (m_metKeys[i].compare("MET_Track") == 0)
        {
	  xaod_key = "MET_Track";
	  xaod_subkey = "Track";
        }
      else if (m_metKeys[i].compare("MET_PFlow") == 0)
        {
	  xaod_key = "MET_Reference_AntiKt4EMPFlow";
	  xaod_subkey = "FinalClus";
        }
      else
        {
	  //quiet// ATH_MSG_WARNING("m_metKeys member not recognized: " << m_metKeys[i]);
	  continue;
        }

      const xAOD::MissingETContainer* xMissEt = 0;

      bool sc_exists = evtStore()->contains<xAOD::MissingETContainer>(xaod_key);

      if (sc_exists)
        {
	  ATH_CHECK(evtStore()->retrieve(xMissEt, xaod_key));


	  if (!xMissEt)
            {
	      ATH_MSG_DEBUG("Unable to retrieve MissingETContainer: " << xaod_key);
            }
	  else
            {
	      ATH_MSG_DEBUG("Filling histograms for term " << m_metKeys[i]);
		
	      if ((*xMissEt)[xaod_subkey]) {
                float ex = (*xMissEt)[xaod_subkey]->mpx() / CLHEP::GeV;
                float ey = (*xMissEt)[xaod_subkey]->mpy() / CLHEP::GeV;
                float et = (*xMissEt)[xaod_subkey]->met() / CLHEP::GeV;
                float phi = (*xMissEt)[xaod_subkey]->phi();
                float sumet = (*xMissEt)[xaod_subkey]->sumet() / CLHEP::GeV;

	

	

                if (et > 0.)
		  {
                    m_et[i]->Fill(et);
                    m_ex[i]->Fill(ex);
                    m_ey[i]->Fill(ey);
                    m_phi[i]->Fill(phi);
                    m_sumet[i]->Fill(sumet);

                    if (m_metFinKey != "" && i == m_metKeys.size() - 1)
		      {
                        // met Vs SumEt
                        m_metVsSumEt->Fill(sumet, et);
                        m_metVsMetPhi->Fill(phi, et);
                        // Profile Histograms

                        //msg_info// ATH_MSG_INFO("METMonTool::FILL_ONE_ONE::1092");

                        if (TMath::Abs(et) < m_truncatedMean)
			  {
			    if (xJetCollection != 0)
			      {
			      
				xAOD::JetContainer::const_iterator jetItrE = xJetCollection->end();
			      
				xAOD::JetContainer::const_iterator jetItr = xJetCollection->begin();
				for (; jetItr != jetItrE; ++jetItr)
				  {
				  
				    const xAOD::Jet* xjet = *jetItr;
				    const xAOD::JetFourMom_t& jetP4 = xjet->jetP4();
				    if (xjet != 0) fillProfileHistograms(et, phi, jetP4.eta(), jetP4.phi(), m_iJet).ignore();
				  }
			      }

                            if (xhEle != 0) fillProfileHistograms(et, phi, xhEle->eta(), xhEle->phi(), m_iEle).ignore();
                            if (xhMuon != 0) fillProfileHistograms(et, phi, xhMuon->eta(), xhMuon->phi(), m_iMuo).ignore();
			  }
		      }
                    // Mean summaries
                    if (doSummary)
		      {
                        m_exMean->Fill(i + 0.5, ex);
                        m_eyMean->Fill(i + 0.5, ey);
                        m_phiMean->Fill(i + 0.5, phi);
		      }
		  }
	      }// xaod_subket
            }

        }

      else
        {
	  ATH_MSG_DEBUG("Unable to retrieve MissingETContainer: " << xaod_key);
        }
    }


  return StatusCode::SUCCESS;
}


StatusCode METMonTool::fillCalosHistograms()
{
  //msg_info// ATH_MSG_INFO("METMonTool::1126");
  //msg_info// ATH_MSG_INFO("METMonTool::1126");
  //msg_info// ATH_MSG_INFO("METMonTool::1126");

  ATH_MSG_DEBUG("in fillCalosHistograms()");

  // ******************************** //
  // Retrieve Missing Et Calorimeters //
  // ******************************** //

  bool doSummary = (m_calIndices.size() > 1);

  const xAOD::MissingETContainer* xmetCal = 0;

  bool sc_exists = evtStore()->contains<xAOD::MissingETContainer>("MET_Calo");

  if (sc_exists)
    {
      ATH_CHECK(evtStore()->retrieve(xmetCal, "MET_Calo"));

      if (!xmetCal)
        {
	  ATH_MSG_DEBUG("Unable to retrieve MissingETContainer: " << "MET_Calo");
        }
      else
        {
	  ATH_MSG_DEBUG("Filling histograms per calorimeter subsystem with key " << m_metCalKey);

	  if (m_met_cut_80) {
	    if (evtStore()->contains<xAOD::MissingETContainer>("MET_Reference_AntiKt4LCTopo")) {
	      const xAOD::MissingETContainer* xMissEt_forCut = 0;
	      ATH_CHECK(evtStore()->retrieve(xMissEt_forCut, "MET_Reference_AntiKt4LCTopo"));
	      float et_RefFinal = (*xMissEt_forCut)["FinalClus"]->met() / CLHEP::GeV;

	      if (et_RefFinal < m_met_cut) return StatusCode::SUCCESS;
	    }
	  }

	    
	  if (m_doJetcleaning && !m_badJets)
	    {
	      const xAOD::JetContainer* xJetCollection = 0;
	      ATH_CHECK(evtStore()->retrieve(xJetCollection, "AntiKt4LCTopoJets"));
	      if (!xJetCollection)
		{
		  ATH_MSG_WARNING("Unable to retrieve JetContainer: " << "AntiKt4LCTopoJets");
		  //return StatusCode::FAILURE;
		}
	      if (xJetCollection->size() > 0){
		xAOD::JetContainer::const_iterator jetItrE = xJetCollection->end();
		  
		xAOD::JetContainer::const_iterator jetItr = xJetCollection->begin();
		for (; jetItr != jetItrE; ++jetItr)
		  {
		      
		    const xAOD::Jet* xjet = *jetItr;
		    bool isgoodjet =  m_selTool->keep(*xjet);
		    if(! isgoodjet )  return StatusCode::SUCCESS;
		  }
	      }
	    }
	    
	  for (unsigned int i = 0; i < m_calIndices.size(); i++)
            {

	      float ex = (*xmetCal)[m_calStrings[i]]->mpx() / CLHEP::GeV;
		float ey = (*xmetCal)[m_calStrings[i]]->mpy() / CLHEP::GeV;
		  float et = sqrt(ex*ex + ey*ey);// (*xmetCal)["LocHadTopo"]->met() / CLHEP::GeV;
		  float phi = atan2(ey, ex);//  (*xmetCal)["LocHadTopo"]->phi() / CLHEP::GeV;
		  float sumet = (*xmetCal)[m_calStrings[i]]->sumet() / CLHEP::GeV;
		
		
		  if (et > 0.)
		    {
		      m_etCal[i]->Fill(et);
		      m_exCal[i]->Fill(ex);
		      m_eyCal[i]->Fill(ey);
		      m_phiCal[i]->Fill(phi);
		      m_sumetCal[i]->Fill(sumet);
		      // Mean summaries
		      if (doSummary)
			{
			  m_exCalMean->Fill(i + 0.5, ex);
			  m_eyCalMean->Fill(i + 0.5, ey);
			  m_phiCalMean->Fill(i + 0.5, phi);
			}
		    }
            }
        }
    }
  else
    {
      ATH_MSG_DEBUG("Unable to retrieve MissingETContainer: " << "MET_Calo");
    }
  return StatusCode::SUCCESS;
}

StatusCode METMonTool::fillRegionsHistograms()
{
  //msg_info// ATH_MSG_INFO("METMonTool::1159");
  //msg_info// ATH_MSG_INFO("METMonTool::1159");
  //msg_info// ATH_MSG_INFO("METMonTool::1159");

  ATH_MSG_DEBUG("in fillRegionsHistograms()");

  // *************************** //
  // Retrieve Missing Et Regions //
  // *************************** //

  bool doSummary = (m_regIndices.size() > 1);

  const xAOD::MissingETContainer* xmetReg = 0;

  bool sc_exists = evtStore()->contains<xAOD::MissingETContainer>("MET_TruthRegions");

  if (sc_exists)
    {

      ATH_CHECK(evtStore()->retrieve(xmetReg, "MET_TruthRegions"));

      if (!xmetReg)
        {
	  ATH_MSG_DEBUG("Unable to retrieve MissingETContainer: " << "MET_TruthRegions");
        }
      else
        {
	  //s// ATH_MSG_DEBUG("Filling histograms per calorimeter region with key " << m_metRegKey);
	  //s// metReg = missET->getRegions();
	  for (unsigned int i = 0; i < m_regIndices.size(); i++)
            {
	      std::string xaod_truth_region = "";
	      if (i == 0) xaod_truth_region = "Int_Central";
	      else if (i == 1) xaod_truth_region = "Int_EndCap";
	      else if (i == 2) xaod_truth_region = "Int_Forward";
	      else xaod_truth_region = "Int_Central";

	      float ex = (*xmetReg)[xaod_truth_region]->mpx() / CLHEP::GeV;
                float ey = (*xmetReg)[xaod_truth_region]->mpy() / CLHEP::GeV;
		  float et = sqrt(ex*ex + ey*ey);
		    float phi = atan2(ey, ex);
		      float sumet = (*xmetReg)[xaod_truth_region]->sumet() / CLHEP::GeV;

		      if (et > 0.)
			{
			  m_etReg[i]->Fill(et);
			  m_exReg[i]->Fill(ex);
			  m_eyReg[i]->Fill(ey);
			  m_phiReg[i]->Fill(phi);
			  m_sumetReg[i]->Fill(sumet);
			  // Mean summaries
			  if (doSummary)
			    {
			      m_exRegMean->Fill(i + 0.5, ex);
			      m_eyRegMean->Fill(i + 0.5, ey);
			      m_phiRegMean->Fill(i + 0.5, phi);
			    }
			}
            }
        }
    }
  else
    {
      ATH_MSG_DEBUG("Unable to retrieve MissingETContainer: " << "MET_TruthRegions");
    }

  return StatusCode::SUCCESS;
}


StatusCode METMonTool::fillProfileHistograms(float et, float phi, float objEta, float objPhi, int i)
{
  //msg_info// ATH_MSG_INFO("METMonTool::1205");
  //msg_info// ATH_MSG_INFO("METMonTool::1205");
  //msg_info// ATH_MSG_INFO("METMonTool::1205");

  ATH_MSG_DEBUG("in fillProfileHistograms()");

  float dphi = phi - objPhi;
  if (dphi > +m_Pi) dphi = dphi - 2.*m_Pi;
  if (dphi < -m_Pi) dphi = dphi + 2.*m_Pi;
  dphi = fabs(dphi);
  m_metVsEta[i]->Fill(objEta, et);
  //m_metParaVsEta[i]->Fill( objEta, et*cos(dphi) );
  //m_metPerpVsEta[i]->Fill( objEta, et*sin(dphi) );
  m_dphiVsEta[i]->Fill(objEta, dphi);
  m_metVsPhi[i]->Fill(objPhi, et);
  //m_metParaVsPhi[i]->Fill( objPhi, et*cos(dphi) );
  //m_metPerpVsPhi[i]->Fill( objPhi, et*sin(dphi) );
  m_dphiVsPhi[i]->Fill(objPhi, dphi);
  m_metVsEtaPhi[i]->Fill(objEta, objPhi, et);
  return StatusCode::SUCCESS;
}


// *********************************************************************
// Process Histograms
// *********************************************************************


StatusCode METMonTool::procHistograms()
{
  //msg_info// ATH_MSG_INFO("METMonTool::1236");
  //msg_info// ATH_MSG_INFO("METMonTool::1236");
  //msg_info// ATH_MSG_INFO("METMonTool::1236");

  ATH_MSG_DEBUG("in procHistograms()");

  ATH_MSG_DEBUG("procHistograms() ended");
  return StatusCode::SUCCESS;
}
