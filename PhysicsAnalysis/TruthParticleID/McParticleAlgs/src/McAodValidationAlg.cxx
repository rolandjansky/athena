/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// McAodValidationAlg.cxx 
// Implementation file for class McAodValidationAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <cstdlib> // for random numbers
#include <stdexcept>

// FrameWork includes
#include "Gaudi/Property.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

// NavFourMom includes
#include "NavFourMom/IParticleContainer.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleValidationTool.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleAlgs includes
#include "McAodValidationAlg.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
McAodValidationAlg::McAodValidationAlg( const std::string& name, 
                                        ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_valTools   ( this )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "ValidationTools", 
		   m_valTools,
		   "List of validation tools to be ran on the events" );
  // defaults
  m_valTools.push_back( "GenAodValidationTool/GenAodValidation" );
  m_valTools.push_back( "SpclMcValidationTool/SpclMcValidation" );

  declareProperty( "TruthParticles", 
		   m_truthParticlesName = "SpclMC",
		   "Name of the TruthParticleContainer to be read" );
  
  declareProperty( "Seed", 
		   m_seed = 12345678,
		   "Seed for the STL random generator" );
}

/// Destructor
///////////////
McAodValidationAlg::~McAodValidationAlg()
{}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode McAodValidationAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // initializing pseudo-random generator with the seed
  ATH_MSG_INFO
    ("Initializing pseudo-random generator..." 
     << endmsg
     << "\tseed= " << m_seed);
  std::srand( m_seed );
  
  // retrieve the list of validation tools
  if ( !m_valTools.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve McAod validation tools !!" << endmsg
       << " [" << m_valTools << "]");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode McAodValidationAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  ATH_MSG_INFO
    ("==> Finalizing validation AlgTools (" << m_valTools.size() << ")...");

  for ( IValidationTools_t::iterator 
	  iTool = m_valTools.begin(),
	  iEnd  = m_valTools.end();
	iTool != iEnd;
	++iTool ) {
    ATH_MSG_INFO ("-> finalizing [" << iTool->typeAndName() << "]:");
    if ( (*iTool)->finalize().isFailure() ) {
      ATH_MSG_WARNING 
	("Could not finalize validation tool [" 
	 << iTool->typeAndName() << "] !!");
    }
  }

  ATH_MSG_INFO 
    ("==> Finalizing validation AlgTools (" << m_valTools.size() 
     << ")... [DONE]");
  return StatusCode::SUCCESS;
}

StatusCode McAodValidationAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const double random =  std::rand() * 1.;
  ATH_MSG_DEBUG ("Random= " << random);
  
  const TruthParticleContainer * mcParts = 0;
  if ( evtStore()->retrieve( mcParts, m_truthParticlesName ).isFailure() ||
       0 == mcParts ) {
    ATH_MSG_ERROR
      ("Could not retrieve the TruthParticleContainer at ["
       << m_truthParticlesName << "] !!");
    return StatusCode::FAILURE;
  }
  
  // test the symLink feature
  {
    const DataVector<INavigable4Momentum> * inav4mom = 0;
    if ( evtStore()->retrieve( inav4mom, m_truthParticlesName ).isFailure() ||
	 0 == inav4mom ) {
      ATH_MSG_ERROR
	("Could not retrieve the (auto-symlinked) DV<INav4Mom> at ["
	 << m_truthParticlesName << "] !!");
    } else {
      ATH_MSG_DEBUG 
	("Successfully exercised the auto-symlink feature "\
	 "with [DV<INavigable4MomentumCollection>]");
    }
  }

  // test the symLink feature
  {
    const DataVector<IParticle> * ipc = 0;
    if ( evtStore()->retrieve( ipc, m_truthParticlesName ).isFailure() ||
	 0 == ipc ) {
      ATH_MSG_ERROR
	("Could not retrieve the (auto-symlinked) DV<IP> at ["
	 << m_truthParticlesName << "] !!");
    } else {
      ATH_MSG_DEBUG 
	("Successfully exercised the auto-symlink feature with [DV<IParticle>]");
    }
  }

  // test the symLink feature
  {
    const IParticleContainer * ipc = 0;
    if ( evtStore()->retrieve( ipc, m_truthParticlesName ).isFailure() ||
	 0 == ipc ) {
      ATH_MSG_ERROR
	("Could not retrieve the (auto-symlinked) IPC at ["
	 << m_truthParticlesName << "] !!");
    } else {
      ATH_MSG_DEBUG 
	("Successfully exercised the auto-symlink feature "\
	 "with [IParticleContainer]");
    }
  }

  double eneSum = 0.;
  for ( TruthParticleContainer::const_iterator itr = mcParts->begin();
        itr != mcParts->end();
        ++itr ) {
    const TruthParticle * mc = *itr;
    eneSum += mc->e();
  }
  ATH_MSG_DEBUG ("Sum Ene= " << (eneSum * (1./CLHEP::GeV)));

  bool validationIsOK = true;
  ATH_MSG_DEBUG ("Checking the event with the validation AlgTools...");

  for ( IValidationTools_t::iterator 
	  iTool = m_valTools.begin(),
	  iEnd  = m_valTools.end();
	iTool != iEnd;
	++iTool ) {
    ATH_MSG_DEBUG ("\tvalidation with [" << iTool->typeAndName() << "]...");
    if ( !(*iTool)->execute().isSuccess() ) {
      ATH_MSG_WARNING 
	("Event FAILED validation criteria of ["
	 << iTool->typeAndName() << "] !!");
      validationIsOK = false;
    }
  }

  if ( !validationIsOK ) {
    ATH_MSG_ERROR ("Event did not pass the validation !!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

