//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LARCONDITIONSMERGERALG
#define LARCALIBUTILS_LARCONDITIONSMERGERALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "LArRawConditions/LArConditionsContainer.h"

template<class T>
class LArConditionsMergerAlg: public AthAlgorithm {
 public:
  using AthAlgorithm::AthAlgorithm;
  ~LArConditionsMergerAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  
  SG::ReadCondHandleKeyArray<T>  m_readKeys{this, "ReadKeys",{},"Input keys" };
  Gaudi::Property<std::string> m_writeKey{this,"WriteKey","output","Output key" };

  Gaudi::Property<std::string> m_groupingType{this,"GroupingType","","Cool-channel grouping"};

};

#include "LArConditionsMergerAlg.icc"

#include "LArRawConditions/LArPedestalComplete.h"
typedef LArConditionsMergerAlg<LArPedestalComplete>  LArPedestalMerger;

#include "LArRawConditions/LArOFCComplete.h"
typedef LArConditionsMergerAlg<LArOFCComplete>  LArOFCMerger;

#include "LArRawConditions/LArShapeComplete.h"
typedef LArConditionsMergerAlg<LArShapeComplete>  LArShapeMerger;

#include "LArRawConditions/LArRampComplete.h"
typedef LArConditionsMergerAlg<LArRampComplete>  LArRampMerger;

#include "LArRawConditions/LArMphysOverMcalComplete.h"
typedef LArConditionsMergerAlg<LArMphysOverMcalComplete>  LArMphysOverMcalMerger;

#include "LArRawConditions/LArAutoCorrComplete.h"
typedef LArConditionsMergerAlg<LArAutoCorrComplete>  LArAutoCorrMerger;

#endif
