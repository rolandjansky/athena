/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__DISK_OUTPUT_XRD_H
#define SAMPLE_HANDLER__DISK_OUTPUT_XRD_H

//        Copyright Iowa State University 2013.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <SampleHandler/DiskOutput.h>

namespace SH
{
  /// \brief an implementation of DiskOutput using the XRD protocol
  ///
  /// \par Rationale
  ///   this is to be used when storing output n-tuples on xrootd servers
  class DiskOutputXRD final : public DiskOutput
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard default constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
    /// \warning this is only to be used for the root streaming
    ///   mechanism, not by the end user
  public:
    DiskOutputXRD ();


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    /// \pre val_prefix.find ("root://") == 0
  public:
    explicit DiskOutputXRD (const std::string& val_prefix);



    //
    // inherited interface
    //

  protected:
    virtual std::unique_ptr<DiskWriter>
    doMakeWriter (const std::string& sampleName,
                  const std::string& segmentName,
		  const std::string& suffix) const;

  protected:
    virtual std::string
    getTargetURL (const std::string& sampleName,
                  const std::string& segmentName,
                  const std::string& suffix) const;



    //
    // private interface
    //

    /// \brief the prefix to apply when creating files
  private:
    std::string m_prefix;

    ClassDef (DiskOutputXRD, 1);
  };
}

#endif
