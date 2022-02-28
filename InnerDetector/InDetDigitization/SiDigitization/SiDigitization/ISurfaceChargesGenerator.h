/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISurfaceChargesGenerator.h
 * Header file for abstract base class ISurfaceChargesGenerator
 */

#ifndef SIDIGITIZATION_ISURFACECHARGESGENERATOR_H
#define SIDIGITIZATION_ISURFACECHARGESGENERATOR_H

// Input/output classes
#include <list>
#include "SiDigitization/SiSurfaceCharge.h"
#include "HitManagement/TimedHitPtr.h"

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

class SiHit;

namespace InDetDD {
  class SiDetectorElement;
  class SCT_ModuleSideDesign;
}
namespace CLHEP {
  class HepRandomEngine;
}

class ISiSurfaceChargesInserter
{
 public:
  virtual ~ISiSurfaceChargesInserter() {}
  virtual void operator() (const SiSurfaceCharge& scharge) = 0;
};

static const InterfaceID IID_ISurfaceChargesGenerator("ISurfaceChargesGenerator",1,0);

class ISurfaceChargesGenerator : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  //Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISurfaceChargesGenerator; }

  // Destructor:
  virtual ~ISurfaceChargesGenerator() {}

  virtual void process(const InDetDD::SiDetectorElement* ele,
                       const TimedHitPtr<SiHit>& phit,
                       ISiSurfaceChargesInserter& inserter,
                       CLHEP::HepRandomEngine * rndmEngine,
                       const EventContext& ctx) =0;
  /*
  //We may need an alternative method like this, if calls to 
  //SCT_ModuleSideDesign::getMother() prove costly
  //In which case, perhaps this Interface class should be specific to 
  //ITkStrip/SCT rather than common to both, since only ITkStrip
  //Needs to support split sensors in all its SurfaceChargeGenerators
  virtual void processSplit(const InDetDD::SCT_ModuleSideDesign* mother,
			    const InDetDD::SiDetectorElement* ele,
			    const TimedHitPtr<SiHit>& phit,
			    const ISiSurfaceChargesInserter& inserter,
			    CLHEP::HepRandomEngine * rndmEngine,
			    const EventContext& ctx) const =0;
  */
  virtual void setFixedTime(float fixedTime) =0;
};

#endif // SIDIGITIZATION_ISURFACECHARGESGENERATOR_H
