/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinThreshold.h file declares the dqm_algorithms::BinThreshold  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINTHRESHOLD_H
#define DQM_ALGORITHMS_BINTHRESHOLD_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BinThreshold : public dqm_core::Algorithm
        {
	  BinThreshold(const std::string & name);

	    //overwrites virtual functions
	  BinThreshold * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          bool CompareBinThreshold( const std::string & objname, double bincontent, double threshold );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
 
	  private:
	  std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_BINTHRESHOLD_H
