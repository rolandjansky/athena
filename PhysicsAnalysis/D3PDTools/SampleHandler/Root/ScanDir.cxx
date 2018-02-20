/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/ScanDir.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/StringUtil.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskListEOS.h>
#include <SampleHandler/DiskListLocal.h>
#include <SampleHandler/MessageCheck.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SamplePtr.h>
#include <TString.h>
#include <memory>

//
// method implementations
//

namespace SH
{
  ScanDir :: 
  ScanDir ()
    : m_relSampleDepth (-2), m_absSampleDepth (0),
      m_minDepth (0), m_maxDepth (-1),
      m_filePattern (RCU::glob_to_regexp ("*.root*")),
      m_samplePattern (RCU::glob_to_regexp ("*")),
      m_samplePostfix (RCU::glob_to_regexp ("")),
      m_samplePostfixEmpty (true),
      m_extraNameComponent (0)
  {}



  ScanDir& ScanDir ::
  sampleDepth (int val_sampleDepth)
  {
    m_relSampleDepth = val_sampleDepth;
    m_absSampleDepth = val_sampleDepth;
    return *this;
  }



  ScanDir& ScanDir ::
  absSampleDepth (int val_absSampleDepth)
  {
    m_relSampleDepth = 0;
    m_absSampleDepth = val_absSampleDepth;
    return *this;
  }



  ScanDir& ScanDir ::
  sampleName (const std::string& val_sampleName)
  {
    m_sampleName = val_sampleName;
    return *this;
  }



  ScanDir& ScanDir ::
  minDepth (std::size_t val_minDepth)
  {
    m_minDepth = val_minDepth;
    return *this;
  }



  ScanDir& ScanDir ::
  maxDepth (std::size_t val_maxDepth)
  {
    m_maxDepth = val_maxDepth;
    return *this;
  }



  ScanDir& ScanDir ::
  filePattern (const std::string& val_filePattern)
  {
    m_filePattern = RCU::glob_to_regexp (val_filePattern);
    return *this;
  }



  ScanDir& ScanDir ::
  fileRegex (const std::string& val_fileRegex)
  {
    m_filePattern = val_fileRegex;
    return *this;
  }



  ScanDir& ScanDir ::
  samplePattern (const std::string& val_samplePattern)
  {
    m_samplePattern = RCU::glob_to_regexp (val_samplePattern);
    return *this;
  }



  ScanDir& ScanDir ::
  samplePostfix (const std::string& val_samplePostfix)
  {
    m_samplePostfix = RCU::glob_to_regexp (val_samplePostfix);
    m_samplePostfixEmpty = val_samplePostfix.empty();
    return *this;
  }



  ScanDir& ScanDir ::
  sampleRename (const std::string& pattern, const std::string& name)
  {
    m_sampleRename.push_back (std::pair<boost::regex,std::string> (boost::regex (RCU::glob_to_regexp (pattern)), name));
    return *this;
  }



  ScanDir& ScanDir ::
  extraNameComponent (int val_relSampleDepth)
  {
    RCU_REQUIRE (val_relSampleDepth != 0);
    m_extraNameComponent = val_relSampleDepth;
    return *this;
  }



  const ScanDir& ScanDir ::
  scan (SampleHandler& sh, const std::string& dir) const
  {
    DiskListLocal list (dir);
    scan (sh, list);
    return *this;
  }



  const ScanDir& ScanDir ::
  scanEOS (SampleHandler& sh, const std::string& eosDir) const
  {
    DiskListEOS list (eosDir);
    scan (sh, list);
    return *this;
  }



  const ScanDir& ScanDir ::
  scan (SampleHandler& sh, DiskList& list) const
  {
    std::vector<std::string> hierarchy;
    hierarchy.push_back (list.dirname());

    std::map<std::string,SamplePtr> samples;
    typedef std::map<std::string,SamplePtr>::const_iterator samplesIter;
    recurse (samples, list, hierarchy);
    for (samplesIter sample = samples.begin(), end = samples.end();
	 sample != end; ++ sample)
    {
      if (sample->second.get() != 0)
      {
	sh.add (sample->second);
      }
    }
    return *this;
  }



  void ScanDir ::
  recurse (std::map<std::string,SamplePtr>& samples, DiskList& list,
	   const std::vector<std::string>& hierarchy) const
  {
    using namespace msgScanDir;

    ANA_MSG_DEBUG ("scanning directory: " << list.dirname());
    while (list.next())
    {
      std::auto_ptr<DiskList> sublist (list.openDir());

      if (sublist.get() != 0)
      {
	if (hierarchy.size() <= m_maxDepth)
	{
          ANA_MSG_DEBUG ("descending into directory " << list.path());
	  std::vector<std::string> subhierarchy = hierarchy;
	  subhierarchy.push_back (list.fileName());
	  recurse (samples, *sublist, subhierarchy);
	} else
        {
          ANA_MSG_DEBUG ("maxDepth exceeded, skipping directory " << list.path());
        }
      } else
      {
	if (hierarchy.size() > m_minDepth &&
	    RCU::match_expr (m_filePattern, list.fileName()))
	{
          ANA_MSG_DEBUG ("adding file " << list.path());
	  std::vector<std::string> subhierarchy = hierarchy;
	  subhierarchy.push_back (list.fileName());
	  addSampleFile (samples, subhierarchy, list.path());
	} else
        {
          ANA_MSG_DEBUG ("skipping file " << list.path());
        }
      }
    }
  }



  void ScanDir ::
  addSampleFile (std::map<std::string,SamplePtr>& samples,
		 const std::vector<std::string>& hierarchy,
		 const std::string& path) const
  {
    std::string sampleName;

    if (!m_sampleName.empty())
    {
      sampleName = m_sampleName;
    } else
    {
      sampleName = findPathComponent (hierarchy, m_absSampleDepth,
				      m_relSampleDepth);
      if (sampleName.empty())
	return;

      if (!m_samplePostfixEmpty)
      {
	bool done = false;
	for (std::size_t iter = 0, end = sampleName.size();
	     iter != end && !done; ++ iter)
	{
	  if (RCU::match_expr (m_samplePostfix, sampleName.substr (iter)))
	  {
	    if (iter == 0)
	      RCU_THROW_MSG ("sample name matches entire postfix pattern: \"" + sampleName + "\"");
	    sampleName = sampleName.substr (0, iter);
	    done = true;
	  }
	}
      }

      if (m_extraNameComponent != 0)
      {
	std::string component = findPathComponent
	  (hierarchy, m_extraNameComponent, m_extraNameComponent);
	if (component.empty())
	  return;
	sampleName += "_" + component;
      }

      if (!RCU::match_expr (m_samplePattern, sampleName))
	return;

      {
	bool done = false;
	for (SampleRenameIter iter = m_sampleRename.begin(),
	       end = m_sampleRename.end(); !done && iter != end; ++ iter)
	{
	  if (RCU::match_expr (iter->first, sampleName))
	  {
	    sampleName = iter->second;
	    done = true;
	  }
	}
      }
    }

    std::map<std::string,SamplePtr>::iterator iter
      = samples.find (sampleName);
    if (iter == samples.end())
    {
      SamplePtr sample (new SampleLocal (sampleName));
      samples[sampleName] = sample;
      iter = samples.find (sampleName);
    }
    SampleLocal *sample = dynamic_cast<SampleLocal*>(iter->second.get());
    RCU_ASSERT (sample != 0);
    sample->add (path);
  }



  std::string ScanDir ::
  findPathComponent (const std::vector<std::string>& hierarchy,
		     int absSampleDepth,
		     int relSampleDepth) const
  {
    std::string sampleName;

    int myindex = absSampleDepth+1;
    if (relSampleDepth < 0)
      myindex = relSampleDepth + hierarchy.size();
    if (std::size_t (myindex) >= hierarchy.size())
      return sampleName;
    if (myindex > 0)
    {
      sampleName = hierarchy[myindex];
    } else
    {
      sampleName = hierarchy[0];
      while (sampleName.empty() ||
	     sampleName[sampleName.size()-1] == '/' ||
	     myindex < 0)
      {
	while (!sampleName.empty() && sampleName[sampleName.size()-1] == '/')
	  sampleName = sampleName.substr (0, sampleName.size() - 1);
	if (sampleName.empty())
	  return sampleName;
	if (myindex < 0)
	{
	  std::string::size_type split = sampleName.rfind ('/');
	  if (split == std::string::npos)
	  {
	    sampleName.clear ();
	    return sampleName;
	  }
	  sampleName = sampleName.substr (0, split);
	  ++ myindex;
	}
	if (sampleName.empty())
	  return sampleName;
      }
      std::string::size_type split = sampleName.rfind ('/');
      if (split != std::string::npos)
	sampleName = sampleName.substr (split + 1);
    }
    return sampleName;
  }
}
