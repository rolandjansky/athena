/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigBtagValidationTest_H
#define TrigBtagValidationTest_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigBtagEmulationTool/ITrigBtagEmulationTool.h"

#include <string>
#include <map>

class IExecuteTool;

namespace Trig{
    class TrigBtagValidationTest : public AthAlgorithm { 

        public: 

            /// Constructor with parameters: 
            TrigBtagValidationTest(const std::string& name, ISvcLocator* pSvcLocator);

            /// Destructor: 
	    ~TrigBtagValidationTest();

            /// Athena algorithm's Hooks
            StatusCode  initialize();
            StatusCode  execute();
            StatusCode  finalize();

        private: 

            /// Default constructor: 
	    TrigBtagValidationTest();
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

        private:
	    std::vector< std::string > m_toBeEmulatedTriggers;
	    std::map< std::string, long int> m_counterEmulation;
	    std::map< std::string, long int> m_counterTDT;
	    std::map< std::string, long int> m_counterMismatches_tot;
	    std::map< std::string, long int> m_counterMismatches_TDT1EMUL0;
	    std::map< std::string, long int> m_counterMismatches_TDT0EMUL1;
    }; 
}
#endif
