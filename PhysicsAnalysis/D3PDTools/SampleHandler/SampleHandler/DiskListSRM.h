/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__DISK_LIST_SRM_H
#define SAMPLE_HANDLER__DISK_LIST_SRM_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <SampleHandler/DiskList.h>

namespace SH
{
  /// \brief a DiskList implementation for the SRM protocol
  class DiskListSRM : public DiskList
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


    /// \brief make the listing for the given directory
    ///
    /// \param val_dir the directory to list
    /// \par Guarantee
    ///   strong\n
    /// \par Failures
    ///   out of memory II
  public:
    DiskListSRM (const std::string& val_dir);


    /// \brief make the listing for the given directory, but
    ///   replacing the directory with prefix for the path
    ///
    /// \param val_dir the directory to list
    /// \param val_prefix the prefix with which val_dir will be
    ///   replaced in the reported paths
    /// \par Guarantee
    ///   strong\n
    /// \par Failures
    ///   out of memory II\n
    /// \par Rationale
    ///   this mechanism is meant to allow scanning file
    ///   servers using one protocol, but then accessing them using
    ///   another
  public:
    DiskListSRM (const std::string& val_dir, const std::string& val_prefix);



    //
    // inherited interface
    //

    /// \copydoc DiskList::getNext()
  protected:
    virtual bool getNext ();


    /// \copydoc DiskList::getPath()
  protected:
    virtual std::string getPath () const;


    /// \copydoc DiskList::doOpenDir()
  protected:
    virtual DiskList *doOpenDir () const;


    /// \copydoc DiskList::getDirname()
  protected:
    virtual std::string getDirname () const;



    //
    // private interface
    //

    /// \brief the directory we are reading
  private:
    std::string m_dir;

    /// \brief the directory from with to read actual files
  private:
    std::string m_prefix;

    /// \brief the result of the directory listing
  private:
    std::string m_list;

    /// \brief the last file we read
  private:
    std::string m_file;

    /// \brief whether this is a directory
  private:
    bool m_isDir;
  };
}

#endif
