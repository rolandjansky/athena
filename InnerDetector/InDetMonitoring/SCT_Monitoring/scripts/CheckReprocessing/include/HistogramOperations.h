/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class HistogramOperations

  A collection of static methods that act on Histograms
  Implement histogram comparisons here

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#ifndef HISTOGRAM_OPERATIONS_H
#define HISTOGRAM_OPERATIONS_H

#include <string>
#include "TH1.h"

using namespace std;

class HistogramOperations
{
	public:
		static TH1 * MergeHistograms( TH1*, TH1* );
		static double BinwiseCompare( TH1*, TH1*, string, bool );
		static double BinwiseHackCompare( TH1*, TH1*, string );
		static double KolmogorovTest( TH1*, TH1*, string );
		static double CompareMean( TH1*, TH1*, string, bool );
		static double CompareSigma( TH1*, TH1*, string, bool );
		static double CompareTotalEntries( TH1*, TH1*, string );
		static void FileOutput( TH1*, TH1*, string );
};

#endif
