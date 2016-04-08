/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_JETOBJECTSINROI_H
#define TRIGGERMENUANALYSIS_JETOBJECTSINROI_H
/*
  JetObjectsInRoI.h
*/
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"

class TrigT2Jet;
class JetCollection;

namespace HLT {
  class TriggerElement;
  class Navigation;
}

class JetObjectsInRoI : public HLTObjectsInRoI {
public:
  static HLTObjectsInRoI* create();

public:
  JetObjectsInRoI();
  ~JetObjectsInRoI();

  const TrigT2Jet* getL2Jet() const { return m_L2Jet; }
  const JetCollection* getEFJet() const { return m_EFJet; }

  void setL2Jet(const TrigT2Jet* L2jet) { m_L2Jet = L2jet; }
  void setEFJet(const JetCollection* EFjet) { m_EFJet = EFjet; }

protected:
  const TrigT2Jet* m_L2Jet;
  const JetCollection* m_EFJet;
};

#endif // TRIGGERMENUANALYSIS_JETOBJECTSINROI_H
