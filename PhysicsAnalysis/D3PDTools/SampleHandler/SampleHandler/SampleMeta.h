/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__SAMPLE_META_H
#define SAMPLE_HANDLER__SAMPLE_META_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <SampleHandler/Sample.h>

namespace SH
{
  /// \brief A Sample that consists only of Meta-Information.
  ///
  /// The purpose of this sample is that you can fill an entire
  /// SampleHandler with just Meta-Information and then pull the data
  /// for the samples you are actually using into your local sample
  /// via SampleHandler::fetchDefaults().
  class SampleMeta : public Sample
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
    SampleMeta ();


    /// \brief standard constructor
    ///
    /// \param name the name of the sample
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SampleMeta (const std::string& name);



    //
    // inherited interface
    //

    /// \copydoc Sample::getNumFiles
  private:
    virtual std::size_t getNumFiles () const;

    /// \copydoc Sample::getFileName
  private:
    virtual std::string getFileName (std::size_t index) const;

    /// \copydoc Sample::doMakeLocal
  private:
    virtual SamplePtr doMakeLocal () const;

    /// \copydoc Sample::doMakeFileList
  protected:
    virtual std::vector<std::string> doMakeFileList () const;



    //
    // private interface
    //

    ClassDef (SampleMeta, 1);
  };
}

#endif
