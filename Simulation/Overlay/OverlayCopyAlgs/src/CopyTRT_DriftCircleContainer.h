/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYTRTDRIFTCIRCLECONTAINER_H
#define OVERLAYCOPYALGS_COPYTRTDRIFTCIRCLECONTAINER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

class TRT_ID;

class CopyTRT_DriftCircleContainer : public AthReentrantAlgorithm
{
public:

  CopyTRT_DriftCircleContainer(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_inputKey{ this, "InputKey", "", "ReadHandleKey for Pileup TRT_DriftCircleContainer" };
  SG::WriteHandleKey<InDet::TRT_DriftCircleContainer> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output TRT_DriftCircleContainer" };

  const TRT_ID* m_idHelper = nullptr;

};

#endif // OVERLAYCOPYALGS_COPYTRTDRIFTCIRCLECONTAINER_H
