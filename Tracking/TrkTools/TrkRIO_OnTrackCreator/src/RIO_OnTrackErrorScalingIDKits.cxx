/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "RIO_OnTrackErrorScalingKit.h"
#include "InDetRIO_OnTrack/PixelRIO_OnTrackErrorScaling.h"
#include "InDetRIO_OnTrack/SCTRIO_OnTrackErrorScaling.h"
#include "InDetRIO_OnTrack/TRTRIO_OnTrackErrorScaling.h"

class PixelRIO_OnTrackErrorScalingKit
  : public RIO_OnTrackErrorScalingSpecialisedKit<PixelRIO_OnTrackErrorScaling>
{
public:
  virtual unsigned int nParametres() const override {
    return PixelRIO_OnTrackErrorScaling::kNParamTypes;
  }
  virtual const char* const* paramNames()  const override {
    return PixelRIO_OnTrackErrorScaling::paramNames();
  }

};

// the run1 kit only removes the 2 last two parameters which are for IBL
class PixelRIO_OnTrackErrorScalingRun1Kit
  : public PixelRIO_OnTrackErrorScalingKit
{
public:
  virtual unsigned int nParametres() const override {
    return PixelRIO_OnTrackErrorScaling::kPixIBLPhi;
  }

};

class SCTRIO_OnTrackErrorScalingKit
  : public RIO_OnTrackErrorScalingSpecialisedKit<SCTRIO_OnTrackErrorScaling>
{
public:
  virtual unsigned int nParametres() const override {
    return SCTRIO_OnTrackErrorScaling::kNParamTypes;
  }
  virtual const char* const* paramNames()  const override {
    return SCTRIO_OnTrackErrorScaling::paramNames();
  }

};

class TRTRIO_OnTrackErrorScalingKit
  : public RIO_OnTrackErrorScalingSpecialisedKit<TRTRIO_OnTrackErrorScaling>
{
public:
  virtual unsigned int nParametres() const override {
    return TRTRIO_OnTrackErrorScaling::kNParamTypes;
  }
  virtual const char* const* paramNames()  const override {
    return TRTRIO_OnTrackErrorScaling::paramNames();
  }

};

namespace {
  // register all kits with the help of a dumm function which sets a global anonymous bool
  bool registered = ( []() -> bool {
    return
         RIO_OnTrackErrorScalingKitManager::instance().registerKit("PixelRIO_OnTrackErrorScaling",    new PixelRIO_OnTrackErrorScalingKit)
      && RIO_OnTrackErrorScalingKitManager::instance().registerKit("PixelRIO_OnTrackErrorScalingRun1",new PixelRIO_OnTrackErrorScalingRun1Kit)
      && RIO_OnTrackErrorScalingKitManager::instance().registerKit("SCTRIO_OnTrackErrorScaling",      new SCTRIO_OnTrackErrorScalingKit)
      && RIO_OnTrackErrorScalingKitManager::instance().registerKit("TRTRIO_OnTrackErrorScaling",      new TRTRIO_OnTrackErrorScalingKit) ;
    } ) ();
}
