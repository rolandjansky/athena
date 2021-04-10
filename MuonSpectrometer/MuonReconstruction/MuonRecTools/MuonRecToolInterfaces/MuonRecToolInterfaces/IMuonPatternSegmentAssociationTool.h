/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonPatternSegmentAssociationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUONMUONPATTERNSEGMENTASSOCIATIONTOOL_H
#define IMUONMUONPATTERNSEGMENTASSOCIATIONTOOL_H

#include <map>

#include "GaudiKernel/IAlgTool.h"
#include "MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h"

namespace Muon {
    class MuonSegmentCombination;
    class MuonPatternCombination;

    static const InterfaceID IID_IMuonPatternSegmentAssociationTool("Muon::IMuonPatternSegmentAssociationTool", 1, 0);

    class IMuonPatternSegmentAssociationTool : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID();

        typedef MuonSegmentCombPatternCombAssociationMap AssociationMap;
        typedef std::pair<AssociationMap::const_iterator, AssociationMap::const_iterator> AssociationMapRange;

        virtual const AssociationMap& map() const = 0;

        virtual void insert(const MuonSegmentCombination* segmentCombi, const MuonPatternCombination* associatedPatternCombi) const = 0;

        virtual void reset() const = 0;

        virtual AssociationMapRange find(const MuonSegmentCombination* segmentCombi) const = 0;

        virtual unsigned int count(const MuonSegmentCombination* segmentCombi) const = 0;
    };

    inline const InterfaceID& Muon::IMuonPatternSegmentAssociationTool::interfaceID() { return IID_IMuonPatternSegmentAssociationTool; }

}  // namespace Muon

#endif
