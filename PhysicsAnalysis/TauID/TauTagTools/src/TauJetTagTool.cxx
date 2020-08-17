/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : TauJetTagTool.cxx
Package : offline/PhysicsAnalysis/TauID/TauTagTools
Purpose : create a collection of TauJetTag

*****************************************************************************/

#include "Gaudi/Property.h"

#include "StoreGate/StoreGateSvc.h"

#include "xAODTau/TauJetContainer.h"

#include "TauTagTools/TauJetTagTool.h"
#include "TagEvent/TauJetAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include "TauAnalysisTools/ITauSelectionTool.h"

#include <sstream>

//using namespace Analysis;

/** the constructor */
TauJetTagTool::TauJetTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  m_eleBDTLoose(),
  m_eleBDTMedium(),
  m_eleBDTTight(),
  m_muonVeto(),
  m_jetBDTLoose(),
  m_jetBDTMedium(),
  m_jetBDTTight(),
  // m_jetLLHLoose(),
  // m_jetLLHMedium(),
  // m_jetLLHTight(),
  m_eleOLR(),
  m_tauJetPtCut(15000.),
  m_containerName()
{

  /** TauJet AOD Container Name */
  declareProperty("Container",     m_containerName);

  /** selection cut of Pt */ 
  declareProperty("EtCut",         m_tauJetPtCut = 15.0*CLHEP::GeV); 

  /** selection tool */
  declareProperty("EleBDTLooseTauSelectionTool",     m_eleBDTLoose );
  declareProperty("EleBDTMediumTauSelectionTool",    m_eleBDTMedium);
  declareProperty("EleBDTTightTauSelectionTool",     m_eleBDTTight );
  declareProperty("MuonVetoTauSelectionTool",        m_muonVeto    );
  declareProperty("JetBDTSigLooseTauSelectionTool",  m_jetBDTLoose );
  declareProperty("JetBDTSigMediumTauSelectionTool", m_jetBDTMedium);
  declareProperty("JetBDTSigTightTauSelectionTool",  m_jetBDTTight );
  // declareProperty("JetLLHSigLooseTauSelectionTool",  m_jetLLHLoose );
  // declareProperty("JetLLHSigMediumTauSelectionTool", m_jetLLHMedium);
  // declareProperty("JetLLHSigTightTauSelectionTool",  m_jetLLHTight );
  declareProperty("EleOLRTauSelectionTool",          m_eleOLR );

  declareInterface<TauJetTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  TauJetTagTool::initialize() {
  ATH_MSG_DEBUG( "in intialize()" );
  ATH_CHECK(m_eleBDTLoose.retrieve());
  ATH_CHECK(m_eleBDTMedium.retrieve());
  ATH_CHECK(m_eleBDTTight.retrieve());
  ATH_CHECK(m_muonVeto.retrieve());	
  ATH_CHECK(m_jetBDTLoose.retrieve());
  ATH_CHECK(m_jetBDTMedium.retrieve());
  ATH_CHECK(m_jetBDTTight.retrieve());
  ATH_CHECK(m_eleOLR.retrieve());
  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode TauJetTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode TauJetTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                 const int max) {

  ATH_MSG_DEBUG( "in attributeSpecification()" );

  
  /** specifiy the TauJet the attributes */

  /** number of TauJet */
  attrMap[ TauJetAttributeNames[TauID::NTau] ] = AthenaAttributeType("unsigned int",  TauJetAttributeUnitNames[TauID::NTau],  TauJetAttributeGroupNames[TauID::NTau]);

  std::ostringstream os;
  for (int i=1; i<= max; ++i) {

    /** pt */
    os.str(""); 
    os << TauJetAttributeNames[TauID::Pt] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", TauJetAttributeUnitNames[TauID::Pt],  TauJetAttributeGroupNames[TauID::Pt]);
    m_ptStr.push_back( os.str() );

  
    /** eta */
    os.str(""); 
    os << TauJetAttributeNames[TauID::Eta] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", TauJetAttributeUnitNames[TauID::Eta],  TauJetAttributeGroupNames[TauID::Eta]);
    m_etaStr.push_back( os.str() );
 
  
    /** phi */
    os.str(""); 
    os << TauJetAttributeNames[TauID::Phi] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", TauJetAttributeUnitNames[TauID::Phi],  TauJetAttributeGroupNames[TauID::Phi]);
    m_phiStr.push_back( os.str() );

  
     /** PID bitmask */
    os.str(""); 
    os << TauJetAttributeNames[TauID::PID] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", TauJetAttributeUnitNames[TauID::PID],  TauJetAttributeGroupNames[TauID::PID]);
    m_pidStr.push_back( os.str() );
    

  }

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode TauJetTagTool::execute(TagFragmentCollection& tauJetTagColl, const int max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** retrieve the AOD tauJet container */
  const xAOD::TauJetContainer *tauJetContainer;
  StatusCode sc = evtStore()->retrieve( tauJetContainer, m_containerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "No AOD TauJet container found in SG" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "AOD TauJet container successfully retrieved" );

  xAOD::TauJetContainer userContainer(  SG::VIEW_ELEMENTS );
  userContainer = *tauJetContainer;
  AnalysisUtils::Sort::pT( &userContainer );

  /** make the selection */
  int i=0;
  xAOD::TauJetContainer::const_iterator tauJetItr  = userContainer.begin();
  xAOD::TauJetContainer::const_iterator tauJetItrE = userContainer.end();

  for (; tauJetItr != tauJetItrE && i< max; ++tauJetItr) { 

    ATH_MSG_DEBUG( "Before the tau selection" );

    bool select =  ( (*tauJetItr)->pt() > m_tauJetPtCut )          &&   // pt cut 
      ( (*tauJetItr)->nTracks()==1 || (*tauJetItr)->nTracks()==3 ) &&   // 1 or 3 tracks 
      ( fabs((*tauJetItr)->charge()) == 1.0 )                      &&   // right charge 
      ( m_jetBDTLoose->accept(**tauJetItr) );                           // loose BDT preselection 

    if ( select ) { 
  
      ATH_MSG_DEBUG( "Filling selected tau" );
      
      /** pt */
      tauJetTagColl.insert( m_ptStr[i], (*tauJetItr)->pt() * (*tauJetItr)->charge() );
 
      /** eta */
      tauJetTagColl.insert( m_etaStr[i], (*tauJetItr)->eta() );
      
      /** phi */
      tauJetTagColl.insert( m_phiStr[i], (*tauJetItr)->phi() );
      
      /** PID */
      unsigned int pid = 0;
      if ((*tauJetItr)->nTracks()==1)       pid |= 1<<0;
      if ((*tauJetItr)->nTracks()==3)       pid |= 1<<1;
      
      if (m_eleBDTLoose->accept(**tauJetItr))    pid |= 1<<8;
      if (m_eleBDTMedium->accept(**tauJetItr))   pid |= 1<<9;
      if (m_eleBDTTight->accept(**tauJetItr))    pid |= 1<<10;
      if (m_muonVeto->accept(**tauJetItr))       pid |= 1<<11;
      if (m_jetBDTLoose->accept(**tauJetItr))    pid |= 1<<17;
      if (m_jetBDTMedium->accept(**tauJetItr))   pid |= 1<<12;
      if (m_jetBDTTight->accept(**tauJetItr))    pid |= 1<<13;
      // if (m_jetLLHLoose->accept(**tauJetItr))    pid |= 1<<14;
      // if (m_jetLLHMedium->accept(**tauJetItr))   pid |= 1<<15;
      // if (m_jetLLHTight->accept(**tauJetItr))    pid |= 1<<16;
      if (m_eleOLR->accept(**tauJetItr))         pid |= 1<<18;
      
      tauJetTagColl.insert( m_pidStr[i], pid );
      
      /** we got one more tau written */
      i++;

    }
   
  }

  /** number of selected TauJet */
  tauJetTagColl.insert(TauJetAttributeNames[TauID::NTau], i);

  return StatusCode::SUCCESS;
}
