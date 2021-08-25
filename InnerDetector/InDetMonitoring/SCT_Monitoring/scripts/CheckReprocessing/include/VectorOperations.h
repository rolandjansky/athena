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

namespace VectorOperations{
		void PathCOUT( const std::vector<std::string> & vec);
		void PathCERR( const std::vector<std::string> & vec );
		std::vector<std::string> SplitString( std::string & s, char delimiter, bool IncludeEmptyStrings = false );
		std::vector<std::string> CombineUniques( const std::vector<std::string> & v1, const std::vector<std::string> & v2);
		int VectorContains( std::vector<std::string>*, std::string* );
		bool FullSearch( const std::vector<std::string> & v1, const std::vector<std::string> & v2);
		int ComparePaths( const std::vector<std::string> & v1, const std::vector<std::string> & v2 );
		int CompareObjectNames( const std::string& name1, const std::string & name2 );
		int CyclicCompareNames( const std::string& name1, const std::string & name2 );
};

#endif
