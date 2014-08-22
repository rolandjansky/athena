/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class RunInput

  An object to load histograms from a root file

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#include "RunInput.h"
#include "TFile.h"
#include "TKey.h"
#include <iostream>
#include <stdlib.h>
#include "VectorOperations.h"
#include "TClass.h"

using namespace std;

//Default constructor
RunInput::RunInput()
{
}

//Constructor to load a file
RunInput::RunInput( string FileName, ConfigurationLoader * InputConfiguration )
{
	//Populate the lists of histogram identifiers to search for
	LoadConfiguration(InputConfiguration);

	//Open the file read-only
	inputFile = new TFile( FileName.c_str(), "READ" );
	if ( inputFile->IsOpen() )
	{
		//Explore the file structure and retrieve histograms
		ExploreDirectory( inputFile, vector<string>(), 0 );
	}
	else
	{
		cerr << "FATAL: Failed to open file " << FileName << endl;
		exit(1);
	}
}

//Populate the search strings from a file
void RunInput::LoadConfiguration( ConfigurationLoader * InputConfiguration )
{
	//Get the full search information
	searchStrings = InputConfiguration->GetLoadingSearchStrings();

	//Now make the quick search strings - lists of unique identifiers at each level
	for ( int pathIndex = 0; pathIndex < InputConfiguration->GetMaximumPathDepth(); pathIndex++ )
	{
		//Pre-populate the quick search
		vector<string> emptyVector;
		quickSearchStrings.push_back(emptyVector);
	}
	for ( int searchIndex = 0; searchIndex < searchStrings.size(); searchIndex++ )
	{
		//Look for unique search strings along the length of this vector
		for ( int pathIndex = 0; pathIndex < searchStrings[searchIndex].size(); pathIndex++ )
		{
			//* will match with anything, so forget about the other strings at this level
			if ( searchStrings[searchIndex][pathIndex] == "*" )
			{
				quickSearchStrings[pathIndex].clear();
				quickSearchStrings[pathIndex].push_back("*");
			}
			else
			{
				//Remove any qualifiers
				string includeSearchString = ( VectorOperations::SplitString( searchStrings[searchIndex][pathIndex], ',' ) )[0];
				int uniqueIndex = VectorOperations::VectorContains( &quickSearchStrings[pathIndex], &includeSearchString );
				if ( uniqueIndex == -1 )
				{
					//New unique string
					quickSearchStrings[pathIndex].push_back(includeSearchString);
				}
			}
		}
	}
}

//Check for a compatible entry at a given directory level
bool RunInput::QuickSearchCheck( string TestString, int PathDepth )
{
	//Check any search strings go this deep
	if ( PathDepth < quickSearchStrings.size() )
	{
		//Check for a match with any unique search string at this depth
		for ( int searchIndex = 0; searchIndex < quickSearchStrings[PathDepth].size(); searchIndex++ )
		{
			//Match if searching for *, or if the search string is contained in the test string
			string quickString = quickSearchStrings[PathDepth][searchIndex];
			if ( quickString == "*" || strstr( TestString.c_str(), quickString.c_str() ) != 0 )
			{
				return true;
			}
		}

		//No matches
		return false;
	}
	else
	{
		return false;
	}
}

//Check for a fully compatible search string
bool RunInput::FullSearchCheck( vector<string> & TestVector )
{
	//Check for a match with any full search vector
	for ( int searchIndex = 0; searchIndex < searchStrings.size(); searchIndex++ )
	{
		if ( VectorOperations::FullSearch( TestVector, searchStrings[searchIndex] ) )
		{
			//If it matches, you're done
			return true;
		}
	}

	//No matches
	return false;
}

//Explore the directory structure
void RunInput::ExploreDirectory( TDirectoryFile * InputDirectory, vector<string> PathToHere, int PathDepth )
{
	//Loop over all objects in the directory
	TKey * testKey;
	TIter directoryKeys( InputDirectory->GetListOfKeys() );
	while ( testKey = ( TKey* )directoryKeys() )
	{
		//Check if the object type is interesting
		string keyClassName = testKey->GetClassName();
		TObject * keyObject = testKey->ReadObj();
		if ( keyClassName == "TDirectoryFile" || keyObject->IsA()->InheritsFrom("TH1") )
		{
			//Check if the object is one we're looking for
			string keyName = testKey->GetName();
			for ( int searchIndex = 0; searchIndex < searchStrings.size(); searchIndex++ )
			{
				//Accept the object if the search string is * or if the search string is found in the object name
				//This is simply an optimisation, to prevent exploration of the whole tree
				if ( QuickSearchCheck( keyName, PathDepth ) )
				{
					//Add the object name to the path
					vector<string> objectPath = PathToHere;
					objectPath.push_back(keyName);

					//Explore the subdirectory
					if ( keyClassName == "TDirectoryFile" )
					{
						//Retrieve the subdirectory object
						TDirectoryFile * subDirectory = ( TDirectoryFile* )keyObject;

						//Explore the subdirectory
						ExploreDirectory( subDirectory, objectPath, PathDepth + 1 );
					}
					else if ( keyObject->IsA()->InheritsFrom("TH1") && FullSearchCheck(objectPath) )
					{
						//Store the histogram
						storedHistograms.push_back( ( TH1* )keyObject );
						storedPaths.push_back(objectPath);
						storedTypes.push_back(keyClassName);
					}

					//Stop looking for search matches
					break;
				}
			}
		}
	}
}

//Constructor for a merged file
RunInput::RunInput( vector< vector<string> > InputPaths, vector< TH1* > InputHistograms, vector<string> InputTypes ) : storedHistograms(InputHistograms),
	storedTypes(InputTypes), storedPaths(InputPaths)
{
}

//Destructor
RunInput::~RunInput()
{
	//Close the file
	inputFile->Close();
}

//Get the stored paths
vector< vector<string> > RunInput::GetHistogramPaths()
{
	return storedPaths;
}

//Get the stored histograms
vector< TH1* > RunInput::GetHistograms()
{
	return storedHistograms;
}

//Get the stored histogram types
vector<string> RunInput::GetHistogramTypes()
{
	return storedTypes;
}
