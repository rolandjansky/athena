/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "TauAnalysisTools/TauSelectionTool.h"

namespace TauAnalysisTools {
  
  TauSelectionTool::TauSelectionTool( const std::string& name )
    : asg::AsgTool( name ),
      m_iSelectionCuts(NoCut), // initialize with 'no' cuts
      m_vPtRegion({}),
      m_vAbsEtaRegion({}),
      m_vAbsCharges({}),
      m_vNTracks({}),
      m_vJetBDTRegion({}),
      m_iJetIDWP(JetIDWP::JetBDTNONE),
      m_vEleBDTRegion({}),
      m_iEleBDTWP(EleBDTWP::EleBDTNONE),
      m_dPtMin(NAN), // in GeV
      m_dPtMax(NAN), // in GeV
      m_dAbsEtaMin(NAN), 
      m_dAbsEtaMax(NAN), 
      m_iAbsCharge(NAN),
      m_iNTrack(NAN),
      m_dJetBDTMin(NAN), 
      m_dJetBDTMax(NAN), 
      m_dEleBDTMin(NAN), 
      m_dEleBDTMax(NAN), 
      m_aAccept( "TauSelection" )
  {
    declarePropertiesBaseline();
  }

  void TauSelectionTool::declarePropertiesBaseline(){
    /*
      properties containing 'Region' are a vector of lower and upper bounds
      other properties named in plural are a list of exact values to cut on
      other properties are single cuts
    */
    declareProperty( "SelectionCuts", m_iSelectionCuts); // initialize with 'no' cuts
    declareProperty( "PtRegion", m_vPtRegion ); // in GeV
    declareProperty( "PtMin", m_dPtMin); // in GeV
    declareProperty( "PtMax", m_dPtMax); // in GeV
    declareProperty( "AbsEtaRegion", m_vAbsEtaRegion);
    declareProperty( "AbsEtaMin", m_dAbsEtaMin); 
    declareProperty( "AbsEtaMax", m_dAbsEtaMax); 
    declareProperty( "AbsCharges", m_vAbsCharges);
    declareProperty( "AbsCharge", m_iAbsCharge);
    declareProperty( "NTracks", m_vNTracks);
    declareProperty( "NTrack", m_iNTrack);
    declareProperty( "JetBDTRegion", m_vJetBDTRegion);
    declareProperty( "JetBDTMin", m_dJetBDTMin); 
    declareProperty( "JetBDTMax", m_dJetBDTMax); 
    declareProperty( "JetIDWP", m_iJetIDWP);	
    declareProperty( "EleBDTRegion", m_vEleBDTRegion);
    declareProperty( "EleBDTMin", m_dEleBDTMin); 
    declareProperty( "EleBDTMax", m_dEleBDTMax); 
    declareProperty( "EleBDTWP", m_iEleBDTWP);
  }

  void TauSelectionTool::setRecommendedProperties(){
    ATH_MSG_INFO( "setting recommended tau selection cuts" );
    m_iSelectionCuts = SelectionCuts(CutPt | CutAbsEta | CutAbsCharge | CutNTrack | CutJetIDWP | CutEleBDTWP );
    m_dPtMin = 20;
    m_vAbsEtaRegion = {0,1.37,1.52,2.5};
    m_iAbsCharge = 1;
    m_vNTracks = {1,3};
    m_iJetIDWP = JetBDTTight;
    m_iEleBDTWP = EleBDTLoose;
  }

  template<typename T, typename U>
  void TauSelectionTool::FillRegionVector(std::vector<T>* vRegion, U tMin, U tMax){
    if (vRegion->size()>0)
      return;
    if (tMin == tMin) 		// if tMin is NAN, then this assumption fails and -inf is added to the vector
      vRegion->push_back(tMin);
    else
      // vRegion->push_back(-1./0.);
      vRegion->push_back(-std::numeric_limits<T>::infinity());

    if (tMax == tMax)		// if tMax is NAN, then this assumption fails and inf is added to the vector
      vRegion->push_back(tMax);
    else
      vRegion->push_back(std::numeric_limits<T>::infinity());
  }

  template<typename T, typename U>
  void TauSelectionTool::FillValueVector(std::vector<T>* vRegion, U tVal){
    if (vRegion->size()>0)
      return;
    if (tVal == tVal)		// if tMax is NAN, then this assumption fails and nothing is added to the vector
      vRegion->push_back(tVal);
  }

  template<typename T>
  void TauSelectionTool::PrintConfigRegion(std::string sCutName, std::vector<T>* vRegion){
    unsigned int iNumRegion = vRegion->size()/2;
    for( unsigned int iRegion = 0; iRegion < iNumRegion; iRegion++ ){
      ATH_MSG_DEBUG( sCutName<<": " << vRegion->at(iRegion*2) << " to " << vRegion->at(iRegion*2+1) );
    }
  }

  template<typename T>
  void TauSelectionTool::PrintConfigValue(std::string sCutName, std::vector<T>* vRegion){
    for( unsigned int iRegion = 0; iRegion < vRegion->size(); iRegion++ ){
      ATH_MSG_DEBUG( sCutName<<": " << vRegion->at(iRegion) );
    }
  }
  
  StatusCode TauSelectionTool::initialize() {
    FillRegionVector(&m_vPtRegion, m_dPtMin, m_dPtMax);
    FillRegionVector(&m_vAbsEtaRegion, m_dAbsEtaMin, m_dAbsEtaMax);
    FillRegionVector(&m_vJetBDTRegion, m_dJetBDTMin, m_dJetBDTMax );
    FillRegionVector(&m_vEleBDTRegion, m_dEleBDTMin, m_dEleBDTMax );
    FillValueVector(&m_vAbsCharges, m_iAbsCharge );
    FillValueVector(&m_vNTracks, m_iNTrack );
    // Greet the user:
    ATH_MSG_INFO( "Initialising TauSelectionTool" );
    PrintConfigRegion("Pt",&m_vPtRegion);
    PrintConfigRegion("AbsEta",&m_vAbsEtaRegion);
    PrintConfigValue("AbsCharge",&m_vAbsCharges);
    PrintConfigValue("NTrack",&m_vNTracks);
    PrintConfigRegion("BDTJetScore",&m_vJetBDTRegion);
    PrintConfigRegion("BDTEleScore",&m_vEleBDTRegion);
    std::string sCuts = "";
    if (m_iSelectionCuts & CutPt) sCuts+= "Pt ";
    if (m_iSelectionCuts & CutAbsEta) sCuts+= "AbsEta ";
    if (m_iSelectionCuts & CutAbsCharge) sCuts+= "AbsCharge ";
    if (m_iSelectionCuts & CutNTrack) sCuts+= "NTrack ";
    if (m_iSelectionCuts & CutJetBDTScore) sCuts+= "JetBDTScore ";
    if (m_iSelectionCuts & CutJetIDWP) sCuts+= "JetIDWP ";
    if (m_iSelectionCuts & CutEleBDTScore) sCuts+= "EleBDTScore ";
    if (m_iSelectionCuts & CutEleBDTWP) sCuts+= "EleBDTWP ";
        
    ATH_MSG_DEBUG( "cuts: " << sCuts);
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  const Root::TAccept& TauSelectionTool::getTAccept() const {

    return m_aAccept;
  }

  const Root::TAccept&
  TauSelectionTool::accept( const xAOD::IParticle* xP ) const {

    // Reset the result:
    m_aAccept.clear();
    
    // Check if this is a tau:
    if( xP->type() != xAOD::Type::Tau ) {
      ATH_MSG_ERROR( "accept(...) Function received a non-tau" );
      return m_aAccept;
    }

    // Cast it to a tau:
    const xAOD::TauJet* xTau = dynamic_cast< const xAOD::TauJet* >( xP );
    if( ! xTau ) {
      ATH_MSG_FATAL( "accept(...) Failed to cast particle to tau" );
      return m_aAccept;
    }

    // Let the specific function do the work:
    return accept( *xTau );
  }

  const Root::TAccept& TauSelectionTool::accept( const xAOD::TauJet& xTau ) const {

    // Reset the result:
    m_aAccept.clear();
    
    if (m_iSelectionCuts & CutPt)
      if (!checkPtRegion(xTau))
	return m_aAccept;
    if (m_iSelectionCuts & CutAbsEta)
      if (!checkAbsEtaRegion(xTau))
	return m_aAccept;
    if (m_iSelectionCuts & CutAbsCharge)
      if (!checkAbsCharge(xTau))
	return m_aAccept;
    if (m_iSelectionCuts & CutNTrack)
      if (!checkNTracks(xTau))
	return m_aAccept;
    if (m_iSelectionCuts & CutJetBDTScore)
      if (!checkJetBDTScore(xTau))
    	return m_aAccept;
    if (m_iSelectionCuts & CutJetIDWP)
      if (!checkJetIDWP(xTau))
    	return m_aAccept;
    if (m_iSelectionCuts & CutEleBDTScore)
      if (!checkEleBDTScore(xTau))
    	return m_aAccept;
    if (m_iSelectionCuts & CutEleBDTWP)
      if (!checkEleBDTWP(xTau))
    	return m_aAccept;
    
    // Return the result:
    return m_aAccept;
  }

  bool TauSelectionTool::checkPtRegion( const xAOD::TauJet& xTau ) const{
    // check regions of pt, if tau is in one region then return true; false otherwise 
    m_aAccept.addCut( "Pt",
		      "Selection of taus according to their transverse momentum" );
    // save tau pt in GeV
    double pt = xTau.pt() / 1000.;
    // in case of only one entry in vector, run for lower limits
    if (m_vPtRegion.size() == 1){
      if ( pt >= m_vPtRegion.at(0) ){
	m_aAccept.setCutResult( "Pt", true );
        return true;
      }
    }
    unsigned int iNumPtRegion = m_vPtRegion.size()/2;
    for( unsigned int iPtRegion = 0; iPtRegion < iNumPtRegion; iPtRegion++ ){
      if ( pt >= m_vPtRegion.at(iPtRegion*2) and pt <= m_vPtRegion.at(iPtRegion*2+1)){
	m_aAccept.setCutResult( "Pt", true );
	return true;
      }
    }
    ATH_MSG_VERBOSE( "Tau failed pt region requirement, tau pt: " << pt );
    return false;
  }

  bool TauSelectionTool::checkAbsEtaRegion( const xAOD::TauJet& xTau ) const{
    // check regions of eta, if tau is in one region then return true; false otherwise 
    m_aAccept.addCut( "AbsEta",
		      "Selection of taus according to their absolute pseudorapidity" );
    unsigned int iNumEtaRegion = m_vAbsEtaRegion.size()/2;
    for( unsigned int iEtaRegion = 0; iEtaRegion < iNumEtaRegion; iEtaRegion++ ){
      if ( std::abs( xTau.eta() ) >= m_vAbsEtaRegion.at(iEtaRegion*2) and std::abs( xTau.eta() ) <= m_vAbsEtaRegion.at(iEtaRegion*2+1)){
	m_aAccept.setCutResult( "Eta", true );
	return true;
      }
    }
    ATH_MSG_VERBOSE( "Tau failed eta region requirement, tau eta: " << xTau.eta() );
    return false;
  }

  bool TauSelectionTool::checkAbsCharge( const xAOD::TauJet& xTau ) const{
    // check charge, if tau has one of the charges requiered then return true; false otherwise 
    m_aAccept.addCut( "AbsCharge",
		      "Selection of taus according to their absolute charge" );
    for( unsigned int iCharge = 0; iCharge < m_vAbsCharges.size(); iCharge++ ){
      if ( std::abs( xTau.charge() ) == m_vAbsCharges.at(iCharge) ){
	m_aAccept.setCutResult( "AbsCharge", true );
	return true;
      }
    }
    ATH_MSG_VERBOSE( "Tau failed charge requirement, tau charge: " << xTau.charge() );
    return false;
  }

  bool TauSelectionTool::checkNTracks( const xAOD::TauJet& xTau ) const{
    // check track multiplicity, if tau has one of the number of tracks requiered then return true; false otherwise 
    m_aAccept.addCut( "NTrack",
		      "Selection of taus according to their number of associated tracks" );
    for( unsigned int iNumTrack = 0; iNumTrack < m_vNTracks.size(); iNumTrack++ ){
      if ( xTau.nTracks() == m_vNTracks.at(iNumTrack) ){
	m_aAccept.setCutResult( "NTrack", true );
	return true;
      }
    }
    ATH_MSG_VERBOSE( "Tau failed nTracks requirement, tau number of tracks: " << xTau.nTracks() );
    return false;
  }

  bool TauSelectionTool::checkJetBDTScore( const xAOD::TauJet& xTau ) const{
    // check JetBDTscore, if tau has a JetBDT score in one of the regions requiered then return true; false otherwise 
    m_aAccept.addCut( "JetBDTScore",
		      "Selection of taus according to their JetBDTScore" );
    double dJetBDTScore = xTau.discriminant(xAOD::TauJetParameters::TauID(15));
    unsigned int iNumJetBDTRegion = m_vJetBDTRegion.size()/2;
    for( unsigned int iJetBDTRegion = 0; iJetBDTRegion < iNumJetBDTRegion; iJetBDTRegion++ ){
      if ( dJetBDTScore >= m_vJetBDTRegion.at(iJetBDTRegion*2) and dJetBDTScore <= m_vJetBDTRegion.at(iJetBDTRegion*2+1)){
	m_aAccept.setCutResult( "JetBDTScore", true );
	return true;
      }
    }
    ATH_MSG_VERBOSE( "Tau failed JetBDTScore requirement, tau JetBDTScore: " << dJetBDTScore );
    return false;
  }

  bool TauSelectionTool::checkJetIDWP( const xAOD::TauJet& xTau ) const{
    // check Jet ID working point, if tau passes JetID working point then return true; false otherwise 
    m_aAccept.addCut( "JetIDWP",
		      "Selection of taus according to their JetIDScore" );
    bool pass = false;
    if (m_iJetIDWP == JetBDTNONE)
      if (!xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose)) 
	pass = true;
    if (m_iJetIDWP == JetBDTLoose)
      if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose)) 
	pass = true;
    if (m_iJetIDWP == JetBDTMedium)
      if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium)) 
	pass = true;
    if (m_iJetIDWP == JetBDTTight)
      if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight)) 
	pass = true;
    if (m_iJetIDWP == JetBDTLooseNotMedium)
      if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium))
        pass = true;
    if (m_iJetIDWP == JetBDTLooseNotTight)
      if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight))
        pass = true;
    if (m_iJetIDWP == JetBDTMediumNotTight)
      if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight))
        pass = true;
    if (pass){
      m_aAccept.setCutResult( "JetIDWP", true );
      return true;
    }
    ATH_MSG_VERBOSE( "Tau failed JetID working point requirement" );
    return false;
  }

  bool TauSelectionTool::checkEleBDTScore( const xAOD::TauJet& xTau ) const{
    // check EleBDTscore, if tau has a EleBDT score in one of the regions requiered then return true; false otherwise 
    m_aAccept.addCut( "EleBDTScore",
		      "Selection of taus according to their EleBDTScore" );
    double dEleBDTScore = xTau.discriminant(xAOD::TauJetParameters::TauID(16));
    unsigned int iNumEleBDTRegion = m_vEleBDTRegion.size()/2;    
    for( unsigned int iEleBDTRegion = 0; iEleBDTRegion < iNumEleBDTRegion; iEleBDTRegion++ ){
      if ( xTau.discriminant(xAOD::TauJetParameters::TauID(15)) >= m_vEleBDTRegion.at(iEleBDTRegion*2) and xTau.discriminant(xAOD::TauJetParameters::TauID(15)) <= m_vEleBDTRegion.at(iEleBDTRegion*2+1)){
	m_aAccept.setCutResult( "EleBDTScore", true );
	return true;
      }
    }
    ATH_MSG_VERBOSE( "Tau failed EleBDTScore requirement, tau EleBDTScore: " << dEleBDTScore );
    return false;
  }

  bool TauSelectionTool::checkEleBDTWP( const xAOD::TauJet& xTau ) const{
    // check EleBDTscore, if tau passes EleBDT working point then return true; false otherwise 
    m_aAccept.addCut( "EleBDTWP",
		      "Selection of taus according to their EleBDTScore" );
    bool pass = false;
    if (m_iEleBDTWP == EleBDTLoose)
      if (xTau.isTau(xAOD::TauJetParameters::ElectronVetoLoose )) 
	pass = true;
    if (m_iEleBDTWP == EleBDTMedium)
      if (xTau.isTau(xAOD::TauJetParameters::ElectronVetoMedium )) 
	pass = true;
    if (m_iEleBDTWP == EleBDTTight)
      if (xTau.isTau(xAOD::TauJetParameters::ElectronVetoTight )) 
	pass = true;
    if (pass){
      m_aAccept.setCutResult( "EleBDTWP", true );
      return true;
    }
    ATH_MSG_VERBOSE( "Tau failed EleBDT working point requirement" );
    return false;
  }
  

} // namespace TauAnalysisTools
