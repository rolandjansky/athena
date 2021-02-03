/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2MINBIAS_MBTSHYPOALG_H
#define TRIGT2MINBIAS_MBTSHYPOALG_H

#include "DecisionHandling/HypoBase.h"
#include "MbtsHypoTool.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"

// STL includes
#include <string>

/**
 * @class MbtsHypoAlg
 * @brief 
 **/
class MbtsHypoAlg : public ::HypoBase {
public:
  MbtsHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MbtsHypoAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  ToolHandleArray<MbtsHypoTool> m_hypoTools{this, "HypoTools", {}};
  SG::ReadHandleKey<xAOD::TrigT2MbtsBitsContainer> m_MbtsBitsKey{this, "MbtsBitsKey", "Undefined", ""};
};

#endif // TRIGT2MINBIAS_MBTSHYPOALG_H
