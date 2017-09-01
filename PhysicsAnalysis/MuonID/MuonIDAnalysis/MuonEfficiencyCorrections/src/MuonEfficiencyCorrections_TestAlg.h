// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG
#define CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
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

            /// Scale factor tool
            ToolHandle<IMuonEfficiencyScaleFactors> m_sf_Tool;
            ToolHandle<IMuonEfficiencyScaleFactors> m_isosf_Tool;
            ToolHandle<IMuonEfficiencyScaleFactors> m_ttvasf_Tool;
            ToolHandle<IMuonTriggerScaleFactors> m_trigsf_Tool;
            ToolHandle<IPileupReweightingTool> m_prw_Tool;

    };
// class MuonEfficiencyCorrections_TestAlg

}// namespace CP

#endif //CPTOOLTESTS_MuonEfficiencyCorrections_TESTALG
