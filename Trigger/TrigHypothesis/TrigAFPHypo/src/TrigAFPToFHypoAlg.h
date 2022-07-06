/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGAFPTOFHYPOALG_H
#define TRIGAFPTOFHYPOALG_H

#include "DecisionHandling/HypoBase.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODForward/AFPVertexContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODForward/AFPVertex.h"

#include "TrigAFPToFHypoTool.h"

/**
 * @class TrigAFPToFHypoAlg
 * @brief
 **/

class TrigAFPToFHypoAlg : public::HypoBase
{
 public:
  TrigAFPToFHypoAlg(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &context) const override;

 private:
  SG::ReadHandleKey< xAOD::VertexContainer > m_inputPrmVtxKey {this, "VertexContainer", "HLT_IDVertex_FS", "Primary vertex collection"};
  SG::ReadHandleKey<xAOD::AFPVertexContainer> m_inputAfpVtxKey{this, "AFPVertexContainer", "HLT_AFPVertex", "AFP ToF vertex collection"};
  ToolHandleArray<TrigAFPToFHypoTool> m_hypoTools{this, "HypoTools", {}};
};

#endif // TRIGAFPTOFHYPOALG_H
