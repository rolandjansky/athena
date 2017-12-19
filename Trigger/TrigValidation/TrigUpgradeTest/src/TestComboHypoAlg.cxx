/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "TestComboHypoAlg.h"



namespace HLTTest {

TestComboHypoAlg::TestComboHypoAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ) {
  //declareProperty( "Property", m_nProperty );
  declareProperty( "Input1", m_recoInput1 ); 
  declareProperty( "Input2", m_recoInput2 );
  declareProperty( "Property1", m_property1 );
  declareProperty( "Property2", m_property2 );
  declareProperty( "Threshold1", m_threshold1 = 0);
  declareProperty( "Threshold2", m_threshold2 = 0);
  declareProperty( "Output1", m_output1 );
  declareProperty( "Output2", m_output2 );
  declareProperty( "DecisionLabel", m_decisionLabel );
}

TestComboHypoAlg::~TestComboHypoAlg() {}

StatusCode TestComboHypoAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  CHECK( m_recoInput1.initialize() );
  CHECK( m_recoInput2.initialize() );
  CHECK( m_output1.initialize() );
  CHECK( m_output2.initialize() );  
  return StatusCode::SUCCESS;
}

StatusCode TestComboHypoAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

  bool TestComboHypoAlg::passed( const Decision* d1, const Decision* d2 ) const {
    {
      auto feature1 = d1->objectLink<xAOD::TrigCompositeContainer>( "feature" );    
      float v = (*feature1)->getDetail<float>( m_property1 );
      if ( v < m_threshold1 )
	return false;
    }
    {
      auto feature2 = d2->objectLink<xAOD::TrigCompositeContainer>( "feature" );    
      float v = (*feature2)->getDetail<float>( m_property2 );
      if ( v < m_threshold2 )
	return false;
    }
    return true;    
  }

  
StatusCode TestComboHypoAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  auto input1 = SG::makeHandle(m_recoInput1);
  auto input2 = SG::makeHandle(m_recoInput2);
  
  auto decisions1 = std::make_unique<DecisionContainer>();
  auto aux1 = std::make_unique<DecisionAuxContainer>();
  decisions1->setStore( aux1.get() );

  auto decisions2 = std::make_unique<DecisionContainer>();
  auto aux2 = std::make_unique<DecisionAuxContainer>();
  decisions2->setStore( aux2.get() );

  // pre-recate decision objects for each container
  size_t counter1 = 0;
  for ( auto obj1Iter = input1->begin(); obj1Iter != input1->end(); ++obj1Iter, ++counter1 ) {
    auto d = newDecisionIn( decisions1.get() );
    d->setObjectLink("feature", ElementLink<xAOD::TrigCompositeContainer>( m_recoInput1.key(),  counter1) );
  }
  
  size_t counter2 = 0;
  for ( auto obj2Iter = input2->begin(); obj2Iter != input2->end(); ++obj2Iter, ++counter2 ) {
    auto d = newDecisionIn( decisions2.get() );
    d->setObjectLink("feature", ElementLink<xAOD::TrigCompositeContainer>( m_recoInput2.key(),  counter2) );
  }

  
  counter1 = 0;
  for ( auto obj1Iter = decisions1->begin(); obj1Iter != decisions1->end(); ++obj1Iter, ++counter1 ) {
    counter2 = 0;
    for ( auto obj2Iter = decisions2->begin(); obj2Iter != decisions2->end(); ++obj2Iter, ++counter2 ) {
      if (  passed(*obj1Iter, *obj2Iter)  ) {
	auto did = HLT::Identifier( m_decisionLabel ).numeric();	
	addDecisionID( did,  decisions1.get()->at(counter1) );
	addDecisionID( did,  decisions2.get()->at(counter2) ); 
      }
    }
  }

  {
    auto h = SG::makeHandle( m_output1 );
    CHECK( h.record( std::move( decisions1 ) , std::move( aux1 ) ) );
  }
  {
    auto h = SG::makeHandle( m_output2 );
    CHECK( h.record( std::move( decisions2 ) , std::move( aux2 ) ) );
  }

  return StatusCode::SUCCESS;
}

} //> end namespace HLTTest
