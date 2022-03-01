/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MuonCalibrationPeriodTool_H
#define MCAST_MuonCalibrationPeriodTool_H

// Framework include(s):
#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/AsgMetadataTool.h>

// Local include(s):
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
// EDM include(s):

#include "xAODEventInfo/EventInfo.h"

namespace CP {

    class MuonCalibrationPeriodTool : public virtual IMuonCalibrationAndSmearingTool,
                                      public virtual ISystematicsTool,
                                      public asg::AsgMetadataTool {
        // Create a proper constructor for Athena
        ASG_TOOL_CLASS3(MuonCalibrationPeriodTool, CP::IMuonCalibrationAndSmearingTool, CP::ISystematicsTool, CP::IReentrantSystematicsTool)

    public:
        // There are a few recommended options by MCP to setup the tool
        // (https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC16)
        //  1) Correct data for the sagitta bias with CB calibration    - for most precision analyses
        //  2) Correct data for the sagitta bias with ID+MS calibration - for analyses limited by resolution systematics or unblinded high
        //  pT analyses 3) Do not correct data for the sagitta bias with ID+MS calibration  - for blinded high_pt analyses
        enum CalibMode {
            noOption = -1,  // <--- default
            correctData_CB = 0,
            correctData_IDMS = 1,
            notCorrectData_IDMS = 2
        };
        /// Apply the correction on a modifyable object
        virtual CorrectionCode applyCorrection(xAOD::Muon& mu) const override;
        /// Create a corrected copy from a constant muon
        virtual CorrectionCode correctedCopy(const xAOD::Muon& input, xAOD::Muon*& output) const override;
        /// Get the expected pT resolution
        virtual double expectedResolution(const std::string& DetType, const xAOD::Muon& mu, const bool mc) const override;
        /// Get the expected pT resolution - int argument is more efficient
        virtual double expectedResolution(const int& DetType, const xAOD::Muon& mu, const bool mc) const override;
        /// Expert method to apply the MC correction on a modifyable trackParticle for ID- or MS-only corrections
        virtual CorrectionCode applyCorrectionTrkOnly(xAOD::TrackParticle& inTrk, const int DetType) const override;

        // Interface - Is the tool affected by a specific systematic?
        virtual bool isAffectedBySystematic(const SystematicVariation& systematic) const override;
        // Interface - Which systematics have an effect on the tool's behaviour?
        virtual SystematicSet affectingSystematics() const override;
        // Interface - Systematics to be used for physics analysis
        virtual SystematicSet recommendedSystematics() const override;
        // Interface - Use specific systematic
        virtual SystematicCode applySystematicVariation(const SystematicSet& systConfig) override;

        // Constructor
        MuonCalibrationPeriodTool(const std::string& name);

        // Destructor
        virtual ~MuonCalibrationPeriodTool();

        // Initializing the subtools
        virtual StatusCode initialize() override;
        // Retrieve the runNumber of the event and assign the correct tool
        virtual StatusCode beginEvent() override;

    private:
        unsigned int getPeriod() const;
        const CP::IMuonCalibrationAndSmearingTool* getTool() const;

        // Have three tool instances for each Monte Carlo campaign
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_1516;
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_17;
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_18;

        mutable const CP::IMuonCalibrationAndSmearingTool* m_activeTool;
        const xAOD::EventInfo* m_evInfo;

        std::string m_sagittaRelease;
        std::string m_release;
        bool m_do2StationsHighPt;
        bool m_doExtraSmearing;

        int m_calib_mode;

        // Monte Carlo runNumbers correspond to different production campaigns
        std::vector<unsigned int> m_MCperiods1516;
        std::vector<unsigned int> m_MCperiods17;
        std::vector<unsigned int> m_MCperiods18;
        // Optionally one can use the random run number to assign the right tool in MC
        bool m_useRndRun;
    };

}  // namespace CP

#endif
