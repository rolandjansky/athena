#ifndef FORCEIDCONDITIONSALG_H
#define FORCEIDCONDITIONSALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

class ForceIDConditionsAlg : public AthReentrantAlgorithm {
 public:
  ForceIDConditionsAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator) {};
  virtual ~ForceIDConditionsAlg() {};

  virtual StatusCode initialize() override;
  
  virtual StatusCode execute(const EventContext& ctx) const override;

 protected:
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

};

#endif
