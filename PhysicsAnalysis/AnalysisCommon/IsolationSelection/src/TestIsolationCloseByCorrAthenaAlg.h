// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONSELECTION_TESTISOLATIONCLOSEBYCORRATHENAALG_H
#define ISOLATIONSELECTION_TESTISOLATIONCLOSEBYCORRATHENAALG_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"

namespace CP {

    class TestIsolationCloseByCorrAthenaAlg : public AthAlgorithm {
        
        public:
            TestIsolationCloseByCorrAthenaAlg(const std::string& name, ISvcLocator* svcLoc);
      
            virtual StatusCode initialize();
            virtual StatusCode execute();   
      
        private:
            ToolHandle<CP::IIsolationCloseByCorrectionTool> m_isoCloseByCorrTool;            
            ToolHandle<CP::IIsolationSelectionTool> m_isoSelectorTool;
            
            
    };
    
}
#endif
