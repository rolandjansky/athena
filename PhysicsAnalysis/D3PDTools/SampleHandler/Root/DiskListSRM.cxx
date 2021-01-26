/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <SampleHandler/DiskListSRM.h>

#include <vector>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace SH
{
  void DiskListSRM :: 
  testInvariant () const
  {
  }



  DiskListSRM :: 
  DiskListSRM (const std::string& val_dir)
    : m_dir (val_dir), m_prefix (val_dir)
  {
    RCU_NEW_INVARIANT (this);
  }



  DiskListSRM ::
  DiskListSRM (const std::string& val_dir, const std::string& val_prefix)
    : m_dir (val_dir), m_prefix (val_prefix)
  {
    RCU_NEW_INVARIANT (this);
  }



  bool DiskListSRM ::
  getNext ()
  {
    RCU_CHANGE_INVARIANT (this);

    if (m_list.empty())
    {
      m_list = RCU::Shell::exec_read ("srmls " + m_dir);
      std::string::size_type split1 = m_list.find ('\n');
      if (split1 == std::string::npos)
	return false;
      m_list = m_list.substr (split1 + 1);
    }

    while (!m_list.empty())
    {
      std::string::size_type split1 = m_list.find ('\n');
      if (split1 == std::string::npos)
	return false;

      const std::string line = m_list.substr (0, split1);
      m_list = m_list.substr (split1 + 1);
      if (line.size() > 2)
      {
	std::string::size_type split2 = line.rfind ('/', line.size()-2);
	if (split2 != std::string::npos)
	{
	  m_file = line.substr (split2 + 1);
	  if (m_file[m_file.size()-1] == '/')
	  {
	    m_isDir = true;
	    m_file = m_file.substr (0, m_file.size()-1);
	  } else
	    m_isDir = false;
	  return true;
	}
      }
    }
    return false;
  }



  std::string DiskListSRM ::
  getPath () const
  {
    RCU_READ_INVARIANT (this);
    return m_prefix + "/" + m_file;
  }



  DiskList *DiskListSRM ::
  doOpenDir () const
  {
    RCU_READ_INVARIANT (this);

    if (m_file.empty() || !m_isDir)
      return 0;

    return new DiskListSRM (m_dir + "/" + m_file, m_prefix + "/" + m_file);
  }



  std::string DiskListSRM ::
  getDirname () const
  {
    RCU_READ_INVARIANT (this);
    return m_dir;
  }
}
