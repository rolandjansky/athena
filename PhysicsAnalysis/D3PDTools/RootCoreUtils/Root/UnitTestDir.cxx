//          Copyright Nils Krumnack 2011 - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/UnitTestDir.h>

#include <cstdlib>
#include <TSystem.h>
#include <TString.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace RCU
{
  void UnitTestDir ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  UnitTestDir ::
  UnitTestDir (const std::string& package, const std::string& name)
    : m_cleanup (getenv ("ROOTCORE_AUTO_UT") != 0)
  {
    TString output = "/tmp";
    {
      const char *tmpdir = getenv ("TMPDIR");
      if (tmpdir)
	output = tmpdir;
    };
    output += ("/ut-" + package + "-" + name + ".$$").c_str();
    gSystem->ExpandPathName (output);
    m_path = output.Data ();
    
    if (gSystem->MakeDirectory (output.Data()) != 0)
      RCU_THROW_MSG (("could not create output directory " + output).Data());
  }



  UnitTestDir ::
  ~UnitTestDir ()
  {
    RCU_DESTROY_INVARIANT (this);

    if (m_cleanup)
      gSystem->Exec (("rm -rf " + m_path).c_str());
    else
      RCU_PRINT_MSG ("unit test data located at " + m_path);
  }



  const std::string& UnitTestDir ::
  path () const
  {
    RCU_READ_INVARIANT (this);
    return m_path;
  }



  bool UnitTestDir ::
  cleanup () const
  {
    RCU_READ_INVARIANT (this);
    return m_cleanup;
  }



  void UnitTestDir ::
  cleanup (bool val_cleanup)
  {
    RCU_CHANGE_INVARIANT (this);
    m_cleanup = val_cleanup;
  }
}
