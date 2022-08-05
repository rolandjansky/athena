/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MUONCALIBTOOL_H
#define MCAST_MUONCALIBTOOL_H

// Framework include(s):
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "PATInterfaces/SystematicsCache.h"
#include "xAODEventInfo/EventInfo.h"

#include "MuonMomentumCorrections/MuonObj.h"
#include "MuonMomentumCorrections/IMuonCalibIntTool.h"


namespace CP {

    class MuonCalibTool : public virtual IMuonCalibrationAndSmearingTool,
                                           public asg::AsgTool {
        // Create a proper constructor for Athena
        ASG_TOOL_CLASS3(MuonCalibTool, CP::IMuonCalibrationAndSmearingTool, CP::ISystematicsTool,
                        CP::IReentrantSystematicsTool)
                        
    public:
        enum CalibMode {
            noOption = -1,  // <--- default
            correctData_CB = 0,
            correctData_IDMS = 1,
            notCorrectData_IDMS = 2,
            userDefined = 99,
        };

        // Interface methods that must be defined
        // Interface - Apply the correction on a modifyable object
        virtual CorrectionCode applyCorrection(xAOD::Muon& mu) const override;
        // Interface - Create a corrected copy from a constant muon
        virtual CorrectionCode correctedCopy(const xAOD::Muon& input, xAOD::Muon*& output) const override;
        // Interface - Is the tool affected by a specific systematic?
        virtual bool isAffectedBySystematic(const SystematicVariation& systematic) const override;
        // Interface - Which systematics have an effect on the tool's behaviour?
        virtual SystematicSet affectingSystematics() const override;
        // Interface - Systematics to be used for physics analysis
        virtual SystematicSet recommendedSystematics() const override;
        // Interface - Use specific systematic
        virtual StatusCode applySystematicVariation(const SystematicSet& systConfig) override;
        // Interface - get the expected resolution of the muon
        virtual double expectedResolution(const std::string& DetType, const xAOD::Muon& mu, const bool addMCCorrectionSmearing) const override;
        // Interface - get the expected resolution of the muon
        virtual double expectedResolution(const int& DetType, const xAOD::Muon& mu, const bool addMCCorrectionSmearing) const override;
        // Interface - Expert method to apply the MC correction on a modifyable trackParticle for ID- or MS-only corrections
        virtual CorrectionCode applyCorrectionTrkOnly(xAOD::TrackParticle& inTrk, const int DetType) const override;

    public:
        // Constructor
        MuonCalibTool(const std::string& name);

        // Destructor
        virtual ~MuonCalibTool() = default;

        virtual StatusCode initialize() override;

      
   protected:
        // Event info
        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};

        Gaudi::Property<bool> m_isRun3{this, "IsRun3Geo", false, "Needed for MuonSelectionTool"}; 

        // Properties of the tool
        Gaudi::Property<std::string> m_release{this, "release", "Recs2022_06_15_Rel22PreRecs",     "Release"};
        // Systematics scheme 
        Gaudi::Property<std::string> m_sysScheme{this, "systematicScheme", "Corr_Scale",     "Systematic scheme to be configured"};

        Gaudi::Property<bool> m_validationMode{this, "expert_validationMode", false, "Expert only option. Puts the tool in the validation mode setup"};
        Gaudi::Property<bool> m_expertMode_isData{this, "expertMode_isData", false, "Expert only option. Overwrites the isData Flag"};
        Gaudi::Property<bool> m_useRndRun{this, "useRandomRunNumber", false, "To use the random run number for deciding which calibration to apply"};

        Gaudi::Property<int> m_calibMode{this, "calibMode", -1, "Calib mode"};


        // Scale and Smear options
        // Do direct CB calibration 
        Gaudi::Property<bool> m_doDirectCBCalib{this, "doDirectCBCalib", true,     "Do direct calibration of CB tracks, otherwise, do ID+MS"};


        // SagittaBias properties
        Gaudi::Property<bool> m_doEtaSagittaSys{this, "doEtaSagittaSys",   false, "Do Eta dependant systematic system"};
        Gaudi::Property<bool> m_applyCorrectionOnData{this, "applyCorrectionOnData",   true, "If to apply sagitta corrections on data, or take the full effect as systematic"};


        // High pT correction options
        Gaudi::Property<bool>  m_extra_highpt_smearing{this, "doExtraSmearing", false, "TODO"};
        Gaudi::Property<bool>  m_2stations_highpt_smearing{this, "do2StationsHighPt", false, "TODO"};
        Gaudi::Property<float> m_HighPtSystThreshold{this, "HighPtSystThr", 300.0, "Thershold for high pT smearing in GeV"};      

        asg::AnaToolHandle<CP::IMuonSelectionTool> m_MuonSelectionTool;

        asg::AnaToolHandle<CP::IMuonCalibIntTool> m_MuonIntSagittaTool;
        asg::AnaToolHandle<CP::IMuonCalibIntScaleSmearTool> m_MuonIntScaleSmearTool;

        asg::AnaToolHandle<CP::IMuonCalibIntTool> m_MuonIntHighTSmearTool;


        // internal tool function
        // Converts xAOD object to an internal MuonObj for easier transfer of information
        MCP::MuonObj convertToMuonObj(const xAOD::Muon& mu) const;
        MCP::MuonObj convertToMuonObj(const xAOD::TrackParticle& inTrk, const int DetType) const;
        /// Decorate all information that's needed to ensure reproducibility of the smearing
        void initializeRandNumbers(MCP::MuonObj& obj) const;


        MCP::DataYear getPeriod(bool isData) const; 


    };  // class MuonCalibTool

}  // namespace CP

#endif
