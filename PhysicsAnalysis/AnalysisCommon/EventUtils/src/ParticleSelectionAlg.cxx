///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSelectionAlg.cxx
// Implementation file for class ParticleSelectionAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ParticleSelectionAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/StackElement.h"
// #include "TrigDecisionTool/TrigDecisionTool.h"
// #include "ExpressionEvaluation/TriggerDecisionProxyLoader.h"

// EDM includes
#include "EventInfo/EventStreamInfo.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODTracking/NeutralParticleContainer.h"
#include "xAODTracking/NeutralParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/ParticleAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/CompositeParticleAuxContainer.h"
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"


///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ParticleSelectionAlg::ParticleSelectionAlg( const std::string& name,
                                            ISvcLocator* pSvcLocator ) :
  ::AthAnalysisAlgorithm( name, pSvcLocator ),
  m_evtInfoName("EventInfo"),
  m_inPrimVtxCont("PrimaryVertices"),
  m_inCollKey(""),
  m_outCollKey(""),
  m_writeSplitAux(true),
  m_outOwnPolicyName("VIEW_ELEMENTS"),
  m_selection(""),
  m_doCutFlow(false),
  m_cutBKCName(name),
  m_parser(nullptr),
  // m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_nEventsProcessed(0),
  m_outOwnPolicy(SG::VIEW_ELEMENTS),
  m_contType(UNKNOWN),
  m_cutBKStartIdx(0),
  m_selToolIdxOffset(),
  m_selWPVToolIdxOffset(),
  m_idxSelParster(0)
{
  declareProperty("EventInfo",                 m_evtInfoName,   "Input container name");
  declareProperty("PrimaryVertexContainer",    m_inPrimVtxCont, "The input primary vertex container name");
  declareProperty("InputContainer",            m_inCollKey,     "Input container name");
  declareProperty("OutputContainer",           m_outCollKey,
                  "The name of the output container with the deep copy of selected xAOD::IParticles");
  declareProperty("WriteSplitOutputContainer", m_writeSplitAux,
                  "Decide if we want to write a fully-split AuxContainer such that we can remove any variables");
  declareProperty("OutputContainerOwnershipPolicy", m_outOwnPolicyName,
                  "Defines the ownership policy of the output container (default: 'OWN_ELEMENTS'; also allowed: 'VIEW_ELEMENTS')");

  declareProperty("Selection",               m_selection,
                  "The selection string that defines which xAOD::IParticles to select from the container");

  declareProperty("DoCutBookkeeping",       m_doCutFlow,
                  "If true (deault: false), do the bookkeeping of how many particles passed which selection cuts");
  declareProperty("CutBookkeeperContainer", m_cutBKCName,
                  "The name of the resulting xAOD::CutBookkeeperContainer");
}



// Destructor
///////////////
ParticleSelectionAlg::~ParticleSelectionAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode ParticleSelectionAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_inCollKey );
  ATH_MSG_DEBUG ( " using = " << m_outCollKey );
  ATH_MSG_DEBUG ( " using = " << m_writeSplitAux );
  ATH_MSG_DEBUG ( " using = " << m_outOwnPolicyName );
  ATH_MSG_DEBUG ( " using = " << m_selection );
  ATH_MSG_DEBUG ( " using DoCutBookkeeping = " << m_doCutFlow );
  ATH_MSG_DEBUG ( " using = " << m_cutBKCName );

  // initialize proxy loaders for expression parsing
  if ( !(m_selection.value().empty()) ){
    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
    // proxyLoaders->push_back(new ExpressionParsing::TriggerDecisionProxyLoader(m_trigDecisionTool));
    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
    // load the expressions
    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
    m_parser->loadExpression( m_selection.value() );
  }

  // initialize the counters
  m_nEventsProcessed = 0;

  // Determine the ownership policy of the output container
  if ( m_outOwnPolicyName.value() == "OWN_ELEMENTS" ) {
    m_outOwnPolicy = SG::OWN_ELEMENTS;
  }
  else if ( m_outOwnPolicyName.value() == "VIEW_ELEMENTS" ) {
    m_outOwnPolicy = SG::VIEW_ELEMENTS;
  }
  else {
    ATH_MSG_ERROR("Unrecognized ownership policy for the output container: " << m_outOwnPolicyName );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
  return StatusCode::SUCCESS;
}



StatusCode ParticleSelectionAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Clean up the memory
  if (m_parser) {
    delete m_parser;
    m_parser = 0;
  }

  return StatusCode::SUCCESS;
}



StatusCode ParticleSelectionAlg::beginRun()
{
  ATH_MSG_DEBUG ("BeginRun " << name() << "...");

  // Nothing to be done here, if cut-flow bookkeeping was not requested
  if (!m_doCutFlow){ return StatusCode::SUCCESS; }

  // Get some properties from the input meta data
  std::string inputStreamName = "Stream";
  const CLID clid = ClassID_traits<EventStreamInfo>::ID();
  std::vector<std::string> streamNameList;
  inputMetaStore()->keys(clid, streamNameList);
  if ( streamNameList.size() >=1 ){ inputStreamName = streamNameList[0]; }
  // Get the processing cycle number that we need to set for us
  int maxInputCycle = -1;
  if ( inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_cutBKCName.value()) ){
    xAOD::CutBookkeeperContainer* inCutBKCont = nullptr;
    ATH_CHECK( inputMetaStore()->retrieve(inCutBKCont, m_cutBKCName.value() ) );
    if (inCutBKCont){
      maxInputCycle = inCutBKCont->maxCycle();
    }
  }
  if (maxInputCycle<0){ maxInputCycle = 0; }
  else { maxInputCycle += 1; }

  // Check if we already have a container in the output meta-data store
  xAOD::CutBookkeeperContainer* cutBKCont = nullptr;
  if ( outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_cutBKCName.value()) ){
    ATH_CHECK( inputMetaStore()->retrieve(cutBKCont, m_cutBKCName.value() ) );
    // Set the index where we will start having our CutBookkeepers in this container
    m_cutBKStartIdx = cutBKCont->size();
  }
  else {
    // Create and register the container that will hold the cut-flow information
    cutBKCont = new xAOD::CutBookkeeperContainer();
    // Take care of the peculiarities of the new xAOD EDM, i.e., create the needed AuxStore
    xAOD::CutBookkeeperAuxContainer* cutBKAuxCont = new xAOD::CutBookkeeperAuxContainer();
    cutBKCont->setStore( cutBKAuxCont ); //gives it a new associated aux container
    ATH_CHECK( outputMetaStore()->record( cutBKCont, m_cutBKCName.value() ) );
    ATH_CHECK( outputMetaStore()->record( cutBKAuxCont, m_cutBKCName.value()+"Aux." ) );
    ATH_MSG_VERBOSE( "Recorded xAOD::CutBookkeeperContainer " << m_cutBKCName.value() << "Aux." );
  }

  //------------- for the ExpressionParsing in this algorithm --------------
  if ( !(m_selection.value().empty()) ){
    // Create a new xAOD::CutBookkeeper and add it to the container
    xAOD::CutBookkeeper* cutBK = new xAOD::CutBookkeeper();
    cutBKCont->push_back(cutBK);
    // Now, set its properties
    cutBK->setName(this->name());
    cutBK->setDescription(m_selection.value());
    cutBK->setCutLogic(xAOD::CutBookkeeper::CutLogic::REQUIRE); // Each cut must be passed, thus REQUIRE, i.e, logical AND
    cutBK->setInputStream(inputStreamName);
    cutBK->setCycle(maxInputCycle);
    m_idxSelParster = cutBK->index();
  }

  return StatusCode::SUCCESS;
}




StatusCode ParticleSelectionAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );


  // Figure out what type the input container has, if we didn't do it yet
  if ( m_contType == UNKNOWN ){
    if ( evtStore()->contains<xAOD::PhotonContainer>(m_inCollKey.value()) ){ m_contType = PHOTON; }
    else if ( evtStore()->contains<xAOD::ElectronContainer>(m_inCollKey.value()) ){ m_contType = ELECTRON; }
    else if ( evtStore()->contains<xAOD::MuonContainer>(m_inCollKey.value()) ){ m_contType = MUON; }
    else if ( evtStore()->contains<xAOD::TauJetContainer>(m_inCollKey.value()) ){ m_contType = TAU; }
    else if ( evtStore()->contains<xAOD::JetContainer>(m_inCollKey.value()) ){ m_contType = JET; }
    else if ( evtStore()->contains<xAOD::TruthParticleContainer>(m_inCollKey.value()) ){ m_contType = TRUTHPARTICLE; }
    else if ( evtStore()->contains<xAOD::CompositeParticleContainer>(m_inCollKey.value()) ){ m_contType = COMPOSITEPARTICLE; }
    else if ( evtStore()->contains<xAOD::PFOContainer>(m_inCollKey.value()) ){ m_contType = PARITCLEFLOW; }
    else if ( evtStore()->contains<xAOD::NeutralParticleContainer>(m_inCollKey.value()) ){ m_contType = NEUTRALPARTICLE; }
    else if ( evtStore()->contains<xAOD::TrackParticleContainer>(m_inCollKey.value()) ){ m_contType = TRACKPARTICLE; }
    else if ( evtStore()->contains<xAOD::ParticleContainer>(m_inCollKey.value()) ){ m_contType = PARTICLE; }
    else if ( evtStore()->contains<xAOD::CaloClusterContainer>(m_inCollKey.value()) ){ m_contType = CALOCLUSTER; }
  }
  if ( m_contType == UNKNOWN ){
    ATH_MSG_FATAL("We couldn't determine the type of the container... abort!");
    return StatusCode::FAILURE;
  }

  // Get the input container and create the output containers
  const xAOD::IParticleContainer* inContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve( inContainer, m_inCollKey.value() ) );
  ATH_MSG_DEBUG ( "Input collection = " << m_inCollKey.value()
                  << " retrieved from StoreGate which has " << inContainer->size() << " entries." );

  // --------------------------------------------------------------------------
  // Do the expression parsing once per event already here
  // --------------------------------------------------------------------------
  std::vector<int> resultVec;
  resultVec.reserve(inContainer->size());
  if ( !(m_selection.value().empty()) ){
    resultVec = m_parser->evaluateAsVector();
    // Check that the lengths of the input container and the result vector are the same
    if ( inContainer && inContainer->size() != resultVec.size() ) {
      ATH_MSG_ERROR("We got an input container, but its size (" << inContainer->size()
                    << ") doesn't match the size of the result vector: " << resultVec.size() );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Have a container of size " << inContainer->size()
                    << " and a result vector of size " << resultVec.size() );
  } // End: If expression parsing was requested in the first place


  // --------------------------------------------------------------------------
  // Do the heavy lifting of actually creating the new and reduced output container(s)
  // --------------------------------------------------------------------------
  if ( m_contType == PHOTON ){
    ATH_CHECK( (this->selectParticles<xAOD::PhotonContainer,xAOD::PhotonAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == ELECTRON ){
    ATH_CHECK( (this->selectParticles<xAOD::ElectronContainer,xAOD::ElectronAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == MUON ){
    ATH_CHECK( (this->selectParticles<xAOD::MuonContainer,xAOD::MuonAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == TAU ){
    ATH_CHECK( (this->selectParticles<xAOD::TauJetContainer,xAOD::TauJetAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == JET ){
    ATH_CHECK( (this->selectParticles<xAOD::JetContainer,xAOD::JetAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == TRUTHPARTICLE ){
    ATH_CHECK( (this->selectParticles<xAOD::TruthParticleContainer,xAOD::TruthParticleAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == COMPOSITEPARTICLE ){
    ATH_CHECK( (this->selectParticles<xAOD::CompositeParticleContainer,xAOD::CompositeParticleAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == PARITCLEFLOW ){
    ATH_CHECK( (this->selectParticles<xAOD::PFOContainer,xAOD::PFOAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == NEUTRALPARTICLE ){
    ATH_CHECK( (this->selectParticles<xAOD::NeutralParticleContainer,xAOD::NeutralParticleAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == TRACKPARTICLE ){
    ATH_CHECK( (this->selectParticles<xAOD::TrackParticleContainer,xAOD::TrackParticleAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == PARTICLE ){
    ATH_CHECK( (this->selectParticles<xAOD::ParticleContainer,xAOD::ParticleAuxContainer>(inContainer,resultVec)) );
  }
  else if ( m_contType == CALOCLUSTER ){
    ATH_CHECK( (this->selectParticles<xAOD::CaloClusterContainer,xAOD::CaloClusterAuxContainer>(inContainer,resultVec)) );
  }

  return StatusCode::SUCCESS;
}
