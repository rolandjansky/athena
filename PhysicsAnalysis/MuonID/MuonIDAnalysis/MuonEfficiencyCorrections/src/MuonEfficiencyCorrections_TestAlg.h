/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG
#define CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "AsgTools/ToolHandleArray.h"

#include "MuonEfficiencyCorrections/MuonSFTestHelper.h"

#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

namespace CP {

/// small test algorithm to quickly test/demonstrate the usage of the MuonEfficiencyCorrections code within athena

    class MuonEfficiencyCorrections_TestAlg: public AthAlgorithm {

        public:
            /// Regular Algorithm constructor
            MuonEfficiencyCorrections_TestAlg(const std::string& name, ISvcLocator* svcLoc);

            /// Function initialising the algorithm
            virtual StatusCode initialize();
            /// Function executing the algorithm
            virtual StatusCode execute();
            virtual ~MuonEfficiencyCorrections_TestAlg() {
            }

        private:
            /// muon container
            std::string m_sgKey;
            ServiceHandle<ITHistSvc> m_histSvc;

            ToolHandleArray<IMuonEfficiencyScaleFactors> m_effi_SF_tools;
            ToolHandleArray<IMuonEfficiencyScaleFactors> m_comparison_tools;

            /// Scale factor tool
            ToolHandle<IPileupReweightingTool> m_prw_Tool;
            ToolHandle<IMuonSelectionTool> m_sel_tool;

            std::unique_ptr<TestMuonSF::MuonSFTestHelper> m_test_helper;
            std::unique_ptr<TestMuonSF::MuonSFTestHelper> m_comparison_helper;

            std::string m_first_release_name;
            std::string m_second_release_name;
    };

} // namespace CP

#endif //CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG
