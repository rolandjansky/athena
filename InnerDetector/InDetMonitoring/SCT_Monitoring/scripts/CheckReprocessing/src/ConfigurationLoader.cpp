/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class ConfigurationLoader

  An object to load and parse the configuration file

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#include "ConfigurationLoader.h"
#include <iostream>
#include <fstream>
#include "VectorOperations.h"
#include <stdlib.h>

//Default constructor
ConfigurationLoader::ConfigurationLoader() : maximumPathDepth(-1)
{
}

//Constuctor with file name as argument
ConfigurationLoader::ConfigurationLoader( string InputFileName ) : maximumPathDepth(0)
{
	vector<string> inputVector, inputComparisons, inputArguments;
	string inputString;
	char inputLine[1000];

	//Open the file
	ifstream * searchStringFile = new ifstream( InputFileName.c_str() );
	if ( searchStringFile->is_open() )
	{
		//Read in the whole file
		while ( searchStringFile->good() )
		{
			//Read a line, ignore empty lines
			searchStringFile->getline( inputLine, 1000 );
			inputString = inputLine;

			//Split off comments
			inputVector = VectorOperations::SplitString( inputString, '#', true );
			if ( inputVector.size() > 0 )
			{
				inputString = inputVector[0];
			}

			//Ignore empty lines
			if ( inputString != "" )
			{
				//Split the input at whitespace
				inputVector = VectorOperations::SplitString( inputString, ' ' );
				if ( inputVector.size() > 2 )
				{
					//First column is the comparsion name(s), split by comma. Ditto second, but is arguments
					inputComparisons = VectorOperations::SplitString( inputVector[0], ',' );
					inputArguments = VectorOperations::SplitString( inputVector[1], ',' );
					inputVector.erase( inputVector.begin(), inputVector.begin() + 2 );
					comparisonNames.push_back(inputComparisons);
					comparisonArguments.push_back(inputArguments);

					//Other columns are the search strings
					loadingSearchStrings.push_back(inputVector);
					if ( inputVector.size() > maximumPathDepth )
					{
						maximumPathDepth = inputVector.size();
					}
				}
				else
				{
					cerr << "FATAL: Badly formed config file. Error here:" << endl;
					cerr << inputString << endl;
					exit(1);
				}
			}
		}

		//Initialise the match counter
		numberOfMatches = vector<int>( loadingSearchStrings.size(), 0 );
	}
	else
	{
		cerr << "FATAL: Unable to open configuration file " << InputFileName << endl;
		exit(1);
	}
}

//Destructor
ConfigurationLoader::~ConfigurationLoader()
{
}

//Return the srtings to search for when loading objects
vector< vector<string> > ConfigurationLoader::GetLoadingSearchStrings()
{
	return loadingSearchStrings;
}

//Return the comparison names associated with a path
vector<string> ConfigurationLoader::GetComparisonNamesForPath( vector<string> InputPath, vector<string> & OutputArguments )
{
	vector<string> matchingComparisons, matchingArguments;

	//Loop over all search strings to find the ones that match
	for ( int searchIndex = 0; searchIndex < loadingSearchStrings.size(); searchIndex++ )
	{
		if ( VectorOperations::FullSearch( InputPath, loadingSearchStrings[searchIndex] ) )
		{
			//Add comparison names for search strings that match
			matchingComparisons.insert( matchingComparisons.end(), comparisonNames[searchIndex].begin(), comparisonNames[searchIndex].end() );
			matchingArguments.insert( matchingArguments.end(), comparisonArguments[searchIndex].begin(), comparisonArguments[searchIndex].end() );

			//Record that a match was found
			numberOfMatches[searchIndex]++;
		}
	}

	OutputArguments = matchingArguments;
	return matchingComparisons;
}

//Return the maximum path depth
int ConfigurationLoader::GetMaximumPathDepth()
{
	return maximumPathDepth;
}

//Output how many histograms matched against each search string
void ConfigurationLoader::OutputComparisonStatistics()
{
	for ( int searchIndex = 0; searchIndex < loadingSearchStrings.size(); searchIndex++ )
	{
		if ( numberOfMatches[searchIndex] == 0 )
		{
			cerr << "WARNING: ";
			VectorOperations::PathCERR( loadingSearchStrings[searchIndex] );
			cerr << " did not match any histograms" << endl;
		}
		else
		{
			cout << "INFO: ";
			VectorOperations::PathCOUT( loadingSearchStrings[searchIndex] );
			cout << " matched " << numberOfMatches[searchIndex] << " histogram";

			if ( numberOfMatches[searchIndex] > 1 )
			{
				cout << "s";
			}

			cout << endl;
		}
	}
}
