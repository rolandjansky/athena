/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISCT_SurfaceChargesGenerator.h
 * Header file for abstract base class ISCT_SurfaceChargesGenerator
 * (c) ATLAS Detector software
 * Interface for the Sct_SurfaceCharge generator classes
 * Version  23/08/2007 Kondo Gnanvo
 */

#ifndef SCT_DIGITIZATION_ISCT_SURFACECHARGESGENERATOR_H
#define SCT_DIGITIZATION_ISCT_SURFACECHARGESGENERATOR_H

// Input/output classes
#include <list>
#include "SiDigitization/SiSurfaceCharge.h"
#include "HitManagement/TimedHitPtr.h"

#include "GaudiKernel/IAlgTool.h"

class SiHit;

namespace InDetDD {
  class SiDetectorElement;
}
namespace CLHEP {
  class HepRandomEngine;
}

class ISiSurfaceChargesInserter
{
 public:
  virtual ~ISiSurfaceChargesInserter() {}
  virtual void operator() (const SiSurfaceCharge& scharge) const = 0;
};

static const InterfaceID IID_ISCT_SurfaceChargesGenerator("ISCT_SurfaceChargesGenerator",1,0);

class ISCT_SurfaceChargesGenerator : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  //Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISCT_SurfaceChargesGenerator; }
  
  // Destructor:
  virtual ~ISCT_SurfaceChargesGenerator() {}

  virtual void setDetectorElement(const InDetDD::SiDetectorElement* ele) = 0 ;
  virtual void process(const TimedHitPtr<SiHit>& phit,
                       const ISiSurfaceChargesInserter& inserter) const =0;
  virtual void processFromTool(const SiHit* phit, const ISiSurfaceChargesInserter& inserter, const float p_eventTime, const unsigned short p_eventId) const =0;
  //TESTING NEW METHODS
  virtual void setComTime(float comTime) =0;                                 
  virtual void setFixedTime(float fixedTime) =0;                             
  virtual void setCosmicsRun(bool cosmicsRun) =0;                            
  virtual void setComTimeFlag(bool useComTime) =0;                                 
  virtual void setRandomEngine(CLHEP::HepRandomEngine* rndmEngine) =0;       
};

#endif // SIDIGITIZATION_ISCT_SURFACECHARGESGENERATOR_H
