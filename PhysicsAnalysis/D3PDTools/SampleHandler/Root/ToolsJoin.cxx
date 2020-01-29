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

#include <SampleHandler/ToolsJoin.h>

#include <memory>
#include <RootCoreUtils/StringUtil.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>

//
// method implementations
//

namespace SH
{
  void mergeSamples (SampleHandler& sh, const std::string& sampleName,
		     const std::string& pattern)
  {
    SampleHandler mysh;
    std::unique_ptr<SampleLocal> mysample (new SampleLocal (sampleName));

    boost::regex mypattern (pattern.c_str());
    for (SampleHandler::iterator sample = sh.begin(),
	   end = sh.end(); sample != end; ++ sample)
    {
      if (RCU::match_expr (mypattern, (*sample)->name()))
      {
	for (unsigned file = 0, end = (*sample)->numFiles();
	     file != end; ++ file)
	{
	  mysample->add ((*sample)->fileName (file));
	}
      } else
      {
	mysh.add (*sample);
      }
    }
    mysh.add (mysample.release());
    swap (mysh, sh);
  }
}
