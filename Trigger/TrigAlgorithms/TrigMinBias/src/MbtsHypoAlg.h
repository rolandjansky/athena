/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_MBTSHYPOALG_H
#define TRIGMINBIAS_MBTSHYPOALG_H

#include "DecisionHandling/HypoBase.h"
#include "MbtsHypoTool.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"

// STL includes
#include <string>

/**
 * @class MbtsHypoAlg
 * @brief
 **/
class MbtsHypoAlg : public ::HypoBase
{
public:
  MbtsHypoAlg(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &context) const override;

private:
  ToolHandleArray<MbtsHypoTool> m_hypoTools{this, "HypoTools", {}};
  SG::ReadHandleKey<xAOD::TrigT2MbtsBitsContainer> m_MbtsBitsKey{this, "MbtsBitsKey", "Undefined"};
};

#endif // TRIGMINBIAS_MBTSHYPOALG_H
