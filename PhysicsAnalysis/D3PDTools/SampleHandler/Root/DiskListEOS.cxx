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

#include <SampleHandler/DiskListEOS.h>

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
  void DiskListEOS :: 
  testInvariant () const
  {
  }



  DiskListEOS :: 
  DiskListEOS (const std::string& val_dir)
    : m_dir (val_dir), m_prefix ("root://eosatlas.cern.ch/" + val_dir), m_isRead (false)
  {
    RCU_NEW_INVARIANT (this);
  }



  DiskListEOS ::
  DiskListEOS (const std::string& val_dir, const std::string& val_prefix)
    : m_dir (val_dir), m_prefix (val_prefix), m_isRead (false)
  {
    RCU_NEW_INVARIANT (this);
  }



  bool DiskListEOS ::
  getNext ()
  {
    RCU_CHANGE_INVARIANT (this);

    if (!m_isRead)
    {
      m_list = RCU::Shell::exec_read ("eos ls -l " + m_dir);
      m_isRead = true;
    }

    while (!m_list.empty())
    {
      std::string::size_type split1 = m_list.find ('\n');
      if (split1 == std::string::npos)
	return false;

      std::string line (m_list.substr (0, split1));
      m_list = m_list.substr (split1 + 1);

      // rationale: this should handle it correctly if there is a
      //   formatting escape sequence at the beginning of the line.
      std::string::size_type split2 = line.find ("r");
      m_isDir = line[split2-1] == 'd';
      line = line.substr (split2);

      std::istringstream str (line);
      std::string fields [9];
      for (unsigned iter = 0, end = 9; iter != end; ++ iter)
      {
	if (!(str >> fields[iter]))
	  RCU_THROW_MSG ("failed to parse line: " + line);
      }
      if (fields[0].empty())
	  RCU_THROW_MSG ("failed to parse line: " + line);
      m_file = fields[8];
      return true;
    }
    return false;
  }



  std::string DiskListEOS ::
  getPath () const
  {
    RCU_READ_INVARIANT (this);
    return m_prefix + "/" + m_file;
  }



  DiskList *DiskListEOS ::
  doOpenDir () const
  {
    RCU_READ_INVARIANT (this);

    if (m_file.empty() || !m_isDir)
      return 0;

    return new DiskListEOS (m_dir + "/" + m_file, m_prefix + "/" + m_file);
  }



  std::string DiskListEOS ::
  getDirname () const
  {
    RCU_READ_INVARIANT (this);
    return m_dir;
  }
}
