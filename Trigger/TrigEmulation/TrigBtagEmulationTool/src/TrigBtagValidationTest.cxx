/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

#include "src/TrigBtagValidationTest.h"

namespace Trig {

TrigBtagValidationTest::TrigBtagValidationTest(const std::string& name, 
					       ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator) 
{}

StatusCode TrigBtagValidationTest::initialize() {
  ATH_MSG_INFO("Initializing " << name() );

  ATH_CHECK( m_histSvc.retrieve() );
  ATH_CHECK( m_trigDec.retrieve() );

  m_h_tdtpass = new TH1F( "h_tdtpass", "h_tdtpass", m_emulatedChains.size(), 0, m_emulatedChains.size());
  m_h_tbetpass = new TH1F( "h_tbetpass", "h_tbetpass", m_emulatedChains.size(), 0, m_emulatedChains.size());
  m_h_miss = new TH1F( "h_miss", "h_miss", m_emulatedChains.size(), 0, m_emulatedChains.size());
  m_h_falsepositive = new TH1F( "h_falsepositive", "h_falsepositive", m_emulatedChains.size(), 0, m_emulatedChains.size());
  ATH_CHECK( m_histSvc->regHist("/VALIDATION/h_tdtpass", m_h_tdtpass) );
  ATH_CHECK( m_histSvc->regHist("/VALIDATION/h_tbetpass", m_h_tbetpass) );
  ATH_CHECK( m_histSvc->regHist("/VALIDATION/h_miss", m_h_miss) );
  ATH_CHECK( m_histSvc->regHist("/VALIDATION/h_falsepositive", m_h_falsepositive) );
  
  for(unsigned int i = 1; i <= m_emulatedChains.size(); i++) {
    m_h_tdtpass->GetXaxis()->SetBinLabel(i, m_emulatedChains[i-1].c_str());
    m_h_miss->GetXaxis()->SetBinLabel(i, m_emulatedChains[i-1].c_str());
    m_h_falsepositive->GetXaxis()->SetBinLabel(i, m_emulatedChains[i-1].c_str());
  }
  
  return StatusCode::SUCCESS;
}
  
StatusCode TrigBtagValidationTest::execute() 
{
  ATH_MSG_DEBUG("Executing " << name() );
  const auto& emulCtx =  m_emulationTool->populateJetManagersTriggerObjects();
  int chain_idx = 0;
  for(const auto& chain: m_emulatedChains) {
    bool tbet_pass = m_emulationTool->isPassed(chain, emulCtx);
    bool tdt_pass = m_trigDec->isPassed(chain);
    ATH_MSG_DEBUG( chain << " TDT:" << (tdt_pass ? "PASS":"NO") << " TBET:" << (tbet_pass ? "PASS":"NO"));
    
    m_h_tdtpass->Fill( tdt_pass ? chain_idx +.5 : -1 );
    m_h_tbetpass->Fill( tbet_pass ? chain_idx +.5 : -1 );
    m_h_miss->Fill( (tdt_pass && !tbet_pass) ? chain_idx +.5 : -1 );
    m_h_falsepositive->Fill( (!tdt_pass && tbet_pass) ? chain_idx +.5 : -1 );
    
    chain_idx++;
  }
  
  return StatusCode::SUCCESS;
}

} // namespace
