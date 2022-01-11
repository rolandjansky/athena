/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/ToolsDiscovery.h>

#include <CxxUtils/StringUtils.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/StringUtil.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskListLocal.h>
#include <SampleHandler/GridTools.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleGrid.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SamplePtr.h>
#include <SampleHandler/ScanDir.h>
#include <TChain.h>
#include <TChainElement.h>
#include <TFile.h>
#include <TTree.h>
#include <fstream>
#include <memory>

//
// method implementations
//

namespace SH
{
  void scanDir (SampleHandler& sh, DiskList& list,
		const std::string& pattern,
		const std::string& samplePattern,
		const std::string& samplePostfix)
  {
    ScanDir()
      .sampleDepth (0)
      .minDepth (1)
      .maxDepth (1)
      .filePattern (pattern)
      .samplePattern (samplePattern)
      .samplePostfix (samplePostfix)
      .scan (sh, list);
  }



  void scanDir (SampleHandler& sh, const std::string& dir)
  {
    ScanDir()
      .sampleDepth (0)
      .minDepth (1)
      .maxDepth (1)
      .scan (sh, dir);
  }



  void scanDir (SampleHandler& sh, const std::string& dir,
		const std::string& prefix)
  {
    DiskListLocal list (dir, prefix);
    ScanDir()
      .sampleDepth (0)
      .minDepth (1)
      .maxDepth (1)
      .scan (sh, list);
  }



  void scanFiles (SampleHandler& sh, DiskList& list,
		  const std::string& pattern)
  {
    ScanDir()
      .sampleDepth (-1)
      .filePattern (pattern)
      .samplePostfix (".*")
      .scan (sh, list);
  }



  Sample *makeFromTChain (const std::string& name, const TChain& chain)
  {
    std::unique_ptr<SampleLocal> result (new SampleLocal (name));
    result->meta()->setString (MetaFields::treeName, chain.GetName());

    TIter chainIter (chain.GetListOfFiles());
    TChainElement *chainElement = 0;
    while ((chainElement = dynamic_cast<TChainElement*>(chainIter.Next())) != 0)
      result->add (chainElement->GetTitle());
    return result.release();
  }



  void scanSingleDir (SampleHandler& sh, const std::string& name,
		      DiskList& list, const std::string& pattern)
  {
    ScanDir()
      .sampleDepth (0)
      .filePattern (pattern)
      .sampleRename ("*", name)
      .scan (sh, list);
  }



  void scanDQ2 (SampleHandler& sh, const std::string& pattern)
  {
    if (pattern.find ("*") == std::string::npos)
    {
      addGrid (sh, pattern);
    } else
    {
      std::set<std::string> types = {"DATASET", "DIDType.DATASET"};
      if (pattern.back() == '/')
	types = {"CONTAINER", "DIDType.CONTAINER"};

      auto subresult = rucioListDids (pattern);
      for (auto& ds : subresult)
      {
	if (types.find (ds.type) != types.end())
	  addGrid (sh, ds.scope + ":" + ds.name);
      }
    }
  }



  void scanRucio (SampleHandler& sh, const std::string& pattern,
		  bool alwaysQuery)
  {
    if (pattern.find ("*") == std::string::npos && !alwaysQuery)
    {
      addGrid (sh, pattern);
    } else
    {
      auto subresult = rucioListDids (pattern);
      bool added = false;
      for (std::string type : {"CONTAINER", "DIDType.CONTAINER", "DATASET", "DIDType.DATASET"})
      {
	for (auto& ds : subresult)
	{
	  if (ds.type == type)
	  {
	    addGrid (sh, ds.scope + ":" + ds.name);
	    added = true;
	  }
	}
	if (added)
	  return;
      }
      RCU_THROW_MSG ("failed to find any datasets matching pattern: " + pattern);
    }
  }



  void addGrid (SampleHandler& sh, const std::string& ds)
  {
    RCU_ASSERT_SOFT (ds.find ("*") == std::string::npos);

    std::string name;
    if (ds[ds.size()-1] == '/')
      name = ds.substr (0, ds.size()-1);
    else
      name = ds;

    auto sample = std::make_unique<SampleGrid> (name);
    sample->meta()->setString (MetaFields::gridName, ds);
    sample->meta()->setString (MetaFields::gridFilter, MetaFields::gridFilter_default);
    sh.add (sample.release());
  }



  void addGridCombined (SampleHandler& sh, const std::string& dsName,
                        const std::vector<std::string>& dsList)
  {
    std::string name;
    for (const std::string &ds : dsList)
    {
      RCU_ASSERT_SOFT (ds.find ("*") == std::string::npos);

      if (!name.empty())
        name.append(",");

      if (ds.at(ds.size() - 1) == '/')
        name.append(ds.substr (0, ds.size() - 1));
      else
        name.append(ds);
    }

    auto sample = std::make_unique<SampleGrid> (dsName);
    sample->meta()->setString (MetaFields::gridName, name);
    sample->meta()->setString (MetaFields::gridFilter, MetaFields::gridFilter_default);
    sh.add (sample.release());
  }


  void addGridCombinedFromFile (SampleHandler& sh, const std::string& dsName,
                                const std::string& dsFile)
  {
    std::ifstream file (dsFile.c_str());

    std::string name;
    std::string ds;
    while (std::getline (file, ds))
    {
      ds = CxxUtils::StringUtils::trim(ds);
      if (ds.empty() || ds.at(0) == '#')
        continue;

      RCU_ASSERT_SOFT (ds.find ("*") == std::string::npos);

      if (!name.empty())
        name.append(",");

      if (ds.at(ds.size() - 1) == '/')
        name.append(ds.substr (0, ds.size() - 1));
      else
        name.append(ds);
    }
    if (!file.eof())
      RCU_THROW_MSG ("failed to read file: " + dsFile);

    auto sample = std::make_unique<SampleGrid> (dsName);
    sample->meta()->setString (MetaFields::gridName, name);
    sample->meta()->setString (MetaFields::gridFilter, MetaFields::gridFilter_default);
    sh.add (sample.release());
  }


  void makeGridDirect (SampleHandler& sh, const std::string& disk,
		       const std::string& from, const std::string& to,
		       bool allow_partial)
  {
    SampleHandler mysh;

    for (SampleHandler::iterator sample = sh.begin(),
	   end = sh.end(); sample != end; ++ sample)
    {
      SampleGrid *grid = dynamic_cast<SampleGrid*>(*sample);

      if (grid == 0)
      {
	mysh.add (*sample);
      } else
      {
	const std::string ds = grid->meta()->castString (MetaFields::gridName);
	if (ds.empty())
	  RCU_THROW_MSG ("no dataset configured for grid dataset " + ds);

	boost::regex pattern (RCU::glob_to_regexp (grid->meta()->castString (MetaFields::gridFilter, MetaFields::gridFilter_default)));

        std::set<std::string> knownFiles;
        std::map<std::string,std::string> usedFiles;
        for (auto& entry : rucioListFileReplicas (ds))
        {
          if (RCU::match_expr (pattern, entry.name))
          {
            knownFiles.insert (entry.name);
            if (entry.disk == disk)
            {
              std::string url = entry.replica;
              const auto split = url.find (from);
              if (split != std::string::npos)
                url = url.substr (0, split) + to + url.substr (split + from.size());
              usedFiles[entry.name] = url;
            }
          }
        }

        if (usedFiles.empty())
        {
          if (allow_partial)
	    RCU_WARN_MSG ("dataset " + ds + " not at " + disk + ", skipped");
        } else if (knownFiles.size() != usedFiles.size())
        {
          if (allow_partial)
          {
            RCU_WARN_MSG ("only incomplete version of dataset " + ds + " at " + disk);
          } else
          {
            usedFiles.clear ();
          }
        }

        if (usedFiles.size() == 0)
        {
	  sh.add (*sample);
        } else
	{
	  std::unique_ptr<SampleLocal> mysample
	    (new SampleLocal (grid->name()));
	  *mysample->meta() = *grid->meta();

	  for (const auto& file : usedFiles)
	  {
            mysample->add (file.second);
	  }
	  mysh.add (mysample.release());
	}
      }
    }
    swap (sh, mysh);
  }



  void scanForTrees (SampleHandler& sh, const Sample& sample,
		     const std::string& pattern)
  {
    SamplePtr mysample = sample.makeLocal();
    if (mysample->numFiles() == 0)
    {
      sh.add (const_cast<Sample*>(&sample));
      return;
    }
    std::unique_ptr<TFile> file (TFile::Open (mysample->fileName(0).c_str()));
    if (!file.get())
      RCU_THROW_MSG ("could not open file: " + mysample->fileName(0));
    TObject *object = 0;
    boost::regex mypattern (pattern);
    for (TIter iter (file->GetListOfKeys()); (object = iter.Next()); )
    {
      if (RCU::match_expr (mypattern, object->GetName()) &&
	  dynamic_cast<TTree*>(file->Get(object->GetName())))
      {
	std::string newName = sample.name() + "_" + object->GetName();
        std::unique_ptr<Sample> newSample
	  (dynamic_cast<Sample*>(sample.Clone (newName.c_str())));
        newSample->name (newName);
        newSample->meta()->setString (MetaFields::treeName, object->GetName());
	sh.add (newSample.release());
      }
    }
  }



  void scanForTrees (SampleHandler& sh, const std::string& pattern)
  {
    SH::SampleHandler sh_new;

    for (SampleHandler::iterator sample = sh.begin(),
	   end = sh.end(); sample != end; ++ sample)
    {
      scanForTrees (sh_new, **sample, pattern);
    }
    swap (sh, sh_new);
  }



  void readFileList (SampleHandler& sh, const std::string& name,
		     const std::string& file)
  {
    std::ifstream myfile (file.c_str());

    auto sample = std::make_unique<SampleLocal> (name);
    std::string line;
    while (std::getline (myfile, line))
    {
      line = CxxUtils::StringUtils::trim(line);
      if (!line.empty() && line.at(0) != '#')
      {
	sample->add (line);
      }
    }
    if (!myfile.eof())
      RCU_THROW_MSG ("failed to read file: " + file);
    sh.add (sample.release());
  }
}
