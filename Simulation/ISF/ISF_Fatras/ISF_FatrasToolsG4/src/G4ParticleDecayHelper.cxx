/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4ParticleDecayHelper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
// iFatras
#include "ISF_FatrasToolsG4/G4ParticleDecayHelper.h"
#include "ISF_FatrasToolsG4/PDGToG4Particle.h"
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"
#include "ISF_Geant4Tools/IG4RunManagerHelper.h"
// ISF
#include "ISF_Event/ISFParticle.h"
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFTruthIncident.h"

// Gaudi & StoreGate
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Random/RandFlat.h"
// G4
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4DecayTable.hh"
#include "G4DecayProducts.hh"
#include <cmath>

double iFatras::G4ParticleDecayHelper::s_speedOfLightSI =  CLHEP::c_light*CLHEP::s/CLHEP::mm;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::G4ParticleDecayHelper::G4ParticleDecayHelper(const std::string& t, const std::string& n, const IInterface* p):    
   AthAlgTool(t,n,p),
   m_particleBroker("ISF_ParticleBroker", n),
   m_truthRecordSvc("ISF_TruthRecordSvc", n),
   m_rndmSvc("AtDSFMTGenSvc", n),
   m_randomEngine(0),
   m_randomEngineName("FatrasRnd"),
   m_G4RandomEngineName("FatrasG4"),
   m_g4RunManagerHelper("iGeant4::G4RunManagerHelper/G4RunManagerHelper"),
   m_pdgToG4Conv("iFatras::PDGToG4Particle/PDGToG4ParticleConverter"),
   m_g4runManager(0),
   m_validationMode(false),
   m_validationTool(""),
   m_nProcessedParticles(0),
   m_nStableParticles(0),
   m_nExitedParticles(0),
   m_nDecayedParticles(0),
   m_nChargedParticles(0),
   m_nNeutralParticles(0)
{
  declareInterface<IParticleDecayHelper>(this);
    
  // ISF: truth and broker service
  declareProperty("ParticleBroker",           m_particleBroker,       "ISF Particle Broker Svc");
  declareProperty("ParticleTruthSvc",         m_truthRecordSvc,       "ISF Particle Truth Svc");
  // random number initializations 
  declareProperty("RandomNumberService",      m_rndmSvc,              "Random number generator");
  declareProperty("RandomStreamName",         m_randomEngineName,     "Name of the random number stream");;
  declareProperty("G4RandomStreamName",       m_G4RandomEngineName,   "Name of the random number stream for G4 tools");
  
  // tool declarations -------------------------------------------------
  declareProperty("G4RunManagerHelper"      , m_g4RunManagerHelper);
  declareProperty("PDGToG4ParticleConverter", m_pdgToG4Conv);
  // validation output section
  declareProperty( "ValidationMode",  m_validationMode );
  declareProperty( "PhysicsValidationTool",     m_validationTool  );

}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::G4ParticleDecayHelper::~G4ParticleDecayHelper()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::G4ParticleDecayHelper::initialize()
{

  ATH_MSG_INFO( "initialize()" );

  // ISF Services
  if ( m_particleBroker.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " <<  m_particleBroker );
      return StatusCode::FAILURE;
  }
  if (m_truthRecordSvc.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_truthRecordSvc );
      return StatusCode::FAILURE;
  }

  // Random number service
  if ( m_rndmSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Could not retrieve " << m_rndmSvc );
    return StatusCode::FAILURE;
  }
  //Get own engine with own seeds:
  m_randomEngine = m_rndmSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL(  "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE;
  }
  // G4ParticleDecayCreator uses functions from G4 (G4VDecayChannel::DecayIt() and G4DecayTable::SelectADecayChannel())
  // which use CLHEP::HepRandom directly with the default engine. Therefore we set the default engine to a special stream,
  // analog to G4AtlasAlg.
  // Set the random number generator to AtRndmGen
  CLHEP::HepRandomEngine* engine = m_rndmSvc->GetEngine(m_G4RandomEngineName);
  if (!engine) {
    ATH_MSG_FATAL(  "Could not get random engine '" << m_G4RandomEngineName << "'" );
    return StatusCode::FAILURE;
  }
  CLHEP::HepRandom::setTheEngine(engine);

  // get G4RunManagerHelper ( no G4RunManager initialization done )
  if (m_g4RunManagerHelper.retrieve().isFailure()) {
    ATH_MSG_FATAL(  "Could not get g4RunManagerHelper '" << m_g4RunManagerHelper << "'" );
    return StatusCode::FAILURE;
  } else {
    std::cout <<"f4dec: g4RunManagerHelper retrieved:"<< m_g4RunManagerHelper<< std::endl;
  }

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::G4ParticleDecayHelper::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );  
  return StatusCode::SUCCESS;
}

double iFatras::G4ParticleDecayHelper::freePath(const ISF::ISFParticle& isp) const {
  // get the particle properties    
  int pdgCode =  isp.pdgCode();

  // initialize G4RunManager if not done already
  if (!m_g4runManager) {
    bool g4mgr = initG4RunManager();
    if (!g4mgr) ATH_MSG_WARNING(  "initialization of G4RunManager failed in G4ParticleDecayHelper" );
  }

  if (!m_pdgToG4Conv && m_pdgToG4Conv.retrieve().isFailure()) return -1.;

  G4ParticleDefinition* pDef = m_pdgToG4Conv->getParticleDefinition(pdgCode);
  
  // for the processing    
  bool isStable = true;

  if( !pDef )      {
    ATH_MSG_WARNING( "[ decay ] could not find particle properties" << " for PDG code " << pdgCode );
    return -1.;
  }
  // set is stable from Geant4 particle definition
  isStable = pDef->GetPDGStable();
  // set muons to stable
  if ( std::abs(pdgCode) == 13) isStable = true;
  // we have a stable particle --- return
  if (isStable) return -1.;
  
  double mass = pDef->GetPDGMass();
  // take momentum from ParticleState rather than associated truth
  CLHEP::HepLorentzVector particleMom( isp.momentum().x(),
                                       isp.momentum().y(),
                                       isp.momentum().z(),
                                       sqrt(isp.momentum().mag2()+mass*mass) );

  // get average lifetime
  double tau = pDef->GetPDGLifeTime()*1e-9;
  // sample the lifetime
  double lifeTime = -tau*log(CLHEP::RandFlat::shoot(m_randomEngine));
  // distance in mm
  double pathlength =
    lifeTime*s_speedOfLightSI*particleMom.gamma()*particleMom.beta();
  return pathlength;
}

void iFatras::G4ParticleDecayHelper::decay(const ISF::ISFParticle& particleToDecay,
					   const Amg::Vector3D& vertex, 
					   const Amg::Vector3D& momentum,
					   double timeStamp) const
{

  /*-------------------------------------------------------------------
   *  The actual decay
   *-------------------------------------------------------------------*/
  ++m_nDecayedParticles;
  ATH_MSG_DEBUG( "[ decay ] calling G4ParticleDecayCreator with " << particleToDecay );

  // perform the decay 
  const ISF::ISFParticleVector decayProducts = decayParticle(particleToDecay,vertex,momentum,timeStamp);

  // fill them into broker & truth svc
  handleDecayParticles(particleToDecay,decayProducts);
}

                
void iFatras::G4ParticleDecayHelper::handleDecayParticles(const ISF::ISFParticle& particle,
                                                          const ISF::ISFParticleVector& decayProducts) const {
   // process the decay products ---------------------------------------
  Barcode::PhysicsProcessCode                 process = 201;
   // (i) none       
   if (!decayProducts.size()) {
        ATH_MSG_WARNING("[ decay ] Particle Decay Creator did not return any"
            << " decay products for particle with PDG code "
            << particle.pdgCode() );       
   } else {
   // (ii) many
      std::ostringstream productSummaryString;
      productSummaryString << "[ decay ] products:";
      //!< @TODO
      // truth service !!!
      // simulate the tracks of the daughter particles ------- run over decay products
      
      ISF::ISFParticleVector::const_iterator decayProductsItr = decayProducts.begin();
      for (; decayProductsItr != decayProducts.end(); ++decayProductsItr)
      {
	productSummaryString << "     - "  << (**decayProductsItr) << '\n';
	// in the validation mode, add process info
	if (m_validationMode) {
	  ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
	  validInfo->setProcess(process);
	  if (particle.getUserInformation()) validInfo->setGeneration(particle.getUserInformation()->generation()+1);
	  else validInfo->setGeneration(1);     // assume parent is a primary track
	  (*decayProductsItr)->setUserInformation(validInfo);
	}
	// register next geo (is current), next flavor can be defined by filter
	(*decayProductsItr)->setNextGeoID( particle.nextGeoID() );
	// feed it the particle broker with parent information
	m_particleBroker->push(*decayProductsItr, &particle);
      }//loop over all decay products
      ATH_MSG_VERBOSE(  productSummaryString.str() );     

      // register TruthIncident
      ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(particle),
				   decayProducts,
				   process,
				   particle.nextGeoID(),  // inherits from the parent
				   ISF::fKillsPrimary );
      m_truthRecordSvc->registerTruthIncident( truth);
      
      // save info for validation
      if (m_validationMode && m_validationTool) {
	Amg::Vector3D* nMom = 0;
	m_validationTool->saveISFVertexInfo(process,particle.position(),particle,particle.momentum(),nMom,decayProducts);
	delete nMom;
      }

    }    
}

std::vector<ISF::ISFParticle*>
iFatras::G4ParticleDecayHelper::decayParticle(const ISF::ISFParticle& parent,
                                               const Amg::Vector3D& vertex, 
                                               const Amg::Vector3D& momentum,
                                               double timeStamp) const
{
  // return vector for children
  std::vector<ISF::ISFParticle*> children;

  // initialize G4RunManager if not done already
  if (!m_g4runManager) {
    bool g4mgr = initG4RunManager();
    if (!g4mgr) ATH_MSG_WARNING(  "initialization of G4RunManager failed in G4ParticleDecayHelper" );
  }

  if (!m_pdgToG4Conv) {

    if( m_pdgToG4Conv.retrieve().isFailure()) return children;

    if( msgLvl(MSG::VERBOSE)) {
      ATH_MSG_VERBOSE(  "List of particles with decay info:" );
      m_pdgToG4Conv->printListOfParticles( true);
    }
  }

  int pdgCode = parent.pdgCode();
    
  G4ParticleDefinition* pDef = m_pdgToG4Conv->getParticleDefinition( pdgCode);
  if( !pDef)
  {
    ATH_MSG_WARNING( "[ decay ] could not find geant4 equivalent"
          << " for particle with pdg id " << pdgCode );
    return children;
  }
  
  ATH_MSG_DEBUG(  "[ decay ] Decaying " << pDef->GetParticleName() );
  
  G4DecayTable* dt = pDef->GetDecayTable();
  if( !dt)
  {
    ATH_MSG_WARNING( "[ decay ] empty decay table for"
          << " particle with pdg id " << pdgCode );
    return children;
  }
    
  if( msgLvl(MSG::VERBOSE))
  {
    ATH_MSG_VERBOSE(  "[ decay ] Decay table:" );
    dt->DumpInfo();
  }
  
  G4VDecayChannel* channel = dt->SelectADecayChannel();
  if( !channel)
  {
    ATH_MSG_ERROR( "[ decay ] error selecting decay channel for"
          << " particle with pdg id " << pdgCode );
    return children;
  }
    
  if( msgLvl(MSG::DEBUG))
  {
    ATH_MSG_DEBUG(  "[ decay ] Decay channel:" );
    channel->DumpInfo();
  }
  
  G4DecayProducts* products = channel->DecayIt();
  if( !products)
  {
    ATH_MSG_ERROR( "[ decay ] error in decay product generation"
          << " for particle with pdg id " << pdgCode );
    return children;
  }
  
  if( msgLvl(MSG::VERBOSE))
  {
    ATH_MSG_VERBOSE(  "[ decay ] Decay products:" );
    products->DumpInfo();
  }

  // the parent energy
  double parentE = std::sqrt( std::pow( pDef->GetPDGMass(), 2) +
                              momentum.mag2());
  
  products->Boost( momentum.x()/parentE,
                   momentum.y()/parentE,
                   momentum.z()/parentE);
  if( msgLvl(MSG::VERBOSE))
  {
    ATH_MSG_VERBOSE( "[ decay ] Decay products after boost:" );
    products->DumpInfo();
  }

  G4int nProducts = products->entries();
  for( G4int i=0; i < nProducts; ++i)
  {
    G4DynamicParticle* prod = products->PopProducts();
    if( !prod)
    {
      ATH_MSG_WARNING( "[ decay ] Could not retrieve product " << i);
      continue;
    }
    
    // the decay product
    const G4ThreeVector &mom= prod->GetMomentum();
    Amg::Vector3D amgMom( mom.x(), mom.y(), mom.z() );

    ISF::TruthBinding * truthBinding = NULL;
    if (parent.getTruthBinding()) {
       ATH_MSG_VERBOSE("Could retrieve TruthBinding from original ISFParticle");
       truthBinding = new ISF::TruthBinding(*parent.getTruthBinding());
    }
    else ATH_MSG_WARNING("Could not retrieve original TruthBinding  from ISFParticle");
    ISF::ISFParticle* childParticle = new ISF::ISFParticle( vertex,
                                                            amgMom,
                                                            prod->GetMass(),
                                                            prod->GetCharge(),
                                                            prod->GetPDGcode(),
                                                            timeStamp, 
                                                            parent,
							    Barcode::fUndefinedBarcode,
							    truthBinding );

    children.push_back( childParticle);
  }
  return children;
}

bool iFatras::G4ParticleDecayHelper::initG4RunManager() const {

  m_g4runManager = m_g4RunManagerHelper->fastG4RunManager();

  return true;
}
