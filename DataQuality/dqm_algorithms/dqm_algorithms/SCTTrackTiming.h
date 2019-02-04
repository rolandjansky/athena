/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_SCTTRACKTIMING_H
#define DQM_ALGORITHMS_SCTTRACKTIMING_H

#include <string>

#include "dqm_core/Algorithm.h"


namespace dqm_algorithms {

class SCTTrackTiming : public dqm_core::Algorithm {
public:

  SCTTrackTiming();
  
  virtual ~SCTTrackTiming() = default;
  virtual dqm_core::Algorithm*  clone();
  virtual dqm_core::Result*     execute( const std::string& name, 
					 const TObject& data,
                                         const dqm_core::AlgorithmConfig& config );
  using dqm_core::Algorithm::printDescription;
  void                  printDescription(std::ostream& out);

protected:

  std::string  m_name;

  int m_NbinsX;
};

} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_SCTTRACKTIMING_H
