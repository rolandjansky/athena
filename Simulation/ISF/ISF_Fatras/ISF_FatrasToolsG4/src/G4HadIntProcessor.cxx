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

#include <G4HadronElasticProcess.hh>

// Boost
#include <boost/math/special_functions/round.hpp>

// Trk inlcude
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
#include "globals.hh"
#include "G4CrossSectionDataStore.hh"
#include "G4HadronElasticDataSet.hh"
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

// Generators
#include "GeneratorObjects/HepMcParticleLink.h"


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
  base_class(t,n,p),
  m_rndGenSvc("AtDSFMTGenSvc", n),
  m_g4RunManagerHelper("iGeant4::G4RunManagerHelper/G4RunManagerHelper"),
  m_doElastic(false),
  m_hadIntProbScale(1.0),
  m_minMomentum(50.0),
  m_cloneParameters(false),
  m_parametricScattering(false),
  m_g4runManager(nullptr),
  m_g4zeroPos(nullptr),
  m_g4step(nullptr),
  m_g4stepPoint(nullptr),
  m_particleBroker("ISF_ParticleBrokerSvc", n),
  m_truthRecordSvc("ISF_ValidationTruthService", n), 
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
  // steering
  declareProperty("MomentumCut"                     , m_minMomentum                                                       );     
  declareProperty("DoElasticInteractions"            , m_doElastic                                                       );
  declareProperty("HadronicInteractionScaleFactor"   , m_hadIntProbScale=1.0  , "Scale probability of HadrInteractions"  );     
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

  ATH_MSG_INFO( " ---------- Statistics output -------------------------- " );
  //ATH_MSG_INFO( "                     Minimum energy cut for brem photons : " <<   m_minimumBremPhotonMomentum  );
  //ATH_MSG_INFO( "                     Brem photons (above cut, recorded)  : " <<   m_recordedBremPhotons        );

  // Release G4StepPoint to G4Step owner to prevent double-delete
  m_g4step->SetPreStepPoint(m_g4stepPoint.release());

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
    return m_g4HadrInelasticProcesses.end();

  }

  // get Geant4 processes
  G4ProcessVector *physIntVector = parDef->GetProcessManager()->GetPostStepProcessVector(typeGPIL);
  //G4ProcessVector *physIntVector = parDef->GetProcessManager()->GetProcessVector(idxAll);
  //  G4ProcessVector *physIntVector = parDef->GetProcessManager()->GetPostStepProcessVector(typeDoIt);
  if ( !physIntVector) {
    ATH_MSG_WARNING( "  [ ---- ] No Geant4 processes registered for PDG code " << pdg << " particles" );
    return m_g4HadrInelasticProcesses.end();
  }

  // loop over all processes of current particle type
  for( G4int np=0; np < physIntVector->size(); np++) {
    // get current process
    G4VProcess* curProc = (*physIntVector)(np);
    // NULL means the process is inactivated by a user on fly.
    if ( curProc == 0 ) continue;

    ATH_MSG_VERBOSE( "  [ g4sim ] Found Geant4 process " << curProc->GetProcessName());

    G4HadronInelasticProcess *hadInelastic = dynamic_cast<G4HadronInelasticProcess*>( curProc);
    G4HadronElasticProcess *hadElastic = dynamic_cast<G4HadronElasticProcess*>( curProc);
    ATH_MSG_DEBUG( "  hadronic process inelastic,elastic " << hadInelastic << ", " << hadElastic);
    if ( !hadInelastic && !hadElastic) {
      ATH_MSG_VERBOSE( "  [ g4sim ] Current process not an inelastic or elastic  hadronic process -> process not registered" );
      continue;
    }

    if (hadInelastic || hadElastic) {
      //Prepare and build the Physics table for primaries 
      curProc->PreparePhysicsTable(*parDef);
      curProc->BuildPhysicsTable(*parDef);
    }

    if(hadInelastic){
      ret = m_g4HadrInelasticProcesses.insert( std::pair<int,G4VProcess*>( pdg, hadInelastic) );
      ATH_MSG_DEBUG( "  [ g4sim ] Registered Geant4 hadronic interaction processes for particles with pdg code " << pdg );
    }
    if(m_doElastic && hadElastic ){
      ret = m_g4HadrElasticProcesses.insert( std::pair<int,G4VProcess*>( pdg, hadElastic) );
      G4ProcessType pType = curProc->GetProcessType();
      ATH_MSG_DEBUG( "  [ g4sim ] Registered Geant4 ELASTIC hadronic interaction processes for particles with pdg code " 
		     << pdg << "and process " <<  pType);
    } 


  } // process loop

  // return iterator to insterted G4VProcess
  return ret.first;
}

bool iFatras::G4HadIntProcessor::hadronicInteraction(const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
						     double, double, double,
						     const Trk::MaterialProperties& mprop, double pathCorrection,
						     Trk::ParticleHypothesis particle) const
{
  // do not treat geantinos
  if (particle==Trk::geantino) return false;

  bool processSecondaries = true;

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
    return doHadronicInteraction( 0., position, momentum, &(ematprop->material()), particle, processSecondaries);
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
  m_g4zeroPos.reset(new G4ThreeVector(0, 0, 0));
  m_g4step.reset(new G4Step());
  m_g4stepPoint.reset(new G4StepPoint());
  m_g4step->SetPreStepPoint(m_g4stepPoint.get()); // ownership taken

  // define the available G4Material
  m_g4Material.clear();

  G4NistManager* G4Nist = G4NistManager::Instance();
  G4Material* air = G4Nist->G4NistManager::FindOrBuildMaterial("G4_AIR");
  if (air) {
    G4MaterialCutsCouple* airCuts = new G4MaterialCutsCouple(air);
    // airCuts->SetIndex(0);    // ?
    std::pair<G4Material*,G4MaterialCutsCouple*> airMat(air,airCuts);
    m_g4Material.push_back(std::pair<float,std::pair<G4Material*,G4MaterialCutsCouple*> >(0.,airMat));
  }
  G4Material* h = G4Nist->G4NistManager::FindOrBuildMaterial("G4_H");
  if (h) {
    G4MaterialCutsCouple* hCuts = new G4MaterialCutsCouple(h);
    std::pair<G4Material*,G4MaterialCutsCouple*> hMat(h,hCuts);
    m_g4Material.push_back(std::pair<float,std::pair<G4Material*,G4MaterialCutsCouple*> >(1.,hMat));
  }
  G4Material* al = G4Nist->G4NistManager::FindOrBuildMaterial("G4_Al");
  if (al) {
    G4MaterialCutsCouple* alCuts = new G4MaterialCutsCouple(al);
    std::pair<G4Material*,G4MaterialCutsCouple*> alMat(al,alCuts);
    m_g4Material.push_back(std::pair<float,std::pair<G4Material*,G4MaterialCutsCouple*> >(13.,alMat));
  }
  G4Material* si = G4Nist->G4NistManager::FindOrBuildMaterial("G4_Si");
  if (si) {
    G4MaterialCutsCouple* siCuts = new G4MaterialCutsCouple(si);
    std::pair<G4Material*,G4MaterialCutsCouple*> siMat(si,siCuts);
    m_g4Material.push_back(std::pair<float,std::pair<G4Material*,G4MaterialCutsCouple*> >(14.,siMat));
  }
  G4Material* ar = G4Nist->G4NistManager::FindOrBuildMaterial("G4_Ar");
  if (ar) {
    G4MaterialCutsCouple* arCuts = new G4MaterialCutsCouple(ar);
    std::pair<G4Material*,G4MaterialCutsCouple*> arMat(ar,arCuts);
    m_g4Material.push_back(std::pair<float,std::pair<G4Material*,G4MaterialCutsCouple*> >(18.,arMat));
  }
  G4Material* fe = G4Nist->G4NistManager::FindOrBuildMaterial("G4_Fe");
  if (fe) {
    G4MaterialCutsCouple* feCuts = new G4MaterialCutsCouple(fe);
    std::pair<G4Material*,G4MaterialCutsCouple*> feMat(fe,feCuts);
    m_g4Material.push_back(std::pair<float,std::pair<G4Material*,G4MaterialCutsCouple*> >(26.,feMat));
  }
  G4Material* pb = G4Nist->G4NistManager::FindOrBuildMaterial("G4_Pb");
  if (pb) {
    G4MaterialCutsCouple* pbCuts = new G4MaterialCutsCouple(pb);
    std::pair<G4Material*,G4MaterialCutsCouple*> pbMat(pb,pbCuts);
    m_g4Material.push_back(std::pair<float,std::pair<G4Material*,G4MaterialCutsCouple*> >(82.,pbMat));
  }
 
  ATH_MSG_INFO("material vector size for had interaction:"<< m_g4Material.size());

  //G4cout << *(G4Material::GetMaterialTable()) << std::endl;

  return true;
}

ISF::ISFParticleVector iFatras::G4HadIntProcessor::getHadState(const ISF::ISFParticle* parent, 
							       double time, const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
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
  std::map<int, G4VProcess*>::iterator processIter_inelast = m_g4HadrInelasticProcesses.find(pdg);
  std::map<int, G4VProcess*>::iterator processIter_elast   = m_g4HadrElasticProcesses.find(pdg);

  if ( (processIter_inelast==m_g4HadrInelasticProcesses.end()) && (processIter_elast==m_g4HadrElasticProcesses.end()) ) {
    ATH_MSG_DEBUG ( " [ g4sim ] No hadronic interactions registered for current particle type (pdg=" << pdg << ")" );
    initProcessPDG(pdg);
    return chDef;       // this interaction aborted but next may go through
  }
  //if ( processIter_inelast==m_g4HadrInelasticProcesses.end()) return chDef;

  ATH_MSG_DEBUG ( " [ g4sim ] Found registered hadronic interactions for current particle type (pdg=" << pdg << ")" );

  // setup up G4Track ------------------------------------------------------------------------------
  //
  const G4ParticleDefinition *g4parDef = G4ParticleTable::GetParticleTable()->FindParticle(pdg);
  if ( g4parDef==0) {
    ATH_MSG_WARNING( "[ ---- ] Unable to find G4ParticleDefinition for particle with PID=" << pdg << " --> skipping hadronic interactions" );
    return chDef;
  }
  G4DynamicParticle* inputPar = new G4DynamicParticle();
  inputPar->SetDefinition( g4parDef);
  // input momentum - respect upper limits
  if ( momentum.mag()>1.e08 ) { 
    ATH_MSG_WARNING( "input momentum beyond limit" << momentum.mag() << " --> skipping hadronic interaction" );
    return chDef;
  } 
  const G4ThreeVector mom( momentum.x(), momentum.y(), momentum.z() );
  inputPar->SetMomentum( mom);
  // position and timing dummy
  std::unique_ptr<G4Track> g4track = std::make_unique<G4Track>( inputPar, 0 /* time */, *m_g4zeroPos);

  // setup up G4Material ---------------------------------------------------------------------------
  std::pair<G4Material*, G4MaterialCutsCouple*> g4mat = retrieveG4Material(ematprop);
  if (!g4mat.first) return chDef;

  // further G4 initializations (G4Step, G4MaterialCutsCouple, ...)
  m_g4stepPoint->SetMaterial( g4mat.first);
  m_g4stepPoint->SetMaterialCutsCouple( g4mat.second);

  // preparing G4Step and G4Track
  m_g4step->DeleteSecondaryVector();
  g4track->SetStep(m_g4step.get());

  // by default, the current process is the inelastic hadr. interaction
  // Does not take ownership (see definition of m_g4HadrInelasticProcesses) -> no delete or smart pointer
  G4VProcess *process = processIter_inelast!=m_g4HadrInelasticProcesses.end() ? processIter_inelast->second : 0;
 
  // if elastic interactions are enabled and there is a elastic process
  // in the m_g4HadrProcesses_Elastic std::map 
  if( m_doElastic && (processIter_elast!=m_g4HadrElasticProcesses.end()) ) {
    double rand   = CLHEP::RandFlat::shoot(m_randomEngine, 0., 1.);

    // use a 50% chance to use either elastic or inelastic processes : TODO retrieve cross-section
    if( rand < 0.5) process = processIter_elast->second;
  }

  if (!process)
  {
    ATH_MSG_WARNING( " [ ---- ] Cannot get G4VProcess from map (nullptr)!");
    return chDef;
  }

  ATH_MSG_VERBOSE ( " [ g4sim ] Computing " << process->GetProcessName() << " process with current particle" );

  // Does not take ownership -> no delete or smart pointer
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
      if ( dynPar->GetTotalMomentum() < m_minMomentum)
        continue;

      // get dynamic particle
      const G4ParticleDefinition *parDef = trk->GetParticleDefinition();

      // skip ions
      if (parDef->GetPDGEncoding()>1.e09) continue;

      //Prepare and build the physics table for secondaries
      //process->PreparePhysicsTable(*parDef);
      //process->BuildPhysicsTable(*parDef);

      ATH_MSG_VERBOSE( " [ g4sim ] Adding child particle to particle stack (pdg=" << parDef->GetPDGEncoding()
		       << " p=" << dynPar->GetTotalMomentum() );


      // create the particle to be put into ISF stack
      const G4ThreeVector &momG4 = dynPar->GetMomentum();
      Amg::Vector3D mom( momG4.x(), momG4.y(), momG4.z() );

      //Let's make sure the new ISFParticle get some valid TruthBinding and HepMcParticleLink objects
      ISF::TruthBinding* truthBinding = NULL;
      if (parent->getTruthBinding()) {
 	        ATH_MSG_VERBOSE("Could retrieve TruthBinding from original ISFParticle");
 	        truthBinding = new ISF::TruthBinding(*parent->getTruthBinding());
      }
      else
 	        ATH_MSG_WARNING("Could not retrieve TruthBinding from original ISFParticle, might cause issues later on.");
      const HepMcParticleLink * partLink = nullptr;
      if (parent->getParticleLink()) {
	ATH_MSG_VERBOSE("Could retrieve HepMcParticleLink from original ISFParticle");
	partLink = new HepMcParticleLink(Barcode::fUndefinedBarcode, parent->getParticleLink()->eventIndex(), parent->getParticleLink()->getEventCollection());
      }
      else {
	ATH_MSG_WARNING("Could not retrieve HepMcParticleLink from original ISFParticle, creating one from only Barcode::fUndefinedBarcode.");
	partLink = new HepMcParticleLink(Barcode::fUndefinedBarcode);
      }

      ISF::ISFParticle* cParticle = new ISF::ISFParticle( position,
                                                          mom,
                                                          parDef->GetPDGMass(),
                                                          parDef->GetPDGCharge(),
                                                          parDef->GetPDGEncoding(),
                                                          time, 
                                                          *parent,
							  Barcode::fUndefinedBarcode,
							  truthBinding,
                                                          partLink);
      cParticle->setNextGeoID( parent->nextGeoID() );
      cParticle->setNextSimID( parent->nextSimID() );
      // process sampling tool takes care of validation info
      *childrenIt = cParticle;
      ++childrenIt; numChildren++;
    }

    children.resize(numChildren);
    // truth info handled by process sampling tool

    // free up memory
    g4change->Clear();
    return children;

  }

  // free up memory
  g4change->Clear();
  return chDef;
}



bool iFatras::G4HadIntProcessor::doHadronicInteraction(double time, const Amg::Vector3D& position, const Amg::Vector3D& momentum,
						       const Trk::Material *ematprop,
						       Trk::ParticleHypothesis /*particle*/,  
						       bool  processSecondaries) const
{
  // get parent particle
  // @TODO: replace by Fatras internal bookkeeping
  const ISF::ISFParticle *parent = ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  assert(parent);

  ISF::ISFParticleVector ispVec=getHadState(parent, time, position, momentum, ematprop);

  if (!ispVec.size()) return false;

  // push onto ParticleStack

  if (processSecondaries) {
    for (unsigned int ic=0; ic<ispVec.size(); ic++) {
	//First let's make sure that new ISFParticles have valid truth info
	if (!ispVec[ic]->getTruthBinding()) {
		ispVec[ic]->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
	}
	if (!ispVec[ic]->getParticleLink()) {
		ispVec[ic]->setParticleLink(new HepMcParticleLink(*parent->getParticleLink()));
	}
	m_particleBroker->push(ispVec[ic], parent);
    }
  }

  return true;

}

ISF::ISFParticleVector iFatras::G4HadIntProcessor::doHadIntOnLayer(const ISF::ISFParticle* parent, double time, 
								   const Amg::Vector3D& position, const Amg::Vector3D& momentum,
								   const Trk::Material *emat,
								   Trk::ParticleHypothesis /*particle=Trk::pion*/) const 
{
 
  return getHadState(parent, time, position, momentum, emat);

}

std::pair<G4Material*,G4MaterialCutsCouple*> iFatras::G4HadIntProcessor::retrieveG4Material(const Trk::Material *ematprop) const {
  
  if (!m_g4Material.size()) {
    ATH_MSG_WARNING(" no predefined G4 material available for hadronic interaction " );
    return std::pair<G4Material*,G4MaterialCutsCouple*> (0,0); 
  }
  
  // in the absence of detailed material composition, use average Z
  // if not available on input, take Al
  float  iZ = ematprop ? ematprop->averageZ() : 13 ;

  // choose from predefined materials
  unsigned int imat=0;
  
  while (imat < m_g4Material.size() && iZ > m_g4Material[imat].first ) imat++; 
  
  unsigned int iSel=imat< m_g4Material.size() ? imat : m_g4Material.size()-1;

  if (iSel>0) { 
    // pick randomly to reproduce the average Z 
    //double rnd   = CLHEP::RandFlat::shoot(m_randomEngine, 0., 1.);
    //if (rnd < (iZ-m_g4Material[iSel-1].first)/(m_g4Material[iSel].first-m_g4Material[iSel-1].first)) iSel--;
    // weighted   
    float dz2 =  -pow(m_g4Material[iSel-1].first,2)+pow(m_g4Material[iSel].first,2);
    double rnd   = CLHEP::RandFlat::shoot(m_randomEngine, 0., 1.);
    if (iZ*iZ+pow(m_g4Material[iSel-1].first,2) < rnd*dz2) iSel--;
  }  

  return m_g4Material[iSel].second;

}
