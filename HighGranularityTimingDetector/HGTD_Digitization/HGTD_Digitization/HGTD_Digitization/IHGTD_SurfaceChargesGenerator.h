/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_Digitization/IHGTD_SurfaceChargesGenerator.h
 *
 * @author Tao Wang <tao.wang@cern.ch>
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 *
 * @date September, 2021
 *
 * @brief Interface for the Surface charge generation tool.
 *
 * TODO: - Methods used in InDet like
 *
 *   virtual void setComTime(float comTime) = 0;
 *   virtual void setFixedTime(float fixedTime) = 0;
 *   virtual void setCosmicsRun(bool cosmicsRun) = 0;
 *   virtual void setComTimeFlag(bool useComTime) = 0;
 *
 *   might be needed in the future!
 *
 */

#ifndef HGTD_DIGITZATION_IHGTD_SURFACECHARGESGENERATOR_H
#define HGTD_DIGITZATION_IHGTD_SURFACECHARGESGENERATOR_H

#include "GaudiKernel/IAlgTool.h"

#include "HitManagement/TimedHitPtr.h"
#include "InDetSimEvent/SiHit.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/SiSurfaceCharge.h"

namespace InDetDD {
class SolidStateDetectorElementBase;
}

namespace CLHEP {
class HepRandomEngine;
}

class IHGTD_SurfaceChargesGenerator : virtual public IAlgTool {

public:
  // Destructor:
  virtual ~IHGTD_SurfaceChargesGenerator() {}

  virtual void
  createSurfaceChargesFromHit(const TimedHitPtr<SiHit>& hit,
                              SiChargedDiodeCollection* diode_coll,
                              const InDetDD::SolidStateDetectorElementBase* ele,
                              CLHEP::HepRandomEngine* rndmEngine,
                              const EventContext& ctx) const = 0;

  virtual void setIntegratedLuminosity(float integrated_luminosity) = 0;

  virtual void setSmearingTime(bool smear_meantime) = 0;

  // Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IHGTD_SurfaceChargesGenerator, 1, 0);

};

#endif // HGTD_DIGITZATION_IHGTD_SURFACECHARGESGENERATOR_H
