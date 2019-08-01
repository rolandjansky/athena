/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__KUBERNETES_DRIVER_H
#define EVENT_LOOP__KUBERNETES_DRIVER_H

#include <EventLoop/Global.h>

#include <EventLoop/BatchDriver.h>
#include <SampleHandler/Global.h>

namespace EL
{
  class KubernetesDriver : public BatchDriver
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard default constructor
    /// guarantee: strong
    /// failures: low level errors I
  public:
    KubernetesDriver ();



    //
    // interface inherited from BatchDriver
    //

  private:
    virtual std::string batchReleaseSetup (bool sharedFileSystem) const override;

    /// effects: perform the actual local submission with njob jobs
    /// guarantee: strong
    /// failures: submission errors
    /// rationale: the virtual part of batch submission
  private:
    virtual void
    batchSubmit (const std::string& location, const SH::MetaObject& options,
 		 const std::vector<std::size_t>& jobIndices, bool resubmit)
      const override;



    //
    // private interface
    //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(KubernetesDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
