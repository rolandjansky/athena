/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
  report any issues on JIRA: https://its.cern.ch/jira/browse/TAUAT/?selectedTab=com.atlassian.jira.jira-projects-plugin:issues-panel
*/

// stl include(s)
#include <vector>
#include <string>
#include <sstream>

// ROOT include(s)
#include "TEnv.h"

namespace TauAnalysisTools
{
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<std::string>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<size_t>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<int>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<float>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<double>& vOut);
}

#endif // not CONFIGHELPER_H
