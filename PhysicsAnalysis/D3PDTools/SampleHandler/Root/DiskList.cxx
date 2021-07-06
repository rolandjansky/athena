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

#include <SampleHandler/DiskList.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace SH
{
  void DiskList :: 
  testInvariant () const
  {
    RCU_INVARIANT (m_state <= S_BROKEN);
  }


  DiskList ::
  ~DiskList ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  DiskList :: 
  DiskList ()
    : m_state (S_BLANK)
  {
    RCU_NEW_INVARIANT (this);
  }



  bool DiskList ::
  next ()
  {
    RCU_CHANGE_INVARIANT (this);
    switch (m_state)
    {
    case S_BLANK:
    case S_VALID:
      m_state = S_BROKEN;
      if (getNext())
	m_state = S_VALID;
      else
	m_state = S_DONE;
      return m_state == S_VALID;
    case S_DONE:
      RCU_THROW_MSG ("already finished processing list");
    case S_BROKEN:
      RCU_THROW_MSG ("list is in error state");
    };
    return false; // compiler dummy
  }



  std::string DiskList ::
  path () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2_SOFT (m_state == S_VALID, "getNext() has not been called successfully");
    return getPath();
  }



  std::string DiskList ::
  fileName () const
  {
    // no invariant used
    std::string result = path();
    std::string::size_type split = result.rfind ('/');
    if (split != std::string::npos)
      result = result.substr (split + 1);
    return result;
  }



  DiskList *DiskList ::
  openDir () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2_SOFT (m_state == S_VALID, "getNext() has not been called successfully");
    return doOpenDir();
  }



  std::string DiskList ::
  dirname () const
  {
    RCU_READ_INVARIANT (this);
    return getDirname ();
  }
}
