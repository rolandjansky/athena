/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONPRDSELECTIONTOOL_H
#define MUON_IMUONPRDSELECTIONTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonLayerEvent/MuonLayerPrepRawData.h"
#include "MuonLayerEvent/MuonLayerROTs.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "TrkParameters/TrackParameters.h"

static const InterfaceID IID_IMuonPRDSelectionTool("Muon::IMuonPRDSelectionTool", 1, 0);

namespace Muon {

    class MuonSegment;
    class MdtDriftCircleOnTrack;
    class MuonClusterOnTrack;

    /** Interface for tools calculating hit count summaries for track */
    class IMuonPRDSelectionTool : virtual public IAlgTool {
    public:
        /** IAlgTool interface */
        static const InterfaceID& interfaceID();

        /** IMuonPRDSelectionTool interface: calibrateAndSelect */
        virtual bool calibrateAndSelect(const MuonSystemExtension::Intersection& intersection, const MuonLayerPrepRawData& layerPrepRawData,
                                        MuonLayerROTs& layerROTs) const = 0;

        /** IMuonPRDSelectionTool interface: calibrateAndSelectMdt */
        virtual bool calibrateAndSelectMdt(const MuonSystemExtension::Intersection& intersection, const MdtPrepDataCollection& prds,
                                           std::vector<const MdtDriftCircleOnTrack*>& rots) const = 0;

        /** IMuonPRDSelectionTool interface:  calibrate and select single MDT */
        virtual const MdtDriftCircleOnTrack* calibrateAndSelect(const MuonSystemExtension::Intersection& intersection,
                                                                const MdtPrepData& mdt) const = 0;

        /** IMuonPRDSelectionTool interface:  calibrate and select single cluster */
        virtual const MuonClusterOnTrack* calibrateAndSelect(const Trk::TrackParameters& pars, const MuonCluster& clus) const = 0;
        virtual const MuonClusterOnTrack* calibrateAndSelect(const MuonSystemExtension::Intersection& intersection,
                                                             const MuonCluster& clus) const = 0;
    };

    inline const InterfaceID& IMuonPRDSelectionTool::interfaceID() { return IID_IMuonPRDSelectionTool; }

}  // namespace Muon

#endif  // IMuonPRDSelectionTool_H
