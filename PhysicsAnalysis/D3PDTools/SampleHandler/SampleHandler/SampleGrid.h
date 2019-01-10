/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_SAMPLE_GRID_HH
#define SAMPLE_HANDLER_SAMPLE_GRID_HH

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
  /// \brief This class implements a Sample located on the grid.
  ///
  /// This sample is (somewhat) special, in that it doesn't contain a
  /// file list per se.  Instead it relies on its meta-data entries to
  /// identify a grid dataset or dataset container, and a pattern for
  /// the files to use within.
  ///
  /// The main purpose as such is to pass it as an input sample into
  /// the EL::GridDriver.  However, in practice one can still use this
  /// like a regular sample (using direct access via FAX, or
  /// pre-loading via rucio).
  ///
  /// Normally you will not create these samples directly, but rely on
  /// the various data discovery methods, or one at a time via \ref
  /// addGrid()
  ///
  /// \sa https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SampleHandler#Grid_Data_Discovery_Using_DQ2
  class SampleGrid : public Sample
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
    /// \warning
    ///   only to be used by serialization code
  public:
    SampleGrid ();


    /// \brief standard constructor
    ///
    /// \param name the name of the sample
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   low level errors II
  public:
    SampleGrid (const std::string& name);



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



    //
    // private interface
    //

    ClassDef (SampleGrid, 1);
  };
}

#endif
