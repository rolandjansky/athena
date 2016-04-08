/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////// 
/// Analysis skeleton
/// also for distributed analysis examples
/// Author: Ketevi A. Assamagan
/// BNL, July 22, 2004
///
/// DESCRIPTION:
///
/// This class is an analysis skeleton - The user can implement her analysis here
/// Ketevi A. Assamagan on June 22, 2004
///
/// Vivek Jain - March 11, 2010
/// This file has been extensively modified since creation. It now shows you how to access
/// trigger information. Preparation of objects, e.g., electrons, muons, jets, etc., 
/// has been pushed off into the UserAnalysisUtils package
///
/// The code here also does overlap removal
///
/// Please see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PhysicsAnalysisWorkBookAODAnalysisRel15#Writing_your_own_analysis_code for details relating to release 15
///
/// Author: J.Kirk
/// March, 2009
///
/// DESCRIPTION:Validation for Jpsi MC - taken from AnalysisSkeleton
///////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////

// the first two come for free when using AthAlgorithm
//#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"


#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


//#include "egammaEvent/ElectronContainer.h"
//#include "egammaEvent/EMShower.h"
//#include "egammaEvent/egammaParamDefs.h"

#include "McParticleEvent/TruthParticleContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "ParticleTruth/TrackParticleTruthCollectionContainer.h"
#include "GeneratorObjects/HepMcParticleLink.h"


#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"
#include "CLHEP/Vector/LorentzVector.h"

//#include "JetTagInfo/TruthInfo.h"

#include "TrigDecisionTool/ChainGroup.h"

#include "TrigParticle/TrigL2Bphys.h" // makes clang32 happy
#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"

#include "TrigBphysValidation/JpsiVal.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"

#include "HepMC/GenEvent.h"

// Event Info
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"


#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

//#include "BPhysAnalysisTools/BPhysToolBox.h"


using namespace Analysis;
using namespace Rec;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

JpsiVal::JpsiVal(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_analysisTools( "AnalysisTools" ),
  m_analysisSelectionTool( "UserAnalysisSelectionTool" ),
  m_analysisPreparationTool( "UserAnalysisPreparationTool" ),
  m_analysisOverlapCheckingTool( "UserAnalysisOverlapCheckingTool" ),
  m_analysisOverlapRemovalTool( "UserAnalysisOverlapRemovalTool" ),
  m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
m_thistSvc(nullptr),
m_h_jpsiphysn(nullptr),
m_h_jpsiphysmass(nullptr),
m_h_jpsiphystype(nullptr),
m_hiNRoI_1(nullptr),
m_hiNRoI_2(nullptr),
m_hiNRoI_3(nullptr),
m_hiNRoI_4(nullptr),
m_hiNRoI_5(nullptr),
m_hiNRoI_6(nullptr),
m_hiNRoI_7(nullptr),
m_hiNRoI_8(nullptr),
m_hiNRoI_9(nullptr),
m_hiNRoI_10(nullptr),
m_hpt_muplus(nullptr),
m_hpt_muminus(nullptr),
m_hphi_muplus(nullptr),
m_hphi_muminus(nullptr),
m_heta_muplus(nullptr),
m_heta_muminus(nullptr),
m_hpt_muplus_inroi(nullptr),
m_hpt_muminus_inroi(nullptr),
m_hphi_muplus_inroi(nullptr),
m_hphi_muminus_inroi(nullptr),
m_heta_muplus_inroi(nullptr),
m_heta_muminus_inroi(nullptr),
m_hpt_muplus_tr(nullptr),
m_hpt_muminus_tr(nullptr),
m_hphi_muplus_tr(nullptr),
m_hphi_muminus_tr(nullptr),
m_heta_muplus_tr(nullptr),
m_heta_muminus_tr(nullptr),
m_hmass_Jpsi(nullptr),
m_hmass_Jpsi_b(nullptr),
m_hmass_Jpsi_m(nullptr),
m_hmass_Jpsi_e(nullptr),
m_Ntrack_Mup(0),
m_Ntrack_Mup_pT(0),
m_Ntrack_Mup_inRoI(0),
m_Ntrack_Mup_inRoI_pT(0),
m_Ntrack_Mum(0),
m_Ntrack_Mum_pT(0),
m_Ntrack_Mum_inRoI(0),
m_Ntrack_Mum_inRoI_pT(0),
m_tree_AS(nullptr),
m_aan_Jpsiphys(0),
m_aan_JpsiMass(0),
m_aan_JpsiFitMass(0),
m_doTrigger(true),
m_all(nullptr),
m_allL1(nullptr),
m_allL2(nullptr),
m_allEF(nullptr)
{

  /** switches to control the analysis through job options */

  declareProperty("TrackAlgo", m_trackAlgo=2);
		 // = 2);
  declareProperty("TriggerName", m_triggerName="L1_MU4");
  declareProperty( "AnalysisTools",               m_analysisTools );
  declareProperty( "AnalysisSelectionTool",       m_analysisSelectionTool);
  declareProperty( "AnalysisPreparationTool",     m_analysisPreparationTool);
  declareProperty( "AnalysisOverlapCheckingTool", m_analysisOverlapCheckingTool);
  declareProperty( "AnalysisOverlapRemovalTool",  m_analysisOverlapRemovalTool);
  declareProperty( "TrigDecisionTool",            m_trigDec, "The tool to access TrigDecision");
  declareProperty("McParticleContainer", m_truthParticleContainerName = "SpclMC");
  declareProperty("McEventContainer", m_MCEventContainerName = "GEN_AOD");
  declareProperty("TrackContainer", m_TrackContainerName = "HLT_TrigIDSCAN_Muon");


}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

JpsiVal::~JpsiVal() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate
/// get a handle on the analysis tools
/// book histograms

StatusCode JpsiVal::beginRun() {

  ATH_MSG_DEBUG("Initializing JpsiVal (before eventloop)");

  // retrieve trigger decision tool
  // needs to be done before the first run/event since a number of
  // BeginRun/BeginEvents are registered by dependent services

  if ( m_doTrigger ) {
     StatusCode sc = StatusCode::SUCCESS;
     sc = m_trigDec.retrieve();
     if ( sc.isFailure() )ATH_MSG_WARNING("Can't get handle on TrigDecisionTool");
     else ATH_MSG_DEBUG("Got handle on TrigDecisionTool");
     
  }


  return StatusCode::SUCCESS;
} 

StatusCode JpsiVal::initialize() {

  ATH_MSG_INFO("Initializing JpsiVal");

  /// get a handle on the analysis tools
  //  StatusCode sc = m_analysisTools.retrieve();
  //if ( sc.isFailure() ) {
  //  ATH_MSG_ERROR("Can't get handle on analysis tools");
  //  return sc;
  // }

  /// get a handle on the preparartion analysis tools
  //sc = m_analysisSelectionTool.retrieve();
  //if ( sc.isFailure() ) {
  //  ATH_MSG_ERROR("Can't get handle on analysis selection tool");
  //  return sc;
  // }

  //sc = m_analysisPreparationTool.retrieve();
  //if ( sc.isFailure() ) {
  //  ATH_MSG_ERROR("Can't get handle on analysis preparation tool");
  //  return sc;
  // }

  //sc = m_analysisOverlapCheckingTool.retrieve();
  //if ( sc.isFailure() ) {
  //  ATH_MSG_ERROR("Can't get handle on analysis overlap checking tool");
  //  return sc;
  // }

  //sc = m_analysisOverlapRemovalTool.retrieve();
  //if ( sc.isFailure() ) {
  //  ATH_MSG_ERROR("Can't get handle on analysis overlap removal tool");
  //  return sc;
  // }

  /** get a handle on the NTuple and histogramming service */
  StatusCode sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
     return sc;
  }

  // at this point declare new vectors to put in ntuple
 m_Ntrack_Mup=0;
  m_Ntrack_Mup_pT=0;
  m_Ntrack_Mup_inRoI=0;
  m_Ntrack_Mup_inRoI_pT=0;
  m_Ntrack_Mum=0;
  m_Ntrack_Mum_pT=0;
  m_Ntrack_Mum_inRoI=0;
  m_Ntrack_Mum_inRoI_pT=0;

  m_aan_JpsiMass = new std::vector<double>;
  m_aan_JpsiFitMass = new std::vector<double>;
 

  /** now add branches and leaves to the AAN tree */
  // the TTree
  m_tree_AS = new TTree("tree_AS","TTree of AnalysisSkleton");
  sc = m_thistSvc->regTree("/AANT/tree_AS", m_tree_AS);

  // first add Event info stuff

  m_tree_AS->Branch("NJpsiphys",  &m_aan_Jpsiphys,   "NJpsiphys/I");   
  m_tree_AS->Branch("JpsiMass",   &m_aan_JpsiMass);
  m_tree_AS->Branch("JpsiFitMass",  &m_aan_JpsiFitMass);


  /// ROOT histograms ---------------------------------------
  m_h_jpsiphysn      = new TH1F("jpsiphys_n","N jpsiphys",10,0,10);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/jpsiphys_n",m_h_jpsiphysn);
  m_h_jpsiphysmass     = new TH1F("jpsiphys_mass","jpsiphys mass",50,1500,2500.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/jpsiphys_mass",m_h_jpsiphysmass);
  m_h_jpsiphystype     = new TH1F("jpsiphys_type","jpsiphys type",10,0.,10.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/jpsiphys_type",m_h_jpsiphystype);

  m_hiNRoI_1     = new TH1F("Nroi_1","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_1",m_hiNRoI_1);
  m_hiNRoI_2     = new TH1F("Nroi_2","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_2",m_hiNRoI_2);
  m_hiNRoI_3     = new TH1F("Nroi_3","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_3",m_hiNRoI_3);
  m_hiNRoI_4     = new TH1F("Nroi_4","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_4",m_hiNRoI_4);
  m_hiNRoI_5     = new TH1F("Nroi_5","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_5",m_hiNRoI_5);
  m_hiNRoI_6     = new TH1F("Nroi_6","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_6",m_hiNRoI_6);
  m_hiNRoI_7     = new TH1F("Nroi_7","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_7",m_hiNRoI_7);
  m_hiNRoI_8     = new TH1F("Nroi_8","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_8",m_hiNRoI_8);
  m_hiNRoI_9     = new TH1F("Nroi_9","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_9",m_hiNRoI_9);
  m_hiNRoI_10     = new TH1F("Nroi_10","RoI multiplicity threshold 1",30,0.,30.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/Nroi_10",m_hiNRoI_10);


  const int NBINS=16;
  Float_t x_pt[NBINS+1]={0.,500.,1000.,1500.,2000.,2500.,3000.,3500.,4000.,4500.,5000.,6000.,7000.,8000.,10000,12000.,15000.};

  m_hpt_muplus = new TH1F("hpt_muplus","pT for muplus",NBINS,x_pt);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hpt_muplus",m_hpt_muplus);
  m_hpt_muminus = new TH1F("hpt_muminus","pT for muminus",NBINS,x_pt);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hpt_muminus",m_hpt_muminus);
  m_hphi_muplus = new TH1F("hphi_muplus","phi for muplus",20,-3.5,3.5);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hphi_muplus",m_hphi_muplus);
  m_hphi_muminus = new TH1F("hphi_muminus","phi for muminus",20,-3.5,3.5);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hphi_muminus",m_hphi_muminus);
  m_heta_muplus = new TH1F("heta_muplus","eta for muplus",20,-3.,3.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/heta_muplus",m_heta_muplus);
  m_heta_muminus = new TH1F("heta_muminus","eta for muminus",20,-3.,3.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/heta_muminus",m_heta_muminus);

  m_hpt_muplus_inroi = new TH1F("hpt_muplus_inroi","pT for muplus",NBINS,x_pt);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hpt_muplus_inroi",m_hpt_muplus_inroi);
  m_hpt_muminus_inroi = new TH1F("hpt_muminus_inroi","pT for muminus",NBINS,x_pt);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hpt_muminus_inroi",m_hpt_muminus_inroi);
  m_heta_muplus_inroi = new TH1F("heta_muplus_inroi","eta for muplus",20,-3,3);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/heta_muplus_inroi",m_heta_muplus_inroi);
  m_heta_muminus_inroi = new TH1F("heta_muminus_inroi","eta for muminus",20,-3,3);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/heta_muminus_inroi",m_heta_muminus_inroi);
  m_hphi_muplus_inroi = new TH1F("hphi_muplus_inroi","eta for muplus",20,-3.5,3.5);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hphi_muplus_inroi",m_hphi_muplus_inroi);
  m_hphi_muminus_inroi = new TH1F("hphi_muminus_inroi","eta for muminus",20,-3.5,3.5);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hphi_muminus_inroi",m_hphi_muminus_inroi);

  m_hpt_muplus_tr = new TH1F("hpt_muplus_tr","pT for muplus with track",NBINS,x_pt);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hpt_muplus_tr",m_hpt_muplus_tr);
  m_hpt_muminus_tr = new TH1F("hpt_muminus_tr","pT for muminus with track",NBINS,x_pt);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hpt_muminus_tr",m_hpt_muminus_tr);
  m_hphi_muplus_tr = new TH1F("hphi_muplus_tr","phi for muplus",20,-3.5,3.5);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hphi_muplus_tr",m_hphi_muplus_tr);
  m_hphi_muminus_tr = new TH1F("hphi_muminus_tr","phi for muminus",20,-3.5,3.5);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hphi_muminus_tr",m_hphi_muminus_tr);
  m_heta_muplus_tr = new TH1F("heta_muplus_tr","eta for muplus",20,-3.,3.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/heta_muplus_tr",m_heta_muplus_tr);
  m_heta_muminus_tr = new TH1F("heta_muminus_tr","eta for muminus",20,-3.,3.);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/heta_muminus_tr",m_heta_muminus_tr);


  m_hmass_Jpsi = new TH1F("hmass_Jpsi","Jpsi mass",100,1.6,4.25);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hmass_Jpsi",m_hmass_Jpsi);
  m_hmass_Jpsi_b = new TH1F("hmass_Jpsi_b","Jpsi mass",100,1.6,4.25);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hmass_Jpsi_b",m_hmass_Jpsi_b);
  m_hmass_Jpsi_m = new TH1F("hmass_Jpsi_m","Jpsi mass",100,1.6,4.25);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hmass_Jpsi_m",m_hmass_Jpsi_m);
  m_hmass_Jpsi_e = new TH1F("hmass_Jpsi_e","Jpsi mass",100,1.6,4.25);
  sc = m_thistSvc->regHist("/AANT/Jpsiphys/hmass_Jpsi_e",m_hmass_Jpsi_e);


  if (sc.isFailure()) { 
    ATH_MSG_ERROR("ROOT Hist registration failed"); 
    return sc; 
  }
  /// end ROOT Histograms ------------------------------------------

  // define chain groups using regular expressions and relying on the
  // trigger chain name convention: all L1 items start their name from
  // L1_ etc; in fact, the TrigDecisionTool already defines these
  // groups by default, but let's do it again as an example
  m_all   = m_trigDec->getChainGroup(".*");
  m_allL1 = m_trigDec->getChainGroup("L1_.*"); 
  m_allL2 = m_trigDec->getChainGroup("L2_.*");
  m_allEF = m_trigDec->getChainGroup("EF_.*");



  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode JpsiVal::finalize() {
   
  return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
/// Clear - clear CBNT members
StatusCode JpsiVal::initEvent() {
  /// For Athena-Aware NTuple

  m_aan_Jpsiphys  = 0;

  m_aan_JpsiMass->clear();
  m_aan_JpsiFitMass->clear();

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode JpsiVal::execute() {
  //
  //  m_eventNr++;
  ATH_MSG_DEBUG(" in execute()");

  StatusCode sc;

  // initialize first before processing each event
  sc = initEvent();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("initEvent failed. Continue");
  }

  ATH_MSG_DEBUG("in execute after initEvent()");

  // first check if the MU4_J5 trigger fired, otherwise the tracking wasn't run
  ATH_MSG_DEBUG("Get L1 info");
 // L1
  std::string L1trig=m_triggerName;
  bool L1trig_passed=false;
  //  const std::vector<const LVL1CTP::Lvl1Item*> allItems = m_trigDec->getL1Items();
  //Trig::ExpertMethods* em = m_trigDec->ExperimentalAndExpertMethods();

  std::vector<std::string> allItems = m_trigDec->getListOfTriggers("L1_.*");


  /*  if (!allItems.empty()) {

    for (std::vector<std::string>::const_iterator itemIt = allItems.begin();
          itemIt != allItems.end(); ++itemIt) {

       const LVL1CTP::Lvl1Item* aItem = em->getItemDetails(*itemIt);
      if (!aItem) continue;
      if (aItem->name()=="") continue;
      ATH_MSG_DEBUG("Trigger Item with name: " << aItem->name());
      if ( aItem->name()!=L1trig) continue;


      ATH_MSG_INFO("Item " << aItem->name() << " : Item ID " << aItem->hashId() << " : " << aItem->isPassed());
      if (aItem->isPassed()) L1trig_passed=true;

    }
  } else {
    ATH_MSG_WARNING ("Could not retrieve L1 Items !!" );
      return StatusCode::SUCCESS;
      }*/

   ATH_MSG_INFO("Item " << L1trig << " is passed " << m_trigDec->isPassed(L1trig));
   L1trig_passed = m_trigDec->isPassed(L1trig);

  if (!L1trig_passed) return StatusCode::SUCCESS;

  //BPhysToolBox myBPhysToolBox;

  //get the truth partciles
  const McEventCollection* McEvent(0);
  //  sc = m_StoreGate->retrieve(McEvent,"GEN_AOD");
  sc = evtStore()->retrieve(McEvent,m_MCEventContainerName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("No McEventCollection available in StoreGate");
    return sc;
  };
  ATH_MSG_DEBUG("retrieved MCEventCollection");

  //myBPhysToolBox.setMcEventCollection(McEvent);
  //ATH_MSG_DEBUG("set Bphystoolbox collection");

  //  bool foundSignalEventInTruth = false;
  const HepMC::GenParticle* genJpsi=NULL;
  const HepMC::GenParticle* genMuPlus=NULL;
  const HepMC::GenParticle* genMuMinus=NULL;
  std::vector<const HepMC::GenParticle*>::iterator truePtclItr;
  std::vector<int> decayTopology;
  std::vector<std::vector<const HepMC::GenParticle*> > trueParticlesHolder;
  //
  const int JPSI_PID=443;
  const int MUPLUS_PID=-13;
  const int MUMINUS_PID=13;

  HepMC::GenEvent::particle_const_iterator genItr;
  HepMC::GenVertex::particle_iterator genOutVtxItr;
  bool FoundTrueDecay=false;
  const HepMC::GenEvent *genEvent;
  genEvent=*(McEvent->begin());
  for(genItr=genEvent->particles_begin();genItr!=genEvent->particles_end();genItr++){
    if((*genItr)->pdg_id()== JPSI_PID)  {
      genJpsi = *genItr;
      ATH_MSG_DEBUG("Found Jpsi ");
      if ((*genItr)->end_vertex()==NULL) continue;
      for(genOutVtxItr=(*genItr)->end_vertex()->particles_begin(HepMC::children);genOutVtxItr!=(*genItr)->end_vertex()->particles_end(HepMC::children);genOutVtxItr++){
	ATH_MSG_DEBUG ("Found Jpsi Daughter with PID:  " << (*genOutVtxItr)->pdg_id());
	if ((*genOutVtxItr)->pdg_id() == MUPLUS_PID) {
	  ATH_MSG_DEBUG ("Found MUPLUS ");
	  genMuPlus = *genOutVtxItr;
	 }
	if ((*genOutVtxItr)->pdg_id() == MUMINUS_PID) {
	  ATH_MSG_DEBUG( "Found MUMINUS " );
	  genMuMinus = *genOutVtxItr;
	 }
      } // loop over Jpsi daughters
      if (genMuPlus==NULL || genMuMinus==NULL) continue;
      FoundTrueDecay=true;
    }
  }
  if (!FoundTrueDecay) {
    ATH_MSG_DEBUG ("Did not find correct decay topology");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Found correct decay topology");
  ATH_MSG_DEBUG ("Muplus with pT eta phi " << genMuPlus->momentum().perp() << " " << genMuPlus->momentum().pseudoRapidity() << " " << genMuPlus->momentum().phi());
  ATH_MSG_DEBUG( "Muinus with pT eta phi " << genMuMinus->momentum().perp() << " " << genMuMinus->momentum().pseudoRapidity() << " " << genMuMinus->momentum().phi());

  // histogram truth info
  m_hpt_muplus->Fill(genMuPlus->momentum().perp());
  m_hpt_muminus->Fill(genMuMinus->momentum().perp());

  if (genMuPlus->momentum().perp() > 1500.) {
    m_heta_muplus->Fill(genMuPlus->momentum().pseudoRapidity());
    m_hphi_muplus->Fill(genMuPlus->momentum().phi());
  }
  if (genMuMinus->momentum().perp() > 1500.) {
      m_heta_muminus->Fill(genMuMinus->momentum().pseudoRapidity());
      m_hphi_muminus->Fill(genMuMinus->momentum().phi());
  }
  ATH_MSG_DEBUG ("filled truth histograms");

  // check if truth particles are within a JET ROI
  bool  Mup_inRoI=false;
  bool  Mum_inRoI=false;

  // now Jet RoIs
  // const int NCUT=10;
  //  float E_Cut[NCUT]={2000.,3000.,4000.,5000.,6000.,7000.,8000.,9000.,10000.,11000.};
  // int NClus[NCUT];

  // retrieve the container of LVL1 ROIs
  // retrieve L1Result
  const LVL1_ROI* lvl1ROI=0;
  sc = evtStore()->retrieve(lvl1ROI);
  if(sc.isFailure()) {
    ATH_MSG_WARNING ("no LVL1_ROI container found in TDS");
    return StatusCode::SUCCESS;
  }



  LVL1_ROI::muons_type::const_iterator itMuon   =(lvl1ROI->getMuonROIs()).begin();
  LVL1_ROI::muons_type::const_iterator itMuon_e =(lvl1ROI->getMuonROIs()).end();

  // const float ETCUT=4.;
  const float Pi = 3.14159265358979323846;

  for (; itMuon != itMuon_e; ++itMuon) {
    //        if ((*itMuon).getET4x4() <= ETCUT) continue;
        float MuonPhi=(*itMuon).getPhi();
        if (MuonPhi > Pi ) MuonPhi =  MuonPhi - 2*Pi;

	// check only decay particles
	float dphitemp=genMuPlus->momentum().phi() - MuonPhi;
	float detatemp=genMuPlus->momentum().pseudoRapidity() - (*itMuon).getEta();
	float absdphi = fabs(dphitemp);
	if (2.*Pi-absdphi < absdphi) {
	  if (dphitemp > 0) {
              dphitemp = dphitemp - 2.* Pi;
	  } else {
	    dphitemp = 2* Pi - absdphi;
	  }
	}

	if ( sqrt(pow(dphitemp,2)+pow(detatemp,2))<0.1) {
	  Mup_inRoI=true;
          ATH_MSG_DEBUG ("Muplus matched with RoI at eta phi " << (*itMuon).getEta() << " " << (*itMuon).getPhi() << " " << (*itMuon).getThrValue() );
	}
	dphitemp= genMuMinus->momentum().phi() - MuonPhi;
	detatemp=genMuMinus->momentum().pseudoRapidity() - (*itMuon).getEta();
	absdphi = fabs(dphitemp);

	if (2.*Pi-absdphi < absdphi) {
	  if (dphitemp > 0) {
	    dphitemp = dphitemp - 2.* Pi;
	  } else {
	    dphitemp = 2* Pi - absdphi;
	  }
	}
	if ( sqrt(pow(dphitemp,2)+pow(detatemp,2))<0.1) Mum_inRoI=true;
  } // end loop over RoIs


  if (Mup_inRoI) {
    m_hpt_muplus_inroi->Fill(genMuPlus->momentum().perp());
         if (genMuPlus->momentum().perp()>1500.) {
	   m_heta_muplus_inroi->Fill(genMuPlus->momentum().pseudoRapidity());
         m_hphi_muplus_inroi->Fill(genMuPlus->momentum().phi());
	 }
      }
      if (Mum_inRoI) {
        m_hpt_muminus_inroi->Fill(genMuMinus->momentum().perp());
	if (genMuMinus->momentum().perp()>1500.) {
	  m_heta_muminus_inroi->Fill(genMuMinus->momentum().pseudoRapidity());
         m_hphi_muminus_inroi->Fill(genMuMinus->momentum().phi());
	}
      }
      ATH_MSG_DEBUG ( "checked if partciles are in RoI" );

      // get tracks
      // retrieve truth map from storegate
      bool FoundTruth=true;
      const TrigInDetTrackTruthMap* p_map;
      sc=evtStore()->retrieve(p_map,"TrigInDetTrackTruthMap");
      if (sc.isFailure()) {
        ATH_MSG_WARNING ( " didn't find any TrackTruthMap objects" );
        FoundTruth=false;
      }

      // look for tracks
      bool Flag_track_Mup=false;
      bool Flag_track_Mum=false;
      //bool Flag_track_Mup_pT=false;
      //bool Flag_track_Mum_pT=false;
      // bool found_all=false;
      // bool foundEF_all = false;
      // Int_t Id_track_Mup=-99;
      //Int_t Id_track_Mum=-99;
      TrigInDetTrack* track_Mup=NULL;
      TrigInDetTrack* track_Mum=NULL;
      //retrieve Bphysics and IdScan Track collections

      /*      std::string TrigInDetTrackTags[]={ "HLT_TrigL2SiTrackFinder_Muon" //"HLT_TrigIDSCAN_Muon"
					 //					 "HLT_TrigIDSCAN_Bphysics",
					 //"HLT_TrigIDSCAN_FullScan",
					 //"HLT_TrigSiTrack_Bphysics",
					 // "HLT_TrigSiTrack_FullScan"
                                       };
      int ntag=1;
      */
      //      for (int iTag=0; iTag < ntag; iTag++) {
	TrigInDetTrackCollection* trigInDetTrackCollection;
	//	StatusCode sc = evtStore()->retrieve(trigInDetTrackCollection,TrigInDetTrackTags[iTag] );
	sc = evtStore()->retrieve(trigInDetTrackCollection,m_TrackContainerName );
	if (sc.isFailure()) {
	  //	  ATH_MSG_WARNING("REGTEST No TrigInDetTrackCollection found with key " << TrigInDetTrackTags[iTag]);
	  ATH_MSG_WARNING("REGTEST No TrigInDetTrackCollection found with key " << m_TrackContainerName);
	  //continue;
	} else {
	//	ATH_MSG_INFO("REGTEST TrigInDetTrackCollections retrieved with key " << TrigInDetTrackTags[iTag]);
	ATH_MSG_INFO("REGTEST TrigInDetTrackCollections retrieved with key " << m_TrackContainerName);

	ATH_MSG_INFO("REGTEST TrigInDetTrackCollection->size() returns " << trigInDetTrackCollection->size() << " RoI ID " << trigInDetTrackCollection->RoI_ID() );

	if (trigInDetTrackCollection->size() > 0) {
	TrigInDetTrackCollection::const_iterator track  = trigInDetTrackCollection->begin();
	TrigInDetTrackCollection::const_iterator lastTrack = trigInDetTrackCollection->end();


	for (int itrk=0; track !=lastTrack; ++itrk, track++) {

	  if ((*track)->algorithmId() != m_trackAlgo) continue;
	  ATH_MSG_DEBUG (" track " << itrk << " correct algo " );
	  // find kine particle for this track

	  if (!FoundTruth) continue;

	  ATH_MSG_DEBUG (" check for truth info " );
	  if (!(p_map->hasTruth(*track))) continue;
	  const HepMcParticleLink* p_truth=NULL;
	  const TrigInDetTrackTruth* p_trtruth = p_map->truth(*track);
	  ATH_MSG_DEBUG (" track has truth info, nmatches = " <<p_trtruth->nrMatches());
	  int nMatches = p_trtruth->nrMatches() ;

	  if (nMatches>0) {
	    p_truth = p_map->bestMatchSi(*track);
	    unsigned int nMatchHits = p_trtruth->nrCommonHitsBestSi();
	    ATH_MSG_DEBUG (" number of matched hits: " << nMatchHits );
	    if (nMatchHits>3) {
	      ATH_MSG_DEBUG (" PDG id " << p_truth->cptr()->pdg_id() );
	      ATH_MSG_DEBUG (" truth pointer " << p_truth->cptr() << " gen MuPlus " << genMuPlus << " gen Muminus " << genMuMinus );
		//attention
	      if (p_truth->cptr() == genMuPlus) {
		Flag_track_Mup=true;
		track_Mup=(*track);
		//attention
                     //if (fabs((*track)->param()->pT()) > 1400.) Flag_track_Mup_pT=true;
		     ATH_MSG_DEBUG ("Found Muplus track " << itrk << " pT " << (*track)->param()->pT() );
	      }
	      //attention
	      if (p_truth->cptr() == genMuMinus) {
		Flag_track_Mum=true;
		track_Mum=(*track);
		//attention
		//if (fabs((*track)->param()->pT()) > 1400.) Flag_track_Mum_pT=true;
		ATH_MSG_DEBUG ("Found minus track " << itrk << " pT " << (*track)->param()->pT() );
	      }
	    }  // if track has matching hits
	  } // if more than 3 matching hits
	} // end loop over tracks
	} // end if size > 0
	} // end if track collection found
	//      } // end loop over track collections


      if (Flag_track_Mup) {
	if (Mup_inRoI) {
	  m_Ntrack_Mup_inRoI++;
	  m_hpt_muplus_tr->Fill(genMuPlus->momentum().perp());
	  //attention
	  if (genMuPlus->momentum().perp()>1500.) {
	    m_Ntrack_Mup_inRoI_pT++;
	    m_heta_muplus_tr->Fill(genMuPlus->momentum().pseudoRapidity());
	    m_hphi_muplus_tr->Fill(genMuPlus->momentum().phi());
	  }
	}
	m_Ntrack_Mup++;
	if (genMuPlus->momentum().perp()>1500.) {
	  m_Ntrack_Mup_pT++;
	}
      }
      if (Flag_track_Mum) {
	m_Ntrack_Mum++;
	if (Mum_inRoI) {
	  m_Ntrack_Mum_inRoI++;
	  m_hpt_muminus_tr->Fill(genMuMinus->momentum().perp());

	  if (genMuMinus->momentum().perp()>1500.) {
	    m_Ntrack_Mum_inRoI_pT++;
	    m_heta_muminus_tr->Fill(genMuMinus->momentum().pseudoRapidity());
	    m_hphi_muminus_tr->Fill(genMuMinus->momentum().phi());
	  }
	}
	if (genMuMinus->momentum().perp()>1500.) {
	  m_Ntrack_Mum_pT++;
	}
      }

      // mass
      const float m_mu = 113.43;

      if (genJpsi->momentum().perp() > 1000.)  {
	if (Flag_track_Mup & Flag_track_Mum) {
	  float px_1 = fabs(track_Mup->param()->pT()) * cos(track_Mup->param()->phi0());
	  float py_1 = fabs(track_Mup->param()->pT()) * sin(track_Mup->param()->phi0());
	  float theta_1 = 2. * atan(exp(-1.0*(track_Mup->param()->eta())));
	  float pz_1 = fabs(track_Mup->param()->pT()) / tan(theta_1);
	  float E_1 = sqrt(px_1*px_1 + py_1*py_1 + pz_1*pz_1 + m_mu*m_mu);

	  float px_2 = fabs(track_Mum->param()->pT()) * cos(track_Mum->param()->phi0());
	  float py_2 = fabs(track_Mum->param()->pT()) * sin(track_Mum->param()->phi0());
	  float theta_2 = 2. * atan(exp(-1.0*(track_Mum->param()->eta())));
	  float pz_2 = fabs(track_Mum->param()->pT()) / tan(theta_2);
	  float E_2 = sqrt(px_2*px_2 + py_2*py_2 + pz_2*pz_2 + m_mu*m_mu);

	  float pxsum = px_1 + px_2;
	  float pysum = py_1 + py_2;
	  float pzsum = pz_1 + pz_2;
	  float Esum = E_1 + E_2;

	  Float_t Mass_Jpsi = sqrt( Esum*Esum - pxsum*pxsum - pysum*pysum - pzsum*pzsum);

	  m_hmass_Jpsi->Fill(Mass_Jpsi/1000.);
	  if (fabs(genJpsi->momentum().pseudoRapidity()) < 1.0) {
	    m_hmass_Jpsi_b->Fill(Mass_Jpsi/1000.);
	  } else {
	    if (fabs(genJpsi->momentum().pseudoRapidity()) < 1.5) {
	      m_hmass_Jpsi_m->Fill(Mass_Jpsi/1000.);
	    } else {
	      m_hmass_Jpsi_e->Fill(Mass_Jpsi/1000.);
	}
	  }
	} // if both muon tracks found
      } // if Jpsi mom > 10 GeV




      // look for EF tracks
      /*
      bool Flag_EFtrack_Mup=false;
      bool Flag_EFtrack_Mum=false;
      bool Flag_EFtrack_Mup_pT=false;
      bool Flag_EFtrack_Mum_pT=false;
      Int_t Id_EFtrack_Mup=-99;
      Int_t Id_EFtrack_Mum=-99;
      */
      std::string trackPtags[]={ "HLT_InDetTrigParticleCreation_Bphysics_EFID",
				 "HLT_InDetTrigParticleCreation_FullScan_EFID"};

      int nEFtag=2;

      //      StatusCode returnsc=StatusCode::SUCCESS;
      for (int itag=0; itag<nEFtag; itag++){
	const Rec::TrackParticleContainer*  pTrackParticleC;
	StatusCode sc = evtStore()->retrieve(pTrackParticleC, trackPtags[itag]);
	if (sc.isFailure()) {
	  ATH_MSG_WARNING ("REGTEST No TrackParticleContainer found with tag " << trackPtags[itag]);
	  continue;
	}
	ATH_MSG_INFO("TrackParticleContainer found with tag " << trackPtags[itag]
		     << " and size " << pTrackParticleC->size());

	Rec::TrackParticleContainer::const_iterator trackItr  = pTrackParticleC->begin();
	Rec::TrackParticleContainer::const_iterator trackItrE = pTrackParticleC->end();

	int nUnmatchedToTruth=0;
	//put a default value if no match to truth
	//int truthBarcode=-9999;
	// int truthNt=-9999;
	int partPdgId=-9999;
	// int parentPdgId=-9999;

	ATH_MSG_INFO("Now loop over tracks ");
    for (int itrk=1; trackItr != trackItrE; ++trackItr, itrk++) {

   bool m_first=true;

	    //double NumSiKineHitsDTT = -1.;
            //double NumTRTKineHitsDTT = -1.;

	    //double numSiKineHits= NumSiKineHitsDTT;
            //double numTRTKineHits=NumTRTKineHitsDTT;


	    //numSiKineHits =  NumSiKineHitsDTT;
            //numTRTKineHits = NumTRTKineHitsDTT;


	    TrackParticleTruthCollection truthCollection;
	    const TrackParticleTruthCollection* pTruthCollection(0);


	    sc = getTrackParticleTruth(truthCollection);
	    if (sc.isFailure()){
    	       ATH_MSG_WARNING("Did not find trackparticle truthcollection ");
	    }
	    pTruthCollection=& truthCollection;


            // first try to find truth
	    // Get the HepMCParticleLink
	    bool truthFind = false;
	    const TrackParticleTruthCollection* tpTruthColl(0);

	    TrackParticleTruthCollection::const_iterator fzTruth= truthCollection.begin();
            TrackParticleTruthCollection::const_iterator lastTruthTrack= truthCollection.end();

		for (int countTruth=0; (fzTruth !=lastTruthTrack) && !truthFind; ++fzTruth, ++countTruth) {
		  truthFind = false;

		  ATH_MSG_VERBOSE("Filling truth for the track no: " << countTruth+1 );

		  tpTruthColl = pTruthCollection;
		  if (tpTruthColl!=0){
		    ATH_MSG_VERBOSE("Finding TrackParticleTruth corresponding to TrackParticle..." );

		    const Rec::TrackParticle* myTrackParticle = (*trackItr);
		    ElementLink<Rec::TrackParticleContainer> trackPrtlink;
		    trackPrtlink.setElement(const_cast<Rec::TrackParticle*>(myTrackParticle));
		    trackPrtlink.setStorableObject(*pTrackParticleC);
		    TrackParticleTruthCollection::const_iterator tempTrackPrtTruthItr;
		    tempTrackPrtTruthItr = tpTruthColl->find(Rec::TrackParticleTruthKey(trackPrtlink));

		    if (tempTrackPrtTruthItr != tpTruthColl->end()){
		      const HepMcParticleLink & temHepMcLink = (*tempTrackPrtTruthItr).second.particleLink();
		      long theBarcode(0);
		      if (temHepMcLink.eventIndex()==0) theBarcode=temHepMcLink.barcode();
		      //truthBarcode = theBarcode;

		      const HepMC::GenParticle  * thePart(0);
		      if (theBarcode!=0){
			    thePart = temHepMcLink.cptr();
			    if (thePart!=0){
			      if (theBarcode != thePart->barcode()){
				ATH_MSG_WARNING (" hepMCLink/HepMC missmatch: " << theBarcode << "/" << thePart->barcode() );
			      }
			      partPdgId = thePart->pdg_id();

			      if (m_first) ATH_MSG_DEBUG ("Getting PDG-Id from G4 truth." );
			      m_first=false;
			      ATH_MSG_VERBOSE ("Getting PDG-Id from G4 : " << partPdgId );
			    }
		      } else {
			ATH_MSG_DEBUG("Null barcode =" << theBarcode
				      << " HepMcLink.eventIndex(): " << temHepMcLink.eventIndex());
		      }
		      if (thePart){
			ATH_MSG_VERBOSE(*thePart);
			float lphi, leta,lp;
			lphi = thePart->momentum().phi();
			leta = thePart->momentum().eta();
			lp = thePart->momentum().e();
			ATH_MSG_VERBOSE ("kine  : " << lphi << " " << leta << " " << lp );
		      }

		      ATH_MSG_VERBOSE ("Found truth matching for track " << itrk+1 );
		      truthFind = true;

		    }
		    else{
		      ATH_MSG_VERBOSE (" Could not find truth matching for track - try again " << itrk+1);
		    }
		  } // end of test (if tpTruthColl !=0)
		}// end loop using varName = countTruth
	    if(!truthFind){
	      ++nUnmatchedToTruth;
	    }
	      }

  } // end loop over EFtag
      ATH_MSG_DEBUG("At end of execute() " );
  return StatusCode::SUCCESS;
}



//================================================================

StatusCode JpsiVal::getTrackParticleTruth( TrackParticleTruthCollection& result) {


  const DataHandle<TrackParticleTruthCollectionContainer > trackpTruthCollectionContainer;
  const DataHandle<TrackParticleTruthCollectionContainer > lastTrackpTruthCollectionContainer;

  //m_storeGate->dump();

  StatusCode sc = evtStore()->retrieve(trackpTruthCollectionContainer, lastTrackpTruthCollectionContainer);
  if (sc.isFailure() ){
    ATH_MSG_WARNING ("didn't find any TrigTrackParticleTruthCandidate objects " );
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_DEBUG ("FOUND Trig TrackParticleTruthCollectionContainers");
  

  // EFFECIVELY LOOP OVER SLICES
  for ( ; trackpTruthCollectionContainer != lastTrackpTruthCollectionContainer; ++trackpTruthCollectionContainer ) {
    ATH_MSG_DEBUG("got TrackParticleTruthCollectionContainer of key: " << trackpTruthCollectionContainer.key() );
    if (trackpTruthCollectionContainer.key().find("HLT") != std::string::npos) {

      DataVector<TrackParticleTruthCollection>::const_iterator cIt;
      for ( cIt = trackpTruthCollectionContainer->begin(); cIt != trackpTruthCollectionContainer->end(); ++cIt){
	TrackParticleTruthCollection::const_iterator sIt;
	for ( sIt = (*cIt)->begin(); sIt != (*cIt)->end(); ++sIt ) {
	  result[sIt->first] = sIt->second;
	}
      }
    }
  }
  ATH_MSG_DEBUG ("got TrackParticleTruthCollection of size: " << result.size() );

  return StatusCode::SUCCESS;
}
