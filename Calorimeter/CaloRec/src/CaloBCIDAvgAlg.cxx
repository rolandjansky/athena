/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloBCIDAvgAlg.h" 
#include "AthenaMonitoringKernel/Monitored.h"
#include "CxxUtils/aligned_vector.h"
#include "TFile.h"

//#define DONTDO

StatusCode CaloBCIDAvgAlg::initialize() {
  ATH_MSG_INFO( " initialize "  );

  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_bcidAvgKey.initialize());
  ATH_CHECK(m_mcSym.initialize(!m_isSC));
  ATH_CHECK(m_bcidLumiKey.initialize());
  if (! m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode  CaloBCIDAvgAlg::finalize() {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------------

StatusCode CaloBCIDAvgAlg::execute(const EventContext& ctx) const
{
  auto timer = Monitored::Timer("TIME_exec");
  auto monitoring = Monitored::Group( m_monTool, timer);

  SG::ReadHandle<xAOD::EventInfo> ei(m_eventInfoKey,ctx);
  const int bcid = ei->bcid();
  float avgInteractionsPerCrossingMC = 1;
  if (ei->eventType (xAOD::EventInfo::IS_SIMULATION)) {
    avgInteractionsPerCrossingMC = ei->averageInteractionsPerCrossing();
  }

  const LArMCSym* mcSym=nullptr;
  if ( ! m_isSC) {
    SG::ReadCondHandle<LArMCSym> mcSymHdl(m_mcSym,ctx);
    mcSym=*mcSymHdl;
  }

  SG::ReadCondHandle<CaloBCIDLumi> bcidLumi (m_bcidLumiKey, ctx);
  CxxUtils::vec_aligned_vector<float> avgEshift;
  bcidLumi->calc (bcid, avgInteractionsPerCrossingMC, avgEshift);
  auto result = std::make_unique<CaloBCIDAverage>(mcSym, std::move(avgEshift));

#ifdef DONTDO // some debug code, please, ignore
  std::cout << "BCIDAlg corrections for BCID : " << bcid << std::endl;
  for (size_t i = 0; i < mcSym->symIds().size(); i++) {
    const HWIdentifier hwid = mcSym->symIds()[i];
    float eshift=avgEshift[i];
    if ( fabsf(1e9*(eshift)) > 0.001 ){
      unsigned id32=hwid.get_identifier32().get_compact();
      std::cout << "Alg BCID " << bcid << ", cell [" << id32 <<"] = " <<(double)eshift << std::endl;
    }
  }
#endif

  SG::WriteHandle<CaloBCIDAverage> writeHdl(m_bcidAvgKey,ctx);
  ATH_CHECK(writeHdl.record(std::move(result)));

  return StatusCode::SUCCESS;
}
