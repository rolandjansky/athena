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

            /// Default constructor: 
            TrigBtagEmulationToolTest();
            StatusCode Method1();
            StatusCode Method2();
            void writeEmulationSummary();
            float ratio(float,float);

        private:
            ToolHandle<Trig::TrigDecisionTool> m_trigdec;
            ToolHandle<ITrigBtagEmulationTool> m_emulationTool;

            std::vector<std::string>   m_triggerList;
            StoreGateSvc              *m_storeGate;

            //counters
            std::map<std::string, int> m_counter;
            std::map<std::string, int> m_counter_emu;
            std::map<std::string, int> m_counter2;
            std::map<std::string, int> m_counter_emu2;

    }; 
}
#endif
