/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_SAMPLE_COMPOSITE_HH
#define SAMPLE_HANDLER_SAMPLE_COMPOSITE_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <string>
#include <vector>
#include <SampleHandler/Sample.h>

namespace SH
{
  /// \brief This module defines an implementation of Sample that
  /// contains composite samples.
  /// \warning This class hasn't been maintained in a long time and
  /// probably has never been fully functional in the first place.
  class SampleComposite : public Sample
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
    ///   strong
    /// \par Failures
    ///   out of memory I
    /// \warning only to be used by serialization code
  public:
    SampleComposite ();


    /// \brief standard constructor
    ///
    /// \param name the name of the sample
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   low level errors II
  public:
    SampleComposite (const std::string& name);


    /// \brief add a sample to the list
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   low level errors II\n
    ///   sample contains this sample
    /// \pre !sample.empty()
  public:
    void add (const SamplePtr& sample);



    //
    // inherited interface
    //

    /// \copydoc Sample::getNumFiles
  protected:
    virtual std::size_t getNumFiles () const;

    /// \copydoc Sample::getFileName
  protected:
    virtual std::string getFileName (std::size_t index) const;

    /// \copydoc Sample::doMakeLocal
  protected:
    virtual SamplePtr doMakeLocal () const;

    /// \copydoc Sample::doMakeFileList
  protected:
    virtual std::vector<std::string> doMakeFileList () const;

    /// \copydoc Sample::doUpdateLocation
  protected:
    virtual void
    doUpdateLocation (const std::string& from, const std::string& to);

    /// \copydoc Sample::getContains
  protected:
    virtual bool getContains (const std::string& name) const;

    /// \copydoc Sample::doAddSamples
  protected:
    virtual void doAddSamples (SampleHandler& result);



    //
    // private interface
    //

    /// \brief the list of samples we use
  private:
    std::vector<SamplePtr> m_samples;

    /// \brief the iterator for \ref m_samples
  private:
    typedef std::vector<SamplePtr>::const_iterator SamplesIter;

    ClassDef (SampleComposite, 1);
  };
}

#endif
