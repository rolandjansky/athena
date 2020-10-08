/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : ExoticPhysTagTool.cxx
Package : offline/PhysicsAnalysis/ExoticPhys/ExoticPhysTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a ExoticPhysTag - a word to encode ExoticPhys specific information such
          as the results of hypotheses

*****************************************************************************/

#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "ExoticPhysTagTools/ExoticPhysTagTool.h"
#include "TagEvent/ExoticPhysAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
ExoticPhysTagTool::ExoticPhysTagTool (const std::string& type, const 
std::string& name, const IInterface* parent) : 
  AthAlgTool( type, name, parent ) {

  /** Container Names */
  declareProperty("JetContainer",  m_jetContainerName = "AntiKt4TopoJets");

  /** Pt cut on jte - modifiable in job options */
  declareProperty("EtCut",        m_jetPtCut = 15.0*CLHEP::GeV);
  
  declareInterface<ExoticPhysTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode ExoticPhysTagTool::initialize() {
  ATH_MSG_DEBUG( "in intialize()" );
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode ExoticPhysTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                     const int max) {

  ATH_MSG_DEBUG( "in attributeSpecification()" );

  /** specifiy the ExoticPhys the attributes */

  attrMap[ ExoticAttributeNames[0] ] = AthenaAttributeType("unsigned int", ExoticAttributeUnitNames[0], ExoticAttributeGroupNames[0]);

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode ExoticPhysTagTool::execute(TagFragmentCollection& exoticTagCol, const int max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** fill the ExoticPhys analysis tag */

  unsigned int fragment = 0x0;
  exoticTagCol.insert( ExoticAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode  ExoticPhysTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}


