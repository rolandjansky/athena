/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4HadIntProcessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Class Header
#include "ISF_FatrasToolsG4/G4HadIntProcessor.h"

// Fatras
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"

// ISF 
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleVector.h"
#include "ISF_Event/ParticleClipboard.h"
#include "ISF_Event/ParticleUserInformation.h"
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Geant4Tools/IG4RunManagerHelper.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFTruthIncident.h"

#include <G4WHadronElasticProcess.hh>
#include <G4HadronElasticProcess.hh>

// Boost
#include <boost/math/special_functions/round.hpp>

// Trk inlcude
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
#include "TrkSurfaces/Surface.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"

// Geant4
#include "G4Material.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BIC.hh"
#include "FTFP_BERT.hh"
#include "G4UImanager.hh"
#include "G4NistManager.hh"
#include "G4VEnergyLossProcess.hh"
#include <G4ProductionCutsTable.hh>
#include <G4MaterialCutsCouple.hh>

#include <G4NistManager.hh>
#include "G4WHadronElasticProcess.hh"
#include "globals.hh"
#include "G4CrossSectionDataStore.hh"
#include "G4HadronElasticDataSet.hh"
#include "G4VQCrossSection.hh"

#include "G4QCHIPSWorld.hh"
#include "G4Element.hh"
#include "G4ElementVector.hh"
#include "G4IsotopeVector.hh"
#include "G4Neutron.hh"
#include "G4ProductionCutsTable.hh"


// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Random/RandFlat.h"


//FADS classes
#include "FadsActions/FadsRunAction.h"
#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsSteppingAction.h"
#include "FadsActions/FadsTrackingAction.h"
#include "FadsActions/FadsStackingAction.h"
#include "FadsPhysics/PhysicsListCatalog.h"

// using namespace std;

// STD
#include <math.h>

// ROOT
#include "TTree.h"

// statics doubles
// sqrt(2)
double iFatras::G4HadIntProcessor::s_projectionFactor = sqrt(2.);

// constructor
iFatras::G4HadIntProcessor::G4HadIntProcessor(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_rndGenSvc("AtDSFMTGenSvc", n),
  m_g4RunManagerHelper("iGeant4::G4RunManagerHelper/G4RunManagerHelper"),
  m_doElastic(false),
  m_hadIntProbScale(1.0),
  m_minEnergy(50.0),
  m_cloneParameters(false),
  m_parametricScattering(false),
  m_g4runManager(0),
  m_g4physicsList(0),
  m_g4detector(0),
  m_g4dynPar(0),
  m_g4zeroPos(0),
  m_g4step(0),
  m_g4stepPoint(0),
  m_trackingGeometry(0),
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc", n),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_particleBroker("ISF_ParticleParticleBroker", n),
  m_truthRecordSvc("ISF_TruthRecordSvc", n), 
  m_processCode(121),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd"),
  m_validationMode(false),
  m_validationTool(""),
  m_validationTreeName("FatrasMaterialEffects"),
  m_validationTreeDescription("Validation output from the McMaterialEffectsUpdator"),
  m_validationTreeFolder("/val/FatrasSimulationMaterial"),
  m_validationTree(0),
  m_layerIndex(0),
  m_tInX0(0.),
  m_thetaMSproj(0.),
  m_thetaMSphi(0.),
  m_thetaMStheta(0.),
  m_deltaP(0.),
  m_deltaPsigma(0.),
  m_bremValidation(false),
  m_bremValidationTreeName("FatrasBremPhotons"),
  m_bremValidationTreeDescription("Validation output from the McMaterialEffectsUpdator"),
  m_bremValidationTreeFolder("/val/FatrasBremPhotons"),
  m_bremValidationTree(0),
  m_bremPointX(0.),
  m_bremPointY(0.),
  m_bremPointR(0.),
  m_bremPointZ(0.),
  m_bremMotherEnergy(0.),
  m_bremPhotonEnergy(0.),
  m_bremPhotonAngle(0.),
  m_edValidation(false),
  m_edValidationTreeName("FatrasEnergyInCaloDeposit"),     
  m_edValidationTreeDescription("Validation output from the McMaterialEffectUpdator"),
  m_edValidationTreeFolder("/val/FatrasEnergyInCaloDeposit"),   
  m_edValidationTree(0),                   
  m_edLayerIntersectX(0.),         
  m_edLayerIntersectY(0.),         
  m_edLayerIntersectZ(0.),         
  m_edLayerIntersectR(0.),         
  m_edLayerEnergyDeposit(0.),     
  m_edLayerSample(0)
{
  declareInterface<iFatras::IHadronicInteractionProcessor>(this);
  // steering
  declareProperty("MinEnergyCut"                     , m_minEnergy                                                       );     
  declareProperty("DoElasticInteractions"            , m_doElastic                                                       );
  declareProperty("HadronicInteractionScaleFactor"   , m_hadIntProbScale=1.0  , "Scale probability of HadrInteractions"  );     
  // TrackingGeometry Service
  declareProperty("TrackingGeometrySvc"              , m_trackingGeometrySvc                                             );
  // ISF Services and Tools
  declareProperty("ParticleBroker"                   , m_particleBroker       , "ISF Particle Broker"                    );
  declareProperty("TruthRecordSvc"                   , m_truthRecordSvc       , "ISF Particle Truth Svc"                 );
  // random number generator
  declareProperty("RandomNumberService"                 , m_rndGenSvc          , "Random number generator");
  declareProperty("RandomStreamName"                    , m_randomEngineName   , "Name of the random number stream");
  declareProperty("ValidationMode"                      , m_validationMode);
  declareProperty("PhysicsValidationTool"               , m_validationTool);
  declareProperty("G4RunManagerHelper"                  , m_g4RunManagerHelper);
}



// destructor
iFatras::G4HadIntProcessor::~G4HadIntProcessor()
{}

// Athena standard methods
// initialize
StatusCode iFatras::G4HadIntProcessor::initialize()
{
  // ISF Services
  if (m_particleBroker.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve ParticleBroker: " << m_particleBroker );
    return StatusCode::FAILURE;
  }
  if (m_truthRecordSvc.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve TruthRecordSvc: " << m_truthRecordSvc );
    return StatusCode::FAILURE;
  }

  if (m_validationMode){
    
    // retrieve the physics validation tool
    if (m_validationTool.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_validationTool );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_VERBOSE( "Successfully retrieved " << m_validationTool );
  }

  // get the tracking geometry for layer lookup     
  // get the TrackingGeometrySvc
  if (m_trackingGeometrySvc.retrieve().isSuccess()){
    ATH_MSG_INFO( "Successfully retrieved " << m_trackingGeometrySvc );
    m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
  } else {
    ATH_MSG_WARNING( "Couldn't retrieve " << m_trackingGeometrySvc << ". " );
    ATH_MSG_WARNING( " -> Trying to retrieve default '" << m_trackingGeometryName << "' from DetectorStore." );
  }

  // get the random generator serice
  if (m_rndGenSvc.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_rndGenSvc );
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_VERBOSE( "Successfully retrieved " << m_rndGenSvc );

  //Get own engine with own seeds:
  m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE;
  }

  // all good
  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


// finalize
StatusCode iFatras::G4HadIntProcessor::finalize()
{

  // clean up locally stored Geant4 instances
  delete m_g4dynPar;
  delete m_g4zeroPos;
  //delete m_g4step; will be deleted via m_g4stepPoint
  delete m_g4stepPoint;

  ATH_MSG_INFO( " ---------- Statistics output -------------------------- " );
  //ATH_MSG_INFO( "                     Minimum energy cut for brem photons : " <<   m_minimumBremPhotonMomentum  );
  //ATH_MSG_INFO( "                     Brem photons (above cut, recorded)  : " <<   m_recordedBremPhotons        );

  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}


std::map<int,G4VProcess*>::iterator iFatras::G4HadIntProcessor::initProcessPDG(int pdg) const
{
  ATH_MSG_VERBOSE( "  [ g4sim ] Registering Geant4 processes for particles with pdg code " << pdg );

  // std::map::insert return value
  std::pair< std::map<int,G4VProcess*>::iterator,bool > ret;


  G4ParticleDefinition *parDef = G4ParticleTable::GetParticleTable()->FindParticle( pdg);

  // check if everythin is set up properly
  if ( !parDef || !parDef->GetProcessManager() ) {
    ATH_MSG_WARNING( "  [ ---- ] Unable to register particle type with PDG code " << pdg );    
    return m_g4HadrProcesses.end();

  }

  // get Geant4 processes
  G4ProcessVector *physIntVector = parDef->GetProcessManager()->GetPostStepProcessVector(typeGPIL);
  //G4ProcessVector *physIntVector = parDef->GetProcessManager()->GetProcessVector(idxAll);
  //  G4ProcessVector *physIntVector = parDef->GetProcessManager()->GetPostStepProcessVector(typeDoIt);
  if ( !physIntVector) {
    ATH_MSG_WARNING( "  [ ---- ] No Geant4 processes registered for PDG code " << pdg << " particles" );
    return m_g4HadrProcesses.end();
  }

  // loop over all processes of current particle type
  for( G4int np=0; np < physIntVector->size(); np++) {
    // get current process
    G4VProcess* curProc = (*physIntVector)(np);
    // NULL means the process is inactivated by a user on fly.
    if ( curProc == 0 ) continue;

    ATH_MSG_VERBOSE( "  [ g4sim ] Found Geant4 process " << curProc->GetProcessName());

    G4HadronInelasticProcess *hadProc = dynamic_cast<G4HadronInelasticProcess*>( curProc);
    G4WHadronElasticProcess *hadProc1 = dynamic_cast<G4WHadronElasticProcess*>( curProc);
    ATH_MSG_DEBUG( "  hadproc 0,1 " << hadProc << ", " << hadProc1);
    if ( !hadProc && !hadProc1) {
      ATH_MSG_VERBOSE( "  [ g4sim ] Current process not an inelastic or elastic  hadronic process -> process not registered" );
      continue;
    }

    if(!hadProc1){    
      ret = m_g4HadrProcesses.insert( std::pair<int,G4VProcess*>( pdg, hadProc) );
      ATH_MSG_DEBUG( "  [ g4sim ] Registered Geant4 hadronic interaction processes for particles with pdg code " << pdg );
    }
    if(hadProc1){
      if(m_doElastic){   
        ret = m_g4HadrProcesses_Elastic.insert( std::pair<int,G4VProcess*>( pdg, hadProc1) );

        G4ProcessType pType = curProc->GetProcessType();
        ATH_MSG_DEBUG( "  [ g4sim ] Registered Geant4 ELASTIC hadronic interaction processes for particles with pdg code " << pdg << "and process " <<  pType);
      }
    } 


  } // process loop

  // return iterator to insterted G4VProcess
  return ret.first;
}


StatusCode iFatras::G4HadIntProcessor::updateTrackingGeometry() const
{

  // -------------------- public TrackingGeometry (from DetectorStore) ----------------------------
  // get the DetectorStore

  StatusCode s = detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName);
  if (s.isFailure())
  {
    ATH_MSG_FATAL( "Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore." );
    ATH_MSG_FATAL( "  - probably the chosen layout is not supported / no cool tag exists. "                     );
    return s;
  }

  return s;
}


bool iFatras::G4HadIntProcessor::hadronicInteraction(const Trk::TrackParameters& parm, double, double,
    const Trk::MaterialProperties& mprop, double pathCorrection,
    Trk::ParticleHypothesis particle) const
{
  // do not treat geantinos
  if (particle==Trk::geantino) return false;

  bool processSecondaries = true;
  // need to identify the layer first
  if (!m_trackingGeometry && updateTrackingGeometry().isFailure()){
    ATH_MSG_WARNING( "[ ---- ] Could not retrieve TrackingGeometry. Secondaries will not be processed." );
    processSecondaries = false;
  }

  // the layer material
  const Trk::MaterialProperties* ematprop  = dynamic_cast<const Trk::MaterialProperties*>(&(mprop));
  if (!ematprop) {
    ATH_MSG_WARNING("[ ---- ] Unable to cast MaterialProperties->MaterialProperties -> no material interactions for this particle");
    return false;
  }
  ATH_MSG_DEBUG("[ g4sim ] Material t/X0, t/L0 : " << pathCorrection*ematprop->thicknessInX0() << ", " << pathCorrection*ematprop->thicknessInL0() );

  // compute a random interaction- and radiation length,
  // according to the mean values
  double meanIntLength   = pathCorrection*ematprop->l0();
  double rndIntLength    = CLHEP::RandExponential::shoot(m_randomEngine, meanIntLength);
  double thickness       = pathCorrection*ematprop->thickness();

  // test for hadronic interactions
  if ( rndIntLength < thickness ) {
    ATH_MSG_DEBUG(" [ g4sim ] computing hadronic interaction on current particle in current material layer");
    return doHadronicInteraction( 0., parm, &(ematprop->material()), particle, processSecondaries);
  }

  // hadronic interaction did not happen
  return false;
}


bool iFatras::G4HadIntProcessor::initG4RunManager() const {

  // Get the G4RunManagerHelper ( no initialization of G4RunManager done )
  if (m_g4RunManagerHelper.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Could not retrieve " << m_g4RunManagerHelper );
    return false;
  }

  m_g4runManager = m_g4RunManagerHelper->fastG4RunManager();
  m_g4runManager->SetVerboseLevel(10);

  initProcessPDG( 211);
  initProcessPDG(-211);
  initProcessPDG(2212);
  initProcessPDG(-2212);
  initProcessPDG(2112);
  initProcessPDG( 130);
  initProcessPDG( 321);
  initProcessPDG( 111);
  initProcessPDG(-321);

  // set up locally stored Geant4 instances
  m_g4dynPar          = new G4DynamicParticle();
  m_g4zeroPos         = new G4ThreeVector(0, 0, 0);
  m_g4step            = new G4Step();
  m_g4stepPoint       = new G4StepPoint();
  m_g4step->SetPreStepPoint( m_g4stepPoint);

  return true;
}

ISF::ISFParticleVector iFatras::G4HadIntProcessor::getHadState(const ISF::ISFParticle* parent,
					 double time, const Trk::TrackParameters& parm,
					 const Trk::Material *ematprop) const
{
  ISF::ISFParticleVector chDef(0); 

  int pdg = parent->pdgCode();

  // ions not handled at the moment
  if ( pdg>10000 ) return chDef;

  // initialize G4RunManager if not done already
  if (!m_g4runManager) {
    ATH_MSG_DEBUG("[ g4sim ] Initializing G4RunManager");
    bool g4mgr = initG4RunManager();
    if (!g4mgr) return chDef;
  }

  // find corresponding hadronic interaction process ----------------------------------------------
  //
  std::map<int, G4VProcess*>::iterator processIter_inelast = m_g4HadrProcesses.find(pdg);
  std::map<int, G4VProcess*>::iterator processIter_elast   = m_g4HadrProcesses_Elastic.find(pdg);

  if ( (processIter_inelast==m_g4HadrProcesses.end()) && (processIter_elast==m_g4HadrProcesses_Elastic.end()) ) {
    ATH_MSG_DEBUG ( " [ g4sim ] No hadronic interactions registered for current particle type (pdg=" << pdg << ")" );

    initProcessPDG(pdg);
    //return chDef;
  }
  if ( processIter_inelast==m_g4HadrProcesses.end()) return chDef;

  ATH_MSG_DEBUG ( " [ g4sim ] Found registered hadronic interactions for current particle type (pdg=" << pdg << ")" );

  // setup up G4Track ------------------------------------------------------------------------------
  //
  const G4ParticleDefinition *g4parDef = G4ParticleTable::GetParticleTable()->FindParticle(pdg);
  if ( g4parDef==0) {
    ATH_MSG_WARNING( "[ ---- ] Unable to find G4ParticleDefinition for particle with PID=" << pdg << " --> skipping hadronic interactions" );
    return chDef;
  }
   m_g4dynPar->SetDefinition( g4parDef);
  const G4ThreeVector mom( parm.momentum().x(), parm.momentum().y(), parm.momentum().z() );
  m_g4dynPar->SetMomentum( mom);
  G4Track* g4track = new G4Track( m_g4dynPar, 0 /* time */, *m_g4zeroPos);
  G4TouchableHandle g4touchable(new G4TouchableHistory());
  g4track->SetTouchableHandle( g4touchable);

  // setup up G4Material ---------------------------------------------------------------------------
  // if not available on input, take Al
  if (!ematprop) ematprop=new Trk::Material(88.93,388.62,26.98,13.,0.0027);
  //
  // make Z being an integeter
  int  iZ = boost::math::iround(ematprop->averageZ());
  
  // Uses physics table and coupling cuts to make material
  G4NistManager *G4Nist = G4NistManager::Instance(); 
  G4Element     *g4elem = G4Nist->G4NistManager::FindOrBuildElement(iZ,false);
  // debug output:
  //G4Nist->G4NistManager::PrintElement(iZ);

  // setup the G4Material
  G4Material *g4mat  = new G4Material( "LayerMaterial",
                                       ematprop->rho*(CLHEP::g/CLHEP::mm3) /* density */,
                                       1 /* num elements */,
                                       kStateSolid 
                                       /* TODO: temp?, pressure? */);
  // sanity checks
  if(g4mat == 0 || g4elem ==0) {
    ATH_MSG_WARNING ( " Unable to create G4Material or G4Element with Z=" << iZ << " --> skipping hadronic interaction");
    return chDef;
  }
  // add the G4Element to our material
  g4mat->AddElement(g4elem, 1. /* fraction */);

  // further G4 initializations (G4Step, G4MaterialCutsCouple, ...)
  m_g4stepPoint->SetMaterial( g4mat);
  G4MaterialCutsCouple *g4matCuts = new G4MaterialCutsCouple( g4mat);
  m_g4stepPoint->SetMaterialCutsCouple( g4matCuts);
  g4matCuts->SetIndex(0);
  G4ProductionCutsTable* pkt = G4ProductionCutsTable::GetProductionCutsTable();
  std::vector<G4double>* vc = const_cast<std::vector<G4double>*>(pkt->GetEnergyCutsVector(3));
  vc->push_back(0.0);

  // preparing G4Step and G4Track
  m_g4step->DeleteSecondaryVector();
  g4track->SetStep( m_g4step);


  // the current process is the inelastic hadr. interaction
  G4VProcess *process = processIter_inelast->second;
 
   //Prepare and build the Physics table for primaries 
   process->PreparePhysicsTable(*g4parDef);
   process->BuildPhysicsTable(*g4parDef);

  // if elastic interactions are enabled and there is a elastic process
  // in the m_g4HadrProcesses_Elastic std::map 
  if( m_doElastic && (processIter_elast!=m_g4HadrProcesses_Elastic.end()) ) {
    double rand   = CLHEP::RandFlat::shoot(m_randomEngine, 0., 1.);

    // use a 50% chance to use either elastic or inelastic processes
    if( rand < 0.5) process = processIter_elast->second;
  }

  ATH_MSG_VERBOSE ( " [ g4sim ] Computing " << process->GetProcessName() << "  " << process->GetProcessSubType() << " process with current particle" );
  ATH_MSG_VERBOSE( "g4track, m_g4step" <<  g4track << " ," << m_g4step );

  // do the G4VProcess (actually a G4HadronicProcess) ------------------------------------
  //
  //process->SetVerboseLevel(10);
  //ATH_MSG_VERBOSE ( "Verbose Level is " << process->GetVerboseLevel() ); 
  
  G4VParticleChange* g4change = process->PostStepDoIt(*g4track, *m_g4step);
  if (!g4change) {
    ATH_MSG_WARNING( " [ ---- ] Geant4 did not return any hadronic interaction information of particle with pdg=" << pdg );
    return chDef;
  }

  // process the secondaries ------------------------------------------------------------------
  unsigned int numSecondaries = g4change->GetNumberOfSecondaries();
  ATH_MSG_DEBUG( "[ g4sim ] Material update created " <<  numSecondaries << " Geant4 particle (s)." );

  // green light for secondaries 
  if ( numSecondaries ) {

    ISF::ISFParticleVector           children(numSecondaries);
    ISF::ISFParticleVector::iterator childrenIt = children.begin();
    unsigned short                numChildren = 0;
    for ( unsigned int i = 0; i < numSecondaries; i++ ){

      // get Geant4 created particle (G4Track)
      G4Track *trk = g4change->GetSecondary(i);
      const G4DynamicParticle *dynPar = trk->GetDynamicParticle();

      // drop if below energy threshold
      if ( dynPar->GetTotalEnergy() < m_minEnergy)
        continue;

      // get dynamic particle
      const G4ParticleDefinition *parDef = trk->GetParticleDefinition();

      // skip ions
      if (parDef->GetPDGEncoding()>1.e09) continue;

      //Prepare and build the physics table for secondaries
      //process->PreparePhysicsTable(*parDef);
      //process->BuildPhysicsTable(*parDef);

      ATH_MSG_VERBOSE( " [ g4sim ] Adding child particle to particle stack (pdg=" << parDef->GetPDGEncoding() << " E=" << dynPar->GetTotalEnergy() );

      // the parent particle
      // ISF::ISFParticle *parent = const_cast<ISF::ISFParticle*>(ISF::ParticleClipboard::getInstance().getParticle() );
      // child particle vector for TruthIncident
      //  Reserve space for as many paricles as created due to hadr. int.
      //  However, the number of child particles for TruthIncident might be
      //  smaller due to (momentum) cuts
      //  ISF::ISFParticleVector           children(numSecondaries);
      //  ISF::ISFParticleVector::iterator childrenIt = children.begin();
      // unsigned short                numChildren = 0;


      // create the particle to be put into ISF stack
      const G4ThreeVector &momG4 = dynPar->GetMomentum();
      Amg::Vector3D mom( momG4.x(), momG4.y(), momG4.z() );
      ISF::ISFParticle* cParticle = new ISF::ISFParticle( parm.position(),
                                                          mom,
                                                          parDef->GetPDGMass(),
                                                          parDef->GetPDGCharge(),
                                                          parDef->GetPDGEncoding(),
                                                          time, 
                                                          *parent );
      cParticle->setNextGeoID( parent->nextGeoID() );
      cParticle->setNextSimID( parent->nextSimID() );
      // in the validation mode, add process info
      if (m_validationMode) {
	ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
        validInfo->setProcess(process->GetProcessSubType());
        if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
	else validInfo->setGeneration(1);     // assume parent is a primary track
        cParticle->setUserInformation(validInfo);
      }
      *childrenIt = cParticle;
      ++childrenIt; numChildren++;
      // push onto ParticleStack
      // @TODO: TruthRecordSvc
    }


    children.resize(numChildren);
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
				 children,
				 process->GetProcessSubType(),
				 parent->nextGeoID(),
				 ISF::fKillsPrimary );
    m_truthRecordSvc->registerTruthIncident( truth);

    // save info for validation
    if (m_validationMode && m_validationTool) {
      Amg::Vector3D* nMom = 0;
      m_validationTool->saveISFVertexInfo(process->GetProcessSubType(),parm.position(),*parent,parm.momentum(),nMom,children);
      delete nMom;
    }

    // free up memory
    g4change->Clear();

    return children;

  }

  // free up memory
  g4change->Clear();

  return chDef;
}



bool iFatras::G4HadIntProcessor::doHadronicInteraction(double time,const Trk::TrackParameters &parm,
						       const Trk::Material *ematprop,
						       Trk::ParticleHypothesis /*particle*/,  
						       bool  processSecondaries) const
{
  // get parent particle
  // @TODO: replace by Fatras internal bookkeeping
  const ISF::ISFParticle *parent = ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  assert(parent);

  ISF::ISFParticleVector ispVec=getHadState(parent,time, parm, ematprop);

  if (!ispVec.size()) return false;

  // push onto ParticleStack

  if (processSecondaries) {
    for (unsigned int ic=0; ic<ispVec.size(); ic++) m_particleBroker->push(ispVec[ic], parent);
  }

  return true;

}

ISF::ISFParticleVector iFatras::G4HadIntProcessor::doHadIntOnLayer(const ISF::ISFParticle* parent, double time, const Trk::TrackParameters& parm,
				       const Trk::MaterialProperties *ematprop,
								   Trk::ParticleHypothesis /*particle=Trk::pion*/) const 
{
 
  return getHadState(parent,time, parm, &(ematprop->material()));

}
