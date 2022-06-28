/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MuonCalibIntHighpTSmearTool_H
#define MCAST_MuonCalibIntHighpTSmearTool_H

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

    class MuonCalibIntHighpTSmearTool : public virtual IMuonCalibIntTool,
                                           public asg::AsgTool {
        // Create a proper constructor for Athena
        ASG_TOOL_CLASS3(MuonCalibIntHighpTSmearTool, CP::IMuonCalibIntTool, CP::ISystematicsTool,
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
        MuonCalibIntHighpTSmearTool(const std::string& name);
        
        // Destructor
        virtual ~MuonCalibIntHighpTSmearTool() = default;

        virtual StatusCode initialize() override;

      
   protected:
        // Event info
        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};
        
        // Release scheme 
        Gaudi::Property<std::string> m_release{this, "release", "Recs2022_03_20", "Release version"};
        
        typedef std::map<MCP::MST_Categories, std::shared_ptr<MCP::CalibContainer>>  HighpTConstMap;

        // Corrections from the data
        std::map<MCP::DataYear, HighpTConstMap> m_MS_Misaligned_p1;
        std::map<MCP::DataYear, HighpTConstMap> m_MS_Misaligned_p2;
        std::map<MCP::DataYear, HighpTConstMap> m_MS_AlignedAndCorrected_p1;
        std::map<MCP::DataYear, HighpTConstMap> m_MS_AlignedAndCorrected_p2;

        MCP::MST_Categories ConvertToMacroCategory(const int raw_mst_category) const;



    };  // class MuonCalibIntHighpTSmearTool

}  // namespace CP

#endif
