/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideReference.h file declares the dqm_algorithms::DivideReference class.
 *  \author andrea.dotti@cern.ch
 */

#ifndef DQM_ALGORITHMS_DIVIDEREFERENCE_H
#define DQM_ALGORITHMS_DIVIDEREFERENCE_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
  struct DivideReference : public dqm_core::Algorithm
    {
      DivideReference(const std::string& name );
      DivideReference* clone();
      using dqm_core::Algorithm::printDescription;
      void printDescription(std::ostream& out);
      dqm_core::Result* execute(const std::string& , const TObject& , const dqm_core::AlgorithmConfig& );
      private:
      std::string m_name;
  };
}

#endif // DQM_ALGORITHMS_DIVIDEREFERENCE_H
