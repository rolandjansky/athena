/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Alexander Madsen
/// @author Nils Krumnack



#ifndef EVENT_LOOP_PRUN_DRIVER_H
#define EVENT_LOOP_PRUN_DRIVER_H

#include "EventLoop/Driver.h"

namespace SH {
  class SampleGrid;
}

namespace EL {

  /// \brief a \ref Driver to submit jobs via prun

  class PrunDriver final : public Driver {

  public:

    PrunDriver ();

    void testInvariant () const;

    static void status(const std::string& location);

    static void setState(const std::string& location,
			 const std::string& task,
			 const std::string& state);

  protected:
    virtual ::StatusCode
    doManagerStep (Detail::ManagerData& data) const override;

  private:
    ::StatusCode doRetrieve (Detail::ManagerData& data) const;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(EL::PrunDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
