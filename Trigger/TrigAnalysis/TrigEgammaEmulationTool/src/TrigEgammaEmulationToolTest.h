/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaMatchingToolTest.h 

#ifndef TrigEgammaEmulationToolTest_H
#define TrigEgammaEmulationToolTest_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigEgammaMatchingTool/TrigEgammaMatchingToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"


class TrigEgammaEmulationToolTest : public AthMonitorAlgorithm 
{  
  public:

    struct TrigInfo{
      std::string trigger;
      std::string type;
      float threshold;
      bool etcut;
      bool idperf;
      bool gsf;
      bool lrt;
    };


  public:
    /// Constructor with parameters: 
    TrigEgammaEmulationToolTest(const std::string& name, ISvcLocator* pSvcLocator);
    /// Destructor: 
    virtual ~TrigEgammaEmulationToolTest(); 

    /// Athena algorithm's Hooks
    virtual StatusCode  initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;


  private:

      asg::AcceptInfo m_accept;

      ToolHandle<Trig::TrigDecisionTool> m_trigdec;
      ToolHandle<TrigEgammaMatchingToolMT> m_matchTool;
      ToolHandle<Trig::TrigEgammaEmulationToolMT> m_emulatorTool;

      Gaudi::Property<std::vector<std::string>> m_electronTriggerList {this, "ElectronTriggerList", {}, };
      Gaudi::Property<std::vector<std::string>> m_photonTriggerList {this, "PhotonTriggerList", {}, };

      SG::ReadHandleKey<xAOD::ElectronContainer> m_offElectronKey{ this, "ElectronKey", "Electrons", ""};
      SG::ReadHandleKey<xAOD::PhotonContainer> m_offPhotonKey{ this, "PhotonKey", "Photons", ""};

      asg::AcceptData setAccept( const TrigCompositeUtils::Decision *dec, 
                                 const TrigEgammaEmulationToolTest::TrigInfo info) const;

      TrigEgammaEmulationToolTest::TrigInfo getInfo( std::string trigger ) const;


      void fillLabel( const ToolHandle<GenericMonitoringTool>& groupHandle, 
                      const std::string &histname, 
                      const std::string &label ) const;
}; 

#endif
