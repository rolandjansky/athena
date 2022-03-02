/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Topo_L1TopoSimulation
#define L1Topo_L1TopoSimulation

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoSimulation/IInputTOBConverter.h"

#include "PeriodicScaler.h"
#include "TrigConfBase/MsgStream.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigConfData/L1Menu.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"

#include "xAODTrigger/L1TopoSimResultsContainer.h"
#include "xAODTrigger/L1TopoSimResultsAuxContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TH1F.h"
#include <memory>

namespace LVL1 {

   class L1TopoSimulation : public AthAlgorithm {
   public:
      L1TopoSimulation(const std::string &name, ISvcLocator *pSvcLocator);

      virtual StatusCode initialize() override;
      virtual StatusCode start() override;
      virtual StatusCode execute() override;
      virtual StatusCode finalize() override;
     
      // make algorithm is clonable
      virtual bool isClonable() const override;
     
      /**
         @brief Retrieve the L1Topo hardware bits from the DAQ RODs
         
         No need to cache them within this AthAlgorithm; just pass
         them to TopoSteering, which will then do all the work with
         them.
      */
      StatusCode retrieveHardwareDecision();

   private:
      void WriteEDM(std::unique_ptr<xAOD::L1TopoSimResultsContainer> &container, const std::string &name, unsigned int clock, uint32_t word);
      void WriteEDM(std::unique_ptr<xAOD::L1TopoSimResultsContainer> &container, const std::string &name, unsigned int clock, uint64_t word);

      std::unique_ptr<TCS::TopoSteering>  m_topoSteering; //!< the topo steering
      std::unique_ptr<LVL1::PeriodicScaler> m_scaler {nullptr}; //! prescale decision tool

      std::unique_ptr< xAOD::L1TopoSimResultsContainer > m_l1topoContainer;
      std::unique_ptr< xAOD::L1TopoSimResultsAuxContainer > m_l1topoAuxContainer;

     // Services and input tools
      ServiceHandle<ITHistSvc> m_histSvc { this, "HistSvc", "THistSvc/THistSvc", "Histogramming service for L1Topo algorithms" };

      ToolHandle<IInputTOBConverter> m_emtauInputProvider  { this, "EMTAUInputProvider",  "LVL1::EMTauInputProvider/EMTauInputProvider",   "Tool to fill the EMTAU TOBs of the topo input event"         };
      ToolHandle<IInputTOBConverter> m_jetInputProvider    { this, "JetInputProvider",    "LVL1::JetInputProvider/JetInputProvider",       "Tool to fill the Jet TOBs of the topo input event"           };
      ToolHandle<IInputTOBConverter> m_energyInputProvider { this, "EnergyInputProvider", "LVL1::EnergyInputProvider/EnergyInputProvider", "Tool to fill the energy and MET TOBs of the topo input event"};
      ToolHandle<IInputTOBConverter> m_muonInputProvider   { this, "MuonInputProvider",   "LVL1::MuonInputProvider/MuonInputProvider",     "Tool to fill the muon TOBs of the topo input event"          };

      // outputs
      SG::WriteHandleKey<LVL1::FrontPanelCTP>  m_topoCTPLocation { this, "TopoCTPLocation", LVL1::DEFAULT_L1TopoCTPLocation, "StoreGate key of topo decision output for CTP"}; ///< SG key of decision bits for CTP
      SG::WriteHandleKey<LVL1::FrontPanelCTP>  m_topoOverflowCTPLocation { this, "TopoOverflowCTPLocation", LVL1::DEFAULT_L1TopoOverflowCTPLocation, "StoreGate key of topo overflow output for CTP"}; ///< SG key of overflow bits for CTP
     
      SG::WriteHandleKey<LVL1::FrontPanelCTP>  m_legacyTopoCTPLocation { this, "LegacyTopoCTPLocation", LVL1::DEFAULT_L1TopoLegacyCTPLocation, "StoreGate key of topo decision output for CTP"}; ///< SG key of decision bits for CTP
      SG::WriteHandleKey<LVL1::FrontPanelCTP>  m_legacyTopoOverflowCTPLocation { this, "LegacyTopoOverflowCTPLocation", LVL1::DEFAULT_L1TopoLegacyOverflowCTPLocation, "StoreGate key of topo overflow output for CTP"}; ///< SG key of overflow bits for CTP

      // Writing L1Topo EDMs
      SG::WriteHandleKey< xAOD::L1TopoSimResultsContainer > m_legacyL1topoKey {this,"Key_LegacyL1TopoSimContainer","L1_LegacyTopoSimResults","Output legacy l1topo container"};
      SG::WriteHandleKey< xAOD::L1TopoSimResultsContainer > m_l1topoKey {this,"Key_L1TopoSimContainer","L1_TopoSimResults","Output l1topo container"};

      Gaudi::Property<bool> m_isLegacyTopo { this, "IsLegacyTopo", false, "Simulation of Legacy L1Topo boards" };
      Gaudi::Property<bool> m_enableInputDump { this, "EnableInputDump", false, "Enable writing of input data for standalone running" };
      Gaudi::Property<bool> m_enableBitwise { this, "UseBitwise", false, "Boolean to enable the bitwise version of software algorithms"}; 
      Gaudi::Property<std::string> m_inputDumpFile { this, "InputDumpFile", "inputdump.txt", "File name for dumping input data" };
      Gaudi::Property<int> m_topoOutputLevel { this, "TopoOutputLevel", TrigConf::MSGTC::WARNING, "OutputLevel for L1Topo algorithms"};
      Gaudi::Property<int> m_topoSteeringOutputLevel { this, "TopoSteeringOutputLevel", TrigConf::MSGTC::WARNING, "OutputLevel for L1Topo steering"};

      // Properties for hardware monitoring
      Gaudi::Property<bool> m_fillHistogramsBasedOnHardwareDecision { this, "FillHistoBasedOnHardware", false, "Fill accept/reject histograms based on hdw; default based on sim" };
      Gaudi::Property<unsigned int> m_prescaleForDAQROBAccess { this, "PrescaleDAQROBAccess", 4, "Prescale factor for requests for DAQ ROBs: can be used to avoid overloading ROS. Zero means disabled, 1 means always, N means sample only 1 in N events"}; 
      Gaudi::Property<unsigned int> m_prescale { this, "Prescale", 1, "Internal prescale factor for this algorithm, implemented with a periodic scaler: so 1 means run every time, N means run every 1 in N times it is called; the other times it will exit without doing anything"};
      Gaudi::Property<std::string> m_histBaseDir { this, "MonHistBaseDir", "L1/L1TopoAlgorithms", "Base directory for monitoring histograms will be /EXPERT/<MonHistBaseDir>"}; 
   };

}
#endif
