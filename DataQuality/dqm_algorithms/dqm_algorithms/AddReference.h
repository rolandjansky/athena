/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AddReference.h file declares the dqm_algorithms::AddReference class.
 *  \author andrea.dotti@cern.ch
 */

#ifndef DQM_ALGORITHMS_ADDREFERENCE_H
#define DQM_ALGORITHMS_ADDREFERENCE_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
  struct AddReference : public dqm_core::Algorithm
    {
      AddReference(const std::string& name );
      AddReference* clone();
      using dqm_core::Algorithm::printDescription;
      void printDescription(std::ostream& out);
      dqm_core::Result* execute(const std::string& , const TObject& , const dqm_core::AlgorithmConfig& );
      private:
      std::string m_name;
  };
}

#endif // DQM_ALGORITHMS_ADDREFERENCE_H
