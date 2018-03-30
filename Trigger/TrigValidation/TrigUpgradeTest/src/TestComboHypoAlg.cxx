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
      if ( not feature1.isValid() )  {
	ATH_MSG_ERROR( "Can not find reference to the object from the decision1" );
	return false; //StatusCode::FAILURE;
      }
      if ( (*feature1)->hasDetail<float>(m_property1 ) ){
	float v = (*feature1)->getDetail<float>( m_property1 );
	ATH_MSG_DEBUG("Prop1="<<v);
	if ( v < m_threshold1 )
	  return false;
      }
      else ATH_MSG_ERROR( "Cannot find detail "<<m_property1<<" in feature1");
    }
    
    {      
      auto feature2 = d2->objectLink<xAOD::TrigCompositeContainer>( "feature" );
      if ( not feature2.isValid() )  {
	ATH_MSG_ERROR( "Can not find reference to the object from the decision2" );
	return false;//StatusCode::FAILURE;
      }
      if ( (*feature2)->hasDetail<float>(m_property2 ) ){
	float v = (*feature2)->getDetail<float>( m_property2 );
	ATH_MSG_DEBUG("Prop2="<<v);
	if ( v < m_threshold2 )
	  return false;
      } else ATH_MSG_ERROR( "Cannot find detail "<<m_property2<<" in feature2");
    }
    
    return true;    
  }


  // StatusCode TestComboHypoAlg::execute_oninput( int inputCounter, const EventContext& context ) const {
  //   ReadHandle<DecisionContainer> previousDecisionsHandle;
  //   if (inputCounter==0)       previousDecisionsHandle = SG::makeHandle( m_previousDecisions1, context );
  //   else if (inputCounter==1)  previousDecisionsHandle = SG::makeHandle( m_previousDecisions2, context );

  //   if( not previousDecisionsHandle.isValid() ) {//implicit
  //     ATH_MSG_ERROR( "No implicit RH for previous decisions on input "<<inputCounter<<": is this expected?" );
  //     return StatusCode::SUCCESS;      
  //   }
    
  //   ATH_MSG_DEBUG( "Running on input "<<inputCounter<<" with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

  //   ReadHandle<DecisionContainer> recoInput;
  //   if (inputCounter==0)  recoInput = SG::makeHandle(m_recoInput1, context);
  //   else if (inputCounter==1)  recoInput = SG::makeHandle(m_recoInput2, context);
  //   ATH_MSG_DEBUG( "and with "<< recoInput->size() <<" reco inputs");
    
  //   // new output decisions
  //   auto decisions = std::make_unique<DecisionContainer>();
  //   auto aux = std::make_unique<DecisionAuxContainer>();
  //   decisions->setStore( aux.get() );

    
  //   std::vector<const FeatureOBJ*> featureFromDecision;
  //   for ( auto previousDecision: *previousDecisionsHandle ) {     
  //     auto featurelink = (previousDecision)->objectLink<FeatureContainer>( m_linkName.value() );
  //     CHECK( featurelink.isValid() );
  //     const FeatureOBJ* feature = *featurelink;
  //     featureFromDecision.push_back( feature);
  //   }

  //   // reduce if same feature is found
  //   //
    
  //   ATH_MSG_DEBUG("Found "<<featureFromDecision.size()<<" features "<<m_linkName.value() <<" mapped from previous decisions");
    
  //   size_t counter = 0;
  //   //map reco object and decision: find in reco obejct the initial RoI and map it to the correct decision
  //   for (auto recoobj: *recoInput){
  //     auto roiEL = recoobj->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
  //     CHECK( roiEL.isValid() );
  //     auto featurelink = (recoobj)->objectLink<FeatureContainer>( m_linkName.value() );
  //     CHECK( featurelink.isValid() );
  //     if ( not featurelink.isValid() )  {
  // 	ATH_MSG_ERROR( " Can not find reference to " + m_linkName.value() + " from the decision" );
  // 	return StatusCode::FAILURE;
  //     }
      
  //     ATH_MSG_DEBUG("Found link from the reco object to feature "<<m_linkName.value() );
  //     const FeatureOBJ* feature = *featurelink;
  //     // find the same roi in the previous decisions
  //     bool foundRoIInDecision=false;
  //      size_t pos=distance(featureFromDecision.begin(), find(featureFromDecision.begin(), featureFromDecision.end(), feature));
  //      if (pos < featureFromDecision.size()){
  // 	 foundRoIInDecision=true;	 
  //      }
       
  //      if (foundRoIInDecision){
  // 	 ATH_MSG_DEBUG("Found link from the reco object to the previous decision at position "<<pos);
  // 	 auto d = newDecisionIn(decisions.get());
  // 	 d->setObjectLink( "feature", ElementLink<xAOD::TrigCompositeContainer>(m_recoInput.key(), counter) );// feature used by the Tool
  // 	 d->setObjectLink( "initialRoI", featurelink );// this is used by the InputMaker
  // 	 d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisions.key(), pos) );// link to previous decision object
  //      }
  //      else{
  // 	 ATH_MSG_ERROR( " Can not find reference to previous decision from feature " + m_linkName.value() + " from reco object " << counter );
  // 	 return StatusCode::FAILURE;
  //      }
  //      counter++;
  //   }
    

  // }

  StatusCode TestComboHypoAlg::execute_r( const EventContext& context ) const {  

    ATH_MSG_DEBUG ("Executing " << name() << "...");

    auto previousDecisionsHandle1 = SG::makeHandle( m_previousDecisions1, context );
    if( not previousDecisionsHandle1.isValid() ) {//implicit
      ATH_MSG_DEBUG( "No implicit RH for previous decisions1: is this expected?" );
      return StatusCode::SUCCESS;      
    }
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle1->size() <<" implicit " <<previousDecisionsHandle1.key()<<" for previous decisions of input 1");

    auto previousDecisionsHandle2 = SG::makeHandle( m_previousDecisions2, context );
    if( not previousDecisionsHandle2.isValid() ) {//implicit
      ATH_MSG_DEBUG( "No implicit RH for previous decisions2: is this expected?" );
      return StatusCode::SUCCESS;      
    }
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle2->size() <<" implicit "<<previousDecisionsHandle2.key()<<" for previous decisions of input 2");
    
    auto recoInput1 = SG::makeHandle(m_recoInput1);
    auto recoInput2 = SG::makeHandle(m_recoInput2);

    ATH_MSG_DEBUG( "and with "<< recoInput1->size() <<" reco1 inputs, and "<<recoInput2->size() <<" reco2 inputs");
    
    auto decisions1 = std::make_unique<DecisionContainer>();
    auto aux1 = std::make_unique<DecisionAuxContainer>();
    decisions1->setStore( aux1.get() );
    
    auto decisions2 = std::make_unique<DecisionContainer>();
    auto aux2 = std::make_unique<DecisionAuxContainer>();
    decisions2->setStore( aux2.get() );


    // find RoIs from previous decisions
    std::vector<const FeatureOBJ*> featureFromDecision1;
    for ( auto previousDecision: *previousDecisionsHandle1 ) {
      auto featurelink = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
      CHECK( featurelink.isValid() );
      const FeatureOBJ* feature = *featurelink;
      featureFromDecision1.push_back( feature);
    }
    ATH_MSG_DEBUG("Found "<<featureFromDecision1.size()<<" RoIs from decision input1");

   std::vector<const FeatureOBJ*> featureFromDecision2;
    for ( auto previousDecision: *previousDecisionsHandle2 ) {
      auto featurelink = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
      CHECK( featurelink.isValid() );
      const FeatureOBJ* feature = *featurelink;
      featureFromDecision2.push_back( feature);
    }
    ATH_MSG_DEBUG("Found "<<featureFromDecision2.size()<<" RoIs from decision input2");

    //map reco object and decision: find in reco obejct the initial RoI and map it to the correct decision
    size_t counter1 = 0;
    for (auto recoobj: *recoInput1){
      auto featurelink = recoobj->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
      CHECK( featurelink.isValid() );      
      ATH_MSG_DEBUG("Found link from the reco object1 to RoI" );
      const FeatureOBJ* feature = *featurelink;
      // find the same roi in the previous decisions
      bool foundRoIInDecision=false;
       size_t pos=distance(featureFromDecision1.begin(), find(featureFromDecision1.begin(), featureFromDecision1.end(), feature));
       if (pos < featureFromDecision1.size()){
	 foundRoIInDecision=true;	 
       }
       
       if (foundRoIInDecision){
	 ATH_MSG_DEBUG("Found the same RoI on decision at pos "<<pos);
	 auto d = newDecisionIn(decisions1.get());
	 d->setObjectLink( "feature", ElementLink<xAOD::TrigCompositeContainer>(m_recoInput1.key(), counter1) );// feature used by the Tool
	 d->setObjectLink( "initialRoI", featurelink );// this is used by the InputMaker
	 d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisions1.key(), pos) );// link to previous decision object
       }
       else{
	 ATH_MSG_ERROR( " Can not find reference to previous decision from RoI from reco object " << counter1 );
	 return StatusCode::FAILURE;
       }
       counter1++;
    }


    size_t counter2 = 0;
     for (auto recoobj: *recoInput2){
      auto featurelink = recoobj->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
      CHECK( featurelink.isValid() );      
      ATH_MSG_DEBUG("Found link from the reco object2 to RoI" );
      const FeatureOBJ* feature = *featurelink;
      // find the same roi in the previous decisions
      bool foundRoIInDecision=false;
       size_t pos=distance(featureFromDecision2.begin(), find(featureFromDecision2.begin(), featureFromDecision2.end(), feature));
       if (pos < featureFromDecision2.size()){
	 foundRoIInDecision=true;	 
       }
       
       if (foundRoIInDecision){
	 ATH_MSG_DEBUG("Found the same RoI on decision at pos "<<pos);
	 auto d = newDecisionIn(decisions2.get());
	 d->setObjectLink( "feature", ElementLink<xAOD::TrigCompositeContainer>(m_recoInput2.key(), counter2) );// feature used by the Tool
	 d->setObjectLink( "initialRoI", featurelink );// this is used by the InputMaker
	 d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisions2.key(), pos) );// link to previous decision object
       }
       else{
	 ATH_MSG_ERROR( " Can not find reference to previous decision from RoI from reco object " << counter2 );
	 return StatusCode::FAILURE;
       }
       counter2++;
    }
    
    // // pre-recate decision objects for each container
    // size_t counter1 = 0;
    // for ( auto previousDecision: *previousDecisionsHandle1 ) {
    //   auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    //   CHECK( roiEL.isValid() );
    //   auto d = newDecisionIn( decisions1.get() );
    //   if (counter1<input1->size())
    // 	d->setObjectLink( "feature", ElementLink<xAOD::TrigCompositeContainer>( m_recoInput1.key(),  counter1) );
    //   else
    // 	ATH_MSG_DEBUG( "Feature not added to the new decision of type 1: counter =" << counter1<<" list size = "<<input1->size());
    //   d->setObjectLink( "initialRoI", roiEL );
    //   d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisions1.key(), counter1) );
    //   counter1++;
    // }
    ATH_MSG_DEBUG( "Found  "<<counter1<<" rois from input 1 " );
    
    // size_t counter2 = 0;
    // for ( auto previousDecision: *previousDecisionsHandle2 ) {
    //   auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    //   CHECK( roiEL.isValid() );    
    //   auto d = newDecisionIn( decisions2.get() );
    //   //get the feature
    //   if (counter2<input2->size())
    // 	d->setObjectLink( "feature", ElementLink<xAOD::TrigCompositeContainer>( m_recoInput2.key(),  counter2) );
    //   else
    // 	ATH_MSG_DEBUG( "Feature not added to the new decision of type 2");      
    //   d->setObjectLink( "initialRoI", roiEL );// this is used by the InputMaker
    //   d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisions2.key(), counter2) );
    //   counter2++;
    // }
    ATH_MSG_DEBUG( "Found  "<<counter2<<" rois from input 2 " );

    // this is the tool
    if (decisions1->size() ==0 || decisions2->size() == 0)
      ATH_MSG_DEBUG("No combined hypo found, Do not go to the tool since n.dec1="<<  decisions1->size() << " and n.dec2="<< decisions2->size() <<" ");
    else
      ATH_MSG_DEBUG("Go to the tool with n.dec1="<<  decisions1->size() << " and n.dec2="<< decisions2->size() <<" ");

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
    //    ATH_MSG_DEBUG ( "Exit with "<<decisions1->size() <<" decision from input 1 and " <<decisions2->size()<<" form input 2");
    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest
