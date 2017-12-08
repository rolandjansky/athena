/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4ParticlePropertyTool.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "ISF_FatrasToolsG4/G4ParticlePropertyTool.h"
#include "ISF_FatrasToolsG4/PDGToG4Particle.h"

// G4
#include "G4ParticleDefinition.hh"

// Standard C Library
#include <cmath>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::G4ParticlePropertyTool::G4ParticlePropertyTool( const std::string& t,
                                                        const std::string& n,
                                                        const IInterface* p)
        : AthAlgTool( t,n,p),
          m_pdgToG4Conv(
              "iFatras::PDGToG4Particle/FatrasPDGToG4ParticleConverter")
{
  declareInterface<IParticlePropertyTool>(this);

  // tool declarations
  declareProperty( "PDGToG4ParticleConverter", m_pdgToG4Conv);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::G4ParticlePropertyTool::~G4ParticlePropertyTool()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::G4ParticlePropertyTool::initialize()
{

  ATH_MSG_INFO( "initialize()" );

  if( m_pdgToG4Conv.retrieve().isFailure())
  {
    ATH_MSG_FATAL( "Could not find PDGToG4 converter." );
    return StatusCode::FAILURE;
  }
  else
      ATH_MSG_DEBUG( "Retrieve " << m_pdgToG4Conv );

  if( msgLvl(MSG::VERBOSE) )
  {
    ATH_MSG_VERBOSE( "List of known particles:" );
    m_pdgToG4Conv->printListOfParticles();
  }
  
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::G4ParticlePropertyTool::finalize()
{


  ATH_MSG_INFO( "finalize() successful" );

  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
std::vector<std::pair<int,std::string> >
iFatras::G4ParticlePropertyTool::listOfParticles() const
{
  return m_pdgToG4Conv->listOfParticles();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::G4ParticlePropertyTool::basicInfo( int pdgCode,
                                           int* charge,
                                           double* mass) const
{
  G4ParticleDefinition* pDef =  m_pdgToG4Conv->getParticleDefinition( pdgCode);
  
  if (!pDef)
  {
    ATH_MSG_ERROR( "Could not find geant4 equivalent"
          << " for particle with pdg id " << pdgCode );
    return StatusCode::FAILURE;
  }

  if (charge)
      *charge = int( round( pDef->GetPDGCharge()));

  if (mass)
      *mass = pDef->GetPDGMass();

  return StatusCode::SUCCESS;
}
