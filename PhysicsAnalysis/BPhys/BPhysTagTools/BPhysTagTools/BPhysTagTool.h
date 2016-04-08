/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BPhysTagTools.h
// Author: James Catmore (James.Catmore@cern.ch), Pavel Reznicek (Pavel.Reznicek@cern.ch),
// This is B-Physics TAG-word filling algorithm
// AS OF RELEASE 19 THIS IS NOW A DUMMY AND IS NOT DOING ANYTHING

#ifndef BPHYSTAGTOOL_H
#define BPHYSTAGTOOL_H

#include <string>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

///////////////////////////////////////////////////////////////////////////////


// Interface ID for BPhysTagTool
static const InterfaceID IID_BPhysTagTool("BPhysTagTool", 1, 0);

class BPhysTagTool : public AthAlgTool {

public:

  // Standard Constructor
  BPhysTagTool(const std::string&, const std::string&, const IInterface*);

  // AlgTool and IAlgTool interface methods
  static const InterfaceID& interfaceID() { return IID_BPhysTagTool; };

  // Overriding initialize, finalize and execute
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>&, const int);
  virtual StatusCode execute(TagFragmentCollection&, const int);
  virtual StatusCode finalize();

protected:

  // Standard destructor
  virtual ~BPhysTagTool();

};

#endif // BPHYSTAGTOOL_H
