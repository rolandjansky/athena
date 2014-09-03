/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_Geant4Tools/TransportTool.h"
#include "ISF_Geant4Tools/G4AtlasRunManager.h"

#include "ISF_Geant4Interfaces/IPhysicsValidationUserAction.h"
#include "ISF_Geant4Interfaces/ITrackProcessorUserAction.h"
#include "ISF_Geant4Interfaces/IMCTruthUserAction.h"
#include "ISF_Geant4Interfaces/ISDActivateUserAction.h"

// ISF classes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ITruthBinding.h"
#include "ISF_Event/ISFParticleVector.h"

#include "ISF_HepMC_Event/HepMC_TruthBinding.h"

#include "HepMC/GenParticle.h"

// Athena classes
//#include "G4AtlasAlg/AthenaTrackingAction.h"
#include "G4AtlasAlg/PreEventActionManager.h"
#include "G4AtlasAlg/AthenaStackingAction.h"
//#include "G4AtlasAlg/TruthHepMCEventConverter.h"

#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/EventInformation.h"

#include "GeneratorObjects/McEventCollection.h"

//#include "AthenaPython/PyAthenaTool.h"

// FADS classes
#include "FadsActions/FadsRunAction.h"
#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsSteppingAction.h"
#include "FadsActions/FadsTrackingAction.h"
#include "FadsActions/FadsStackingAction.h"
#include "FadsPhysics/PhysicsListCatalog.h"
#include "FadsGeometry/FadsDetectorConstruction.h"

#include "MCTruth/PrimaryParticleInformation.h"

// Geant4 classes
#include "G4UImanager.hh"
#include "G4LorentzVector.hh"
#include "G4PrimaryVertex.hh"
#include "G4Trajectory.hh"
#include "G4Geantino.hh"


//________________________________________________________________________
iGeant4::G4TransportTool::G4TransportTool(const std::string& t,
				      const std::string& n,
				      const IInterface*  p )
: AthAlgTool(t,n,p),
  m_rndmGenSvc("AtDSFMTGenSvc",n),
  m_g4RunManagerHelper("iGeant4::G4RunManagerHelper/G4RunManagerHelper"),
  m_physicsValidationUserAction("iGeant4::PhysicsValidationUserAction/PhysicsValidationUserAction"),
  m_trackProcessorUserAction("iGeant4::TrackProcessorUserAction/TrackProcessorUserAction"),
  m_mcTruthUserAction("iGeant4::MCTruthUserAction/ISF_MCTruthUserAction"),
  m_sdActivateUserAction("iGeant4::SDActivateUserAction/SDActivateUserAction"),
  m_storeGate(0),
  m_mcEventCollectionName("TruthEvent")
    //, m_particleBroker("ISF_ParticleParticleBroker",n)
    //, m_particleHelper("ISF::ParticleHelper/ParticleHelper")
    //, m_configTool("PyAthena::Tool/G4ConfigTool")
{

  declareInterface<ITransportTool>(this);

  //declareProperty( "ParticleBroker" , m_particleBroker  , "ISF Particle Broker Svc");
  //declareProperty( "ParticleHelper" , m_particleHelper  , "ISF Particle Helper");
  //declareProperty("Geant4ConfigTool", m_geant4ConfigTool, "Geant4 configuration tool");

  declareProperty("McEventCollection",          m_mcEventCollectionName);

  declareProperty( "Dll",                       m_libList ="");
  declareProperty( "Physics",                   m_physList="");
  declareProperty( "FieldMap",                  m_fieldMap="");
  declareProperty( "RandomGenerator",           m_rndmGen ="");
  declareProperty( "RandomNumberService",       m_rndmGenSvc);

  declareProperty( "KillAllNeutrinos",          m_KillAllNeutrinos=true);
  declareProperty( "KillLowEPhotons",           m_KillLowEPhotons=-1.);

  declareProperty( "ReleaseGeoModel",           m_releaseGeoModel=true);

  declareProperty( "PhysicsValidationUserAction",  m_physicsValidationUserAction);
  declareProperty( "TrackProcessorUserAction",  m_trackProcessorUserAction);
  declareProperty( "MCTruthUserAction",         m_mcTruthUserAction);
  declareProperty( "SDActivateUserAction",      m_sdActivateUserAction);
  declareProperty( "G4RunManagerHelper",        m_g4RunManagerHelper);

  // get G4AtlasRunManager
  ATH_MSG_DEBUG("initialize G4AtlasRunManager");

  if (m_g4RunManagerHelper.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_g4RunManagerHelper);
  else {
    ATH_MSG_FATAL("Could not get "<<m_g4RunManagerHelper);
  }

  //p_runMgr = G4AtlasRunManager::GetG4AtlasRunManager();    // clashes with use of G4HadIntProcessor
  p_runMgr = m_g4RunManagerHelper ? m_g4RunManagerHelper->g4RunManager() : 0;

  if (p_runMgr) {

    G4VUserPhysicsList *thePL=FADS::PhysicsListCatalog::GetInstance()->GetMainPhysicsList();
    
    //  p_runMgr->SetPhysicsList(thePL);  //*AS* should be obsolete, line below is actually used
    p_runMgr->SetUserInitialization(thePL);
    p_runMgr->SetUserInitialization(new FADS::FadsDetectorConstruction);
    
    
    //trackingAction =new AthenaTrackingAction;
    m_stackingAction =new AthenaStackingAction;  // *AS* is this the same as below?
    
    p_runMgr->SetUserAction(FADS::FadsRunAction::GetRunAction());
    p_runMgr->SetUserAction(FADS::FadsEventAction::GetEventAction());
    p_runMgr->SetUserAction(FADS::FadsSteppingAction::GetSteppingAction());
    p_runMgr->SetUserAction(FADS::FadsTrackingAction::GetTrackingAction());
    p_runMgr->SetUserAction(FADS::FadsStackingAction::GetStackingAction());
  }
}

//________________________________________________________________________
iGeant4::G4TransportTool::~G4TransportTool()
{}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::initialize()
{
  ATH_MSG_VERBOSE("initialize");

  if (m_physicsValidationUserAction.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_physicsValidationUserAction);
  else {
    ATH_MSG_DEBUG("Could not get "<<m_physicsValidationUserAction);
    //return StatusCode::FAILURE;
  }
  if (m_trackProcessorUserAction.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_trackProcessorUserAction);
  else {
    ATH_MSG_FATAL("Could not get "<<m_trackProcessorUserAction);
    return StatusCode::FAILURE;
  }
  if (m_mcTruthUserAction.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_mcTruthUserAction);
  else {
    ATH_MSG_FATAL("Could not get "<<m_mcTruthUserAction);
    return StatusCode::FAILURE;
  }
  if (m_sdActivateUserAction.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_sdActivateUserAction);
  else {
    ATH_MSG_FATAL("Could not get "<<m_sdActivateUserAction);
    return StatusCode::FAILURE;
  }

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  if ( svcLocator->service("StoreGateSvc", m_storeGate).isFailure()) {
    ATH_MSG_WARNING("AthenaHitsCollectionHelper: could not accessStoreGateSvc!");
    return StatusCode::FAILURE;
  }
  
  G4UImanager *ui = G4UImanager::GetUIpointer();

  if (!m_libList.empty()) {
    ATH_MSG_INFO("G4AtlasAlg specific libraries requested ") ;
    std::string temp="/load "+m_libList;
    ui->ApplyCommand(temp);
  }

  if (!m_physList.empty()) {
    ATH_MSG_INFO("requesting a specific physics list "<< m_physList) ;
    std::string temp="/Physics/GetPhysicsList "+m_physList;
    ui->ApplyCommand(temp);
  }

  if (!m_fieldMap.empty()) {
    ATH_MSG_INFO("requesting a specific field map "<< m_fieldMap) ;
    ATH_MSG_INFO("the field is initialized straight away") ;
    std::string temp="/MagneticField/Select "+m_fieldMap;
    ui->ApplyCommand(temp);
    ui->ApplyCommand("/MagneticField/Initialize");
  }

  m_stackingAction->KillAllNeutrinos(m_KillAllNeutrinos);
  m_stackingAction->KillLowEPhotons (m_KillLowEPhotons);

  p_runMgr->SetReleaseGeo( m_releaseGeoModel );

  // *AS* TEST:
  // *AS* p_runMgr->Initialize();
  // *AS* but this is a good place


  

  ATH_MSG_VERBOSE("++++++++++++  ISF G4 G4TransportTool initialized  ++++++++++++");

  ATH_MSG_DEBUG("Setting checkmode to true");
  ui->ApplyCommand("/geometry/navigator/check_mode true");

  if (m_rndmGen=="athena" || m_rndmGen=="ranecu")     {
    // Set the random number generator to AtRndmGen
    if (m_rndmGenSvc.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not initialize ATLAS Random Generator Service");
      return StatusCode::FAILURE;
    }
    CLHEP::HepRandomEngine* engine = m_rndmGenSvc->GetEngine("AtlasG4");
    CLHEP::HepRandom::setTheEngine(engine);
    ATH_MSG_DEBUG("Random nr. generator is set to Athena");
  }
  else if (m_rndmGen=="geant4" || m_rndmGen.empty()) {
    ATH_MSG_INFO("Random nr. generator is set to Geant4");
  }

  ATH_MSG_DEBUG("initalize");
  /*
  if (m_particleBroker.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_particleBroker);
  else {
    ATH_MSG_FATAL("Could not get "<<m_particleBroker);
    return StatusCode::FAILURE;
  }
  //p_runMgr->setParticleBroker(&m_particleBroker);

  if (m_particleHelper.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_particleHelper);
  else {
    ATH_MSG_FATAL("Could not get "<<m_particleHelper);
    return StatusCode::FAILURE;
  }
  //p_runMgr->setParticleHelper(&m_particleHelper);
  */
  /*
  if (m_configTool.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_configTool);
  else {
    ATH_MSG_FATAL("Could not get "<<m_configTool);
    return StatusCode::FAILURE;
  }
  */
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::finalize()
{
  ATH_MSG_VERBOSE("++++++++++++  ISF G4 G4TransportTool finalized  ++++++++++++");

  ATH_MSG_DEBUG("\t terminating the current G4 run");

  p_runMgr->RunTermination();

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::process(const ISF::ISFParticle& isp) const
{
  
  static PreEventActionManager *preEvent=PreEventActionManager::
    GetPreEventActionManager();
  ATH_MSG_VERBOSE("++++++++++++  ISF G4 G4TransportTool execute  ++++++++++++");

  preEvent->Execute();

  ATH_MSG_DEBUG("Calling ISF_Geant4 ProcessEvent");

  G4Event* inputEvent=ISF_to_G4Event(isp);
  if (inputEvent) {

    bool abort = p_runMgr->ProcessEvent(inputEvent);

    if (abort) {
      ATH_MSG_WARNING("Event was aborted !! ");
      //ATH_MSG_WARNING("Simulation will now go on to the next event ");
      //ATH_MSG_WARNING("setFilterPassed is now False");
      //setFilterPassed(false);
      return 0;
    }
  }
  else {
    ATH_MSG_ERROR("ISF Event conversion failed ");
    return 0;
  }

  // const DataHandle <TrackRecordCollection> tracks;

  // StatusCode sc = m_storeGate->retrieve(tracks,m_trackCollName);

  // if (sc.isFailure()) {
  //   ATH_MSG_WARNING(" Cannot retrieve TrackRecordCollection " << m_trackCollName);
  // }

  // not implemented yet... need to get particle stack from Geant4 and convert to ISFParticle
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::processVector(const ISF::ConstISFParticleVector& particles) const
{
  ATH_MSG_DEBUG("processing vector of "<<particles.size()<<" particles");

  static PreEventActionManager *preEvent=PreEventActionManager::
    GetPreEventActionManager();
  ATH_MSG_VERBOSE("++++++++++++  ISF G4 G4TransportTool execute  ++++++++++++");

  preEvent->Execute();

  ATH_MSG_DEBUG("Calling ISF_Geant4 ProcessEvent");

  G4Event* inputEvent=ISF_to_G4Event(particles);
  if (inputEvent) {
    bool abort = p_runMgr->ProcessEvent(inputEvent);
    
    if (abort) {
      ATH_MSG_WARNING("Event was aborted !! ");
      //ATH_MSG_WARNING("Simulation will now go on to the next event ");
      //ATH_MSG_WARNING("setFilterPassed is now False");
      //setFilterPassed(false);
      return 0;
    }
  }
  else {
    ATH_MSG_ERROR("ISF Event conversion failed ");
    return 0;
  }

  // const DataHandle <TrackRecordCollection> tracks;

  // StatusCode sc = m_storeGate->retrieve(tracks,m_trackCollName);

  // if (sc.isFailure()) {
  //   ATH_MSG_WARNING(" Cannot retrieve TrackRecordCollection " << m_trackCollName);
  // }

  // not implemented yet... need to get particle stack from Geant4 and convert to ISFParticle
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
G4Event* iGeant4::G4TransportTool::ISF_to_G4Event(const ISF::ConstISFParticleVector& particles) const
{

  G4Event * g4evt=new G4Event();

  ISF::ConstISFParticleVector::const_iterator partIt    = particles.begin();
  ISF::ConstISFParticleVector::const_iterator partItEnd = particles.end();
    
  int n_pp=0;
  for ( ; partIt != partItEnd; partIt++) {

    const ISF::ISFParticle& isp = (**partIt);
    
    addPrimaryVertex(g4evt,isp);
    
    n_pp++;
  }

  EventInformation *eventInfo=new EventInformation();
  eventInfo->SetNrOfPrimaryParticles(n_pp);
  eventInfo->SetNrOfPrimaryVertices(n_pp); // special case for ISF batches of particles
  eventInfo->SetHepMCEvent(genEvent());
  g4evt->SetUserInformation(eventInfo);
  
  return g4evt;
}

//________________________________________________________________________
G4Event* iGeant4::G4TransportTool::ISF_to_G4Event(const ISF::ISFParticle& isp) const
{
  
  G4Event * g4evt=new G4Event();
  
  addPrimaryVertex(g4evt,isp);

  EventInformation *eventInfo=new EventInformation();
  eventInfo->SetNrOfPrimaryParticles(1);
  eventInfo->SetNrOfPrimaryVertices(1); // special case for ISF batches of particles
  eventInfo->SetHepMCEvent(genEvent());
  g4evt->SetUserInformation(eventInfo);

  return g4evt;

}

//________________________________________________________________________
void iGeant4::G4TransportTool::addPrimaryVertex(G4Event* g4evt, const ISF::ISFParticle& isp) const
{
  /*
    see conversion from PrimaryParticleInformation to TrackInformation in
    http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Simulation/G4Atlas/G4AtlasAlg/src/AthenaStackingAction.cxx#0044

    need to check with
    http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Simulation/G4Atlas/G4AtlasAlg/src/TruthHepMCEventConverter.cxx#0151

    that we don't miss something
  */

  G4ParticleTable * ptable = G4ParticleTable::GetParticleTable();
  const G4ParticleDefinition* particle_definition = 0;
  if (isp.pdgCode()==999){
    particle_definition = G4Geantino::GeantinoDefinition();
  }
  else {
    particle_definition = ptable->FindParticle(isp.pdgCode());
  }

  // ptable->DumpTable();

  if(particle_definition==0) {
    ATH_MSG_ERROR("ISF_to_G4Event particle conversion failed. ISF_Particle PDG code = " << isp.pdgCode() <<
                  "\n This usually indicates a problem with the evgen step.\n" <<
                  "Please report this to the Generators group, mentioning the release and generator used for evgen and the PDG code above." );
    return;
  }

  // create a new vertex
  G4PrimaryVertex* vertex =
    new G4PrimaryVertex(isp.position().x(),isp.position().y(),isp.position().z(),isp.timeStamp());

  
  // create new primaries and set them to the vertex
  //  G4double mass =  particle_definition->GetPDGMass();
  G4double px = isp.momentum().x();
  G4double py = isp.momentum().y();
  G4double pz = isp.momentum().z();

  G4PrimaryParticle* particle =
    new G4PrimaryParticle(particle_definition,px,py,pz);

  // UserInformation
  PrimaryParticleInformation* ppi=new PrimaryParticleInformation(0,&isp);

  ISF::ITruthBinding *truth = isp.truthBinding();
  // if truth binding exists -> try to retrieve a HepMC::GenParticle from it

  if ( truth) {
    // GenParticleTruth type gives a HepMC::GenParticle as truthParticle()
    const ISF::HepMC_TruthBinding *mctruth = dynamic_cast<const ISF::HepMC_TruthBinding* >(truth);
    if (mctruth) {
      HepMC::GenParticle* genpart= &(mctruth->truthParticle());

      ppi->SetParticle(genpart);
      ppi->SetRegenerationNr(0); // // *AS* this may not be true if this is not a real primary particle
    }
  }

  particle->SetUserInformation(ppi);

  vertex->SetPrimary( particle );

  g4evt->AddPrimaryVertex( vertex );

  return;
}

//________________________________________________________________________
HepMC::GenEvent* iGeant4::G4TransportTool::genEvent() const
{

  // collect last GenEvent from McEventCollection
  
  HepMC::GenEvent* mcEvent(0);

  McEventCollection* mcEventCollection; 

  // retrieve McEventCollection from storegate
  if (m_storeGate->contains<McEventCollection>(m_mcEventCollectionName)) {
    StatusCode status = m_storeGate->retrieve( mcEventCollection, m_mcEventCollectionName);
    if (status.isFailure())
      ATH_MSG_WARNING( "Unable to retrieve McEventCollection with name=" << m_mcEventCollectionName
		       << ". Will create new collection.");
    else {
      ATH_MSG_DEBUG( "Sucessfully retrieved McEventCollection with name=" << m_mcEventCollectionName);
      
      mcEvent = mcEventCollection->back();
    }
  }

  return mcEvent;

}
