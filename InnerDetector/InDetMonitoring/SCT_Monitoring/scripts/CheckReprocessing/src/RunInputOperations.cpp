/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class RunInputOperations

  A collection of static methods that act on the RunInput objects
  The main file merge and comparison methods are here, along with the method for pairing up histograms

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#include "RunInputOperations.h"
#include "HistogramOperations.h"
#include "VectorOperations.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

//Take two identical inputs and merge
RunInput * RunInputOperations::Merge( RunInput * FirstInput, RunInput * SecondInput )
{
	//Pair up the histograms
	vector< vector<string> > paths;
	vector< pair< TH1*, TH1* > > histogramPairs;
	vector<string> types;
	CompareStructures( FirstInput, SecondInput, paths, histogramPairs, types );

	//Merge the histograms
	vector< TH1* > histograms;
	for ( int histogramIndex = 0; histogramIndex < histogramPairs.size(); histogramIndex++ )
	{
		TH1 * firstHistogram = histogramPairs[histogramIndex].first;
		TH1 * secondHistogram = histogramPairs[histogramIndex].second;
		histograms.push_back( HistogramOperations::MergeHistograms( firstHistogram, secondHistogram ) );
	}

	return new RunInput( paths, histograms, types );
}

//Compare the histograms for two inputs with different versions
void RunInputOperations::Compare( RunInput * NewInput, RunInput * OldInput, ConfigurationLoader * InputConfiguration )
{
	//Store the total result of the comparisons
	double result = 0.0;

	//Pair up the histograms
	vector< vector<string> > paths;
	vector< pair< TH1*, TH1* > > histogramPairs;
	vector<string> types;
	CompareStructures( NewInput, OldInput, paths, histogramPairs, types );

	//Compare histograms
	vector<string> comparisonNames, comparisonArguments;
	for ( int histogramIndex = 0; histogramIndex < histogramPairs.size(); histogramIndex++ )
	{
		//Store the result of the comparisons, where 0.0 means identical
		double comparisonValue = 0.0;

		//Retrieve histogram pair
		TH1 * firstHistogram = histogramPairs[histogramIndex].first;
		TH1 * secondHistogram = histogramPairs[histogramIndex].second;

		//Find out which comparisons to use for this histogram pair
		comparisonNames = InputConfiguration->GetComparisonNamesForPath( paths[histogramIndex], comparisonArguments );

		//Loop over the comparison names for this histogram
		for ( int comparisonIndex = 0; comparisonIndex < comparisonNames.size(); comparisonIndex++ )
		{
			//Retrieve comparison name and argument
			string comparison = comparisonNames[comparisonIndex];
			string argument = comparisonArguments[comparisonIndex];

			//////////////////////////////////////
			//                                  //
			//  This is where to add new tests  //
			//                                  //
			//////////////////////////////////////
			//Check the requested comparison against recognised names
			if ( comparison == "binwise" )
			  {
			    //Compare the contents of each bin by percent
			    comparisonValue += HistogramOperations::BinwiseCompare( firstHistogram, secondHistogram, argument, true );
			  }
			else if ( comparison == "binwiseVal" )
			  {
			    //Compare the contents of each bin by value
			    comparisonValue += HistogramOperations::BinwiseCompare( firstHistogram, secondHistogram, argument, false );
			  }
			else if ( comparison == "binwiseHack" )
			  {
			    //Compare the contents of each bin, with a nasty hack to allow different percentage weights per bin
			    comparisonValue += HistogramOperations::BinwiseHackCompare( firstHistogram, secondHistogram, argument );
			  }
			else if ( comparison == "kolmogorov" )
			  {
			    //Perform a Kolmogorov-Smirnov test
			    comparisonValue += HistogramOperations::KolmogorovTest( firstHistogram, secondHistogram, argument );
			  }
			else if ( comparison == "mean" )
			  {
			    //Compare histogram means by percent
			    comparisonValue += HistogramOperations::CompareMean( firstHistogram, secondHistogram, argument, true );
			  }
			else if ( comparison == "meanVal" )
			  {
			    //Compare histogram means by value
			    comparisonValue += HistogramOperations::CompareMean( firstHistogram, secondHistogram, argument, false );
			  }
			else if ( comparison == "sigma" )
			  {
			    //Compare histogram sigmas by percent
			    comparisonValue += HistogramOperations::CompareSigma( firstHistogram, secondHistogram, argument, true );
			  }
			else if ( comparison == "sigmaVal" )
			  {
			    //Compare histogram sigmas by value
			    comparisonValue += HistogramOperations::CompareSigma( firstHistogram, secondHistogram, argument, false );
			  }
			else if ( comparison == "entries" )
			  {
			    //Compare number of histogram entries
			    comparisonValue += HistogramOperations::CompareTotalEntries( firstHistogram, secondHistogram, argument );
			  }
			else if ( comparison == "dummy" )
			  {
			    //Do nothing
			  }
			else
			  {
			    cerr << "Unrecognised comparison name: " << comparison << endl;
			    exit(1);
			  }
		}
		
		//Save and superimpose the histograms if they aren't identical
		if ( comparisonValue != 0.0 )
		{
			HistogramOperations::FileOutput( firstHistogram, secondHistogram, "mismatch" );
			result += comparisonValue;
		}
	}

	//Output the result
	InputConfiguration->OutputComparisonStatistics();
	if ( result != 0.0 )
	{
		//Should output the run number
		cerr << "BAD RUN: Histograms do not match in " << paths[0][0] << endl;
	}
}

//Pair up histograms from two different inputs, return the number of resolved imperfect matches
void RunInputOperations::CompareStructures( RunInput * FirstInput, RunInput * SecondInput, vector< vector<string> > & Paths, vector< pair< TH1*, TH1* > > & HistogramPairs, vector<string> & Types )
{
	//Retrieve paths
	vector< vector<string> > firstPaths = FirstInput->GetHistogramPaths();
	vector< vector<string> > secondPaths = SecondInput->GetHistogramPaths();

	//Retrieve histograms
	vector< TH1* > firstHistograms = FirstInput->GetHistograms();
	vector< TH1* > secondHistograms = SecondInput->GetHistograms();

	//Retrieve types
	vector<string> firstTypes = FirstInput->GetHistogramTypes();
	vector<string> secondTypes = SecondInput->GetHistogramTypes();

	//Make a list of all the unique histogram types
	vector<string> allTypes;

	//Split histograms from first input by type
	vector< vector< vector<string> > > firstComparePaths, secondComparePaths;
	vector< vector< TH1* > > firstCompareHistograms, secondCompareHistograms;
	for ( int firstIndex = 0; firstIndex < firstTypes.size(); firstIndex++ )
	{
		int typeIndex = VectorOperations::VectorContains( &allTypes, &firstTypes[firstIndex] );
		if ( typeIndex == -1 )
		{
			//New type - store it
			allTypes.push_back( firstTypes[firstIndex] );

			//Store the path - pad the list for second input too
			vector< vector<string> > newPathList;
			secondComparePaths.push_back( newPathList );
			newPathList.push_back( firstPaths[firstIndex] );
			firstComparePaths.push_back( newPathList );

			//Store the histogram - pad the list for second input too
			vector< TH1* > newHistogramList;
			secondCompareHistograms.push_back( newHistogramList );
			newHistogramList.push_back( firstHistograms[firstIndex] );
			firstCompareHistograms.push_back( newHistogramList );
		}
		else
		{
			//Existing type - store path and histogram appropriately
			firstComparePaths[typeIndex].push_back( firstPaths[firstIndex] );
			firstCompareHistograms[typeIndex].push_back( firstHistograms[firstIndex] );
		}
	}

	//Split histograms from second input by type
	for ( int secondIndex = 0; secondIndex < secondTypes.size(); secondIndex++ )
	{
		int typeIndex = VectorOperations::VectorContains( &allTypes, &secondTypes[secondIndex] );
		if ( typeIndex == -1 )
		{
			//New type - store it
			allTypes.push_back( secondTypes[secondIndex] );

			//Store the path - pad the list for first input too
			vector< vector<string> > newPathList;
			firstComparePaths.push_back( newPathList );
			newPathList.push_back( secondPaths[secondIndex] );
			secondComparePaths.push_back( newPathList );

			//Store the histogram - pad the list for first input too
			vector< TH1* > newHistogramList;
			firstCompareHistograms.push_back( newHistogramList );
			newHistogramList.push_back( secondHistograms[secondIndex] );
			secondCompareHistograms.push_back( newHistogramList );
		}
		else
		{
			//Existing type - store path and histogram appropriately
			secondComparePaths[typeIndex].push_back( secondPaths[secondIndex] );
			secondCompareHistograms[typeIndex].push_back( secondHistograms[secondIndex] );
		}       
	}

	//Pair up the histograms by type
	vector< pair< int, int > > pathPairs;
	for ( int typeIndex = 0; typeIndex < allTypes.size(); typeIndex++ )
	{
		pathPairs = CalculatePathPairs( firstComparePaths[typeIndex], secondComparePaths[typeIndex] );
		for ( int pairIndex = 0; pairIndex < pathPairs.size(); pairIndex++ )
		{
			int firstIndex = pathPairs[pairIndex].first;
			int secondIndex = pathPairs[pairIndex].second;

			//Store the histogram path
			Paths.push_back( firstComparePaths[typeIndex][firstIndex] );

			//Store the merged histogram pair
			TH1 * firstHistogram = firstCompareHistograms[typeIndex][firstIndex];
			TH1 * secondHistogram = secondCompareHistograms[typeIndex][secondIndex];
			HistogramPairs.push_back( make_pair( firstHistogram, secondHistogram ) );

			//Store the histogram type
			Types.push_back( allTypes[typeIndex] );
		}
	}
}

//Make a list of the histogram indices to pair up
vector< pair< int, int > > RunInputOperations::CalculatePathPairs( vector< vector<string> > PathSet1, vector< vector<string> > PathSet2 )
{
	vector< pair< int, int > > pathPairs;
	vector< vector<int> > allPathScores;
	vector<int> pathScores;
	vector<bool> firstHasMatch, secondHasMatch;
	int matchIndex;
	bool wasSwapped = false;

	//Make sure the first path list is the shortest
	if ( PathSet2.size() < PathSet1.size() )
	{
		vector< vector<string> > temporaryPathSet = PathSet1;
		PathSet1 = PathSet2;
		PathSet2 = temporaryPathSet;
		wasSwapped = true;
	}

	//Initialise match checking
	for ( int firstIndex = 0; firstIndex < PathSet1.size(); firstIndex++ )
	{
		firstHasMatch.push_back(false);
	}
	for ( int secondIndex = 0; secondIndex < PathSet2.size(); secondIndex++ )
	{
		secondHasMatch.push_back(false);
	}

	//Loop over the first path list - look for perfect matches
	int maximumScore = 0;
	for ( int firstIndex = 0; firstIndex < PathSet1.size(); firstIndex++ )
	{
		bool perfectMatch = false;

		//Compare the first path with each second path
		for ( int secondIndex = 0; secondIndex < PathSet2.size(); secondIndex++ )
		{
			if ( !secondHasMatch[secondIndex] )
			{
				int pathScore = VectorOperations::ComparePaths( PathSet1[firstIndex], PathSet2[secondIndex] );
				pathScores.push_back(pathScore);

				//If there's a perfect match, stop looking
				if ( pathScore == 0 )
				{
					perfectMatch = true;
					matchIndex = secondIndex;
					break;
				}

				//Find the biggest mismatch
				if ( pathScore > maximumScore )
				{
					maximumScore = pathScore;
				}
			}
			else
			{
				pathScores.push_back(1000);
			}
		}

		//Store the match or leave for best guessing
		if (perfectMatch)
		{
			//Store the match information
			firstHasMatch[firstIndex] = true;
			secondHasMatch[matchIndex] = true;
			if (wasSwapped)
			{
				pathPairs.push_back( make_pair( matchIndex, firstIndex ) );
			}
			else
			{
				pathPairs.push_back( make_pair( firstIndex, matchIndex ) );
			}
		}

		allPathScores.push_back(pathScores);
		pathScores.clear();
	}

	//Go for the best match of the path scores, then remove it, and so on
	int bestMatchFirstIndex, bestMatchSecondIndex, minimumScore;
	while (true)
	{
		bool isDone = true;
		minimumScore = maximumScore;

		//Look for the best match between remaining unpaired paths
		for ( int firstIndex = 0; firstIndex < allPathScores.size(); firstIndex++ )
		{
			if ( !firstHasMatch[firstIndex] )
			{
				isDone = false;

				for ( int secondIndex = 0; secondIndex < allPathScores[firstIndex].size(); secondIndex++ )
				{
					//Find the minimum score with an unmatched pair
					if ( allPathScores[firstIndex][secondIndex] < minimumScore && !secondHasMatch[secondIndex] )
					{
						minimumScore = allPathScores[firstIndex][secondIndex];
						bestMatchFirstIndex = firstIndex;
						bestMatchSecondIndex = secondIndex;
					}
				}
			}
		}

		//Check if all first paths are paired
		if (isDone)
		{
			break;
		}
		else
		{
			//Match up the best pair
			firstHasMatch[bestMatchFirstIndex] = true;
			secondHasMatch[bestMatchSecondIndex] = true;
			if (wasSwapped)
			{
				pathPairs.push_back( make_pair( bestMatchSecondIndex, bestMatchFirstIndex ) );
			}
			else
			{
				pathPairs.push_back( make_pair( bestMatchFirstIndex, bestMatchSecondIndex ) );
			}

			//Output the guess
			if ( minimumScore > 100 )
			{
				cerr << "WARNING: Stupid match between ";
				VectorOperations::PathCERR( PathSet1[bestMatchFirstIndex] );
				cerr << " and ";
				VectorOperations::PathCERR( PathSet2[bestMatchSecondIndex] );
				cerr << endl;
			}
			else
			{
				cout << "INFO: Best guess match between ";
				VectorOperations::PathCOUT( PathSet1[bestMatchFirstIndex] );
				cout << " and ";
				VectorOperations::PathCOUT( PathSet2[bestMatchSecondIndex] );
				cout << endl;
			}
		}
	}

	//Report unpaired paths
	for ( int secondIndex = 0; secondIndex < secondHasMatch.size(); secondIndex++ )
	{
		if ( !secondHasMatch[secondIndex] )
		{
			cerr << "WARNING: Unpaired histogram: ";
			VectorOperations::PathCERR( PathSet2[secondIndex] );
			cerr << endl;
		}
	}

	return pathPairs;
}
