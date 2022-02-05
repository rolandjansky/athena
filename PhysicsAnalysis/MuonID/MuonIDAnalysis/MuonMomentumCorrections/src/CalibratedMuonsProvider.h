/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBRATEDMUONSPROVIDER
#define CALIBRATEDMUONSPROVIDER

#include <AsgAnalysisInterfaces/IPileupReweightingTool.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

namespace CP {

    /// decorates a muon collection with efficiency and scale factor

    class CalibratedMuonsProvider : public AthAlgorithm {
    public:
        /// Regular Algorithm constructor
        CalibratedMuonsProvider(const std::string& name, ISvcLocator* svcLoc);

        /// Function initialising the algorithm
        virtual StatusCode initialize() override;
        /// Function executing the algorithm
        virtual StatusCode execute() override;

    private:
        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};
        /// muon container
        SG::ReadHandleKey<xAOD::MuonContainer> m_inputKey{this, "Input", "Muons", " Name of the muon container to calibrate"};
        SG::WriteHandleKey<xAOD::MuonContainer> m_outputKey{this, "Output", "CalibratedMuons",
                                                            "Name of the final container written to storegate"};

        /// Muon calibration tool
        ToolHandle<IMuonCalibrationAndSmearingTool> m_tool{this, "Tool", ""};
        ToolHandle<IPileupReweightingTool> m_prwTool{this, "prwTool", ""};
        Gaudi::Property<bool> m_useRndNumber{this, "useRndRunNumber", false};

    };  // class

}  // namespace CP

#endif  //
