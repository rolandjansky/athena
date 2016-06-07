/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_RPC_OccupancyHoleFinder_H
#define DQM_ALGORITHMS_RPC_OccupancyHoleFinder_H

#include <dqm_core/Algorithm.h>

class TH1;
class TH2;

namespace dqm_algorithms
{
	struct RPC_OccupancyHoleFinder : public dqm_core::Algorithm
        {
	  RPC_OccupancyHoleFinder(const std::string & name);
	  ~RPC_OccupancyHoleFinder();

	    //overwrites virtual functions
	  RPC_OccupancyHoleFinder * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
 
	  std::string getChamberName(const TH2* histo, int biny); 
	  
	  private:
	  std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_RPC_OccupancyHoleFinder_H
