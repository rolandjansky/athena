/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : TauIdentificationTagTool.cxx
Package : offline/PhysicsAnalysis/TauID/TauTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a TauIdentificationTag - word to encode tauJet specific informations
*****************************************************************************/

#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TauTagTools/TauIdentificationTagTool.h"
#include "TagEvent/TauIdentificationAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
TauIdentificationTagTool::TauIdentificationTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
  AthAlgTool( type, name, parent ) {

  /** TauJet AOD Container Names */
  declareProperty("Container",   m_tauJetContainerName = "TauJetCollection");

  /** selection cut - modifiable in job options */
  declareProperty("EtCut",        m_tauJetPtCut = 15.0*CLHEP::GeV);
  declareProperty("Likelihood",   m_likelihood = -6.0);
  declareProperty("HadEnergyFrac",m_hadEnergyFrac = 0.9);
  
  declareInterface<TauIdentificationTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  TauIdentificationTagTool::initialize() {
  ATH_MSG_DEBUG( "in intialize()" );
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode TauIdentificationTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                            const int max) {

  ATH_MSG_DEBUG( "in attributeSpecification()" );

  /** specifiy the TauId the attributes */

  attrMap[ TauIdAttributeNames[0] ] = AthenaAttributeType("unsigned int",  TauIdAttributeUnitNames[0] ,  TauIdAttributeGroupNames[0] );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode TauIdentificationTagTool::execute(TagFragmentCollection& tauIdTagCol, const int max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** fill the TauIdPhys analysis tag */

  unsigned int fragment = 0x0;
  tauIdTagCol.insert( TauIdAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode TauIdentificationTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}
