/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTCOMBINATIONCLEANERTOOL_H
#define MUON_IMUONSEGMENTCOMBINATIONCLEANERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

namespace Muon {

    /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
    class IMuonSegmentCombinationCleanerTool : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentCombinationCleanerTool("Muon::IMuonSegmentCombinationCleanerTool", 1, 0);
            return IID_IMuonSegmentCombinationCleanerTool;
        }

        /** clean phi hits on a segment combination */
        virtual std::unique_ptr<MuonSegmentCombinationCollection> clean(const MuonSegmentCombinationCollection& combiCol,
                                                                        MuonSegmentCombPatternCombAssociationMap* segPattMap) const = 0;
    };

}  // namespace Muon

#endif
