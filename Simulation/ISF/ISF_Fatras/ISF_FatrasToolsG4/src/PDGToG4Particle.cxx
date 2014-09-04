/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PDGToG4Particle.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasToolsG4/PDGToG4Particle.h"

// Geant4
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4DecayTable.hh"

//#define PDGTOG4PARTICLE_USE_SUSY

#ifdef PDGTOG4PARTICLE_USE_SUSY

#include "Sleptons/G4SElectronMinus.hh"
#include "Sleptons/G4SElectronPlus.hh"
#include "Sleptons/G4SMuonMinus.hh"
#include "Sleptons/G4SMuonPlus.hh"
#include "Sleptons/G4STauMinus.hh"
#include "Sleptons/G4STauPlus.hh"

#include "Gauginos/G4Gravitino.hh"
#include "Gauginos/G4Neutralino.hh"

#endif

std::map<int,G4ParticleDefinition*>
iFatras::PDGToG4Particle::s_predefinedParticles;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::PDGToG4Particle::PDGToG4Particle(const std::string& t,
                                         const std::string& n,
                                         const IInterface* p)
        : AthAlgTool(t,n,p),
          m_pdgG4ParticleMap()
{
  declareInterface<iFatras::PDGToG4Particle>(this);

  declareProperty("UseParticles", m_useParticles);
  declareProperty("PrintList",    m_printList=false);

  if( !s_predefinedParticles.size())
      fillPredefinedParticles();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::PDGToG4Particle::~PDGToG4Particle()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::PDGToG4Particle::initialize()
{
  // Update output level
  ATH_MSG_INFO( "initialize()" );

  /*-----------------------------------------------------------------------
   *  Fill map of particles
   *-----------------------------------------------------------------------*/
  if( m_useParticles.size() == 0)
  {
    ATH_MSG_INFO( "using all predefined particles" );
    m_pdgG4ParticleMap.insert( s_predefinedParticles.begin(),
                               s_predefinedParticles.end());
    ATH_MSG_INFO( "loaded " << m_pdgG4ParticleMap.size() << " particles" );
  }
  else
  {
    std::vector<int>::const_iterator pdgIt = m_useParticles.begin();
    for( ; pdgIt != m_useParticles.end(); ++pdgIt)
    {
      PDGG4ParticleMap::const_iterator mapIt =
          s_predefinedParticles.find( *pdgIt);
      if( mapIt == s_predefinedParticles.end())
      {
        ATH_MSG_WARNING( "particle with pdg code " << *pdgIt
              << " not found in list of predefined particles, ignoring" );
        continue;
      }
      
      if( m_pdgG4ParticleMap.find( *pdgIt) != m_pdgG4ParticleMap.end())
      {
        ATH_MSG_WARNING( "particle with pdg code " << *pdgIt
              << " already loaded, ignoring" );
        continue;
      }
      
      m_pdgG4ParticleMap[*pdgIt] = mapIt->second;
    }
  }

  if( m_printList)
  {
    ATH_MSG_INFO( "List of loaded particles:" );
    printListOfParticles();
  }
            
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::PDGToG4Particle::finalize()
{

  ATH_MSG_INFO( "finalize() successful" );  
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
G4ParticleDefinition*
iFatras::PDGToG4Particle::getParticleDefinition( int pdgCode) const
{
  G4ParticleDefinition* ret = 0;
  
  PDGG4ParticleMap::const_iterator it =
      m_pdgG4ParticleMap.find( pdgCode);
  
  if( it != m_pdgG4ParticleMap.end()) ret = it->second;
  else
  {
    it = m_pdgG4ParticleMap.find( std::abs( pdgCode));
    if( it != m_pdgG4ParticleMap.end())
    {
      if( std::abs( it->second->GetPDGCharge()) < 0.1)
          ret = it->second;
      else
      {
        ATH_MSG_WARNING( "PDG Code " << pdgCode << " not found,"
              << " " << -pdgCode << " exists but is charged!" );
      }
    }
  }
  
  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
std::vector<std::pair<int,std::string> >
iFatras::PDGToG4Particle::listOfParticles() const
{
  std::vector<std::pair<int,std::string> > ret;
  
  for( PDGG4ParticleMap::const_iterator it = m_pdgG4ParticleMap.begin();
       it != m_pdgG4ParticleMap.end(); ++it)
  {
    // Only return matter particles (for consistency with HepPDT)
    if( it->first > 0)
    {
      ret.push_back( std::make_pair( it->first, it->second->GetParticleName()));
    }
  }
    
  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
iFatras::PDGToG4Particle::printListOfParticles( bool withDecayTableOnly) const
{
  std::cout << "****************************************"
            << "****************************************"
            << std::endl;
  
  std::cout << "* "
            << std::setw( 8) << "PDG Code" << " "
            << std::setw(16) << "Particle Name" << " "
            << std::setw( 8) << "G4 PDGID" << " "
            << std::setw( 8) << "Mass" << " "
            << std::setw(16) << "Width" << " "
            << std::setw( 6) << "Charge" << " "
            << std::setw( 7) << "#DecChn" << " "
            << std::endl;
  
  std::cout << "****************************************"
            << "****************************************"
            << std::endl;
  
  for( PDGG4ParticleMap::const_iterator it = m_pdgG4ParticleMap.begin();
       it != m_pdgG4ParticleMap.end(); ++it)
  {
    G4DecayTable* dt = it->second->GetDecayTable();
    int nDecayChannels = 0;
    if( dt) nDecayChannels = dt->entries();
    else if( withDecayTableOnly) continue;
    
    std::cout << "* "
              << std::setw( 8) << it->first << " "
              << std::setw(16) << it->second->GetParticleName() << " "
              << std::setw( 8) << it->second->GetPDGEncoding() << " "
              << std::setw( 8) << it->second->GetPDGMass() << " "
              << std::setw(16) << it->second->GetPDGWidth() << " "
              << std::setw( 6) << it->second->GetPDGCharge() << " "
              << std::setw( 7) << nDecayChannels << " "
              << std::endl;
  }
  
  std::cout << "****************************************"
            << "****************************************"
            << std::endl;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
iFatras::PDGToG4Particle::fillPredefinedParticles()
{
  // Gauge and Higgs Bosons
  addParticle( G4Gamma::GammaDefinition());

  // Leptons
  addParticle( G4Electron::ElectronDefinition());
  addParticle( G4NeutrinoE::NeutrinoEDefinition());
  addParticle( G4MuonMinus::MuonMinusDefinition());
  addParticle( G4NeutrinoMu::NeutrinoMuDefinition());
  addParticle( G4TauMinus::TauMinusDefinition());
  addParticle( G4NeutrinoTau::NeutrinoTauDefinition());
  addParticle( G4Positron::PositronDefinition());
  addParticle( G4AntiNeutrinoE::AntiNeutrinoEDefinition());
  addParticle( G4MuonPlus::MuonPlusDefinition());
  addParticle( G4AntiNeutrinoMu::AntiNeutrinoMuDefinition());
  addParticle( G4TauPlus::TauPlusDefinition());
  addParticle( G4AntiNeutrinoTau::AntiNeutrinoTauDefinition());
  
  // Light I=1 Mesons
  addParticle( G4PionZero::PionZeroDefinition());
  addParticle( G4PionPlus::PionPlusDefinition());
  addParticle( G4PionMinus::PionMinusDefinition());

  // Light I=0 Mesons
  addParticle( G4Eta::EtaDefinition());
  addParticle( G4EtaPrime::EtaPrimeDefinition());

  // Strange Mesons
  addParticle( G4KaonZeroLong::KaonZeroLongDefinition());
  addParticle( G4KaonZeroShort::KaonZeroShortDefinition());
  addParticle( G4KaonZero::KaonZeroDefinition());
  addParticle( G4KaonPlus::KaonPlusDefinition());
  addParticle( G4AntiKaonZero::AntiKaonZeroDefinition());
  addParticle( G4KaonMinus::KaonMinusDefinition());

  // Charmed Mesons
  addParticle( G4DMesonPlus::DMesonPlusDefinition());
  addParticle( G4DMesonZero::DMesonZeroDefinition());
  addParticle( G4DsMesonPlus::DsMesonPlusDefinition());
  addParticle( G4DMesonMinus::DMesonMinusDefinition());
  addParticle( G4AntiDMesonZero::AntiDMesonZeroDefinition());
  addParticle( G4DsMesonMinus::DsMesonMinusDefinition());

  // Bottom Mesons
  addParticle( G4BMesonZero::BMesonZeroDefinition());
  addParticle( G4BMesonPlus::BMesonPlusDefinition());
  addParticle( G4BsMesonZero::BsMesonZeroDefinition());
  addParticle( G4AntiBMesonZero::AntiBMesonZeroDefinition());
  addParticle( G4BMesonMinus::BMesonMinusDefinition());
  addParticle( G4AntiBsMesonZero::AntiBsMesonZeroDefinition());

  // ccbar Mesons
  addParticle( G4JPsi::JPsiDefinition());

  // Light Baryons
  addParticle( G4Proton::ProtonDefinition());
  addParticle( G4Neutron::NeutronDefinition());
  addParticle( G4AntiProton::AntiProtonDefinition());
  addParticle( G4AntiNeutron::AntiNeutronDefinition());

  // Strange Baryons
  addParticle( G4Lambda::LambdaDefinition());
  addParticle( G4SigmaPlus::SigmaPlusDefinition());
  addParticle( G4SigmaZero::SigmaZeroDefinition());
  addParticle( G4SigmaMinus::SigmaMinusDefinition());
  addParticle( G4XiZero::XiZeroDefinition());
  addParticle( G4XiMinus::XiMinusDefinition());
  addParticle( G4OmegaMinus::OmegaMinusDefinition());
  addParticle( G4AntiLambda::AntiLambdaDefinition());
  addParticle( G4AntiSigmaPlus::AntiSigmaPlusDefinition());
  addParticle( G4AntiSigmaZero::AntiSigmaZeroDefinition());
  addParticle( G4AntiSigmaMinus::AntiSigmaMinusDefinition());
  addParticle( G4AntiXiZero::AntiXiZeroDefinition());
  addParticle( G4AntiXiMinus::AntiXiMinusDefinition());
  addParticle( G4AntiOmegaMinus::AntiOmegaMinusDefinition());
  
  // Charmed Baryons
  addParticle( G4LambdacPlus::LambdacPlusDefinition());
  addParticle( G4SigmacPlusPlus::SigmacPlusPlusDefinition());
  addParticle( G4SigmacPlus::SigmacPlusDefinition());
  addParticle( G4SigmacZero::SigmacZeroDefinition());
  addParticle( G4XicPlus::XicPlusDefinition());
  addParticle( G4XicZero::XicZeroDefinition());
  addParticle( G4OmegacZero::OmegacZeroDefinition());
  addParticle( G4AntiLambdacPlus::AntiLambdacPlusDefinition());
  addParticle( G4AntiSigmacPlusPlus::AntiSigmacPlusPlusDefinition());
  addParticle( G4AntiSigmacPlus::AntiSigmacPlusDefinition());
  addParticle( G4AntiSigmacZero::AntiSigmacZeroDefinition());
  addParticle( G4AntiXicPlus::AntiXicPlusDefinition());
  addParticle( G4AntiXicZero::AntiXicZeroDefinition());
  addParticle( G4AntiOmegacZero::AntiOmegacZeroDefinition());

#ifdef PDGTOG4PARTICLE_USE_SUSY
  // SUSY particles from G4Extensions in ATHENA (incomplete)
  addParticle( G4SElectronMinus::SElectronMinusDefinition());
  addParticle( G4SElectronPlus::SElectronPlusDefinition());
  addParticle( G4SMuonMinus::SMuonMinusDefinition());
  addParticle( G4SMuonPlus::SMuonPlusDefinition());
  addParticle( G4STauMinus::STauMinusDefinition());
  addParticle( G4STauPlus::STauPlusDefinition());
#endif
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
iFatras::PDGToG4Particle::addParticle( G4ParticleDefinition* pDef)
{
  if( !pDef)
      return;
  
  s_predefinedParticles[pDef->GetPDGEncoding()] = pDef;
}
