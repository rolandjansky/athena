/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthParticleFilterBaseTool.cxx 
// Implementation file for class TruthParticleFilterBaseTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes

// CLHEP/HepMC includes
#include "GeneratorObjects/McEventCollection.h"

// McParticleKernel includes
#include "McParticleKernel/IMcVtxFilterTool.h"
#include "McParticleKernel/ITruthIsolationTool.h"

// McParticleTools includes
#include "TruthParticleFilterBaseTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
TruthParticleFilterBaseTool::TruthParticleFilterBaseTool( const std::string& type, 
                                                          const std::string& name, 
                                                          const IInterface* parent ) : 
  AthAlgTool( type, name, parent )
{
  //
  // Property declaration
  // 

  declareProperty( "McEvents",       
		   m_mcEventsName = "TruthEvent",
		   "Name of the input McEventCollection we want to filter" );

  declareProperty( "McEventsOutput", 
		   m_mcEventsOutputName = "GEN_AOD",
		   "Name of the output McEventCollection which has been filtered" );

  declareProperty( "McVtxFilterTool",
		   m_mcVtxFilterTool = McVtxFilterTool_t( "McVtxFilterTool",
							  this ),
		   "Pointer to the McVtxFilterTool to be able to select additional "
		   "vertices on some decay pattern(s) criterion(s)" );

  declareProperty( "TruthIsolationTool",
		   m_isolationTool = IsolTool_t( "TruthIsolationTool" ),
		   "Pointer to the TruthIsolationTool to be able to compute "
		   "transverse energy isolations for various isolation cones cuts. " 
		   "See McParticleEvent/TruthParticleParameters.h for cone cuts." );


  declareProperty( "DoEtIsolations",
		   m_doEtIsolation = false,
		   "Switch to compute or not the Et-isolations for TruthParticle "
		   "(and their underlying @c HepMC::GenParticle).\n" 
		   "Default is to not compute these Et-isolations (and save CPU)." );

  declareInterface<ITruthParticleFilterTool>(this);
}

/// Destructor
///////////////
TruthParticleFilterBaseTool::~TruthParticleFilterBaseTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode TruthParticleFilterBaseTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  /// Retrieves a private AlgTool to filter a McEventCollection
  if ( !m_mcVtxFilterTool.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Creation of algTool IMcVtxFilterTool FAILED !");
    return StatusCode::FAILURE;
  }

  /// Retrieves the isolation tool (public, to be used also in TruthParticleCnvTool)
  if( m_doEtIsolation.value() && !m_isolationTool.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Creation of TruthIsolationTool FAILED !");
    return StatusCode::FAILURE;    
  }

  ATH_MSG_INFO
    (" DoEtIsolations: [" << std::boolalpha << m_doEtIsolation.value()
     << "]" << endreq
     << " McEvents:       [" << m_mcEventsName.value() << "]" << endreq
     << " McEventsOutput: [" << m_mcEventsOutputName.value() << "]");
  
  // Give the concrete (derived) tool a chance to initialize itself
  if ( initializeTool().isFailure() ) {
    ATH_MSG_ERROR("Could not initialize concrete tool !");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Options of the McVtxFilterTool:");
  m_mcVtxFilterTool->displayOptions();

  return StatusCode::SUCCESS;
}

StatusCode TruthParticleFilterBaseTool::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");

  m_mcVtxFilterTool->stats();

  // Give the concrete (derived) tool a chance to finalize itself
  if ( finalizeTool().isFailure() ) {
    ATH_MSG_ERROR("Could not finalize concrete tool !");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TruthParticleFilterBaseTool::execute()
{  
  ATH_MSG_DEBUG("Executing " << name() << "...");

  const McEventCollection * mcEvent = 0;
  if ( evtStore()->retrieve( mcEvent, m_mcEventsName.value() ).isFailure() ||
       0 == mcEvent ) {
    ATH_MSG_ERROR("Could not retrieve McEventCollection at ["
                  << m_mcEventsName.value() << "] !!");
    return StatusCode::FAILURE;
  }

  McEventCollection * filterMcEvent = new McEventCollection;
  if ( evtStore()->record( filterMcEvent, 
                           m_mcEventsOutputName.value() ).isFailure() ) {
    ATH_MSG_ERROR("Could not record McEventCollection at ["
                  << m_mcEventsOutputName.value() << "] !!");
    delete filterMcEvent;
    filterMcEvent = 0;
    return StatusCode::FAILURE;
  }

  if ( evtStore()->setConst( filterMcEvent ).isFailure() ) {
    ATH_MSG_WARNING("Could not setConst McEventCollection at ["
		    << m_mcEventsOutputName << "] !!");
  }

  // Compute isolation for gamma/lepton.
  if ( m_doEtIsolation.value() ) {
    ATH_MSG_VERBOSE("Computing Et isolations...");
    if ( m_isolationTool->buildEtIsolations(m_mcEventsName.value()).isFailure() ) {
      ATH_MSG_ERROR("Could not compute Et isolations !!");
      return StatusCode::FAILURE;
    }
  } //> end do Et-isolation

  if ( this->buildMcAod( mcEvent, filterMcEvent ).isFailure() ) {
    ATH_MSG_ERROR("Could not buildMcAod(in,out) !!");
    return StatusCode::FAILURE;
  }

  // We have slimmed the filterMcEvent.
  // To not bias the map of isolation energies for this GenEvent, we alias
  // it to its original one
  if ( m_doEtIsolation.value() &&
       !m_isolationTool->registerAlias( m_mcEventsOutputName.value(), 
                                        m_mcEventsName.value() 
                                        ).isSuccess() ) {
    ATH_MSG_WARNING("Could not create an alias in the map of "\
		    "isolation energies !");
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

