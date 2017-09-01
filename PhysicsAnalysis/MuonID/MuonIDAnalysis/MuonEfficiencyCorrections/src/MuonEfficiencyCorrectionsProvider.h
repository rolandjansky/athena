// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFCORECTIONSPROVIDER
#define MUONEFFCORECTIONSPROVIDER
//author: will buttinger

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"

namespace CP {

/// decorates a muon collection with efficiency and scale factor

    class MuonEfficiencyCorrectionsProvider: public AthAlgorithm {

        public:
            /// Regular Algorithm constructor
            MuonEfficiencyCorrectionsProvider(const std::string& name, ISvcLocator* svcLoc);

            /// Function initialising the algorithm
            virtual StatusCode initialize();
            /// Function executing the algorithm
            virtual StatusCode execute();

        private:
            /// muon container
            std::string m_sgKey;

            /// Scale factor tool
            ToolHandle<IMuonEfficiencyScaleFactors> m_sf_Tool;

    };
// class

}// namespace CP

#endif //
