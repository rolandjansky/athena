///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AddVarTool.cxx
// Implementation file for class AddVarTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "AddVarTool.h"

// STL includes
#include <vector>
#include <string>

// FrameWork includes
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/StackElement.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "ExpressionEvaluation/TriggerDecisionProxyLoader.h"

// EDM includes
#include "AthContainers/AuxElement.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
// #include "xAODBTagging/BTagVertexContainer.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
AddVarTool::AddVarTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent ) :
  ::AthAlgTool  ( type, name, parent ),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_parser(0),
  m_inCollKey(""),
  m_selection(""),
  m_varTypeIndex(-1),
  //m_inContIdx(-1),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IAugmentationTool >(this);

  declareProperty("AddVarTo",          m_inCollKey="",
                  "Name of the container or object where the new variable will be added to" );

  declareProperty("VarName",           m_varName="",   "The name of the new variable" );
  declareProperty("VarType",           m_varType="float",
                  "The type of the new variable (allowed values are: 'bool', 'int', 'float')" );

  declareProperty("Selection",         m_selection="",
                  "The selection string that defines which xAOD::IParticles to select from the container" );
}

// Destructor
///////////////
AddVarTool::~AddVarTool()
{}



// Athena algtool's Hooks
////////////////////////////
StatusCode AddVarTool::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_inCollKey );
  ATH_MSG_DEBUG ( " using = " << m_varName );
  ATH_MSG_DEBUG ( " using = " << m_varType );
  ATH_MSG_DEBUG ( " using = " << m_selection );

  // Try to decode the variable type
  if ( m_varType.value() == "int" ) {
    m_varTypeIndex = 1;
  }
  else if ( m_varType.value() == "bool" ) {
    m_varTypeIndex = 2;
  }
  else if ( m_varType.value() == "float" ) {
    m_varTypeIndex = 11;
  }
  else {
    ATH_MSG_ERROR("Didn't recognize the variable type. "
                  << "Allowed values for the property 'VarType' are: "
                  << "'bool', 'int', 'float'");
    return StatusCode::FAILURE;
  }

  // initialize proxy loaders for expression parsing
  ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
  proxyLoaders->push_back(new ExpressionParsing::TriggerDecisionProxyLoader(m_trigDecisionTool));
  proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
  proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));

  // load the expressions
  m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
  m_parser->loadExpression( m_selection.value() );

  return StatusCode::SUCCESS;
}




StatusCode AddVarTool::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  if (m_parser) {
    delete m_parser;
    m_parser = 0;
  }

  return StatusCode::SUCCESS;
}




#define EVALUATE_CONTAINERTYPE( CONTAINERTYPE )                                                        \
else if ( evtStore()->contains< CONTAINERTYPE >( m_inCollKey.value() ) ) {                             \
  const CONTAINERTYPE* inContainer(0);                                                                 \
  ATH_CHECK( evtStore()->retrieve( inContainer, m_inCollKey.value() ) );                               \
  ATH_MSG_DEBUG ( "Input collection = '" << m_inCollKey.value()                                        \
                  << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );  \
  if ( !inContainer ) {                                                                                \
    ATH_MSG_ERROR( "We don't have a valid pointer to "#CONTAINERTYPE"." );                             \
    return StatusCode::FAILURE;                                                                        \
  }                                                                                                    \
  ExpressionParsing::StackElement selectionResult = m_parser->evaluate();                              \
  if ( !(selectionResult.isVector()) ) {                                                               \
    ATH_MSG_ERROR ("Some unexpected format of the expression parser result. Should be vector-type.");  \
    return StatusCode::FAILURE;                                                                        \
  }                                                                                                    \
  if ( m_varTypeIndex == 1 ) {                                                                         \
    const std::vector<int>& resultVec( selectionResult.vectorValue<int>() );                           \
    if ( resultVec.size() != inContainer->size() ) {                                                   \
      ATH_MSG_ERROR("The result vector doesn't have the same size as the container to decorate!");     \
      return StatusCode::FAILURE;                                                                      \
    }                                                                                                  \
    SG::AuxElement::Decorator<int> intDeco (m_varName.value());                                                \
    for ( std::size_t i=0; i<resultVec.size(); ++i ) {                                                 \
      ATH_MSG_VERBOSE("Got a container integer result: " << resultVec.at(i) );                         \
      const auto* part = inContainer->at(i);                                                           \
      intDeco(*part) = resultVec.at(i);                                                                \
    }                                                                                                  \
  }                                                                                                    \
  else if ( m_varTypeIndex == 2 ) {                                                                    \
    const std::vector<int>& resultVec( selectionResult.vectorValue<int>() );                           \
    if ( resultVec.size() != inContainer->size() ) {                                                   \
      ATH_MSG_ERROR("The result vector doesn't have the same size as the container to decorate!");     \
      return StatusCode::FAILURE;                                                                      \
    }                                                                                                  \
    SG::AuxElement::Decorator<bool> boolDeco (m_varName.value());                                              \
    for ( std::size_t i=0; i<resultVec.size(); ++i ) {                                                 \
      ATH_MSG_VERBOSE("Got a container boolean result: " << static_cast<bool>(resultVec.at(i)) );      \
      const auto* part = inContainer->at(i);                                                           \
      boolDeco(*part) = static_cast<bool>(resultVec.at(i));                                            \
    }                                                                                                  \
  }                                                                                                    \
  else if ( m_varTypeIndex == 11 ) {                                                                   \
    const std::vector<double>& resultVec( selectionResult.vectorValue<double>() );                     \
    if ( resultVec.size() != inContainer->size() ) {                                                   \
      ATH_MSG_ERROR("The result vector doesn't have the same size as the container to decorate!");     \
      return StatusCode::FAILURE;                                                                      \
    }                                                                                                  \
    SG::AuxElement::Decorator<float> floatDeco (m_varName.value());                                            \
    for ( std::size_t i=0; i<resultVec.size(); ++i ) {                                                 \
      ATH_MSG_VERBOSE("Got a container float result: " << static_cast<float>(resultVec.at(i)) );       \
      const auto* part = inContainer->at(i);                                                           \
      floatDeco(*part) = static_cast<float>(resultVec.at(i));                                          \
    }                                                                                                  \
  }                                                                                                    \
}



StatusCode AddVarTool::addBranches() const
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> addBranches " << name() << " on " << m_nEventsProcessed << ". event..." );


  //-----------------------------------------
  // Get the input container where we want to add the variable to
  //-----------------------------------------
  const SG::AuxElement* inAuxElement(0);

  // if ( evtStore()->contains< SG::AuxElement >( m_inCollKey.value() ) ) {
  //   // This file holds an SG::AuxElement
  //   ATH_CHECK( evtStore()->retrieve( inAuxElement, m_inCollKey.value() ) );
  //   ATH_MSG_DEBUG ( "Input SG::AuxElement = '" << m_inCollKey.value() << "' retrieved from StoreGate" );
  // }
  if ( evtStore()->contains< xAOD::EventInfo >( m_inCollKey.value() ) ) {
    // This file holds an xAOD::EventInfo
    const xAOD::EventInfo* eventInfo;
    ATH_CHECK( evtStore()->retrieve( eventInfo, m_inCollKey.value() ) );
    ATH_MSG_DEBUG ( "Input xAOD::EventInfo = '" << m_inCollKey.value() << "' retrieved from StoreGate" );
    inAuxElement = dynamic_cast<const xAOD::EventInfo*>(eventInfo);
    ATH_CHECK( this->evaluateAuxElement(inAuxElement) );
  }

  // Use a pre-processor macro do generate the code for all container types,
  // i.e., DataVector< SomeObjectInheritingFrom : public SG::AuxElement >
  EVALUATE_CONTAINERTYPE(xAOD::IParticleContainer)
  EVALUATE_CONTAINERTYPE(xAOD::VertexContainer)
  //EVALUATE_CONTAINERTYPE(xAOD::BTagVertexContainer)
  EVALUATE_CONTAINERTYPE(xAOD::BTaggingContainer)
  //EVALUATE_CONTAINERTYPE(xAOD::MissingETComponentMap)
  EVALUATE_CONTAINERTYPE(xAOD::MissingETContainer)
  EVALUATE_CONTAINERTYPE(xAOD::TruthEventContainer)
  EVALUATE_CONTAINERTYPE(xAOD::TruthVertexContainer)

  else {
    if ( m_nEventsProcessed <= 10 ) {
      ATH_MSG_WARNING ( "Input collection  = '" << m_inCollKey.value()
                        << "' could not be retrieved from StoreGate! "
                        << " This message will only be repeated 10 times..." );
    }
    else {
      ATH_MSG_DEBUG ( "Input collection  = '" << m_inCollKey.value()
                      << "' could not be retrieved from StoreGate! " );
    }
    return StatusCode::SUCCESS;
  } // End: if/elif/else on input container type


  return StatusCode::SUCCESS;
}




StatusCode AddVarTool::evaluateAuxElement( const SG::AuxElement* inAuxElement ) const
{
  if ( !inAuxElement ) {
    ATH_MSG_ERROR( "We don't have a valid pointer to SG::AuxElement" );
    return StatusCode::FAILURE;
  }

  // Evaluate the string expression for this event
  ExpressionParsing::StackElement selectionResult = m_parser->evaluate();
  if ( !(selectionResult.isScalar()) ) {
    ATH_MSG_ERROR ("Some unexpected format of the expression parser result. Should be scalar-type.");
    return StatusCode::FAILURE;
  }

  // Now, get the result and record it to the object
  if ( m_varTypeIndex == 1 ) { // This is an integer
    int result = selectionResult.scalarValue<int>();
    ATH_MSG_VERBOSE("Got an integer result: " << result );
    inAuxElement->auxdecor<int> (m_varName.value()) = result;
  }
  else if ( m_varTypeIndex == 2 ) { // This is a boolean
    bool result = selectionResult.scalarValue<bool>();
    ATH_MSG_VERBOSE("Got a boolean result: " << result );
    inAuxElement->auxdecor<bool> (m_varName.value()) = result;
  }
  else if ( m_varTypeIndex == 11 ) { // This is a float
    float result = selectionResult.scalarValue<float>();
    ATH_MSG_VERBOSE("Got a float result: " << result );
    inAuxElement->auxdecor<float> (m_varName.value()) = result;
  }

  return StatusCode::SUCCESS;
}
