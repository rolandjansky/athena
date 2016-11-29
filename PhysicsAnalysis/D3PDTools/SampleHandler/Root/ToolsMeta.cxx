/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/ToolsMeta.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/Sample.h>
#include <TSystem.h>
#include <fstream>
#include <sstream>

//
// method implementations
//

namespace SH
{
  void readSusyMeta (const SampleHandler& sh, const std::string& inputFile)
  {
    TString myfile = inputFile;
    gSystem->ExpandPathName (myfile);
    std::ifstream file (myfile.Data());
    std::string line;

    if (!file)
      RCU_THROW_MSG ("failed to read file: " + inputFile);

    while (std::getline (file, line))
    {
      if (line[0] >= '0' && line[0] <= '9')
      {
	std::istringstream str (line);
	std::string DSID, name;
	float crossSection = 0, kFactor = 0, filterEfficiency = 0,
	  xsUncertainty = 0;

	if (!(str >> DSID >> name >> crossSection >> kFactor >> filterEfficiency
	      >> xsUncertainty))
	  RCU_THROW_MSG ("failed to parse line: " + line);

	std::string mydsid = "." + DSID + ".";
	for (SampleHandler::iterator sample = sh.begin(),
	       end = sh.end(); sample != end; ++ sample)
	{
	  if ((*sample)->name().find (mydsid) != std::string::npos)
	  {
	    (*sample)->meta()->setDouble (MetaFields::crossSection, crossSection);
	    (*sample)->meta()->setDouble (MetaFields::kfactor, kFactor);
	    (*sample)->meta()->setDouble (MetaFields::filterEfficiency, filterEfficiency);
	    (*sample)->meta()->setDouble (MetaFields::crossSectionRelUncertainty, xsUncertainty);
	  }
	}
      }
    }
  }

  void readSusyMetaDir (const SampleHandler& sh, const std::string& inputDir)
  {
    TString mydir = inputDir;
    gSystem->ExpandPathName (mydir);
    void *dirp = 0;

    try
    {
      dirp = gSystem->OpenDirectory (mydir.Data());
      const char *file = 0;
      while ((file = gSystem->GetDirEntry (dirp)))
      {
	std::string myfile = inputDir + "/" + file;
	if (myfile.size() > 4 && myfile.substr (myfile.size()-4) == ".txt")
	  readSusyMeta (sh, myfile);
      }
      gSystem->FreeDirectory (dirp);
    } catch (...)
    {
      gSystem->FreeDirectory (dirp);
      throw;
    }
  }
}
