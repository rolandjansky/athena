/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
  @file main.cpp

  The entry point for the application
  Process the command line arguments, load the files, merge corresponding inputs and then compare

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
  */

#include "ConfigurationLoader.h"
#include "RunInput.h"
#include "RunInputOperations.h"
#include "VectorOperations.h"
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include "TFile.h"

using namespace std;

//Takes two command line arguments, each with a comma-separated list of the input files, like this:
// -new NewFile1.root,NewFile2.root -old OldFile1.root,OldFile2.root,OldFile3.root
//Alternatively takes directory names
// -newDir Directory1 -oldDir Directory2
//Should be able to deal with multiple arguments of either type
int main( int argc, char * argv[] )
{
	DIR * directory;
	struct dirent * directoryEntry;
	vector<string> newInputFiles, oldInputFiles;
	string configPath = "config/HistogramLoad.txt";

	//Loop over all arguments, separate into input files and output files
	for ( int argumentIndex = 1; argumentIndex < argc; argumentIndex++ )
	{
		string argument = argv[argumentIndex];
		if ( argument == "-config" )
		{
			//Load a config file from a location other than the default
			if ( argumentIndex + 1 < argc )
			{
				argumentIndex++;
				configPath = argv[argumentIndex];
			}
		}
		else if ( argument == "-new" )
		{
			//Panda should provide comma separated list of files in dataset
			if ( argumentIndex + 1 < argc )
			{
				argumentIndex++;
				newInputFiles = VectorOperations::SplitString( argv[argumentIndex], ',' );
			}
		}
		else if ( argument == "-old" )
		{
			//Panda should provide comma separated list of files in dataset
			if ( argumentIndex + 1 < argc )
			{
				argumentIndex++;
				oldInputFiles = VectorOperations::SplitString( argv[argumentIndex], ',' );
			}
		}
		else if ( argument == "-newDir" )
		{
			//Load all the files from a directory
			if ( argumentIndex + 1 < argc )
			{
				argumentIndex++;
				string directoryName = argv[argumentIndex];

				//Open the directory
				directory = opendir( argv[argumentIndex] );
				if (directory)
				{
					while ( ( directoryEntry = readdir(directory) ) != nullptr )
					{
						string entryName = directoryEntry->d_name;
						if ( entryName != "." && entryName != ".." )
						{
							newInputFiles.push_back( directoryName + "/" + entryName );
						}
					}
					closedir(directory);
				}
				else
				{
					cerr << "FATAL: Unable to open directory " << directoryName << endl;
					exit(1);
				}
			}
		}
		else if ( argument == "-oldDir" )
		{
			//Load all the files from a directory
			if ( argumentIndex + 1 < argc )
			{
				argumentIndex++;
				string directoryName = argv[argumentIndex];

				//Open the directory
				directory = opendir( argv[argumentIndex] );
				if (directory)
				{
					while ( ( directoryEntry = readdir(directory) ) != nullptr )
					{
						string entryName = directoryEntry->d_name;
						if ( entryName != "." && entryName != ".." )
						{
							oldInputFiles.push_back( directoryName + "/" + entryName );
						}
					}
					closedir(directory);
				}
				else
				{
					cerr << "FATAL: Unable to open directory " << directoryName << endl;
					exit(1);
				}
			}
		}
		else
		{
			cerr << "FATAL: Unrecognised argument: " << argument << endl;
			exit(1);
		}
	}

	//Load the configuration
	ConfigurationLoader * inputConfiguration = new ConfigurationLoader(configPath);
	
	//Make newInput object
	RunInput * newInput;
	if ( newInputFiles.size() > 0 )
	{
		cout << "INFO: New reprocessing input file " << newInputFiles[0] << endl;
		newInput = new RunInput( newInputFiles[0], inputConfiguration );

		//Merge in any additional input files
		for ( int inputIndex = 1; inputIndex < newInputFiles.size(); inputIndex++ )
		{
			cout << "INFO: Merging new reprocessing input file " << newInputFiles[inputIndex] << endl;
			newInput = RunInputOperations::Merge( newInput, new RunInput( newInputFiles[inputIndex], inputConfiguration ) );
		}
	}
	else
	{
		cerr << "FATAL: No new reprocessing input files" << endl;
		exit(1);
	}

	//Make oldInput object
	RunInput * oldInput;
	if ( oldInputFiles.size() > 0 )
	{
		cout << "INFO: Old reprocessing input file " << oldInputFiles[0] << endl;
		oldInput = new RunInput( oldInputFiles[0], inputConfiguration );

		//Merge in any additional input files
		for ( int inputIndex = 1; inputIndex < oldInputFiles.size(); inputIndex++ )
		{
			cout << "INFO: Merging old reprocessing input file " << oldInputFiles[inputIndex] << endl;
			oldInput = RunInputOperations::Merge( oldInput, new RunInput( oldInputFiles[inputIndex], inputConfiguration ) );
		}
	}
	else
	{
		cerr << "FATAL: No old reprocessing input files" << endl;
		exit(1);
	}

	//Create a file to store the superimposed mismatched histograms
	TFile * outputFile = new TFile( "comparison.root", "RECREATE" );

	//Compare new input and old input
	cout << "INFO: Begining comparison" << endl;
	RunInputOperations::Compare( newInput, oldInput, inputConfiguration );

	//Tidy up
	outputFile->Close();
	return 0;
}
