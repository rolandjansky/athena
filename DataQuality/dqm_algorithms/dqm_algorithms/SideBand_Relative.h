/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SideBand_Relative.h file declares the dqm_algorithms::SideBand_Relative  class.
 * \author Andrea Dotti
*/

#ifndef DQM_ALGORITHMS_SIDEBAND_RELATIVE_H
#define DQM_ALGORITHMS_SIDEBAND_RELATIVE_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/SideBand.h>

namespace dqm_algorithms
{
	struct SideBand_Relative : public SideBand
        {
	    SideBand_Relative():  SideBand("SideBand_Relative")  {};
              
	};

}

#endif // DQM_ALGORITHMS_SIDEBAND_RELATIVE_H
