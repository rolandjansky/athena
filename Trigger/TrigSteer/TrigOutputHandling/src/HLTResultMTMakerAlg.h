/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTRESULTMTMAKERALG_H
#define TRIGOUTPUTHANDLING_HLTRESULTMTMAKERALG_H

#include "TrigOutputHandling/HLTResultMTMaker.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

/**
 * @class HLTResultMTMakerAlg 
 * @brief Simply calls the result maker tool to create HLTResultMT. The algorithm is meant to be used only offline.
 * Online, the result maker is called directly by the event loop manager.
 **/
class HLTResultMTMakerAlg : public AthReentrantAlgorithm {
public:
  HLTResultMTMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HLTResultMTMakerAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  ToolHandle<HLTResultMTMaker> m_resultMaker { this, "ResultMaker", "HLTResultMTMaker" };
  
};

#endif // TRIGOUTPUTHANDLING_HLTRESULTMTMAKERALG_H
