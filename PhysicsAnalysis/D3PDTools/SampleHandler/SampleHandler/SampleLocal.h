/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_SAMPLE_LOCAL_HH
#define SAMPLE_HANDLER_SAMPLE_LOCAL_HH

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
  /// \brief A Sample based on a simple file list.
  ///
  /// This is the archetypical implementation of a sample, which is
  /// really just a list of file names that can directly be passed to
  /// TFile::Open().
  ///
  /// While you can in principle create these samples yourself, you
  /// will probably do better if you rely on the various data
  /// discovery methods to create the samples for you.
  ///
  /// \sa https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SampleHandler#Data_Discovery
  class SampleLocal : public Sample
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
    SampleLocal ();


    /// \brief standard constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SampleLocal (const std::string& name);


    /// \brief add a file to the list
    ///
    /// \pre !file.empty()
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   low level errors II
  public:
    void add (const std::string& file);



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



    //
    // private interface
    //

    /// \brief the list of files we use
  private:
    std::vector<std::string> m_files;

    /// \brief the iterator for \ref m_files
  private:
    typedef std::vector<std::string>::const_iterator FilesIter;

    /// \brief the mutable iterator for \ref m_files
  private:
    typedef std::vector<std::string>::iterator FilesMIter;

    ClassDef (SampleLocal, 1);
  };
}

#endif
