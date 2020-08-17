/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : SUSYPhysTagTool.cxx
Package : offline/PhysicsAnalysis/SUSYPhys/SUSYPhysTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a SUSYPhysTag - a word to encode SUSYPhys specific information such
          as the results of hypotheses

*****************************************************************************/

#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"


#include "JetEvent/JetCollection.h"

#include "SUSYPhysTagTools/SUSYPhysTagTool.h"
#include "TagEvent/SUSYPhysAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
SUSYPhysTagTool::SUSYPhysTagTool (const std::string& type, const 
std::string& name, const IInterface* parent) : 
  AthAlgTool( type, name, parent ) {

  /** AOD Container Names */
  declareProperty("JetContainer",    m_jetContainerName = "AntiKt4TopoJets");

  /** Pt cut on jte - modifiable in job options */
  declareProperty("EtCut",        m_jetPtCut = 15.0*CLHEP::GeV);
  
  declareInterface<SUSYPhysTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  SUSYPhysTagTool::initialize() {
  ATH_MSG_DEBUG( "in intialize()" );
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode SUSYPhysTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                   const int max) {

  ATH_MSG_DEBUG( "in attributeSpecification()" );

  /** specifiy the SUSYPhys the attributes */

  attrMap[ SusyAttributeNames[0] ] = AthenaAttributeType("unsigned int", SusyAttributeUnitNames[0] , SusyAttributeGroupNames[0]);

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode SUSYPhysTagTool::execute(TagFragmentCollection& susyTagCol, const int max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** fill the SUSYPhys analysis tag */

  unsigned int fragment = 0x0;
  susyTagCol.insert( SusyAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode  SUSYPhysTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}
