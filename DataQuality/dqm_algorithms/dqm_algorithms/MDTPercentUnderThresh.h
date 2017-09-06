/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_MDTPERCENTUNDERTHRESH_H
#define DQM_ALGORITHMS_MDTPERCENTUNDERTHRESH_H


#include <string>
#include "TObject.h"
#include "dqm_core/Algorithm.h"


namespace dqm_algorithms {

class MDTPercentUnderThresh : public dqm_core::Algorithm {
public:

  MDTPercentUnderThresh();
  
  virtual ~MDTPercentUnderThresh();
  virtual dqm_core::Algorithm*  clone();
  virtual dqm_core::Result*     execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& config );
  using dqm_core::Algorithm::printDescription;
  virtual void                  printDescription(std::ostream& out);

private:
  std::string  m_name;
};

} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_MDTPERCENTUNDERTHRESH_H
