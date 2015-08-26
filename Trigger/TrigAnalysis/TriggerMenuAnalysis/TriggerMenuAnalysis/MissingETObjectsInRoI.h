/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MissingETObjectsInRoI_h__
#define __MissingETObjectsInRoI_h__
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

  const TrigMissingET* getL2MissingET() const { return mL2MissingET; }
  const TrigMissingET* getEFMissingET() const { return mEFMissingET; }

  void setL2MissingET(const TrigMissingET* L2MissingET) { mL2MissingET = L2MissingET; }
  void setEFMissingET(const TrigMissingET* EFMissingET) { mEFMissingET = EFMissingET; }

//   bool runBySameFex(const HLTObjectsInRoI* x) const;
//   bool setDataFromTE(int isig, 
// 		     const HLT::TriggerElement* te, 
// 		     HLT::Navigation* navitool,int level);
  
protected:
  const TrigMissingET* mL2MissingET;
  const TrigMissingET* mEFMissingET;

};

#endif // __MissingETObjectsInRoI_h__
