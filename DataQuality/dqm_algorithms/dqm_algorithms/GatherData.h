/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_GATHERDATA_H
#define DQM_ALGORITHMS_GATHERDATA_H



#include "dqm_core/Algorithm.h"
#include <string>
#include <iosfwd>

namespace dqm_algorithms {

class GatherData : public dqm_core::Algorithm {
public:

  GatherData();
  
  virtual ~GatherData();
  virtual dqm_core::Algorithm*  clone();
  virtual dqm_core::Result*     execute( const std::string& name, const TObject& data,
                                         const dqm_core::AlgorithmConfig& config );
  using dqm_core::Algorithm::printDescription;
  virtual void                  printDescription(std::ostream& out);

protected:

  std::string  m_name;
};

} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_GATHERDATA_H
