/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_SAMPLE_HIST_HH
#define SAMPLE_HANDLER_SAMPLE_HIST_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <string>
#include <SampleHandler/Sample.h>

namespace SH
{
  /// \brief A sample that represents a single histogram file.
  ///
  /// This sample is kind of unusual, the main purpose for having it
  /// is to describe the histogram files produced by EventLoop
  /// together with all their meta-data.
  class SampleHist : public Sample
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
    SampleHist ();


    /// \brief standard constructor
    ///
    /// \param name the name of the sample
    /// \param file the histogram file
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SampleHist (const std::string& name, const std::string& file);



    //
    // interface inherited from Sample
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

    /// \copydoc Sample::doReadHist
  protected:
    virtual TObject *doReadHist (const std::string& name) const;



    //
    // private interface
    //

    /// \brief the path to the file we use
  private:
    std::string m_file;

    ClassDef (SampleHist, 1);
  };
}

#endif
