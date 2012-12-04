/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : StandardModelPhysTagTool.cxx
Package : offline/PhysicsAnalysis/StandardModelPhys/StandardModelPhysTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a StandardModelPhysTag - a word to encode StandardModelPhys specific information such
          as the results of hypotheses

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/GaudiException.h"

#include "StoreGate/StoreGateSvc.h"

#include "JetEvent/JetCollection.h"

#include "StandardModelPhysTagTools/StandardModelPhysTagTool.h"
#include "TagEvent/StandardModelPhysAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
StandardModelPhysTagTool::StandardModelPhysTagTool (const std::string& type, const 
std::string& name, const IInterface* parent) : 
    AlgTool( type, name, parent ) {

  /** AOD Container Names */
  declareProperty("JetContainer",    m_jetContainerName = "AntiKt4TopoJets");

  /** Pt cut on jte - modifiable in job options */
  declareProperty("EtCut",        m_jetPtCut = 15.0*CLHEP::GeV);
  
  declareInterface<StandardModelPhysTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode StandardModelPhysTagTool::initialize() {
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
StatusCode StandardModelPhysTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, 
                                                            const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endreq;

  /** specifiy the StandardModelPhys the attributes */

  attrMap[ SmAttributeNames[0] ] = AthenaAttributeType("unsigned int", SmAttributeUnitNames[0], SmAttributeGroupNames[0]) ;

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode StandardModelPhysTagTool::execute(TagFragmentCollection& smTagCol, const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in execute()" << endreq;

  /** fill the StandardModelPhys analysis tag */
  
  unsigned int fragment = 0x0;
  smTagCol.insert( SmAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode StandardModelPhysTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

/** destructor */
StandardModelPhysTagTool::~StandardModelPhysTagTool() {}



