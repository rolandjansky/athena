/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaMatchingToolTest.h 

#ifndef TrigEgammaMatchingToolTest_H
#define TrigEgammaMatchingToolTest_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
class IExecuteTool;

namespace Trig{
    class TrigEgammaMatchingToolTest : public AthAlgorithm { 

        public: 

            /// Constructor with parameters: 
            TrigEgammaMatchingToolTest(const std::string& name, ISvcLocator* pSvcLocator);

            /// Destructor: 
            ~TrigEgammaMatchingToolTest(); 

            /// Athena algorithm's Hooks
            StatusCode  initialize();
            StatusCode  execute();
            StatusCode  finalize();

        private: 

            /// Default constructor: 
            TrigEgammaMatchingToolTest();

        private:
            ToolHandle<Trig::TrigDecisionTool> m_trigdec;
            ToolHandle<ITrigEgammaMatchingTool> m_matchTool;

            std::vector<std::string> m_triggerList;
            std::map<std::string,int> m_counterBits;
            StoreGateSvc * m_storeGate;
    }; 
}
#endif
