/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MUONCALIBINTSAGITTATOOL_H
#define MCAST_MUONCALIBINTSAGITTATOOL_H

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

    class MuonCalibIntSagittaTool : public virtual IMuonCalibIntTool,
                                           public asg::AsgTool {
        // Create a proper constructor for Athena
        ASG_TOOL_CLASS3(MuonCalibIntSagittaTool, CP::IMuonCalibIntTool, CP::ISystematicsTool,
                        CP::IReentrantSystematicsTool)

    public:
        // Interface methods that must be defined
        // Interface - Apply the correction on a modifyable object
        virtual CorrectionCode applyCorrection(MCP::MuonObj& mu) const override;
     
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
        MuonCalibIntSagittaTool(const std::string& name);

        // Destructor
        virtual ~MuonCalibIntSagittaTool() = default;

        virtual StatusCode initialize() override;

      
   protected:
        // Event info
        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};
        
        // Release scheme 
        Gaudi::Property<std::string>  m_release{this, "release", "Recs2022_03_20",     "Release"};

        // Sagitta corrections
        Gaudi::Property<bool>  m_doEtaSagittaSys{this, "doEtaSagittaSys",   false, "Do Eta dependant systematic system"};
        Gaudi::Property<bool> m_applyCorrectionOnData{this, "applyCorrectionOnData",   true, "If to apply sagitta corrections on data, or take the full effect as systematic"};

        // Rho value to be used
        Gaudi::Property<bool> m_useFixedRho{this, "useFixedRho",   true, "Force Fixed Rho"};
        Gaudi::Property<float> m_fixedRhoValue{this, "fixedRhoValue", 1,     "Value for rho, if it is forced"};

        // Systematics scheme 
        Gaudi::Property<std::string>  m_sysScheme{this, "systematicScheme", "Corr_Scale",     "Systematic scheme to be configured"};
        Gaudi::Property<float>  m_extraRebiasSys{this, "extraRebias", 0.00002,     "Additional systematic applied for the residual bias observed"};

        // For systematics
        struct ParameterSetSagitta {
            double SagittaRho{0.};
            double SagittaBias{0.};
            double SagittaDataStat{0.};
            double SagittaEtaSlice{0.};
        };
        
        // calculate the parameter set for the given systematic
        StatusCode calcSystematicVariation(const SystematicSet& systConfig, ParameterSetSagitta& param) const;

        SystematicsCache<ParameterSetSagitta> m_Parameters{this};
        const ParameterSetSagitta* m_currentParameters{nullptr};

        typedef std::map<MCP::SagittaCorrection, std::shared_ptr<MCP::CalibContainer>>  SagittaCorrConstMap;

        // Corrections from the data
        std::map<MCP::DataYear, SagittaCorrConstMap> m_sagittaCorrConst_ID;
        std::map<MCP::DataYear, SagittaCorrConstMap> m_sagittaCorrConst_ME;
        std::map<MCP::DataYear, SagittaCorrConstMap> m_sagittaCorrConst_CB;

        // Corrections from the MC - what is typically referred to as the kinematic term
        std::map<MCP::DataYear, SagittaCorrConstMap> m_sagittaCorrConst_mcID;
        std::map<MCP::DataYear, SagittaCorrConstMap> m_sagittaCorrConst_mcME;
        std::map<MCP::DataYear, SagittaCorrConstMap> m_sagittaCorrConst_mcCB;


        void applySagittaCorrection(double& pt, const std::vector<double>& correction, const int& charge, const bool& isData) const;


        // Get the set of correction, based on data + systematic setting
        std::vector<double> getCorrections(const MCP::TrackCalibObj& mu) const;

        // Statistical combination
        double statCombCorrection(const MCP::MuonObj& mu, double corrIDpT, double corrMEpT, double CBpT) const;
        CorrectionCode applyStatCombination(AmgVector(5) parsID, const AmgSymMatrix(5)& covID, AmgVector(5) parsMS, const AmgSymMatrix(5)& covMS, int charge, AmgVector(5) & parsCB, AmgSymMatrix(5) & covCB, double& chi2) const;


    };  // class MuonCalibIntSagittaTool

}  // namespace CP

#endif
