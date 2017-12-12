/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/HEPTopTaggerMomentsTool.h"
#include "JetSubStructureUtils/HEPTopTaggerImpl.h"

#include "JetSubStructureMomentTools/VariableRPseudoJetCalTool.h"

#include "fastjet/PseudoJet.hh"

#include "xAODJet/JetAuxContainer.h"

#include "TLorentzVector.h"

// ====================================================================================
//
// ====================================================================================
HEPTopTaggerMomentsTool::HEPTopTaggerMomentsTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
  ,m_variableRPseudoJetCalTool("VariableRPseudoJetCalTool")
  ,m_jetFromPseudojetTool("JetFromPseudojetTool")
{
  m_HEPTopTaggerImpl = new HEPTopTaggerImpl(name+"_Impl");

  // HEPTopTaggerMomentsTool properties
  declareProperty("HTT_VarRPseudoJetCalTool", m_variableRPseudoJetCalTool, "IVariableRPseudoJetCalTool for subjet calibration in HTT");
  declareProperty("JetFromPseudojetTool",   m_jetFromPseudojetTool);

  declareProperty("AttrLinkUserPrefix",     m_HTTAttrPrefix="",        "Custom prefix for attribute link names, default HTT{defConf}_{varName}");
  declareProperty("StoreFullHTTResult",     m_storeFullHTTResult=true, "Store all HTT output variables");
  declareProperty("StoreUntaggedHTTCandidates", m_storeUntaggedHTTCandidates=true, "Store HTT candidates that are not tagged");
  declareProperty("PredefHTTConf",          m_predefHTTConf="",        "String characterizing predefined HTT configurations [def, pTOrd, mOrd, pTOrdAShapeFirst, mOrdAShapeFirst]");
  declareProperty("CalibrateHTTSubjets",    m_calibrateHTTSubjets=true,"Calibrate subjets inside HEPTopTagger algorithm");

  declareProperty("RunOnLinkedJetName",     m_runOnLinkedJetName="",   "Name of associated jet. If set, run HTT on linked instead of input jet.");
}
HEPTopTaggerMomentsTool::~HEPTopTaggerMomentsTool()
{
  delete m_HEPTopTaggerImpl;
}
// ====================================================================================
//
// ====================================================================================
StatusCode HEPTopTaggerMomentsTool::setPredefConfiguration( std::string predefConfName )
{
  ATH_MSG_INFO( "Trying to set predef HTT configuration " << predefConfName );
  if( m_HTTAttrPrefix == "" || m_HTTAttrPrefix == ("HTT"+m_predefHTTConf+"_")) // not set or old, possibly different conf
    m_HTTAttrPrefix = "HTT"+predefConfName+"_";
  m_predefHTTConf  = predefConfName;
  
  // def == mOrd
  m_HEPTopTaggerImpl->setTopCandSortingMode(  HEPTopTaggerImpl::SortByMass );
  m_HEPTopTaggerImpl->setAcutBeforeSorting(   false );
  m_HEPTopTaggerImpl->setTwoStepFilter(       false );
  m_HEPTopTaggerImpl->setOptimalRTagger(      false );

  if( predefConfName == "" )
    return StatusCode::SUCCESS;
  else if( predefConfName == "def" || predefConfName == "mOrd" )
    m_HEPTopTaggerImpl->setTopCandSortingMode( HEPTopTaggerImpl::SortByMass );
  else if( predefConfName == "pTOrd" )
    m_HEPTopTaggerImpl->setTopCandSortingMode( HEPTopTaggerImpl::SortByPt );
  else if( predefConfName == "AOrd" )
    m_HEPTopTaggerImpl->setTopCandSortingMode( HEPTopTaggerImpl::SortByAshapeVariable );
  else if( predefConfName == "mOrdAShapeFirst" ){
    m_HEPTopTaggerImpl->setTopCandSortingMode( HEPTopTaggerImpl::SortByMass );
    m_HEPTopTaggerImpl->setAcutBeforeSorting(  true );
  }
  else if( predefConfName == "pTOrdAShapeFirst" ){
    m_HEPTopTaggerImpl->setTopCandSortingMode( HEPTopTaggerImpl::SortByPt );
    m_HEPTopTaggerImpl->setAcutBeforeSorting(  true );
  }
  else if( predefConfName == "defOptimalR" || predefConfName == "mOrdOptimalR" ){
    m_HEPTopTaggerImpl->setTopCandSortingMode( HEPTopTaggerImpl::SortByMass );
    m_HEPTopTaggerImpl->setOptimalRTagger(     true );
  }
  else if( predefConfName == "pTOrdOptimalR" ){
    m_HEPTopTaggerImpl->setTopCandSortingMode(  HEPTopTaggerImpl::SortByPt );
    m_HEPTopTaggerImpl->setOptimalRTagger(      true );
  }
  else if( predefConfName == "AOrdOptimalR" ){
    m_HEPTopTaggerImpl->setTopCandSortingMode(  HEPTopTaggerImpl::SortByAshapeVariable );
    m_HEPTopTaggerImpl->setOptimalRTagger(      true );
  }
  else if( predefConfName == "mOrdAShapeFirstOptimalR" ){
    m_HEPTopTaggerImpl->setTopCandSortingMode(  HEPTopTaggerImpl::SortByMass);
    m_HEPTopTaggerImpl->setOptimalRTagger(      true );
    m_HEPTopTaggerImpl->setAcutBeforeSorting(   true );
  }
  else if( predefConfName == "pTOrdAShapeFirstOptimalR" ){
    m_HEPTopTaggerImpl->setTopCandSortingMode(  HEPTopTaggerImpl::SortByPt );
    m_HEPTopTaggerImpl->setOptimalRTagger(      true );
    m_HEPTopTaggerImpl->setAcutBeforeSorting(   true );
  }
  else{
    ATH_MSG_ERROR("Unknown predef configuration " << predefConfName << ". Try one of def, pTOrd, mOrd, pTOrdAShapeFirst, mOrdAShapeFirst" ); 
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
// ====================================================================================
//
// ====================================================================================
StatusCode HEPTopTaggerMomentsTool::initialize( ){
  ATH_MSG_INFO("Initialize HEPTopTagger");
  m_HTTAttrPrefix = "HTT"+m_predefHTTConf+"_";

  // initialize subjet calibration tool for HTT if not available
  if( !m_variableRPseudoJetCalTool.retrieve().isSuccess() ){
    ATH_MSG_INFO("Couldn't retrieve VariableRPseudoJetCalTool " << m_variableRPseudoJetCalTool.name());
    ATH_MSG_INFO("Will initialize VariableRPseudoJetCalTool with default parameters to be used by HEPTopTagger");
    VariableRPseudoJetCalTool* newVarRPJCalTool = new VariableRPseudoJetCalTool( m_variableRPseudoJetCalTool.name() );
    ATH_CHECK( newVarRPJCalTool->setProperty("IsData",        false) );
    ATH_CHECK( newVarRPJCalTool->setProperty("JetAlgorithm",  "CamKt" ) );
    ATH_CHECK( newVarRPJCalTool->setProperty("JetInputs",     "LCTopo" ) );
    ATH_CHECK( newVarRPJCalTool->setProperty("ConfigFile",    "CamKt_JES_HTT.config") );
    ATH_CHECK( newVarRPJCalTool->setProperty("CalibSequence", "Origin_EtaJES") );
    ATH_CHECK(  newVarRPJCalTool->setProperty("JetFromPseudojetTool", m_jetFromPseudojetTool ) );
    std::vector<int> calRadii =  {20, 25, 30, 35, 40, 45, 50, 55, 60};
    ATH_CHECK( newVarRPJCalTool->setProperty("JetRadiusVec", calRadii) );
    ATH_CHECK( newVarRPJCalTool->initialize() );
    m_variableRPseudoJetCalTool = ToolHandle<IVariableRPseudoJetCalTool>( newVarRPJCalTool );     
  }
  if( m_calibrateHTTSubjets ){ 
    m_HEPTopTaggerImpl->setCalibrateSubjets( true );
    m_HEPTopTaggerImpl->setPseudoJetCalTool( dynamic_cast<IPseudoJetCalibrationTool*>(m_variableRPseudoJetCalTool.operator->()) );
  }
  else
    m_HEPTopTaggerImpl->setCalibrateSubjets( false );
  m_HEPTopTaggerImpl->setStoreUntagged( m_storeUntaggedHTTCandidates );
  if( m_HEPTopTaggerImpl->initialize() != 0 ) return StatusCode::FAILURE;
  ATH_CHECK(setPredefConfiguration( m_predefHTTConf ));

  print();
  return StatusCode::SUCCESS;
}
// ====================================================================================
//
// ====================================================================================
int HEPTopTaggerMomentsTool::modifyJet(xAOD::Jet &jet) const
{
  const fastjet::PseudoJet* pjet = NULL;
  if( m_runOnLinkedJetName == "" )
    pjet = jet.getPseudoJet();
  else{
    const ElementLink<xAOD::JetContainer>& jetLink  = jet.getAttribute<ElementLink<xAOD::JetContainer> >(m_runOnLinkedJetName);
    pjet = (*jetLink)->getPseudoJet();
  }

  // step 1: run HEPTopTagger
  HEPTopTaggerImpl::HTTResult httResult = m_HEPTopTaggerImpl->runTagger( *pjet );
 
  // step 2: decorate jet
  fastjet::PseudoJet  topCandidatePj = httResult.getTopCandidate();
  TLorentzVector      topCandidate4Mom( topCandidatePj.px(), topCandidatePj.py(), topCandidatePj.pz(), topCandidatePj.e() );

  jet.setAttribute(getHTTAttributeName("isTagged"), httResult.isTagged());
  jet.setAttribute(getHTTAttributeName("isValidCandidate"), httResult.isValidCandidate());
  jet.setAttribute< TLorentzVector >(getHTTAttributeName("topCandidate"),   topCandidate4Mom );
  jet.setAttribute< fastjet::PseudoJet >(getHTTAttributeName("topCandidateFJ"), topCandidatePj );

  if( m_storeFullHTTResult ){
    std::vector< TLorentzVector > topChildren4Mom;
    topChildren4Mom.reserve(3);
    for( auto &child : httResult.getTopChildren() )
      topChildren4Mom.push_back( TLorentzVector( child.px(), child.py(), child.pz(), child.e() ) );

    int bIdx;
    int W1Idx;
    int W2Idx;
    double m123;
    double m12;
    double m13;
    double m23;
    httResult.getChildIndices( bIdx, W1Idx, W2Idx );
    httResult.getMasses( m123, m12, m13, m23 );
    jet.setAttribute(getHTTAttributeName("tagMass"),        httResult.getTagMass());
    jet.setAttribute(getHTTAttributeName("tagMassRatios"),  httResult.getTagMassRatios());
    jet.setAttribute(getHTTAttributeName("tagPt"),          httResult.getTagPt());
    jet.setAttribute< std::vector< TLorentzVector > >( getHTTAttributeName("topChildren"), topChildren4Mom );
    jet.setAttribute(getHTTAttributeName("bIdx"),           bIdx);             
    jet.setAttribute(getHTTAttributeName("W1Idx"),          W1Idx);
    jet.setAttribute(getHTTAttributeName("W2Idx"),          W2Idx); 
    jet.setAttribute(getHTTAttributeName("bestMassRatio"),  httResult.getBestMassRatio());
    jet.setAttribute(getHTTAttributeName("AshapeVariable"), httResult.getAshapeVariable());
    jet.setAttribute(getHTTAttributeName("optimalR"),       httResult.getOptimalR());
    jet.setAttribute(getHTTAttributeName("atanM13M12"),     httResult.getAtanM13M12());
    jet.setAttribute(getHTTAttributeName("m23M123"),        m23/m123);
  }
  return 0;
}
// ====================================================================================
//
// ====================================================================================
int HEPTopTaggerMomentsTool::modify(xAOD::JetContainer& jets) const
{
  for( xAOD::Jet* jet : jets ){
    // run HTT on jet
    int jetResult = this->modifyJet( *jet );
    if (jetResult != 0) return jetResult;
  }
  return 0;
}
// ====================================================================================
//
// ====================================================================================
void HEPTopTaggerMomentsTool::print() const
{
  ATH_MSG_INFO("=========== " << name() << " parameters: ===========" );
  m_HEPTopTaggerImpl->printConfig();

  std::string linkedAttributes = getHTTAttributeName("isTagged") + ", " + getHTTAttributeName("isValidCandidate") + ", " + getHTTAttributeName("topCandidate");
  if( m_storeFullHTTResult ){
    linkedAttributes += ", "+getHTTAttributeName("tagMass");
    linkedAttributes += ", "+getHTTAttributeName("tagMassRatios");
    linkedAttributes += ", "+getHTTAttributeName("tagPt");
    linkedAttributes += ", "+getHTTAttributeName("topChildren");
    linkedAttributes += ", "+getHTTAttributeName("bIdx");
    linkedAttributes += ", "+getHTTAttributeName("W1Idx");
    linkedAttributes += ", "+getHTTAttributeName("W2Idx");
    linkedAttributes += ", "+getHTTAttributeName("bestMassRatio");
    linkedAttributes += ", "+getHTTAttributeName("AshapeVariable");
    linkedAttributes += ", "+getHTTAttributeName("optimalR");
    linkedAttributes += ", "+getHTTAttributeName("atanM13M12");
    linkedAttributes += ", "+getHTTAttributeName("m23M123");
  }
  ATH_MSG_INFO("  linked attributes  " << linkedAttributes );
}
// ====================================================================================
//
// ====================================================================================
HEPTopTaggerImpl* HEPTopTaggerMomentsTool::getHEPTopTaggerImpl( ){
  return m_HEPTopTaggerImpl;
}
// ====================================================================================
//
// ====================================================================================
std::string HEPTopTaggerMomentsTool::getHTTAttributeName( const char* variableName ) const
{
 return m_HTTAttrPrefix+variableName;
}
