/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// DecisionHandling includes


#include "DecisionHandling/HLTIdentifier.h"

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "DumpDecisions.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DumpDecisions::DumpDecisions( const std::string& name, 
			      ISvcLocator* pSvcLocator )
  : AthReentrantAlgorithm( name, pSvcLocator )
{
  declareProperty( "Decisions", m_decisionKey, "Input Decisions" );
  declareProperty( "VerbosityLevel", m_verbosityLevel, "3 - tries to print as much possible, 2 - only list of objects and their decisions, 1 - only list of active objets" );
}

// Destructor
///////////////
DumpDecisions::~DumpDecisions()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DumpDecisions::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  CHECK( m_decisionKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode DumpDecisions::finalize() {
  ATH_MSG_INFO ( "Finalizing " << name() << "..." );

  return StatusCode::SUCCESS;
}

StatusCode DumpDecisions:: execute_r( const EventContext& ctx ) const {  
  using namespace TrigCompositeUtils;
  //  DecisionInput decisionInput;
  auto decisionInput = SG::makeHandle( m_decisionKey, ctx );
  //  CHECK( decisionInput.retrieve()  );
  //  CHECK ( decisionInput.retrieve( m_decisionKey, ctx ) );
  ATH_MSG_DEBUG( "Retrieved decision with the key " << m_decisionKey.key() );
  ATH_MSG_DEBUG( "Pointer value " << decisionInput.cptr() );
  for ( auto d: *decisionInput ) {
    DecisionIDContainer ids;
    TrigCompositeUtils::decisionIDs( d, ids );
    ATH_MSG_DEBUG( "Decision object with " << ids.size() << " decisions" );
    if ( m_verbosityLevel >= 2 ) {
      for ( auto id: ids ) {
	ATH_MSG_DEBUG( "Passing decision " << HLT::Identifier( id ) );
      }      
    }
  }
  return StatusCode::SUCCESS;
}
  

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


