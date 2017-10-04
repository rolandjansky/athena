/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */


// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "AsgTools/ToolHandleArray.h"

#include "PATInterfaces/SystematicsUtil.h"

#include <MuonEfficiencyCorrections/MuonSFTestHelper.h>
#include <MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h>
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

namespace Trig{
    class MuonTriggerSF_TestAlg: virtual public AthAlgorithm {
        public:

            MuonTriggerSF_TestAlg(const std::string& name, ISvcLocator* pSvcLocator);
            virtual ~MuonTriggerSF_TestAlg() {
            }

            StatusCode initialize();
            StatusCode execute();
            StatusCode finalize();
        private:
            // Properties:
            std::string m_outputStream;
            std::string m_inputContainerName;
            std::vector<std::string> m_triggers;
            std::vector<TestMuonSF::TriggerSFBranch_Ptr> m_branches;

            // Tools and services:
            ServiceHandle<ITHistSvc> m_histSvc;
            ToolHandle<CP::IMuonTriggerScaleFactors> m_trigEff;
            ToolHandle<CP::IPileupReweightingTool> m_prwTool;
            TTree* m_tree;
            unsigned int m_runNumber;            
  
    };
}
