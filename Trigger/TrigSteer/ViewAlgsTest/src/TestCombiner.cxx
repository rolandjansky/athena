///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TestCombiner.cxx 
// Implementation file for class TestCombiner
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// ViewAlgsTest includes
#include "TestCombiner.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TestCombiner::TestCombiner( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), 
  m_inputA("InputA"), 
  m_inputB("InputB"),
  m_output("Output"), 
  m_outputAux("OutputAux.")  
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
TestCombiner::~TestCombiner()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode TestCombiner::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");


  return StatusCode::SUCCESS;
}

StatusCode TestCombiner::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TestCombiner::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  CHECK( m_inputA.isValid() );
  CHECK( m_inputB.isValid() );
  ATH_MSG_DEBUG("A size "<< m_inputA->size());
  ATH_MSG_DEBUG("B size "<< m_inputB->size());
  m_output = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_output->setStore(m_outputAux.ptr());

  for ( auto objA: *m_inputA.cptr() ) {
    xAOD::TrigComposite* o  = new xAOD::TrigComposite(); 
    m_output->push_back(o);    
    o->setDetail("et", 3.34f);
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


