/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*! \file MaskedBinRow.h file declares the dqm_algorithms::MaskedBinRow  class.
 * \author R. Calkins
*/

#ifndef DQM_ALGORITHMS_MASKEDBINROW_H
#define DQM_ALGORITHMS_MASKEDBINROW_H

#include <dqm_core/Algorithm.h>
#include <string>
#include <iosfwd>

namespace dqm_algorithms
{
	struct MaskedBinRow : public dqm_core::Algorithm
        {
	  MaskedBinRow();

	  ~MaskedBinRow();

	    //overwrites virtual functions
	  MaskedBinRow * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_MASKEDBINROW_H
