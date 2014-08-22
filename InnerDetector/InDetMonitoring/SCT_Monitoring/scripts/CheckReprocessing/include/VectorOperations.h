/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class VectorOperations

  A collection of static methods that act on vectors

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#ifndef VECTOR_OPERATIONS_H
#define VECTOR_OPERATIONS_H

#include <vector>
#include <string>

using namespace std;

class VectorOperations
{
	public:
		static void PathCOUT( vector<string> );
		static void PathCERR( vector<string> );
		static vector<string> SplitString( string, char, bool IncludeEmptyStrings = false );
		static vector<string> CombineUniques( vector<string>, vector<string> );
		static int VectorContains( vector<string>*, string* );
		static bool FullSearch( vector<string>&, vector<string>& );
		static int ComparePaths( vector<string>, vector<string> );
		static int CompareObjectNames( string, string );
		static int CyclicCompareNames( string, string );
};

#endif
