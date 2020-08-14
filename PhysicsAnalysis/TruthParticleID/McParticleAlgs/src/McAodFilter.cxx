/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// McAodFilter.cxx 
// Implementation file for class McAodFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <algorithm>

// FrameWork includes
#include "Gaudi/Property.h"

// McParticleUtils includes
#include "McParticleUtils/McVtxFilter.h"

// McParticleKernel includes
#include "McParticleKernel/IMcVtxFilterTool.h"
#include "McParticleKernel/ITruthParticleCnvTool.h"

// McParticleAlgs includes
#include "McAodFilter.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
McAodFilter::McAodFilter( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 

  declareProperty( "DoGenAod",
		   m_doGenAod = false,
		   "Switch to build or not the filtered GenEvent" );

  std::string descr = "";
  descr += "Switch to build or not the TruthParticleContainer ";
  descr += "from the filtered GenEvent";
  declareProperty( "DoTruthParticles", 
		   m_doTruthParticles = false,
		   descr );
  
  descr = "Tool to filter an McEventCollection according to some DecayPattern";
  declareProperty( "McVtxFilterTool",
		   m_mcVtxFilterTool = IMcVtxFilterTool_t( "McVtxFilterTool",
							   this ),
		   descr );

  descr = "Tool to convert a GenEvent into a TruthParticleContainer";
  declareProperty( "TruthParticleCnvTool",
		   m_truthParticleCnvTool = CnvTool_t( "TruthParticleCnvTool",
						       this ),
		   descr );
}

/// Destructor
///////////////
McAodFilter::~McAodFilter()
{}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode McAodFilter::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  /// Retrieves a private AlgTool to filter a McEventCollection
  if ( !m_mcVtxFilterTool.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Creation of algTool McVtxFilterTool FAILED !");
    return StatusCode::FAILURE;
  }
  
  /// Retrieves a private AlgTool to convert GenEvent to TruthParticleContainer
  if ( !m_truthParticleCnvTool.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Creation of algTool TruthParticleCnvTool FAILED !");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode McAodFilter::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  m_mcVtxFilterTool->stats();
  return StatusCode::SUCCESS;
}

StatusCode McAodFilter::execute()
{  
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("Executing " << name() << "...");

  if ( m_doGenAod ) {
    sc = m_mcVtxFilterTool->execute();
    if ( !sc.isSuccess() ) {
      ATH_MSG_WARNING ("Problem filtering the McEventCollection !!");
    }
  }

  /// Now create an AOD-compliant container from a McEventCollection
  if ( m_doTruthParticles ) {
    if ( sc.isSuccess() ) {
      sc = m_truthParticleCnvTool->execute();
      if ( !sc.isSuccess() ) {
	ATH_MSG_WARNING
	  ("Could not convert McEventCollection ==> TruthParticles !!");
      }
    } else {
      ATH_MSG_WARNING
	("Could not convert McEventCollection ==> TruthParticles because"\
	 " McEventCollection filtering failed");
    }
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

