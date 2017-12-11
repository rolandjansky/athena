/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigBtagEmulationToolTest_H
#define TrigBtagEmulationToolTest_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigBtagEmulationTool/ITrigBtagEmulationTool.h"

#include <string>
#include <map>

class IExecuteTool;

namespace Trig{
    class TrigBtagEmulationToolTest : public AthAlgorithm { 

        public: 

            /// Constructor with parameters:
            TrigBtagEmulationToolTest(const std::string& name, ISvcLocator* pSvcLocator);

            /// Destructor: 
            ~TrigBtagEmulationToolTest(); 

            /// Athena algorithm's Hooks
            StatusCode  initialize();
            StatusCode  execute();
            StatusCode  finalize();

        private:

	    /// Constructor with parameters: 
	    TrigBtagEmulationToolTest();

        private:
            ToolHandle<Trig::TrigDecisionTool> m_trigdec;
            ToolHandle<ITrigBtagEmulationTool> m_emulationTool;

            std::vector<std::string>   m_triggerList;
            StoreGateSvc              *m_storeGate;
    }; 
}
#endif
