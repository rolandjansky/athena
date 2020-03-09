/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRUNCATIONANALYSISALG_H
#define TRIGOUTPUTHANDLING_TRUNCATIONANALYSISALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigger/TrigCompositeContainer.h"

/**
 * @class TruncationAnalysisAlg
 * @brief Retrieves and prints the content of HLT result truncation debugging information object
 **/
class TruncationAnalysisAlg : public AthReentrantAlgorithm {
public:
  TruncationAnalysisAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_truncationInfoKey {
    this, "TruncationInfoKey", "TruncationDebugInfo", "Key of the truncation debugging info object"};
};

#endif // TRIGOUTPUTHANDLING_TRUNCATIONANALYSISALG_H
