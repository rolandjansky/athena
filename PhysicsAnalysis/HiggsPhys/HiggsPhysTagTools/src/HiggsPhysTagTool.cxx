/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : HiggsPhysTagTool.cxx
Package : offline/PhysicsAnalysis/HiggsPhys/HiggsPhysTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a HiggsPhysTag - a word to encode HiggsPhys specific information such
          as the results of hypotheses

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "StoreGate/StoreGateSvc.h"

#include "JetEvent/JetCollection.h"

#include "HiggsPhysTagTools/HiggsPhysTagTool.h"
#include "TagEvent/HiggsPhysAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
HiggsPhysTagTool::HiggsPhysTagTool (const std::string& type, const 
std::string& name, const IInterface* parent) : 
    AlgTool( type, name, parent ) {

  /** Container Names */
  declareProperty("JetContainer",  m_jetContainerName = "AntiKt4TopoJets");

  /** Pt cut on jte - modifiable in job options */
  declareProperty("EtCut",        m_jetPtCut = 15.0*CLHEP::GeV);
  
  declareInterface<HiggsPhysTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode HiggsPhysTagTool::initialize() {
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

/** build the attribute list - called in initialize */
StatusCode HiggsPhysTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                    const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endreq;

  /** specifiy the HiggsPhys the attributes */

  attrMap[ HiggsAttributeNames[0] ] = AthenaAttributeType("unsigned int", HiggsAttributeUnitNames[0], HiggsAttributeGroupNames[0]);

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode HiggsPhysTagTool::execute(TagFragmentCollection& higgsTagCol, const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in execute()" << endreq;

  /** fill the HiggsPhys analysis tag */

  unsigned int fragment = 0x0;
  higgsTagCol.insert( HiggsAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode HiggsPhysTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

/** destructor */
HiggsPhysTagTool::~HiggsPhysTagTool() {}

