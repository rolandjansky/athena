///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMCAnalysis.cxx 
// Implementation file for class HepMCAnalysis
// Author: W. Ehrenfeld, S.Johnert
/////////////////////////////////////////////////////////////////// 

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "Gaudi/Property.h"

// ROOT
#include "TFile.h"
#include "TH1.h"
#include "TString.h"

// HepMCAnalysis tool
#include "Configuration.h"
#include "baseAnalysis.h"
#include "JetFinder.h"
#include "ttbarAnalysis.h"
#include "bbbarAnalysis.h"
#include "ZtautauAnalysis.h"
#include "WtaunuAnalysis.h"
#include "JetAnalysis.h"
#include "WplusJetAnalysis.h"
#include "ZAnalysis.h"
#include "UEAnalysis.h"
#include "EtMissAnalysis.h"
#include "ElasScatAnalysis.h"

#include "HepMCAnalysis_i/HepMCAnalysis.h"
#include "HepMCAnalysis_i/UserAnalysis.h"

#include "HepMCAnalysis_i/LeptonJetAnalysis.h"
#include "HepMCAnalysis_i/ParticleContentAnalysis.h"
#include "HepMCAnalysis_i/PdfAnalysis.h"



// ---------------------------------------------------------------------- 
HepMCAnalysis::HepMCAnalysis(const std::string &name, ISvcLocator *pSvcLocator): 
    AthAlgorithm(name, pSvcLocator), m_histSvc("ITHistSvc", name)
{
  declareProperty("McEventKey", m_key = "GEN_EVENT");
  declareProperty("McEventInfoKey", m_infokey = "");

  // analyses 
  declareProperty("JetFinder", m_JetFinder = true);
  declareProperty("JetAnalysis", m_JetAnalysis = false);
  declareProperty("WplusJetAnalysis", m_WplusJetAnalysis = false);
  declareProperty("ZAnalysis", m_ZAnalysis = false);
  declareProperty("ZtautauAnalysis", m_ZtautauAnalysis = false);
  declareProperty("WtaunuAnalysis", m_WtaunuAnalysis = false);
  declareProperty("ttbarAnalysis", m_ttbarAnalysis = false);
  declareProperty("bbbarAnalysis", m_bbbarAnalysis = false);
  declareProperty("UEAnalysis", m_UEAnalysis = false);
  declareProperty("EtmissAnalysis", m_EtmissAnalysis = false);
  declareProperty("ElasScatAnalysis", m_ElasScatAnalysis = false);
  declareProperty("UserAnalysis", m_UserAnalysis = false);

  declareProperty("LeptonJetAnalysis", m_LeptonJetAnalysis = false);
  declareProperty("ParticleContentAnalysis", m_ParticleContentAnalysis = false);
  declareProperty("PdfAnalysis", m_PdfAnalysis = false);

  // analysis variables
  declareProperty("max_eta", m_max_eta = 2.5);
  declareProperty("min_pt", m_min_pt = 0.5);
  declareProperty("jet_coneRadius", m_jet_coneRadius = 0.4);
  declareProperty("jet_overlapThreshold", m_jet_overlapThreshold = 0.75);
  declareProperty("jet_ptmin", m_jet_ptmin = 15.0);
  declareProperty("lepton_ptmin", m_lepton_ptmin = 15.0);
  declareProperty("DeltaR_lepton_track", m_DeltaR_lepton_track = 0.4);
  declareProperty("OutputFile", m_OutputFileName = "defaultOutput.root");
 
  // histo service 
  declareProperty("ITHistSvc", m_histSvc);
}

// ---------------------------------------------------------------------- 
HepMCAnalysis::~HepMCAnalysis()
{

}

// ---------------------------------------------------------------------- 
StatusCode HepMCAnalysis::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  
  if (m_JetFinder) {
    std::cout << "Adding module JetFinder" << std::endl;
    m_jetfinder.push_back(new JetFinder);
  }
  
    // create analyses and initialize them
  if (m_JetAnalysis) {
    std::cout << "Adding module JetAnalysis" << std::endl;
    m_analysis.push_back(new JetAnalysis);
  }

  if (m_WplusJetAnalysis) {
    std::cout << "Adding module WplusJetAnalysis" << std::endl;
    m_analysis.push_back(new WplusJetAnalysis);
  }

  if (m_ZAnalysis) {
    std::cout << "Adding module ZAnalysis" << std::endl;
    m_analysis.push_back(new ZAnalysis);
  }
  if (m_ZtautauAnalysis) {
    std::cout << "Adding module ZtautauAnalysis" << std::endl;
    m_analysis.push_back(new ZtautauAnalysis);
  }

  if (m_WtaunuAnalysis) {
    std::cout << "Adding module WtaunuAnalysis" << std::endl;
    m_analysis.push_back(new WtaunuAnalysis);
  }

  if (m_ttbarAnalysis) {
    std::cout << "Adding module ttbarAnalysis" << std::endl;
    m_analysis.push_back(new ttbarAnalysis);
  }

  if (m_bbbarAnalysis) {
    std::cout << "Adding module bbbarAnalysis" << std::endl;
    m_analysis.push_back(new bbbarAnalysis);
  }

  if (m_UEAnalysis) {
    std::cout << "Adding module UEAnalysis" << std::endl;
    m_analysis.push_back(new UEAnalysis);
  } 
   
  if (m_EtmissAnalysis) {
    std::cout << "Adding module EtMissAnalysis" << std::endl;
    m_analysis.push_back(new EtMissAnalysis);
  }

  if (m_ElasScatAnalysis) {
    std::cout << "Adding module ElasScatAnalysis" << std::endl;
    m_analysis.push_back(new ElasScatAnalysis);
  }

  if (m_UserAnalysis) {
    std::cout << "Adding module UserAnalysis" << std::endl;
    m_analysis.push_back(new UserAnalysis);
  }
  ATH_MSG_DEBUG("Analyses added");

  if (m_LeptonJetAnalysis) {
    std::cout << "Adding module LeptonJetAnalysis" << std::endl;
    m_analysis.push_back(new LeptonJetAnalysis);
  }
  ATH_MSG_DEBUG("Analyses added");

  if (m_ParticleContentAnalysis) {
    std::cout << "Adding module ParticleContentAnalysis" << std::endl;
    m_analysis.push_back(new ParticleContentAnalysis);
  }
  ATH_MSG_DEBUG("Analyses added");

  if (m_PdfAnalysis) {
    std::cout << "Adding module PdfAnalysis" << std::endl;
    m_analysis.push_back(new PdfAnalysis);
  }
  ATH_MSG_DEBUG("Analyses added");


  for (std::vector<baseAnalysis*>::const_iterator i(m_jetfinder.begin()); i != m_jetfinder.end(); ++i) {
    (*i)->Init(m_max_eta,m_min_pt);
    (*i)->InitJetFinder(m_jet_coneRadius,m_jet_overlapThreshold,m_jet_ptmin, m_lepton_ptmin, m_DeltaR_lepton_track);
  }

  for (std::vector<baseAnalysis*>::const_iterator i(m_analysis.begin()); i != m_analysis.end(); ++i) {
    (*i)->Init(m_max_eta,m_min_pt);
    (*i)->InitJetFinder(m_jet_coneRadius,m_jet_overlapThreshold,m_jet_ptmin, m_lepton_ptmin, m_DeltaR_lepton_track);
  }

  //TH1::SetDirectory(0) prevent histograms to be added to the list of histiograms of currently opened file (solve conflict with TestHepMC)
  for ( std::vector< baseAnalysis* >::const_iterator ana = m_analysis.begin(); ana != m_analysis.end(); ++ana ) 
    {
      std::vector<TH1D*> hists = (*ana)->Histograms();
      for (std::vector<TH1D* >::iterator h = hists.begin(); h != hists.end(); h++)
	(*h)->SetDirectory(0);
    }

  ATH_MSG_DEBUG( "Analyses initialized" );

  // get histogram service
  if ( m_histSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve service " << m_histSvc );
    return StatusCode::FAILURE;
  } 
  
  ATH_MSG_DEBUG( "Retrieved service " << m_histSvc );
  ATH_MSG_DEBUG( "Successful initialisation" );
  
  return StatusCode::SUCCESS;
}

StatusCode  GetRunEventNumber(AthAlgorithm*  a,int& runNumber,int& evtNumber,  const std::string im);
StatusCode  GetEvents( AthAlgorithm* a, std::vector<HepMC::GenEvent*>& evts, const std::string km);
// ---------------------------------------------------------------------- 
StatusCode HepMCAnalysis::execute()
{  
  ATH_MSG_DEBUG("Executing " << name());

  int runNumber = 0, evtNumber = 0;

  StatusCode  runeventnumber_statuscode =GetRunEventNumber(this,runNumber,evtNumber,m_infokey);
  if (runeventnumber_statuscode==StatusCode::FAILURE) return StatusCode::FAILURE;
  std::vector<HepMC::GenEvent*> evts;
  StatusCode  events_statuscode=GetEvents(this,evts,m_key);
  if (events_statuscode==StatusCode::FAILURE) return StatusCode::FAILURE;
  for (auto evt: evts) {
      // call JetFinder
      std::vector< fastjet::PseudoJet > inclusive_jets;
      for ( std::vector< baseAnalysis* >::const_iterator i( m_jetfinder.begin() ); i != m_jetfinder.end(); ++i ) {
        inclusive_jets = (*i)->GetJet(evt);
      }
      ATH_MSG_DEBUG( "after jet finder" );
      for ( std::vector< baseAnalysis* >::const_iterator analysis = m_analysis.begin(); analysis != m_analysis.end(); ++analysis ) {
        (*analysis)->SetJet( &inclusive_jets );
        (*analysis)->Process( evt );     // call analysis
      }
      ATH_MSG_DEBUG("After process function");

      // delete jet object, missing Et etc.   
      for ( std::vector<baseAnalysis*>::const_iterator i( m_jetfinder.begin() ); i != m_jetfinder.end(); ++i ) {
        (*i)->ClearEvent(evt);
      }

      ATH_MSG_DEBUG("After jetfinder mem cleanup");
    }
  for (size_t i=0;i<evts.size();i++) delete evts[i];
  ATH_MSG_DEBUG("Successful execution");
  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------- 
StatusCode HepMCAnalysis::finalize()
{
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  
  StatusCode sc = Algorithm::finalize();
  if (sc.isFailure()) {
    msg(MSG::INFO) << "Error in finalizing of HepMCAnalysis_i" << endmsg;
    return sc;
    //return StatusCode::FAILURE;
  }

  // average histograms
  for ( std::vector< baseAnalysis* >::const_iterator analysis = m_analysis.begin(); analysis != m_analysis.end(); ++analysis ) {
    (*analysis)->averagedHistograms();
  }

  // register histos with histogram service
  TH1D *hist = 0;

  for ( std::vector< baseAnalysis* >::const_iterator ana = m_analysis.begin(); ana != m_analysis.end(); ++ana ) {
    while ( ( hist =(*ana)->popHisto() ) ) {
      TString histname("/hepmcanalysis/");
      std::string name = hist->GetName();
      std::string dir = name.substr(0, name.find("_"));
      histname += dir;
      histname += "/";
      histname += hist->GetName();
      ATH_MSG_INFO( histname );

      sc = m_histSvc->regHist( histname.Data(), hist );
      if ( sc.isFailure() ) {
	ATH_MSG_INFO( "Histogram service error" );
	return sc;
      }
    }
  }

  ATH_MSG_INFO( "Histograms registered successfully" );
  
  // clean up analysis modules
  for ( std::vector< baseAnalysis* >::const_iterator j = m_jetfinder.begin(); j != m_jetfinder.end(); ++j ) {
    delete (*j);
  }
  m_jetfinder.clear();

  for ( std::vector< baseAnalysis* >::const_iterator ana = m_analysis.begin(); ana != m_analysis.end(); ++ana ) {
    delete (*ana);
  }
  m_analysis.clear();
  
  ATH_MSG_INFO( "Analyses clean-up done" );

  return StatusCode::SUCCESS;
}

