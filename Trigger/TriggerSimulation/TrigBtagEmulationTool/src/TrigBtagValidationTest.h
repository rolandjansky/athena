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
#include <tuple>

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

        private:
            ToolHandle<Trig::TrigDecisionTool> m_trigdec;
            ToolHandle<ITrigBtagEmulationTool> m_emulationTool;

            std::vector<std::string>   m_triggerList;
            StoreGateSvc              *m_storeGate;

        private:
	    std::vector< std::string > m_toBeEmulatedTriggers;

	    enum { EMUL, TDT, mismatchesTOT , mismatchesTDT1EMUL0, mismatchesTDT0EMUL1 };
	    std::map< std::string,std::tuple<long int,long int,long int,long int,long int> > m_counters;
    }; 
}
#endif
