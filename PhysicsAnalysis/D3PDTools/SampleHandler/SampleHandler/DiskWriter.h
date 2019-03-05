/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef SAMPLE_HANDLER__DISK_WRITER_H
#define SAMPLE_HANDLER__DISK_WRITER_H


#include <SampleHandler/Global.h>

class TFile;
#include <string>

namespace SH
{
  /// \brief an interface that manages writing a single output file
  ///
  /// \par Rationale
  ///   This interface allows to write files to arbitrary storage
  ///   systems.  Most importantly, it allows to execute actions after
  ///   a file has been closed.
  /// \warning This interface may still change, if new output types
  ///   are added that need new functionality.
  /// \sa DiskOutput
  class DiskWriter
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
  public:
    DiskWriter ();


    /// \brief standard destructor
    ///
    /// \par Guarantee
    ///   no-fail
    /// \par Rationale
    ///   virtual destructor for base class
  public:
    virtual ~DiskWriter ();


    /// \brief the path where this file can be accessed or the
    ///   empty string if it is not known (yet).
    ///
    /// \par Rationale
    ///   while it is not guaranteed that every writer will
    ///   know how to handle this, most writers will, and it can be
    ///   useful.  however, not every writer will know this until
    ///   after the file is closed.
  public:
    std::string path () const;


    /// \brief the file we are writing to
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   file already closed
    /// \par Postcondition
    ///   result != 0
  public:
    TFile *file ();


    /// \brief closes the file we are writing to
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors\n
    ///   file already closed
  public:
    void close ();



    //
    // virtual interface
    //

    /// \copydoc path()
    /// \par Rationale
    ///   the virtual part of DiskWriter::path
  private:
    virtual std::string getPath () const = 0;


    /// \copydoc file()
    /// \par Rationale
    ///   the virtual part of DiskWriter::file
  private:
    virtual TFile *getFile () = 0;


    /// \copydoc close()
    /// \par Rationale
    ///   the virtual part of DiskWriter::close
  private:
    virtual void doClose () = 0;
  };
}

#endif
