// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: TestIsolationAthenaAlg.h 764207 2016-07-25 18:16:03Z alesage $
#ifndef ISOLATIONSELECTION_TESTISOLATIONATHENAALG_H
#define ISOLATIONSELECTION_TESTISOLATIONATHENAALG_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "IsolationSelection/IIsolationSelectionTool.h"

namespace CP {
    class TestIsolationAthenaAlg: public AthAlgorithm {
        public:
            TestIsolationAthenaAlg(const std::string& name, ISvcLocator* svcLoc);

            /// Function initialising the algorithm
            virtual StatusCode initialize();
            /// Function executing the algorithm
            virtual StatusCode execute();

        private:
            std::string m_sgKeyPhotons;
            std::string m_sgKeyElectrons;
            std::string m_sgKeyMuons;
            ToolHandle<CP::IIsolationSelectionTool> m_isoTool;
    };
}
#endif
