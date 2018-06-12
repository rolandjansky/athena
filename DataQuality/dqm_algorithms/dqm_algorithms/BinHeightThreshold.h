/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinHeightThreshold.h file declares the dqm_algorithms::BinHeightThreshold  class.
 * \author Margherita Spalla. Copied and adapted from BinsThreshold.h
*/

#ifndef DQM_ALGORITHMS_BINHEIGHTTHRESHOLD_H
#define DQM_ALGORITHMS_BINHEIGHTTHRESHOLD_H

#include <dqm_core/Algorithm.h>


namespace dqm_algorithms
{
	struct BinHeightThreshold : public dqm_core::Algorithm
        {
	  enum binStatus {aRedBin,aYellowBin,aGreenBin,anUndefBin=-1};
	  BinHeightThreshold();

	    //overwrites virtual functions
	  BinHeightThreshold * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          BinHeightThreshold::binStatus CompareBinHeightThreshold(double bincontent, double thresholdGr, double thresholdRed);
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
 
	  private:
	};
}

#endif // DQM_ALGORITHMS_BINHEIGHTTHRESHOLD_H
