/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SAMPLE_HANDLER__DISK_WRITER_XRD_H
#define SAMPLE_HANDLER__DISK_WRITER_XRD_H


#include <SampleHandler/Global.h>

#include <SampleHandler/DiskWriter.h>
#include <memory>

namespace SH
{
  /// \brief an implementation of DiskWriter for the XRD protocol
  /// \sa DiskOutputXRD
  class DiskWriterXRD final : public DiskWriter
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
    /// \pre val_path.find ("root://") == 0
  public:
    DiskWriterXRD (const std::string& val_path);


    /// \brief standard destructor
    /// \par Guarantee
    ///   no-fail
  public:
    ~DiskWriterXRD ();



    //
    // inherited interface
    //

    /// \copydoc DiskWriter::getPath()
  private:
    virtual std::string getPath () const override;


    /// \copydoc DiskWriter::getFile()
  private:
    virtual TFile *getFile () override;


    /// \copydoc DiskWriter::doClose()
  private:
    virtual void doClose () override;



    //
    // private interface
    //

    /// \brief the temporary path being used
  private:
    std::string m_tmp;

    /// \brief the path being used
  private:
    std::string m_path;

    /// \brief the actual file object
  private:
    std::unique_ptr<TFile> m_file;
  };
}

#endif
