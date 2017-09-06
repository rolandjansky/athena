/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_MDTOVERVIEW_H
#define DQM_ALGORITHMS_MDTOVERVIEW_H

#include <string>

#include "dqm_core/Algorithm.h"

class TF1;
class TH1;
class TAxis;

namespace dqm_algorithms {

class MDTOverview : public dqm_core::Algorithm {
public:

  MDTOverview(const std::string & name);
  
  virtual ~MDTOverview();
  virtual dqm_core::Algorithm*  clone();
  virtual dqm_core::Result*     execute( const std::string& name, const TObject& object,
                                         const dqm_core::AlgorithmConfig& config );
  using dqm_core::Algorithm::printDescription;
  virtual void                  printDescription(std::ostream& out);

private:
  std::string  m_name;
};

} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_MDTOVERVIEW_H
