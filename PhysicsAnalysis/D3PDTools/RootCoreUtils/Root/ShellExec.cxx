//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/ShellExec.h>

#include <cstdio>
#include <vector>
#include <TSystem.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace RCU
{
  namespace Shell
  {
    void exec (const std::string& cmd)
    {
      if (gSystem->Exec (cmd.c_str()) != 0)
	RCU_THROW_MSG ("command failed: " + cmd);
    }



    std::string exec_read (const std::string& cmd)
    {
      int rc = 0;
      std::string result = exec_read (cmd, rc);
      if (rc != 0)
	RCU_THROW_MSG ("command failed: " + cmd + "\nwith output:\n" + result);
      return result;
    }



    std::string exec_read (const std::string& cmd, int& rc)
    {
      std::string result;
      FILE *pipe = 0;
      try
      {
	std::vector<char> buffer (1024);
	size_t read;
	  
	pipe = popen (cmd.c_str(), "r");
	while ((read = fread (&buffer[0], 1, buffer.size(), pipe)) > 0)
	{
	  result.append (&buffer[0], read);
	}
	rc = pclose (pipe);
	pipe = nullptr;
	return result;
      } catch (...)
      {
	if (pipe)
	  rc = pclose (pipe);
	throw;
      }
    }



    std::string quote (const std::string& name)
    {
      std::string result;
      for (std::string::const_iterator iter = name.begin(),
	     end = name.end(); iter != end; ++ iter)
      {
	if (!isalnum (*iter) && *iter != '/' && *iter != '.' &&
	    *iter != '-')
	  result += '\\';
	result += *iter;
      };
      if (result.empty())
	result = "\"\"";
      return result;
    }
  }
}
