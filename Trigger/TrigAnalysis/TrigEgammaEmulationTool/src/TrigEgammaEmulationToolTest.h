/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaMatchingToolTest.h 

#ifndef TrigEgammaEmulationToolTest_H
#define TrigEgammaEmulationToolTest_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

#include <string>
#include <map>

class IExecuteTool;

namespace Trig{
    class TrigEgammaEmulationToolTest : public AthAlgorithm { 

        public: 

            /// Constructor with parameters: 
            TrigEgammaEmulationToolTest(const std::string& name, ISvcLocator* pSvcLocator);

            /// Destructor: 
            virtual ~TrigEgammaEmulationToolTest(); 

            /// Athena algorithm's Hooks
            virtual StatusCode  initialize() override;
            virtual StatusCode  execute() override;
            virtual StatusCode  finalize() override;

        private: 

            /// Default constructor: 
            TrigEgammaEmulationToolTest();
            StatusCode Method1();
            StatusCode Method2();
            void writeEmulationSummary();
            asg::AcceptData setAccept(const HLT::TriggerElement*);
            float ratio(float,float);


            void count( std::string key ){
              if(m_countMap.find(key) == m_countMap.end()){
                m_countMap[key]= 1;
              }else{
                m_countMap[key]++;
              }
            }




        private:
            ToolHandle<Trig::TrigDecisionTool> m_trigdec;
            ToolHandle<ITrigEgammaMatchingTool> m_matchTool;
            ToolHandle<ITrigEgammaEmulationTool> m_emulationTool;

            std::map<std::string, unsigned >m_countMap;
            std::vector<std::string>   m_triggerList;
            StoreGateSvc              *m_storeGate;
            asg::AcceptInfo            m_accept;

            const xAOD::ElectronContainer   *m_offElectrons;

            //counters
            std::map<std::string, int> m_counter;
            std::map<std::string, int> m_counter_emu;
            std::map<std::string, int> m_counter2;
            std::map<std::string, int> m_counter_emu2;

    }; 
}
#endif
