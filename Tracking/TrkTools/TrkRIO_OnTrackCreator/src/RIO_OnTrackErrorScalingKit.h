/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _RIO_OnTrackErrorScalingKit_H_
#define _RIO_OnTrackErrorScalingKit_H_

#include "TrkRIO_OnTrack/RIO_OnTrackErrorScaling.h"
#include "KitManager.h"

class RIO_OnTrackErrorScalingKit
{
public:
  virtual ~RIO_OnTrackErrorScalingKit() {}

  virtual unsigned int nParametres()                        const = 0;
  virtual const char* const* paramNames()                   const = 0;
  virtual size_t getParamIndex(const std::string &name)     const;

  virtual std::unique_ptr<RIO_OnTrackErrorScaling> create() const = 0;
};

template <class T>
class RIO_OnTrackErrorScalingSpecialisedKit : public RIO_OnTrackErrorScalingKit
{
 public:
  virtual std::unique_ptr<RIO_OnTrackErrorScaling> create() const override {
    std::unique_ptr<RIO_OnTrackErrorScaling> error_scaling( new T);
    error_scaling->params().resize(this->nParametres());
    return error_scaling;
  }
};

typedef KitManager<RIO_OnTrackErrorScalingKit> RIO_OnTrackErrorScalingKitManager;

#endif
