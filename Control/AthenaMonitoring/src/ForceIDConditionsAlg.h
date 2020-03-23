/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef FORCEIDCONDITIONSALG_H
#define FORCEIDCONDITIONSALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

class ForceIDConditionsAlg : public AthReentrantAlgorithm {
 public:
  ForceIDConditionsAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator) {};
  virtual ~ForceIDConditionsAlg() {};

  virtual StatusCode initialize() override;
  
  virtual StatusCode execute(const EventContext&) const override { return StatusCode::SUCCESS; };

 protected:
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer"};

};

#endif
