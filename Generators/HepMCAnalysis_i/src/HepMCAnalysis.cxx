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
#include "GaudiKernel/Property.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"

#include "HepMC/GenEvent.h"

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

using std::cout;
using std::endl;
using std::string;
using std::vector;

// ---------------------------------------------------------------------- 
HepMCAnalysis::HepMCAnalysis(const string &name, ISvcLocator *pSvcLocator): 
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
void HepMCAnalysis::MeVToGeV(HepMC::GenEvent *event)
{
  for (HepMC::GenEvent::particle_iterator p = event->particles_begin(); p != event->particles_end(); ++p) {
    const HepMC::FourVector fv((*p)->momentum().px() / 1000.0,
			       (*p)->momentum().py() / 1000.0,
			       (*p)->momentum().pz() / 1000.0,
			       (*p)->momentum().e() / 1000.0);
    
    (*p)->set_momentum(fv);
  }
}

// ---------------------------------------------------------------------- 
StatusCode HepMCAnalysis::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  
  if (m_JetFinder) {
    cout << "Adding module JetFinder" << endl;
    m_jetfinder.push_back(new JetFinder);
  }
  
    // create analyses and initialize them
  if (m_JetAnalysis) {
    cout << "Adding module JetAnalysis" << endl;
    m_analysis.push_back(new JetAnalysis);
  }

  if (m_WplusJetAnalysis) {
    cout << "Adding module WplusJetAnalysis" << endl;
    m_analysis.push_back(new WplusJetAnalysis);
  }

  if (m_ZAnalysis) {
    cout << "Adding module ZAnalysis" << endl;
    m_analysis.push_back(new ZAnalysis);
  }
  if (m_ZtautauAnalysis) {
    cout << "Adding module ZtautauAnalysis" << endl;
    m_analysis.push_back(new ZtautauAnalysis);
  }

  if (m_WtaunuAnalysis) {
    cout << "Adding module WtaunuAnalysis" << endl;
    m_analysis.push_back(new WtaunuAnalysis);
  }

  if (m_ttbarAnalysis) {
    cout << "Adding module ttbarAnalysis" << endl;
    m_analysis.push_back(new ttbarAnalysis);
  }

  if (m_bbbarAnalysis) {
    cout << "Adding module bbbarAnalysis" << endl;
    m_analysis.push_back(new bbbarAnalysis);
  }

  if (m_UEAnalysis) {
    cout << "Adding module UEAnalysis" << endl;
    m_analysis.push_back(new UEAnalysis);
  } 
   
  if (m_EtmissAnalysis) {
    cout << "Adding module EtMissAnalysis" << endl;
    m_analysis.push_back(new EtMissAnalysis);
  }

  if (m_ElasScatAnalysis) {
    cout << "Adding module ElasScatAnalysis" << endl;
    m_analysis.push_back(new ElasScatAnalysis);
  }

  if (m_UserAnalysis) {
    cout << "Adding module UserAnalysis" << endl;
    m_analysis.push_back(new UserAnalysis);
  }
  ATH_MSG_DEBUG("Analyses added");

  if (m_LeptonJetAnalysis) {
    cout << "Adding module LeptonJetAnalysis" << endl;
    m_analysis.push_back(new LeptonJetAnalysis);
  }
  ATH_MSG_DEBUG("Analyses added");

  if (m_ParticleContentAnalysis) {
    cout << "Adding module ParticleContentAnalysis" << endl;
    m_analysis.push_back(new ParticleContentAnalysis);
  }
  ATH_MSG_DEBUG("Analyses added");

  if (m_PdfAnalysis) {
    cout << "Adding module PdfAnalysis" << endl;
    m_analysis.push_back(new PdfAnalysis);
  }
  ATH_MSG_DEBUG("Analyses added");


  for (vector<baseAnalysis*>::const_iterator i(m_jetfinder.begin()); i != m_jetfinder.end(); ++i) {
    (*i)->Init(m_max_eta,m_min_pt);
    (*i)->InitJetFinder(m_jet_coneRadius,m_jet_overlapThreshold,m_jet_ptmin, m_lepton_ptmin, m_DeltaR_lepton_track);
  }

  for (vector<baseAnalysis*>::const_iterator i(m_analysis.begin()); i != m_analysis.end(); ++i) {
    (*i)->Init(m_max_eta,m_min_pt);
    (*i)->InitJetFinder(m_jet_coneRadius,m_jet_overlapThreshold,m_jet_ptmin, m_lepton_ptmin, m_DeltaR_lepton_track);
  }

  //TH1::SetDirectory(0) prevent histograms to be added to the list of histiograms of currently opened file (solve conflict with TestHepMC)
  for ( vector< baseAnalysis* >::const_iterator ana = m_analysis.begin(); ana != m_analysis.end(); ++ana ) 
    {
      vector<TH1D*> hists = (*ana)->Histograms();
      for (vector<TH1D* >::iterator h = hists.begin(); h != hists.end(); h++)
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

// ---------------------------------------------------------------------- 
StatusCode HepMCAnalysis::execute()
{  
  ATH_MSG_DEBUG("Executing " << name());

  int runNumber = 0, evtNumber = 0;

  // load event info
  const EventInfo* mcInfoptr;
  if ( evtStore()->retrieve( mcInfoptr, m_infokey ).isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve EventInfo" );
    return StatusCode::FAILURE;
  
  } else{
    runNumber = mcInfoptr->event_ID()->run_number();
    evtNumber = mcInfoptr->event_ID()->event_number();
    ATH_MSG_DEBUG( "run: " << runNumber << " event: " << evtNumber );
  }
  ATH_MSG_DEBUG("Event info loaded");

  // load HepMC info
  const McEventCollection* mcCollptr;
  if ( evtStore()->retrieve( mcCollptr, m_key ).isFailure() ) {
    ATH_MSG_WARNING( "Could not retrieve McEventCollection" );
    return StatusCode::SUCCESS;
  
  } else { 
    ATH_MSG_DEBUG("HepMC info loaded");

    // loop over all events in McEventCollection
    for ( McEventCollection::const_iterator itr = mcCollptr->begin(); itr != mcCollptr->end(); ++itr ) {
      HepMC::GenEvent *evt = (*itr);

      // convert units 
      MeVToGeV( evt );
      
      // call JetFinder
      vector< fastjet::PseudoJet > inclusive_jets;
      for ( vector< baseAnalysis* >::const_iterator i( m_jetfinder.begin() ); i != m_jetfinder.end(); ++i ) {
        inclusive_jets = (*i)->GetJet(evt);
      }
      ATH_MSG_DEBUG( "after jet finder" );

//      int ret_all = true;
      for ( vector< baseAnalysis* >::const_iterator analysis = m_analysis.begin(); analysis != m_analysis.end(); ++analysis ) {
        (*analysis)->SetJet( &inclusive_jets );
//        int ret = 
        (*analysis)->Process( evt );     // call analysis

//        if ( !ret ) {
//          ret_all = false;        
// }
      }
      ATH_MSG_DEBUG("After process function");

      // delete jet object, missing Et etc.   
      for ( vector<baseAnalysis*>::const_iterator i( m_jetfinder.begin() ); i != m_jetfinder.end(); ++i ) {
        (*i)->ClearEvent(evt);
      }

      ATH_MSG_DEBUG("After jetfinder mem cleanup");
    }
  }

  ATH_MSG_DEBUG("Successful execution");
  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------- 
StatusCode HepMCAnalysis::finalize()
{
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  
  StatusCode sc = Algorithm::finalize();
  if (sc.isFailure()) {
    msg(MSG::INFO) << "Error in finalizing of HepMCAnalysis_i" << endreq;
    return sc;
    //return StatusCode::FAILURE;
  }

  // average histograms
  for ( vector< baseAnalysis* >::const_iterator analysis = m_analysis.begin(); analysis != m_analysis.end(); ++analysis ) {
    (*analysis)->averagedHistograms();
  }

  // register histos with histogram service
  TH1D *hist = 0;

  for ( vector< baseAnalysis* >::const_iterator ana = m_analysis.begin(); ana != m_analysis.end(); ++ana ) {
    while ( ( hist =(*ana)->popHisto() ) ) {
      TString histname("/hepmcanalysis/");
      string name = hist->GetName();
      string dir = name.substr(0, name.find("_"));
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
  for ( vector< baseAnalysis* >::const_iterator j = m_jetfinder.begin(); j != m_jetfinder.end(); ++j ) {
    delete (*j);
  }
  m_jetfinder.clear();

  for ( vector< baseAnalysis* >::const_iterator ana = m_analysis.begin(); ana != m_analysis.end(); ++ana ) {
    delete (*ana);
  }
  m_analysis.clear();
  
  ATH_MSG_INFO( "Analyses clean-up done" );

  return StatusCode::SUCCESS;
}

