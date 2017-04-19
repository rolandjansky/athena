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

  class PrunDriver : public Driver {

  public:

    PrunDriver ();

    void testInvariant () const;

    virtual void doSubmit(const Job& job, const std::string& location) const;

    virtual bool doRetrieve(const std::string& location) const;

    static void status(const std::string& location);

    static void setState(const std::string& location,
			 const std::string& task,
			 const std::string& state);

    ClassDef(EL::PrunDriver, 1);
  };
}

#endif
