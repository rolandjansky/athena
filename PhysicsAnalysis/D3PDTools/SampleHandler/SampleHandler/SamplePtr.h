/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_SAMPLE_PTR_HH
#define SAMPLE_HANDLER_SAMPLE_PTR_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <memory>

namespace SH
{
  /// \brief A smart pointer class that holds a single Sample object.
  ///
  /// To avoid various memory management issues, samples are usually
  /// held via such a smart pointer.  Since the reference count is
  /// internal to the Sample object it is safe to assign the same
  /// Sample to multiple smart pointers.
  /// \par Rationale
  ///   This class is mostly to make SampleHandler independent of
  ///   boost, otherwise I could have used boost::intrusive_ptr.
  /// \relates Sample
  class SamplePtr
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    SamplePtr ();


    /// \brief initializing constructor
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    SamplePtr (Sample *val_sample);


    /// \brief initializing constructor
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    explicit SamplePtr (std::unique_ptr<Sample> val_sample);


    /// \brief standard copy constructor
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    SamplePtr (const SamplePtr& that);


    /// \brief standard destructor
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    ~SamplePtr ();


    /// \brief standard assignment operator
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    SamplePtr& operator = (const SamplePtr& that);


    /// \brief get() == 0
    ///
    /// \return get() == 0
    /// \par Guarantee
    ///   no-fail
  public:
    bool empty () const;


    /// \brief the sample itself
    ///
    /// \return the sample itself
    /// \par Guarantee
    ///   no-fail
  public:
    Sample *get () const;


    /// \brief the sample itself
    ///
    /// \return the sample itself
    /// \par Guarantee
    ///   no-fail
    /// \pre !empty()
  public:
    Sample *operator -> () const;


    /// \brief the sample itself
    ///
    /// \return the sample itself
    /// \par Guarantee
    ///   no-fail
    /// \pre !empty()
  public:
    Sample& operator * () const;



    //
    // private interface
    //

    /// \brief the sample contained
  private:
    Sample *m_sample;
  };
}

#endif
