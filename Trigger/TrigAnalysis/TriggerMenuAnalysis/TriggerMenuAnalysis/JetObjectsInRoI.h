/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __JetObjectsInRoI_h__
#define __JetObjectsInRoI_h__
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

  const TrigT2Jet* getL2Jet() const { return mL2Jet; }
  const JetCollection* getEFJet() const { return mEFJet; }

  void setL2Jet(const TrigT2Jet* L2jet) { mL2Jet = L2jet; }
  void setEFJet(const JetCollection* EFjet) { mEFJet = EFjet; }

protected:
  const TrigT2Jet* mL2Jet;
  const JetCollection* mEFJet;
};

#endif // __JetObjectsInRoI_h__
