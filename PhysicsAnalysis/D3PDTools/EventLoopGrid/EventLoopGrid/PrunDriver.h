/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*- mode: c++ -*-
#ifndef EVENT_LOOP_PRUN_DRIVER_H
#define EVENT_LOOP_PRUN_DRIVER_H

#include "EventLoop/Driver.h"

namespace SH {
  class SampleGrid;
}

namespace EL {

  class PrunDriver final : public Driver {

  public:

    PrunDriver ();

    void testInvariant () const;

    virtual bool doRetrieve(const std::string& location) const override;

    static void status(const std::string& location);

    static void setState(const std::string& location,
			 const std::string& task,
			 const std::string& state);

  protected:
    virtual ::StatusCode
    doSubmitStep (Detail::JobSubmitInfo& info,
                  Detail::JobSubmitStep step) const override;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(EL::PrunDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
