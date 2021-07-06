/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/SamplePtr.h>

#include <RootCoreUtils/Assert.h>
#include <SampleHandler/Sample.h>

//
// method implementations
//

namespace SH
{
  void SamplePtr ::
  testInvariant () const
  {
  }



  SamplePtr ::
  SamplePtr ()
    : m_sample (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  SamplePtr ::
  SamplePtr (Sample *val_sample)
    : m_sample (val_sample)
  {
    if (m_sample)
      m_sample->alloc ();

    RCU_NEW_INVARIANT (this);
  }



  SamplePtr ::
  SamplePtr (std::unique_ptr<Sample> val_sample)
    : SamplePtr (val_sample.release())
  {
    // no invariant used
  }



  SamplePtr ::
  SamplePtr (const SamplePtr& that)
    : m_sample (that.m_sample)
  {
    if (m_sample)
      m_sample->alloc ();

    RCU_NEW_INVARIANT (this);
  }



  SamplePtr ::
  ~SamplePtr ()
  {
    RCU_DESTROY_INVARIANT (this);

    if (m_sample)
      m_sample->release ();
  }



  SamplePtr& SamplePtr ::
  operator = (const SamplePtr& that)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_READ_INVARIANT (&that);

    if (that.m_sample)
      that.m_sample->alloc ();
    if (m_sample)
      m_sample->release ();
    m_sample = that.m_sample;
    return *this;
  }



  bool SamplePtr ::
  empty () const
  {
    RCU_READ_INVARIANT (this);
    return m_sample == 0;
  }



  Sample *SamplePtr ::
  get () const
  {
    RCU_READ_INVARIANT (this);
    return m_sample;
  }



  Sample *SamplePtr ::
  operator -> () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (!empty());
    return m_sample;
  }



  Sample& SamplePtr ::
  operator * () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (!empty());
    return *m_sample;
  }
}
