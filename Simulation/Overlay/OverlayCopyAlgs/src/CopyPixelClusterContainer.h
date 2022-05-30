/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYPIXELCLUSTERCONTAINER_H
#define OVERLAYCOPYALGS_COPYPIXELCLUSTERCONTAINER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

class PixelID;

class CopyPixelClusterContainer : public AthReentrantAlgorithm
{
public:

  CopyPixelClusterContainer(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<InDet::PixelClusterContainer> m_inputKey{ this, "InputKey", "", "ReadHandleKey for Pileup PixelClusterContainer" };
  SG::WriteHandleKey<InDet::PixelClusterContainer> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output PixelClusterContainer" };

  const PixelID* m_idHelper = nullptr;

};

#endif // OVERLAYCOPYALGS_COPYPIXELCLUSTERCONTAINER_H
