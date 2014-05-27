//          Copyright Nils Krumnack 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/hadd.h>

#include <sstream>
#include <TFileMerger.h>
#include <TList.h>
#include <TSystem.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/PrintMsg.h>

//
// method implementations
//

namespace RCU
{
  void hadd (std::string output_file,
	     std::vector<std::string> input_files,
	     unsigned max_files)
  {
    std::ostringstream cmd;
    cmd << "$ROOTCOREBIN/user_scripts/RootCoreUtils/rcu_hadd " << max_files
	<< " " << output_file;
    for (std::vector<std::string>::const_iterator input = input_files.begin(),
	   end = input_files.end(); input != end; ++ input)
    {
      cmd << " " << *input;
    }
    int result = gSystem->Exec (cmd.str().c_str());
    if (result != 0)
      RCU_THROW_MSG ("failed to make merged file: " + output_file);
  }



  void hadd_core (std::string output_file,
		  std::vector<std::string> input_files,
		  unsigned max_files)
  {
    TFileMerger merger (false, false);

    merger.SetMsgPrefix ("rcu_hadd");
    merger.SetPrintLevel (98);

    if (max_files > 0)
    {
      merger.SetMaxOpenedFiles (max_files);
    }

    if (!merger.OutputFile (output_file.c_str(), false, 1) )
    {
      RCU_THROW_MSG ("error opening target file: " + output_file);
    }

    for (std::vector<std::string>::const_iterator input = input_files.begin(),
	   end = input_files.end(); input != end; ++ input)
    {
      if (!merger.AddFile (input->c_str()))
      {
	RCU_THROW_MSG ("error adding input file: " + *input);
      }         
    }
    merger.SetNotrees (false);

    bool status = merger.Merge();

    if (status)
    {
      std::ostringstream msg;
      msg << "merged " << merger.GetMergeList()->GetEntries()
	  << " input files into " << output_file;
      RCU_PRINT_MSG (msg.str());
    } else
    {
      RCU_THROW_MSG ("hadd failure during the merge");
    }
  }
}
