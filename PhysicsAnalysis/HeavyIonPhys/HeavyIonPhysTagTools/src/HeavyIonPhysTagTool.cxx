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
#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "HeavyIonPhysTagTools/HeavyIonPhysTagTool.h"
#include "TagEvent/HeavyIonPhysAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** the constructor */
HeavyIonPhysTagTool::HeavyIonPhysTagTool (const std::string& type, const 
std::string& name, const IInterface* parent) : 
    AthAlgTool( type, name, parent )
{

  /** Pt cut on jte - modifiable in job options */
  declareProperty("EtCut",        m_jetPtCut = 15.0*CLHEP::GeV);
  
  declareInterface<HeavyIonPhysTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode HeavyIonPhysTagTool::initialize() {
  ATH_MSG_DEBUG( "in intialize()" );
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode HeavyIonPhysTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                       const int max) {

  ATH_MSG_DEBUG( "in attributeSpecification()" );

  /** specifiy the HeavyIonPhys the attributes */

  attrMap[ HeavyIonAttributeNames[0] ] = AthenaAttributeType("unsigned int", HeavyIonAttributeUnitNames[0], HeavyIonAttributeGroupNames[0]);

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode HeavyIonPhysTagTool::execute(TagFragmentCollection& heavyIonTagCol, const int max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** fill the HeavyIonPhys analysis tag */

  unsigned int fragment = 0x0;
  heavyIonTagCol.insert( HeavyIonAttributeNames[0], fragment );

  /** add more stuff if necessary */
  for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode HeavyIonPhysTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}



