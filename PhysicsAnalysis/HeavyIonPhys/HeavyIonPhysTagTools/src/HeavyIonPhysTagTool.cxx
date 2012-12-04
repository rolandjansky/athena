/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : HeavyIonPhysTagTool.cxx
Package : offline/PhysicsAnalysis/HeavyIonPhys/HeavyIonPhysTagTools
Author  : Ketevi A. Assamagan
Created : June 2006
Purpose : create a HeavyIonPhysTag - a word to encode ExoticPhys specific information such
          as the results of hypotheses

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "StoreGate/StoreGateSvc.h"

#include "HeavyIonPhysTagTools/HeavyIonPhysTagTool.h"
#include "TagEvent/HeavyIonPhysAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
HeavyIonPhysTagTool::HeavyIonPhysTagTool (const std::string& type, const 
std::string& name, const IInterface* parent) : 
    AlgTool( type, name, parent ) {

  /** Pt cut on jte - modifiable in job options */
  declareProperty("EtCut",        m_jetPtCut = 15.0*CLHEP::GeV);
  
  declareInterface<HeavyIonPhysTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode HeavyIonPhysTagTool::initialize() {
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
StatusCode HeavyIonPhysTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                       const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endreq;

  /** specifiy the HeavyIonPhys the attributes */

  attrMap[ HeavyIonAttributeNames[0] ] = AthenaAttributeType("unsigned int", HeavyIonAttributeUnitNames[0], HeavyIonAttributeGroupNames[0]);

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode HeavyIonPhysTagTool::execute(TagFragmentCollection& heavyIonTagCol, const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in execute()" << endreq;

  /** fill the HeavyIonPhys analysis tag */

  unsigned int fragment = 0x0;
  heavyIonTagCol.insert( HeavyIonAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode HeavyIonPhysTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

/** destructor */
HeavyIonPhysTagTool::~HeavyIonPhysTagTool() {}


