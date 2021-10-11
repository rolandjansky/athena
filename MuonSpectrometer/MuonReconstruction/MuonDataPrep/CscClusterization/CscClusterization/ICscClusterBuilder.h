/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
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

#include <map>
#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

class ICscClusterBuilder : virtual public IAlgTool {
public:  // Static methods
    // Return the interface ID.
    static const InterfaceID& interfaceID() {
        /** Must declare this, with name of interface*/
        static const InterfaceID IID_ICscClusterBuilder("ICscClusterBuilder", 1, 0);
        return IID_ICscClusterBuilder;
    }

    virtual StatusCode getClusters(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect,
                                   Muon::CscPrepDataContainer* object) = 0;

    virtual ~ICscClusterBuilder() = default;
};
#endif
