//          Copyright Nils Krumnack 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/Locate.h>

#include <deque>
#include <TSystem.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace RCU
{
  std::string locate (const std::string& locations)
  {
    std::deque<std::string> files;
    {
      std::string::size_type pos = 0, pos2 = 0;
      while ((pos2 = locations.find ("::", pos)) != std::string::npos)
      {
	files.push_back (locations.substr (pos, pos2 - pos));
	pos = pos2 + 2;
      }
      files.push_back (locations.substr (pos));
    }
    std::string name;
    {
      for (std::deque<std::string>::const_iterator file = files.begin(),
	     end = files.end(); file != end; ++ file)
      {
	std::string::size_type split = file->rfind ('/');
	if (split == std::string::npos)
	  RCU_THROW_MSG ("file name " + *file + " does not contain a \"/\"");
	std::string myname = file->substr (split + 1);
	if (myname.empty())
	  RCU_THROW_MSG ("file name " + *file + " should not end with a \"/\"");
	if (name.empty())
	  name = myname;
	else if (name != myname)
	  RCU_THROW_MSG ("inconsistent file names " + name + " and " + myname);
      }
    }
    RCU_ASSERT (!name.empty());

    files.push_front ("$ROOTCOREBIN/data/RootCoreUtils/download/" + name);
    for (std::deque<std::string>::iterator file = files.begin(),
	   end = files.end(); file != end; ++ file)
    {
      TString myfile = *file;
      gSystem->ExpandPathName (myfile);
      *file = myfile.Data();
    }

    for (std::deque<std::string>::const_iterator file = files.begin(),
	   end = files.end(); file != end; ++ file)
    {
      if (file->find ("http://") == 0)
      {
	try
	{
	  Shell::exec ("$ROOTCOREDIR/scripts/download.sh " + Shell::quote (*file) + " " + Shell::quote (files[0]));
	  return files[0];
	} catch (...)
	{
	  // rationale: ignoring all exceptions, since we might have
	  //   more locations to try
	}
      } else
      {
	if (gSystem->AccessPathName (file->c_str()) == 0)
	{
	  return *file;
	}
      }
    }
    RCU_THROW_MSG ("failed to find file at " + locations);
    return ""; // compiler dummy
  }
}
