/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYCALOCALIBRATIONHITCONTAINER_H
#define OVERLAYCOPYALGS_COPYCALOCALIBRATIONHITCONTAINER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

class CopyCaloCalibrationHitContainer : public AthReentrantAlgorithm
{
public:

  CopyCaloCalibrationHitContainer(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<CaloCalibrationHitContainer> m_signalInputKey{ this, "SignalInputKey", "", "ReadHandleKey for Signal CaloCalibrationHitContainer" };
  SG::WriteHandleKey<CaloCalibrationHitContainer> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output CaloCalibrationHitContainer" };

  Gaudi::Property<std::string> m_collectionName{ this, "collectionName", "", "CaloCalibrationHitContainer collection name" };
};

#endif // OVERLAYCOPYALGS_COPYCALOCALIBRATIONHITCONTAINER_H
