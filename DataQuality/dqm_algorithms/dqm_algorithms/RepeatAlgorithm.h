/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqaRepeatAlgorithm_h
#define dqaRepeatAlgorithm_h

#include <string>
#include <vector>

#include "dqm_core/Algorithm.h"

namespace dqm_algorithms {

class RepeatAlgorithm : public dqm_core::Algorithm {
public:

  RepeatAlgorithm( const RepeatAlgorithm& other );
  RepeatAlgorithm();
  RepeatAlgorithm& operator=(const RepeatAlgorithm& other);
  
  virtual ~RepeatAlgorithm();
  virtual dqm_core::Algorithm*  clone();
  virtual dqm_core::Result*     execute( const std::string& name, const TObject& data,
                                         const dqm_core::AlgorithmConfig& config );
  dqm_core::AlgorithmConfig*    ConfigureSubAlg(const dqm_core::AlgorithmConfig& config, TObject* reference);

  using Algorithm::printDescription;
  virtual void                  printDescription(std::ostream& out);

protected:

  std::shared_ptr<dqm_core::Algorithm> m_subalg;

};

} //namespace dqi

#endif
