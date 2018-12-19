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

#include <EventLoop/OutputStream.h>

#include <EventLoop/Algorithm.h>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskOutput.h>

//
// method implementations
//

ClassImp (EL::OutputStream)

namespace EL
{
  const std::string OutputStream::optType = "EventLoop_OutputStreamType";
  const std::string OutputStream::optMergeCmd = "EventLoop_MergeCmd";
  const std::string OutputStream::optContainerSuffix = "EventLoop_DSSuffix";

  void OutputStream ::
  testInvariant () const
  {
    RCU_INVARIANT (this);
    RCU_INVARIANT (!m_label.empty());
  }



  OutputStream ::
  OutputStream ()
    : m_label ("out"), m_output (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  OutputStream ::
  OutputStream (const std::string& val_label)
    : m_label (val_label), m_output (0)
  {
    RCU_REQUIRE_SOFT (!val_label.empty());

    RCU_NEW_INVARIANT (this);
  }



  OutputStream ::
  OutputStream (const std::string& val_label, const std::string& val_type)
    : m_label (val_label), m_output (0)
  {
    RCU_REQUIRE_SOFT (!val_label.empty());

    m_options.setString(optType, val_type);

    RCU_NEW_INVARIANT (this);
  }



  OutputStream ::
  OutputStream (const OutputStream& that)
    : TObject (that), m_label (that.m_label), 
      m_options (that.m_options), m_output (0)
  {
    RCU_READ_INVARIANT (&that);

    if (that.m_output != 0)
    {
      m_output = dynamic_cast<SH::DiskOutput*>(that.m_output->Clone ());
      RCU_ASSERT (m_output != 0);
    }
  }



  OutputStream ::
  ~OutputStream ()
  {
    RCU_DESTROY_INVARIANT (this);

    delete m_output;
  }



  OutputStream& OutputStream ::
  operator = (OutputStream that)
  {
    // no invariant used
    that.swap (*this);
    return *this;
  }



  void OutputStream ::
  swap (OutputStream& that)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_CHANGE_INVARIANT (&that);

    m_label.swap (that.m_label);
    m_options.swap (that.m_options);

    SH::DiskOutput *tmp_output = m_output;
    m_output = that.m_output;
    that.m_output = tmp_output;
  }



  const std::string& OutputStream ::
  label () const
  {
    RCU_READ_INVARIANT (this);
    return m_label;
  }



  void OutputStream ::
  label (const std::string& val_label)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (!val_label.empty());
    m_label = val_label;
  }



  SH::MetaObject *OutputStream ::
  options ()
  {
    RCU_READ_INVARIANT (this);
    return &m_options;
  }



  const SH::MetaObject *OutputStream ::
  options () const
  {
    RCU_READ_INVARIANT (this);
    return &m_options;
  }



  const SH::DiskOutput *OutputStream ::
  output() const
  {
    RCU_READ_INVARIANT (this);
    return m_output;
  }



  void OutputStream ::
  output (SH::DiskOutput *output_swallow)
  {
    RCU_CHANGE_INVARIANT (this);
    delete m_output;
    m_output = output_swallow;
  }
}
