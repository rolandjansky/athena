/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISurfaceChargesGenerator.h
 * Header file for abstract base class ISurfaceChargesGenerator
 * (c) ATLAS Detector software
 * Interface for the SurfaceCharge generator classes
 * Version  23/08/2007 Kondo Gnanvo
 */

#ifndef SIDIGITIZATION_ISURFACECHARGESGENERATOR_H
#define SIDIGITIZATION_ISURFACECHARGESGENERATOR_H

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

static const InterfaceID IID_ISurfaceChargesGenerator("ISurfaceChargesGenerator",1,0);

class ISurfaceChargesGenerator : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  //Retrieve interface ID
  static const InterfaceID & interfaceID() {return IID_ISurfaceChargesGenerator; }
  
  // Destructor:
  virtual ~ISurfaceChargesGenerator() {}

  virtual void setDetectorElement(const InDetDD::SiDetectorElement *ele) = 0 ;
  virtual void process(const TimedHitPtr<SiHit> & phit,
                       const ISiSurfaceChargesInserter& inserter) const =0;
  virtual void processFromTool(const SiHit* phit, const ISiSurfaceChargesInserter& inserter, const float p_eventTime, const unsigned short p_eventId) const =0;
  //TESTING NEW METHODS
  virtual void setComTime(float comTime) =0;                                 
  virtual void setFixedTime(float fixedTime) =0;                             
  virtual void setCosmicsRun(bool cosmicsRun) =0;                            
  virtual void setComTimeFlag(bool useComTime) =0;                                 
  virtual void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) =0;       
};

#endif // SIDIGITIZATION_ISURFACECHARGESGENERATOR_H
