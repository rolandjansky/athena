/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_CREATEFULLSCANROI_H
#define HLTSEEDING_CREATEFULLSCANROI_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include <string>

/**
 * @class CreateFullScanRoI
 * @brief in every event creates collection with single FullScanRoI (for testing MET, Jet and similar FS reco)
 **/
class CreateFullScanRoI : public AthReentrantAlgorithm {
public:
  CreateFullScanRoI(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CreateFullScanRoI() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_roisKey{ this, "RoIsKey", "FullScanRoIs", "name of the output"};
};

#endif // HLTSEEDING_CREATEFULLSCANROI_H
