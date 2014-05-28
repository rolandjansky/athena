/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file GraphPrint.h file declares the dqm_algorithms::GraphPrint  class.
 * \author Jahred Adelman
*/
#ifndef DQM_ALGORITHMS_GRAPHPRINT_H
#define DQM_ALGORITHMS_GRAPHPRINT_H
#include <string>
#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
  class GraphPrint : public dqm_core::Algorithm
    {
    public:
      GraphPrint();
      virtual GraphPrint* clone();
      virtual dqm_core::Result* execute(const std::string &,
					const TObject & obj, 
					const dqm_core::AlgorithmConfig & conf );
      virtual ~GraphPrint();
      using dqm_core::Algorithm::printDescription;
      void printDescription(std::ostream& out);
    };
}

#endif // DQM_ALGORITHMS_GRAPHTEST_H
