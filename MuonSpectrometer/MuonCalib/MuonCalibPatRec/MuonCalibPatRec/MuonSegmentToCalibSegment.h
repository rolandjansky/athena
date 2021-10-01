/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MUONSEGMENTTOCALIBSEGMENT_H
#define MUONCALIB_MUONSEGMENTTOCALIBSEGMENT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MdtCalibSvc/MdtCalibrationTool.h"
#include "MuonCalibEvent/MuonCalibPatternCollection.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"

namespace Muon {
    class MuonPatternCombination;
}

namespace MuonCalib {

    /** @class MuonSegmentToCalibSegment
    Algorithm to retrieve MuonSegments or MuonSegmentCombinations from StoreGate and
    output the muon calibration input.
        @author Niels.Van.Eldik@cern.ch
    */

    class MuonSegmentToCalibSegment : public AthAlgorithm {
    public:
        /** Algorithm constructor */
        MuonSegmentToCalibSegment(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~MuonSegmentToCalibSegment() = default;

        /** Algorithm initialize */
        StatusCode initialize();

        /** Algorithm execute, called once per event */
        StatusCode execute();

    private:
        template <class container_type>
        StatusCode retrieveContainer(const EventContext& ctx, const SG::ReadHandleKey<container_type>& key,
                                     const container_type*& container_ptr) const;

        StatusCode convertPatterns(const EventContext& ctx);

        /** save global patterns to storegate */
        StatusCode savePatterns(const EventContext& ctx, std::unique_ptr<MuonCalibPatternCollection> newPatterns) const;

        MuonCalibSegment* createMuonCalibSegment(const Muon::MuonSegment& seg, const MuonGM::MuonDetectorManager* MuonDetMgr) const;
        MuonCalibPattern* createMuonCalibPattern(const Muon::MuonPatternCombination* pat) const;
        Identifier getChId(const Muon::MuonSegment& seg) const;
        Amg::Transform3D getGlobalToStation(const Identifier& id, const MuonGM::MuonDetectorManager* MuonDetMgr) const;

        unsigned int getQuality(const Muon::MuonSegment& seg) const;

        /**  segment location */
        Gaudi::Property<bool> m_readSegments{this, "ReadSegments", false};
        Gaudi::Property<bool> m_useCscSegments{this, "UseCscSegments", false};

        SG::ReadHandleKey<MuonSegmentCombinationCollection> m_CombSegKey{this, "CombiSegmentKey", "MooreSegmentCombinations"};

        SG::ReadHandleKey<MuonSegmentCombinationCollection> m_CscSegKey{this, "CombiSegmentKey", "Csc4dSegmentCombinations"};

        SG::ReadHandleKeyArray<Trk::SegmentCollection> m_TrkSegKey{this, "SegmentLocations", {"MooreSegments", "ConvertedMBoySegments"}};

        Gaudi::Property<std::vector<int>> m_segment_authors{this, "SegmentAuthors", {3, 4}};
        Gaudi::Property<bool> m_newImpactParameter{this, "NewImpactParameter", false};

        /** pattern location */

        SG::WriteHandleKey<MuonCalibPatternCollection> m_patternKey{this, "PatternKey", "PatternsForCalibration"};

        /** MuonDetectorManager from the conditions store */
        SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                                "Key of input MuonDetectorManager condition data"};

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        /** pointer to MdtCalibSvc */
        ToolHandle<MdtCalibrationTool> m_calibrationTool{this, "CalibrationTool", "MdtCalibrationTool"};

        /** IdentifierTool initialization */
        ToolHandle<Muon::IMuonPatternSegmentAssociationTool> m_assocTool{
            this, "PatternSegmentAssociationTool", "Muon::MuonPatternSegmentAssociationTool/MuonPatternSegmentAssociationTool"};
        ToolHandle<IIdToFixedIdTool> m_idToFixedIdTool{this, "IdToFixedIdTool", "MuonCalib::IdToFixedIdTool/MuonCalib_IdToFixedIdTool"};

        /** -1: Take infirmation from error-strategy
             0: fitted t0 is not applied to drift times
             1: fitted t0 is applied to drift time
             */
        Gaudi::Property<int> m_updateForT0Shift{this, "UpdateForT0Shift", -1};
        Gaudi::Property<bool> m_doTof{this, "DoTOF", true};
        Gaudi::Property<bool> m_cosmics_tof{this, "DoCosmicsTof", false};
        /** maximum number of segments each algorithm can store in ntuple */
        Gaudi::Property<int> m_maxStoredSegs{this, "MaxPossibleSegments", 1000};
    };

}  // namespace MuonCalib

#endif
