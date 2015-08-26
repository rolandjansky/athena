/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TauObjectsInRoI_h__
#define __TauObjectsInRoI_h__
/*
  TauObjectsInRoI.h
*/
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"

class TrigTauCluster;
class TrigTau;

namespace Analysis{
  class TauJetContainer;
}

namespace HLT {
  class TriggerElemetn;
  class Navigation;
}

class TauObjectsInRoI : public HLTObjectsInRoI {
public:
  static HLTObjectsInRoI* create();

public:
  TauObjectsInRoI();
  ~TauObjectsInRoI();

  inline const TrigTauCluster* getTauCluster() const;
  inline const TrigTau* getTau() const;
  inline const Analysis::TauJetContainer* getEFTau() const;

  inline void setTauCluster(const TrigTauCluster* x);
  inline void setTau(const TrigTau* x);
  inline void setEFTau(const Analysis::TauJetContainer* x);

protected:
  const TrigTauCluster* mTauCluster;
  const TrigTau* mTau;
  const Analysis::TauJetContainer* mEFTau;
};

const TrigTauCluster* TauObjectsInRoI::getTauCluster() const {
  return mTauCluster;
}

const TrigTau* TauObjectsInRoI::getTau() const {
  return mTau;
}

const Analysis::TauJetContainer* TauObjectsInRoI::getEFTau() const {
  return mEFTau;
}
void TauObjectsInRoI::setTauCluster(const TrigTauCluster* x) {
  mTauCluster = x;
}

void TauObjectsInRoI::setTau(const TrigTau* x) {
  mTau = x;
}

void TauObjectsInRoI::setEFTau(const Analysis::TauJetContainer* x){
  mEFTau = x;
}

#endif //__TauObjectsInRoI_h__
