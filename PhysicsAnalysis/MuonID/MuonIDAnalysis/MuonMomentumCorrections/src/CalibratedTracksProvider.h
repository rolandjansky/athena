/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBRATEDTRACKSPROVIDER
#define CALIBRATEDTRACKSPROVIDER

// Gaudi/Athena include(s):
#include <AsgAnalysisInterfaces/IPileupReweightingTool.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace CP {

    /// decorates a track collection with efficiency and scale factor

    class CalibratedTracksProvider : public AthAlgorithm {
    public:
        /// Regular Algorithm constructor
        CalibratedTracksProvider(const std::string& name, ISvcLocator* svcLoc);

        /// Function initialising the algorithm
        virtual StatusCode initialize() override;
        /// Function executing the algorithm
        virtual StatusCode execute() override;

    private:
        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};
        /// track container

        SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputKey{
            this, "Input", "InDetTrackParticles",
            " Name of the track particle container to calibrated (Optionally ExtrapolatedMuonTrackParticle)"};
        SG::WriteHandleKey<xAOD::TrackParticleContainer> m_outputKey{this, "Output", "CalibratedInDetTrackParticles",
                                                                     "Name of the final container written to storegate"};

        /// Calibration tool handle
        ToolHandle<IMuonCalibrationAndSmearingTool> m_tool{this, "Tool", "",
                                                           "Configured instance of the MuonMomentum and calibration tool"};
        ToolHandle<IPileupReweightingTool> m_prwTool{this, "prwTool", "", "Configured instance of the prwReweightingTool, if needed "};

        /// detector type of track (MS or ID)
        Gaudi::Property<int> m_detType{this, "DetType", 1,
                                       "Which kind of track particle container is parsed. Either the Id tracks (2) or the MS tracks(1)"};
        Gaudi::Property<bool> m_useRndNumber{this, "useRndRunNumber"};

    };  // class

}  // namespace CP

#endif  //
