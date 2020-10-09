/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : JetTaggingTagTool.cxx
Package : offline/PhysicsAnalysis/JetTagging/JetTaggingTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a Flavor Tagging Event Tag - a word to encode Flavor Tagging specific 
           information, perhaps about different tagging options.

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "StoreGate/StoreGateSvc.h"

#include "JetEvent/JetCollection.h"

#include "JetTaggingTagTools/JetTaggingTagTool.h"
#include "TagEvent/JetTaggingAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
JetTaggingTagTool::JetTaggingTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
    AthAlgTool( type, name, parent ) {

  /** AOD Container Names */
  declareProperty("JetContainer",    m_jetContainerName = "AntiKt4H1TopoJets");

  /** Pt cut on jte - modifiable in job options */
  declareProperty("EtCut",        m_jetPtCut = 15.0*CLHEP::GeV);
  
  declareInterface<JetTaggingTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode JetTaggingTagTool::initialize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in intialize()" << endmsg;

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetTaggingTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                     const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endmsg;

  /** specifiy the Jet Tagging the attributes */

  attrMap[ JetTaggingAttributeNames[0] ] = AthenaAttributeType("unsigned int",JetTaggingAttributeUnitNames[0], JetTaggingAttributeGroupNames[0]);

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode JetTaggingTagTool::execute(TagFragmentCollection& jetTagCol, const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in execute()" << endmsg;

  /** fill the JetTagging analysis tag */

  unsigned int fragment = 0x0;
  jetTagCol.insert( JetTaggingAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode JetTaggingTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}



