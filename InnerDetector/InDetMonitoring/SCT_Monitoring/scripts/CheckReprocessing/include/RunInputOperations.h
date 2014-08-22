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

#ifndef RUN_INPUT_OPERATIONS_H
#define RUN_INPUT_OPERATIONS_H

#include "RunInput.h"
#include <vector>

class RunInputOperations
{
	public:
		static RunInput * Merge( RunInput*, RunInput* );
		static void Compare( RunInput*, RunInput*, ConfigurationLoader* );

	private:
		static void CompareStructures( RunInput*, RunInput*, vector< vector<string> >&, vector< pair< TH1*, TH1* > >&, vector<string>& );
		static vector< pair< int, int > > CalculatePathPairs( vector< vector<string> >, vector< vector<string> > );
};

#endif
