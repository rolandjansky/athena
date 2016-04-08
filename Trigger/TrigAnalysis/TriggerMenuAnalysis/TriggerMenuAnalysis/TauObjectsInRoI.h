/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TAUOBJECTSINROI_H
#define TRIGGERMENUANALYSIS_TAUOBJECTSINROI_H
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
  const TrigTauCluster* m_tauCluster;
  const TrigTau* m_tau;
  const Analysis::TauJetContainer* m_EFTau;
};

const TrigTauCluster* TauObjectsInRoI::getTauCluster() const {
  return m_tauCluster;
}

const TrigTau* TauObjectsInRoI::getTau() const {
  return m_tau;
}

const Analysis::TauJetContainer* TauObjectsInRoI::getEFTau() const {
  return m_EFTau;
}
void TauObjectsInRoI::setTauCluster(const TrigTauCluster* x) {
  m_tauCluster = x;
}

void TauObjectsInRoI::setTau(const TrigTau* x) {
  m_tau = x;
}

void TauObjectsInRoI::setEFTau(const Analysis::TauJetContainer* x){
  m_EFTau = x;
}

#endif //TRIGGERMENUANALYSIS_TAUOBJECTSINROI_H
