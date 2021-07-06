/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/DiskListLocal.h>

#include <memory>
#include <TSystem.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace SH
{
  void DiskListLocal :: 
  testInvariant () const
  {
  }



  DiskListLocal :: 
  DiskListLocal (const std::string& val_dir)
    : m_dir (val_dir), m_prefix (val_dir), m_dirp (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  DiskListLocal ::
  DiskListLocal (const std::string& val_dir, const std::string& val_prefix)
    : m_dir (val_dir), m_prefix (val_prefix), m_dirp (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  DiskListLocal :: 
  ~DiskListLocal ()
  {
    RCU_DESTROY_INVARIANT (this);

    if (m_dirp)
    {
      gSystem->FreeDirectory (m_dirp);
      m_dirp = 0;
    }
  }



  bool DiskListLocal ::
  getNext ()
  {
    RCU_CHANGE_INVARIANT (this);

    if (!m_dirp)
    {
      m_dirp = gSystem->OpenDirectory (m_dir.c_str());
      if (!m_dirp)
	RCU_THROW_MSG ("could not open directory " + m_dir);
    }

    const char *subresult = 0;
    do
    {
      subresult = gSystem->GetDirEntry (m_dirp);
      if (subresult == 0)
      {
	gSystem->FreeDirectory (m_dirp);
	m_dirp = 0;
	m_file.clear ();
	return false;
      }
    } while (strcmp (subresult, ".") == 0 || strcmp (subresult, "..") == 0);
    m_file = subresult;
    return subresult;
  }



  std::string DiskListLocal ::
  getPath () const
  {
    RCU_READ_INVARIANT (this);
    return m_prefix + "/" + m_file;
  }



  DiskList *DiskListLocal ::
  doOpenDir () const
  {
    RCU_READ_INVARIANT (this);

    std::unique_ptr<DiskListLocal> result
      (new DiskListLocal (m_dir + "/" + m_file, m_prefix + "/" + m_file));
    result->m_dirp = gSystem->OpenDirectory (result->m_dir.c_str());
    if (result->m_dirp)
      return result.release();
    return 0;
  }



  std::string DiskListLocal ::
  getDirname () const
  {
    RCU_READ_INVARIANT (this);
    return m_dir;
  }
}
