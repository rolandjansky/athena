/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class RunInput

  An object to load histograms from a root file

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#ifndef RUN_INPUT_H
#define RUN_INPUT_H

#include <string>
#include <vector>
#include "TH1.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "ConfigurationLoader.h"

using namespace std;

class RunInput
{
	public:
		RunInput();
		RunInput( string, ConfigurationLoader* );
		RunInput( vector< vector<string> >, vector< TH1* >, vector<string> );
		~RunInput();

		vector< vector<string> > GetHistogramPaths();
		vector< TH1* > GetHistograms();
		vector<string> GetHistogramTypes();

	private:
		void ExploreDirectory( TDirectoryFile*, vector<string>, int );
		void LoadConfiguration( ConfigurationLoader* );
		bool QuickSearchCheck( string, int );
		bool FullSearchCheck( vector<string>& );

		vector< TH1* > storedHistograms;
		vector< vector<string> > storedPaths, searchStrings, quickSearchStrings;
		vector<string> storedTypes;
		TFile * inputFile;
};

#endif
