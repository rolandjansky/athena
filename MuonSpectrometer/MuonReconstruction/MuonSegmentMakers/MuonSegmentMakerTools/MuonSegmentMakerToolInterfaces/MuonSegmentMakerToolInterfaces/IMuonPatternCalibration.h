/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTMAKETOOLINTERFACES_MUON_IMUONPATTERNCALIBRATION_H
#define MUONSEGMENTMAKETOOLINTERFACES_MUON_IMUONPATTERNCALIBRATION_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
//

namespace Muon {
    class MuonClusterOnTrack;
    class MdtDriftCircleOnTrack;

    /** @brief The IMuonCalibration is a pure virtual interface for tools to calibrate PRD hits  */
    class IMuonPatternCalibration : virtual public IAlgTool {
    public:
        typedef std::vector<const MuonClusterOnTrack*> ClusterVec;
        typedef std::vector<const MdtDriftCircleOnTrack*> MdtVec;
        typedef std::vector<MdtVec> MdtVecVec;

        struct ROTRegion {
            int regionId{0};
            Amg::Vector3D regionPos{0., 0., 0.};
            Amg::Vector3D regionDir{0., 0., 0.};
            ClusterVec clusters;
            MdtVecVec mdts;
        };

        typedef std::vector<ROTRegion> ROTsPerRegion;

        /** access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonPatternCalibration("Muon::IMuonPatternCalibration", 1, 0);
            return IID_IMuonPatternCalibration;
        }

        virtual void calibrate(const MuonPatternCombination& pat, ROTsPerRegion& hitsPerRegion, const EventContext& ctx) const = 0;
        virtual int getRegionId(const Identifier& id) const = 0;
        virtual void clearRotsPerRegion(ROTsPerRegion& hitsPerRegion) const = 0;
        virtual bool checkForPhiMeasurements(const MuonPatternCombination& pat) const = 0;

        virtual ~IMuonPatternCalibration() = default;
    };

}  // namespace Muon

#endif
