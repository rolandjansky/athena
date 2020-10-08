/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : JetMissingEtIdentificationTagTool.cxx
Package : offline/PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools
Purpose : create a JetMissingEtIdentificationTag - word to encode Jet and 
          Mising Et specific information
*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODJet/JetContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "JetMissingEtTagTools/JetMissingEtIdentificationTagTool.h"
#include "TagEvent/JetMissingEtIdentificationAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include "JetMissingEtTagTools/JetMissingEtTagTool.h"

// define some global/static selectors
// looseBadTool, tightBadTool and isUglyTool are defined here
#include "JetSelectorDefs.h"

#include <vector>
/** the constructor */
JetMissingEtIdentificationTagTool::JetMissingEtIdentificationTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 

  AthAlgTool( type, name, parent ){

  /** AOD Container Names */
  declareProperty("JetContainer",    m_jetContainerName    = "AntiKt4TopoJets_TAGcalibskim");
  declareProperty("MissingEtObject", m_missingEtObjectName = "MET_RefFinal");

  /** Pt cut on jte - modifiable in job options */
  declareProperty("BadEtCut",        m_badjetPtCut = 20.0*CLHEP::GeV);

  /** use EM scale instead of calibrated scale */
  declareProperty("UseEMScale",      m_useEMScale=false);

  declareInterface<JetMissingEtIdentificationTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  JetMissingEtIdentificationTagTool::initialize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in intialize()" << endmsg;


  CHECK(initJetSelectors());

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetMissingEtIdentificationTagTool::attributeSpecification(
           std::map<std::string,AthenaAttributeType>& attrMap, const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endmsg;

  /** specify the Jet and MissingET the attributes */

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

  // determine jet scale to use
  xAOD::JetScale scale = m_useEMScale ? xAOD::JetEMScaleMomentum : xAOD::JetAssignedScaleMomentum ;

  unsigned int AnyBadJet = 0x0;

  /** select and store jets that pass selection cuts into 'selecteJets' vector */
  for ( auto *calibratedJet : *jetContainer ) {

    const xAOD::JetFourMom_t &jetP4 = calibratedJet->jetP4(scale);

    /** select and store Jets */
    double pt = jetP4.Pt();

    ATH_MSG_DEBUG( " Before touching signal state:" << pt );
    if ( pt < m_badjetPtCut ) continue;

    if (!isUglyTool->accept(*calibratedJet))                 AnyBadJet |= 1<<0;
    if (!looseBadTool->accept( *calibratedJet))              AnyBadJet |= 1<<2;
    if (!tightBadTool->accept( *calibratedJet))              AnyBadJet |= 1<<4;
    
  }

  jetMissingEtTagCol.insert( JetMissingEtAttributeNames[0], AnyBadJet);

  return StatusCode::SUCCESS;
}


/** finialize - called once at the end */
StatusCode JetMissingEtIdentificationTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

