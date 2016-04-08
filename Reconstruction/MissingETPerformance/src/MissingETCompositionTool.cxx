/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

#include "xAODEventInfo/EventInfo.h"

//#include "CLHEP/Units/PhysicalConstants.h"

#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"

#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"

#include "JetEvent/Jet.h"
#include "egammaEvent/egamma.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

#include "MissingETPerformance/MissingETCompositionTool.h"

#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"

//------------------------------------------------------------------------------
MissingETCompositionTool::MissingETCompositionTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) : AthAlgTool( type, name, parent ) {

  declareInterface<MissingETCompositionTool>( this );

  declareProperty("EventInfoKey",         m_evtInfoKey="EventInfo");
  declareProperty("FolderName",           m_folderName="");

  declareProperty("MakeEventDisplays",    _make_event_displays=false);

  _n_event_displays=0;
}

//------------------------------------------------------------------------------

StatusCode MissingETCompositionTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  sc = CBNT_initialize_jets();
  sc = CBNT_initialize_taus();
  sc = CBNT_initialize_electrons();
  sc = CBNT_initialize_gammas();
  sc = CBNT_initialize_muonboy();
  sc = CBNT_initialize_muid();
  sc = CBNT_initialize_refmuon();
  sc = CBNT_initialize_calocluster();
  sc = CBNT_initialize_eflow();
  sc = CBNT_initialize_tracks();

  //number of each in the map
  h_njets_compmap = new TH1F("h_njets_compmap","njets_compmap",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/njets_compmap",h_njets_compmap);

  h_ntaus_compmap = new TH1F("h_ntaus_compmap","ntaus_compmap",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/ntaus_compmap",h_ntaus_compmap);

  h_neles_compmap = new TH1F("h_neles_compmap","neles_compmap",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/neles_compmap",h_neles_compmap);

  h_ngammas_compmap = new TH1F("h_ngammas_compmap","ngammas_compmap",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/ngammas_compmap",h_ngammas_compmap);

  h_nmuboy_compmap = new TH1F("h_nmuboy_compmap","nmuboy_compmap",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/nmuboy_compmap",h_nmuboy_compmap);

  h_nmuids_compmap = new TH1F("h_nmuids_compmap","nmuids_compmap",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/nmuids_compmap",h_nmuids_compmap);

  h_nrefmuon_compmap = new TH1F("h_nrefmuon_compmap","nrefmuon_compmap",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/nrefmuon_compmap",h_nrefmuon_compmap);

  h_nclus_compmap = new TH1F("h_nclus_compmap","nclus_compmap",500,0,500);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/nclus_compmap",h_nclus_compmap);

  h_eflow_nclus_compmap = new TH1F("h_eflow_nclus_compmap","eflow nclus_compmap",500,0,500);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_nclus_compmap",h_eflow_nclus_compmap);

  h_eflow_ntrks_compmap = new TH1F("h_eflow_ntrks_compmap","eflow_ntrks_compmap",500,0,500);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_ntrks_compmap",h_eflow_ntrks_compmap);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_jets()
{
  StatusCode sc;
  //
  //jets
  //
  h_New_V_Old_RefJet_metx = new TH2F("h_New_V_Old_RefJet_metx","New_V_Old_RefJet_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefJet_metx",h_New_V_Old_RefJet_metx);

  h_New_minus_Old_RefJet_metx = new TH1F("h_New_minus_Old_RefJet_metx","New_minus_Old_RefJet_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefJet_metx",h_New_minus_Old_RefJet_metx);

  h_New_V_Old_RefJet_mety = new TH2F("h_New_V_Old_RefJet_mety","New_V_Old_RefJet_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefJet_mety",h_New_V_Old_RefJet_mety);

  h_New_minus_Old_RefJet_mety = new TH1F("h_New_minus_Old_RefJet_mety","New_minus_Old_RefJet_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefJet_mety",h_New_minus_Old_RefJet_mety);

  h_New_V_Old_RefJet_met = new TH2F("h_New_V_Old_RefJet_met","New_V_Old_RefJet_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefJet_met",h_New_V_Old_RefJet_met);

  h_New_minus_Old_RefJet_met = new TH1F("h_New_minus_Old_RefJet_met","New_minus_Old_RefJet_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefJet_met",h_New_minus_Old_RefJet_met);

  h_New_V_Old_SoftJets_metx = new TH2F("h_New_V_Old_SoftJets_metx","New_V_Old_SoftJets_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_SoftJets_metx",h_New_V_Old_SoftJets_metx);

  h_New_minus_Old_SoftJets_metx = new TH1F("h_New_minus_Old_SoftJets_metx","New_minus_Old_SoftJets_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_SoftJets_metx",h_New_minus_Old_SoftJets_metx);

  h_New_V_Old_SoftJets_mety = new TH2F("h_New_V_Old_SoftJets_mety","New_V_Old_SoftJets_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_SoftJets_mety",h_New_V_Old_SoftJets_mety);

  h_New_minus_Old_SoftJets_mety = new TH1F("h_New_minus_Old_SoftJets_mety","New_minus_Old_SoftJets_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_SoftJets_mety",h_New_minus_Old_SoftJets_mety);

  h_New_V_Old_SoftJets_met = new TH2F("h_New_V_Old_SoftJets_met","New_V_Old_SoftJets_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_SoftJets_met",h_New_V_Old_SoftJets_met);

  h_New_minus_Old_SoftJets_met = new TH1F("h_New_minus_Old_SoftJets_met","New_minus_Old_SoftJets_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_SoftJets_met",h_New_minus_Old_SoftJets_met);

  h_jetweightx_V_jetweighty = new TH2F("h_jetweightx_V_jetweighty","jet weightx vs jet weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx_V_jetweighty",h_jetweightx_V_jetweighty);

  h_jetweightx_V_jetweightet = new TH2F("h_jetweightx_V_jetweightet","jet weightx vs jet weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx_V_jetweightet",h_jetweightx_V_jetweightet);

  h_jetweightx = new TH1F("h_jetweightx","jet weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx",h_jetweightx);

  h_jetweightx_V_jetpx = new TH2F("h_jetweightx_V_jetpx","jet weightx vs jet px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx_V_jetpx",h_jetweightx_V_jetpx);

  h_jetweightx_V_jetpt = new TH2F("h_jetweightx_V_jetpt","jet weightx vs jet pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx_V_jetpt",h_jetweightx_V_jetpt);

  h_jetweightx_V_jeteta = new TH2F("h_jetweightx_V_jeteta","jet weightx vs jet eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx_V_jeteta",h_jetweightx_V_jeteta);

  h_jetweightx_V_jetphi = new TH2F("h_jetweightx_V_jetphi","jet weightx vs jet phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx_V_jetphi",h_jetweightx_V_jetphi);

  h_jetweightx_V_jeteta_V_jetphi = new TH3F("h_jetweightx_V_jeteta_V_jetphi","jet weightx vs jet eta vs jet phi",100,-5.,5.,63,-3.15,3.15 ,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx_V_jeteta_V_jetphi",h_jetweightx_V_jeteta_V_jetphi);

  h_jetweightx_V_jeteta_V_jetpt = new TH3F("h_jetweightx_V_jeteta_V_jetpt","jet weightx vs jet eta vs jet pt",100,-5.,5.,50,0.,1000.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightx_V_jeteta_V_jetpt",h_jetweightx_V_jeteta_V_jetpt);

  h_jetweighty = new TH1F("h_jetweighty","jet weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweighty",h_jetweighty);

  h_jetweighty_V_jetpy = new TH2F("h_jetweighty_V_jetpy","jet weighty vs jet py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweighty_V_jetpy",h_jetweighty_V_jetpy);

  h_jetweighty_V_jetpt = new TH2F("h_jetweighty_V_jetpt","jet weighty vs jet pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweighty_V_jetpt",h_jetweighty_V_jetpt);

  h_jetweighty_V_jeteta = new TH2F("h_jetweighty_V_jeteta","jet weighty vs jet eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweighty_V_jeteta",h_jetweighty_V_jeteta);

  h_jetweighty_V_jetphi = new TH2F("h_jetweighty_V_jetphi","jet weighty vs jet phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweighty_V_jetphi",h_jetweighty_V_jetphi);

  h_jetweighty_V_jeteta_V_jetphi = new TH3F("h_jetweighty_V_jeteta_V_jetphi","jet weighty vs jet eta vs jet phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweighty_V_jeteta_V_jetphi",h_jetweighty_V_jeteta_V_jetphi);

  h_jetweighty_V_jeteta_V_jetpt = new TH3F("h_jetweighty_V_jeteta_V_jetpt","jet weighty vs jet eta vs jet pt",100,-5.,5.,50,0.,1000.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweighty_V_jeteta_V_jetpt",h_jetweighty_V_jeteta_V_jetpt);

  h_jetweightet = new TH1F("h_jetweightet","jet weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightet",h_jetweightet);

  h_jetweightet_V_jetpt = new TH2F("h_jetweightet_V_jetpt","jet weightet vs jet pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightet_V_jetpt",h_jetweightet_V_jetpt);

  h_jetweightet_V_jeteta = new TH2F("h_jetweightet_V_jeteta","jet weightet vs jet eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightet_V_jeteta",h_jetweightet_V_jeteta);

  h_jetweightet_V_jetphi = new TH2F("h_jetweightet_V_jetphi","jet weightet vs jet phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightet_V_jetphi",h_jetweightet_V_jetphi);

  h_jetweightet_V_jeteta_V_jetphi = new TH3F("h_jetweightet_V_jeteta_V_jetphi","jet weightet vs jet eta vs jet phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightet_V_jeteta_V_jetphi",h_jetweightet_V_jeteta_V_jetphi);

  h_jetweightet_V_jeteta_V_jetpt = new TH3F("h_jetweightet_V_jeteta_V_jetpt","jet weightet vs jet eta vs jet pt",100,-5.,5.,50,0.,1000.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/jetweightet_V_jeteta_V_jetpt",h_jetweightet_V_jeteta_V_jetpt);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_jets() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_taus()
{
  StatusCode sc;
  //
  //taus
  //
  h_tauweightx_V_tauweighty = new TH2F("h_tauweightx_V_tauweighty","tau weightx vs tau weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx_V_tauweighty",h_tauweightx_V_tauweighty);

  h_tauweightx_V_tauweightet = new TH2F("h_tauweightx_V_tauweightet","tau weightx vs tau weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx_V_tauweightet",h_tauweightx_V_tauweightet);

  h_New_V_Old_RefTau_metx = new TH2F("h_New_V_Old_RefTau_metx","New_V_Old_RefTau_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefTau_metx",h_New_V_Old_RefTau_metx);

  h_New_minus_Old_RefTau_metx = new TH1F("h_New_minus_Old_RefTau_metx","New_minus_Old_RefTau_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefTau_metx",h_New_minus_Old_RefTau_metx);

  h_New_V_Old_RefTau_mety = new TH2F("h_New_V_Old_RefTau_mety","New_V_Old_RefTau_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefTau_mety",h_New_V_Old_RefTau_mety);

  h_New_minus_Old_RefTau_mety = new TH1F("h_New_minus_Old_RefTau_mety","New_minus_Old_RefTau_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefTau_mety",h_New_minus_Old_RefTau_mety);

  h_New_V_Old_RefTau_met = new TH2F("h_New_V_Old_RefTau_met","New_V_Old_RefTau_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefTau_met",h_New_V_Old_RefTau_met);

  h_New_minus_Old_RefTau_met = new TH1F("h_New_minus_Old_RefTau_met","New_minus_Old_RefTau_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefTau_met",h_New_minus_Old_RefTau_met);

  h_tauweightx = new TH1F("h_tauweightx","tau weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx",h_tauweightx);

  h_tauweightx_V_taupx = new TH2F("h_tauweightx_V_taupx","tau weightx vs tau px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx_V_taupx",h_tauweightx_V_taupx);

  h_tauweightx_V_taupt = new TH2F("h_tauweightx_V_taupt","tau weightx vs tau pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx_V_taupt",h_tauweightx_V_taupt);

  h_tauweightx_V_taueta = new TH2F("h_tauweightx_V_taueta","tau weightx vs tau eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx_V_taueta",h_tauweightx_V_taueta);

  h_tauweightx_V_tauphi = new TH2F("h_tauweightx_V_tauphi","tau weightx vs tau phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx_V_tauphi",h_tauweightx_V_tauphi);

  h_tauweightx_V_taueta_V_tauphi = new TH3F("h_tauweightx_V_taueta_V_tauphi","tau weightx vs tau eta vs tau phi",100,-5.,5.,63,-3.15,3.15 ,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx_V_taueta_V_tauphi",h_tauweightx_V_taueta_V_tauphi);

  h_tauweightx_V_taueta_V_taupt = new TH3F("h_tauweightx_V_taueta_V_taupt","tau weightx vs tau eta vs tau pt",100,-5.,5.,50,0.,1000.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightx_V_taueta_V_taupt",h_tauweightx_V_taueta_V_taupt);

  h_tauweighty = new TH1F("h_tauweighty","tau weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweighty",h_tauweighty);

  h_tauweighty_V_taupy = new TH2F("h_tauweighty_V_taupy","tau weighty vs tau py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweighty_V_taupy",h_tauweighty_V_taupy);

  h_tauweighty_V_taupt = new TH2F("h_tauweighty_V_taupt","tau weighty vs tau pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweighty_V_taupt",h_tauweighty_V_taupt);

  h_tauweighty_V_taueta = new TH2F("h_tauweighty_V_taueta","tau weighty vs tau eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweighty_V_taueta",h_tauweighty_V_taueta);

  h_tauweighty_V_tauphi = new TH2F("h_tauweighty_V_tauphi","tau weighty vs tau phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweighty_V_tauphi",h_tauweighty_V_tauphi);

  h_tauweighty_V_taueta_V_tauphi = new TH3F("h_tauweighty_V_taueta_V_tauphi","tau weighty vs tau eta vs tau phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweighty_V_taueta_V_tauphi",h_tauweighty_V_taueta_V_tauphi);

  h_tauweighty_V_taueta_V_taupt = new TH3F("h_tauweighty_V_taueta_V_taupt","tau weighty vs tau eta vs tau pt",100,-5.,5.,50,0.,1000.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweighty_V_taueta_V_taupt",h_tauweighty_V_taueta_V_taupt);

  h_tauweightet = new TH1F("h_tauweightet","tau weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightet",h_tauweightet);

  h_tauweightet_V_taupt = new TH2F("h_tauweightet_V_taupt","tau weightet vs tau pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightet_V_taupt",h_tauweightet_V_taupt);

  h_tauweightet_V_taueta = new TH2F("h_tauweightet_V_taueta","tau weightet vs tau eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightet_V_taueta",h_tauweightet_V_taueta);

  h_tauweightet_V_tauphi = new TH2F("h_tauweightet_V_tauphi","tau weightet vs tau phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightet_V_tauphi",h_tauweightet_V_tauphi);

  h_tauweightet_V_taueta_V_tauphi = new TH3F("h_tauweightet_V_taueta_V_tauphi","tau weightet vs tau eta vs tau phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightet_V_taueta_V_tauphi",h_tauweightet_V_taueta_V_tauphi);

  h_tauweightet_V_taueta_V_taupt = new TH3F("h_tauweightet_V_taueta_V_taupt","tau weightet vs tau eta vs tau pt",100,-5.,5.,50,0.,1000.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/tauweightet_V_taueta_V_taupt",h_tauweightet_V_taueta_V_taupt);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_taus() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_electrons()
{
  StatusCode sc;
  //
  //electrons
  //
  h_eleweightx_V_eleweighty = new TH2F("h_eleweightx_V_eleweighty","ele weightx vs ele weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx_V_eleweighty",h_eleweightx_V_eleweighty);

  h_eleweightx_V_eleweightet = new TH2F("h_eleweightx_V_eleweightet","ele weightx vs ele weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx_V_eleweightet",h_eleweightx_V_eleweightet);

  h_New_V_Old_RefEle_metx = new TH2F("h_New_V_Old_RefEle_metx","New_V_Old_RefEle_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefEle_metx",h_New_V_Old_RefEle_metx);

  h_New_minus_Old_RefEle_metx = new TH1F("h_New_minus_Old_RefEle_metx","New_minus_Old_RefEle_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefEle_metx",h_New_minus_Old_RefEle_metx);

  h_New_V_Old_RefEle_mety = new TH2F("h_New_V_Old_RefEle_mety","New_V_Old_RefEle_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefEle_mety",h_New_V_Old_RefEle_mety);

  h_New_minus_Old_RefEle_mety = new TH1F("h_New_minus_Old_RefEle_mety","New_minus_Old_RefEle_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefEle_mety",h_New_minus_Old_RefEle_mety);

  h_New_V_Old_RefEle_met = new TH2F("h_New_V_Old_RefEle_met","New_V_Old_RefEle_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefEle_met",h_New_V_Old_RefEle_met);

  h_New_minus_Old_RefEle_met = new TH1F("h_New_minus_Old_RefEle_met","New_minus_Old_RefEle_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefEle_met",h_New_minus_Old_RefEle_met);

  h_eleweightx = new TH1F("h_eleweightx","ele weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx",h_eleweightx);

  h_eleweightx_V_elepx = new TH2F("h_eleweightx_V_elepx","ele weightx vs ele px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx_V_elepx",h_eleweightx_V_elepx);

  h_eleweightx_V_elept = new TH2F("h_eleweightx_V_elept","ele weightx vs ele pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx_V_elept",h_eleweightx_V_elept);

  h_eleweightx_V_eleeta = new TH2F("h_eleweightx_V_eleeta","ele weightx vs ele eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx_V_eleeta",h_eleweightx_V_eleeta);

  h_eleweightx_V_elephi = new TH2F("h_eleweightx_V_elephi","ele weightx vs ele phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx_V_elephi",h_eleweightx_V_elephi);

  h_eleweightx_V_eleeta_V_elephi = new TH3F("h_eleweightx_V_eleeta_V_elephi","ele weightx vs ele eta vs ele phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx_V_eleeta_V_elephi",h_eleweightx_V_eleeta_V_elephi);

  h_eleweightx_V_eleeta_V_elept = new TH3F("h_eleweightx_V_eleeta_V_elept","ele weightx vs ele eta vs ele pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightx_V_eleeta_V_elept",h_eleweightx_V_eleeta_V_elept);

  h_eleweighty = new TH1F("h_eleweighty","ele weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweighty",h_eleweighty);

  h_eleweighty_V_elepy = new TH2F("h_eleweighty_V_elepy","ele weighty vs ele py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweighty_V_elepy",h_eleweighty_V_elepy);

  h_eleweighty_V_elept = new TH2F("h_eleweighty_V_elept","ele weighty vs ele pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweighty_V_elept",h_eleweighty_V_elept);

  h_eleweighty_V_eleeta = new TH2F("h_eleweighty_V_eleeta","ele weighty vs ele eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweighty_V_eleeta",h_eleweighty_V_eleeta);

  h_eleweighty_V_elephi = new TH2F("h_eleweighty_V_elephi","ele weighty vs ele phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweighty_V_elephi",h_eleweighty_V_elephi);

  h_eleweighty_V_eleeta_V_elephi = new TH3F("h_eleweighty_V_eleeta_V_elephi","ele weighty vs ele eta vs ele phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweighty_V_eleeta_V_elephi",h_eleweighty_V_eleeta_V_elephi);

  h_eleweighty_V_eleeta_V_elept = new TH3F("h_eleweighty_V_eleeta_V_elept","ele weighty vs ele eta vs ele pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweighty_V_eleeta_V_elept",h_eleweighty_V_eleeta_V_elept);

  h_eleweightet = new TH1F("h_eleweightet","ele weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightet",h_eleweightet);

  h_eleweightet_V_elept = new TH2F("h_eleweightet_V_elept","ele weightet vs ele pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightet_V_elept",h_eleweightet_V_elept);

  h_eleweightet_V_eleeta = new TH2F("h_eleweightet_V_eleeta","ele weightet vs ele eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightet_V_eleeta",h_eleweightet_V_eleeta);

  h_eleweightet_V_elephi = new TH2F("h_eleweightet_V_elephi","ele weightet vs ele phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightet_V_elephi",h_eleweightet_V_elephi);

  h_eleweightet_V_eleeta_V_elephi = new TH3F("h_eleweightet_V_eleeta_V_elephi","ele weightet vs ele eta vs ele phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightet_V_eleeta_V_elephi",h_eleweightet_V_eleeta_V_elephi);

  h_eleweightet_V_eleeta_V_elept = new TH3F("h_eleweightet_V_eleeta_V_elept","ele weightet vs ele eta vs ele pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eleweightet_V_eleeta_V_elept",h_eleweightet_V_eleeta_V_elept);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_electrons() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_gammas()
{
  StatusCode sc;
  //
  //gammas
  //

  h_gammaweightx_V_gammaweighty = new TH2F("h_gammaweightx_V_gammaweighty","gamma weightx vs gamma weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx_V_gammaweighty",h_gammaweightx_V_gammaweighty);

  h_gammaweightx_V_gammaweightet = new TH2F("h_gammaweightx_V_gammaweightet","gamma weightx vs gamma weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx_V_gammaweightet",h_gammaweightx_V_gammaweightet);

  h_New_V_Old_RefGamma_metx = new TH2F("h_New_V_Old_RefGamma_metx","New_V_Old_RefGamma_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefGamma_metx",h_New_V_Old_RefGamma_metx);

  h_New_minus_Old_RefGamma_metx = new TH1F("h_New_minus_Old_RefGamma_metx","New_minus_Old_RefGamma_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefGamma_metx",h_New_minus_Old_RefGamma_metx);

  h_New_V_Old_RefGamma_mety = new TH2F("h_New_V_Old_RefGamma_mety","New_V_Old_RefGamma_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefGamma_mety",h_New_V_Old_RefGamma_mety);

  h_New_minus_Old_RefGamma_mety = new TH1F("h_New_minus_Old_RefGamma_mety","New_minus_Old_RefGamma_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefGamma_mety",h_New_minus_Old_RefGamma_mety);

  h_New_V_Old_RefGamma_met = new TH2F("h_New_V_Old_RefGamma_met","New_V_Old_RefGamma_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_RefGamma_met",h_New_V_Old_RefGamma_met);

  h_New_minus_Old_RefGamma_met = new TH1F("h_New_minus_Old_RefGamma_met","New_minus_Old_RefGamma_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_RefGamma_met",h_New_minus_Old_RefGamma_met);

  h_gammaweightx = new TH1F("h_gammaweightx","gamma weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx",h_gammaweightx);

  h_gammaweightx_V_gammapx = new TH2F("h_gammaweightx_V_gammapx","gamma weightx vs gamma px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx_V_gammapx",h_gammaweightx_V_gammapx);

  h_gammaweightx_V_gammapt = new TH2F("h_gammaweightx_V_gammapt","gamma weightx vs gamma pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx_V_gammapt",h_gammaweightx_V_gammapt);

  h_gammaweightx_V_gammaeta = new TH2F("h_gammaweightx_V_gammaeta","gamma weightx vs gamma eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx_V_gammaeta",h_gammaweightx_V_gammaeta);

  h_gammaweightx_V_gammaphi = new TH2F("h_gammaweightx_V_gammaphi","gamma weightx vs gamma phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx_V_gammaphi",h_gammaweightx_V_gammaphi);

  h_gammaweightx_V_gammaeta_V_gammaphi = new TH3F("h_gammaweightx_V_gammaeta_V_gammaphi","gamma weightx vs gamma eta vs gamma phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx_V_gammaeta_V_gammaphi",h_gammaweightx_V_gammaeta_V_gammaphi);

  h_gammaweightx_V_gammaeta_V_gammapt = new TH3F("h_gammaweightx_V_gammaeta_V_gammapt","gamma weightx vs gamma eta vs gamma pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightx_V_gammaeta_V_gammapt",h_gammaweightx_V_gammaeta_V_gammapt);

  h_gammaweighty = new TH1F("h_gammaweighty","gamma weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweighty",h_gammaweighty);

  h_gammaweighty_V_gammapy = new TH2F("h_gammaweighty_V_gammapy","gamma weighty vs gamma py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweighty_V_gammapy",h_gammaweighty_V_gammapy);

  h_gammaweighty_V_gammapt = new TH2F("h_gammaweighty_V_gammapt","gamma weighty vs gamma pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweighty_V_gammapt",h_gammaweighty_V_gammapt);

  h_gammaweighty_V_gammaeta = new TH2F("h_gammaweighty_V_gammaeta","gamma weighty vs gamma eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweighty_V_gammaeta",h_gammaweighty_V_gammaeta);

  h_gammaweighty_V_gammaphi = new TH2F("h_gammaweighty_V_gammaphi","gamma weighty vs gamma phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweighty_V_gammaphi",h_gammaweighty_V_gammaphi);

  h_gammaweighty_V_gammaeta_V_gammaphi = new TH3F("h_gammaweighty_V_gammaeta_V_gammaphi","gamma weighty vs gamma eta vs gamma phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweighty_V_gammaeta_V_gammaphi",h_gammaweighty_V_gammaeta_V_gammaphi);

  h_gammaweighty_V_gammaeta_V_gammapt = new TH3F("h_gammaweighty_V_gammaeta_V_gammapt","gamma weighty vs gamma eta vs gamma pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweighty_V_gammaeta_V_gammapt",h_gammaweighty_V_gammaeta_V_gammapt);

  h_gammaweightet = new TH1F("h_gammaweightet","gamma weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightet",h_gammaweightet);

  h_gammaweightet_V_gammapt = new TH2F("h_gammaweightet_V_gammapt","gamma weightet vs gamma pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightet_V_gammapt",h_gammaweightet_V_gammapt);

  h_gammaweightet_V_gammaeta = new TH2F("h_gammaweightet_V_gammaeta","gamma weightet vs gamma eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightet_V_gammaeta",h_gammaweightet_V_gammaeta);

  h_gammaweightet_V_gammaphi = new TH2F("h_gammaweightet_V_gammaphi","gamma weightet vs gamma phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightet_V_gammaphi",h_gammaweightet_V_gammaphi);

  h_gammaweightet_V_gammaeta_V_gammaphi = new TH3F("h_gammaweightet_V_gammaeta_V_gammaphi","gamma weightet vs gamma eta vs gamma phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightet_V_gammaeta_V_gammaphi",h_gammaweightet_V_gammaeta_V_gammaphi);

  h_gammaweightet_V_gammaeta_V_gammapt = new TH3F("h_gammaweightet_V_gammaeta_V_gammapt","gamma weightet vs gamma eta vs gamma pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/gammaweightet_V_gammaeta_V_gammapt",h_gammaweightet_V_gammaeta_V_gammapt);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_gammas() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_muonboy()
{
  StatusCode sc;
  //
  //muonboy muons
  //
  h_muonweightx_V_muonweighty = new TH2F("h_muonweightx_V_muonweighty","muon weightx vs muon weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx_V_muonweighty",h_muonweightx_V_muonweighty);

  h_muonweightx_V_muonweightet = new TH2F("h_muonweightx_V_muonweightet","muon weightx vs muon weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx_V_muonweightet",h_muonweightx_V_muonweightet);

  h_New_V_Old_MuonBoy_metx = new TH2F("h_New_V_Old_MuonBoy_metx","New_V_Old_MuonBoy_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_MuonBoy_metx",h_New_V_Old_MuonBoy_metx);

  h_New_minus_Old_MuonBoy_metx = new TH1F("h_New_minus_Old_MuonBoy_metx","New_minus_Old_MuonBoy_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_MuonBoy_metx",h_New_minus_Old_MuonBoy_metx);

  h_New_V_Old_MuonBoy_mety = new TH2F("h_New_V_Old_MuonBoy_mety","New_V_Old_MuonBoy_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_MuonBoy_mety",h_New_V_Old_MuonBoy_mety);

  h_New_minus_Old_MuonBoy_mety = new TH1F("h_New_minus_Old_MuonBoy_mety","New_minus_Old_MuonBoy_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_MuonBoy_mety",h_New_minus_Old_MuonBoy_mety);

  h_New_V_Old_MuonBoy_met = new TH2F("h_New_V_Old_MuonBoy_met","New_V_Old_MuonBoy_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_MuonBoy_met",h_New_V_Old_MuonBoy_met);

  h_New_minus_Old_MuonBoy_met = new TH1F("h_New_minus_Old_MuonBoy_met","New_minus_Old_MuonBoy_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_MuonBoy_met",h_New_minus_Old_MuonBoy_met);

  h_muonweightx = new TH1F("h_muonweightx","muon weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx",h_muonweightx);

  h_muonweightx_V_muonpx = new TH2F("h_muonweightx_V_muonpx","muon weightx vs muon px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx_V_muonpx",h_muonweightx_V_muonpx);

  h_muonweightx_V_muonpt = new TH2F("h_muonweightx_V_muonpt","muon weightx vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx_V_muonpt",h_muonweightx_V_muonpt);

  h_muonweightx_V_muoneta = new TH2F("h_muonweightx_V_muoneta","muon weightx vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx_V_muoneta",h_muonweightx_V_muoneta);

  h_muonweightx_V_muonphi = new TH2F("h_muonweightx_V_muonphi","muon weightx vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx_V_muonphi",h_muonweightx_V_muonphi);

  h_muonweightx_V_muoneta_V_muonphi = new TH3F("h_muonweightx_V_muoneta_V_muonphi","muon weightx vs muon eta vs muon phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx_V_muoneta_V_muonphi",h_muonweightx_V_muoneta_V_muonphi);

  h_muonweightx_V_muoneta_V_muonpt = new TH3F("h_muonweightx_V_muoneta_V_muonpt","muon weightx vs muon eta vs muon pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightx_V_muoneta_V_muonpt",h_muonweightx_V_muoneta_V_muonpt);

  h_muonweighty = new TH1F("h_muonweighty","muon weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweighty",h_muonweighty);

  h_muonweighty_V_muonpy = new TH2F("h_muonweighty_V_muonpy","muon weighty vs muon py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweighty_V_muonpy",h_muonweighty_V_muonpy);

  h_muonweighty_V_muonpt = new TH2F("h_muonweighty_V_muonpt","muon weighty vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweighty_V_muonpt",h_muonweighty_V_muonpt);

  h_muonweighty_V_muoneta = new TH2F("h_muonweighty_V_muoneta","muon weighty vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweighty_V_muoneta",h_muonweighty_V_muoneta);

  h_muonweighty_V_muonphi = new TH2F("h_muonweighty_V_muonphi","muon weighty vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweighty_V_muonphi",h_muonweighty_V_muonphi);

  h_muonweighty_V_muoneta_V_muonphi = new TH3F("h_muonweighty_V_muoneta_V_muonphi","muon weighty vs muon eta vs muon phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweighty_V_muoneta_V_muonphi",h_muonweighty_V_muoneta_V_muonphi);

  h_muonweighty_V_muoneta_V_muonpt = new TH3F("h_muonweighty_V_muoneta_V_muonpt","muon weighty vs muon eta vs muon pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweighty_V_muoneta_V_muonpt",h_muonweighty_V_muoneta_V_muonpt);

  h_muonweightet = new TH1F("h_muonweightet","muon weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightet",h_muonweightet);

  h_muonweightet_V_muonpt = new TH2F("h_muonweightet_V_muonpt","muon weightet vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightet_V_muonpt",h_muonweightet_V_muonpt);

  h_muonweightet_V_muoneta = new TH2F("h_muonweightet_V_muoneta","muon weightet vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightet_V_muoneta",h_muonweightet_V_muoneta);

  h_muonweightet_V_muonphi = new TH2F("h_muonweightet_V_muonphi","muon weightet vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightet_V_muonphi",h_muonweightet_V_muonphi);

  h_muonweightet_V_muoneta_V_muonphi = new TH3F("h_muonweightet_V_muoneta_V_muonphi","muon weightet vs muon eta vs muon phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightet_V_muoneta_V_muonphi",h_muonweightet_V_muoneta_V_muonphi);

  h_muonweightet_V_muoneta_V_muonpt = new TH3F("h_muonweightet_V_muoneta_V_muonpt","muon weightet vs muon eta vs muon pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muonweightet_V_muoneta_V_muonpt",h_muonweightet_V_muoneta_V_muonpt);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_muonboy() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_muid()
{
  StatusCode sc;
  //
  //muid muons
  //
  h_muidweightx_V_muidweighty = new TH2F("h_muidweightx_V_muidweighty","muon weightx vs muon weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx_V_muidweighty",h_muidweightx_V_muidweighty);

  h_muidweightx_V_muidweightet = new TH2F("h_muidweightx_V_muidweightet","muon weightx vs muon weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx_V_muidweightet",h_muidweightx_V_muidweightet);

  h_New_V_Old_MuID_metx = new TH2F("h_New_V_Old_MuID_metx","New_V_Old_MuID_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_MuID_metx",h_New_V_Old_MuID_metx);

  h_New_minus_Old_MuID_metx = new TH1F("h_New_minus_Old_MuID_metx","New_minus_Old_MuID_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_MuID_metx",h_New_minus_Old_MuID_metx);

  h_New_V_Old_MuID_mety = new TH2F("h_New_V_Old_MuID_mety","New_V_Old_MuID_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_MuID_mety",h_New_V_Old_MuID_mety);

  h_New_minus_Old_MuID_mety = new TH1F("h_New_minus_Old_MuID_mety","New_minus_Old_MuID_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_MuID_mety",h_New_minus_Old_MuID_mety);

  h_New_V_Old_MuID_met = new TH2F("h_New_V_Old_MuID_met","New_V_Old_MuID_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_MuID_met",h_New_V_Old_MuID_met);

  h_New_minus_Old_MuID_met = new TH1F("h_New_minus_Old_MuID_met","New_minus_Old_MuID_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_MuID_met",h_New_minus_Old_MuID_met);

  h_muidweightx = new TH1F("h_muidweightx","muon weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx",h_muidweightx);

  h_muidweightx_V_muonpx = new TH2F("h_muidweightx_V_muonpx","muon weightx vs muon px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx_V_muonpx",h_muidweightx_V_muonpx);

  h_muidweightx_V_muonpt = new TH2F("h_muidweightx_V_muonpt","muon weightx vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx_V_muonpt",h_muidweightx_V_muonpt);

  h_muidweightx_V_muoneta = new TH2F("h_muidweightx_V_muoneta","muon weightx vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx_V_muoneta",h_muidweightx_V_muoneta);

  h_muidweightx_V_muonphi = new TH2F("h_muidweightx_V_muonphi","muon weightx vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx_V_muonphi",h_muidweightx_V_muonphi);

  h_muidweightx_V_muoneta_V_muonphi = new TH3F("h_muidweightx_V_muoneta_V_muonphi","muon weightx vs muon eta vs muon phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx_V_muoneta_V_muonphi",h_muidweightx_V_muoneta_V_muonphi);

  h_muidweightx_V_muoneta_V_muonpt = new TH3F("h_muidweightx_V_muoneta_V_muonpt","muon weightx vs muon eta vs muon pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightx_V_muoneta_V_muonpt",h_muidweightx_V_muoneta_V_muonpt);

  h_muidweighty = new TH1F("h_muidweighty","muon weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweighty",h_muidweighty);

  h_muidweighty_V_muonpy = new TH2F("h_muidweighty_V_muonpy","muon weighty vs muon py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweighty_V_muonpy",h_muidweighty_V_muonpy);

  h_muidweighty_V_muonpt = new TH2F("h_muidweighty_V_muonpt","muon weighty vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweighty_V_muonpt",h_muidweighty_V_muonpt);

  h_muidweighty_V_muoneta = new TH2F("h_muidweighty_V_muoneta","muon weighty vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweighty_V_muoneta",h_muidweighty_V_muoneta);

  h_muidweighty_V_muonphi = new TH2F("h_muidweighty_V_muonphi","muon weighty vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweighty_V_muonphi",h_muidweighty_V_muonphi);

  h_muidweighty_V_muoneta_V_muonphi = new TH3F("h_muidweighty_V_muoneta_V_muonphi","muon weighty vs muon eta vs muon phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweighty_V_muoneta_V_muonphi",h_muidweighty_V_muoneta_V_muonphi);

  h_muidweighty_V_muoneta_V_muonpt = new TH3F("h_muidweighty_V_muoneta_V_muonpt","muon weighty vs muon eta vs muon pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweighty_V_muoneta_V_muonpt",h_muidweighty_V_muoneta_V_muonpt);

  h_muidweightet = new TH1F("h_muidweightet","muon weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightet",h_muidweightet);

  h_muidweightet_V_muonpt = new TH2F("h_muidweightet_V_muonpt","muon weightet vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightet_V_muonpt",h_muidweightet_V_muonpt);

  h_muidweightet_V_muoneta = new TH2F("h_muidweightet_V_muoneta","muon weightet vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightet_V_muoneta",h_muidweightet_V_muoneta);

  h_muidweightet_V_muonphi = new TH2F("h_muidweightet_V_muonphi","muon weightet vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightet_V_muonphi",h_muidweightet_V_muonphi);

  h_muidweightet_V_muoneta_V_muonphi = new TH3F("h_muidweightet_V_muoneta_V_muonphi","muon weightet vs muon eta vs muon phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightet_V_muoneta_V_muonphi",h_muidweightet_V_muoneta_V_muonphi);

  h_muidweightet_V_muoneta_V_muonpt = new TH3F("h_muidweightet_V_muoneta_V_muonpt","muon weightet vs muon eta vs muon pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/muidweightet_V_muoneta_V_muonpt",h_muidweightet_V_muoneta_V_muonpt);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_muid() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_refmuon()
{
  StatusCode sc;
  //
  //ref muons (energy in calo)
  //
  h_refmuonweightx_V_refmuonweighty = new TH2F("h_refmuonweightx_V_refmuonweighty","muon weightx vs muon weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx_V_refmuonweighty",h_refmuonweightx_V_refmuonweighty);

  h_refmuonweightx_V_refmuonweightet = new TH2F("h_refmuonweightx_V_refmuonweightet","muon weightx vs muon weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx_V_refmuonweightet",h_refmuonweightx_V_refmuonweightet);

  h_New_V_Old_refMuon_metx = new TH2F("h_New_V_Old_refMuon_metx","New_V_Old_refMuon_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_refMuon_metx",h_New_V_Old_refMuon_metx);

  h_New_minus_Old_refMuon_metx = new TH1F("h_New_minus_Old_refMuon_metx","New_minus_Old_refMuon_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_refMuon_metx",h_New_minus_Old_refMuon_metx);

  h_New_V_Old_refMuon_mety = new TH2F("h_New_V_Old_refMuon_mety","New_V_Old_refMuon_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_refMuon_mety",h_New_V_Old_refMuon_mety);

  h_New_minus_Old_refMuon_mety = new TH1F("h_New_minus_Old_refMuon_mety","New_minus_Old_refMuon_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_refMuon_mety",h_New_minus_Old_refMuon_mety);

  h_New_V_Old_refMuon_met = new TH2F("h_New_V_Old_refMuon_met","New_V_Old_refMuon_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_refMuon_met",h_New_V_Old_refMuon_met);

  h_New_minus_Old_refMuon_met = new TH1F("h_New_minus_Old_refMuon_met","New_minus_Old_refMuon_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_refMuon_met",h_New_minus_Old_refMuon_met);

  h_refmuonweightx = new TH1F("h_refmuonweightx","muon weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx",h_refmuonweightx);

  h_refmuonweightx_V_muonpx = new TH2F("h_refmuonweightx_V_muonpx","muon weightx vs muon px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx_V_muonpx",h_refmuonweightx_V_muonpx);

  h_refmuonweightx_V_muonpt = new TH2F("h_refmuonweightx_V_muonpt","muon weightx vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx_V_muonpt",h_refmuonweightx_V_muonpt);

  h_refmuonweightx_V_muoneta = new TH2F("h_refmuonweightx_V_muoneta","muon weightx vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx_V_muoneta",h_refmuonweightx_V_muoneta);

  h_refmuonweightx_V_muonphi = new TH2F("h_refmuonweightx_V_muonphi","muon weightx vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx_V_muonphi",h_refmuonweightx_V_muonphi);

  h_refmuonweightx_V_muoneta_V_muonphi = new TH3F("h_refmuonweightx_V_muoneta_V_muonphi","muon weightx vs muon eta vs muon phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx_V_muoneta_V_muonphi",h_refmuonweightx_V_muoneta_V_muonphi);

  h_refmuonweightx_V_muoneta_V_muonpt = new TH3F("h_refmuonweightx_V_muoneta_V_muonpt","muon weightx vs muon eta vs muon pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightx_V_muoneta_V_muonpt",h_refmuonweightx_V_muoneta_V_muonpt);

  h_refmuonweighty = new TH1F("h_refmuonweighty","muon weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweighty",h_refmuonweighty);

  h_refmuonweighty_V_muonpy = new TH2F("h_refmuonweighty_V_muonpy","muon weighty vs muon py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweighty_V_muonpy",h_refmuonweighty_V_muonpy);

  h_refmuonweighty_V_muonpt = new TH2F("h_refmuonweighty_V_muonpt","muon weighty vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweighty_V_muonpt",h_refmuonweighty_V_muonpt);

  h_refmuonweighty_V_muoneta = new TH2F("h_refmuonweighty_V_muoneta","muon weighty vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweighty_V_muoneta",h_refmuonweighty_V_muoneta);

  h_refmuonweighty_V_muonphi = new TH2F("h_refmuonweighty_V_muonphi","muon weighty vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweighty_V_muonphi",h_refmuonweighty_V_muonphi);

  h_refmuonweighty_V_muoneta_V_muonphi = new TH3F("h_refmuonweighty_V_muoneta_V_muonphi","muon weighty vs muon eta vs muon phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweighty_V_muoneta_V_muonphi",h_refmuonweighty_V_muoneta_V_muonphi);

  h_refmuonweighty_V_muoneta_V_muonpt = new TH3F("h_refmuonweighty_V_muoneta_V_muonpt","muon weighty vs muon eta vs muon pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweighty_V_muoneta_V_muonpt",h_refmuonweighty_V_muoneta_V_muonpt);

  h_refmuonweightet = new TH1F("h_refmuonweightet","muon weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightet",h_refmuonweightet);

  h_refmuonweightet_V_muonpt = new TH2F("h_refmuonweightet_V_muonpt","muon weightet vs muon pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightet_V_muonpt",h_refmuonweightet_V_muonpt);

  h_refmuonweightet_V_muoneta = new TH2F("h_refmuonweightet_V_muoneta","muon weightet vs muon eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightet_V_muoneta",h_refmuonweightet_V_muoneta);

  h_refmuonweightet_V_muonphi = new TH2F("h_refmuonweightet_V_muonphi","muon weightet vs muon phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/refmuonweightet_V_muonphi",h_refmuonweightet_V_muonphi);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_refmuon() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_calocluster()
{
  StatusCode sc;
  //
  //CaloClusters (MET_CellOut)
  //
  h_clusweightx_V_clusweighty = new TH2F("h_clusweightx_V_clusweighty","clus weightx vs clus weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx_V_clusweighty",h_clusweightx_V_clusweighty);

  h_clusweightx_V_clusweightet = new TH2F("h_clusweightx_V_clusweightet","clus weightx vs clus weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx_V_clusweightet",h_clusweightx_V_clusweightet);

  h_New_V_Old_CellOut_metx = new TH2F("h_New_V_Old_CellOut_metx","New_V_Old_CellOut_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_CellOut_metx",h_New_V_Old_CellOut_metx);

  h_New_minus_Old_CellOut_metx = new TH1F("h_New_minus_Old_CellOut_metx","New_minus_Old_CellOut_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_CellOut_metx",h_New_minus_Old_CellOut_metx);

  h_New_V_Old_CellOut_mety = new TH2F("h_New_V_Old_CellOut_mety","New_V_Old_CellOut_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_CellOut_mety",h_New_V_Old_CellOut_mety);

  h_New_minus_Old_CellOut_mety = new TH1F("h_New_minus_Old_CellOut_mety","New_minus_Old_CellOut_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_CellOut_mety",h_New_minus_Old_CellOut_mety);

  h_New_V_Old_CellOut_met = new TH2F("h_New_V_Old_CellOut_met","New_V_Old_CellOut_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_CellOut_met",h_New_V_Old_CellOut_met);

  h_New_minus_Old_CellOut_met = new TH1F("h_New_minus_Old_CellOut_met","New_minus_Old_CellOut_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_CellOut_met",h_New_minus_Old_CellOut_met);

  h_clusweightx = new TH1F("h_clusweightx","clus weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx",h_clusweightx);

  h_clusweightx_V_cluspx = new TH2F("h_clusweightx_V_cluspx","clus weightx vs clus px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx_V_cluspx",h_clusweightx_V_cluspx);

  h_clusweightx_V_cluspt = new TH2F("h_clusweightx_V_cluspt","clus weightx vs clus pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx_V_cluspt",h_clusweightx_V_cluspt);

  h_clusweightx_V_cluseta = new TH2F("h_clusweightx_V_cluseta","clus weightx vs clus eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx_V_cluseta",h_clusweightx_V_cluseta);

  h_clusweightx_V_clusphi = new TH2F("h_clusweightx_V_clusphi","clus weightx vs clus phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx_V_clusphi",h_clusweightx_V_clusphi);

  h_clusweightx_V_cluseta_V_clusphi = new TH3F("h_clusweightx_V_cluseta_V_clusphi","clus weightx vs clus eta vs clus phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx_V_cluseta_V_clusphi",h_clusweightx_V_cluseta_V_clusphi);

  h_clusweightx_V_cluseta_V_cluspt = new TH3F("h_clusweightx_V_cluseta_V_cluspt","clus weightx vs clus eta vs clus pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightx_V_cluseta_V_cluspt",h_clusweightx_V_cluseta_V_cluspt);

  h_clusweighty = new TH1F("h_clusweighty","clus weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweighty",h_clusweighty);

  h_clusweighty_V_cluspy = new TH2F("h_clusweighty_V_cluspy","clus weighty vs clus py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweighty_V_cluspy",h_clusweighty_V_cluspy);

  h_clusweighty_V_cluspt = new TH2F("h_clusweighty_V_cluspt","clus weighty vs clus pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweighty_V_cluspt",h_clusweighty_V_cluspt);

  h_clusweighty_V_cluseta = new TH2F("h_clusweighty_V_cluseta","clus weighty vs clus eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweighty_V_cluseta",h_clusweighty_V_cluseta);

  h_clusweighty_V_clusphi = new TH2F("h_clusweighty_V_clusphi","clus weighty vs clus phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweighty_V_clusphi",h_clusweighty_V_clusphi);

  h_clusweighty_V_cluseta_V_clusphi = new TH3F("h_clusweighty_V_cluseta_V_clusphi","clus weighty vs clus eta vs clus phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweighty_V_cluseta_V_clusphi",h_clusweighty_V_cluseta_V_clusphi);

  h_clusweighty_V_cluseta_V_cluspt = new TH3F("h_clusweighty_V_cluseta_V_cluspt","clus weighty vs clus eta vs clus pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweighty_V_cluseta_V_cluspt",h_clusweighty_V_cluseta_V_cluspt);

  h_clusweightet = new TH1F("h_clusweightet","clus weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightet",h_clusweightet);

  h_clusweightet_V_cluspt = new TH2F("h_clusweightet_V_cluspt","clus weightet vs clus pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightet_V_cluspt",h_clusweightet_V_cluspt);

  h_clusweightet_V_cluseta = new TH2F("h_clusweightet_V_cluseta","clus weightet vs clus eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightet_V_cluseta",h_clusweightet_V_cluseta);

  h_clusweightet_V_clusphi = new TH2F("h_clusweightet_V_clusphi","clus weightet vs clus phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightet_V_clusphi",h_clusweightet_V_clusphi);

  h_clusweightet_V_cluseta_V_clusphi = new TH3F("h_clusweightet_V_cluseta_V_clusphi","clus weightet vs clus eta vs clus phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightet_V_cluseta_V_clusphi",h_clusweightet_V_cluseta_V_clusphi);

  h_clusweightet_V_cluseta_V_cluspt = new TH3F("h_clusweightet_V_cluseta_V_cluspt","clus weightet vs clus eta vs clus pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/clusweightet_V_cluseta_V_cluspt",h_clusweightet_V_cluseta_V_cluspt);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_calocluster() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_eflow()
{
  StatusCode sc;
  //Eflow
  h_New_V_Old_CellOutEflow_metx = new TH2F("h_New_V_Old_CellOutEflow_metx","New_V_Old_CellOutEflow_metx",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_CellOutEflow_metx",h_New_V_Old_CellOutEflow_metx);

  h_New_minus_Old_CellOutEflow_metx = new TH1F("h_New_minus_Old_CellOutEflow_metx","New_minus_Old_CellOutEflow_metx",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_CellOutEflow_metx",h_New_minus_Old_CellOutEflow_metx);

  h_New_V_Old_CellOutEflow_mety = new TH2F("h_New_V_Old_CellOutEflow_mety","New_V_Old_CellOutEflow_mety",500,-250.,250.,500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_CellOutEflow_mety",h_New_V_Old_CellOutEflow_mety);

  h_New_minus_Old_CellOutEflow_mety = new TH1F("h_New_minus_Old_CellOutEflow_mety","New_minus_Old_CellOutEflow_mety",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_CellOutEflow_mety",h_New_minus_Old_CellOutEflow_mety);

  h_New_V_Old_CellOutEflow_met = new TH2F("h_New_V_Old_CellOutEflow_met","New_V_Old_CellOutEflow_met",500,0.,250.,500,0.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_V_Old_CellOutEflow_met",h_New_V_Old_CellOutEflow_met);

  h_New_minus_Old_CellOutEflow_met = new TH1F("h_New_minus_Old_CellOutEflow_met","New_minus_Old_CellOutEflow_met",2000,-10.,10.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/New_minus_Old_CellOutEflow_met",h_New_minus_Old_CellOutEflow_met);

  h_eflow_clusweightx_V_clusweighty = new TH2F("h_eflow_clusweightx_V_clusweighty","eflow clus weightx vs clus weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx_V_clusweighty",h_eflow_clusweightx_V_clusweighty);

  h_eflow_clusweightx_V_clusweightet = new TH2F("h_eflow_clusweightx_V_clusweightet","eflow clus weightx vs clus weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx_V_clusweightet",h_eflow_clusweightx_V_clusweightet);

  h_eflow_clusweightx = new TH1F("h_eflow_clusweightx","eflow_clus weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx",h_eflow_clusweightx);

  h_eflow_clusweightx_V_cluspx = new TH2F("h_eflow_clusweightx_V_cluspx","eflow_clus weightx vs eflow_clus px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx_V_cluspx",h_eflow_clusweightx_V_cluspx);

  h_eflow_clusweightx_V_cluspt = new TH2F("h_eflow_clusweightx_V_cluspt","eflow_clus weightx vs eflow_clus pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx_V_cluspt",h_eflow_clusweightx_V_cluspt);

  h_eflow_clusweightx_V_cluseta = new TH2F("h_eflow_clusweightx_V_cluseta","eflow_clus weightx vs eflow_clus eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx_V_cluseta",h_eflow_clusweightx_V_cluseta);

  h_eflow_clusweightx_V_clusphi = new TH2F("h_eflow_clusweightx_V_clusphi","eflow_clus weightx vs eflow_clus phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx_V_clusphi",h_eflow_clusweightx_V_clusphi);

  h_eflow_clusweightx_V_cluseta_V_clusphi = new TH3F("h_eflow_clusweightx_V_cluseta_V_clusphi","eflow_clus weightx vs eflow_clus eta vs eflow_clus phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx_V_cluseta_V_clusphi",h_eflow_clusweightx_V_cluseta_V_clusphi);

  h_eflow_clusweightx_V_cluseta_V_cluspt = new TH3F("h_eflow_clusweightx_V_cluseta_V_cluspt","eflow_clus weightx vs eflow_clus eta vs eflow_clus pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightx_V_cluseta_V_cluspt",h_eflow_clusweightx_V_cluseta_V_cluspt);

  h_eflow_clusweighty = new TH1F("h_eflow_clusweighty","eflow_clus weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweighty",h_eflow_clusweighty);

  h_eflow_clusweighty_V_cluspy = new TH2F("h_eflow_clusweighty_V_cluspy","eflow_clus weighty vs eflow_clus py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweighty_V_cluspy",h_eflow_clusweighty_V_cluspy);

  h_eflow_clusweighty_V_cluspt = new TH2F("h_eflow_clusweighty_V_cluspt","eflow_clus weighty vs eflow_clus pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweighty_V_cluspt",h_eflow_clusweighty_V_cluspt);

  h_eflow_clusweighty_V_cluseta = new TH2F("h_eflow_clusweighty_V_cluseta","eflow_clus weighty vs eflow_clus eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweighty_V_cluseta",h_eflow_clusweighty_V_cluseta);

  h_eflow_clusweighty_V_clusphi = new TH2F("h_eflow_clusweighty_V_clusphi","eflow_clus weighty vs eflow_clus phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweighty_V_clusphi",h_eflow_clusweighty_V_clusphi);

  h_eflow_clusweighty_V_cluseta_V_clusphi = new TH3F("h_eflow_clusweighty_V_cluseta_V_clusphi","eflow_clus weighty vs eflow_clus eta vs eflow_clus phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweighty_V_cluseta_V_clusphi",h_eflow_clusweighty_V_cluseta_V_clusphi);

  h_eflow_clusweighty_V_cluseta_V_cluspt = new TH3F("h_eflow_clusweighty_V_cluseta_V_cluspt","eflow_clus weighty vs eflow_clus eta vs eflow_clus pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweighty_V_cluseta_V_cluspt",h_eflow_clusweighty_V_cluseta_V_cluspt);

  h_eflow_clusweightet = new TH1F("h_eflow_clusweightet","eflow_clus weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightet",h_eflow_clusweightet);

  h_eflow_clusweightet_V_cluspt = new TH2F("h_eflow_clusweightet_V_cluspt","eflow_clus weightet vs eflow_clus pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightet_V_cluspt",h_eflow_clusweightet_V_cluspt);

  h_eflow_clusweightet_V_cluseta = new TH2F("h_eflow_clusweightet_V_cluseta","eflow_clus weightet vs eflow_clus eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightet_V_cluseta",h_eflow_clusweightet_V_cluseta);

  h_eflow_clusweightet_V_clusphi = new TH2F("h_eflow_clusweightet_V_clusphi","eflow_clus weightet vs eflow_clus phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightet_V_clusphi",h_eflow_clusweightet_V_clusphi);

  h_eflow_clusweightet_V_cluseta_V_clusphi = new TH3F("h_eflow_clusweightet_V_cluseta_V_clusphi","eflow_clus weightet vs eflow_clus eta vs eflow_clus phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightet_V_cluseta_V_clusphi",h_eflow_clusweightet_V_cluseta_V_clusphi);

  h_eflow_clusweightet_V_cluseta_V_cluspt = new TH3F("h_eflow_clusweightet_V_cluseta_V_cluspt","eflow_clus weightet vs eflow_clus eta vs eflow_clus pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_clusweightet_V_cluseta_V_cluspt",h_eflow_clusweightet_V_cluseta_V_cluspt);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_eflow() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::CBNT_initialize_tracks()
{
  StatusCode sc;
  //tracks
  h_eflow_trkweightx_V_trkweighty = new TH2F("h_eflow_trkweightx_V_trkweighty","eflow trk weightx vs clus weighty",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx_V_trkweighty",h_eflow_trkweightx_V_trkweighty);

  h_eflow_trkweightx_V_trkweightet = new TH2F("h_eflow_trkweightx_V_trkweightet","eflow trk weightx vs trk weightet",400,-2.,2.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx_V_trkweightet",h_eflow_trkweightx_V_trkweightet);

  h_eflow_trkweightx = new TH1F("h_eflow_trkweightx","eflow_trk weightx",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx",h_eflow_trkweightx);

  h_eflow_trkweightx_V_trkpx = new TH2F("h_eflow_trkweightx_V_trkpx","eflow_trk weightx vs eflow_trk px",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx_V_trkpx",h_eflow_trkweightx_V_trkpx);

  h_eflow_trkweightx_V_trkpt = new TH2F("h_eflow_trkweightx_V_trkpt","eflow_trk weightx vs eflow_trk pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx_V_trkpt",h_eflow_trkweightx_V_trkpt);

  h_eflow_trkweightx_V_trketa = new TH2F("h_eflow_trkweightx_V_trketa","eflow_trk weightx vs eflow_trk eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx_V_trketa",h_eflow_trkweightx_V_trketa);

  h_eflow_trkweightx_V_trkphi = new TH2F("h_eflow_trkweightx_V_trkphi","eflow_trk weightx vs eflow_trk phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx_V_trkphi",h_eflow_trkweightx_V_trkphi);

  h_eflow_trkweightx_V_trketa_V_trkphi = new TH3F("h_eflow_trkweightx_V_trketa_V_trkphi","eflow_trk weightx vs eflow_trk eta vs eflow_trk phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx_V_trketa_V_trkphi",h_eflow_trkweightx_V_trketa_V_trkphi);

  h_eflow_trkweightx_V_trketa_V_trkpt = new TH3F("h_eflow_trkweightx_V_trketa_V_trkpt","eflow_trk weightx vs eflow_trk eta vs eflow_trk pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightx_V_trketa_V_trkpt",h_eflow_trkweightx_V_trketa_V_trkpt);

  h_eflow_trkweighty = new TH1F("h_eflow_trkweighty","eflow_trk weighty",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweighty",h_eflow_trkweighty);

  h_eflow_trkweighty_V_trkpy = new TH2F("h_eflow_trkweighty_V_trkpy","eflow_trk weighty vs eflow_trk py",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweighty_V_trkpy",h_eflow_trkweighty_V_trkpy);

  h_eflow_trkweighty_V_trkpt = new TH2F("h_eflow_trkweighty_V_trkpt","eflow_trk weighty vs eflow_trk pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweighty_V_trkpt",h_eflow_trkweighty_V_trkpt);

  h_eflow_trkweighty_V_trketa = new TH2F("h_eflow_trkweighty_V_trketa","eflow_trk weighty vs eflow_trk eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweighty_V_trketa",h_eflow_trkweighty_V_trketa);

  h_eflow_trkweighty_V_trkphi = new TH2F("h_eflow_trkweighty_V_trkphi","eflow_trk weighty vs eflow_trk phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweighty_V_trkphi",h_eflow_trkweighty_V_trkphi);

  h_eflow_trkweighty_V_trketa_V_trkphi = new TH3F("h_eflow_trkweighty_V_trketa_V_trkphi","eflow_trk weighty vs eflow_trk eta vs eflow_trk phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweighty_V_trketa_V_trkphi",h_eflow_trkweighty_V_trketa_V_trkphi);

  h_eflow_trkweighty_V_trketa_V_trkpt = new TH3F("h_eflow_trkweighty_V_trketa_V_trkpt","eflow_trk weighty vs eflow_trk eta vs eflow_trk pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweighty_V_trketa_V_trkpt",h_eflow_trkweighty_V_trketa_V_trkpt);

  h_eflow_trkweightet = new TH1F("h_eflow_trkweightet","eflow_trk weightet",400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightet",h_eflow_trkweightet);

  h_eflow_trkweightet_V_trkpt = new TH2F("h_eflow_trkweightet_V_trkpt","eflow_trk weightet vs eflow_trk pt",200,0,1000,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightet_V_trkpt",h_eflow_trkweightet_V_trkpt);

  h_eflow_trkweightet_V_trketa = new TH2F("h_eflow_trkweightet_V_trketa","eflow_trk weightet vs eflow_trk eta",200,-5.,5.,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightet_V_trketa",h_eflow_trkweightet_V_trketa);

  h_eflow_trkweightet_V_trkphi = new TH2F("h_eflow_trkweightet_V_trkphi","eflow_trk weightet vs eflow_trk phi",126,-3.15,3.15,400,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightet_V_trkphi",h_eflow_trkweightet_V_trkphi);

  h_eflow_trkweightet_V_trketa_V_trkphi = new TH3F("h_eflow_trkweightet_V_trketa_V_trkphi","eflow_trk weightet vs eflow_trk eta vs eflow_trk phi",100,-5.,5.,63,-3.15,3.15 ,200,0.0,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightet_V_trketa_V_trkphi",h_eflow_trkweightet_V_trketa_V_trkphi);

  h_eflow_trkweightet_V_trketa_V_trkpt = new TH3F("h_eflow_trkweightet_V_trketa_V_trkpt","eflow_trk weightet vs eflow_trk eta vs eflow_trk pt",100,-5.,5.,20,0.,200.,200,0.,2.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/eflow_trkweightet_V_trketa_V_trkpt",h_eflow_trkweightet_V_trketa_V_trkpt);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool CBNT_initialize_tracks() fail" << endreq; }

  return sc;
}


StatusCode MissingETCompositionTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode MissingETCompositionTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  sc = missingETCompositionPlots(data);

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool execute() fail" << endreq; }

  return sc;
}

StatusCode MissingETCompositionTool::missingETCompositionPlots(MissingETData *data) {

  msg() << MSG::DEBUG << "in missingETCompositionPlots() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  //retrieve object
  const MissingETComposition *pComp = data->metComposition();

  if (_make_event_displays) {
    if (evtStore()->contains<xAOD::EventInfo>(m_evtInfoKey)) {
      const xAOD::EventInfo* pevt = 0;
      sc = evtStore()->retrieve(pevt, m_evtInfoKey);
      if( !sc.isFailure() ){
	unsigned int run = pevt->runNumber();
	unsigned int evt = pevt->eventNumber();

	event_displays.push_back(new evt_display(run,evt,m_thistSvc,m_folderName,&msg()));
	msg() << MSG::WARNING << "Making Event Display for run " << run << ", event " << evt << endreq; 

	msg() << MSG::WARNING << "MET_Topo METx   = " << data->topo()->etx()/1000. << endreq;
	msg() << MSG::WARNING << "MET_Topo METy   = " << data->topo()->ety()/1000. << endreq;
	msg() << MSG::WARNING << "MET_Topo MET    = " << data->topo()->et()/1000. << endreq;
	msg() << MSG::WARNING << "MET_Topo METphi = " << data->topo()->phi() << endreq;
	msg() << MSG::WARNING << "MET_Topo SUMET  = " << data->topo()->sumet()/1000. << endreq;

	msg() << MSG::WARNING << "MET_CorrTopo METx   = " << data->corrTopo()->etx()/1000. << endreq;
	msg() << MSG::WARNING << "MET_CorrTopo METy   = " << data->corrTopo()->ety()/1000. << endreq;
	msg() << MSG::WARNING << "MET_CorrTopo MET    = " << data->corrTopo()->et()/1000. << endreq;
	msg() << MSG::WARNING << "MET_CorrTopo METphi = " << data->corrTopo()->phi() << endreq;
	msg() << MSG::WARNING << "MET_CorrTopo SUMET  = " << data->corrTopo()->sumet()/1000. << endreq;

	msg() << MSG::WARNING << "MET_LocHadTopo METx   = " << data->locHadTopo()->etx()/1000. << endreq;
	msg() << MSG::WARNING << "MET_LocHadTopo METy   = " << data->locHadTopo()->ety()/1000. << endreq;
	msg() << MSG::WARNING << "MET_LocHadTopo MET    = " << data->locHadTopo()->et()/1000. << endreq;
	msg() << MSG::WARNING << "MET_LocHadTopo METphi = " << data->locHadTopo()->phi() << endreq;
	msg() << MSG::WARNING << "MET_LocHadTopo SUMET  = " << data->locHadTopo()->sumet()/1000. << endreq;

	msg() << MSG::WARNING << "MET_Cryo METx   = " << data->cryo()->etx()/1000. << endreq;
	msg() << MSG::WARNING << "MET_Cryo METy   = " << data->cryo()->ety()/1000. << endreq;
	msg() << MSG::WARNING << "MET_Cryo MET    = " << data->cryo()->et()/1000. << endreq;
	msg() << MSG::WARNING << "MET_Cryo METphi = " << data->cryo()->phi() << endreq;
	msg() << MSG::WARNING << "MET_Cryo SUMET  = " << data->cryo()->sumet()/1000. << endreq;

      }//retrieved container
      else {
	return sc;
      }
    }//event container is there
    else {//containter isn't there
      msg() << MSG::WARNING << "can't find event information in storegate with name " 
	    << m_evtInfoKey <<  ", not making MET Composition event displays" << endreq;
      _make_event_displays = false;
    }//containter isn't there 
  }//make event displays

  ////////////////////////////////////////////////////////////////////////////////////
  //jets
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<Jet> fJet(pComp->beginIterator<Jet>());
  MissingETComposition::const_iterator<Jet> lJet(pComp->endIterator<Jet>());

  unsigned int njets = 0;
  MissingET met_refjet;
  MissingET met_softjets;

  for ( ; fJet != lJet; fJet++ )
    {
//       msg() << MSG::INFO
// 	     << "jet " << i << " px, py, pt, eta, phi, EMJES,GCWJES, weight x, weight y, weight pt, status word= "
// 	     << (*fJet)->px() << ", "
// 	     << (*fJet)->py() << ", "
//  	     << (*fJet)->pt() << ", "
//  	     << (*fJet)->eta() << ", "
//  	     << (*fJet)->phi() << ", "
//  	     << (*fJet)->getMoment("EMJES") << ", "
//  	     << (*fJet)->getMoment("GCWJES") << ", "
// 	     << fJet.getParameter().wpx() << ", "
//  	     << fJet.getParameter().wpy() << ", "
//  	     << fJet.getParameter().wet() << ", "
//  	     << fJet.getParameter().statusWord() << endreq;

      h_jetweightx->Fill(fJet.getParameter().wpx());
      h_jetweightx_V_jetpt->Fill((*fJet)->pt()/1000.,fJet.getParameter().wpx());
      h_jetweightx_V_jetpx->Fill((*fJet)->px()/1000.,fJet.getParameter().wpx());
      h_jetweightx_V_jeteta->Fill((*fJet)->eta(),fJet.getParameter().wpx());
      h_jetweightx_V_jetphi->Fill((*fJet)->phi(),fJet.getParameter().wpx());
      h_jetweightx_V_jeteta_V_jetphi->Fill((*fJet)->eta(),(*fJet)->phi(),fJet.getParameter().wpx());
      h_jetweightx_V_jeteta_V_jetpt->Fill((*fJet)->eta(),(*fJet)->pt()/1000.,fJet.getParameter().wpx());

      h_jetweighty->Fill(fJet.getParameter().wpy());
      h_jetweighty_V_jetpt->Fill((*fJet)->pt()/1000.,fJet.getParameter().wpy());
      h_jetweighty_V_jetpy->Fill((*fJet)->py()/1000.,fJet.getParameter().wpy());
      h_jetweighty_V_jeteta->Fill((*fJet)->eta(),fJet.getParameter().wpy());
      h_jetweighty_V_jetphi->Fill((*fJet)->phi(),fJet.getParameter().wpy());
      h_jetweighty_V_jeteta_V_jetphi->Fill((*fJet)->eta(),(*fJet)->phi(),fJet.getParameter().wpy());
      h_jetweighty_V_jeteta_V_jetpt->Fill((*fJet)->eta(),(*fJet)->pt()/1000.,fJet.getParameter().wpy());

      h_jetweightet->Fill(fJet.getParameter().wet());
      h_jetweightet_V_jetpt->Fill((*fJet)->pt()/1000.,fJet.getParameter().wet());
      h_jetweightet_V_jeteta->Fill((*fJet)->eta(),fJet.getParameter().wet());
      h_jetweightet_V_jetphi->Fill((*fJet)->phi(),fJet.getParameter().wet());
      h_jetweightet_V_jeteta_V_jetphi->Fill((*fJet)->eta(),(*fJet)->phi(),fJet.getParameter().wet());
      h_jetweightet_V_jeteta_V_jetpt->Fill((*fJet)->eta(),(*fJet)->pt()/1000.,fJet.getParameter().wet());

      h_jetweightx_V_jetweighty->Fill(fJet.getParameter().wpy(),fJet.getParameter().wpx());
      h_jetweightx_V_jetweightet->Fill(fJet.getParameter().wet(),fJet.getParameter().wpx());

      if (_make_event_displays) {
	event_displays.back()->h_jeteta_V_phi_evt->Fill((*fJet)->eta(),(*fJet)->phi(),(*fJet)->pt()*fJet.getParameter().wet()/1000.);
      }

      if ((*fJet)->pt() > 20000.) {

	met_refjet.add((*fJet)->px()*fJet.getParameter().wpx(),(*fJet)->py()*fJet.getParameter().wpy(),
		       (*fJet)->pt()*fJet.getParameter().wet(),
		       (*fJet)->eta(),
		       1.0);
      }
      else {
	met_softjets.add((*fJet)->px()*fJet.getParameter().wpx(),(*fJet)->py()*fJet.getParameter().wpy(),
		       (*fJet)->pt()*fJet.getParameter().wet(),
		       (*fJet)->eta(),
		       1.0);
      }
      njets++;
    }
//   msg() << MSG::INFO
// 	 << "met refjet sums(px, py, et) = ("
// 	 << met_refjet.etx() << ", "
// 	 << met_refjet.ety() << ", "
// 	 << met_refjet.sumet() << ")" << endreq;

  if (_make_event_displays) {
    msg() << MSG::WARNING << "MET_RefJet METx   = " << met_refjet.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefJet METy   = " << met_refjet.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefJet MET    = " << met_refjet.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefJet METphi = " << met_refjet.phi() << endreq;
    msg() << MSG::WARNING << "MET_RefJet SUMET  = " << met_refjet.sumet()/1000. << endreq;
  }

  h_njets_compmap->Fill(njets);

  h_New_V_Old_RefJet_metx->Fill(data->refJet()->etx()/1000.,met_refjet.etx()/1000.);
  h_New_minus_Old_RefJet_metx->Fill(data->refJet()->etx()/1000. - met_refjet.etx()/1000.);

  h_New_V_Old_RefJet_mety->Fill(data->refJet()->ety()/1000.,met_refjet.ety()/1000.);
  h_New_minus_Old_RefJet_mety->Fill(data->refJet()->ety()/1000. - met_refjet.ety()/1000.);

  h_New_V_Old_RefJet_met->Fill(data->refJet()->et()/1000.,met_refjet.et()/1000.);
  h_New_minus_Old_RefJet_met->Fill(data->refJet()->et()/1000. - met_refjet.et()/1000.);


  h_New_V_Old_SoftJets_metx->Fill(data->softJets()->etx()/1000.,met_softjets.etx()/1000.);
  h_New_minus_Old_SoftJets_metx->Fill(data->softJets()->etx()/1000. - met_softjets.etx()/1000.);

  h_New_V_Old_SoftJets_mety->Fill(data->softJets()->ety()/1000.,met_softjets.ety()/1000.);
  h_New_minus_Old_SoftJets_mety->Fill(data->softJets()->ety()/1000. - met_softjets.ety()/1000.);

  h_New_V_Old_SoftJets_met->Fill(data->softJets()->et()/1000.,met_softjets.et()/1000.);
  h_New_minus_Old_SoftJets_met->Fill(data->softJets()->et()/1000. - met_softjets.et()/1000.);

  ////////////////////////////////////////////////////////////////////////////////////
  //taus
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<Analysis::TauJet> fTau(pComp->beginIterator<Analysis::TauJet>());
  MissingETComposition::const_iterator<Analysis::TauJet> lTau(pComp->endIterator<Analysis::TauJet>());

  unsigned int ntaus = 0;
  MissingET met_reftau;

  for ( ; fTau != lTau; fTau++ )
    {
//       msg() << MSG::INFO
// 	     << "tau " << i << " px, py, pt, eta, phi, weight x, weight y, weight pt, status word= "
// 	     << (*fTau)->px() << ", "
// 	     << (*fTau)->py() << ", "
//  	     << (*fTau)->pt() << ", "
//  	     << (*fTau)->eta() << ", "
//  	     << (*fTau)->phi() << ", "
// 	     << fTau.getParameter().wpx() << ", "
//  	     << fTau.getParameter().wpy() << ", "
//  	     << fTau.getParameter().wet() << ", "
//  	     << fTau.getParameter().statusWord() << endreq;

      met_reftau.add((*fTau)->px()*fTau.getParameter().wpx(),(*fTau)->py()*fTau.getParameter().wpy(),
		     (*fTau)->pt()*fTau.getParameter().wet(),
		     (*fTau)->eta(),
		     1.0);

      h_tauweightx->Fill(fTau.getParameter().wpx());
      h_tauweightx_V_taupt->Fill((*fTau)->pt()/1000.,fTau.getParameter().wpx());
      h_tauweightx_V_taupx->Fill((*fTau)->px()/1000.,fTau.getParameter().wpx());
      h_tauweightx_V_taueta->Fill((*fTau)->eta(),fTau.getParameter().wpx());
      h_tauweightx_V_tauphi->Fill((*fTau)->phi(),fTau.getParameter().wpx());
      h_tauweightx_V_taueta_V_tauphi->Fill((*fTau)->eta(),(*fTau)->phi(),fTau.getParameter().wpx());
      h_tauweightx_V_taueta_V_taupt->Fill((*fTau)->eta(),(*fTau)->pt()/1000.,fTau.getParameter().wpx());

      h_tauweighty->Fill(fTau.getParameter().wpy());
      h_tauweighty_V_taupt->Fill((*fTau)->pt()/1000.,fTau.getParameter().wpy());
      h_tauweighty_V_taupy->Fill((*fTau)->py()/1000.,fTau.getParameter().wpy());
      h_tauweighty_V_taueta->Fill((*fTau)->eta(),fTau.getParameter().wpy());
      h_tauweighty_V_tauphi->Fill((*fTau)->phi(),fTau.getParameter().wpy());
      h_tauweighty_V_taueta_V_tauphi->Fill((*fTau)->eta(),(*fTau)->phi(),fTau.getParameter().wpy());
      h_tauweighty_V_taueta_V_taupt->Fill((*fTau)->eta(),(*fTau)->pt()/1000.,fTau.getParameter().wpy());

      h_tauweightet->Fill(fTau.getParameter().wet());
      h_tauweightet_V_taupt->Fill((*fTau)->pt()/1000.,fTau.getParameter().wet());
      h_tauweightet_V_taueta->Fill((*fTau)->eta(),fTau.getParameter().wet());
      h_tauweightet_V_tauphi->Fill((*fTau)->phi(),fTau.getParameter().wet());
      h_tauweightet_V_taueta_V_tauphi->Fill((*fTau)->eta(),(*fTau)->phi(),fTau.getParameter().wet());
      h_tauweightet_V_taueta_V_taupt->Fill((*fTau)->eta(),(*fTau)->pt()/1000.,fTau.getParameter().wet());

      h_tauweightx_V_tauweighty->Fill(fTau.getParameter().wpy(),fTau.getParameter().wpx());
      h_tauweightx_V_tauweightet->Fill(fTau.getParameter().wet(),fTau.getParameter().wpx());

      if (_make_event_displays) {
	event_displays.back()->h_taueta_V_phi_evt->Fill((*fTau)->eta(),(*fTau)->phi(),(*fTau)->pt()*fTau.getParameter().wet()/1000.);
      }

      ntaus++;
    }
//   msg() << MSG::INFO
// 	 << "met reftau sums(px, py, et) = ("
// 	 << met_reftau.etx() << ", "
// 	 << met_reftau.ety() << ", "
// 	 << met_reftau.sumet() << ")" << endreq;

  if (_make_event_displays) {
    msg() << MSG::WARNING << "MET_RefTau METx   = " << met_reftau.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefTau METy   = " << met_reftau.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefTau MET    = " << met_reftau.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefTau METphi = " << met_reftau.phi() << endreq;
    msg() << MSG::WARNING << "MET_RefTau SUMET  = " << met_reftau.sumet()/1000. << endreq;
  }

  h_ntaus_compmap->Fill(ntaus);

  h_New_V_Old_RefTau_metx->Fill(data->refTau()->etx()/1000.,met_reftau.etx()/1000.);
  h_New_minus_Old_RefTau_metx->Fill(data->refTau()->etx()/1000. - met_reftau.etx()/1000.);

  h_New_V_Old_RefTau_mety->Fill(data->refTau()->ety()/1000.,met_reftau.ety()/1000.);
  h_New_minus_Old_RefTau_mety->Fill(data->refTau()->ety()/1000. - met_reftau.ety()/1000.);

  h_New_V_Old_RefTau_met->Fill(data->refTau()->et()/1000.,met_reftau.et()/1000.);
  h_New_minus_Old_RefTau_met->Fill(data->refTau()->et()/1000. - met_reftau.et()/1000.);

  ////////////////////////////////////////////////////////////////////////////////////
  //egamma - electrons
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<egamma> fEle(pComp->beginIterator<egamma>());
  MissingETComposition::const_iterator<egamma> lEle(pComp->endIterator<egamma>());

  unsigned int neles = 0;
  MissingET met_refele;

  for ( ; fEle != lEle; fEle++ )
    {
      if (!(*fEle)->author(egammaParameters::AuthorElectron)) {continue;}

//       msg() << MSG::INFO
// 	     << "electron " << i << " px, py, pt, weight x, weight y, weight pt, status word= "
// 	     << (*fEle)->px() << ", "
// 	     << (*fEle)->py() << ", "
//  	     << (*fEle)->pt() << ", "
// 	     << fEle.getParameter().wpx() << ", "
//  	     << fEle.getParameter().wpy() << ", "
//  	     << fEle.getParameter().wet() << ", "
//  	     << fEle.getParameter().statusWord() << endreq;

      met_refele.add((*fEle)->px()*fEle.getParameter().wpx(),(*fEle)->py()*fEle.getParameter().wpy(),
 	      (*fEle)->pt()*fEle.getParameter().wet(),
	      (*fEle)->eta(),
	      1.0);

      h_eleweightx->Fill(fEle.getParameter().wpx());
      h_eleweightx_V_elept->Fill((*fEle)->pt()/1000.,fEle.getParameter().wpx());
      h_eleweightx_V_elepx->Fill((*fEle)->px()/1000.,fEle.getParameter().wpx());
      h_eleweightx_V_eleeta->Fill((*fEle)->eta(),fEle.getParameter().wpx());
      h_eleweightx_V_elephi->Fill((*fEle)->phi(),fEle.getParameter().wpx());
      h_eleweightx_V_eleeta_V_elephi->Fill((*fEle)->eta(),(*fEle)->phi(),fEle.getParameter().wpx());
      h_eleweightx_V_eleeta_V_elept->Fill((*fEle)->eta(),(*fEle)->pt()/1000.,fEle.getParameter().wpx());

      h_eleweighty->Fill(fEle.getParameter().wpy());
      h_eleweighty_V_elept->Fill((*fEle)->pt()/1000.,fEle.getParameter().wpy());
      h_eleweighty_V_elepy->Fill((*fEle)->py()/1000.,fEle.getParameter().wpy());
      h_eleweighty_V_eleeta->Fill((*fEle)->eta(),fEle.getParameter().wpy());
      h_eleweighty_V_elephi->Fill((*fEle)->phi(),fEle.getParameter().wpy());
      h_eleweighty_V_eleeta_V_elephi->Fill((*fEle)->eta(),(*fEle)->phi(),fEle.getParameter().wpy());
      h_eleweighty_V_eleeta_V_elept->Fill((*fEle)->eta(),(*fEle)->pt()/1000.,fEle.getParameter().wpy());

      h_eleweightet->Fill(fEle.getParameter().wet());
      h_eleweightet_V_elept->Fill((*fEle)->pt()/1000.,fEle.getParameter().wet());
      h_eleweightet_V_eleeta->Fill((*fEle)->eta(),fEle.getParameter().wet());
      h_eleweightet_V_elephi->Fill((*fEle)->phi(),fEle.getParameter().wet());
      h_eleweightet_V_eleeta_V_elephi->Fill((*fEle)->eta(),(*fEle)->phi(),fEle.getParameter().wet());
      h_eleweightet_V_eleeta_V_elept->Fill((*fEle)->eta(),(*fEle)->pt()/1000.,fEle.getParameter().wet());

      h_eleweightx_V_eleweighty->Fill(fEle.getParameter().wpy(),fEle.getParameter().wpx());
      h_eleweightx_V_eleweightet->Fill(fEle.getParameter().wet(),fEle.getParameter().wpx());

      if (_make_event_displays) {
	event_displays.back()->h_eleeta_V_phi_evt->Fill((*fEle)->eta(),(*fEle)->phi(),(*fEle)->pt()*fEle.getParameter().wet()/1000.);
      }

      neles++;
    }//loop over electrons

  if (_make_event_displays) {
    msg() << MSG::WARNING << "MET_RefEle METx   = " << met_refele.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefEle METy   = " << met_refele.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefEle MET    = " << met_refele.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefEle METphi = " << met_refele.phi() << endreq;
    msg() << MSG::WARNING << "MET_RefEle SUMET  = " << met_refele.sumet()/1000. << endreq;
  }

  h_neles_compmap->Fill(neles);

  h_New_V_Old_RefEle_metx->Fill(data->refEle()->etx()/1000.,met_refele.etx()/1000.);
  h_New_minus_Old_RefEle_metx->Fill(data->refEle()->etx()/1000. - met_refele.etx()/1000.);

  h_New_V_Old_RefEle_mety->Fill(data->refEle()->ety()/1000.,met_refele.ety()/1000.);
  h_New_minus_Old_RefEle_mety->Fill(data->refEle()->ety()/1000. - met_refele.ety()/1000.);

  h_New_V_Old_RefEle_met->Fill(data->refEle()->et()/1000.,met_refele.et()/1000.);
  h_New_minus_Old_RefEle_met->Fill(data->refEle()->et()/1000. - met_refele.et()/1000.);

  ////////////////////////////////////////////////////////////////////////////////////
  //egamma - photon
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<egamma> fGamma(pComp->beginIterator<egamma>());
  MissingETComposition::const_iterator<egamma> lGamma(pComp->endIterator<egamma>());

  unsigned int ngammas = 0;
  MissingET met_refgamma;

  for ( ; fGamma != lGamma; fGamma++)
    {
      if (!(*fGamma)->author(egammaParameters::AuthorPhoton)) {continue;}

//       msg() << MSG::INFO
// 	     << "photon " << i << " px, py, pt, weight x, weight y, weight pt, status word= "
// 	     << (*fGamma)->px() << ", "
// 	     << (*fGamma)->py() << ", "
//  	     << (*fGamma)->pt() << ", "
// 	     << fGamma.getParameter().wpx() << ", "
//  	     << fGamma.getParameter().wpy() << ", "
//  	     << fGamma.getParameter().wet() << ", "
//  	     << fGamma.getParameter().statusWord() << endreq;

      met_refgamma.add((*fGamma)->px()*fGamma.getParameter().wpx(),(*fGamma)->py()*fGamma.getParameter().wpy(),
 	      (*fGamma)->pt()*fGamma.getParameter().wet(),
	      (*fGamma)->eta(),
	      1.0);

      h_gammaweightx->Fill(fGamma.getParameter().wpx());
      h_gammaweightx_V_gammapt->Fill((*fGamma)->pt()/1000.,fGamma.getParameter().wpx());
      h_gammaweightx_V_gammapx->Fill((*fGamma)->px()/1000.,fGamma.getParameter().wpx());
      h_gammaweightx_V_gammaeta->Fill((*fGamma)->eta(),fGamma.getParameter().wpx());
      h_gammaweightx_V_gammaphi->Fill((*fGamma)->phi(),fGamma.getParameter().wpx());
      h_gammaweightx_V_gammaeta_V_gammaphi->Fill((*fGamma)->eta(),(*fGamma)->phi(),fGamma.getParameter().wpx());
      h_gammaweightx_V_gammaeta_V_gammapt->Fill((*fGamma)->eta(),(*fGamma)->pt()/1000.,fGamma.getParameter().wpx());

      h_gammaweighty->Fill(fGamma.getParameter().wpy());
      h_gammaweighty_V_gammapt->Fill((*fGamma)->pt()/1000.,fGamma.getParameter().wpy());
      h_gammaweighty_V_gammapy->Fill((*fGamma)->py()/1000.,fGamma.getParameter().wpy());
      h_gammaweighty_V_gammaeta->Fill((*fGamma)->eta(),fGamma.getParameter().wpy());
      h_gammaweighty_V_gammaphi->Fill((*fGamma)->phi(),fGamma.getParameter().wpy());
      h_gammaweighty_V_gammaeta_V_gammaphi->Fill((*fGamma)->eta(),(*fGamma)->phi(),fGamma.getParameter().wpy());
      h_gammaweighty_V_gammaeta_V_gammapt->Fill((*fGamma)->eta(),(*fGamma)->pt()/1000.,fGamma.getParameter().wpy());

      h_gammaweightet->Fill(fGamma.getParameter().wet());
      h_gammaweightet_V_gammapt->Fill((*fGamma)->pt()/1000.,fGamma.getParameter().wet());
      h_gammaweightet_V_gammaeta->Fill((*fGamma)->eta(),fGamma.getParameter().wet());
      h_gammaweightet_V_gammaphi->Fill((*fGamma)->phi(),fGamma.getParameter().wet());
      h_gammaweightet_V_gammaeta_V_gammaphi->Fill((*fGamma)->eta(),(*fGamma)->phi(),fGamma.getParameter().wet());
      h_gammaweightet_V_gammaeta_V_gammapt->Fill((*fGamma)->eta(),(*fGamma)->pt()/1000.,fGamma.getParameter().wet());

      h_gammaweightx_V_gammaweighty->Fill(fGamma.getParameter().wpy(),fGamma.getParameter().wpx());
      h_gammaweightx_V_gammaweightet->Fill(fGamma.getParameter().wet(),fGamma.getParameter().wpx());

      if (_make_event_displays) {
	event_displays.back()->h_gammaeta_V_phi_evt->Fill((*fGamma)->eta(),(*fGamma)->phi(),(*fGamma)->pt()*fGamma.getParameter().wet()/1000.);
      }

      ngammas++;
    }
//   msg() << MSG::INFO
// 	 << "met refgamma sums(px, py, et) = ("
// 	 << met_refgamma.etx() << ", "
// 	 << met_refgamma.ety() << ", "
// 	 << met_refgamma.sumet() << ")" << endreq;

  if (_make_event_displays) {
    msg() << MSG::WARNING << "MET_RefGamma METx   = " << met_refgamma.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefGamma METy   = " << met_refgamma.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefGamma MET    = " << met_refgamma.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefGamma METphi = " << met_refgamma.phi() << endreq;
    msg() << MSG::WARNING << "MET_RefGamma SUMET  = " << met_refgamma.sumet()/1000. << endreq;
  }

  h_ngammas_compmap->Fill(ngammas);

  h_New_V_Old_RefGamma_metx->Fill(data->refGamma()->etx()/1000.,met_refgamma.etx()/1000.);
  h_New_minus_Old_RefGamma_metx->Fill(data->refGamma()->etx()/1000. - met_refgamma.etx()/1000.);

  h_New_V_Old_RefGamma_mety->Fill(data->refGamma()->ety()/1000.,met_refgamma.ety()/1000.);
  h_New_minus_Old_RefGamma_mety->Fill(data->refGamma()->ety()/1000. - met_refgamma.ety()/1000.);

  h_New_V_Old_RefGamma_met->Fill(data->refGamma()->et()/1000.,met_refgamma.et()/1000.);
  h_New_minus_Old_RefGamma_met->Fill(data->refGamma()->et()/1000. - met_refgamma.et()/1000.);

  ////////////////////////////////////////////////////////////////////////////////////
  //muid
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<Analysis::Muon> fMuon_id(pComp->beginIterator<Analysis::Muon>());
  MissingETComposition::const_iterator<Analysis::Muon> lMuon_id(pComp->endIterator<Analysis::Muon>());

  unsigned int nmuid = 0;
  MissingET met_muid;

  for ( ; fMuon_id != lMuon_id; fMuon_id++)
    {
      if (MissingETtags::usesREFMUON(fMuon_id.getParameter().statusWord())) {
	//not part of muid term
	continue;
      }
      if (!(MissingETtags::usesMUID(fMuon_id.getParameter().statusWord()))) {
	//not part of muid term
	continue;
      }

//       msg() << MSG::INFO
// 	     << "muon " << i << " px, py, pt, weight x, weight y, weight pt, status word= "
// 	     << (*fMuon_id)->px() << ", "
// 	     << (*fMuon_id)->py() << ", "
//  	     << (*fMuon_id)->pt() << ", "
// 	     << fMuon_id.getParameter().wpx() << ", "
//  	     << fMuon_id.getParameter().wpy() << ", "
//  	     << fMuon_id.getParameter().wet() << ", "
//  	     << fMuon_id.getParameter().statusWord() << endreq;

      //get the right px,py,pt based on status word
      float px = 0.;
      float py = 0.;
      float pt = 0.;
      float eta = 0.;
      float phi = 0.;

      if (MissingETtags::usesDEFAULT(fMuon_id.getParameter().statusWord())) {
	px = (*fMuon_id)->px();
	py = (*fMuon_id)->py();
	pt = (*fMuon_id)->pt();
	eta = (*fMuon_id)->eta();
	phi = (*fMuon_id)->phi();
      } else if (MissingETtags::usesSPECTRO(fMuon_id.getParameter().statusWord())) {
	px = (*fMuon_id)->muonSpectrometerTrackParticle()->px();
	py = (*fMuon_id)->muonSpectrometerTrackParticle()->py();
	pt = (*fMuon_id)->muonSpectrometerTrackParticle()->pt();
	eta = (*fMuon_id)->muonSpectrometerTrackParticle()->eta();
	phi = (*fMuon_id)->muonSpectrometerTrackParticle()->phi();
      } else if (MissingETtags::usesTRACK(fMuon_id.getParameter().statusWord())) {
	px = (*fMuon_id)->track()->px();
	py = (*fMuon_id)->track()->py();
	pt = (*fMuon_id)->track()->pt();
	eta = (*fMuon_id)->track()->eta();
	phi = (*fMuon_id)->track()->phi();
      }
      else {
	msg() << MSG::WARNING << "bad status word in muid term!!!, = " << fMuon_id.getParameter().statusWord() << endreq;
      }
      met_muid.add(px*fMuon_id.getParameter().wpx(),py*fMuon_id.getParameter().wpy(),
 	      pt*fMuon_id.getParameter().wet(),eta, 1.0);

      h_muidweightx->Fill(fMuon_id.getParameter().wpx());
      h_muidweightx_V_muonpt->Fill(pt/1000.,fMuon_id.getParameter().wpx());
      h_muidweightx_V_muonpx->Fill(px/1000.,fMuon_id.getParameter().wpx());
      h_muidweightx_V_muoneta->Fill(eta,fMuon_id.getParameter().wpx());
      h_muidweightx_V_muonphi->Fill(phi,fMuon_id.getParameter().wpx());
      h_muidweightx_V_muoneta_V_muonphi->Fill(eta,phi,fMuon_id.getParameter().wpx());
      h_muidweightx_V_muoneta_V_muonpt->Fill(eta,pt/1000.,fMuon_id.getParameter().wpx());

      h_muidweighty->Fill(fMuon_id.getParameter().wpy());
      h_muidweighty_V_muonpt->Fill(pt/1000.,fMuon_id.getParameter().wpy());
      h_muidweighty_V_muonpy->Fill(py/1000.,fMuon_id.getParameter().wpy());
      h_muidweighty_V_muoneta->Fill(eta,fMuon_id.getParameter().wpy());
      h_muidweighty_V_muonphi->Fill(phi,fMuon_id.getParameter().wpy());
      h_muidweighty_V_muoneta_V_muonphi->Fill(eta,phi,fMuon_id.getParameter().wpy());
      h_muidweighty_V_muoneta_V_muonpt->Fill(eta,pt/1000.,fMuon_id.getParameter().wpy());

      h_muidweightet->Fill(fMuon_id.getParameter().wet());
      h_muidweightet_V_muonpt->Fill(pt/1000.,fMuon_id.getParameter().wet());
      h_muidweightet_V_muoneta->Fill(eta,fMuon_id.getParameter().wet());
      h_muidweightet_V_muonphi->Fill(phi,fMuon_id.getParameter().wet());
      h_muidweightet_V_muoneta_V_muonphi->Fill(eta,phi,fMuon_id.getParameter().wet());
      h_muidweightet_V_muoneta_V_muonpt->Fill(eta,pt/1000.,fMuon_id.getParameter().wet());

      h_muidweightx_V_muidweighty->Fill(fMuon_id.getParameter().wpy(),fMuon_id.getParameter().wpx());
      h_muidweightx_V_muidweightet->Fill(fMuon_id.getParameter().wet(),fMuon_id.getParameter().wpx());

      if (_make_event_displays) {
	event_displays.back()->h_muideta_V_phi_evt->Fill(eta,phi,pt*fMuon_id.getParameter().wet()/1000.);
      }

      nmuid++;
    }
//   msg() << MSG::WARNING
// 	 << "met muid sums(px, py, et) = ("
// 	 << met_muid.etx() << ", "
// 	 << met_muid.ety() << ", "
// 	 << met_muid.sumet() << ")" << endreq;

  if (_make_event_displays) {
    msg() << MSG::WARNING << "MET_MuID METx   = " << met_muid.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_MuID METy   = " << met_muid.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_MuID MET    = " << met_muid.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_MuID METphi = " << met_muid.phi() << endreq;
    msg() << MSG::WARNING << "MET_MuID SUMET  = " << met_muid.sumet()/1000. << endreq;
  }

  h_nmuids_compmap->Fill(nmuid);

  h_New_V_Old_MuID_metx->Fill(data->MuID()->etx()/1000.,met_muid.etx()/1000.);
  h_New_minus_Old_MuID_metx->Fill(data->MuID()->etx()/1000. - met_muid.etx()/1000.);

  h_New_V_Old_MuID_mety->Fill(data->MuID()->ety()/1000.,met_muid.ety()/1000.);
  h_New_minus_Old_MuID_mety->Fill(data->MuID()->ety()/1000. - met_muid.ety()/1000.);

  h_New_V_Old_MuID_met->Fill(data->MuID()->et()/1000.,met_muid.et()/1000.);
  h_New_minus_Old_MuID_met->Fill(data->MuID()->et()/1000. - met_muid.et()/1000.);

  ////////////////////////////////////////////////////////////////////////////////////
  //muonboy
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<Analysis::Muon> fMuon(pComp->beginIterator<Analysis::Muon>());
  MissingETComposition::const_iterator<Analysis::Muon> lMuon(pComp->endIterator<Analysis::Muon>());

  unsigned int nmuboy = 0;
  MissingET met_muonboy;

  for ( ; fMuon != lMuon; fMuon++)
    {
      if (MissingETtags::usesREFMUON(fMuon.getParameter().statusWord())) {
	//not part of muonboy term
	continue;
      }
      if (MissingETtags::usesMUID(fMuon.getParameter().statusWord())) {
	//not part of muonboy term
	continue;
      }

//       msg() << MSG::INFO
// 	     << "muon " << i << " px, py, pt, weight x, weight y, weight pt, status word= "
// 	     << (*fMuon)->px() << ", "
// 	     << (*fMuon)->py() << ", "
//  	     << (*fMuon)->pt() << ", "
// 	     << fMuon.getParameter().wpx() << ", "
//  	     << fMuon.getParameter().wpy() << ", "
//  	     << fMuon.getParameter().wet() << ", "
//  	     << fMuon.getParameter().statusWord() << endreq;

      //get the right px,py,pt based on status word
      float px = 0.;
      float py = 0.;
      float pt = 0.;
      float eta = 0.;
      float phi = 0.;

      if (MissingETtags::usesDEFAULT(fMuon.getParameter().statusWord())) {
	px = (*fMuon)->px();
	py = (*fMuon)->py();
	pt = (*fMuon)->pt();
	eta = (*fMuon)->eta();
	phi = (*fMuon)->phi();
      } else if (MissingETtags::usesSPECTRO(fMuon.getParameter().statusWord())) {
	px = (*fMuon)->muonSpectrometerTrackParticle()->px();
	py = (*fMuon)->muonSpectrometerTrackParticle()->py();
	pt = (*fMuon)->muonSpectrometerTrackParticle()->pt();
	eta = (*fMuon)->muonSpectrometerTrackParticle()->eta();
	phi = (*fMuon)->muonSpectrometerTrackParticle()->phi();
      } else if (MissingETtags::usesTRACK(fMuon.getParameter().statusWord())) {
	px = (*fMuon)->track()->px();
	py = (*fMuon)->track()->py();
	pt = (*fMuon)->track()->pt();
	eta = (*fMuon)->track()->eta();
	phi = (*fMuon)->track()->phi();
      }
      else {
	msg() << MSG::WARNING << "bad status word in muonboy term!!!, = " << fMuon.getParameter().statusWord() << endreq;
      }

      met_muonboy.add(px*fMuon.getParameter().wpx(),py*fMuon.getParameter().wpy(),
 	      pt*fMuon.getParameter().wet(),eta, 1.0);

      h_muonweightx->Fill(fMuon.getParameter().wpx());
      h_muonweightx_V_muonpt->Fill(pt/1000.,fMuon.getParameter().wpx());
      h_muonweightx_V_muonpx->Fill(px/1000.,fMuon.getParameter().wpx());
      h_muonweightx_V_muoneta->Fill(eta,fMuon.getParameter().wpx());
      h_muonweightx_V_muonphi->Fill(phi,fMuon.getParameter().wpx());
      h_muonweightx_V_muoneta_V_muonphi->Fill(eta,phi,fMuon.getParameter().wpx());
      h_muonweightx_V_muoneta_V_muonpt->Fill(eta,pt/1000.,fMuon.getParameter().wpx());

      h_muonweighty->Fill(fMuon.getParameter().wpy());
      h_muonweighty_V_muonpt->Fill(pt/1000.,fMuon.getParameter().wpy());
      h_muonweighty_V_muonpy->Fill(py/1000.,fMuon.getParameter().wpy());
      h_muonweighty_V_muoneta->Fill(eta,fMuon.getParameter().wpy());
      h_muonweighty_V_muonphi->Fill(phi,fMuon.getParameter().wpy());
      h_muonweighty_V_muoneta_V_muonphi->Fill(eta,phi,fMuon.getParameter().wpy());
      h_muonweighty_V_muoneta_V_muonpt->Fill(eta,pt/1000.,fMuon.getParameter().wpy());

      h_muonweightet->Fill(fMuon.getParameter().wet());
      h_muonweightet_V_muonpt->Fill(pt/1000.,fMuon.getParameter().wet());
      h_muonweightet_V_muoneta->Fill(eta,fMuon.getParameter().wet());
      h_muonweightet_V_muonphi->Fill(phi,fMuon.getParameter().wet());
      h_muonweightet_V_muoneta_V_muonphi->Fill(eta,phi,fMuon.getParameter().wet());
      h_muonweightet_V_muoneta_V_muonpt->Fill(eta,pt/1000.,fMuon.getParameter().wet());

      h_muonweightx_V_muonweighty->Fill(fMuon.getParameter().wpy(),fMuon.getParameter().wpx());
      h_muonweightx_V_muonweightet->Fill(fMuon.getParameter().wet(),fMuon.getParameter().wpx());

      if (_make_event_displays) {
	event_displays.back()->h_muoneta_V_phi_evt->Fill(eta,phi,pt*fMuon.getParameter().wet()/1000.);
      }

      nmuboy++;
    }
//   msg() << MSG::WARNING
// 	 << "met muonboy sums(px, py, et) = ("
// 	 << met_muonboy.etx() << ", "
// 	 << met_muonboy.ety() << ", "
// 	 << met_muonboy.sumet() << ")" << endreq;

  if (_make_event_displays) {
    msg() << MSG::WARNING << "MET_MuonBoy METx   = " << met_muonboy.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_MuonBoy METy   = " << met_muonboy.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_MuonBoy MET    = " << met_muonboy.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_MuonBoy METphi = " << met_muonboy.phi() << endreq;
    msg() << MSG::WARNING << "MET_MuonBoy SUMET  = " << met_muonboy.sumet()/1000. << endreq;
  }

  h_nmuboy_compmap->Fill(nmuboy);

  h_New_V_Old_MuonBoy_metx->Fill(data->MuonBoy()->etx()/1000.,met_muonboy.etx()/1000.);
  h_New_minus_Old_MuonBoy_metx->Fill(data->MuonBoy()->etx()/1000. - met_muonboy.etx()/1000.);

  h_New_V_Old_MuonBoy_mety->Fill(data->MuonBoy()->ety()/1000.,met_muonboy.ety()/1000.);
  h_New_minus_Old_MuonBoy_mety->Fill(data->MuonBoy()->ety()/1000. - met_muonboy.ety()/1000.);

  h_New_V_Old_MuonBoy_met->Fill(data->MuonBoy()->et()/1000.,met_muonboy.et()/1000.);
  h_New_minus_Old_MuonBoy_met->Fill(data->MuonBoy()->et()/1000. - met_muonboy.et()/1000.);

  ////////////////////////////////////////////////////////////////////////////////////
  //refmuon
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<Analysis::Muon> frefMuon(pComp->beginIterator<Analysis::Muon>());
  MissingETComposition::const_iterator<Analysis::Muon> lrefMuon(pComp->endIterator<Analysis::Muon>());

  unsigned int nrefmuon = 0;
  MissingET met_refmuon;

  for ( ; frefMuon != lrefMuon; frefMuon++)
    {
//       msg() << MSG::INFO
// 	     << "muon " << i << " px, py, pt, weight x, weight y, weight pt, status word= "
// 	     << (*frefMuon)->px() << ", "
// 	     << (*frefMuon)->py() << ", "
//  	     << (*frefMuon)->pt() << ", "
// 	     << frefMuon.getParameter().wpx() << ", "
//  	     << frefMuon.getParameter().wpy() << ", "
//  	     << frefMuon.getParameter().wet() << ", "
//  	     << frefMuon.getParameter().statusWord() << endreq;

      //get the right px,py,pt based on status word
      float px = 0.;
      float py = 0.;
      float pt = 0.;
      float eta = 0.;
      float phi = 0.;

      if (MissingETtags::usesREFMUON(frefMuon.getParameter().statusWord())) {
	//this muon is used in RefMuon term
	px = (*frefMuon)->px();
	py = (*frefMuon)->py();
	pt = (*frefMuon)->pt();
	eta = (*frefMuon)->eta();
	phi = (*frefMuon)->eta();
      }
      else {
	continue;
      }
      met_refmuon.add(px*frefMuon.getParameter().wpx(),py*frefMuon.getParameter().wpy(),
 	      pt*frefMuon.getParameter().wet(),eta, 1.0);

      h_refmuonweightx->Fill(frefMuon.getParameter().wpx());
      h_refmuonweightx_V_muonpt->Fill(pt/1000.,frefMuon.getParameter().wpx());
      h_refmuonweightx_V_muonpx->Fill(px/1000.,frefMuon.getParameter().wpx());
      h_refmuonweightx_V_muoneta->Fill(eta,frefMuon.getParameter().wpx());
      h_refmuonweightx_V_muonphi->Fill(phi,frefMuon.getParameter().wpx());
      h_refmuonweightx_V_muoneta_V_muonphi->Fill(eta,phi,frefMuon.getParameter().wpx());
      h_refmuonweightx_V_muoneta_V_muonpt->Fill(eta,pt/1000.,frefMuon.getParameter().wpx());

      h_refmuonweighty->Fill(frefMuon.getParameter().wpy());
      h_refmuonweighty_V_muonpt->Fill(pt/1000.,frefMuon.getParameter().wpy());
      h_refmuonweighty_V_muonpy->Fill(py/1000.,frefMuon.getParameter().wpy());
      h_refmuonweighty_V_muoneta->Fill(eta,frefMuon.getParameter().wpy());
      h_refmuonweighty_V_muonphi->Fill(phi,frefMuon.getParameter().wpy());
      h_refmuonweighty_V_muoneta_V_muonphi->Fill(eta,phi,frefMuon.getParameter().wpy());
      h_refmuonweighty_V_muoneta_V_muonpt->Fill(eta,pt/1000.,frefMuon.getParameter().wpy());

      h_refmuonweightet->Fill(frefMuon.getParameter().wet());
      h_refmuonweightet_V_muonpt->Fill(pt/1000.,frefMuon.getParameter().wet());
      h_refmuonweightet_V_muoneta->Fill(eta,frefMuon.getParameter().wet());
      h_refmuonweightet_V_muonphi->Fill(phi,frefMuon.getParameter().wet());

      h_refmuonweightx_V_refmuonweighty->Fill(frefMuon.getParameter().wpy(),frefMuon.getParameter().wpx());
      h_refmuonweightx_V_refmuonweightet->Fill(frefMuon.getParameter().wet(),frefMuon.getParameter().wpx());

      if (_make_event_displays) {
	event_displays.back()->h_refmuoneta_V_phi_evt->Fill(eta,phi,pt*frefMuon.getParameter().wet()/1000.);
      }

      nrefmuon++;
    }

  if (_make_event_displays) {
    msg() << MSG::WARNING << "MET_RefMuon METx   = " << met_refmuon.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefMuon METy   = " << met_refmuon.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefMuon MET    = " << met_refmuon.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefMuon METphi = " << met_refmuon.phi() << endreq;
    msg() << MSG::WARNING << "MET_RefMuon SUMET  = " << met_refmuon.sumet()/1000. << endreq;
  }

  h_nrefmuon_compmap->Fill(nrefmuon);

  h_New_V_Old_refMuon_metx->Fill(data->refMuon()->etx()/1000.,met_refmuon.etx()/1000.);
  h_New_minus_Old_refMuon_metx->Fill(data->refMuon()->etx()/1000. - met_refmuon.etx()/1000.);

  h_New_V_Old_refMuon_mety->Fill(data->refMuon()->ety()/1000.,met_refmuon.ety()/1000.);
  h_New_minus_Old_refMuon_mety->Fill(data->refMuon()->ety()/1000. - met_refmuon.ety()/1000.);

  h_New_V_Old_refMuon_met->Fill(data->refMuon()->et()/1000.,met_refmuon.et()/1000.);
  h_New_minus_Old_refMuon_met->Fill(data->refMuon()->et()/1000. - met_refmuon.et()/1000.);

  ////////////////////////////////////////////////////////////////////////////////////
  //CellOut
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<CaloCluster> fClus(pComp->beginIterator<CaloCluster>());
  MissingETComposition::const_iterator<CaloCluster> lClus(pComp->endIterator<CaloCluster>());

  unsigned int ncalolus = 0;
  MissingET met_caloClus;

  for ( ; fClus != lClus; fClus++ )
    {
      if (MissingETtags::usesEFLOW_CLUSTER(fClus.getParameter().statusWord())) {continue;}

//       (*fClus)->setSignalState(P4SignalState::CALIBRATED);
//       (*fClus)->setSignalState(P4SignalState::UNCALIBRATED);
//       (*fClus)->setSignalState(P4SignalState::ALTCALIBRATED);

//       msg() << MSG::INFO
// 	    << "cluster " << ic << " px, py, pt, eta, phi, weight x, weight y, weight pt, status word= "
// 	    << (*fClus)->px() << ", "
// 	    << (*fClus)->py() << ", "
// 	    << (*fClus)->pt() << ", "
// 	    << (*fClus)->eta() << ", "
// 	    << (*fClus)->phi() << ", "
// 	    << fClus.getParameter().wpx() << ", "
// 	    << fClus.getParameter().wpy() << ", "
// 	    << fClus.getParameter().wet() << ", "
// 	    << fClus.getParameter().statusWord() << endreq;

//       NavigationToken<CaloCell,double> cToken;
//       (*fClus)->fillToken(cToken,1.0);
//       NavigationToken<CaloCell,double>::const_iterator fCell = cToken.begin();
//       NavigationToken<CaloCell,double>::const_iterator lCell = cToken.end();
//       for ( ;  fCell != lCell; fCell++ ) {
// 	float cellweight = fCell.getParameter();
// 	float cellpx = (*fCell)->px();
// 	float cellpy = (*fCell)->py();
// 	float cellpt = (*fCell)->pt();
// 	float cellet = (*fCell)->et();
//       }

      met_caloClus.add((*fClus)->px()*fClus.getParameter().wpx(),(*fClus)->py()*fClus.getParameter().wpy(),
	      (*fClus)->pt()*fClus.getParameter().wet(),
	      (*fClus)->eta(),     
	      1.0);

      h_clusweightx->Fill(fClus.getParameter().wpx());
      h_clusweightx_V_cluspt->Fill((*fClus)->pt()/1000.,fClus.getParameter().wpx());
      h_clusweightx_V_cluspx->Fill((*fClus)->px()/1000.,fClus.getParameter().wpx());
      h_clusweightx_V_cluseta->Fill((*fClus)->eta(),fClus.getParameter().wpx());
      h_clusweightx_V_clusphi->Fill((*fClus)->phi(),fClus.getParameter().wpx());
      h_clusweightx_V_cluseta_V_clusphi->Fill((*fClus)->eta(),(*fClus)->phi(),fClus.getParameter().wpx());
      h_clusweightx_V_cluseta_V_cluspt->Fill((*fClus)->eta(),(*fClus)->pt()/1000.,fClus.getParameter().wpx());

      h_clusweighty->Fill(fClus.getParameter().wpy());
      h_clusweighty_V_cluspt->Fill((*fClus)->pt()/1000.,fClus.getParameter().wpy());
      h_clusweighty_V_cluspy->Fill((*fClus)->py()/1000.,fClus.getParameter().wpy());
      h_clusweighty_V_cluseta->Fill((*fClus)->eta(),fClus.getParameter().wpy());
      h_clusweighty_V_clusphi->Fill((*fClus)->phi(),fClus.getParameter().wpy());
      h_clusweighty_V_cluseta_V_clusphi->Fill((*fClus)->eta(),(*fClus)->phi(),fClus.getParameter().wpy());
      h_clusweighty_V_cluseta_V_cluspt->Fill((*fClus)->eta(),(*fClus)->pt()/1000.,fClus.getParameter().wpy());

      h_clusweightet->Fill(fClus.getParameter().wet());
      h_clusweightet_V_cluspt->Fill((*fClus)->pt()/1000.,fClus.getParameter().wet());
      h_clusweightet_V_cluseta->Fill((*fClus)->eta(),fClus.getParameter().wet());
      h_clusweightet_V_clusphi->Fill((*fClus)->phi(),fClus.getParameter().wet());
      h_clusweightet_V_cluseta_V_clusphi->Fill((*fClus)->eta(),(*fClus)->phi(),fClus.getParameter().wet());
      h_clusweightet_V_cluseta_V_cluspt->Fill((*fClus)->eta(),(*fClus)->pt()/1000.,fClus.getParameter().wet());

      h_clusweightx_V_clusweighty->Fill(fClus.getParameter().wpy(),fClus.getParameter().wpx());
      h_clusweightx_V_clusweightet->Fill(fClus.getParameter().wet(),fClus.getParameter().wpx());

      if (_make_event_displays) {
	event_displays.back()->h_cluseta_V_phi_evt->Fill((*fClus)->eta(),(*fClus)->phi(),(*fClus)->pt()*fClus.getParameter().wet()/1000.);
      }

      ncalolus++;
    }//end loop over CaloClusters in composition map

//   msg() << MSG::INFO
// 	<< "met clus sums(px, py, et) = ("
// 	<< met_caloClus.etx() << ", "
// 	<< met_caloClus.ety() << ", "
// 	<< met_caloClus.sumet() << ")" << endreq;

  if (_make_event_displays) {
    msg() << MSG::WARNING << "MET_CellOut METx   = " << met_caloClus.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_CellOut METy   = " << met_caloClus.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_CellOut MET    = " << met_caloClus.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_CellOut METphi = " << met_caloClus.phi() << endreq;
    msg() << MSG::WARNING << "MET_CellOut SUMET  = " << met_caloClus.sumet()/1000. << endreq;
  }

  msg() << MSG::WARNING << "METComp, nClusters=" << ncalolus << endreq;
  h_nclus_compmap->Fill(ncalolus);

  h_New_V_Old_CellOut_metx->Fill(data->cellOut()->etx()/1000.,met_caloClus.etx()/1000.);
  h_New_minus_Old_CellOut_metx->Fill(data->cellOut()->etx()/1000. - met_caloClus.etx()/1000.);

  h_New_V_Old_CellOut_mety->Fill(data->cellOut()->ety()/1000.,met_caloClus.ety()/1000.);
  h_New_minus_Old_CellOut_mety->Fill(data->cellOut()->ety()/1000. - met_caloClus.ety()/1000.);

  h_New_V_Old_CellOut_met->Fill(data->cellOut()->et()/1000.,met_caloClus.et()/1000.);
  h_New_minus_Old_CellOut_met->Fill(data->cellOut()->et()/1000. - met_caloClus.et()/1000.);


  ////////////////////////////////////////////////////////////////////////////////////
  //CellOut_Eflow
  ////////////////////////////////////////////////////////////////////////////////////
  MissingETComposition::const_iterator<CaloCluster> efClus(pComp->beginIterator<CaloCluster>());
  MissingETComposition::const_iterator<CaloCluster> elClus(pComp->endIterator<CaloCluster>());

  unsigned int neflow_clus = 0;
  unsigned int neflow_trks = 0;
  MissingET met_eflow;

  for ( ; efClus != elClus; efClus++ )
    {
      if (!(MissingETtags::usesEFLOW_CLUSTER(efClus.getParameter().statusWord()))) {continue;}

      met_eflow.add((*efClus)->px()*efClus.getParameter().wpx(),(*efClus)->py()*efClus.getParameter().wpy(),
	      (*efClus)->pt()*efClus.getParameter().wet(),
	      (*efClus)->eta(),     
	      1.0);

      h_eflow_clusweightx->Fill(efClus.getParameter().wpx());
      h_eflow_clusweightx_V_cluspt->Fill((*efClus)->pt()/1000.,efClus.getParameter().wpx());
      h_eflow_clusweightx_V_cluspx->Fill((*efClus)->px()/1000.,efClus.getParameter().wpx());
      h_eflow_clusweightx_V_cluseta->Fill((*efClus)->eta(),efClus.getParameter().wpx());
      h_eflow_clusweightx_V_clusphi->Fill((*efClus)->phi(),efClus.getParameter().wpx());
      h_eflow_clusweightx_V_cluseta_V_clusphi->Fill((*efClus)->eta(),(*efClus)->phi(),efClus.getParameter().wpx());
      h_eflow_clusweightx_V_cluseta_V_cluspt->Fill((*efClus)->eta(),(*efClus)->pt()/1000.,efClus.getParameter().wpx());

      h_eflow_clusweighty->Fill(efClus.getParameter().wpy());
      h_eflow_clusweighty_V_cluspt->Fill((*efClus)->pt()/1000.,efClus.getParameter().wpy());
      h_eflow_clusweighty_V_cluspy->Fill((*efClus)->py()/1000.,efClus.getParameter().wpy());
      h_eflow_clusweighty_V_cluseta->Fill((*efClus)->eta(),efClus.getParameter().wpy());
      h_eflow_clusweighty_V_clusphi->Fill((*efClus)->phi(),efClus.getParameter().wpy());
      h_eflow_clusweighty_V_cluseta_V_clusphi->Fill((*efClus)->eta(),(*efClus)->phi(),efClus.getParameter().wpy());
      h_eflow_clusweighty_V_cluseta_V_cluspt->Fill((*efClus)->eta(),(*efClus)->pt()/1000.,efClus.getParameter().wpy());

      h_eflow_clusweightet->Fill(efClus.getParameter().wet());
      h_eflow_clusweightet_V_cluspt->Fill((*efClus)->pt()/1000.,efClus.getParameter().wet());
      h_eflow_clusweightet_V_cluseta->Fill((*efClus)->eta(),efClus.getParameter().wet());
      h_eflow_clusweightet_V_clusphi->Fill((*efClus)->phi(),efClus.getParameter().wet());
      h_eflow_clusweightet_V_cluseta_V_clusphi->Fill((*efClus)->eta(),(*efClus)->phi(),efClus.getParameter().wet());
      h_eflow_clusweightet_V_cluseta_V_cluspt->Fill((*efClus)->eta(),(*efClus)->pt()/1000.,efClus.getParameter().wet());

      h_eflow_clusweightx_V_clusweighty->Fill(efClus.getParameter().wpy(),efClus.getParameter().wpx());
      h_eflow_clusweightx_V_clusweightet->Fill(efClus.getParameter().wet(),efClus.getParameter().wpx());

      neflow_clus++;
    }//end loop over CaloClusters in composition map

  msg() << MSG::WARNING << "METComp, nClusters used in eflow=" << neflow_clus << endreq;

  //tracks
  MissingETComposition::const_iterator<Rec::TrackParticle> efTrk(pComp->beginIterator<Rec::TrackParticle>());
  MissingETComposition::const_iterator<Rec::TrackParticle> elTrk(pComp->endIterator<Rec::TrackParticle>());

  for ( ; efTrk != elTrk; efTrk++ )
    {

      met_eflow.add((*efTrk)->px()*efTrk.getParameter().wpx(),(*efTrk)->py()*efTrk.getParameter().wpy(),
	      (*efTrk)->pt()*efTrk.getParameter().wet(),
	      (*efTrk)->eta(),     
	      1.0);

      h_eflow_trkweightx->Fill(efTrk.getParameter().wpx());
      h_eflow_trkweightx_V_trkpt->Fill((*efTrk)->pt()/1000.,efTrk.getParameter().wpx());
      h_eflow_trkweightx_V_trkpx->Fill((*efTrk)->px()/1000.,efTrk.getParameter().wpx());
      h_eflow_trkweightx_V_trketa->Fill((*efTrk)->eta(),efTrk.getParameter().wpx());
      h_eflow_trkweightx_V_trkphi->Fill((*efTrk)->phi(),efTrk.getParameter().wpx());
      h_eflow_trkweightx_V_trketa_V_trkphi->Fill((*efTrk)->eta(),(*efTrk)->phi(),efTrk.getParameter().wpx());
      h_eflow_trkweightx_V_trketa_V_trkpt->Fill((*efTrk)->eta(),(*efTrk)->pt()/1000.,efTrk.getParameter().wpx());

      h_eflow_trkweighty->Fill(efTrk.getParameter().wpy());
      h_eflow_trkweighty_V_trkpt->Fill((*efTrk)->pt()/1000.,efTrk.getParameter().wpy());
      h_eflow_trkweighty_V_trkpy->Fill((*efTrk)->py()/1000.,efTrk.getParameter().wpy());
      h_eflow_trkweighty_V_trketa->Fill((*efTrk)->eta(),efTrk.getParameter().wpy());
      h_eflow_trkweighty_V_trkphi->Fill((*efTrk)->phi(),efTrk.getParameter().wpy());
      h_eflow_trkweighty_V_trketa_V_trkphi->Fill((*efTrk)->eta(),(*efTrk)->phi(),efTrk.getParameter().wpy());
      h_eflow_trkweighty_V_trketa_V_trkpt->Fill((*efTrk)->eta(),(*efTrk)->pt()/1000.,efTrk.getParameter().wpy());

      h_eflow_trkweightet->Fill(efTrk.getParameter().wet());
      h_eflow_trkweightet_V_trkpt->Fill((*efTrk)->pt()/1000.,efTrk.getParameter().wet());
      h_eflow_trkweightet_V_trketa->Fill((*efTrk)->eta(),efTrk.getParameter().wet());
      h_eflow_trkweightet_V_trkphi->Fill((*efTrk)->phi(),efTrk.getParameter().wet());
      h_eflow_trkweightet_V_trketa_V_trkphi->Fill((*efTrk)->eta(),(*efTrk)->phi(),efTrk.getParameter().wet());
      h_eflow_trkweightet_V_trketa_V_trkpt->Fill((*efTrk)->eta(),(*efTrk)->pt()/1000.,efTrk.getParameter().wet());

      h_eflow_trkweightx_V_trkweighty->Fill(efTrk.getParameter().wpy(),efTrk.getParameter().wpx());
      h_eflow_trkweightx_V_trkweightet->Fill(efTrk.getParameter().wet(),efTrk.getParameter().wpx());

      neflow_trks++;
    }

  msg() << MSG::WARNING << "METComp, nTrks used in eflow=" << neflow_trks << endreq;
  h_eflow_nclus_compmap->Fill(neflow_clus);
  h_eflow_ntrks_compmap->Fill(neflow_trks);

  h_New_V_Old_CellOutEflow_metx->Fill(data->cellOutEFlow()->etx()/1000.,met_eflow.etx()/1000.);
  h_New_minus_Old_CellOutEflow_metx->Fill(data->cellOutEFlow()->etx()/1000. - met_eflow.etx()/1000.);

  h_New_V_Old_CellOutEflow_mety->Fill(data->cellOutEFlow()->ety()/1000.,met_eflow.ety()/1000.);
  h_New_minus_Old_CellOutEflow_mety->Fill(data->cellOutEFlow()->ety()/1000. - met_eflow.ety()/1000.);

  h_New_V_Old_CellOutEflow_met->Fill(data->cellOutEFlow()->et()/1000.,met_eflow.et()/1000.);
  h_New_minus_Old_CellOutEflow_met->Fill(data->cellOutEFlow()->et()/1000. - met_eflow.et()/1000.);

  if (_make_event_displays) {
    MissingET met_refFinal;

    //leave out Cryo term
    met_refFinal.setEx(met_refjet.etx()+met_reftau.etx()+met_refele.etx()+met_refgamma.etx()+met_muonboy.etx()+met_refmuon.etx()+met_caloClus.etx());
    met_refFinal.setEy(met_refjet.ety()+met_reftau.ety()+met_refele.ety()+met_refgamma.ety()+met_muonboy.ety()+met_refmuon.ety()+met_caloClus.ety());
    met_refFinal.setEtSum(met_refjet.sumet()+met_reftau.sumet()+met_refele.sumet()+met_refgamma.sumet()+met_muonboy.sumet()+met_refmuon.sumet()+met_caloClus.sumet());

    msg() << MSG::WARNING << "MET_RefFinal (no cryo) METx   = " << met_refFinal.etx()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefFinal (no cryo) METy   = " << met_refFinal.ety()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefFinal (no cryo) MET    = " << met_refFinal.et()/1000. << endreq;
    msg() << MSG::WARNING << "MET_RefFinal (no cryo) METphi = " << met_refFinal.phi() << endreq;
    msg() << MSG::WARNING << "MET_RefFinal (no cryo) SUMET  = " << met_refFinal.sumet()/1000. << endreq;
  }

  if (sc.isFailure()) { msg() << MSG::WARNING << "MissingETCompositionTool missingETCompositionPlots() fail" << endreq; }

  return sc;
}

//------------------------------------------------------------------------------
StatusCode MissingETCompositionTool::finalize() {
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
MissingETCompositionTool::~MissingETCompositionTool() {}

evt_display::evt_display(int run, int event, ITHistSvc *m_thistSvc, const std::string &m_folderName, MsgStream* mLog) {
  
  //msg() << MSG::WARNING << "in evt_display" << endreq;

  char jetmessage[50];
  sprintf(jetmessage,"jeteta_V_phi_run_%devent_%d" , run, event );
  char muonmessage[50];
  sprintf(muonmessage,"muoneta_V_phi_run_%devent_%d" , run, event );
  char refmuonmessage[50];
  sprintf(refmuonmessage,"refmuoneta_V_phi_run_%devent_%d" , run, event );
  char muidmessage[50];
  sprintf(muidmessage,"muideta_V_phi_run_%devent_%d" , run, event );
  char clusmessage[50];
  sprintf(clusmessage,"cluseta_V_phi_run_%devent_%d" , run, event );
  char elemessage[50];
  sprintf(elemessage,"eleeta_V_phi_run_%devent_%d" , run, event );
  char gammamessage[50];
  sprintf(gammamessage,"gammaeta_V_phi_run_%devent_%d" , run, event );
  char taumessage[50];
  sprintf(taumessage,"taueta_V_phi_run_%devent_%d" , run, event );

  h_jeteta_V_phi_evt = new TH2F(jetmessage,jetmessage,100,-5.,5.,63,-3.15,3.15);
  StatusCode sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/"+std::string(jetmessage),h_jeteta_V_phi_evt);

  h_eleeta_V_phi_evt = new TH2F(elemessage,elemessage,100,-5.,5.,63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/"+std::string(elemessage),h_eleeta_V_phi_evt);

  h_muoneta_V_phi_evt = new TH2F(muonmessage,muonmessage,100,-5.,5.,63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/"+std::string(muonmessage),h_muoneta_V_phi_evt);

  h_refmuoneta_V_phi_evt = new TH2F(refmuonmessage,refmuonmessage,100,-5.,5.,63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/"+std::string(refmuonmessage),h_refmuoneta_V_phi_evt);

  h_muideta_V_phi_evt = new TH2F(muidmessage,muidmessage,100,-5.,5.,63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/"+std::string(muidmessage),h_muideta_V_phi_evt);

  h_cluseta_V_phi_evt = new TH2F(clusmessage,clusmessage,100,-5.,5.,63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/"+std::string(clusmessage),h_cluseta_V_phi_evt);

  h_gammaeta_V_phi_evt = new TH2F(gammamessage,gammamessage,100,-5.,5.,63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/"+std::string(gammamessage),h_gammaeta_V_phi_evt);

  h_taueta_V_phi_evt = new TH2F(taumessage,taumessage,100,-5.,5.,63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "MissingETComposition/"+std::string(taumessage),h_taueta_V_phi_evt);

  if (sc.isFailure()) {*mLog << MSG::WARNING << "failure in evt_display" << endreq;}

}

evt_display::~evt_display() {
  if (h_jeteta_V_phi_evt) delete h_jeteta_V_phi_evt;
  if (h_eleeta_V_phi_evt) delete h_eleeta_V_phi_evt;
  if (h_muoneta_V_phi_evt) delete h_muoneta_V_phi_evt;
  if (h_muideta_V_phi_evt) delete h_muideta_V_phi_evt;
  if (h_cluseta_V_phi_evt) delete h_cluseta_V_phi_evt;
  if (h_taueta_V_phi_evt) delete h_taueta_V_phi_evt;
  if (h_gammaeta_V_phi_evt) delete h_gammaeta_V_phi_evt;
}
