/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_TRIGZVERTEXHYPOALG_H
#define TRIGMINBIAS_TRIGZVERTEXHYPOALG_H

#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrigZVertexHypoTool.h"
#include <string>

/**
 * @class TrigZVertexHypoAlg
 * @brief Uses Verter weights found by TrigZFinderAlg/Tool to select events with high multiplicity
 **/
class TrigZVertexHypoAlg : public ::HypoBase {
public:
  TrigZVertexHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigZVertexHypoAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:

  ToolHandleArray<TrigZVertexHypoTool> m_hypoTools{ this, "HypoTools", {}, "Tools that perform actual selection" };
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_zVertexCollKey{ this, "ZVertexKey", "Undefined", "Input" };

};

#endif // TRIGMINBIAS_TRIGZVERTEXHYPOALG_H
