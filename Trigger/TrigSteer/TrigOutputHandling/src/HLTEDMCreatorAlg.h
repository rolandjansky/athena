/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTEDMCREATORALG_H
#define TRIGOUTPUTHANDLING_HLTEDMCREATORALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DecisionHandling/IHLTOutputTool.h"

#include <string>

/**
 * @class HLTEDMCreatorAlg
 * @brief 
 **/
class HLTEDMCreatorAlg : public AthReentrantAlgorithm {
public:
  HLTEDMCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HLTEDMCreatorAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  ToolHandleArray<IHLTOutputTool> m_tools{ this, "OutputTools", {}, "Tools that generate output"};
};

#endif // TRIGOUTPUTHANDLING_HLTEDMCREATORALG_H
