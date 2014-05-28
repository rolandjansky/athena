/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SideBand_Absolute.h file declares the dqm_algorithms::SideBand_Absolute  class.
 * \author Andrea Dotti
*/

#ifndef DQM_ALGORITHMS_SIDEBAND_ABSOLUTE_H
#define DQM_ALGORITHMS_SIDEBAND_ABSOLUTE_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/SideBand.h>

namespace dqm_algorithms
{
	struct SideBand_Absolute : public SideBand
        {
	    SideBand_Absolute():  SideBand("SideBand_Absolute")  {};
              
	};

}

#endif // DQM_ALGORITHMS_SIDEBAND_ABSOLUTE_H
