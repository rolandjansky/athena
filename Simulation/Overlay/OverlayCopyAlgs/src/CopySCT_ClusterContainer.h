/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYSCTCLUSTERCONTAINER_H
#define OVERLAYCOPYALGS_COPYSCTCLUSTERCONTAINER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

class SCT_ID;

class CopySCT_ClusterContainer : public AthReentrantAlgorithm
{
public:

  CopySCT_ClusterContainer(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_inputKey{ this, "InputKey", "", "ReadHandleKey for Pileup SCT_ClusterContainer" };
  SG::WriteHandleKey<InDet::SCT_ClusterContainer> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output SCT_ClusterContainer" };

  const SCT_ID* m_idHelper = nullptr;

};

#endif // OVERLAYCOPYALGS_COPYSCTCLUSTERCONTAINER_H
