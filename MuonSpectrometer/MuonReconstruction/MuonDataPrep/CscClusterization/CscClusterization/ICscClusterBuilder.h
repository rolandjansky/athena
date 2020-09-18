/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ICscClusterBuilder.h

#ifndef ICscClusterBuilder_H
#define ICscClusterBuilder_H

// David Adams and Woochun Park
// February 2007
//
// Interface for tool to fit a CSC cluster.
//
// Returns the coordinate in the measurement plane, i.e. in the
// wire plane in the direction transverse to the strips.
//
// Also optionally rerurns a map of intermediate values indexed by
// name. It is intended that this list be used to tune the tool
// and provide measures of data quality.

#include <vector>
#include <map>
#include "GaudiKernel/IAlgTool.h"
#include "MuonPrepRawData/CscClusterStatus.h"
//#include "CscClusterization/ICscStripBuilder.h"

namespace Muon {
  class CscStripPrepData;
}

/** Must declare this, with name of interface*/
static const InterfaceID IID_ICscClusterBuilder("ICscClusterBuilder", 1, 0);

class ICscClusterBuilder : virtual public IAlgTool {
  
public:  // Static methods
  
  // Return the interface ID.
  static const InterfaceID& interfaceID() { return IID_ICscClusterBuilder; }
  
public:  // Interface methods
  
  virtual StatusCode getClusters(std::vector<IdentifierHash>& idVect,  std::vector<IdentifierHash>& selectedIdVect, Muon::CscPrepDataContainer *object)=0;
};
#endif




