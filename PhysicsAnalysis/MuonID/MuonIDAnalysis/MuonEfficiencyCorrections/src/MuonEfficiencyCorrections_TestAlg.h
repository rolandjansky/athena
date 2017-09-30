// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG
#define CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"

#include "MuonEfficiencyCorrections/MuonSFTestHelper.h"

#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
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

        private:
            /// muon container
            std::string m_sgKey;
            ServiceHandle<ITHistSvc> m_histSvc;
       
            ToolHandleArray<IMuonEfficiencyScaleFactors> m_effi_SF_tools;
            /// Scale factor tool
            ToolHandle<IPileupReweightingTool> m_prw_Tool;
            
            std::unique_ptr<TestMuonSF::MuonSFTestHelper> m_test_helper;
            
    
        
    };

}// namespace CP

#endif //CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG
