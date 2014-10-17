/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : JetMissingEtIdentificationTagTool.cxx
Package : offline/PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a JetMissingEtIdentificationTag - word to encode Jet and 
          Mising Et specific information

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODJet/JetContainer.h"

#include "JetUtils/JetCaloQualityUtils.h"

// #include "MissingETEvent/MissingET.h"

#include "JetMissingEtTagTools/JetMissingEtIdentificationTagTool.h"
#include "TagEvent/JetMissingEtIdentificationAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include "JetMissingEtTagTools/JetMissingEtTagTool.h"

// define some global/static selectors
// veryLooseBadTool, looseBadTool, etc... are defined here
#include "JetSelectorDefs.h"

/** the constructor */
JetMissingEtIdentificationTagTool::JetMissingEtIdentificationTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
    AthAlgTool( type, name, parent ) {

  /** AOD Container Names */
  declareProperty("JetContainer",    m_jetContainerName = "AntiKt4LCTopoJets");
  declareProperty("MissingEtObject", m_missingEtObjectName = "MET_RefFinal");

  /** Pt cut on jte - modifiable in job options */
  declareProperty("BadEtCut",        m_badjetPtCut = 20.0*CLHEP::GeV);

  
  declareInterface<JetMissingEtIdentificationTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  JetMissingEtIdentificationTagTool::initialize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in intialize()" << endreq;


  CHECK(initJetSelectors());

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetMissingEtIdentificationTagTool::attributeSpecification(
           std::map<std::string,AthenaAttributeType>& attrMap, const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endreq;

  /** specifiy the Jet and MissingET the attributes */

  attrMap[ JetMissingEtAttributeNames[0] ] = AthenaAttributeType("unsigned int", JetMissingEtAttributeUnitNames[0], JetMissingEtAttributeGroupNames[0]);

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode JetMissingEtIdentificationTagTool::execute(TagFragmentCollection& jetMissingEtTagCol, 
                                                      const int /*max*/) {
  ATH_MSG_DEBUG("in execute()" );

  /** fill the Jet and MissingET analysis tag */
  
  const xAOD::JetContainer * jetContainer=0;
  StatusCode sc = evtStore()->retrieve( jetContainer, m_jetContainerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "No AOD Jet container ("<<m_jetContainerName<<") found in SG" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("AOD Jet container ("<<m_jetContainerName<<") successfully retrieved" );


  unsigned int AnyBadJet = 0x0;

  for (auto jetItr=jetContainer->begin(); jetItr != jetContainer->end(); ++jetItr){
    const xAOD::Jet* jet = *jetItr;

    ATH_MSG_DEBUG( " Before touching signal state:" << jet->pt() );
    if ( jet->pt() < m_badjetPtCut ) continue;

    if (jet::JetCaloQualityUtils::isUgly( *jetItr ))  AnyBadJet |= 1<<0;
    if (veryLooseBadTool->accept( *jet))               AnyBadJet |= 1<<1;
    if (looseBadTool->accept( *jet))                   AnyBadJet |= 1<<2;
    if (mediumBadTool->accept( *jet))                  AnyBadJet |= 1<<3;
    if (tightBadTool->accept( *jet))                   AnyBadJet |= 1<<4;
    
  }

  jetMissingEtTagCol.insert( JetMissingEtAttributeNames[0], AnyBadJet);

  /** add more stuff if necessary */
  //for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}


/** finialize - called once at the end */
StatusCode JetMissingEtIdentificationTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

/** destructor */
JetMissingEtIdentificationTagTool::~JetMissingEtIdentificationTagTool() {}

