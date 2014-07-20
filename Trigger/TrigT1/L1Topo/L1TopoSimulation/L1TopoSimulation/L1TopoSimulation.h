/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Topo_L1TopoSimulation
#define L1Topo_L1TopoSimulation

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigInterfaces/IMonitoredAlgo.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <memory>

class TH1;
class IMonitorToolBase;

namespace TCS {
   class TopoSteering;
}

namespace TrigConf {
   class IL1TopoConfigSvc;
}

namespace LVL1 {

   class IInputTOBConverter;

   class L1TopoSimulation : public AthAlgorithm, public IMonitoredAlgo {
   public:
      L1TopoSimulation(const std::string &name, ISvcLocator *pSvcLocator);
      ~L1TopoSimulation();

      virtual StatusCode initialize();

      virtual StatusCode start();

      virtual StatusCode execute();

      virtual StatusCode finalize();

   private:

      //! \brief Alg handles to tools and services
      //! @{
      ServiceHandle<TrigConf::IL1TopoConfigSvc> m_l1topoConfigSvc;

      ToolHandleArray < IMonitorToolBase > m_monitors;

      ToolHandle<IInputTOBConverter> m_emtauInputProvider;

      ToolHandle<IInputTOBConverter> m_jetInputProvider;

      ToolHandle<IInputTOBConverter> m_energyInputProvider;
      //! @}

      BooleanProperty m_enableInputDump { false }; // for enabling input dumping
      StringProperty  m_inputDumpFile { "inputdump.txt" }; // input dump file

      std::unique_ptr<TCS::TopoSteering>  m_topoSteering; //!< the topo steering 

      TH1 *  m_DecisionHist[3] { nullptr, nullptr, nullptr };

   };

}
#endif
