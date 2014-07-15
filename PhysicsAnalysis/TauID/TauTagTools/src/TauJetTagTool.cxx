/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : TauJetTagTool.cxx
Package : offline/PhysicsAnalysis/TauID/TauTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a collection of TauJetTag

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "StoreGate/StoreGateSvc.h"

//#include "tauEvent/TauJetContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauJetParameters.h"

#include "TauTagTools/TauJetTagTool.h"
#include "TagEvent/TauJetAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <sstream>

//using namespace Analysis;

/** the constructor */
TauJetTagTool::TauJetTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
    AlgTool( type, name, parent ) {

  /** TauJet AOD Container Name */
  declareProperty("Container",     m_containerName = "TauJetCollection");

  /** selection cut of Pt */
  declareProperty("EtCut",              m_tauJetPtCut = 15.0*CLHEP::GeV);
  declareProperty("TauDetailContainer", m_tauDetailContainer = "TauRecDetailsContainer");
 
  declareInterface<TauJetTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  TauJetTagTool::initialize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in intialize()" << endreq;
  
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc"
         << endreq;
    return sc;
  }
  

  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode TauJetTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode TauJetTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                 const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endreq;

  
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

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in execute()" << endreq;

 
  /** retrieve the AOD tauJet container */
  const xAOD::TauJetContainer *tauJetContainer;
  StatusCode sc = m_storeGate->retrieve( tauJetContainer, m_containerName);
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "No AOD TauJet container found in SG" << endreq;
    return StatusCode::SUCCESS;
  }
  mLog << MSG::DEBUG << "AOD TauJet container successfully retrieved" << endreq;

  xAOD::TauJetContainer userContainer(  SG::VIEW_ELEMENTS );
				      //tauJetContainer->begin(),tauJetContainer->end(),

  userContainer = *tauJetContainer;
 //std::copy (const_cast<TauJetContainer*>(tauJetContainer)->begin(), const_cast<TauJetContainer*>(tauJetContainer)->end(), std::back_inserter(userContainer));
  AnalysisUtils::Sort::pT( &userContainer );//CHANGE ALSO IN THE OTHER CODE

  /** make the selection */
  int i=0;
  xAOD::TauJetContainer::const_iterator tauJetItr  = userContainer.begin();
  xAOD::TauJetContainer::const_iterator tauJetItrE = userContainer.end();
  for (; tauJetItr != tauJetItrE; ++tauJetItr) { 
    /** select  TauJets */

    /////////ASK FOR THE DETAILS NEW FUNCTION

    //    const Analysis::TauCommonDetails* theDetails = (*tauJetItr)->details<Analysis::TauCommonDetails>(m_tauDetailContainer);
 
    //    if ( !theDetails ) continue; 
    
    mLog << MSG::INFO << "Before the tau selection" << endreq;

    bool select =  ( (*tauJetItr)->pt() > m_tauJetPtCut ) && 
      //      ( (*tauJetItr)->numTrack()==1 ||  (*tauJetItr)->numTrack()==3 ) &&
      ( (*tauJetItr)->nTracks()==1 ||  (*tauJetItr)->nTracks()==3 ) &&
      ( fabs((*tauJetItr)->charge()) == 1.0 )    && 
      ( (*tauJetItr)->isTau(xAOD::TauJetParameters::JetBDTSigLoose)) ;


    if ( select ) { 
  
       if ( i<max ) {
	 
	 mLog << MSG::INFO << "While doing the tau selection" << endreq;
    
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

	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::EleBDTLoose))       pid |= 1<<8;
	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::EleBDTMedium))      pid |= 1<<9;
	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::EleBDTTight))       pid |= 1<<10;
	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::MuonVeto))          pid |= 1<<11;
	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium))   pid |= 1<<12;
	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::JetBDTSigTight))    pid |= 1<<13;
	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::TauLlhLoose))       pid |= 1<<14;
	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::TauLlhMedium))      pid |= 1<<15;
	  if ((*tauJetItr)->isTau(xAOD::TauJetParameters::TauLlhTight))       pid |= 1<<16;
	  
          tauJetTagColl.insert( m_pidStr[i], pid );

       }
   
       /** count the total number of tauJets */
       i++;
    }  
  }

  /** number of selected TauJet */
  tauJetTagColl.insert(TauJetAttributeNames[TauID::NTau], i);

  return StatusCode::SUCCESS;
}

/** destructor */
TauJetTagTool::~TauJetTagTool() {}


