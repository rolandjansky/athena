/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class VectorOperations

  A collection of static methods that act on vectors

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#include "VectorOperations.h"
#include <iostream>
#include <cstring>

using namespace std;

//Methods to output a histogram path
void VectorOperations::PathCOUT( vector<string> InputPath )
{
	for  ( int objectIndex = 0; objectIndex < InputPath.size(); objectIndex++ )
	{
		cout << "/" << InputPath[objectIndex];
	}
}
void VectorOperations::PathCERR( vector<string> InputPath )
{
	for  ( int objectIndex = 0; objectIndex < InputPath.size(); objectIndex++ )
	{
		cerr << "/" << InputPath[objectIndex];
	}
}

//Split a string every time you find a given character
vector<string> VectorOperations::SplitString( string Input, char SplitCharacter, bool IncludeEmptyStrings )
{
	vector<string> splitParts;

	while(true)
	{
		//Search for the character
		int position = Input.find_first_of(SplitCharacter);

		if ( position == -1 )
		{
			//Ignore empty strings
			if ( Input != "" || IncludeEmptyStrings )
			{
				splitParts.push_back( Input );
			}

			//If it's not found, you've reached the end
			break;
		}
		else
		{
			//Split the string at the character postion
			string tempString( Input, 0, position );
			Input = Input.substr( position + 1 );

			//Ignore empty strings
			if ( tempString != "" || IncludeEmptyStrings )
			{
				splitParts.push_back( tempString );
			}
		}
	}

	return splitParts;
}

//Return a vector containing all the unique strings from the two input vectors
vector<string> VectorOperations::CombineUniques( vector<string> VectorOne, vector<string> VectorTwo )
{
	vector<string> result;
	vector<string>::iterator stringIterator;

	//Don't assume VectorOne is unique
	for ( stringIterator = VectorOne.begin(); stringIterator != VectorOne.end(); stringIterator++ )
	{
		if ( VectorContains( &result, &(*stringIterator) ) == -1 )
		{
			result.push_back( *stringIterator );
		}
	}

	//Now add in VectorTwo
	for ( stringIterator = VectorTwo.begin(); stringIterator != VectorTwo.end(); stringIterator++ )
	{
		if ( VectorContains( &result, &(*stringIterator) ) == -1 )
		{
			result.push_back( *stringIterator );
		}
	}

	return result;
}

//Return the position of a search string within a vector of strings, or -1 if not found
int VectorOperations::VectorContains( vector<string> * InputVector, string * SearchString )
{
	for ( int searchIndex = 0; searchIndex < InputVector->size(); searchIndex++ )
	{
		if ( (*InputVector)[searchIndex] == (*SearchString) )
		{
			//Found the search string
			return searchIndex;
		}
	}

	//If you've got this far, it wasn't found
	return -1;
}

//Return true if a path matches a given search vector
bool VectorOperations::FullSearch( vector<string> & TestPath, vector<string> & SearchVector )
{
	if ( TestPath.size() <= SearchVector.size() )
	{
		//Check the full depth of the vector
		for ( int pathIndex = 0; pathIndex < TestPath.size(); pathIndex++ )
		{
			//Examine all the parts of this search string
			vector<string> searchParts = VectorOperations::SplitString( SearchVector[pathIndex], ',' );
			for ( int partIndex = 0; partIndex < searchParts.size(); partIndex++ )
			{
				string part = searchParts[partIndex];
				if ( part == "*" )
				{
					//* matches anything

					if ( pathIndex + 1 == TestPath.size() && partIndex + 1 == searchParts.size() )
					{
						//If it matches all the way to the end of the path, return true
						return true;
					}
				}
				else if ( strstr( TestPath[pathIndex].c_str(), part.c_str() ) != 0 )
				{
					//Match if this part is found in the test string

					if ( pathIndex + 1 == TestPath.size() && partIndex + 1 == searchParts.size() )
					{
						//If it matches all the way to the end of the path, return true
						return true;
					}
				}
				else if ( part[0] == '!' && strstr( TestPath[pathIndex].c_str(), part.substr(1).c_str() ) == 0 )
				{
					//Match if this part is not found in the test string

					if ( pathIndex + 1 == TestPath.size() && partIndex + 1 == searchParts.size() )
					{
						//If it matches all the way to the end of the path, return true
						return true;
					}
				}
				else
				{
					//Mismatch
					return false;
				}
			}
		}
	}
	else
	{
		//Search string does not go to this depth
		return false;
	}
}

//Return a score indicating the compatibility of two paths. 0 = identical
int VectorOperations::ComparePaths( vector<string> FirstPath, vector<string> SecondPath )
{
	int pathScore = 0;

	//Make sure the first path is the shortest
	if ( SecondPath.size() < FirstPath.size() )
	{
		vector<string> temporaryPath = FirstPath;
		FirstPath = SecondPath;
		SecondPath = temporaryPath;
	}

	//Compare the object names for where the paths overlap
	for ( int objectIndex = 0; objectIndex < FirstPath.size(); objectIndex++ )
	{
		int score = CompareObjectNames( FirstPath[objectIndex], SecondPath[objectIndex] );
		if ( objectIndex != FirstPath.size() - 1 )
		{
			score *= 100;
		}
		pathScore += score;
	}

	//Add the length of every additional part of the longer path
	for ( int objectIndex = FirstPath.size(); objectIndex < SecondPath.size(); objectIndex++ )
	{
		pathScore += 100 * SecondPath[objectIndex].size();
	}

	return pathScore;
}

//Return a score indicating the compatibility of two strings. 0 = identical
int VectorOperations::CompareObjectNames( string FirstName, string SecondName )
{
	//Make sure the first name is the shortest
	if ( SecondName.size() < FirstName.size() )
	{
		string temporaryName = FirstName;
		FirstName = SecondName;
		SecondName = temporaryName;
	}

	//Compare the names character by character from the beginning
	int fromBeginningMatches = 0;
	for ( int characterIndex = 0; characterIndex < FirstName.size(); characterIndex++ )
	{
		if ( FirstName[characterIndex] == SecondName[characterIndex] )
		{
			fromBeginningMatches++;
		}
		else
		{
			break;
		}
	}

	//Return 0 if identical, extra length of longer string
	if ( fromBeginningMatches == FirstName.size() )
	{
		return SecondName.size() - fromBeginningMatches;
	}
	else
	{
		//Compare the names character by character from the end
		int fromEndMatches = 0;
		for ( int characterIndex = 0; characterIndex < FirstName.size(); characterIndex++ )
		{
			if ( FirstName[ FirstName.size() - characterIndex - 1 ] == SecondName[ SecondName.size() - characterIndex - 1 ] )
			{
				fromEndMatches++;
			}
			else
			{
				break;
			}       

		}

		//If there's a mismatched chunk in the middle, examine it
		if ( FirstName.size() - fromBeginningMatches - fromEndMatches > 2 )
		{
			string firstNameChunk = FirstName.substr( fromBeginningMatches, FirstName.size() - fromBeginningMatches - fromEndMatches );
			string secondNameChunk = SecondName.substr( fromBeginningMatches, SecondName.size() - fromBeginningMatches - fromEndMatches );
			return SecondName.size() - fromBeginningMatches - fromEndMatches - CyclicCompareNames( firstNameChunk, secondNameChunk );
		}
		else
		{
			return SecondName.size() - fromBeginningMatches - fromEndMatches;
		}
	}
}

//Return the length of a matching character sequence in two strings
int VectorOperations::CyclicCompareNames( string FirstName, string SecondName )
{
	//Make sure the first name is the shortest
	if ( SecondName.size() < FirstName.size() )
	{
		string temporaryName = FirstName;
		FirstName = SecondName;
		SecondName = temporaryName;
	}

	//Find the first matching character
	int firstMatch = -1;
	for ( int characterIndex = 0; characterIndex < SecondName.size(); characterIndex++ )
	{
		if ( FirstName[0] == SecondName[characterIndex] )
		{
			firstMatch = characterIndex;
			break;
		}
	}

	if ( firstMatch == -1 )
	{
		//Strings are completely dissimilar
		return 0;
	}
	else
	{
		//Assume there's a similar character seqence that has just been offset (and possibly cycled) in the second string
		int numberMatching = 1;
		for ( int characterIndex = 1; characterIndex < FirstName.size(); characterIndex++ )
		{
			int secondIndex = ( characterIndex + firstMatch ) % SecondName.size();
			if ( FirstName[characterIndex] == SecondName[secondIndex] )
			{
				numberMatching++;
			}
			else
			{
				break;
			}
		}

		return numberMatching;
	}
}
