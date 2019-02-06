/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef SAMPLE_HANDLER__DISK_OUTPUT_H
#define SAMPLE_HANDLER__DISK_OUTPUT_H


#include <SampleHandler/Global.h>

#include <TObject.h>

namespace SH
{
  /// \brief a class representing an output location for files
  ///
  /// \par Rationale
  ///   This is used by EventLoop to specify locations where output
  ///   n-tuples should go if they are not to be stored in the
  ///   submission directory
  /// \par Rationale
  ///   This inherits from TObject, so that it can be written to a
  ///   file
  /// \warning This interface may still change, if new output types
  ///   are added that need new functionality.
  class DiskOutput : public TObject
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    DiskOutput ();


    /// \brief create a writer object for a file with the id
    ///
    /// \param sample the name of the sample this belongs to
    /// \param name the name of the file to create
    /// \param index the index of the file within the sample, or
    ///   otherwise -1
    /// \param suffix the suffix to use for the file
    /// \return the writer object
    /// \pre !sample.empty() || !name.empty()
    /// \pre index >= -1
    /// \post result != 0
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    /// \par Failures
    ///   i/o errors
    /// rationale: you don't need to set all of these, but you need to
    ///   set at least either the name or the sample.
  public:
    DiskWriter *
    makeWriter (const std::string& sample, const std::string& name,
		int index, const std::string& suffix) const;



    //
    // virtual interface
    //

    /// \copydoc makeWriter()
    /// \par Rationale
    ///   the virtual part of DiskOutput::makeWriter
  protected:
    virtual DiskWriter *
    doMakeWriter (const std::string& sample, const std::string& name,
		  int index, const std::string& suffix) const;



    //
    // private interface
    //

    ClassDef (DiskOutput, 1);
  };
}

#endif
