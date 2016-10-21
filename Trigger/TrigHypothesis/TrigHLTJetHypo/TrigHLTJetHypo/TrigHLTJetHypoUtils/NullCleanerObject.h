/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_NULLCLEANEROBJECT_H
#define TRIGHLTJETHYPO_NULLCLEANEROBJECT_H


// ********************************************************************
//
// NAME:     NullCleanerObject.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

// uses a cleaner bridge (wich uses an ICleaner to partition a jet collection.

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerObject.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

namespace AOD{
  class Jet;
}

class NullCleanerObject: public ICleanerObject{
public:

  NullCleanerObject();
  ~NullCleanerObject(){}
  
  pairHypoJetIter operator()(const pairHypoJetIter&) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;
};


#endif
