/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_CHECKMEAN_H
#define DQM_ALGORITHMS_CHECKMEAN_H

#include <string>

#include "dqm_core/Algorithm.h"


namespace dqm_algorithms {

class CheckMean : public dqm_core::Algorithm {
public:

  CheckMean();
  
  virtual ~CheckMean();
  virtual dqm_core::Algorithm*  clone();
  virtual dqm_core::Result*     execute( const std::string& name, const TObject& data,
                                         const dqm_core::AlgorithmConfig& config );
  using dqm_core::Algorithm::printDescription;
  virtual void                  printDescription(std::ostream& out);

protected:

  virtual double  getWarningLimit( const dqm_core::AlgorithmConfig& config, std::string limitName );
  virtual double  getErrorLimit( const dqm_core::AlgorithmConfig& config, std::string limitName );

  std::string  m_name;
  std::string  m_limitName;
};

} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_CHECKMEAN_H
