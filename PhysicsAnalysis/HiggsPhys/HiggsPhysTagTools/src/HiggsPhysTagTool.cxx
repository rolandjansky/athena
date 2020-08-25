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

#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "HiggsPhysTagTools/HiggsPhysTagTool.h"
#include "TagEvent/HiggsPhysAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
HiggsPhysTagTool::HiggsPhysTagTool (const std::string& type, const 
std::string& name, const IInterface* parent) : 
  AthAlgTool( type, name, parent ) {

  /** Container Names */
  declareProperty("JetContainer",  m_jetContainerName = "AntiKt4TopoJets");

  /** Pt cut on jte - modifiable in job options */
  declareProperty("EtCut",        m_jetPtCut = 15.0*CLHEP::GeV);
  
  declareInterface<HiggsPhysTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode HiggsPhysTagTool::initialize() {
  ATH_MSG_DEBUG( "in intialize()" );
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode HiggsPhysTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                    const int max) {

  ATH_MSG_DEBUG( "in attributeSpecification()" );

  /** specifiy the HiggsPhys the attributes */

  attrMap[ HiggsAttributeNames[0] ] = AthenaAttributeType("unsigned int", HiggsAttributeUnitNames[0], HiggsAttributeGroupNames[0]);

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode HiggsPhysTagTool::execute(TagFragmentCollection& higgsTagCol, const int max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** fill the HiggsPhys analysis tag */

  unsigned int fragment = 0x0;
  higgsTagCol.insert( HiggsAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode HiggsPhysTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}
