/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BPhysTagTools
// Author: James Catmore (James.Catmore@cern.ch), Pavel Reznicek (Pavel.Reznicek@cern.ch),
// This is B-Physics TAG-word filling algorithm
// AS OF RELEASE 19 THIS IS NOW A DUMMY AND IS NOT DOING ANYTHING

#include "BPhysTagTools/BPhysTagTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "TagEvent/BPhysAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

///////////////////////////////////////////////////////////////////////////////

BPhysTagTool::BPhysTagTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent) {

  declareInterface<BPhysTagTool>(this);
}

///////////////////////////////////////////////////////////////////////////////

BPhysTagTool::~BPhysTagTool() {}

///////////////////////////////////////////////////////////////////////////////

StatusCode BPhysTagTool::initialize() {

  // Athena message stream
  ATH_MSG_DEBUG( "In intialize()" );
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

StatusCode BPhysTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int /*max*/) {

  ATH_MSG_DEBUG( "In attributeSpecification()" );

  // Build the attribute list - called in initialize and specify the BPhys the attributes
  attrMap[ BPhysAttributeNames[0] ] =AthenaAttributeType(std::string("unsigned int"),  BPhysAttributeUnitNames[0], BPhysAttributeGroupNames[0]);

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

StatusCode BPhysTagTool::execute(TagFragmentCollection& bphysTagColl, const int /*max*/) {

  ATH_MSG_DEBUG( "In execute()" );

  //=============================================================================
  // Fill the B-physics TAG word ================================================
  //=============================================================================

  // The B-physics word (32 bits)
  unsigned int bphysWord = 0x0;

  // Finally write the B-physics word to the TAG file
  bphysTagColl.insert(BPhysAttributeNames[0],bphysWord);

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

StatusCode BPhysTagTool::finalize() {

  ATH_MSG_DEBUG( "In finalize()" );

  return StatusCode::SUCCESS;
}
