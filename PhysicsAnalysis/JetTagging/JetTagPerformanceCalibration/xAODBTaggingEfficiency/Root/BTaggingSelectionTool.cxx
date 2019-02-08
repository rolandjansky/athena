/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"
#include "xAODBTagging/BTagging.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"
#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"

#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"

#include <algorithm>

using std::string;

using CP::CorrectionCode;
using CP::SystematicSet;
using CP::SystematicVariation;
using CP::SystematicCode;
using CP::SystematicRegistry;

using Analysis::Uncertainty;
using Analysis::CalibrationDataVariables;
using Analysis::CalibrationDataContainer;
using Analysis::CalibResult;
using Analysis::CalibrationStatus;
using Analysis::Total;
using Analysis::SFEigen;
using Analysis::SFNamed;
using Analysis::None;

using xAOD::IParticle;

BTaggingSelectionTool::BTaggingSelectionTool( const std::string & name)
  : asg::AsgTool( name ), m_accept( "JetSelection" ), m_spline(nullptr), m_constcut(nullptr)
{
  m_initialised = false;
  declareProperty( "MaxEta", m_maxEta = 2.5 );
  declareProperty( "MinPt", m_minPt = 20000 /*MeV*/);
  declareProperty( "MaxRangePt", m_maxRangePt = 1000000 /*MeV*/);
  declareProperty( "FlvTagCutDefinitionsFileName", m_CutFileName = "", "name of the files containing official cut definitions (uses PathResolver)");
  declareProperty( "TaggerName",                    m_taggerName="",    "tagging algorithm name");
  declareProperty( "OperatingPoint",                m_OP="",            "operating point");
  declareProperty( "JetAuthor",                     m_jetAuthor="",     "jet collection");
}

StatusCode BTaggingSelectionTool::initialize() {
  m_initialised = true;

  if (""==m_OP){
    ATH_MSG_ERROR( "BTaggingSelectionTool wasn't given a working point name" );
    return StatusCode::FAILURE;
  }

  TString pathtofile =  PathResolverFindCalibFile(m_CutFileName);
  m_inf = TFile::Open(pathtofile, "read");
  if (0==m_inf) {
    ATH_MSG_ERROR( "BTaggingSelectionTool couldn't access tagging cut definitions" );
    return StatusCode::FAILURE;
  }

  // The tool supports only Akt4TopoEM, Akt4PV0Track and Akt2PV0Track jets, and VR track jets (AntiKtVR30Rmax4Rmin02TrackJets)
  if ("MV2c20"!=m_taggerName&&
      "MV2c10"!=m_taggerName&&
      "MV2cl100_MV2c100"!=m_taggerName){
    ATH_MSG_ERROR( "BTaggingSelectionTool doesn't support tagger: "+m_taggerName );
    return StatusCode::FAILURE;
  }
  if ("AntiKt4EMTopoJets"  != m_jetAuthor &&
      "AntiKt2PV0TrackJets"!= m_jetAuthor &&
      "AntiKt4PV0TrackJets"!= m_jetAuthor &&
      "AntiKtVR30Rmax4Rmin02TrackJets" !=m_jetAuthor
      ){
    ATH_MSG_ERROR( "BTaggingSelectionTool doesn't support jet collection: "+m_jetAuthor );
    return StatusCode::FAILURE;
  }

  // Change the minPt cut if the user didn't touch it
  if (20000==m_minPt){// is it still the default value
    if ("AntiKt2PV0TrackJets"== m_jetAuthor || "AntiKtVR30Rmax4Rmin02TrackJets"== m_jetAuthor) m_minPt=10000 ;
    if ("AntiKt4PV0TrackJets"== m_jetAuthor) m_minPt= 7000 ;
  }
  // Change the maxRangePt cut if the user didn't touch it
  if (1000000==m_maxRangePt){// is it still the default value
    if ("AntiKt2PV0TrackJets"== m_jetAuthor || "AntiKtVR30Rmax4Rmin02TrackJets"== m_jetAuthor) m_maxRangePt= 400000;
    if ("AntiKt4PV0TrackJets"== m_jetAuthor) m_maxRangePt= 500000;
  }

  // Operating point reading
  TString cutname = m_OP;
  if ("Continuous"==cutname(0,10)){  // For continuous tagging load all flat-cut WPs
    //100% efficiency => MVXWP=-infinity
    m_continuouscuts[0] = -1.e4;

    cutname = m_taggerName+"/"+m_jetAuthor+"/FixedCutBEff_85/cutvalue";
    m_constcut = (TVector*) m_inf->Get(cutname);
    if (m_constcut!=nullptr) m_continuouscuts[1] = m_constcut[0](0);
    else ATH_MSG_ERROR( "Continuous tagging is trying to use an unvalid operating point: FixedCutBEff_85" );

    cutname = m_taggerName+"/"+m_jetAuthor+"/FixedCutBEff_77/cutvalue";
    m_constcut = (TVector*) m_inf->Get(cutname);
    if (m_constcut!=nullptr) m_continuouscuts[2] = m_constcut[0](0);
    else ATH_MSG_ERROR( "Continuous tagging is trying to use an unvalid operating point: FixedCutBEff_77" );

    cutname = m_taggerName+"/"+m_jetAuthor+"/FixedCutBEff_70/cutvalue";
    m_constcut = (TVector*) m_inf->Get(cutname);
    if (m_constcut!=nullptr) m_continuouscuts[3] = m_constcut[0](0);
    else ATH_MSG_ERROR( "Continuous tagging is trying to use an unvalid operating point: FixedCutBEff_70" );

    cutname = m_taggerName+"/"+m_jetAuthor+"/FixedCutBEff_60/cutvalue";
    m_constcut = (TVector*) m_inf->Get(cutname);
    if (m_constcut!=nullptr) m_continuouscuts[4] = m_constcut[0](0);
    else ATH_MSG_ERROR( "Continuous tagging is trying to use an unvalid operating point: FixedCutBEff_60" );

    //0% efficiency => MVXWP=+infinity
    m_continuouscuts[5]= +1.e4;
  }
  else {  // Else load only one WP
    if ("2DFixedCut"==cutname(0,10)){
      cutname = m_taggerName+"/"+m_jetAuthor+"/"+m_OP+"/cutvalue";
      m_constcut = (TVector*) m_inf->Get(cutname);
      if (m_constcut == nullptr) ATH_MSG_ERROR( "Invalid operating point" );
    }
    else if ("FlatBEff"==cutname(0,8)){
      cutname = m_taggerName+"/"+m_jetAuthor+"/"+m_OP+"/cutprofile";
      m_spline = (TSpline3*) m_inf->Get(cutname);
      if (m_spline == nullptr) ATH_MSG_ERROR( "Invalid operating point" );
    }
    else {
      cutname = m_taggerName+"/"+m_jetAuthor+"/"+m_OP+"/cutvalue";
      m_constcut = (TVector*) m_inf->Get(cutname);
      if (m_constcut == nullptr) ATH_MSG_ERROR( "Invalid operating point" );
    }
  }

  m_inf->Close();

  m_accept.addCut( "Eta", "Selection of jets according to their pseudorapidity" );
  m_accept.addCut( "Pt",  "Selection of jets according to their transverse momentum" );
  m_accept.addCut( "WorkingPoint",  "Working point for flavour-tagging of jets according to their b-tagging weight" );

  return StatusCode::SUCCESS;
}
const asg::AcceptInfo& BTaggingSelectionTool::getAcceptInfo() const {
  return m_accept;
}

asg::AcceptData BTaggingSelectionTool::accept( const xAOD::IParticle* p ) const {
  // Check if this is a jet:
  if( p->type() != xAOD::Type::Jet ) {
    ATH_MSG_ERROR( "accept(...) Function received a non-jet" );
    return asg::AcceptData (&m_accept);
  }

  // Cast it to a jet:
  const xAOD::Jet* jet = dynamic_cast< const xAOD::Jet* >( p );
  if( ! jet ) {
    ATH_MSG_FATAL( "accept(...) Failed to cast particle to jet" );
    return asg::AcceptData (&m_accept);
  }

  // Let the specific function do the work:
  return accept( *jet );
}

asg::AcceptData BTaggingSelectionTool::accept( const xAOD::Jet& jet ) const {
  asg::AcceptData acceptData (&m_accept);

  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingSelectionTool has not been initialised");
    return acceptData;
  }

  if ("AntiKt2PV0TrackJets"== m_jetAuthor ||
      "AntiKt4PV0TrackJets"== m_jetAuthor ||
      "AntiKtVR30Rmax4Rmin02TrackJets"== m_jetAuthor
      ){
    // We want at least 2 tracks in a track jet
    acceptData.setCutResult( "NConstituents", jet.numConstituents() >= 2 );
  }

  double pT = jet.pt();
  double eta = jet.eta();

  // Retrieve the tagger weight which was assigned to the jet
  TString cutname = m_OP;
  if ("2D"!=cutname(0,2)){
    double weight_mv2(-10.);
    const xAOD::BTagging* btag = jet.btagging();
    if ((!btag) || (!btag->MVx_discriminant(m_taggerName, weight_mv2))){
      ATH_MSG_ERROR("Failed to retrieve "+m_taggerName+" weight!");
    }
    ATH_MSG_VERBOSE( "MV2c20 " <<  weight_mv2 );
    acceptData |= accept(pT, eta, weight_mv2);
  }
  else {
    double weight_mv2c100(-10.);
    double weight_mv2cl100(-10.);
    const xAOD::BTagging* btag = jet.btagging();
    if ((!btag)){
      ATH_MSG_ERROR("Failed to retrieve the BTagging information");
    }
    if ( (!btag->MVx_discriminant("MV2cl100", weight_mv2cl100 ))|| (!btag->MVx_discriminant("MV2c100", weight_mv2c100 )) ){
      ATH_MSG_ERROR("Failed to retrieve the BTagging "+m_taggerName+" weight!");
    }
    ATH_MSG_VERBOSE( "MV2cl100 "  <<  weight_mv2cl100 );
    ATH_MSG_VERBOSE( "MV2c100 " <<  weight_mv2c100 );
    acceptData |= accept(pT, eta, weight_mv2cl100, weight_mv2c100 );
  }
  return acceptData;
}

asg::AcceptData BTaggingSelectionTool::accept(double pT, double eta, double weight_mv2) const
{
  asg::AcceptData acceptData (&m_accept);

  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingSelectionTool has not been initialised");
    return acceptData;
  }

  // flat cut for out of range pTs
  if (pT>m_maxRangePt)
    pT = m_maxRangePt-500; // 500 MeV below the maximum authorized range

  eta = std::fabs(eta);

  if (! checkRange(pT, eta, acceptData))
    return acceptData;

  // After initialization, either m_spline or m_constcut should be non-zero
  // Else, the initialization was incorrect and should be revisited
  double cutvalue(DBL_MAX);
  if (m_spline != nullptr && m_constcut == nullptr) {
    cutvalue = m_spline->Eval(pT/1000.);
  }
  else if (m_constcut != nullptr && m_spline == nullptr) {
    cutvalue = m_constcut[0](0);
  }
  else ATH_MSG_ERROR( "Bad cut configuration!" );
  ATH_MSG_VERBOSE( "Cut value " << cutvalue );

  if (  weight_mv2 < cutvalue ){
    return acceptData;
  }
  acceptData.setCutResult( "WorkingPoint", true );

  // Return the result:
  return acceptData;
}



asg::AcceptData BTaggingSelectionTool::accept(double pT, double eta, double weight_mv2cl100, double weight_mv2c100) const
{
  asg::AcceptData acceptData (&m_accept);

  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingSelectionTool has not been initialised");
    return acceptData;
  }

  // flat cut for out of range pTs
  if (pT>m_maxRangePt)
    pT = m_maxRangePt-500; // 500 MeV below the maximum authorized range

  eta = std::fabs(eta);

  if (! checkRange(pT, eta, acceptData))
    return acceptData;

  // After initialization, either m_spline or m_constcut should be non-zero
  // Else, the initialization was incorrect and should be revisited
  double cutvalueA(DBL_MAX);
  double cutvalueB(DBL_MAX);
  if (m_constcut != nullptr && m_spline == nullptr) {
    cutvalueA = m_constcut[0](0);
    cutvalueB = m_constcut[0](1);
  }
  else ATH_MSG_ERROR( "Bad cut configuration!" );
  ATH_MSG_VERBOSE( "Cut values " << cutvalueA << " " << cutvalueB );

  if ( !(  weight_mv2cl100 > cutvalueA && weight_mv2c100 < cutvalueB )  ){
    return acceptData;
  }
  acceptData.setCutResult( "WorkingPoint", true ); // IF we arrived here, the jet is tagged

  // Return the result:
  return acceptData;
}




int BTaggingSelectionTool::getQuantile( const xAOD::IParticle* p ) const {
  // Check if this is a jet:
  if( p->type() != xAOD::Type::Jet ) {
    ATH_MSG_ERROR( "accept(...) Function received a non-jet" );
    return -1;
  }

  // Cast it to a jet:
  const xAOD::Jet* jet = dynamic_cast< const xAOD::Jet* >( p );
  if( ! jet ) {
    ATH_MSG_FATAL( "accept(...) Failed to cast particle to jet" );
    return -1;
  }

  // Let the specific function do the work:
  return getQuantile( *jet );
}

int BTaggingSelectionTool::getQuantile( const xAOD::Jet& jet ) const{
  double pT = jet.pt();
  double eta = jet.eta();
  // Retrieve the tagger weight which was assigned to the jet
  double weight_mv2(-10.);
  if (!jet.btagging()->MVx_discriminant(m_taggerName, weight_mv2)){
    ATH_MSG_ERROR("Failed to retrieve "+m_taggerName+" weight!");
  }
  ATH_MSG_VERBOSE( "MV2c20 " <<  weight_mv2 );

  return getQuantile(pT, eta, weight_mv2 );
}


int BTaggingSelectionTool::getQuantile(double pT, double eta, double weight_mv2 ) const
{
  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingSelectionTool has not been initialised");
  }
  //////////////////////
  // Cheatsheet:
  // returns 5 if between 60% and 0%
  // returns 4 if between 70% and 60%
  // returns 3 if between 77% and 70%
  // returns 2 if between 85% and 77%
  // returns 1 if between 100% and 85%
  // returns 0 if smaller than -1e4-> should never happen
  // return -1 if bigger than 1e4 or not in b-tagging acceptance
  //////////////////////

  int bin_index(-1);

  asg::AcceptData acceptData (&m_accept);
  if (! checkRange(pT, eta, acceptData)) return bin_index;

  // If in b-tagging acceptance, cont.tagging
  for (int i=0; i<=5; ++i) {
    if (weight_mv2 < m_continuouscuts[i]) {
      bin_index = i;
      break;
    }
  }

  if (0==bin_index) ATH_MSG_ERROR("B-tagging weight is smaller than 1e-4, this shouldn't happen!");

  return bin_index;
}

bool BTaggingSelectionTool::checkRange(double pT, double eta,
                                       asg::AcceptData& acceptData) const
{
  // Do the |eta| cut:
  if( eta > m_maxEta ) {
    return false;
  }
  acceptData.setCutResult( "Eta", true );

  // Do the pT cut:
  ATH_MSG_VERBOSE( "Jet pT: " << pT );
  if( pT < m_minPt ) {
    return false;
  }
  acceptData.setCutResult( "Pt", true );

  return true;
}

double BTaggingSelectionTool::getCutValue() const
{
  if (m_constcut != nullptr && m_spline == nullptr) {
    return m_constcut[0](0);
  }
  else{
    std::cout << "not using fixed cut flavour tagging!" << std::endl;
    return -1000;
  }
}
