/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_MISSINGETOBJECTSINROI_H
#define TRIGGERMENUANALYSIS_MISSINGETOBJECTSINROI_H
/*
  MissingETObjectsInRoI.h
*/
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"

class TrigMissingET;

namespace HLT {
  class TriggerElement;
  class Navigation;
}

class MissingETObjectsInRoI : public HLTObjectsInRoI {
public:
  static HLTObjectsInRoI* create();

public:
  MissingETObjectsInRoI();
  ~MissingETObjectsInRoI();

  const TrigMissingET* getL2MissingET() const { return m_L2MissingET; }
  const TrigMissingET* getEFMissingET() const { return m_EFMissingET; }

  void setL2MissingET(const TrigMissingET* L2MissingET) { m_L2MissingET = L2MissingET; }
  void setEFMissingET(const TrigMissingET* EFMissingET) { m_EFMissingET = EFMissingET; }

//   bool runBySameFex(const HLTObjectsInRoI* x) const;
//   bool setDataFromTE(int isig, 
// 		     const HLT::TriggerElement* te, 
// 		     HLT::Navigation* navitool,int level);
  
protected:
  const TrigMissingET* m_L2MissingET;
  const TrigMissingET* m_EFMissingET;

};

#endif // TRIGGERMENUANALYSIS_MISSINGETOBJECTSINROI_H
