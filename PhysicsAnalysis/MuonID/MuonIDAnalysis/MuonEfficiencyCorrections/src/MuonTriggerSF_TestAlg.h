/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */
#include "MuonEfficiencyCorrections/MuonSFTestHelper.h"
#include "PATInterfaces/SystematicsUtil.h"

#include <MuonEfficiencyCorrections/MuonSFTestHelper.h>
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
            std::vector<TriggerSFBranch_Ptr> m_branches;

            // Tools and services:
            ServiceHandle<ITHistSvc> m_histSvc;
            ToolHandle<CP::IMuonTriggerScaleFactors> m_trigEff;
            ToolHandle<CP::IPileupReweightingTool> m_prwTool;
            TTree* m_tree;
            unsigned int m_runNumber;            
  
    };
}
