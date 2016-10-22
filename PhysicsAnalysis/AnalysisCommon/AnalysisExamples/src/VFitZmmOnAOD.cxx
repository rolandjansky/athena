/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"


#include "TH1.h"
#include "TH2.h"

// The Muon
#include "muonEvent/Muon.h"

// #include "TrkParticleBase/TrackParticleBase.h"

// PDT particle property
#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"

// Constituent navigation
#include "Navigation/NavigationToken.h"

// common implementation of all particles
#include "ParticleEvent/ParticleBaseContainer.h"

// vertex
#include "VxVertex/VxContainer.h"   
#include "VxVertex/RecVertex.h"

// xAOD vertex
#include "xAODTracking/VertexContainer.h"

// the composite particle
#include "ParticleEvent/CompositeParticle.h"

// particle jets
#include "JetEvent/JetCollection.h"

// analysis tools
#include "AnalysisUtils/AnalysisCombination.h"

// Event Info
#include "xAODEventInfo/EventInfo.h"

// the header file
#include "VFitZmmOnAOD.h"

#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <functional>

// static const double mMuon = 105.658369;
static double mMuon = 105.66; // MeV

const xAOD::Vertex* primVtx=NULL;

using namespace Analysis;


//////////////////////////////////////////////////////////////////////////////////////
// Constructor

VFitZmmOnAOD::VFitZmmOnAOD(const std::string& name,ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_analysisTools( "AnalysisTools", this ),
  m_thistSvc(nullptr),
  m_VKVrtFitter("Trk::TrkVKalVrtFitter", this),
  m_tree_Zll(nullptr),
  m_v0_x(0),
  m_v0_y(0),
  m_v0_z(0),
  m_nZ(0),
  m_Zmode(nullptr),
  m_Zm_vkfit(nullptr),
  m_Zchi2_vkfit(nullptr),
  m_Zdxy_vkfit(nullptr),
  m_Zdz_vkfit(nullptr),
  m_Zm(nullptr),
  m_Zpt(nullptr),
  m_Zpx(nullptr),
  m_Zpy(nullptr),
  m_Zpz(nullptr),
  m_Zp(nullptr),
  m_Ze(nullptr),
  m_Zeta(nullptr),
  m_L1pt_vkfit(nullptr),
  m_L1pt(nullptr),
  m_L1px(nullptr),
  m_L1py(nullptr),
  m_L1pz(nullptr),
  m_L1p(nullptr),
  m_L1e(nullptr),
  m_L1eta(nullptr),
  m_L2pt_vkfit(nullptr),
  m_L2pt(nullptr),
  m_L2px(nullptr),
  m_L2py(nullptr),
  m_L2pz(nullptr),
  m_L2p(nullptr),
  m_L2e(nullptr),
  m_L2eta(nullptr),
  m_runNumber(0),
  m_eventNumber(0),
  m_eventTime(0),
  m_lumiBlock(0),
  m_bCID(0),
  m_lVL1ID(0),
  m_eventWeight(0),
  m_aod_muon_pt(nullptr),
  m_aod_muon_eta(nullptr),
  m_aod_muon_chi2(nullptr),
  m_aod_zmm_mass_hist(nullptr),
  m_aod_muon_charge(nullptr)
{
  // switches to control the analysis through job options :: these are the default
  // to changed in the job options

  // The Electron AOD container name & selection cuts
  declareProperty( "AnalysisTools", m_analysisTools );

  // The Muon AOD container name & selection cuts
  declareProperty("MuonContainer", m_muonContainerName = "MuonCollection");
  declareProperty("MuonEtCut", m_etMuonCut = 6.0*CLHEP::GeV);
  declareProperty("MuonEtaCut", m_etaMuonCut = 2.5);

  // Name of the Vertex container
  declareProperty("VxContainer", m_VxContainerName= "VxPrimaryCandidate",
                                 "Name of the Vertex container");

  // vertex fitter
  declareProperty("TrkVKalVrtFitterTool", m_ToolIVrtFitter);

  }


/////////////////////////////////////////////////////////////////////////////////////
// Destructor - check up memory allocation
// delete any memory allocation on the heap

VFitZmmOnAOD::~VFitZmmOnAOD() {}

////////////////////////////////////////////////////////////////////////////////////
// Initialize
// get a handle on the analysis tools
// book histograms

StatusCode VFitZmmOnAOD::initialize() {

  ATH_MSG_INFO("Initializing VFitZmmOnAOD");

  // get tool svc
  //
  IToolSvc* toolSvc;
  StatusCode sc = service("ToolSvc",toolSvc); 
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR("Unable to retrieve ToolSvc");
    return StatusCode::FAILURE;
  }
  

//  VKalVrt vertex fitter
    if (m_VKVrtFitter.retrieve().isFailure()) {
      ATH_MSG_WARNING("Can't find Trk::TrkVKalVrtFitter");
      return StatusCode::SUCCESS; 
    } else {
      ATH_MSG_INFO("Trk::TrkVKalVrtFitter found");
    }

  // get Particle Property service
  IPartPropSvc* p_PartPropSvc = 0;
  sc = service("PartPropSvc", p_PartPropSvc, true);
  if ( sc.isFailure() || 0 == p_PartPropSvc) {
    ATH_MSG_ERROR("Unable to initialize Particle Properties Service");
    return StatusCode::FAILURE;;
  }
  const HepPDT::ParticleDataTable* particleTable = p_PartPropSvc->PDT();
  const HepPDT::ParticleData* muonPDT = particleTable->particle(13);
  ATH_MSG_DEBUG(":muonPDT: mass=" << muonPDT->mass() << ", id=" << muonPDT->pid() << ", name=" << muonPDT->name() );
  mMuon = muonPDT->mass();

  // get a handle on the analysis tools
  sc = m_analysisTools.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get handle on analysis tools");
    return sc;
  }

  // Return a pointer to THistSvc
  sc = service("THistSvc", m_thistSvc);
  if(sc.isFailure() ){
    ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
    return sc;
  }
  m_Zmode = new std::vector<int>;

  m_Zm_vkfit  = new std::vector<double>;
  m_Zchi2_vkfit  = new std::vector<double>;
  m_Zdxy_vkfit  = new std::vector<double>;
  m_Zdz_vkfit  = new std::vector<double>;

  m_Zm  = new std::vector<double>;
  m_Zpt = new std::vector<double>;
  m_Zpx = new std::vector<double>;
  m_Zpy = new std::vector<double>;
  m_Zpz = new std::vector<double>;
  m_Zp  = new std::vector<double>;
  m_Ze  = new std::vector<double>;
  m_Zeta = new std::vector<double>;

  m_L1pt_vkfit = new std::vector<double>;
  m_L1pt = new std::vector<double>;
  m_L1px = new std::vector<double>;
  m_L1py = new std::vector<double>;
  m_L1pz = new std::vector<double>;
  m_L1e  = new std::vector<double>;
  m_L1p  = new std::vector<double>;
  m_L1eta = new std::vector<double>;

  m_L2pt_vkfit = new std::vector<double>;
  m_L2pt = new std::vector<double>;
  m_L2px = new std::vector<double>;
  m_L2py = new std::vector<double>;
  m_L2pz = new std::vector<double>;
  m_L2e  = new std::vector<double>;
  m_L2p  = new std::vector<double>;
  m_L2eta = new std::vector<double>;
  //
  // event info variables
  /*
  // remove. see comment in addEventInfo()
  m_lvl1TriggerInfo = new std::vector<unsigned int>; 
  m_lvl2TriggerInfo = new std::vector<unsigned int>; 
  m_evtFilterInfo   = new std::vector<unsigned int>;   
  m_streamTagName   = new std::vector<std::string>;   
  m_streamTagType   = new std::vector<std::string>;   
  */
  // the TTree
  m_tree_Zll = new TTree("tree_Zll","TTree of Z->l+l-");
  sc = m_thistSvc->regTree("/AANT/tree_Zll", m_tree_Zll);

  // first add Event info stuff
  m_tree_Zll->Branch("Run",  &m_runNumber,   "Run/I");    // run number
  m_tree_Zll->Branch("Event",&m_eventNumber, "Event/I");  // event number
  m_tree_Zll->Branch("Time", &m_eventTime,   "Time/I");   // time stamp
  m_tree_Zll->Branch("LumiBlock", &m_lumiBlock,"LumiBlock/I"); // lum block num 
  m_tree_Zll->Branch("BCID", &m_bCID,"BCID/I"); // bunch crossing ID
  m_tree_Zll->Branch("LVL1ID", &m_lVL1ID,"LVL1ID/I"); // trigger LVL1 ID
  m_tree_Zll->Branch("Weight", &m_eventWeight, "Weight/D"); // weight

  /*
  // remove. see comment in addEventInfo()
  m_tree_Zll->Branch("StatusElement",  &m_statusElement, "StatusElement/I");
  m_tree_Zll->Branch("LVL1TriggerType",  &m_lvl1TriggerType, "LVL1TriggerType/I");
  m_tree_Zll->Branch("LVL1TriggerInfo",&m_lvl1TriggerInfo);
  m_tree_Zll->Branch("LVL2TriggerInfo",&m_lvl2TriggerInfo);
  m_tree_Zll->Branch("EventFilterInfo",&m_evtFilterInfo);
  m_tree_Zll->Branch("StreamTagName",&m_streamTagName);
  m_tree_Zll->Branch("StreamTagType",&m_streamTagType);
  */  
  // now variables from algorithm
  m_tree_Zll->Branch("v0_x", &m_v0_x, "v0_x/D");
  m_tree_Zll->Branch("v0_y", &m_v0_y, "v0_y/D");
  m_tree_Zll->Branch("v0_z", &m_v0_z, "v0_z/D");

  m_tree_Zll->Branch("nZ", &m_nZ, "nZ/I");
  m_tree_Zll->Branch("Zmode", &m_Zmode);

  m_tree_Zll->Branch("Zm_vkfit", &m_Zm_vkfit);
  m_tree_Zll->Branch("Zchi2_vkfit", &m_Zchi2_vkfit);
  m_tree_Zll->Branch("Zdxy_vkfit", &m_Zdxy_vkfit);
  m_tree_Zll->Branch("Zdz_vkfit", &m_Zdz_vkfit);

  m_tree_Zll->Branch("Zm", &m_Zm);
  m_tree_Zll->Branch("Zpt", &m_Zpt);
  m_tree_Zll->Branch("Zpx", &m_Zpx);
  m_tree_Zll->Branch("Zpy", &m_Zpy);
  m_tree_Zll->Branch("Zpz", &m_Zpz);
  m_tree_Zll->Branch("Ze",  &m_Ze);
  m_tree_Zll->Branch("Zp",  &m_Zp);
  m_tree_Zll->Branch("Zeta", &m_Zeta);

  m_tree_Zll->Branch("L1pt_vkfit", &m_L1pt_vkfit);
  m_tree_Zll->Branch("L1pt", &m_L1pt);
  m_tree_Zll->Branch("L1px", &m_L1px);
  m_tree_Zll->Branch("L1py", &m_L1py);
  m_tree_Zll->Branch("L1pz", &m_L1pz);
  m_tree_Zll->Branch("L1e",  &m_L1e);
  m_tree_Zll->Branch("L1p",  &m_L1p);
  m_tree_Zll->Branch("L1eta", &m_L1eta);

  m_tree_Zll->Branch("L2pt_vkfit", &m_L2pt_vkfit);
  m_tree_Zll->Branch("L2pt", &m_L2pt);
  m_tree_Zll->Branch("L2px", &m_L2px);
  m_tree_Zll->Branch("L2py", &m_L2py);
  m_tree_Zll->Branch("L2pz", &m_L2pz);
  m_tree_Zll->Branch("L2e",  &m_L2e);
  m_tree_Zll->Branch("L2p",  &m_L2p);
  m_tree_Zll->Branch("L2eta", &m_L2eta);

  // the histograms

  // Muon histogram booking 
  m_aod_muon_pt        = new TH1F("aod_muon_pt","aod pt mu",50,0,250.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/Muon/aod_muon_pt", m_aod_muon_pt);
  m_aod_muon_eta       = new TH1F("aod_muon_eta","aod eta mu",70,-3.5,3.5);
  sc = m_thistSvc->regHist("/AANT/Muon/aod_muon_eta", m_aod_muon_eta);
  m_aod_muon_chi2      = new TH1F("aod_muon_chi2","aod chi2 mu",200,0.0,1000.0);
  sc = m_thistSvc->regHist("/AANT/Muon/aod_muon_chi2", m_aod_muon_chi2);
  m_aod_zmm_mass_hist  = new TH1F("Mmm","Mmm",50,0,250.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/Muon/Mmm", m_aod_zmm_mass_hist);
  m_aod_muon_charge = new TH1F("Muon_charge","Muon_charge",10,-5,5);
  sc = m_thistSvc->regHist("/AANT/Muon/Muon_charge", m_aod_muon_charge);

  if( sc.isFailure() ){
    ATH_MSG_ERROR("ROOT Hist aod_zmm_mass_hist registration failed");
    return sc;
  }
  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
// Finalize - delete any memory allocation from the heap

StatusCode VFitZmmOnAOD::finalize() {
  return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
// initialization before processing a new event
StatusCode VFitZmmOnAOD::initEvent() {

  ATH_MSG_DEBUG("initEvent()");

  m_v0_x = -99;
  m_v0_y = -99;
  m_v0_z = -99;
  
  m_nZ = 0;

  m_Zmode->clear();

  m_Zm_vkfit->clear();
  m_Zchi2_vkfit->clear();
  m_Zdxy_vkfit->clear();
  m_Zdz_vkfit->clear();

  m_Zm->clear();
  m_Zpt->clear();  
  m_Zpx->clear();
  m_Zpy->clear();
  m_Zpz->clear();
  m_Zp->clear();
  m_Ze->clear();
  m_Zeta->clear();

  m_L1pt_vkfit->clear();  
  m_L1pt->clear();
  m_L1px->clear();
  m_L1py->clear();
  m_L1pz->clear();
  m_L1p->clear();
  m_L1e->clear();
  m_L1eta->clear();

  m_L2pt_vkfit->clear();  
  m_L2pt->clear();  
  m_L2px->clear();
  m_L2py->clear();
  m_L2pz->clear();
  m_L2p->clear();
  m_L2e->clear();
  m_L2eta->clear();

  //
  m_runNumber=0;
  m_eventNumber=0;
  m_eventTime=0;
  m_lumiBlock=0;
  m_bCID=0;
  m_lVL1ID=0;
  m_eventWeight=0;
  /*
  // remove. see comment in addEventInfo()
  m_statusElement=0;
  m_lvl1TriggerType=0;
  m_lvl1TriggerInfo->clear();
  m_lvl2TriggerInfo->clear();
  m_evtFilterInfo->clear();
  m_streamTagName->clear();
  m_streamTagType->clear();
  */
  //

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
// Execute - called by the event loop on event by event

StatusCode VFitZmmOnAOD::execute() {

  ATH_MSG_DEBUG("execute()");

  StatusCode sc = StatusCode::SUCCESS;

  // initialize first before processing each event
  sc = initEvent();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("initEvent failed. Continue");
  }

  // add event info to ntuple
  sc = addEventInfo();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("could not get EventInfo. Continue");
  }

  // Get primary vertex from StoreGate
  const xAOD::VertexContainer* vxCont(0);
  sc = evtStore()->retrieve(vxCont,m_VxContainerName);
  if (sc.isFailure() ) {
    ATH_MSG_WARNING("No Vertex conainter with key "
		    << m_VxContainerName << " found in StoreGate. Continue");
  } else {
    ATH_MSG_VERBOSE("Found primary vertex info: " <<  m_VxContainerName);
    if(vxCont) {
      // int npvx = vxCont->size();
      xAOD::VertexContainer::const_iterator fz = vxCont->begin();
      primVtx = (*fz);
      m_v0_x = primVtx->position().x();
      m_v0_y = primVtx->position().y();
      m_v0_z = primVtx->position().z();
    }
  }

  // do the Z->mm reconstruction on AOD
  sc = zmm_on_aod();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Z->mm reconstruction on AOD failed");
    return StatusCode::SUCCESS;
  }

  m_tree_Zll->Fill();  
  //
  if(sc.isFailure()) sc = StatusCode::SUCCESS;
  return sc;

}


//////////////////////////////////////////////////////////////////////////////////
// zmm on aod: called by execute()

StatusCode VFitZmmOnAOD::zmm_on_aod() {

  ATH_MSG_DEBUG("zmm_on_aod()");

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve the user container of pre selected muon from TDS
  const MuonContainer* muonTDS=0;
  sc= evtStore()->retrieve( muonTDS, m_muonContainerName);
  if( sc.isFailure()  ||  !muonTDS ) {
    ATH_MSG_WARNING("No AOD muon container of muons found in TDS");
    return StatusCode::SUCCESS;
  }  
  ATH_MSG_DEBUG("MuonContainer successfully retrieved");

  // iterators over the container 
  MuonContainer::const_iterator muonItr  = muonTDS->begin();
  MuonContainer::const_iterator muonItrE = muonTDS->end();
  int imuon=0;
  for (; muonItr != muonItrE; ++muonItr) {
     imuon++;
     m_aod_muon_charge->Fill( (*muonItr)->charge() );
     m_aod_muon_pt->Fill( (*muonItr)->pt(), 1.);
     m_aod_muon_eta->Fill( (*muonItr)->eta(), 1.);
     m_aod_muon_chi2->Fill( (*muonItr)->matchChi2OverDoF(), 1.);
     ATH_MSG_DEBUG("imuon=" <<imuon<< ", AthenaBarCode=" << (*muonItr)->getAthenaBarCode());
  }

  // get the size of the selected muon container
  int nMuons = muonTDS->size();

  // if there are more than one muon in this container
  // then find Z candidate as a composite particle of 2 muons
  if (nMuons > 1) {

    // inputs for vertex fitter
    std::vector<const Rec::TrackParticle *> myTracks;
    std::vector<const Trk::TrackParticleBase *> myTrackBases;
    std::vector<double> myMuonMasses;

    xAOD::Vertex primaryVtx = *primVtx;

    Amg::Vector3D primVtxPos(primVtx->position().x(), primVtx->position().y(), primVtx->position().z());

    // select 2 muons from container
    // check that they have opposite charges
    // then construct the Z as a composite of 2 muons
    AnalysisUtils::Combination<const MuonContainer> comb(muonTDS,2);
    MuonVect muonPair;
    while (comb.goodOnes(this, muonPair, ::selectMuon)) {
      // create the Z candidate as a composite of 2 muons	  
      CompositeParticle* Zmm = new CompositeParticle;
      Zmm->add(INav4MomLink (muonPair[0], *muonTDS),
               INav4MomLink (muonPair[1], *muonTDS) );
      Zmm->set_charge(0);
      Zmm->set_pdgId(PDG::Z0);
      double mZmm = Zmm->m();
      m_aod_zmm_mass_hist->Fill(mZmm);

      myTracks.clear();
      myTracks.push_back(muonPair[0]->track());
      myTracks.push_back(muonPair[1]->track());

      ATH_MSG_DEBUG("AthenaBarCode(mu[0])= " << muonPair[0]->track()->getAthenaBarCode()<< ", AthenaBarCode(mu[1])= " << muonPair[1]->track()->getAthenaBarCode());

      myTrackBases.clear();
      myTrackBases.push_back(muonPair[0]->track());
      myTrackBases.push_back(muonPair[1]->track());

      myMuonMasses.clear();
      myMuonMasses.push_back(mMuon);
      myMuonMasses.push_back(mMuon);

      // vertex contrained to the primary vertex
      m_VKVrtFitter->setVertexForConstraint(primaryVtx);
      m_VKVrtFitter->setMassInputParticles(myMuonMasses);

      // define variables returned by the vertex fit
      Amg::Vector3D retVtxPos;
      TLorentzVector   retMom4;
      long int retQ = 0;
      std::vector<double> errMatrix,chi2PerTrk; // Fit error matrix and chi2 per track
      std::vector< std::vector<double> > trkAtVrt; // "True" tracks passing through vertex [phi, theta, q/pt*sin(theta)]
      double pt_vk_mu1=-1, pt_vk_mu2=-1;
      double  fitChi2_vk=0.;
      double  invMass_VKFit=0;
      double dxy_vk=0, dz_vk=0;

      sc = m_VKVrtFitter->VKalVrtFit(myTrackBases, retVtxPos,retMom4,retQ,errMatrix,chi2PerTrk,trkAtVrt,fitChi2_vk);
      
      if (sc.isSuccess()) {
         invMass_VKFit = retMom4.M();

         pt_vk_mu1 = fabs(1./(trkAtVrt[0][2])) * sin(trkAtVrt[0][1]);
         // double px_vk_mu1 = pt_vk_mu1*cos(trkAtVrt[0][0]);
         // double py_vk_mu1 = pt_vk_mu1*sin(trkAtVrt[0][0]);
         // double pz_vk_mu1 = fabs(1./(trkAtVrt[0][2]));
         pt_vk_mu2 = fabs(1./(trkAtVrt[1][2])) * sin(trkAtVrt[1][1]);

         auto vDist_vk = retVtxPos-primVtxPos;
         dxy_vk = (vDist_vk.x()*retMom4.Px() + vDist_vk.y()*retMom4.Py()) /
		      retMom4.Perp();
		 dz_vk = vDist_vk.z()*retMom4.Pz() / fabs(retMom4.Pz());

         ATH_MSG_DEBUG("VKalVrtFit result: m()= " << invMass_VKFit << ", chi2= " << fitChi2_vk
              << "\n, original muon-1, pt= " << myTracks[0]->pt()
              << ", after vertex fit, pt= " << pt_vk_mu1
              << "\n, original muon-2, pt= " << myTracks[1]->pt()
		       << ", after vertex fit, pt= " << pt_vk_mu2);
      }

      ATH_MSG_DEBUG("Fill tree-variables of Z->mm");
      m_nZ++;
      m_Zmode->push_back(2);

      m_Zm_vkfit->push_back(invMass_VKFit);
      m_Zchi2_vkfit->push_back(fitChi2_vk);
      m_Zdxy_vkfit->push_back( dxy_vk );
      m_Zdz_vkfit->push_back( dz_vk );

      m_Zm->push_back(mZmm);
      m_Zpx->push_back( Zmm->px() );
      m_Zpy->push_back( Zmm->py() );
      m_Zpz->push_back( Zmm->pz() );
      m_Ze->push_back( Zmm->e() );
      m_Zp->push_back( Zmm->p() );
      m_Zpt->push_back( Zmm->pt() );
      m_Zeta->push_back( Zmm->eta() );

      m_L1pt_vkfit->push_back( pt_vk_mu1 );

      m_L1px->push_back( muonPair[0]->px() );
      m_L1py->push_back( muonPair[0]->py() );
      m_L1pz->push_back( muonPair[0]->pz() );
      m_L1e->push_back( muonPair[0]->e() );
      m_L1p->push_back( muonPair[0]->p() );
      m_L1pt->push_back( muonPair[0]->pt() );
      m_L1eta->push_back( muonPair[0]->eta() );

      m_L2pt_vkfit->push_back( pt_vk_mu1 );

      m_L2px->push_back( muonPair[1]->px() );
      m_L2py->push_back( muonPair[1]->py() );
      m_L2pz->push_back( muonPair[1]->pz() );
      m_L2e->push_back( muonPair[1]->e() );
      m_L2p->push_back( muonPair[1]->p() );
      m_L2pt->push_back( muonPair[1]->pt() );
      m_L2eta->push_back( muonPair[1]->eta() );
    }
  }
  
  return StatusCode::SUCCESS;

}

// muon selection function
bool selectMuon(VFitZmmOnAOD *self, const MuonVect &ll) {
  bool test1 = ll[0]->charge() == -(ll[1]->charge());
  bool test2 = (ll[0]->pt() > self->m_etMuonCut) &&
               (ll[1]->pt() > self->m_etMuonCut);
  bool test3 = ( fabs(ll[0]->eta()) < self->m_etaMuonCut ) &&
               ( fabs(ll[1]->eta()) < self->m_etaMuonCut );
  return (test1 && test2 && test3);
}

///
StatusCode VFitZmmOnAOD::addEventInfo() {

  // this code has been taken from CBNT_execute
  // Reconstruction/CBNT_Athena/src/CBNTAA_EventInfo.cxx
  // Need this in the ntuple, but don't want to invoke the CBNT class
  // I have the actual EventNumber, but skipped the sequential count of event #
  // 

  //get EventInfo for run and event number

  const xAOD::EventInfo* eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  
  if (sc.isFailure())
    {
      ATH_MSG_WARNING("Could not retrieve event info");
      return sc;
    }
  
  //
  m_runNumber=eventInfo->runNumber();
  m_eventNumber=eventInfo->eventNumber();
  m_eventTime=eventInfo->timeStamp() ; 
  m_lumiBlock=eventInfo->lumiBlock() ;
  m_bCID=eventInfo->bcid();
  m_eventWeight=eventInfo->mcEventWeight();

  /* Remove this information from the ntuple. This is non-optimal. VJ Mar 29, 2012
  // Got this impression by talking to Attila K.

  const TriggerInfo* myTriggerInfo=eventInfo->trigger_info();
  if (myTriggerInfo!=0) {
    m_lVL1ID=myTriggerInfo->extendedLevel1ID();
    m_statusElement=myTriggerInfo->statusElement();
    m_lvl1TriggerType=myTriggerInfo->level1TriggerType();

    std::vector<TriggerInfo::number_type>::const_iterator lvl1TrigIt=myTriggerInfo->level1TriggerInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator lvl1TrigIt_e=myTriggerInfo->level1TriggerInfo().end();
    for (;lvl1TrigIt!=lvl1TrigIt_e;lvl1TrigIt++)
      m_lvl1TriggerInfo->push_back(*lvl1TrigIt);


    std::vector<TriggerInfo::number_type>::const_iterator lvl2TrigIt=myTriggerInfo->level2TriggerInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator lvl2TrigIt_e=myTriggerInfo->level2TriggerInfo().end();
    for (;lvl2TrigIt!=lvl2TrigIt_e;lvl2TrigIt++)
      m_lvl2TriggerInfo->push_back(*lvl2TrigIt);

    std::vector<TriggerInfo::number_type>::const_iterator evtFilterIt=myTriggerInfo->eventFilterInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator evtFilterIt_e=myTriggerInfo->eventFilterInfo().end();
    for (;evtFilterIt!=evtFilterIt_e;evtFilterIt++)
      m_evtFilterInfo->push_back(*evtFilterIt);


    std::vector<TriggerInfo::StreamTag>::const_iterator streamInfoIt=myTriggerInfo->streamTags().begin();
    std::vector<TriggerInfo::StreamTag>::const_iterator streamInfoIt_e=myTriggerInfo->streamTags().end();
    for (;streamInfoIt!=streamInfoIt_e;streamInfoIt++) { 
      m_streamTagName->push_back(streamInfoIt->name());
      m_streamTagType->push_back(streamInfoIt->type());
    }

  }else
    {
      m_lVL1ID=0;
      m_statusElement=0;
      m_lvl1TriggerType=0;
    }
  
  */
  return StatusCode::SUCCESS;

}
