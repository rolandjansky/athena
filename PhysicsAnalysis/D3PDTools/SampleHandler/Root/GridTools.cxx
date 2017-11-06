/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@iastate.edu) for
// bug reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/GridTools.h>

#include <AsgTools/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/StringUtil.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaObject.h>
#include <chrono>
#include <mutex>

namespace sh = RCU::Shell;

//
// method implementations
//

namespace SH
{
  ANA_MSG_SOURCE (msgGridTools, "SampleHandler_GridTools")
  using namespace msgGridTools;

  namespace
  {
    struct ProxyData
    {
      // the clock we use
      typedef std::chrono::steady_clock clock;

      // don't really need a mutex as the code unlikely to be
      // multi-threaded, but may just as well put one to protect the
      // global/static variable
      std::recursive_mutex mutex;

      // whether we have confirmed that we do have a proxy
      bool haveProxy = false;

      // the expiration time of the proxy (if we have one)
      decltype(clock::now()) proxyExpiration;

      bool checkVomsProxy ()
      {
	std::lock_guard<std::recursive_mutex> lock (mutex);

	if (haveProxy == false)
	{
	  ANA_MSG_INFO ("checking for valid grid proxy");
	  int rc = 0;
	  std::string output =
	    RCU::Shell::exec_read ("voms-proxy-info --actimeleft", rc);
	  if (rc != 0)
	  {
	    ANA_MSG_INFO ("no valid proxy found");
	  } else
	  {
	    std::istringstream str (output);
	    unsigned seconds = 0;
	    if (!(str >> seconds))
	    {
	      ANA_MSG_INFO ("failed to parse command output: " << output);
	    } else
	    {
	      proxyExpiration = clock::now() + std::chrono::seconds (seconds);
	      haveProxy = true;
	    }
	  }
	}

	return haveProxy &&
	  proxyExpiration > clock::now() + std::chrono::minutes (20);
      }

      void ensureVomsProxy ()
      {
	std::lock_guard<std::recursive_mutex> lock (mutex);

	if (checkVomsProxy())
	  return;

	if (haveProxy)
	{
	  ANA_MSG_INFO ("proxy expired or about to expire");
	} else
	{
	  ANA_MSG_INFO ("no proxy found");
	}
	ANA_MSG_INFO ("trying to set up a new proxy");
	haveProxy = false;
	RCU::Shell::exec ("voms-proxy-init -voms atlas");
	ensureVomsProxy ();
      }
    };

    ProxyData& proxyData ()
    {
      static ProxyData result;
      return result;
    }



    /// \brief read all lines beginning with a specific phrase
    /// (without the phrase itself)
    std::vector<std::string>
    readLineList (const std::string& text,
                  const std::string& begin)
    {
      std::vector<std::string> result;

      for (std::string::size_type split = 0;
           (split = text.find (begin, split)) != std::string::npos;
           ++ split)
      {
        if (split == 0 || text[split-1] == '\n')
        {
          split += begin.size();
          auto split2 = text.find ("\n", split);
          if (split2 == std::string::npos)
            split2 = text.size();
          std::string subresult = text.substr (split, split2 - split);
          while (isspace (subresult.front()))
            subresult = subresult.substr (1);
          while (isspace (subresult.back()))
            subresult = subresult.substr (0, subresult.size()-1);
          result.push_back (std::move (subresult));
        }
      }
      return result;
    }



    /// \brief read the line beginning with a specific phrase (without
    /// the phrase itself)
    std::string readLine (const std::string& text,
                          const std::string& begin)
    {
      auto lines = readLineList (text, begin);
      if (lines.empty())
        RCU_THROW_MSG ("failed to find line starting with: " + begin);
      if (lines.size() > 1)
        RCU_THROW_MSG ("multiple lines starting with: " + begin);
      return lines.at(0);
    }



    /// \brief read the line beginning with a specific phrase (without
    /// the phrase itself)
    unsigned readLineUnsigned (const std::string& text,
                               const std::string& begin)
    {
      const auto line = readLine (text, begin);
      std::istringstream str (line);
      unsigned result = 0;
      if (!(str >> result) || !str.eof())
        RCU_THROW_MSG ("failed to convert " + line + " into an unsigned");
      return result;
    }



    /// \brief the command for setting up rucio
    std::string rucioSetupCommand ()
    {
      return "source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh && lsetup --force 'rucio -w'";
    }
  }



  bool checkVomsProxy ()
  {
    return proxyData().checkVomsProxy();
  }



  void ensureVomsProxy ()
  {
    proxyData().ensureVomsProxy();
  }



  std::vector<std::string>
  faxListFilesGlob (const std::string& name, const std::string& filter)
  {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    return faxListFilesRegex (name, RCU::glob_to_regexp (filter));
#pragma GCC diagnostic pop
  }



  std::vector<std::string>
  faxListFilesRegex (const std::string& name, const std::string& filter)
  {
    RCU_REQUIRE_SOFT (!name.empty());
    RCU_REQUIRE_SOFT (name.find('*') == std::string::npos);
    RCU_REQUIRE_SOFT (!filter.empty());

    ensureVomsProxy ();

    static const std::string separator = "------- SampleHandler Split -------";
    std::vector<std::string> result;

    ANA_MSG_INFO ("querying FAX for dataset " << name);
    std::string output = sh::exec_read ("source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh && lsetup --force fax && echo " + separator + " && fax-get-gLFNs " + sh::quote (name));
    auto split = output.rfind (separator + "\n");
    if (split == std::string::npos)
      RCU_THROW_MSG ("couldn't find separator in: " + output);

    std::istringstream str (output.substr (split + separator.size() + 1));
    boost::regex pattern (filter);
    std::string line;
    while (std::getline (str, line))
    {
      if (!line.empty())
      {
	if (line.find ("root:") != 0)
	  RCU_THROW_MSG ("couldn't parse line: " + line);

	std::string::size_type split1 = line.rfind (":");
	std::string::size_type split2 = line.rfind ("/");
	if (split1 < split2)
	  split1 = split2;
	if (split1 != std::string::npos)
	{
	  if (RCU::match_expr (pattern, line.substr (split1+1)))
	    result.push_back (line);
	} else
	  RCU_THROW_MSG ("couldn't parse line: " + line);
      }
    }
    if (result.size() == 0)
      RCU_WARN_MSG ("dataset " + name + " did not contain any files.  this is likely not right");
    return result;
  }



  std::vector<std::string>
  rucioDirectAccessGlob (const std::string& name, const std::string& filter)
  {
    return rucioDirectAccessRegex (name, RCU::glob_to_regexp (filter));
  }



  std::vector<std::string>
  rucioDirectAccessRegex (const std::string& name, const std::string& filter)
  {
    RCU_REQUIRE_SOFT (!name.empty());
    RCU_REQUIRE_SOFT (name.find('*') == std::string::npos);
    RCU_REQUIRE_SOFT (!filter.empty());

    ensureVomsProxy ();

    static const std::string separator = "------- SampleHandler Split -------";

    ANA_MSG_INFO ("querying rucio for dataset " << name);
    std::string output = sh::exec_read (rucioSetupCommand() + " && echo " + separator + " && rucio list-file-replicas --pfns --protocols root " + sh::quote (name));
    auto split = output.rfind (separator + "\n");
    if (split == std::string::npos)
      RCU_THROW_MSG ("couldn't find separator in: " + output);
    std::istringstream str (output.substr (split + separator.size() + 1));

    // this is used to avoid getting two copies of the same file.  we
    // first fill them in a map by filename, then copy them into a
    // vector
    std::map<std::string,std::string> resultMap;

    boost::regex pattern (filter);
    std::string line;
    while (std::getline (str, line))
    {
      if (!line.empty())
      {
	if (line.find ("root:") != 0)
	  RCU_THROW_MSG ("couldn't parse line: " + line);

	std::string::size_type split = line.rfind ("/");
	if (split != std::string::npos)
	{
          std::string filename = line.substr (split+1);
	  if (RCU::match_expr (pattern, filename))
	    resultMap[filename] = line;
	} else
	  RCU_THROW_MSG ("couldn't parse line: " + line);
      }
    }

    std::vector<std::string> result;
    for (const auto& file : resultMap)
      result.push_back (file.second);
    if (result.size() == 0)
      ANA_MSG_WARNING ("dataset " + name + " did not contain any files.  this is likely not right");
    return result;
  }



  std::vector<RucioListDidsEntry> rucioListDids (const std::string& dataset)
  {
    RCU_REQUIRE_SOFT (!dataset.empty());

    ensureVomsProxy ();

    static const std::string separator = "------- SampleHandler Split -------";
    std::vector<RucioListDidsEntry> result;

    ANA_MSG_INFO ("querying rucio for dataset " << dataset);
    std::string output = sh::exec_read (rucioSetupCommand() + " && echo " + separator + " && rucio list-dids " + sh::quote (dataset));
    auto split = output.rfind (separator + "\n");
    if (split == std::string::npos)
      RCU_THROW_MSG ("couldn't find separator in: " + output);

    std::istringstream str (output.substr (split + separator.size() + 1));
    boost::regex pattern ("^\\| ([a-zA-Z0-9_.]+):([a-zA-Z0-9_.]+) +\\| ([a-zA-Z0-9_.]+) +\\| *$");
    std::string line;
    while (std::getline (str, line))
    {
      boost::smatch what;
      if (boost::regex_match (line, what, pattern))
      {
	RucioListDidsEntry entry;
	entry.scope = what[1];
	entry.name = what[2];
	entry.type = what[3];
	result.push_back (entry);
      }
    }
    return result;
  }



  std::vector<RucioListFileReplicasEntry>
  rucioListFileReplicas (const std::string& dataset)
  {
    RCU_REQUIRE_SOFT (!dataset.empty());

    ensureVomsProxy ();

    static const std::string separator = "------- SampleHandler Split -------";
    std::vector<RucioListFileReplicasEntry> result;

    std::string command = rucioSetupCommand() + " && echo " + separator + " && rucio list-file-replicas " + sh::quote (dataset);

    ANA_MSG_INFO ("querying rucio for dataset " << dataset);
    std::string output = sh::exec_read (rucioSetupCommand() + " && echo " + separator + " && rucio list-file-replicas " + sh::quote (dataset));
    auto split = output.rfind (separator + "\n");
    if (split == std::string::npos)
      RCU_THROW_MSG ("couldn't find separator in: " + output);

    std::istringstream str (output.substr (split + separator.size() + 1));
    boost::regex pattern ("^\\| +([^ ]+) +\\| +([^ ]+) +\\| +([^ ]+ [^ ]+) +\\| +([^ ]+) +\\| +([^: ]+): ([^ ]+) +\\| *$");
    std::string line;
    while (std::getline (str, line))
    {
      boost::smatch what;
      if (boost::regex_match (line, what, pattern) &&
          what[1] != "SCOPE")
      {
	RucioListFileReplicasEntry entry;
	entry.scope    = what[1];
	entry.name     = what[2];
	entry.filesize = what[3];
	entry.adler32  = what[4];
	entry.disk     = what[5];
	entry.replica  = what[6];
	result.push_back (entry);
      }
    }
    return result;
  }



  std::map<std::string,std::unique_ptr<MetaObject> >
  rucioGetMetadata (const std::set<std::string>& datasets)
  {
    RCU_REQUIRE_SOFT (!datasets.empty());

    ensureVomsProxy ();

    static const std::string separator = "------- SampleHandler Split -------";
    std::map<std::string,std::unique_ptr<MetaObject> > result;

    std::string command = rucioSetupCommand() + " && echo " + separator + " && rucio get-metadata";
    for (auto& dataset : datasets)
    {
      RCU_REQUIRE_SOFT (!dataset.empty());
      command += " " + sh::quote (dataset);
    }

    ANA_MSG_INFO ("querying rucio for meta-data");
    std::string output = sh::exec_read (command);
    auto split = output.rfind (separator + "\n");
    if (split == std::string::npos)
      RCU_THROW_MSG ("couldn't find separator in: " + output);

    std::istringstream str (output.substr (split + separator.size() + 1));
    boost::regex pattern ("^([^:]+): (.+)$");
    std::string line;
    std::unique_ptr<MetaObject> meta (new MetaObject);

    auto addMeta = [&] ()
    {
      std::string name = meta->castString ("scope") + ":" + meta->castString ("name");
      if (result.find (name) != result.end())
	RCU_THROW_MSG ("read " + name + " twice");
      result[name] = std::move (meta);
    };

    while (std::getline (str, line))
    {
      boost::smatch what;
      if (boost::regex_match (line, what, pattern))
      {
	if (meta->get (what[1]))
	{
	  addMeta ();
	  meta.reset (new MetaObject);
	}
	meta->setString (what[1], what[2]);
      } else if (!line.empty())
      {
	ANA_MSG_WARNING ("couldn't parse line: " << line);
      }
    }
    addMeta ();

    for (auto& dataset : datasets)
    {
      if (result.find (dataset) == result.end())
	RCU_THROW_MSG ("received result for dataset not requested: " + dataset);
    }
    for (auto& subresult : result)
    {
      if (datasets.find (subresult.first) == datasets.end())
	RCU_THROW_MSG ("received result for dataset not requested: " + subresult.first);
    }

    return result;
  }



  RucioDownloadResult rucioDownload (const std::string& location,
                                     const std::string& dataset)
  {
    ensureVomsProxy ();
    
    const std::string separator = "------- SampleHandler Split -------";
    std::string command = rucioSetupCommand() + " && echo " + separator + " && cd " + sh::quote (location) + " && rucio download " + sh::quote (dataset) + " 2>&1";

    ANA_MSG_INFO ("starting rucio download " + dataset + " into " + location);
    std::string output = sh::exec_read (command);
    auto split = output.rfind (separator + "\n");
    if (split == std::string::npos)
      RCU_THROW_MSG ("couldn't find separator in: " + output);
    output = output.substr (split + separator.size() + 1);

    RucioDownloadResult result;
    result.did = readLine (output, "DID ");
    result.totalFiles = readLineUnsigned (output, "Total files : ");
    result.downloadedFiles = readLineUnsigned (output, "Downloaded files : ");
    result.alreadyLocal = readLineUnsigned (output, "Files already found locally : ");
    result.notDownloaded = readLineUnsigned (output, "Files that cannot be downloaded : ");
    return result;
  }



  std::vector<RucioDownloadResult>
  rucioDownloadList (const std::string& location,
                     const std::vector<std::string>& datasets)
  {
    std::vector<RucioDownloadResult> result;
    for (auto& dataset : datasets)
      result.push_back (rucioDownload (location, dataset));
    return result;
  }
}
