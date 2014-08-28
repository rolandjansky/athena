/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#if 0
// Doesn't compile in devval.
///////////////////////////////////////////////////////////////////////////////
// PAUtruthTool.cxx, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////////////////
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "PhotonAnalysisUtils/PAUtruthTool.h"

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrackToCalo/ImpactInCalo.h"

#include "McParticleKernel/ITruthParticleCnvTool.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "egammaEvent/Photon.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

#define UNKNOWN -99999 


using HepGeom::Point3D;
using CLHEP::HepLorentzVector;


// template <class T> class High2LowByPt {
//  public:
//   bool operator () (const T *t1, const T *t2) const {
//     if ( t1 == t2 )  return false ;
//     double pt1 = t1->pt() ;
//     double pt2 = t2->pt() ;
//     // if ( fabs(t1->pt()-t2->pt()) < 1e-10*fabs(t1->pt()+t2->pt()) )  return false ;
//     return ( pt1 > pt2 ) ;
//     // return (t1->pt() > t2->pt());
//   }
// };

// constructor
PAUtruthTool::PAUtruthTool(const std::string& t, const std::string& n, const IInterface* p) 
  : AlgTool(t,n,p),
    m_storeGate(0),
    m_TruthClassifierTool("MCTruthClassifier")
{
  //std::cout << "PAUtruthTool Constructor" << std::endl ;
  declareInterface<IPAUtruthTool>(this);
  declareProperty("ExtrapolateElectronToCaloToolName",m_extrapolateElectronToCaloToolName = "ExtrapolateToCaloTool/extrapolElectronToCaloTool");
  declareProperty("ExtrapolateGammaToCaloToolName"   ,m_extrapolateGammaToCaloToolName    = "ExtrapolateToCaloTool/extrapolGammaToCaloTool");
  //
  declareProperty("TruthClassifierTool"              ,m_TruthClassifierTool           , "MCTruthClassifier Tool" ); 
  //
  // Truth selection:
  declareProperty("EGammasFromTruth",m_egammasFromTruth=true);
  declareProperty("MuonsFromTruth",m_muonsFromTruth=true);
  declareProperty("PartonsFromTruth",m_partonsFromTruth=false);
  declareProperty("HadronsFromTruth",m_hadronsFromTruth=true);
  declareProperty("UseTruthClassifierTool",m_useTruthClassifierTool=true); //use MCclassifier
  declareProperty("PtTruthCut",m_ptTruthCut=5000) ;
  declareProperty("ZTruthConv",m_zTruthConv=50e3) ;  // formerly 2800
  declareProperty("RTruthConv",m_rTruthConv=800) ;
  declareProperty("PtTruthConv",m_ptTruthConv=500) ;
  declareProperty("EtaTruthConv",m_etaTruthConv=2.5) ;
  declareProperty("Z0TruthCut",m_z0TruthCut=1000) ;
  declareProperty("R0TruthCut",m_r0TruthCut=10) ;
  declareProperty("deltaRmatch",m_deltaRmatch=0.1);
  declareProperty("UseG4Particles", m_useG4Particles=true);
  declareProperty("UseExtrapolation", m_useExtrapolation=true);
  declareProperty("UseAtlasExtrapolator", m_useAtlasExtrapolator=true);
  declareProperty("deltaEtaROI", m_deltaEtaROI=9999);
  declareProperty("deltaPhiROI", m_deltaPhiROI=9999);
  declareProperty("deltaEtaMatch", m_deltaEtaMatch=0.1);
  declareProperty("deltaPhiMatch", m_deltaPhiMatch=0.1);
  declareProperty("deltaEtaNarrow", m_deltaEtaNarrow=0.025);
  declareProperty("deltaPhiNarrow", m_deltaPhiNarrow=0.050);
  declareProperty("PtCutCharged", m_ptCutCharged=2000) ;
  declareProperty("PtCutNeutral", m_ptCutNeutral=1000) ;
  // Containers' SG keys
  declareProperty("MCParticleContainer", m_truthParticleContainerName = "SpclMC");
  declareProperty("MCEventCollection", m_truthEventCollectionName = "GEN_AOD");
  declareProperty("TruthJetsContainer", m_truthJetsContainerName = "AntiKt4TruthJets");
  // MC conversion tool                                                                              
  declareProperty( "ConvertFromESD", m_fromESD, "convert to MC ParticleContainer when using ESD files");
  declareProperty( "CnvTool", m_cnvTool = CnvTool_t( "TruthParticleCnvTool/CnvTool", this ),
                   "Handle to the tool converting a McEventCollection into a TruthParticleContainer");

  // reset statistics counters:
  m_numberEventsMC = 0 ;
  m_numberHiggsMC  = 0 ;
  m_numberHggMC    = 0 ;
  m_ncalls_gToCalo = 0 ;
  m_ncalls_eToCalo = 0 ; 
}
//===========================================================
// destructor
PAUtruthTool::~PAUtruthTool()
{}
//===========================================================
// initialize
StatusCode PAUtruthTool::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "... seeking Store Gate" << endreq ;
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    log << MSG::ERROR
	 << "Unable to retrieve pointer to StoreGateSvc"
	 << endreq;
    return sc;
  }

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ) {
    incsvc->addListener( this, "BeginEvent", priority);
    incsvc->addListener( this, "EndEvent");
  } else {
    log << MSG::ERROR << "PAUtruthTool::initialize(), could not get IncidentSvc" << endreq ;
    return sc ;
  }

  IToolSvc* p_toolSvc = 0;
  sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << " Tool Service not found " << endreq;
    return StatusCode::FAILURE;
  }
  IAlgTool* algtool; 

  ListItem ExtrapolateTool1(m_extrapolateElectronToCaloToolName);
  sc = p_toolSvc->retrieveTool(ExtrapolateTool1.type(), ExtrapolateTool1.name(), algtool, this);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Cannot retrieve extrapolElectronToCaloTool " << endreq;
    return StatusCode::FAILURE;
  } else {
    m_eToCalo = dynamic_cast<IExtrapolateToCaloTool*>(algtool);
  }

  ListItem ExtrapolateTool2(m_extrapolateGammaToCaloToolName);
  sc = p_toolSvc->retrieveTool(ExtrapolateTool2.type(), ExtrapolateTool2.name(), algtool, this);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Cannot retrieve extrapolGammaToCaloTool " << endreq;
    return StatusCode::FAILURE;
  } else {
    m_gToCalo = dynamic_cast<IExtrapolateToCaloTool*>(algtool);
  }

  if ( m_TruthClassifierTool.retrieve().isFailure() ) { 
    log << MSG::FATAL << "Failed to retrieve tool " << m_TruthClassifierTool << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved tool " << m_TruthClassifierTool << endreq;
  }

//   log << MSG::DEBUG << "... seeking a handle on the tool service" << endreq ;
//   IToolSvc* toolSvc;
//   sc = service("ToolSvc",toolSvc);
//   if (StatusCode::SUCCESS != sc) {
//     log << MSG::ERROR << " Can't get ToolSvc" << endreq;
//     return StatusCode::FAILURE;
//   }

  //Convert McEventCollection if running on ESD                                                                                    
  if( m_fromESD )
    {
      log << MSG::DEBUG << "... seeking TruthConversionTool" << endreq ;
      sc = m_cnvTool.retrieve();
      if ( sc .isFailure() || m_cnvTool == 0 ) {
        log << MSG::WARNING << "Can't get handle on TruthConversionTool" << endreq;
        //return StatusCode::FAILURE;                                                                                                       
        m_cnvTool = 0 ;
      }
    }

  // truth access

  log << MSG::INFO << "=============== Truth access =============" << endreq ;
  log << MSG::INFO << " o  All photons from H->gg decays" << endreq ;
  if ( m_egammasFromTruth ) log << MSG::INFO << " o  All final state e+/e-/gamma" << endreq ;
  if ( m_muonsFromTruth )   log << MSG::INFO << " o  All final state mu+/mu-" << endreq ;
  if ( m_hadronsFromTruth ) log << MSG::INFO << " o  All final state stable hadrons" << endreq ;
  if ( m_partonsFromTruth ) log << MSG::INFO << " o  All partons" << endreq ;
  log << MSG::INFO << "Keep only particles with Pt > " << m_ptTruthCut << " MeV" << endreq ;
  log << MSG::INFO << "Keep e/gamma conversions/interactions only for R < " 
      << m_rTruthConv << " mm and |Z| < " << m_zTruthConv << " mm" << endreq ;
  if ( m_useG4Particles ) {
    log << MSG::INFO << "Reco->Truth match using final state generator+Geant4 particles" << endreq ;
  } else {
    log << MSG::INFO << "Reco->Truth match using final state generator particles only" << endreq ;
  }


  log << MSG::INFO << name() <<" initialize() successful" << endreq;    

  m_deltaRmatch_squared = pow( m_deltaRmatch , 2 ) ;

  //NO!  this->load() ;

  return StatusCode::SUCCESS;
}
//===========================================================
StatusCode PAUtruthTool::finalize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::DEBUG  << "... in finalize() ..." << endreq ;
  if ( m_dumpTruthFile.is_open() ) {
    log << MSG::DEBUG << "Close file for truth dump" << endreq ;
    m_dumpTruthFile.close() ;
  }
  log << MSG::INFO << "Number of calls to photon extrapolator:   " << m_ncalls_gToCalo << endreq ;
  log << MSG::INFO << "Number of calls to electron extrapolator: " << m_ncalls_eToCalo << endreq ;
  log << MSG::INFO  << name() <<" finalize() successful" << endreq;
  return StatusCode::SUCCESS;
}
//===========================================================
void PAUtruthTool::handle(const Incident& inc) {
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "Got informed of incident: " << inc.type()
      << " generated by: " << inc.source() << endreq;

  if ( inc.type() == "BeginEvent" ){
    StatusCode sc = this->load();
    if ( sc.isFailure() ){
      log << MSG::DEBUG << "Could not load PAUtruthTool in handle()." << endreq;
    }
  }

}
//===========================================================
void PAUtruthTool::clear() {
  m_MCloaded = false ;
  m_isHggMC = false ;
  m_isHiggsMC = false ;
  m_mytruthFinalState.clear() ;
  m_truthJets.clear() ;
  m_matchedWithReco.clear() ;
  //
  m_truthPartToCalo.clear();
  m_gen2truth.clear();
  //
  m_isConvMC.clear() ;
  m_RconvMC.clear() ;
  m_XconvMC.clear() ;
  m_YconvMC.clear() ;
  m_ZconvMC.clear() ;
  //
  m_HiggsZVMC = -99999 ;
  m_HiggsParents.clear();
  m_HiggsDaughters.clear();

  m_GravitonParents.clear();
  m_GravitonDaughters.clear();

  m_HardProcPar.clear();
  m_HardProcProd.clear();
}
//===========================================================
StatusCode PAUtruthTool::load() {

  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "in load()..." << endreq ;

  static bool dontLoad = false ;
  static int  failLoad = 0 ;

  if ( !dontLoad && failLoad>10 ) {
    log << MSG::INFO << "These events seem to be without truth -- STOP TRYING TO LOAD  TRUTH" << endreq ;
    dontLoad = true ;
  }

  if ( dontLoad ) {
    m_MCloaded = false ;
    log << MSG::DEBUG << "SKIP LOADING TRUTH" << endreq ;
    return StatusCode::FAILURE ;
  }

//   // first, check whether THIS run/event is already loaded ...

//   static int last_run_loaded = -99999 ;
//   static int last_event_loaded = -99999 ;

  const EventInfo* eventInfo;
  if ( m_storeGate->retrieve(eventInfo).isFailure() ) {
    log << MSG::WARNING << "Could not get  EventInfo from StoreGate" << endreq;
    m_run0 = m_evt0 = -1 ;
  } else {
    EventID* evtid = eventInfo->event_ID();
    m_run0 = evtid->run_number();
    m_evt0 = evtid->event_number();
  }

//   if ( m_run0 == last_run_loaded && m_evt0 == last_event_loaded ) {
//     log << MSG::DEBUG << "Event already loaded, don't do anything, run=" << m_run0 << " event=" << m_evt0 << endreq ;
//     return StatusCode::SUCCESS ;
//   }

//   // ok, now do the real loading...

  static int evtSeqNumber = -1 ;
  if ( evtSeqNumber < 0 ) {
    log << MSG::DEBUG << "First call of load() ..." << endreq ;
  }
  ++ evtSeqNumber ;

  log << MSG::VERBOSE << "**************** Run=" << m_run0 << " Event=" << m_evt0 << " **************** seq.nr.=" << evtSeqNumber << endreq ;

//   // now set to -99999 ... set to correct values only if everything went fine...
//   last_run_loaded   = -99999 ;
//   last_event_loaded = -99999 ;


  m_MCloaded = false ;

  log << MSG::VERBOSE << "\t now calling PAUtruthTool::clear()" << endreq ;
  this->clear() ;

  log << MSG::VERBOSE << "\t now calling PAUtruthTool::TruthExplorer()" << endreq ;
  StatusCode scTruth = TruthExplorer() ;
  if ( scTruth.isFailure() ) {
    ++ failLoad ;
    return scTruth ;
  }

  StatusCode scJets = TruthJetsLoader() ;
  if ( scJets.isFailure() ) {
    ++ failLoad ;
    return scJets ;
  }

//   last_run_loaded   = m_run0 ;
//   last_event_loaded = m_evt0 ;

  m_MCloaded = true ;
  failLoad = 0 ;

  ++ m_numberEventsMC ;
  if ( this->isHiggsMC() ) ++ m_numberHiggsMC ;
  if ( this->isHggMC() )   ++ m_numberHggMC ;

  log << MSG::VERBOSE << "exiting load()..." << endreq ;
  return StatusCode::SUCCESS ;
}
//===========================================================
StatusCode PAUtruthTool::TruthJetsLoader() {
  MsgStream log( msgSvc(), name() );
  const JetCollection* mcjetTES = 0 ;
  log << MSG::VERBOSE << "Try to retrieve Truth JetCollection with key " << m_truthJetsContainerName
      << " from event store..." << endreq ;
  StatusCode sc=m_storeGate->retrieve( mcjetTES, m_truthJetsContainerName );
  log << MSG::VERBOSE << "   ... retrieve performed..." << endreq ;
  if( sc.isFailure()  ||  ! mcjetTES ) {
    log << MSG::WARNING
	<< "No AOD MC truth particlejet container found in TES"
	<< endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::VERBOSE << "MC Truth Jet Container Successfully Retrieved" << endreq;
  
  JetCollection::const_iterator truItrB = mcjetTES->begin();
  JetCollection::const_iterator truItrE = mcjetTES->end();
  for ( JetCollection::const_iterator truItr=truItrB ; truItr != truItrE ; ++ truItr ) {
    m_truthJets.push_back( *truItr ) ;
  }
  return StatusCode::SUCCESS ;
}

void PAUtruthTool::TruthParticleClassifier(const TruthParticle* tp, 
					   bool& foundHiggs, bool& foundGraviton,
					   bool& foundHgg, bool& foundGgg,
					   const TruthParticle* lastInHardProcess){
  
  MsgStream log( msgSvc(), name() );

  int pdgId = tp->pdgId();
  unsigned nChildren = tp->nDecay();
  
  // look for Higgs 
  // ==============
  log << MSG::VERBOSE << "    ... looking for Higgs" << endreq ;
  if ( (pdgId == 25) && (tp->mother()) && (tp->mother()->pdgId() == 25) ) { 
    log << MSG::VERBOSE << "    ... this is Higgs " << endreq ;
    const TruthParticle* theHiggs = (tp) ;
    unsigned nHiggsMothers = theHiggs->mother()->nParents();
    foundHiggs = true ;
    m_HiggsZVMC = theHiggs->genParticle()->production_vertex()->point3d().z() ;
    for ( unsigned u=0 ; u<nHiggsMothers ; ++u )  
      m_HiggsParents.push_back( theHiggs->mother()->mother(u) ) ;
    for ( unsigned u=0 ; u<nChildren ; ++u ) 
	m_HiggsDaughters.push_back( theHiggs->child(u) ) ;
    if ( theHiggs->nDecay() == 2 ) {
      if ( theHiggs->child(0)->pdgId() == 22 && theHiggs->child(1)->pdgId() == 22 ) {
	foundHgg = true ;
	log << MSG::VERBOSE << "Run=" << m_run0 << " Event=" << m_evt0 << ": Higgs found in Truth, decaying to 2 photons" << endreq ;
      } else {
	log << MSG::VERBOSE << "Run=" << m_run0 << " Event=" << m_evt0 << ": Higgs found in Truth, but does not decay in two gammas" << endreq ;
      }
    } else {
      log << MSG::VERBOSE << "Run=" << m_run0 << " Event=" << m_evt0 << ": Higgs found in Truth, but does not decay in two particles" << endreq ;
    }
  }

  // look for Graviton                                                                                                                                      
  // ==============                                                                                                                                     
  log << MSG::VERBOSE << "    ... looking for Graviton" << endreq ;
  if ( (pdgId == 5000039) && (tp->mother()) && (tp->mother()->pdgId() == 5000039) ) {
    log << MSG::VERBOSE << "    ... this is Graviton" << endreq;
    const TruthParticle* theGraviton = (tp) ;
    unsigned nGravitonMothers = theGraviton->mother()->nParents();
    foundGraviton = true ;
    for ( unsigned u=0 ; u<nGravitonMothers ; ++u )  
      m_GravitonParents.push_back( theGraviton->mother()->mother(u) ) ;
    for ( unsigned u=0 ; u<nChildren ; ++u ) 
      m_GravitonDaughters.push_back( theGraviton->child(u) ) ;
    if ( theGraviton->nDecay() == 2 ) {
      if ( theGraviton->child(0)->pdgId() == 22 && theGraviton->child(1)->pdgId() == 22 ) {
	foundGgg = true ;
	log << MSG::VERBOSE << "Run=" << m_run0 << " Event=" << m_evt0 << ": Graviton found in Truth, decaying to 2 photons" << endreq ;
      } else {
	log << MSG::VERBOSE << "Run=" << m_run0 << " Event=" << m_evt0 << ": Graviton found in Truth, but does not decay in two gammas" << endreq ;
      }
    } else {
      log << MSG::VERBOSE << "Run=" << m_run0 << " Event=" << m_evt0 << ": Graviton found in Truth, but does not decay in two particles" << endreq ;
    }
  }
  
  // try to classify particle type
  // =============================
  bool neutrino        = ( fabs(pdgId) == 12 || fabs(pdgId) == 14 || fabs(pdgId) == 16 ) ;
  bool lepton          = ( fabs(pdgId) == 11 || fabs(pdgId) == 13 || fabs(pdgId) == 15 ) ;
  bool photon          = ( pdgId == 22 ) ;
  bool vectorBoson     = ( pdgId == 23 || fabs(pdgId) == 24 ) ;
  bool higgsParticle   = ( pdgId == 25 ) ;
  bool gravitonParticle = ( pdgId == 5000039 ) ;
  bool qedParticle     = ( pdgId == 22 || fabs(pdgId) == 11 ) ;
  bool muParticle      = ( fabs(pdgId) == 13 ) ;
  bool qcdParticle     = ( pdgId == 21 || ( fabs(pdgId) <= 6 && pdgId != 0 ) ) ;
  bool hadron          = ( !neutrino && !qedParticle && !muParticle && !qcdParticle ) ;

  bool elemParticle    = ( neutrino || lepton || qcdParticle || photon || vectorBoson || higgsParticle || gravitonParticle) ;

  // look for last particle in Hard Process
  // ======================================
  log << MSG::VERBOSE << "    ... looking for last particle in Hard Process" << endreq ;
  if ( isPromptParticleMC(tp) &&
       ( lastInHardProcess == 0 || 
	 tp->genParticle()->barcode() > lastInHardProcess->genParticle()->barcode() 
	 ) ) {
    log << MSG::VERBOSE << "    ... this may be last in hard process " << endreq ;
    if ( elemParticle )  lastInHardProcess = (tp);
  }
 
  if ( ! hasValidStatus(tp) ) return ;                          // skip duplicate particles (HEPEVT Standard)

  unsigned motherPDG = 0 ;
  if ( tp->mother() )  motherPDG = tp->mother()->pdgId() ;
  log << MSG::VERBOSE << "    ... mother pdg is " << motherPDG << endreq ;

  if ( tp->pt()<m_ptTruthCut && !isDirectPhotonMC(tp) ) return ;           // skip if low pt (except for prompt photons)

  log << MSG::VERBOSE << "    ... seeking production vertex ... " << endreq ;
  float r0=0 , z0=0 ;
  const HepMC::GenVertex* genVtx = tp->genParticle()->production_vertex() ;
  if ( genVtx ) {
    const Point3D<double> vtx(genVtx->point3d().x(), genVtx->point3d().y(),genVtx->point3d().z()) ;
    r0 = hypot (vtx.x(), vtx.y());
    z0 = vtx.z() ;
  }

  bool photonFromHiggs = ( pdgId==22 && motherPDG==25 ) ;
  bool photonFromGraviton = ( pdgId==22 && motherPDG==5000039 );

  bool isAlive = isFinalState(tp) ;
  bool interesting = ( photonFromHiggs && isAlive ) 
    || ( photonFromGraviton && isAlive )
    || ( m_egammasFromTruth && qedParticle && isAlive ) 
    || ( m_muonsFromTruth && muParticle && isAlive ) 
    || ( m_hadronsFromTruth && hadron && isAlive ) 
    || ( m_partonsFromTruth && qcdParticle ) ;

  if ( interesting ) {
    m_mytruthFinalState.push_back( tp ) ;
    //std::cout << "truthPart=" << (tp) << "\tgenPart=" << tp->genParticle() << std::endl ;
    m_gen2truth[tp->genParticle()] = (tp) ;
    //std::cout << "stored!" << std::endl ;
    log << MSG::VERBOSE << "    ... interesting particle!" << endreq ;
    log << MSG::VERBOSE 
	<< "\t pdg=" << tp->pdgId() 
	<< "\t r0=" << r0 << "\t z0=" << z0
	<< "\t Pt=" << tp->et()/1000
	<< "\t mother=" << motherPDG << "\t nChildren=" 
	<< tp->nDecay() << endreq ;

    for ( unsigned u=0 ; u<tp->nDecay() ; ++u ) {
      log << MSG::VERBOSE << "\t child[" << u << "]\t " ;
      if ( tp->child(u) ) log << "pdg=" << tp->child(u)->pdgId() << endreq ;
      else log << "has NULL pointer!" << endreq ;
    }

    log << MSG::VERBOSE << "    ... seeking decay info ..." << endreq ;
    bool OKint = false ;
    float Rint=-99999 , Zint=-99999 ;
    float Xint=-99999 , Yint=-99999 ;
    if ( tp->genParticle()->end_vertex() ) {
      const Point3D<double> vtx(tp->genParticle()->end_vertex()->point3d().x(),
			   tp->genParticle()->end_vertex()->point3d().y(),
			   tp->genParticle()->end_vertex()->point3d().z());
      Rint = hypot (vtx.x(), vtx.y());
      Zint = vtx.z() ;
      Xint = vtx.x() ;
      Yint = vtx.y() ;
    }

    if ( nChildren == 2 ) {
      int pdgChild[2] ;
      for ( unsigned u=0 ; u<2 ; ++u ) pdgChild[u] = tp->child(u)->pdgId() ;
      log << MSG::VERBOSE << "\t two children: pdg1=" << pdgChild[0] << " pdg2=" << pdgChild[1] << endreq ;
      if ( pdgId==22 ) {
	if ( pdgChild[0]+pdgChild[1]==0 && pdgChild[0]*pdgChild[1]==-121 ) { // gamma -> e+e-
	  if ( Rint < m_rTruthConv && fabs(Zint) < m_zTruthConv )  OKint = true ;
	} else {
	  log << MSG::DEBUG << "Run=" << m_run0 << " Event=" << m_evt0 << ": truth decay: gamma decays, but not in e+e-" << endreq ;
	  log << MSG::DEBUG << "\t (" << pdgId << ") -> (" << pdgChild[0] << ") (" << pdgChild[1] << ")" << endreq ;
	}
      } else if ( fabs(pdgId)==11 ) {
	if ( pdgChild[0]+pdgChild[1]==22+pdgId && pdgChild[0]*pdgChild[1]==22*pdgId ) { // e(+/-) -> e(+/-)gamma
	  if ( Rint < m_rTruthConv && fabs(Zint) < m_zTruthConv )  OKint = true ;
	} else {
	  log << MSG::DEBUG << "Run=" << m_run0 << " Event=" << m_evt0 << ": truth decay: e(+/-) decays, but not in e(+/-) gamma" << endreq ;
	  log << MSG::DEBUG << "\t (" << pdgId << ") -> (" << pdgChild[0] << ") (" << pdgChild[1] << ")" << endreq ;
	}
      }
    } else if ( nChildren > 0 && qedParticle ) {
      log << MSG::DEBUG << "Run=" << m_run0 << " Event=" << m_evt0 << ": truth decay: e/gamma decays but not in 2 particles" << endreq ;
      log << MSG::DEBUG << "\t (" << pdgId << ") ->" ;
      for ( unsigned u=0 ; u<nChildren ; ++u ) log << " (" << tp->child(u)->pdgId() << ")" ;
      log << endreq ;
    }
    m_isConvMC.push_back( OKint ) ;
    m_RconvMC.push_back( Rint ) ;
    m_ZconvMC.push_back( Zint ) ;
    m_XconvMC.push_back( Xint ) ;
    m_YconvMC.push_back( Yint ) ;
  }  // end if ( interesting )
    
  log << MSG::VERBOSE << "    ... finished with this particle" << endreq ;

  return;
}

//===========================================================
StatusCode PAUtruthTool::TruthExplorer() {
  MsgStream log( msgSvc(), name() );

  static int how_many_evts = -1 ;
  ++ how_many_evts ;

  bool foundHiggs=false , foundHgg=false ;
  bool foundGraviton=false , foundGgg=false ;

  if(m_fromESD){
    if( m_cnvTool == 0 || !m_cnvTool->execute().isSuccess() ){
      log << MSG::WARNING << "Could not convert McEventCollection into a TruthParticleContainer !" << endreq;
      return StatusCode::FAILURE;
    }
  }

  const TruthParticleContainer*  mcpartTES = 0;
  log << MSG::VERBOSE << "Try to retrieve TruthParticleContainer with key " << m_truthParticleContainerName << " from event store..." << endreq ;
  StatusCode sc=m_storeGate->retrieve( mcpartTES,  m_truthParticleContainerName);
  log << MSG::VERBOSE << "   ... retrieve performed..." << endreq ;
  if( sc.isFailure()  ||  ! mcpartTES ) {
    log << MSG::WARNING
        << "No " << m_truthParticleContainerName << " truth particle container found in TES"
	<< endreq; 
    return StatusCode::FAILURE;
  }
  log << MSG::VERBOSE << "MC Truth Container Successfully Retrieved" << endreq;
  
  TruthParticleContainer::const_iterator truItrB = mcpartTES->begin();
  TruthParticleContainer::const_iterator truItrE = mcpartTES->end();

  const TruthParticle* lastInHardProcess = 0;

  log << MSG::VERBOSE << "... looping through TruthParticleContainer ..." << endreq ;

  for ( TruthParticleContainer::const_iterator truItr=truItrB ; truItr != truItrE ; ++ truItr ) {

    log << MSG::VERBOSE << "... next particle ..." << endreq ;
    log << MSG::VERBOSE << "    ... sequential number " << truItr - truItrB << endreq ;
    log << MSG::VERBOSE << "    ... genParticle = " << (*truItr)->genParticle() << endreq ;
    log << MSG::VERBOSE << "    ... status = " << (*truItr)->genParticle()->status() << endreq ;
    log << MSG::VERBOSE << "    ... barcode = " << (*truItr)->genParticle()->barcode() << endreq ;
    log << MSG::VERBOSE << "    ... pid = " << (*truItr)->pdgId() << endreq ;
    log << MSG::VERBOSE << "    ... nMothers = " << (*truItr)->nParents() << endreq ;
    log << MSG::VERBOSE << "    ... nChildrens = " << (*truItr)->nDecay() << endreq ;

    TruthParticleClassifier(*truItr, foundHiggs, foundGraviton, foundHgg, foundGgg, lastInHardProcess);

  } // end loop through TruthParticleContainer

  // sort the truth items by pT
  std::sort( m_mytruthFinalState.begin(), m_mytruthFinalState.end(), PAUutils::High2LowByPt<const TruthParticle>() );

  // set up hard process...

  log << MSG::VERBOSE << "... now setting up hard process" << endreq ;

  if ( lastInHardProcess != 0 ) {
    const std::vector<const TruthParticle*> mothers = getMothers(lastInHardProcess) ;
    for ( unsigned i=0 ; i<mothers.size() ; ++i ) {
      m_HardProcPar.push_back( mothers[i] ) ;
    }
    if ( m_HardProcPar.size() > 0 ) {
      const std::vector<const TruthParticle*> children = getChildren(m_HardProcPar[0]) ;
      for ( unsigned i=0 ; i<children.size() ; ++i ) { // loop through decays of 1st parent
	const TruthParticle* prodPart = children[i];
	// check that all parents of this decay are the same for lastInHardProcess
	const std::vector<const TruthParticle*> mothersOfProd = getMothers(prodPart) ;
	bool ok = ( mothersOfProd.size() == m_HardProcPar.size() ) ;
	for ( unsigned j=0 ; j<mothersOfProd.size() ; ++j ) { // loop through parents
	  const TruthParticle* thisParent = mothersOfProd[j] ;
	  std::vector<const TruthParticle*>::const_iterator where_it = 
	    find( m_HardProcPar.begin() , m_HardProcPar.end() , thisParent ) ;
	  if ( where_it == m_HardProcPar.end() || *where_it != thisParent ) {
	    ok = false ;
	    break ;
	  }
	} // end loop through parents
	if ( ok ) {
	  m_HardProcProd.push_back( prodPart ) ;
	} else {
	  log << MSG::WARNING << "Something wrong in filling vector of hard process products..." << endreq ;
	}
      }
    } // end loop through decays of 1st parent
  } // end if

  // sort these items, so we have reproducible output later on
  std::sort( m_HardProcProd.begin(), m_HardProcProd.end(), PAUutils::High2LowByPt<const TruthParticle>() );
  std::sort( m_HardProcPar.begin() , m_HardProcPar.end() , PAUutils::High2LowByPt<const TruthParticle>() );

  log << MSG::VERBOSE << "... finished setting up hard process" << endreq ;

  for ( unsigned u=0 ; u<m_mytruthFinalState.size() ; ++u ) {
    m_matchedWithReco.push_back( false ) ;
  }

  m_isHiggsMC = foundHiggs ;
  m_isHggMC   = foundHgg ;
  if ( ! foundHiggs ) {
    log << MSG::VERBOSE << "Higgs not found in Truth" << endreq ;
  }

  m_isGravitonMC = foundGraviton ;
  m_isGggMC   = foundGgg ;
  if ( ! foundGraviton ) {
    log << MSG::VERBOSE << "Graviton not found in Truth" << endreq ;
  }

  return StatusCode::SUCCESS ;
}
//===========================================================
StatusCode PAUtruthTool::DumpTruth(bool onlyRunEvt) {
  MsgStream log( msgSvc(), name() );

  static bool firstTime = true ;
  if ( firstTime ) {
    std::string dumpTruthFileName = "dumpTruth.txt" ;
    log << MSG::INFO << "Open file " << dumpTruthFileName << endreq ;
    m_dumpTruthFile.open(dumpTruthFileName.c_str()) ;
    firstTime = false ;
  }

  const TruthParticleContainer*  mcpartTES = 0;
  StatusCode sc=m_storeGate->retrieve( mcpartTES, m_truthParticleContainerName );
  if( sc.isFailure()  ||  ! mcpartTES ) {
    log << MSG::WARNING
        << "No " << m_truthParticleContainerName << " truth particle container found in TDS"
        << endreq;
    return StatusCode::FAILURE;
  }
  log <<MSG::VERBOSE << "MC Truth Container Successfully Retrieved" << endreq;
  
  TruthParticleContainer::const_iterator truItrB = mcpartTES->begin();
  TruthParticleContainer::const_iterator truItrE = mcpartTES->end();

  std::map<const TruthParticle*,int> mapOfTruth ;
  for ( TruthParticleContainer::const_iterator truItr=truItrB ; truItr != truItrE ; ++ truItr ) {
    int seqCounter = truItr - truItrB ;
    const TruthParticle* part = *truItr ;
    mapOfTruth[part] = seqCounter ;
  }

  if ( onlyRunEvt ) {
    m_dumpTruthFile << "{ Run=" << m_run0 << " Event=" << m_evt0 << " not selected }" << std::endl ;
    return StatusCode::SUCCESS ;
  }
  m_dumpTruthFile << "**************** Run=" << m_run0 << " Event=" << m_evt0 << " ****************" << std::endl ;

  for ( TruthParticleContainer::const_iterator truItr=truItrB ; truItr != truItrE ; ++ truItr ) {

    //bool selected = false ;
    //bool matched = false ;
    std::string FLAG = "" ;
    std::vector<const TruthParticle*>::const_iterator where_it = 
      find( m_mytruthFinalState.begin() , m_mytruthFinalState.end() , (*truItr) ) ;
    if ( where_it != m_mytruthFinalState.end() ) {
      //selected = true ;
      FLAG += "S" ;
      int i = where_it - m_mytruthFinalState.begin() ;
      if ( m_matchedWithReco[i] ) {
	//matched = true ;
	FLAG += "M" ;
      }
    }

    int pdgId = (*truItr)->pdgId();
    unsigned nMothers = (*truItr)->nParents();
    unsigned nChildren = (*truItr)->nDecay();
    int seqCounter = truItr - truItrB ;
    const HepMC::GenParticle* gen = (*truItr)->genParticle() ;
    if ( gen ) {
      m_dumpTruthFile << "[" << seqCounter << "] " << FLAG
		      << "\t pid=" << pdgId << "\t nParents=" << nMothers << "\t nChildren=" << nChildren 
		      << "\t barcode=" << gen->barcode() << "\t status=" << gen->status() 
		      << std::endl ;
      if ( gen->production_vertex() ) {
	  const Point3D<double> vtx(gen->production_vertex()->point3d().x(),
			       gen->production_vertex()->point3d().y(),
			       gen->production_vertex()->point3d().z());
          m_dumpTruthFile << "\t r0=" << hypot(vtx.x(), vtx.y()) << "\t z0=" << vtx.z()
			<< std::endl ;
      }
      if ( gen->end_vertex() ) {
	const Point3D<double> vtx(gen->end_vertex()->point3d().x(),
			     gen->end_vertex()->point3d().y(),
			     gen->end_vertex()->point3d().z());
	m_dumpTruthFile << "\t rf=" << hypot(vtx.x(), vtx.y()) << "\t zf=" << vtx.z()
			<< std::endl ;
      }
    }
    m_dumpTruthFile << "\t Pt=" << (*truItr)->et() << "\t eta=" << (*truItr)->eta() 
		    << "\t phi=" << (*truItr)->phi() 
		    << "\t charge=" << (*truItr)->charge() << std::endl ;
    for ( unsigned i=0 ; i<nMothers ; ++i ) {
      const TruthParticle* mother= (*truItr)->mother(i);
      if ( mother )  m_dumpTruthFile << "\t  [" << mapOfTruth[mother] << "](" << mother->pdgId() << ")\t|"
				     << std::endl ;
    }
    m_dumpTruthFile << "\t\t\t|->   [" << seqCounter << "](" << pdgId << ")\t |" << std::endl ;
    for ( unsigned i=0 ; i<nChildren ; ++i ) {
      const TruthParticle* child= (*truItr)->child(i);
      if ( child )  m_dumpTruthFile << "\t\t\t\t\t |-> [" << mapOfTruth[child] << "](" << child->pdgId() << ")"
				    << std::endl ;
    }

  } // end loop on truth iterator
  return StatusCode::SUCCESS ;
}
//===========================================================
const TruthParticle* PAUtruthTool::getTruthMatch(const Rec::TrackParticle* tp) {
  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "getTruthMatch(Rec::TrackParticle)" << endreq;
  /*std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  
    truth =*/ m_TruthClassifierTool->particleTruthClassifier(tp);
  const HepMC::GenParticle* genpart = m_TruthClassifierTool->getGenPart();
  const TruthParticle* truthpart    = getTruthParticle(genpart);
  flagMatch(truthpart);
  return truthpart;
}

//===========================================================
const TruthParticle* PAUtruthTool::getTruthMatch(const Analysis::Muon* mu) {
  /*std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  
    truth =*/ m_TruthClassifierTool->particleTruthClassifier(mu);
  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "getTruthMatch(Analysis::Muon)" << endreq;
  const HepMC::GenParticle* genpart = m_TruthClassifierTool->getGenPart();
  if ( genpart==0 ) debugMessage("MCTruthClassifier failed for Muon",mu,mu->inDetTrackParticle());
  const TruthParticle* truthpart    = getTruthParticle(genpart);
  if ( truthpart==0 ) {
    truthpart = getTruthMatch(dynamic_cast<const I4Momentum*>(mu));
    if ( truthpart )  debugMessage("Muon matched through simple 4mom cone",mu);
    else              debugMessage("Muon match failed",mu);
  }
  flagMatch(truthpart);
  return truthpart;
}

//===========================================================
const TruthParticle* PAUtruthTool::getTruthMatch(const Analysis::Electron* el) {
  MsgStream log( msgSvc(), name() );
  /*std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  
    truth =*/ m_TruthClassifierTool->particleTruthClassifier(el);
  log << MSG::VERBOSE << "getTruthMatch(Analysis::Electron)" << endreq;
  const HepMC::GenParticle* genpart = m_TruthClassifierTool->getGenPart();
  const TruthParticle* truthpart    = getTruthParticle(genpart);
  if ( genpart==0 ) {
    debugMessage("MCTruthClassifier failed for Electron",el,el->trackParticle());
  } else {
    if ( truthpart ) debugMessage("Electron matched through MCTruthClassifier",el,0,genpart);
    else             debugMessage("MCTruthClassifier succeeded for Electron, but no truthparticle",el,0,genpart);
  }
  if ( truthpart==0 ) {
    truthpart = getTruthMatch(el->cluster(),(std::string)"electron");
    if ( truthpart )  debugMessage("Electron matched through cluster",el);
    else              debugMessage("Electron match failed",el);
  }
  if ( truthpart==0 ) {
    truthpart = getTruthMatch(dynamic_cast<const I4Momentum*>(el));
    if ( truthpart )  debugMessage("Electron matched through simple 4mom cone",el);
    else              debugMessage("Electron match failed",el);
  }
  flagMatch(truthpart);
  return truthpart;
}

//===========================================================
const TruthParticle* PAUtruthTool::getTruthMatch(const Analysis::Photon* ph) {
  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "getTruthMatch(Analysis::Photon)" << endreq;
  const TruthParticle* truthpart ;
  if ( m_useTruthClassifierTool ){
    /*std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  
      truth =*/ m_TruthClassifierTool->particleTruthClassifier(ph);
    const HepMC::GenParticle* genpart = m_TruthClassifierTool->getGenPart();
    truthpart    = getTruthParticle(genpart);
  } else if ( m_useExtrapolation ) {
    truthpart = getTruthMatch(ph->cluster(),(std::string)"photon");
  } else {
    truthpart = getTruthMatch(dynamic_cast<const I4Momentum*>(ph));
  }
  flagMatch(truthpart);
  return truthpart;
}

//===========================================================
std::vector<const TruthParticle*> PAUtruthTool::getTruthConvMatch(const Analysis::Photon* ph) {
  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "getTruthConvMatch(Analysis::Photon)" << endreq;
  m_TruthClassifierTool->particleTruthClassifier(ph);
  std::vector<const HepMC::GenParticle*> vec_genpart = m_TruthClassifierTool->getCnvPhotTrkToTruthPart();
  std::vector<const TruthParticle*> vec_truthpart ;
  for (unsigned int i=0 ; i<vec_genpart.size() ; i++) {
    vec_truthpart.push_back(getTruthParticle(vec_genpart.at(i)));
  }
  return vec_truthpart;
}

//===========================================================
void PAUtruthTool::debugMessage(const std::string& msg,const I4Momentum* p,const Rec::TrackParticle* trk,const HepMC::GenParticle* genpart) const {
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "TruthMatch: " << msg << "\trun=" << m_run0 << " event=" << m_evt0 
      << "\tpt=" << p->pt() << "\teta=" << p->eta() ;
  if ( trk && trk->trackSummary() ) {
    log << "\tnBHits=" << trk->trackSummary()->get(Trk::numberOfBLayerHits)
	<< " nPixelHits=" << trk->trackSummary()->get(Trk::numberOfPixelHits)
	<< " nSCTHits=" << trk->trackSummary()->get(Trk::numberOfSCTHits)
	<< " nTRTHits=" << trk->trackSummary()->get(Trk::numberOfTRTHits) ;
  }
  if ( genpart ) {
    log << "\tbarcode=" << genpart->barcode() << " status" << genpart->status() << " pdg=" << genpart->pdg_id() 
	<< " pt=" << genpart->momentum().perp() << " eta=" << genpart->momentum().pseudoRapidity() ;
  }
  log << endreq ;
}

//===========================================================
const TruthParticle* PAUtruthTool::getTruthMatch(const CaloCluster* clus, const std::string partName) {
  int preferredPartPID = 0;
  if ( partName=="photon" )   preferredPartPID = 22;
  if ( partName=="electron" ) preferredPartPID = 11;
  //
  if ( clus == 0 ) return 0 ;
  int bestRank = -99999 ;
  //int uBestMatch = -1 ;
  const TruthParticle* bestEvgenPart = 0;
  float etaClus = clus->etaBE(2);
  float phiClus = clus->phiBE(2);
  for ( unsigned u=0 ; u<m_mytruthFinalState.size() ; ++u ) {
    const TruthParticle* tp = m_mytruthFinalState[u] ;
    if ( tp->charge()==0 && tp->pt()<m_ptCutNeutral ) continue ; // low pt, skip!
    if ( tp->charge()!=0 && tp->pt()<m_ptCutCharged ) continue ; // low pt, skip!
    //float detaROI = etaClus - tp->eta() ;
    //float dphiROI = phiClus - tp->phi() ;
    //if ( inROICone(detaROI,dphiROI) > 1 ) continue ; // out of RoI, skip!
    const TruthParticle* evgenPart = getEvgenAncestor(tp) ;
    if ( evgenPart==0 ) continue ; // no generator ancestor, skip!
    double etaExtr, phiExtr ;
    if ( ! extrapolToCalo(tp,etaExtr,phiExtr) ) continue ; // skip if extrapolation failed
    float deta = etaClus - etaExtr ;
    float dphi = phiClus - phiExtr ;
    float dR2norm = inMatchCone(deta,dphi) ;
    if ( dR2norm > 1 ) continue ; // out of match cone, skip!
    int rank = -1 ;
    if ( inNarrowCone(deta,dphi) < 1 ) { // inside the narrow cone:
      // pt-ranking: 1GeV->1000 , 10GeV->1100 , 100GeV->1200, 1TeV->1300; granularity: dp/p = 10^(1/100) ~ 2%
      rank = 1000 + (int)ceil( 100*log10( tp->pt() / 1000 ) ) ; 
      // photon preference:
      if ( abs(evgenPart->pdgId())==preferredPartPID ) rank += 1000 ;
    } else { // outside the narrow cone, but inside the match cone
      rank = (int)ceil( -50 * log10(dR2norm) ) ;
      if ( rank > 900 )  rank = 900 ;
    }
    if ( rank > bestRank ) {
      bestRank   = rank ;
      //uBestMatch = u ;
      bestEvgenPart = evgenPart ;
    }
  }
  if ( !m_useG4Particles || bestEvgenPart!=0 ) {
    flagMatch(bestEvgenPart);
    return bestEvgenPart ;
  }
  
  // if match with evgen particles failed, look at G4 particles

  const TruthParticleContainer*  mcpartTES = 0;
  StatusCode sc=m_storeGate->retrieve( mcpartTES,  m_truthParticleContainerName);
  if( sc.isFailure()  ||  ! mcpartTES ) {
    // should not happen at this stage, but just in case...
    return 0 ;
  }
  TruthParticleContainer::const_iterator truItrB = mcpartTES->begin();
  TruthParticleContainer::const_iterator truItrE = mcpartTES->end();
  const TruthParticle* bestG4match = 0 ;
  int bestG4rank = -99999 ;
  for ( TruthParticleContainer::const_iterator truItr=truItrB ; truItr!=truItrE ; ++truItr ) {
    const TruthParticle* tp = *truItr;
    // skip non-geant particles
    if ( tp->genParticle()==0 || tp->genParticle()->barcode()<200000 ) continue ;
    // skip decayed/interacted particles
    if ( tp->nDecay()>0 || tp->genParticle()->end_vertex() ) continue ;
    // skip low pt
    if ( tp->charge()==0 && tp->pt()<m_ptCutNeutral ) continue ; 
    if ( tp->charge()!=0 && tp->pt()<m_ptCutCharged ) continue ; 
    // skip neutrinos
    int apdg = abs(tp->pdgId()) ;
    if ( apdg==12 || apdg==14 || apdg==16 ) continue ;
    // extrapolate...
    double etaExtr, phiExtr;
    if ( ! extrapolToCalo(tp,etaExtr,phiExtr) ) continue ; // skip if extrapolation failed
    float deta = etaClus - etaExtr ;
    float dphi = phiClus - phiExtr ;
    float dR2norm = inMatchCone(deta,dphi) ;
    if ( dR2norm > 1 ) continue ; // out of match cone, skip!
    int rank = -1 ;
    if ( inNarrowCone(deta,dphi) < 1 ) { // inside the narrow cone:
      // pt-ranking: 1GeV->1000 , 10GeV->1100 , 100GeV->1200, 1TeV->1300; granularity: dp/p = 10^(1/100) ~ 2%
      rank = 1000 + (int)ceil( 100*log10( tp->pt() / 1000 ) ) ; 
      // photon preference:
      if ( tp->pdgId()==22 ) rank += 1000 ;
    } else { // outside the narrow cone, but inside the match cone
      rank = (int)ceil( -50 * log10(dR2norm) ) ;
      if ( rank > 900 )  rank = 900 ;
    }
    if ( rank > bestG4rank ) {
      bestG4match = tp ;
      bestG4rank  = rank ;
    }
  }
  flagMatch(bestG4match);
  return bestG4match ;
}

//===========================================================
const TruthParticle* PAUtruthTool::getEvgenAncestor(const TruthParticle* tp) {
  const TruthParticle* return_tp = tp;
  while ( return_tp->genParticle() && return_tp->genParticle()->barcode()>200000 ) {
    if ( return_tp->nParents() != 1 ) return 0 ;
    return_tp = return_tp->mother() ;
    if(return_tp == tp) break;
  }
  if ( return_tp->genParticle()==0 ) return 0 ;
  return return_tp ;
}

//===========================================================
void PAUtruthTool::flagMatch(const TruthParticle* tp) {
  if ( tp==0 ) return ;
  for ( unsigned i=0 ; i<m_mytruthFinalState.size() ; ++i ) {
    if ( m_mytruthFinalState[i]==tp ) {
      m_matchedWithReco[i] = true ;
      break ;
    }
  }
  return;
}

//===========================================================
const TruthParticle* PAUtruthTool::getTruthMatch(const I4Momentum* reco4p, bool checkType) {
  // first, check to see it it matches any of the types we've 
  if(checkType){
    const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(reco4p);
    if(tp) return getTruthMatch(tp);

    const Analysis::Muon* mu     = dynamic_cast<const Analysis::Muon*>    (reco4p);
    if(mu) return getTruthMatch(mu);

    const Analysis::Electron* el = dynamic_cast<const Analysis::Electron*>(reco4p);
    if(el) return getTruthMatch(el);

    const Analysis::Photon* ph   = dynamic_cast<const Analysis::Photon*>  (reco4p);
    if(ph) return getTruthMatch(ph);

    const CaloCluster* cl        = dynamic_cast<const CaloCluster*>       (reco4p);
    if(cl) return getTruthMatch(cl);
  }

  int index = getIndexTruthMatch(reco4p) ;
  if ( index >= 0 ) {
    const TruthParticle* tp = m_mytruthFinalState[index] ;
    flagMatch(tp);
    return tp ;
  }
  return 0 ;
}
//===========================================================
int PAUtruthTool::getIndexTruthMatch(const I4Momentum* reco4p) {
  const double PI = 3.141592654 ;
  if ( ! m_MCloaded ) return -1 ;
  if ( reco4p == NULL ) return -1 ;
  float bestdist2 = 99999 ;
  int bestMatch = -1 ;
  float recoEta = reco4p->eta() ;
  float recoPhi = reco4p->phi() ;
  for ( unsigned u=0 ; u<m_mytruthFinalState.size() ; ++u ) {
    float deta = recoEta - (m_mytruthFinalState[u])->eta();
    float dphi = recoPhi - (m_mytruthFinalState[u])->phi();
    while ( dphi > PI )  { dphi -= 2 * PI ; }
    while ( dphi < -PI ) { dphi += 2 * PI ; }
    float dist2 = pow(deta,2) + pow(dphi,2) ;
    if ( dist2 < bestdist2 ) {
      bestdist2 = dist2 ;
      bestMatch = u ;
    }
  }
  if ( bestdist2 > m_deltaRmatch_squared ) bestMatch = -1 ;
  if ( bestMatch >= 0 ) m_matchedWithReco[bestMatch] = true ;
  return bestMatch ;
}
//===========================================================
const TruthParticle* PAUtruthTool::getTruthMatchHardProcess(const I4Momentum* reco4p) {
  const double PI = 3.141592654 ;
  if ( ! m_MCloaded ) return 0 ;
  if ( reco4p == NULL ) return 0 ;
  float bestdist2 = 99999 ;
  int bestMatch = -1 ;
  float recoEta = reco4p->eta() ;
  float recoPhi = reco4p->phi() ;
  for ( unsigned u=0 ; u<m_HardProcProd.size() ; ++u ) {
    float deta = recoEta - (m_HardProcProd[u])->eta();
    float dphi = recoPhi - (m_HardProcProd[u])->phi();
    if ( dphi > PI ) dphi -= 2 * PI ;
    else if ( dphi < -PI ) dphi += 2 * PI ;
    float dist2 = pow(deta,2) + pow(dphi,2) ;
    if ( dist2 < bestdist2 ) {
      bestdist2 = dist2 ;
      bestMatch = u ;
    }
  }
  if ( bestdist2 > m_deltaRmatch_squared ) bestMatch = -1 ;
  if ( bestMatch < 0 ) return 0 ;
  return m_HardProcProd[bestMatch] ;
}
//===========================================================
bool PAUtruthTool::hasValidStatus(const TruthParticle* truePart)  const {
  if ( truePart->genParticle() == 0 ) return false ;
  int status = truePart->genParticle()->status() ;
  return ( status == 1 || status == 2 ) ;
}
//===========================================================
bool PAUtruthTool::isFinalState(const TruthParticle* truePart)  const {
  if ( truePart==0 ) return false ;
  if ( truePart->genParticle()==0 ) return false ;
  // decayed in generator?
  if ( truePart->genParticle()->status()!=1 ) return false ; 
  // geant4 particles are not considered in the final state
  return ( truePart->genParticle()->barcode() < 200000 ) ;
}
//===========================================================
bool PAUtruthTool::isDecayed(const TruthParticle* truePart)  const {
  if ( truePart->genParticle() == 0 ) return false ;
  int status = truePart->genParticle()->status() ;
  return ( status == 2 ) ;
}
//===========================================================
const TruthParticle* PAUtruthTool::getMother(const TruthParticle* truePart) const {
  const std::vector<const TruthParticle*> mothers = getMothers(truePart) ;
  if ( mothers.size() == 1 ) {
    return mothers[0] ;
  } else {
    return 0 ;
  }
}
//===========================================================
const std::vector<const TruthParticle*> PAUtruthTool::getMothers(const TruthParticle* truePart) const {
  std::vector<const TruthParticle*> mothers ;
  if ( truePart == 0 ) return mothers ;
  if ( truePart->nParents() == 0 ) return mothers ;
  const TruthParticle* whichPart = truePart ;

  // if mother is a duplicate, try climbing up the tree by one step...
  // make an initial check by doing it once, first:
  if( whichPart->nParents()==1 && whichPart->pdgId()==whichPart->mother()->pdgId() ){
    whichPart = whichPart->mother();
  }

  std::vector<const TruthParticle*> truePart_mothers;
  
  // now, keep searching until either we get to a case where there are multiple parents,
  // OR the "parent" is the particle itself.  In which case we'll be stuck in an infinite
  // loop, and we'll need to return somewhere.
  while ( whichPart && whichPart->nParents()==1 && whichPart->pdgId()==whichPart->mother()->pdgId() ) {
    if(whichPart->mother() == whichPart)
      break;
    else 
      whichPart = whichPart->mother() ;
    
    // see if we've found this one before
    if(find(truePart_mothers.begin(),truePart_mothers.end(),whichPart) != truePart_mothers.end())
      break;
    else
      truePart_mothers.push_back(whichPart);

    // final check: is this ancestor the particle itself?  If so, break out.
    if(whichPart == truePart)
      break;
  }
  if(!whichPart) return mothers;
  for ( unsigned u=0 ; u<whichPart->nParents() ; ++u ) mothers.push_back( whichPart->mother(u) ) ;
  return mothers ;
}
//===========================================================
const std::vector<const TruthParticle*> PAUtruthTool::getChildren(const TruthParticle* truePart) const {
  std::vector<const TruthParticle*> children ;
  if ( truePart == 0 ) return children ;
  if ( truePart->nDecay() == 0 ) return children ;
  const TruthParticle* whichPart = truePart ;
  // if this particle is a duplicate, try climbing down the tree by one step...
  while ( whichPart->nDecay()==1 && whichPart->pdgId()==whichPart->child(0)->pdgId() ) {
    if(whichPart->child(0) == whichPart){
      break;
    }
    else{
      whichPart = whichPart->child(0) ;
    }
    
    if(whichPart == truePart) break;
  }
  for ( unsigned u=0 ; u<whichPart->nDecay() ; ++u ) children.push_back( whichPart->child(u) ) ;
  return children ;
}
//===========================================================
bool PAUtruthTool::isConvMC(const TruthParticle* truePart,float& XconvMC,float& YconvMC,float& RconvMC,float& ZconvMC)  const { //will be deleted later (Marc)
  RconvMC = +9.999e+10 ;
  ZconvMC = +9.999e+10 ;
  XconvMC = +9.999e+10 ;
  YconvMC = +9.999e+10 ;
  if ( truePart == 0 ) return false ;
  if ( truePart->nDecay() < 2 ) return false ;
  int pdgId  = truePart->pdgId() ;
  bool OKint = false ;
  if ( truePart->genParticle()->end_vertex() ) {
    const Point3D<double> vtx(truePart->genParticle()->end_vertex()->point3d().x(),
			 truePart->genParticle()->end_vertex()->point3d().y(),
			 truePart->genParticle()->end_vertex()->point3d().z());
    RconvMC = hypot (vtx.x(), vtx.y());
    ZconvMC = vtx.z() ;
    XconvMC= vtx.x();
    YconvMC= vtx.y();

    OKint = ( RconvMC < m_rTruthConv ) && ( fabs(ZconvMC) < m_zTruthConv ) ;
  }
  if ( pdgId == 22 ) { // photon
    if ( truePart->nDecay()==2 ) {
      int pdgChild[2] ;
      for ( unsigned u=0 ; u<2 ; ++u ) {
	pdgChild[u] = truePart->child(u)->pdgId() ;
	OKint = OKint && ( fabs(truePart->child(u)->eta()) < m_etaTruthConv ) ;
	OKint = OKint && ( truePart->child(u)->pt() > m_ptTruthConv ) ;
      }
      if ( pdgChild[0]+pdgChild[1]==0 && pdgChild[0]*pdgChild[1]==-121 ) { // gamma -> e+e-
	return OKint ;
      }
    }
  } 
  //if ( fabs(pdgId) == 11 ) { // e+/e-
  //  if ( truePart->nParents()==1 && truePart->mother()->nDecay()==2 ) {
  //    int pdgBrother[2] ;
  //    for ( unsigned u=0 ; u<2 ; ++u ) pdgBrother[u] = truePart->mother()->child(u)->pdgId() ;
  //    if ( pdgBrother[0]+pdgBrother[1]==(22+pdgId) && pdgBrother[0]*pdgBrother[1]==(22*pdgId) ) { // e(+/-) -> e(+/-)gamma
  //	return OKint ;
  //    }
  //  }
  //}
  return false ;
}
//===========================================================
float PAUtruthTool::GetMCweight()  const {

  MsgStream log( msgSvc(), name() );

  const McEventCollection*  mcTruthTES = 0 ;
  log << MSG::VERBOSE << "Try to retrieve MCEventCollection with key " << m_truthEventCollectionName << " from event store..." << endreq ;
  StatusCode sc=m_storeGate->retrieve( mcTruthTES, m_truthEventCollectionName);
  log << MSG::VERBOSE << "   ... retrieve performed..." << endreq ;
  if( sc.isFailure()  ||  ! mcTruthTES ) {
    log << MSG::WARNING
        << "No " << m_truthEventCollectionName << " container found in TES"
        << endreq;
    return -99999 ;
  }
  log << MSG::VERBOSE << m_truthEventCollectionName  << " Container Successfully Retrieved" << endreq;

  float myweight = 0;

  McEventCollection::const_iterator itB = mcTruthTES->begin();
  McEventCollection::const_iterator itE = mcTruthTES->end();

  for ( McEventCollection::const_iterator it=itB ; it != itE ; ++ it ) {
    const HepMC::GenEvent * genEvt = (*it);
    const HepMC::WeightContainer & wtCont= genEvt->weights();
    unsigned sz = wtCont.size();
    log << MSG::VERBOSE << "PAUtruthTool::GetMCweight() size --->> " << sz << " " << wtCont[0] << " " << wtCont[1] << endreq;
    if (sz==1) {
      myweight = wtCont[0];
    } else if (sz>=2) {
      myweight = wtCont[1];
    }
  }

  log << MSG::VERBOSE << "PAUtruthTool::GetMCweight() : Got MC weight ---------->>> " << myweight << endreq;

  return myweight;

}
//===========================================================
bool PAUtruthTool::isConvMC(const TruthParticle* truePart)  const {
  float RconvMC, ZconvMC ;
  float XconvMC,YconvMC;
  return isConvMC(truePart,XconvMC,YconvMC,RconvMC,ZconvMC) ;
}
//===========================================================
bool PAUtruthTool::isFinalStatePhotonMC(const TruthParticle* truePart)  const {
  return ( isFinalState(truePart) && truePart->pdgId()==22 ) ;
}
//===========================================================
bool PAUtruthTool::isQuarkBremMC(const TruthParticle* truePart)  const {
  if ( ! isFinalStatePhotonMC(truePart) ) return false ;
  const std::vector<const TruthParticle*> mothers = getMothers(truePart) ;
  if ( mothers.size() != 1 )  return false ;
  int pdgMother = mothers[0]->pdgId() ;
  return ( pdgMother==21 || ( fabs(pdgMother)<7 && pdgMother!=0 ) ) ;
}

// the nomenclature here is a little bit messy.  I've reorganized things so that
// what used to be "isPromptPhotonMC" is now "isDirectPhotonMC", since that's
// really what they meant.  Now "isPromptPhotonMC" has a different definition,
// one that's less generator-specific.

bool PAUtruthTool::isDirectPhotonMC(const TruthParticle* truePart)  const {
  return ( isFinalStatePhotonMC(truePart) && isPromptParticleMC(truePart) ) ;
}

bool PAUtruthTool::isPromptPhotonMC(const TruthParticle* truePart)  const {
  if( ! isFinalStatePhotonMC(truePart) ) return false ;

  const std::vector<const TruthParticle*> mothers = getMothers(truePart) ;
  if ( mothers.size() != 1 )  return false ;
  int pdgMother = abs(mothers[0]->pdgId());
  
  // FIXME: Marcello claims we need some magic here, to avoid cases where the mother
  // is the photon itself.

  if (pdgMother>6 && pdgMother!=21 && pdgMother!=22 ) return false;
  return true;
}

//===========================================================
bool PAUtruthTool::isPromptParticleMC(const TruthParticle* truePart)  const {
  if ( truePart == 0 ) return false ;
  const std::vector<const TruthParticle*> mothers = getMothers(truePart) ;
  unsigned nmothers = mothers.size() ;
  if ( nmothers == 0 ) {
    // particles with NO mother are NEVER classified as PROMPT:
    return false ;
  } else if ( nmothers == 1 ) {
    // particles with ONE mother are classified as PROMPT if coming from non-qcd-boson decay:
    // (including exotics like heavy bosons, MSSM higgs, graviton)
    int aPdgMother = abs(mothers[0]->pdgId()) ;
    return  (( aPdgMother>=23 && aPdgMother<=39 ) || aPdgMother==5000039 ) ;
  } else {
    // particles with more mothers are classified as PROMPT if they come from at least 1 parton:
    // (is this sensible?)
    int nParentPartons = 0 ;
    for ( unsigned u=0 ; u<nmothers ; ++u ) {
      int pdgMother = mothers[u]->pdgId() ;
      if ( pdgMother==21 || ( fabs(pdgMother)<7 && pdgMother!=0 ) )  ++nParentPartons ;
    }
    return ( nParentPartons >= 1 ) ;
  }
}
//===========================================================
int PAUtruthTool::getHardProcessType() const {

  unsigned nQuarks=0 , nGluons=0 , nPhotons=0 ;
  for ( unsigned i=0 ; i<m_HardProcProd.size() ; ++i ) {
    int pdg = m_HardProcProd[i]->pdgId() ;
    if ( pdg == 22 )
      ++nPhotons ;
    else if ( pdg == 21 )
      ++ nGluons ;
    else if ( pdg != 0 && fabs(pdg)<7 )
      ++ nQuarks ;
  }

  if ( m_isHiggsMC ) {

    if ( m_HiggsParents.size() == 2 ) {
      if ( m_HiggsParents[0]->pdgId() == 21 && m_HiggsParents[1]->pdgId() == 21 ) {
	return IPAUtruthTool::ggH*100+IPAUtruthTool::Higgs*10+nPhotons ; // Higgs from gluon fusion                                        
      } else {
	return IPAUtruthTool::vbfH*100+IPAUtruthTool::Higgs*10+nPhotons ; // Higgs from vector boson fusion                                        
      }
    }
    // need to implement WH, Zh, ttH ...                                                                                                    
    return IPAUtruthTool::unknownSignalType*10+nPhotons;

  } else if( m_isGravitonMC ){
    if ( m_GravitonParents.size() == 2 ) {
      if ( m_GravitonParents[0]->pdgId() == 21 && m_GravitonParents[1]->pdgId() == 21 ) {
        return IPAUtruthTool::ggG*100+IPAUtruthTool::Graviton*10+nPhotons ; // G* from gluon fusion                                            
      } else {
	return IPAUtruthTool::qqG*100+IPAUtruthTool::Graviton*10+nPhotons ; // G* from qqbar          
      }
    }
    return IPAUtruthTool::unknownSignalType*10+nPhotons ;

  } else{
    /*
    if ( m_HardProcProd.size() == 2 ) {
      int njets = 0;
      int ngammas = 0;
      for ( unsigned i=0 ; i<2 ; ++i ) {
	int pdg = m_HardProcProd[i]->pdgId() ;
	if ( pdg == 22 ) ++ ngammas ;
	else if ( pdg == 21 ) ++ njets ;
	else if ( pdg != 0 && fabs(pdg)<7 ) ++ njets ;
      }
      if ( ngammas == 2 )                        return -IPAUtruthTool::QCDgamgam ;
      else if ( ngammas == 1 && njets == 1 )     return -IPAUtruthTool::QCDgamjet ;
      else if (  njets == 2 )                    return -IPAUtruthTool::QCDjetjet ;
    }
    return -IPAUtruthTool::unknownBackgroundType ;
    */
    unsigned nPartons = nQuarks + nGluons ;
    if ( nPhotons==2 )
      return -(IPAUtruthTool::QCDgamgam) ;
    else if ( nPhotons==1 && nPartons>0 )
      return -(IPAUtruthTool::QCDgamjet) ;
    else if ( nPartons>=2 )
      return -(IPAUtruthTool::QCDjetjet) ;
    else
      return -(IPAUtruthTool::unknownBackgroundType*10+nPhotons) ;

  }
}
//===========================================================
bool PAUtruthTool::simpleExtrapolator(const TruthParticle* tp, double& etaCalo, double& phiCalo) {
  if ( tp == 0 ) return false ;
  if ( tp->genParticle() == 0 ) return false ;
  if ( tp->genParticle()->production_vertex() == 0 ) return false ;
  //
  const double B       = 2 ; //Tesla
  //const double Rbarrel = 1700 ; // [mm] approx radius of 2nd sampling in barrel
  //const double Zendcap = 3880 ; // [mm] Z-coordinate of 2nd sampling in endcap
  //const double Rbarrel = 1443 ; // [mm] radius of presampler in barrel
  //const double Rbarrel = 1200 ; // [mm] inner radius of coil in barrel
  //const double Zendcap = 3625 ; // [mm] Z-coordinate of presampler in endcap
  const double Rbarrel = 1500 ; // [mm]
  const double Zendcap = 3600 ; // [mm]
  //
  double q      = tp->charge() ;
  double pt     = tp->pt() ;         // [MeV/c] 
  double eta    = tp->eta() ;
  double phi    = tp->phi() ;
  double sgneta = (eta>=0) ? +1 : -1 ;
  double z0     = tp->genParticle()->production_vertex()->point3d().z() ;
  //
  if ( fabs(z0) > Zendcap ) return false ;
  // 
  if ( q == 0 ) {
    phiCalo = phi ;
    double z1 = z0 + Rbarrel * sinh(eta) ;
    if ( fabs(z1) < Zendcap ) {
      etaCalo = asinh( z1/Rbarrel ) ;
    } else {
      double z1 = Zendcap*sgneta ;
      double r1 = ( z1 - z0 ) / sinh(eta) ;
      etaCalo = asinh( z1/r1 ) ;
    }
  } else {
    double rho = pt / ( 0.3 * fabs(q) * B ) ; // [mm] curvature
    //
    // parametrize helix as:
    // x' = rho*(1-cos(alpha))
    // y' = rho*sin(alpha)
    // z  = z0 + rho*sinh(eta)*alpha
    // where (x',y') are a rotation of (x,y) such that th track is tangent to y' at perigee
    // therefore:
    // r' = 2*rho*sin(alpha/2)
    //
    double alphaB = +9999999 ;
    if ( Rbarrel < 2*rho ) {
      alphaB = 2 * asin( Rbarrel/(2*rho) ) ;
    }
    double alphaE = +9999999 ;
    if ( eta > 0 ) {
      alphaE = ( Zendcap - z0 ) / ( rho * sinh(eta) ) ;
    } else if ( eta < 0 ) {
      alphaE = ( -Zendcap - z0 ) / ( rho * sinh(eta) ) ;
    }
    double alpha = (alphaB<alphaE) ? alphaB : alphaE ;
    if ( alpha >= +9999990 ) return false ;
    //
    phiCalo = phi - q/fabs(q) * alpha/2 ;
    double r1 = 2*rho*sin(alpha/2) ;
    double z1 = z0 + rho*sinh(eta)*alpha ;
    etaCalo = asinh( z1/r1 ) ;
  }
  return true ;
}
//===========================================================
bool PAUtruthTool::particleExtrapolator(const TruthParticle* tp, double& etaCalo, double& phiCalo) {
  etaCalo = -99999.;
  phiCalo = -99999.;

  if ( tp == 0 ) return false ;
  const HepMC::GenParticle* gp = tp->genParticle() ;
  if ( gp == 0 ) return false ;
  HepMC::GenVertex* pvtx = gp->production_vertex();
  if ( pvtx == 0 ) return false ;
  // define particle perigee:
  //std::cout << "defining GlobalPosition ..." << std:: endl ;
  Amg::Vector3D pos( pvtx->position().x() , pvtx->position().y() , pvtx->position().z() ) ;
  //std::cout << "defining GlobalMomentum ..." << std:: endl ;
  Amg::Vector3D mom( gp->momentum().x() , gp->momentum().y() , gp->momentum().z() ) ;
  //std::cout << "defining Perigee ..." << std:: endl ;

  // get extrapolation       
  ImpactInCalo* imp(0);
  if ( tp->charge()==0 ) { //|| fabs(tp->pt()/tp->charge())>20000 ) {
    Trk::NeutralPerigee perigee(pos,mom,tp->charge(),pos);
    imp = m_gToCalo->getImpactInCalo (perigee,
				      Trk::nonInteracting);
    ++ m_ncalls_gToCalo ;
  } else {
    Trk::Perigee perigee(pos,mom,tp->charge(),pos);
    imp = m_eToCalo->getImpactInCalo (perigee,
				      Trk::nonInteracting);
    ++ m_ncalls_eToCalo ;
  }
  if(!imp) return false;
  etaCalo = imp->etaCaloLocal_2();
  phiCalo = imp->phiCaloLocal_2();
  return true ;
}
//=============================
bool PAUtruthTool::extrapolToCalo(const TruthParticle* tp, double& etaCalo, double& phiCalo) {
  MsgStream log(msgSvc(), name());

  etaCalo = -99999 ;
  phiCalo = -99999 ;

  if ( tp==0 ) {
    return false ;
  }

  ParticleExtrapolationMap::iterator it = m_truthPartToCalo.find(tp) ;
  if ( it != m_truthPartToCalo.end() ) {
    log << MSG::VERBOSE << "extrapolToCalo: truth particle " << tp << " already extrapolated" << endreq ;
    etaCalo = (it->second).first ;
    phiCalo = (it->second).second ;
    return true ;
  } else {
    log << MSG::VERBOSE << "extrapolToCalo: extrapolating truth particle " << tp << endreq ;
    bool ok = false ;
    if ( m_useAtlasExtrapolator ) {
      ok = particleExtrapolator(tp,etaCalo,phiCalo) ;
    } else {
      ok = simpleExtrapolator(tp,etaCalo,phiCalo) ;
    }

    if(!ok){
      log << MSG::VERBOSE << "extrapolation is NOT okay - returning dummy values." << endreq;
      etaCalo = -99999;
      phiCalo = -99999;
    }

    m_truthPartToCalo[tp] = EtaPhi(etaCalo,phiCalo) ;
    log << MSG::VERBOSE << "extrapolToCalo: done! " << tp << endreq ;

    return ok ;
  }
}
//=============================
const std::vector<double> PAUtruthTool::giveTrueVertex() const {
  MsgStream log( msgSvc(), name() );

  std::vector<double> vector_vertex;
  
  log << MSG::VERBOSE << "Try to retrieve TruthParticleContainer with key" << m_truthParticleContainerName << " from event store..." << endreq ;
  
  const TruthParticleContainer*  mcpartTES = 0;
  StatusCode sc=m_storeGate->retrieve( mcpartTES, m_truthParticleContainerName );

  //Could we remove this line because it will not learn anything more than other log instructions below ; log << MSG::VERBOSE << "   ... retrieve performed..." << endreq ;
  
  if( sc.isFailure()  ||  ! mcpartTES ) {
    log << MSG::WARNING	<< "No " << m_truthParticleContainerName << " MC truth particle container found in TES"	<< endreq; 
    return vector_vertex ;
  }
  
  log << MSG::VERBOSE << "MC Truth Container Successfully Retrieved" << endreq;
  
  bool found=0;
  
  TruthParticleContainer::const_iterator truItrB = mcpartTES->begin();
  TruthParticleContainer::const_iterator truItrE = mcpartTES->end();
  
  for ( TruthParticleContainer::const_iterator truItr=truItrB ; truItr != truItrE && !found ; ++ truItr ) {
    
    int pdgId = (*truItr)->pdgId();
    unsigned nMothers = (*truItr)->nParents();
    
    //  Added LAL for debugging
    //unsigned nChildren = (*truItr)->nDecay();    
    //     std::cout << "----------" << std::endl;
    //     std::cout << "barcode code=" << (*truItr)->genParticle()->barcode() << ", pid code=" << (*truItr)->pdgId() << ", status=" << (*truItr)->genParticle()->status() << std::endl;
    //     std::cout << "nb mothers=" << nMothers << ", nb children=" << nChildren << std::endl;    
    //     std::cout << "genParticle=" << (*truItr)->genParticle() << std::endl;
    
    if( (*truItr)->genParticle()->status() > 3)
      continue;
    
    if ( ((*truItr)->mother() && (*truItr)->mother()->genParticle() && (*truItr)->mother()->genParticle()->barcode()==1 ) //case of higgs
	 || 
	 ( ((*truItr)->mother() && (*truItr)->mother()->genParticle() && (*truItr)->mother()->nParents()==0 ) ) //case added for Herwig and MC@NLO
	 ||
	 ( ( pdgId==22 || pdgId==111) && (*truItr)->barcode()>10000. && nMothers==0 ) //case of single photons and single pions
	 ) {
      //std::cout<< " z vertex of event is " << (*truItr)->genParticle()-> production_vertex()->point3d().z() << " x of vertex is " <<  (*truItr)->genParticle()-> production_vertex()->point3d().x()<< " le y du vertex " <<  (*truItr)->genParticle()-> production_vertex()->point3d().y() << std::endl;
      if((*truItr)->genParticle()-> production_vertex()){
	vector_vertex.push_back((*truItr)->genParticle()-> production_vertex()->point3d().x());
	vector_vertex.push_back((*truItr)->genParticle()-> production_vertex()->point3d().y());
	vector_vertex.push_back((*truItr)->genParticle()-> production_vertex()->point3d().z());
      }else{
	log << MSG::ERROR << "No production vertex defined" << endreq;
      }
      found=1;
    } //end mother
  } //loop on truth particle
  
  return vector_vertex;
}
//===========================================================
// void PAUtruthTool::leadingPartonCone4(PAU::egamma* g, std::vector<lparton> * m_partonlist){
//   MsgStream log(msgSvc(), name());

//   const TruthParticleContainer*  mcpart = 0;
//   StatusCode sc = m_storeGate->retrieve( mcpart,m_truthParticleContainerName );

//   if( sc.isFailure()  ||  ! mcpart ) {
//     log << MSG::WARNING	<< "No " << m_truthParticleContainerName << " MC truth particle container found in TDS"	<< endreq; 
//     m_partonlist->clear() ;
//     return ;
//   }
  
//   log << MSG::VERBOSE << "MC Truth Container Successfully Retrieved" << endreq;

//   TruthParticleContainer::const_iterator truItr =  mcpart->begin();
//   TruthParticleContainer::const_iterator truItrE = mcpart->end();
  
//   lparton m_parton;

//   m_parton.pt_max = -999999;
//   m_parton.eta = -999999;
//   m_parton.phi = -999999;
//   m_parton.barcode = -999999;
//   m_parton.pdg = -999999;
  
//   for (; truItr<truItrE; truItr++) {
    
//     if ( abs ( (*truItr)->pdgId())<9 || abs ((*truItr)->pdgId()==21 ) ){
//       float dR = sqrt( 2*(cosh((*truItr)->eta()-g->cluster()->etaBE(2))-cos((*truItr)->phi()-g->cluster()->phiBE(2))));
//       if ( dR > 0.4 ) continue ; // PARTON is too far from Cone
//       if ( ((*truItr)->pt())>m_parton.pt_max ) {

// 	m_parton.pt_max = (*truItr)->pt();
// 	m_parton.eta = (*truItr)->eta();
// 	m_parton.phi = (*truItr)->phi();
// 	m_parton.barcode = (*truItr)->genParticle()->barcode();
// 	m_parton.pdg = (*truItr)->pdgId();
	
//       } // end look for Partons
      
//     }// ** seeing truth particles
//   }// ** end seeing truth particles
  
//   m_partonlist->push_back(m_parton);
 
// }
//===========================================================
std::vector<const TruthParticle*> PAUtruthTool::partonsInCone(const I4Momentum* pmom, float dRmax) {
  std::vector<const TruthParticle*> partons;

  MsgStream log(msgSvc(), name());

  const TruthParticleContainer*  mcpart = 0;
  StatusCode sc = m_storeGate->retrieve( mcpart,m_truthParticleContainerName );

  if( sc.isFailure()  ||  ! mcpart ) {
    log << MSG::WARNING	<< "No " << m_truthParticleContainerName << " MC truth particle container found in TDS"	<< endreq; 
    return partons;
  }
  
  log << MSG::VERBOSE << "MC Truth Container Successfully Retrieved" << endreq;

  TruthParticleContainer::const_iterator truItr =  mcpart->begin();
  TruthParticleContainer::const_iterator truItrE = mcpart->end();
  
  for (; truItr<truItrE; truItr++) {
    if ( hasValidStatus(*truItr) ) {
      int pdgId = (*truItr)->pdgId() ;
      if ( ( pdgId!=0 && abs(pdgId)<7 ) || pdgId==21 ) {
	float dR = PAUutils::deltaR( pmom , *truItr ) ;
	if ( dR < dRmax )  partons.push_back(*truItr) ;
      } 
    }
  }

  std::sort( partons.begin() , partons.end() , PAUutils::High2LowByPt<TruthParticle>() ) ;
  return partons ;
}

float PAUtruthTool::getPartonIsolation(const TruthParticle* gmom, float dRmax, bool OnlyBrothers){
  std::vector<const TruthParticle*> partons;

  MsgStream log(msgSvc(), name());

  const TruthParticleContainer*  mcpart = 0;
  StatusCode sc = m_storeGate->retrieve( mcpart,m_truthParticleContainerName );

  if( sc.isFailure()  ||  ! mcpart ) {
    log << MSG::WARNING << "No " << m_truthParticleContainerName << " MC truth particle container found in TDS" << endreq;
    return UNKNOWN;
  }

  log << MSG::VERBOSE << "MC Truth Container Successfully Retrieved" << endreq;

  if(!OnlyBrothers){
    TruthParticleContainer::const_iterator truItr =  mcpart->begin();
    TruthParticleContainer::const_iterator truItrE = mcpart->end();

    bool doMatch=false;                                                                                                                                                     
    for (; truItr<truItrE; truItr++) {                                                                                                                                          
      if ( isParton( (*truItr) ) ) {                                                                                                                                          
	if( (*truItr)->genParticle()->status() != 2 && (*truItr)->genParticle()->status() != 10902 ) continue;

	//** 'youngest approach' : take only youngest partons in cone  **  == those not having any parton children (i.e. string or cluster id) or no children at all.             
	doMatch=true;
	for(unsigned int u=0; u < (*truItr)->nDecay(); u++){
	  if( isParton( (*truItr)->child(u) ) ){
	  
	    doMatch=false; // PYTHIA	    
	    if( (*truItr)->child(u)->genParticle()->status()==158 ){ //HERWIG	      
	      doMatch=true;
	    }
	  }
	}
	if(!doMatch) continue;
      
	float dR = PAUutils::deltaR( gmom , *truItr ) ;
	if ( dR < dRmax ){
	  partons.push_back(*truItr) ;
	}
      }
    }
  }
  else{
    const std::vector<const TruthParticle*> mothers = getMothers( gmom ) ;
       
    if ( mothers.size() < 1 )  return UNKNOWN ;
    
    const std::vector<const TruthParticle*> children = getChildren( mothers[0] );
    
    for ( unsigned i=0 ; i<children.size() ; ++i ) {
      if ( hasValidStatus( children[i] ) || children[i]->status()==10902 ) {
	if ( isParton( children[i] ) ) {
	
	  float dR = PAUutils::deltaR( gmom , children[i] ) ;
	  if ( dR < dRmax )
	    partons.push_back(children[i]);
	}
      }      
    } 
  }


  float cpx=0, cpy=0, cpz=0, ce=0;

  for (unsigned int u=0 ; u<partons.size(); u++ ){
    cpx += partons[u]->px() ;
    cpy += partons[u]->py() ;
    cpz += partons[u]->pz() ;
    ce  += partons[u]->e() ;
  }
  
  CLHEP::HepLorentzVector* cvector = new HepLorentzVector();
  cvector->setPx(cpx) ;
  cvector->setPy(cpy) ;
  cvector->setPz(cpz) ;
  cvector->setE(ce) ;


  double return_et = cvector->et();
  delete cvector;
  return return_et;
}

float PAUtruthTool::getParticleIsolation(const TruthParticle* gmom, float dRmax , bool munu){
  std::vector<const TruthParticle*> truparticles;

  MsgStream log(msgSvc(), name());

  const TruthParticleContainer*  mcpart = 0;
  StatusCode sc = m_storeGate->retrieve( mcpart,m_truthParticleContainerName );

  if( sc.isFailure()  ||  ! mcpart ) {
    log << MSG::WARNING << "No " << m_truthParticleContainerName << " MC truth particle container found in TDS" << endreq;
    return UNKNOWN;
  }

  log << MSG::VERBOSE << "MC Truth Container Successfully Retrieved" << endreq;

  float cpx=0, cpy=0, cpz=0, ce=0;

  TruthParticleContainer::const_iterator truItr =  mcpart->begin();
  TruthParticleContainer::const_iterator truItrE = mcpart->end();

  for (; truItr<truItrE; truItr++) {
    if( ! isFinalState(*truItr) ) continue;

    if( ! munu ){
      int pdgId = fabs( (*truItr)->pdgId() );
      bool neutrino   = ( pdgId == 12 || pdgId == 14 || pdgId == 16 ) ;
      bool muParticle = ( pdgId == 13 ) ;

      if( neutrino || muParticle ) continue;
    }

    float dR = PAUutils::deltaR( gmom , *truItr ) ;
    if ( dR < dRmax && dR>0){
      truparticles.push_back(*truItr) ;
    }
  }

  for (unsigned int u=0 ; u<truparticles.size(); u++ ){
    cpx += truparticles[u]->px() ;
    cpy += truparticles[u]->py() ;
    cpz += truparticles[u]->pz() ;
    ce  += truparticles[u]->e() ;
  }

  CLHEP::HepLorentzVector* cvector = new HepLorentzVector();
  cvector->setPx(cpx) ;
  cvector->setPy(cpy) ;
  cvector->setPz(cpz) ;
  cvector->setE(ce) ;

  double return_et = cvector->et();
  delete cvector;
  return return_et;
}

bool PAUtruthTool::isParton(const TruthParticle* tp)
{
  return ( abs(tp->pdgId())<7 || tp->pdgId()==21 );
}


const TruthParticle* PAUtruthTool::getTruthParticle(const HepMC::GenParticle* genPart) {
  GenToTruthMap::iterator it = m_gen2truth.find(genPart) ;
  if ( it!=m_gen2truth.end() && it->second->genParticle()==genPart ) {
    // found in evgen truthparticles' final state
    return it->second ;
  } else if ( m_useG4Particles ) {
    const TruthParticleContainer*  mcpartTES = 0;
    StatusCode sc=m_storeGate->retrieve( mcpartTES,  m_truthParticleContainerName);
    if( sc.isFailure()  ||  ! mcpartTES ) {
      // should not happen at this stage, but just in case...
      return 0 ;
    }
    TruthParticleContainer::const_iterator truItrB = mcpartTES->begin();
    TruthParticleContainer::const_iterator truItrE = mcpartTES->end();
    for ( TruthParticleContainer::const_iterator truItr=truItrB ; truItr!=truItrE ; ++truItr ) {
      if ( (*truItr)->genParticle()->barcode()<200000 ) continue ;
      if ( (*truItr)->genParticle()==genPart )  return (*truItr) ;
    }
  }
  return 0 ;
}
#endif
