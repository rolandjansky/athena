/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MUONCALIBINTSCALESMEARTOOL_H
#define MCAST_MUONCALIBINTSCALESMEARTOOL_H

// C++ includes
#include <map>
#include <vector>

// Framework include(s):
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "PATInterfaces/SystematicsCache.h"
#include "xAODEventInfo/EventInfo.h"

#include "MuonMomentumCorrections/MuonObj.h"
#include "MuonMomentumCorrections/IMuonCalibIntTool.h"
#include "MuonMomentumCorrections/EnumDef.h"
#include "MuonMomentumCorrections/CalibContainer.h"

namespace CP {

    class MuonCalibIntScaleSmearTool : public virtual IMuonCalibIntScaleSmearTool,
                                           public asg::AsgTool {
        // Create a proper constructor for Athena
        ASG_TOOL_CLASS3(MuonCalibIntScaleSmearTool, CP::IMuonCalibIntScaleSmearTool, CP::ISystematicsTool,
                        CP::IReentrantSystematicsTool)

    public:
        // Interface methods that must be defined
        // Interface - Apply the correction on a modifyable object
        virtual CorrectionCode applyCorrection(MCP::MuonObj& mu) const override;

        virtual double getExpectedResolution(const int &DetType, double pT, double eta, double phi, MCP::DataYear year, bool isData) const override;
     
        // Interface - Is the tool affected by a specific systematic?
        virtual bool isAffectedBySystematic(const SystematicVariation& systematic) const override;
        // Interface - Which systematics have an effect on the tool's behaviour?
        virtual SystematicSet affectingSystematics() const override;
        // Interface - Systematics to be used for physics analysis
        virtual SystematicSet recommendedSystematics() const override;
        // Interface - Use specific systematic
        virtual StatusCode applySystematicVariation(const SystematicSet& systConfig) override;
       
    public:
        // Constructor
        MuonCalibIntScaleSmearTool(const std::string& name);

        // Destructor
        virtual ~MuonCalibIntScaleSmearTool() = default;

        virtual StatusCode initialize() override;

   protected:
        // Event info
        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};
        
        // Release scheme 
        Gaudi::Property<std::string> m_release{this, "release", "Recs2022_03_20", "Release"};

        // Systematics scheme 
        Gaudi::Property<std::string> m_sysScheme{this, "systematicScheme", "Corr_Scale", "Systematic scheme to be configured"};

        // Do direct CB calibration 
        Gaudi::Property<bool> m_doDirectCBCalib{this, "doDirectCBCalib", true, "Do direct calibration of CB tracks, otherwise, do ID+MS"};

        typedef std::map<MCP::ScaleResCorrection, std::shared_ptr<MCP::CalibContainer>>  ScaleSmearCorrConstMap;

        // For systematics
        struct ParameterSetScaleSmear 
        {
            std::map<MCP::ScaleSmearParam, double> m_IDparams;
            std::map<MCP::ScaleSmearParam, double> m_MEparams;
            std::map<MCP::ScaleSmearParam, double> m_CBparams;
        };
        
        // calculate the parameter set for the given systematic
        StatusCode calcSystematicVariation(const SystematicSet& systConfig, ParameterSetScaleSmear& param) const;

        SystematicsCache<ParameterSetScaleSmear> m_Parameters{this};
        const ParameterSetScaleSmear* m_currentParameters{nullptr};

        // Corrections from the data
        std::map<MCP::DataYear, std::map<MCP::ScaleSmearParam, ScaleSmearCorrConstMap>> m_IDparams;
        std::map<MCP::DataYear, std::map<MCP::ScaleSmearParam, ScaleSmearCorrConstMap>> m_MEparams;
        std::map<MCP::DataYear, std::map<MCP::ScaleSmearParam, ScaleSmearCorrConstMap>> m_CBparams;


        // For expected resu
        std::map<MCP::DataYear, std::map<MCP::ExpectedResParam, std::shared_ptr<MCP::CalibContainer>>> m_IDExpectedResparams;
        std::map<MCP::DataYear, std::map<MCP::ExpectedResParam, std::shared_ptr<MCP::CalibContainer>>> m_MEExpectedResparams;

        static constexpr std::array<MCP::ScaleSmearParam,5> m_paramList{MCP::ScaleSmearParam::s0, MCP::ScaleSmearParam::s1, MCP::ScaleSmearParam::r0, MCP::ScaleSmearParam::r1, MCP::ScaleSmearParam::r2};

        std::map<MCP::ScaleSmearParam, double> getConstants(const MCP::TrackCalibObj& trk, const std::map<MCP::ScaleSmearParam, ScaleSmearCorrConstMap>& constants, const std::map<MCP::ScaleSmearParam, double>& direction) const;

        // Apply all the pT corrections
        double getCorrectedPt(const MCP::MuonObj& mu, const MCP::TrackCalibObj& trk, const std::map<MCP::ScaleSmearParam, double>& calibConstant) const;

        // Get the smearing correction
        double getSmearCorr(const MCP::MuonObj& mu, const MCP::TrackCalibObj& trk, double r0, double r1, double r2) const;

        // Apply the ID+MS correction on the CB track
        double getCorrectedCBPtWithIDMSComb(const MCP::MuonObj& mu, const std::map<MCP::ScaleSmearParam, double>&calibIDConstant , const std::map<MCP::ScaleSmearParam, double>& calibMEConstant) const;

    };  // class MuonCalibIntScaleSmearTool

}  // namespace CP

#endif
