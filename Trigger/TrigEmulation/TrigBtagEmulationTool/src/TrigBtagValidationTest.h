/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigBtagValidationTest_H
#define TrigBtagValidationTest_H

#include "TrigBtagEmulationTool/ITrigBtagEmulationTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TH1.h"


namespace Trig {

class TrigBtagValidationTest : 
  public AthAlgorithm {
public:
  /// Constructor with parameters:
  TrigBtagValidationTest(const std::string& name, ISvcLocator* pSvcLocator);
  
  /// Destructor:
  virtual ~TrigBtagValidationTest() = default;
  
  /// Athena algorithm's Hooks
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual unsigned int cardinality() const override { return 1; }
  
private:
  ServiceHandle<ITHistSvc> m_histSvc {this, "THistSvc", "THistSvc", "Histogramming svc" };
  ToolHandle<Trig::ITrigBtagEmulationTool> m_emulationTool {this, "TrigBtagEmulationTool", "", ""};
  PublicToolHandle< Trig::TrigDecisionTool > m_trigDec {this, "TrigDecisionTool", "",""};
  
  Gaudi::Property< std::vector< std::string >> m_emulatedChains {this, "EmulatedChains", {}, ""};
  
  TH1F *m_h_tdtpass = nullptr;
  TH1F *m_h_tbetpass = nullptr;
  TH1F *m_h_miss = nullptr;
  TH1F *m_h_falsepositive = nullptr;
};

} // namespace

#endif
