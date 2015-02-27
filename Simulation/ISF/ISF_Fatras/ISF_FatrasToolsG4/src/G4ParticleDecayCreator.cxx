/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4ParticleDecayCreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasToolsG4/G4ParticleDecayCreator.h"
// iFatras
#include "ISF_FatrasToolsG4/PDGToG4Particle.h"
// Barcode
#include "BarcodeInterfaces/Barcode.h"
// Gaudi Kernel
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
// ISF
#include "ISF_Event/ISFParticle.h"
#include "ISF_Geant4Tools/IG4RunManagerHelper.h"
//CLHEP
#include "HepMC/GenParticle.h"
// G4
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4DecayTable.hh"
#include "G4DecayProducts.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::G4ParticleDecayCreator::G4ParticleDecayCreator( 
    const std::string& t, const std::string& n, const IInterface* p)
        : AthAlgTool(t,n,p),
	  m_g4RunManagerHelper("iGeant4::G4RunManagerHelper/G4RunManagerHelper"),
          m_pdgToG4Conv(""),
	  m_g4runManager(0)
              
{
  declareInterface<iFatras::IParticleDecayer>(this);

  // tool declarations
  declareProperty("G4RunManagerHelper"      , m_g4RunManagerHelper);
  declareProperty("PDGToG4ParticleConverter", m_pdgToG4Conv   );
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::G4ParticleDecayCreator::initialize()
{

  ATH_MSG_INFO( " initialize()" );

  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::G4ParticleDecayCreator::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );  
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
std::vector<ISF::ISFParticle*>
iFatras::G4ParticleDecayCreator::decayParticle(const ISF::ISFParticle& parent,
                                               const Amg::Vector3D& vertex, 
                                               const Amg::Vector3D& momentum,
                                               int pdgCode,
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

    // !< @TODO : insert truth binding 
    ISF::ISFParticle* childParticle = new ISF::ISFParticle( vertex,
                                                            amgMom,
                                                            prod->GetMass(),
                                                            prod->GetCharge(),
                                                            prod->GetPDGcode(),
                                                            timeStamp, 
                                                            parent );

    children.push_back( childParticle);
  }
  return children;
}

bool iFatras::G4ParticleDecayCreator::initG4RunManager() const {

  if (m_g4RunManagerHelper.retrieve().isFailure()) return false;

  m_g4runManager = m_g4RunManagerHelper->fastG4RunManager();

  return true;
}
