/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOMMONALGS_COPYCALOCALIBRATIONHITCONTAINER_H
#define OVERLAYCOMMONALGS_COPYCALOCALIBRATIONHITCONTAINER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

class CopyCaloCalibrationHitContainer : public AthAlgorithm
{
public:

  CopyCaloCalibrationHitContainer(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  SG::ReadHandleKey<CaloCalibrationHitContainer> m_signalInputKey{ this, "SignalInputKey", "", "ReadHandleKey for Signal CaloCalibrationHitContainer" };
  SG::WriteHandleKey<CaloCalibrationHitContainer> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output CaloCalibrationHitContainer" };

  Gaudi::Property<std::string> m_collectionName{ this, "collectionName", "", "CaloCalibrationHitContainer collection name" };
};

#endif // OVERLAYCOMMONALGS_COPYCALOCALIBRATIONHITCONTAINER_H
