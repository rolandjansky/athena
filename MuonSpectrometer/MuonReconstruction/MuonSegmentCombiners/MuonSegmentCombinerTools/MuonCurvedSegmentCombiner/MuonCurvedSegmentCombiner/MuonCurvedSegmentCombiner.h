/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMUONCURVEDSEGMENTCOMBINER_H
#define MUONMUONCURVEDSEGMENTCOMBINER_H

#include <map>
#include <set>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCurvedSegmentCombiner/MCSCSegmentInfo.h"
#include "MuonEDM_AssociationObjects/MuonSegPatAssMap.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegmentCombinerToolInterfaces/IMuonCurvedSegmentCombiner.h"

namespace Muon {
    /** @class MuonCurvedSegmentCombiner

    This is for the Doxygen-Documentation.
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.

    @author  Edward Moyse <Edward.Moyse@cern.ch>
    */

    class MuonCurvedSegmentCombiner : virtual public IMuonCurvedSegmentCombiner, public AthAlgTool {
    public:
        MuonCurvedSegmentCombiner(const std::string&, const std::string&, const IInterface*);

        virtual ~MuonCurvedSegmentCombiner() = default;

        virtual StatusCode initialize();

        /** INSERT main method here.*/
        std::unique_ptr<MuonSegmentCombinationCollection> combineSegments(const MuonSegmentCombinationCollection& mdtCombiColl,
                                                                          const MuonSegmentCombinationCollection& csc4DCombiColl,
                                                                          const MuonSegmentCombinationCollection& csc2DCombiColl,
                                                                          MuonSegmentCombPatternCombAssociationMap* segPattMap) const;

    private:
        typedef std::map<Muon::MuonSegment*, Muon::MCSCSegmentInfo> SegInfoMap;
        void trackParameters(Muon::MuonSegment& seg, double& theta, double& curvature, int& imeth, SegInfoMap segInfoMap) const;
        void extrapolateSegment(Muon::MuonSegment& seg, Muon::MuonSegment& sege, Amg::VectorX& Res, Amg::VectorX& Pull,
                                SegInfoMap segInfoMap) const;

        void fit2Segments(Muon::MuonSegment& seg, Muon::MuonSegment& sege, Amg::VectorX& Res, Amg::VectorX& Pull,
                          SegInfoMap segInfoMap) const;
        void fit2SegmentsC(Muon::MuonSegment& seg, Muon::MuonSegment& sege, Amg::VectorX& Res, Amg::VectorX& Pull,
                           SegInfoMap segInfoMap) const;
        void fit2SegmentsSL(Muon::MuonSegment& seg, Muon::MuonSegment& sege, Amg::VectorX& Res, Amg::VectorX& Pull,
                            SegInfoMap segInfoMap) const;
        std::unique_ptr<MuonSegmentCombinationCollection> processCombinationCollection(
            const MuonSegmentCombinationCollection& mdtCombiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap,
            const SegInfoMap& seg2DCscInfoMap, SegInfoMap seg4DCscInfoMap, SegInfoMap& segInfoMap,
            std::map<Muon::MuonSegment*, const MuonPatternCombination*>& segAssoMap, int& segmentIndex) const;
        void processCscCombinationCollection(const MuonSegmentCombinationCollection& cscCombiCol,
                                             MuonSegmentCombPatternCombAssociationMap* segPattMap, const SegInfoMap& segInfoMap,
                                             SegInfoMap& seg4DCscInfoMap, std::set<Identifier>& cscIdSet,
                                             std::map<Muon::MuonSegment*, const MuonPatternCombination*>& segAssoMap,
                                             int& segmentIndex) const;
        void process2DCscCombinationCollection(const MuonSegmentCombinationCollection& csc2DcombiCol, const SegInfoMap& segInfoMap,
                                               std::set<Identifier> cscIdSet, SegInfoMap& seg2DCscInfoMap,
                                               std::map<Muon::MuonSegment*, const MuonPatternCombination*>& segAssoMap,
                                               int& segmentIndex) const;

        void muonCurvedSegmentCombinations(MuonSegmentCombinationCollection* curvedCombiCol,
                                           MuonSegmentCombPatternCombAssociationMap* segPattMap, SegInfoMap segInfoMap,
                                           const std::map<Muon::MuonSegment*, const MuonPatternCombination*>& segAssoMap) const;

        Muon::MCSCSegmentInfo segInfo(Muon::MuonSegment* seg, SegInfoMap segInfoMap,
                                      std::map<Muon::MuonSegment*, const MuonPatternCombination*> segAssoMap) const;

        unsigned int missedHits(Muon::MuonSegment* segment) const;

    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
            this,
            "MuonIdHelperSvc",
            "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
        };

        ToolHandle<MuonEDMPrinterTool> m_printer{
            this,
            "Printer",
            "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
        };

        bool m_debug;
        bool m_doCosmics;
        bool m_summary;
        bool m_useCscSegments;
        bool m_addAll2DCscs;
        bool m_mergePatterns;

        int m_missedHitsCut;
        double m_pullAssociationCut;
        double m_phiAssociationCut1;
        double m_phiAssociationCut2;
        bool m_addUnassociatedMiddleEndcapSegments;

        typedef SegInfoMap::iterator SegInfoIt;
    };
}  // namespace Muon

#endif
