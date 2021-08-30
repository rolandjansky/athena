/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGENERICALGS_ENDOFEVENTROICONFIRMERALG_H
#define TRIGGENERICALGS_ENDOFEVENTROICONFIRMERALG_H 1

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include <TrigSteeringEvent/TrigRoiDescriptorCollection.h>

/**
 * @class EndOfEventROIConfirmerAlg
 * @brief Ensure that all ReadHandles for triggers running on all HLT-accepted events at the end are available.
 * Empty collections are stored for unavailable keys. 
 */
class EndOfEventROIConfirmerAlg: public AthReentrantAlgorithm {
 public:
  EndOfEventROIConfirmerAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EndOfEventROIConfirmerAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;


 private:
  SG::WriteHandleKeyArray<TrigRoiDescriptorCollection> m_writeHandleKeyArray_ROIs {this, "RoIs", {}, "Keys of the RoI collections"};

};

#endif  // TRIGGENERICALGS_ENDOFEVENTROICONFIRMERALG_H
