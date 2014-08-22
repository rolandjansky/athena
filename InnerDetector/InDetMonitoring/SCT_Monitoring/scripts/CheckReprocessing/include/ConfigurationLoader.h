/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class ConfigurationLoader

  An object to load and parse the configuration file

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#ifndef CONFIGURATION_LOADER_H
#define CONFIGURATION_LOADER_H

#include <vector>
#include <string>

using namespace std;

class ConfigurationLoader
{
	public:
		ConfigurationLoader();
		ConfigurationLoader(string);
		~ConfigurationLoader();

		vector< vector<string> > GetLoadingSearchStrings();
		vector<string> GetComparisonNamesForPath( vector<string>, vector<string>& );
		int GetMaximumPathDepth();
		void OutputComparisonStatistics();

	private:
		vector< vector<string> > loadingSearchStrings, comparisonNames, comparisonArguments;
		vector< int > numberOfMatches;
		int maximumPathDepth;
};

#endif
