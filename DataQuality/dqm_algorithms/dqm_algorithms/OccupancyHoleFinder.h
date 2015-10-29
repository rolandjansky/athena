/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_OCCUPANCYHOLEFINDER_H
#define DQM_ALGORITHMS_OCCUPANCYHOLEFINDER_H

#include <dqm_core/Algorithm.h>

class TH1;
class TH2;

namespace dqm_algorithms
{
	struct OccupancyHoleFinder : public dqm_core::Algorithm
        {
	  OccupancyHoleFinder(const std::string & name);
	  ~OccupancyHoleFinder();

	    //overwrites virtual functions
	  OccupancyHoleFinder * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);

	  //return histogram with median in each vertical strip
	  TH1* getMedian(const TH2* histo);
	  //Have this function call your own function based on name
	  //See MDT_OccupancyHoleFinder.h as an example
	  std::string getChamberName(const TH2* histo, int biny);
	  //Called when calling derived instance 'MDT_OccupancyHoleFinder.h'
	  std::string getMDTChamberName(const TH2* histo, int biny);
	  std::string getMDTChamberNameByCrate(int biny, std::string crate);

        private:
	  std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_OCCUPANCYHOLEFINDER_H
