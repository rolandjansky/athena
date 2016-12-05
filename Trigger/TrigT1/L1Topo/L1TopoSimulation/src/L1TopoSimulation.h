/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Topo_L1TopoSimulation
#define L1Topo_L1TopoSimulation

#include "TrigConfBase/MsgStream.h"


#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <memory>

class TH1;
class IMonitorToolBase;
class ITHistSvc;

namespace LVL1 {
   class PeriodicScaler;
}

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

      virtual StatusCode initialize() override;

      virtual StatusCode beginRun() override;
      virtual StatusCode endRun() override;

      virtual StatusCode start() override;
      virtual StatusCode stop() override;

      virtual StatusCode execute() override;

      virtual StatusCode finalize() override;

   private:

      //! \brief Alg handles to tools and services
      //! @{
      ServiceHandle<TrigConf::IL1TopoConfigSvc> m_l1topoConfigSvc;

      ServiceHandle<ITHistSvc> m_histSvc;

      ToolHandleArray < IMonitorToolBase > m_monitors;

      ToolHandle<IInputTOBConverter> m_emtauInputProvider;

      ToolHandle<IInputTOBConverter> m_jetInputProvider;

      ToolHandle<IInputTOBConverter> m_energyInputProvider;

      ToolHandle<IInputTOBConverter> m_muonInputProvider;

      //! @}

      BooleanProperty m_enableInputDump { false }; // for enabling input dumping
      BooleanProperty m_enableBitwise { false }; // for enabling bitwise algorithms

      StringProperty  m_inputDumpFile { "inputdump.txt" }; // input dump file
      StringProperty  m_topoCTPLocation { "" }; // input dump file
      int m_topoOutputLevel{TrigConf::MSGTC::WARNING};                                  // property to set the outputlevel of the topo algorithms
      int m_topoSteeringOutputLevel{TrigConf::MSGTC::WARNING};                          // property to set the outputlevel of the topo steering

      std::unique_ptr<TCS::TopoSteering>  m_topoSteering; //!< the topo steering 

      TH1 *  m_DecisionHist[3] { nullptr, nullptr, nullptr };

      UnsignedIntegerProperty m_prescale; //! property for prescale factor
      LVL1::PeriodicScaler* m_scaler; //! prescale decision tool

      StringProperty m_histBaseDir; //! sets base dir for monitoring histograms

   };

}
#endif
