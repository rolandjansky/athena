/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_COUNTSBINSGREATERTHAN_H
#define DQM_ALGORITHMS_COUNTSBINSGREATERTHAN_H

#include <string>

#include "dqm_core/Algorithm.h"


namespace dqm_algorithms {

  class CountsBinsGreaterThan : public dqm_core::Algorithm {
  public:

    CountsBinsGreaterThan();
  
    virtual ~CountsBinsGreaterThan();
    virtual dqm_core::Algorithm*  clone();
    virtual dqm_core::Result*     execute( const std::string& name, const TObject& object,
					   const dqm_core::AlgorithmConfig& config );
    using dqm_core::Algorithm::printDescription;
    virtual void                  printDescription(std::ostream& out);

  private:
    std::string  m_name;
  };

} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_COUNTSBINSGREATERTHAN_H
