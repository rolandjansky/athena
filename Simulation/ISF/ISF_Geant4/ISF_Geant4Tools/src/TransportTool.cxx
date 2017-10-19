/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TransportTool.h"

//package includes
#include "ISF_Geant4Tools/G4AtlasRunManager.h"

// Barcode classes
#include "BarcodeInterfaces/IBarcodeSvc.h"

// ISF classes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleVector.h"

// Athena classes
#include "GeneratorObjects/McEventCollection.h"

#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/EventInformation.h"

// HepMC classes
#include "HepMC/GenParticle.h"

// Geant4 classes
#include "G4UImanager.hh"
#include "G4LorentzVector.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Trajectory.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4ParticleTable.hh"
#include "G4TransportationManager.hh"
#include "G4UImanager.hh"
#include "G4ScoringManager.hh"

//________________________________________________________________________
iGeant4::G4TransportTool::G4TransportTool(const std::string& t,
                                          const std::string& n,
                                          const IInterface*  p )
  : AthAlgTool(t,n,p)
  , m_pRunMgr(nullptr)
  , m_useMT(false)
  , m_userActionSvc("",n)
  , m_rndmGenSvc("AtDSFMTGenSvc",n)
  , m_barcodeSvc("",n)
  , m_barcodeGenerationIncrement(Barcode::fUndefinedBarcode)
  , m_g4RunManagerHelper("iGeant4::G4RunManagerHelper/G4RunManagerHelper")
  , m_physListTool("PhysicsListToolBase")
  , m_mcEventCollectionName("TruthEvent")
  , m_quasiStableParticlesIncluded(false)
  , m_worldSolid(nullptr)
{

  declareInterface<ITransportTool>(this);

  // the barcode service (used to compute Vertex Barco des)
  declareProperty("BarcodeSvc",                 m_barcodeSvc           );

  declareProperty("McEventCollection",          m_mcEventCollectionName);

  declareProperty( "Dll",                       m_libList ="");
  declareProperty( "Physics",                   m_physList="");
  declareProperty( "FieldMap",                  m_fieldMap="");
  declareProperty( "RandomGenerator",           m_rndmGen ="");
  declareProperty( "RandomNumberService",       m_rndmGenSvc);

  //declareProperty( "KillAllNeutrinos",          m_KillAllNeutrinos=true);
  //declareProperty( "KillLowEPhotons",           m_KillLowEPhotons=-1.);

  declareProperty( "ReleaseGeoModel",           m_releaseGeoModel=true);
  declareProperty( "RecordFlux",                m_recordFlux=false);

  declareProperty( "PhysicsListTool", m_physListTool);
  declareProperty( "G4RunManagerHelper", m_g4RunManagerHelper);
  declareProperty( "QuasiStableParticlesIncluded", m_quasiStableParticlesIncluded);
  declareProperty( "UserActionSvc", m_userActionSvc);
  // Multi-threading specific settings
  declareProperty("MultiThreading", m_useMT=false);

  // Commands to send to the G4UI
  declareProperty("G4Commands", m_g4commands);

}

//________________________________________________________________________
iGeant4::G4TransportTool::~G4TransportTool()
{}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::initialize()
{
  ATH_MSG_VERBOSE("initialize");

  // get G4AtlasRunManager
  ATH_MSG_DEBUG("initialize G4AtlasRunManager");

  if (m_g4RunManagerHelper.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_g4RunManagerHelper);
  else {
    ATH_MSG_FATAL("Could not get "<<m_g4RunManagerHelper);
  }

  //m_pRunMgr = G4AtlasRunManager::GetG4AtlasRunManager();    // clashes with use of G4HadIntProcessor
  m_pRunMgr = m_g4RunManagerHelper ? m_g4RunManagerHelper->g4RunManager() : 0;

  if(m_physListTool.retrieve().isFailure())
    {
      ATH_MSG_FATAL("Could not get PhysicsListToolBase");
    }
  m_physListTool->SetPhysicsList();

  // retrieve BarcodeSvc
  if ( m_barcodeSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Unable to retrieve BarcodeService. Abort.");
    return StatusCode::FAILURE;
  }
  m_barcodeGenerationIncrement = m_barcodeSvc->particleGenerationIncrement();
  if (m_barcodeGenerationIncrement == Barcode::fUndefinedBarcode) {
    ATH_MSG_FATAL( "'Barcode::fUndefinedBarcode' returned as 'BarcodeGenerationIncrement' by BarcodeService. Abort." );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_userActionSvc.retrieve() );
  m_pRunMgr->SetUserActionSvc( m_userActionSvc.typeAndName() );

  if(m_useMT) {
    // Retrieve the python service to trigger its initialization. This is done
    // here just to make sure things are initialized in the proper order.
    // Hopefully we can drop this at some point.
    ServiceHandle<IService> pyG4Svc("PyAthena::Svc/PyG4AtlasSvc", name());
    ATH_CHECK( pyG4Svc.retrieve() );
  }

  if (m_recordFlux) G4ScoringManager::GetScoringManager();

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

  m_pRunMgr->SetReleaseGeo( m_releaseGeoModel );
  m_pRunMgr->SetRecordFlux( m_recordFlux );

  // *AS* TEST:
  // *AS* m_pRunMgr->Initialize();
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

  // Send UI commands
  for (auto g4command : m_g4commands){
    int the_return = ui->ApplyCommand(g4command);
    ATH_MSG_INFO("Returned " << the_return << " from G4 Command: " << g4command);
  }

  ATH_MSG_DEBUG("initalize");
  /*
    if (m_particleBroker.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_particleBroker);
    else {
    ATH_MSG_FATAL("Could not get "<<m_particleBroker);
    return StatusCode::FAILURE;
    }
    //m_pRunMgr->setParticleBroker(&m_particleBroker);

    if (m_particleHelper.retrieve().isSuccess())
    ATH_MSG_DEBUG("retrieved "<<m_particleHelper);
    else {
    ATH_MSG_FATAL("Could not get "<<m_particleHelper);
    return StatusCode::FAILURE;
    }
    //m_pRunMgr->setParticleHelper(&m_particleHelper);
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

  m_pRunMgr->RunTermination();

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::process(const ISF::ISFParticle& isp)
{
  ATH_MSG_VERBOSE("++++++++++++  ISF G4 G4TransportTool execute  ++++++++++++");

  ATH_MSG_DEBUG("Calling ISF_Geant4 ProcessEvent");

  G4Event* inputEvent=ISF_to_G4Event(isp);
  if (inputEvent) {

    bool abort = m_pRunMgr->ProcessEvent(inputEvent);

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

  // StatusCode sc = evtStore()->retrieve(tracks,m_trackCollName);

  // if (sc.isFailure()) {
  //   ATH_MSG_WARNING(" Cannot retrieve TrackRecordCollection " << m_trackCollName);
  // }

  // not implemented yet... need to get particle stack from Geant4 and convert to ISFParticle
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::processVector(const ISF::ConstISFParticleVector& ispVector)
{
  ATH_MSG_DEBUG("processing vector of "<<ispVector.size()<<" particles");

  ATH_MSG_VERBOSE("++++++++++++  ISF G4 G4TransportTool execute  ++++++++++++");

  ATH_MSG_DEBUG("Calling ISF_Geant4 ProcessEvent");

  G4Event* inputEvent = ISF_to_G4Event(ispVector);
  if (inputEvent) {
    bool abort = m_pRunMgr->ProcessEvent(inputEvent);

    if (abort) {
      ATH_MSG_WARNING("Event was aborted !! ");
      //ATH_MSG_WARNING("Simulation will now go on to the next event ");
      //ATH_MSG_WARNING("setFilterPassed is now False");
      //setFilterPassed(false);
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_ERROR("ISF Event conversion failed ");
    return StatusCode::FAILURE;
  }

  // const DataHandle <TrackRecordCollection> tracks;

  // StatusCode sc = evtStore()->retrieve(tracks,m_trackCollName);

  // if (sc.isFailure()) {
  //   ATH_MSG_WARNING(" Cannot retrieve TrackRecordCollection " << m_trackCollName);
  // }

  // not implemented yet... need to get particle stack from Geant4 and convert to ISFParticle
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
G4Event* iGeant4::G4TransportTool::ISF_to_G4Event(const ISF::ConstISFParticleVector& ispVector)
{

  G4Event * g4evt = new G4Event();

  int n_pp=0;
  for ( const ISF::ISFParticle *ispPtr: ispVector ) {

    const ISF::ISFParticle &isp = *ispPtr;

    if ( !isInsideG4WorldVolume(isp) ) {
        ATH_MSG_WARNING("Unable to convert ISFParticle to G4PrimaryParticle!");
        ATH_MSG_WARNING(" ISFParticle outside Geant4 world volume: " << isp );
        continue;
    }

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
G4Event* iGeant4::G4TransportTool::ISF_to_G4Event(const ISF::ISFParticle& isp)
{

  // wrap the given ISFParticle into a STL vector of ISFParticles with length 1
  // (minimizing code duplication)
  ISF::ConstISFParticleVector ispVector(1, &isp);
  G4Event *g4evt = ISF_to_G4Event( ispVector );

  return g4evt;
}

//________________________________________________________________________
G4PrimaryParticle* iGeant4::G4TransportTool::getPrimaryParticle(const HepMC::GenParticle& gp) const
{

  G4ParticleTable * ptable = G4ParticleTable::GetParticleTable();
  const G4ParticleDefinition* particle_definition = 0;
  if (gp.pdg_id()==998){
    particle_definition = G4ChargedGeantino::Definition();
  }
  else if (gp.pdg_id()==999){
    particle_definition = G4Geantino::GeantinoDefinition();
  }
  else {
    particle_definition = ptable->FindParticle(gp.pdg_id());
  }

  if(particle_definition==0) {
    ATH_MSG_ERROR("ISF_to_G4Event particle conversion failed. ISF_Particle PDG code = " << gp.pdg_id() <<
                  "\n This usually indicates a problem with the evgen step.\n" <<
                  "Please report this to the Generators group, mentioning the release and generator used for evgen and the PDG code above." );
    return 0;
  }

  // create new primaries and set them to the vertex
  //  G4double mass =  particle_definition->GetPDGMass();
  auto &gpMomentum = gp.momentum();
  G4double px = gpMomentum.x();
  G4double py = gpMomentum.y();
  G4double pz = gpMomentum.z();

  G4PrimaryParticle* particle =
    new G4PrimaryParticle(particle_definition,px,py,pz);

  if (gp.end_vertex()){
    // Add all necessary daughter particles
    for (HepMC::GenVertex::particles_out_const_iterator iter=gp.end_vertex()->particles_out_const_begin();
         iter!=gp.end_vertex()->particles_out_const_end(); ++iter){
      if (! (*iter) ) continue;
      G4PrimaryParticle * daught = getPrimaryParticle( **iter );
      particle->SetDaughter( daught );
    }
    // Set the lifetime appropriately - this is slow but rigorous, and we
    //  don't want to end up with something like vertex time that we have
    //  to validate for every generator on earth...
    G4LorentzVector lv0 ( gp.production_vertex()->position().x(),
                          gp.production_vertex()->position().y(),
                          gp.production_vertex()->position().z(),
                          gp.production_vertex()->position().t() );
    G4LorentzVector lv1 ( gp.end_vertex()->position().x(),
                          gp.end_vertex()->position().y(),
                          gp.end_vertex()->position().z(),
                          gp.end_vertex()->position().t() );
    particle->SetProperTime( (lv1-lv0).mag()/CLHEP::c_light );
  }

  // Set the user information for this primary to point to the HepMcParticleLink...
  PrimaryParticleInformation* ppi = new PrimaryParticleInformation(&gp);
  particle->SetUserInformation(ppi);
  ATH_MSG_VERBOSE("ZLM making primary down the line with " << ppi->GetParticleBarcode());

  return particle;
}


//________________________________________________________________________
G4PrimaryParticle* iGeant4::G4TransportTool::getPrimaryParticle(const ISF::ISFParticle& isp) const
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
  if (isp.pdgCode()==998){
    particle_definition = G4ChargedGeantino::Definition();
  }
  else if (isp.pdgCode()==999){
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
    return 0;
  }

  // create new primaries and set them to the vertex
  //  G4double mass =  particle_definition->GetPDGMass();
  auto &ispMomentum = isp.momentum();
  G4double px = ispMomentum.x();
  G4double py = ispMomentum.y();
  G4double pz = ispMomentum.z();

  G4PrimaryParticle* particle =
    new G4PrimaryParticle(particle_definition,px,py,pz);

  auto truthBinding = isp.getTruthBinding();
  if (!truthBinding) {
      G4ExceptionDescription description;
      description << G4String("getPrimaryParticle: ") + "No ISF::TruthBinding associated with ISParticle (" << isp <<")";
      G4Exception("iGeant4::TransportTool", "NoISFTruthBinding", FatalException, description);
      return nullptr; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }
  HepMC::GenParticle*        genpart = truthBinding->getTruthParticle();
  HepMC::GenParticle* primaryGenpart = truthBinding->getPrimaryTruthParticle();

  // UserInformation
  PrimaryParticleInformation* ppi = new PrimaryParticleInformation(primaryGenpart,&isp);

  Barcode::ParticleBarcode barcode = isp.barcode();
  int regenerationNr = (barcode - barcode%m_barcodeGenerationIncrement)/m_barcodeGenerationIncrement;
  ppi->SetRegenerationNr(regenerationNr);

  // if truth binding exists -> try to retrieve the HepMC::GenParticle from it
  if ( genpart ) {
    if (genpart->end_vertex()){
      if(m_quasiStableParticlesIncluded) {
        ATH_MSG_VERBOSE( "Detected primary particle with end vertex." );
        ATH_MSG_VERBOSE( "Will add the primary particle set on." );
        ATH_MSG_VERBOSE( "ISF Particle: " << isp );
        ATH_MSG_VERBOSE( "Primary Particle: " << *genpart );
        ATH_MSG_VERBOSE("Number of daughters of "<<genpart->barcode()<<": " << genpart->end_vertex()->particles_out_size() );
      }
      else {
        ATH_MSG_WARNING( "Detected primary particle with end vertex. This should only be the case if" );
        ATH_MSG_WARNING( "you are running with quasi-stable particle simulation enabled.  This is not" );
        ATH_MSG_WARNING( "yet validated - you'd better know what you're doing.  Will add the primary" );
        ATH_MSG_WARNING( "particle set on." );
        ATH_MSG_WARNING( "ISF Particle: " << isp );
        ATH_MSG_WARNING( "Primary Particle: " << *genpart );
        ATH_MSG_WARNING("Number of daughters of "<<genpart->barcode()<<": " << genpart->end_vertex()->particles_out_size() );
      }
      // Add all necessary daughter particles
      for (HepMC::GenVertex::particles_out_const_iterator iter=genpart->end_vertex()->particles_out_const_begin();
           iter!=genpart->end_vertex()->particles_out_const_end(); ++iter){
        G4PrimaryParticle * daught = getPrimaryParticle( **iter );
        if(m_quasiStableParticlesIncluded) {
          ATH_MSG_VERBOSE ( "Daughter Particle of "<<genpart->barcode()<<": " << **iter );
        }
        else {
          ATH_MSG_WARNING ( "Daughter Particle of "<<genpart->barcode()<<": " << **iter );
        }
        if(nullptr==(*iter)->end_vertex()) {
          if(m_quasiStableParticlesIncluded) {
            ATH_MSG_VERBOSE ( "Number of daughters of "<<(*iter)->barcode()<<": 0 (NULL)." );
          }
          else {
            ATH_MSG_WARNING ( "Number of daughters of "<<(*iter)->barcode()<<": 0 (NULL)." );
          }
        }
        else {
          if(m_quasiStableParticlesIncluded) {
            ATH_MSG_VERBOSE ( "Number of daughters of "<<(*iter)->barcode()<<": " << (*iter)->end_vertex()->particles_out_size() );
          }
          else {
            ATH_MSG_WARNING ( "Number of daughters of "<<(*iter)->barcode()<<": " << (*iter)->end_vertex()->particles_out_size() );
          }
        }
        particle->SetDaughter( daught );
      }
      // Set the lifetime appropriately - this is slow but rigorous, and we
      //  don't want to end up with something like vertex time that we have
      //  to validate for every generator on earth...
      const auto& prodVtx = genpart->production_vertex()->position();
      const auto& endVtx = genpart->end_vertex()->position();
      const G4LorentzVector lv0( prodVtx.x(), prodVtx.y(), prodVtx.z(), prodVtx.t() );
      const G4LorentzVector lv1( endVtx.x(), endVtx.y(), endVtx.z(), endVtx.t() );
      particle->SetProperTime( (lv1-lv0).mag()/CLHEP::c_light );
    } // particle had an end vertex
  } // Truth was detected

  particle->SetUserInformation(ppi);

  return particle;
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

  G4PrimaryParticle * particle = getPrimaryParticle( isp );
  if (!particle) return; // Already printed a warning

  // create a new vertex
  G4PrimaryVertex* vertex =
    new G4PrimaryVertex(isp.position().x(),isp.position().y(),isp.position().z(),isp.timeStamp());

  vertex->SetPrimary( particle );

  g4evt->AddPrimaryVertex( vertex );

}

//________________________________________________________________________
HepMC::GenEvent* iGeant4::G4TransportTool::genEvent() const
{

  // collect last GenEvent from McEventCollection

  HepMC::GenEvent* mcEvent(0);

  McEventCollection* mcEventCollection;

  // retrieve McEventCollection from storegate
  if (evtStore()->contains<McEventCollection>(m_mcEventCollectionName)) {
    StatusCode status = evtStore()->retrieve( mcEventCollection, m_mcEventCollectionName);
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

//________________________________________________________________________
bool iGeant4::G4TransportTool::isInsideG4WorldVolume(const ISF::ISFParticle& isp) {

  // retrieve world solid (volume)
  if (!m_worldSolid) {
    // NB: assuming that the pointers are all valid
    // (simulation is really sick otherwise)
    m_worldSolid = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume()->GetSolid();
  }

  const Amg::Vector3D &pos = isp.position();
  const G4ThreeVector g4Pos( pos.x(), pos.y(), pos.z() );
  EInside insideStatus     = m_worldSolid->Inside( g4Pos );

  bool insideWorld = insideStatus != kOutside;
  return insideWorld;
}
