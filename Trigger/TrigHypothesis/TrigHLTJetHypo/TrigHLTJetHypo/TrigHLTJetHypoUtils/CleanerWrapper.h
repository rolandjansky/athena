/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CLEANERWRAPPER_H
#define TRIGHLTJETHYPO_CLEANERWRAPPER_H


// ********************************************************************
//
// NAME:     CleanerWrapper.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

// uses a cleaner bridge (wich uses an ICleaner to partition a jet collection.

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerObject.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"


namespace AOD{
  class Jet;
}

class CleanerWrapper: public ICleanerObject{
public:

  CleanerWrapper(const CleanerBridge&);
  ~CleanerWrapper(){}
  
  pairHypoJetIter operator()(const pairHypoJetIter&) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  CleanerBridge m_cleaner;
};


#endif
