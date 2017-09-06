/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiCompositeAlgorithm_h
#define dqiCompositeAlgorithm_h

#include <string>
#include <vector>

#include "dqm_core/Algorithm.h"
#include "DataQualityInterfaces/HanConfigCompAlg.h"
#include "DataQualityInterfaces/HanAlgorithmConfig.h"

namespace dqi {

class CompositeAlgorithm : public dqm_core::Algorithm {
public:

  CompositeAlgorithm( const CompositeAlgorithm& other );
  CompositeAlgorithm( HanConfigCompAlg& compAlg );
  
  virtual ~CompositeAlgorithm();
  virtual dqm_core::Algorithm*  clone();
  virtual dqm_core::Result*     execute( const std::string& name, const TObject& data,
                                         const dqm_core::AlgorithmConfig& config );

  virtual void                  printDescription();

protected:

  typedef std::vector<std::pair<dqm_core::Algorithm*, std::string> > AlgVec_t;

  std::string m_name;
  AlgVec_t m_subAlgs;

  HanAlgorithmConfig* ConfigureSubAlg(const dqm_core::AlgorithmConfig& config, std::string subalg);
  
};

} //namespace dqi

#endif
