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
#include <algorithm>
#include <iterator>
#include <cstring>

using namespace std;

namespace VectorOperations{
  //Methods to output a histogram path
  void 
  PathCOUT( const vector<string> & InputPath ){
    for  ( const auto & i: InputPath){
      cout << "/" << i;
    }
  }
  
  void 
  PathCERR( const vector<string> & InputPath ){
    for  ( const auto & i: InputPath){
      cerr << "/" << i;
    }
  }

  //Split a string every time you find a given character
  vector<string> 
  SplitString( const string & Input, char SplitCharacter, bool IncludeEmptyStrings )
  {
    vector<string> splitParts;

    while(true)
    {
      //Search for the character
      int position = Input.find_first_of(SplitCharacter);
      if ( position == -1 ){
        //Ignore empty strings
        if ( Input != "" || IncludeEmptyStrings ){
          splitParts.push_back( Input );
        }
        //If it's not found, you've reached the end
        break;
      } else {
        //Split the string at the character postion
        string tempString( Input, 0, position );
        Input = Input.substr( position + 1 );
        //Ignore empty strings
        if ( tempString != "" || IncludeEmptyStrings ){
          splitParts.push_back( tempString );
        }
      }
    }
    return splitParts;
  }

  //Return a vector containing all the unique strings from the two input vectors
  vector<string> 
  CombineUniques( const vector<string> & VectorOne, const vector<string> & VectorTwo ){
    vector<string> result;
    //Don't assume VectorOne is unique
    for ( const auto & i1: VectorOne ){
      if ( VectorContains( result, i1)  == -1 ){
        result.push_back( i1 );
      }
    }

    //Now add in VectorTwo
    for ( const auto & i2: VectorTwo){
      if ( VectorContains( result, i2 ) == -1 ){
        result.push_back( i2 );
      }
    }
    return result;
  }

  //Return the position of a search string within a vector of strings, or -1 if not found
  int 
  VectorContains( const vector<string> & InputVector, const string & SearchString ){
    auto position = std::find(InputVector.begin(), InputVector.end(), SearchString);
    return (position != InputVector.end()) ? std::distance(InputVector.begin(), position) : -1;
  }

  //Return true if a path matches a given search vector
  bool 
  FullSearch( const vector<string> & TestPath, const vector<string> & SearchVector ){
    if ( TestPath.size() <= SearchVector.size() ){
      //Check the full depth of the vector
      for ( int pathIndex = 0; pathIndex < TestPath.size(); pathIndex++ ){
        //Examine all the parts of this search string
        vector<string> searchParts = SplitString( SearchVector[pathIndex], ',' );
        for ( int partIndex = 0; partIndex < searchParts.size(); partIndex++ ){
          string part = searchParts[partIndex];
          if ( part == "*" ){
            //* matches anything
            if ( pathIndex + 1 == TestPath.size() && partIndex + 1 == searchParts.size() ){
              //If it matches all the way to the end of the path, return true
              return true;
            }
          } else if ( strstr( TestPath[pathIndex].c_str(), part.c_str() ) != 0 ){
            //Match if this part is found in the test string
            if ( pathIndex + 1 == TestPath.size() && partIndex + 1 == searchParts.size() ){
              //If it matches all the way to the end of the path, return true
              return true;
            }
          } else if ( part[0] == '!' && strstr( TestPath[pathIndex].c_str(), part.substr(1).c_str() ) == 0 ){
            //Match if this part is not found in the test string

            if ( pathIndex + 1 == TestPath.size() && partIndex + 1 == searchParts.size() ){
              //If it matches all the way to the end of the path, return true
              return true;
            }
          } else {
            //Mismatch
            return false;
          }
        }
      }
    } else {
      //Search string does not go to this depth
      return false;
    }
  }

  //Return a score indicating the compatibility of two paths. 0 = identical
  int 
  ComparePaths( const vector<string> & FirstPath, const vector<string> & SecondPath ){
    int pathScore = 0;
    
    auto & v1{FirstPath};
    auto & v2{SecondPath};
    if ( SecondPath.size() < FirstPath.size() ){
      v2=FirstPath;
      v1=SecondPath;
    }

    //Compare the object names for where the paths overlap
    for ( int objectIndex = 0; objectIndex < v1.size(); objectIndex++ ){
      int score = CompareObjectNames( v1[objectIndex], v2[objectIndex] );
      if ( objectIndex != v1.size() - 1 ){
        score *= 100;
      }
      pathScore += score;
    }

    //Add the length of every additional part of the longer path
    for ( int objectIndex = v1.size(); objectIndex < v2.size(); objectIndex++ ){
      pathScore += 100 * v2[objectIndex].size();
    }

    return pathScore;
  }

  //Return a score indicating the compatibility of two strings. 0 = identical
  int CompareObjectNames( const string & FirstName, string & SecondName ){
    //Make sure the first name is the shortest
    auto & s1{FirstName};
    auto & s2{SecondName};
    if ( SecondName.size() < FirstName.size() ){
      s2=FirstName;
      s1=SecondName;
    }

    //Compare the names character by character from the beginning
    int fromBeginningMatches = 0;
    for ( int characterIndex = 0; characterIndex < s1.size(); characterIndex++ ) {
      if ( s1[characterIndex] == s2[characterIndex] ) {
        fromBeginningMatches++;
      } else {
        break;
      }
    }

    //Return 0 if identical, extra length of longer string
    if ( fromBeginningMatches == s1.size() )
    {
      return s2.size() - fromBeginningMatches;
    } else {
      //Compare the names character by character from the end
      int fromEndMatches = 0;
      for ( int characterIndex = 0; characterIndex < s1.size(); characterIndex++ ) {
        if ( s1[ s1.size() - characterIndex - 1 ] == s2[ s2.size() - characterIndex - 1 ] ) {
          fromEndMatches++;
        } else {
          break;
        }       

      }

      //If there's a mismatched chunk in the middle, examine it
      if ( s1.size() - fromBeginningMatches - fromEndMatches > 2 ) {
        string firstNameChunk = s1.substr( fromBeginningMatches, s1.size() - fromBeginningMatches - fromEndMatches );
        string secondNameChunk = s2.substr( fromBeginningMatches, s2.size() - fromBeginningMatches - fromEndMatches );
        return s2.size() - fromBeginningMatches - fromEndMatches - CyclicCompareNames( firstNameChunk, secondNameChunk );
      } else {
        return s2.size() - fromBeginningMatches - fromEndMatches;
      }
    }
  }

  //Return the length of a matching character sequence in two strings
  int 
  CyclicCompareNames( const string & FirstName, const string & SecondName ){
    //Make sure the first name is the shortest
    auto & s1{FirstName};
    auto & s2{SecondName};
    if ( SecondName.size() < FirstName.size() ){
      s2=FirstName;
      s1=SecondName;
    }

    //Find the first matching character
    int firstMatch = -1;
    for ( int characterIndex = 0; characterIndex < s2.size(); characterIndex++ ){
      if ( s1[0] == s2[characterIndex] ){
        firstMatch = characterIndex;
        break;
      }
    }

    if ( firstMatch == -1 ) {
      //Strings are completely dissimilar
      return 0;
    } else {
      //Assume there's a similar character seqence that has just been offset (and possibly cycled) in the second string
      int numberMatching = 1;
      for ( int characterIndex = 1; characterIndex < s1.size(); characterIndex++ ) {
        int secondIndex = ( characterIndex + firstMatch ) % s2.size();
        if ( s1[characterIndex] == s2[secondIndex] ) {
          numberMatching++;
        } else {
          break;
        }
      }
      return numberMatching;
    }
  }
}
