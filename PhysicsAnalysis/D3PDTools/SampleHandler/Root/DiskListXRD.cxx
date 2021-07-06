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

#include <SampleHandler/DiskListXRD.h>

#include <sstream>
#include <vector>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>

#include <iostream>

//
// method implementations
//

namespace SH
{
  void DiskListXRD :: 
  testInvariant () const
  {
  }



  DiskListXRD ::
  DiskListXRD (const std::string& val_server, const std::string& val_directory,
	       bool val_laxParser)
    : m_server (val_server), m_directory (val_directory),
      m_laxParser (val_laxParser), m_isRead (false)
  {
    RCU_NEW_INVARIANT (this);
  }



  bool DiskListXRD ::
  getNext ()
  {
    RCU_CHANGE_INVARIANT (this);

    if (!m_isRead)
    {
      std::string command = "xrdfs " + m_server + " ls -l " + m_directory;
      m_list = RCU::Shell::exec_read (command);
      m_context = "command: " + command + "\n" + m_list;
      m_isRead = true;
    }

    while (!m_list.empty())
    {
      std::string::size_type split1 = m_list.find ('\n');
      if (split1 == std::string::npos)
	split1 = m_list.size();

      const std::string line (m_list.substr (0, split1));
      m_list = m_list.substr (split1 + 1);

      std::string::size_type split2 = line.find ('/');
      if (split2 != std::string::npos &&
	  (line[0] == '-' || line[0] == 'd'))
      {
	m_isDir = line[0] == 'd';
	m_file = line.substr (split2);
	return true;
      }

      if (!line.empty())
      {
	std::string message = "failed to parse line: \"" + line + "\"\n" + m_context;

	if (!m_laxParser)
	{
	  RCU_THROW_MSG (message);
	} else
	{
	  RCU_WARN_MSG (message);
	}
      }
    }
    return false;
  }



  std::string DiskListXRD ::
  getPath () const
  {
    RCU_READ_INVARIANT (this);
    return "root://" + m_server + "/" + m_file;
  }



  DiskList *DiskListXRD ::
  doOpenDir () const
  {
    RCU_READ_INVARIANT (this);

    if (m_file.empty() || !m_isDir)
      return 0;

    return new DiskListXRD (m_server, m_file, m_laxParser);
  }



  std::string DiskListXRD ::
  getDirname () const
  {
    RCU_READ_INVARIANT (this);
    return m_directory;
  }
}
