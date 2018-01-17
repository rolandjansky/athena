/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "TestComboHypoAlg.h"



namespace HLTTest {

TestComboHypoAlg::TestComboHypoAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )  {
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
  CHECK( m_previousDecisions1.initialize() );
  renounce(m_previousDecisions1);
  CHECK( m_previousDecisions2.initialize() );
  renounce(m_previousDecisions2);

  CHECK( not m_chainsProperty.empty() );
  
  for ( const std::string& el: m_chainsProperty ) 
    m_chains.insert( HLT::Identifier( el ).numeric() );
  
  for ( const HLT::Identifier& id: m_chains )
    ATH_MSG_DEBUG( "Configured to require chain " << id );
  
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
      ATH_MSG_DEBUG("Prop1="<<v);
      if ( v < m_threshold1 )
	return false;
    }
    {
      auto feature2 = d2->objectLink<xAOD::TrigCompositeContainer>( "feature" );    
      float v = (*feature2)->getDetail<float>( m_property2 );
      ATH_MSG_DEBUG("Prop2="<<v);
      if ( v < m_threshold2 )
	return false;
    }
    return true;    
  }


  StatusCode TestComboHypoAlg::execute_r( const EventContext& context ) const {  

    ATH_MSG_DEBUG ("Executing " << name() << "...");

    auto previousDecisionsHandle1 = SG::makeHandle( m_previousDecisions1, context );
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle1->size() <<" implicit " <<previousDecisionsHandle1.key()<<" for previous decisions of input 1");

    auto previousDecisionsHandle2 = SG::makeHandle( m_previousDecisions2, context );
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle2->size() <<" implicit "<<previousDecisionsHandle2.key()<<" for previous decisions of input 2");
    
    auto input1 = SG::makeHandle(m_recoInput1);
    auto input2 = SG::makeHandle(m_recoInput2);

    ATH_MSG_DEBUG( "and with "<< input1->size() <<" reco1 inputs, and "<<input2->size() <<" reco2 inputs");
    
    auto decisions1 = std::make_unique<DecisionContainer>();
    auto aux1 = std::make_unique<DecisionAuxContainer>();
    decisions1->setStore( aux1.get() );
    
    auto decisions2 = std::make_unique<DecisionContainer>();
    auto aux2 = std::make_unique<DecisionAuxContainer>();
    decisions2->setStore( aux2.get() );
    
    // pre-recate decision objects for each container
    size_t counter1 = 0;
    for ( auto previousDecision: *previousDecisionsHandle1 ) {
      auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
      CHECK( roiEL.isValid() );    
      //    for ( auto obj1Iter = input1->begin(); obj1Iter != input1->end(); ++obj1Iter, ++counter1 ) {
      auto d = newDecisionIn( decisions1.get() );
      d->setObjectLink( "feature", ElementLink<xAOD::TrigCompositeContainer>( m_recoInput1.key(),  counter1) );
      d->setObjectLink( "initialRoI", roiEL );// this is used by the InputMaker
      d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisions1.key(), counter1) );
    }
    
    size_t counter2 = 0;
    for ( auto previousDecision: *previousDecisionsHandle2 ) {
      auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
      CHECK( roiEL.isValid() );    
      //    for ( auto obj2Iter = input2->begin(); obj2Iter != input2->end(); ++obj2Iter, ++counter2 ) {
      auto d = newDecisionIn( decisions2.get() );
      d->setObjectLink("feature", ElementLink<xAOD::TrigCompositeContainer>( m_recoInput2.key(),  counter2) );
      d->setObjectLink( "initialRoI", roiEL );// this is used by the InputMaker
      d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisions2.key(), counter2) );
    }
    
    // this is the tool
    counter1 = 0;
    for ( auto obj1Iter = decisions1->begin(); obj1Iter != decisions1->end(); ++obj1Iter, ++counter1 ) {
      auto previousDecisions1 = (*obj1Iter)->objectLink<DecisionContainer>( "previousDecisions" ); 
      TrigCompositeUtils::DecisionIDContainer objDecisions1;      
      TrigCompositeUtils::decisionIDs( *previousDecisions1, objDecisions1 );
      
      ATH_MSG_DEBUG("Number of previous decisions for input1: " << objDecisions1.size() );
      
      for ( TrigCompositeUtils::DecisionID id : objDecisions1 ) {
	ATH_MSG_DEBUG( " -- found decision " << HLT::Identifier( id ) );
      }
      
      std::vector<TrigCompositeUtils::DecisionID> intersection1;
      std::set_intersection( m_chains.begin(), m_chains.end(),
			   objDecisions1.begin(), objDecisions1.end(),
			   std::back_inserter( intersection1 ) );
    
      if ( not intersection1.empty() ) {

	for ( const HLT::Identifier& did: intersection1 ){
	  ATH_MSG_DEBUG("Found "<<did<<" to candidate "<< counter1 << " in input1");
	}
	counter2 = 0;
	for ( auto obj2Iter = decisions2->begin(); obj2Iter != decisions2->end(); ++obj2Iter, ++counter2 ) {
	  auto previousDecisions2 = (*obj2Iter)->objectLink<DecisionContainer>( "previousDecisions" ); 
	  TrigCompositeUtils::DecisionIDContainer objDecisions2;      
	  TrigCompositeUtils::decisionIDs( *previousDecisions2, objDecisions2 );
      
	  ATH_MSG_DEBUG("Number of previous decisions for input2: " << objDecisions2.size() );
      
	  for ( TrigCompositeUtils::DecisionID id : objDecisions2 ) {
	    ATH_MSG_DEBUG( " -- found decision " << HLT::Identifier( id ) );
	  }

	  // check that the same chain is in this decisions 
	  std::vector<TrigCompositeUtils::DecisionID> intersection2;
	  std::set_intersection( objDecisions1.begin(), objDecisions1.end(),
				 objDecisions2.begin(), objDecisions2.end(),
				 std::back_inserter( intersection2 ) );
    
	  if ( not intersection2.empty() ) {
	    for ( const HLT::Identifier& did: intersection2 ){
	      ATH_MSG_DEBUG("Found "<<did<<" to both candidates for "<< counter2 << " in input2");
	    }
	    ATH_MSG_DEBUG("Found good combined candidate");
	    

	    if (  passed(*obj1Iter, *obj2Iter)  ) {
	      for ( const HLT::Identifier& did: intersection2 ){
		//	      auto did = HLT::Identifier( m_decisionLabel ).numeric();
		ATH_MSG_DEBUG("Adding "<<did<<" to both candaites");
		addDecisionID( did,  decisions1.get()->at(counter1) );
		addDecisionID( did,  decisions2.get()->at(counter2) );
	      }
	    }
	    
	  }
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
