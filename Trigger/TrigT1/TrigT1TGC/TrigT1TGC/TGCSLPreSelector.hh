/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSLPreSelector_hh
#define TGCSLPreSelector_hh

class TGCSectorLogic;
#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.hh"

namespace LVL1TGCTrigger {
class TGCRPhiCoincidenceOut;
class TGCSLPreSelectorOut;

class TGCSLPreSelector {
public:
  void input(TGCRPhiCoincidenceOut* rPhiOut);
  TGCSLPreSelectorOut* select();
  void dumpInput() const;
  void init();

  TGCSLPreSelector(const TGCSLPreSelector& right);
  TGCSLPreSelector& operator=(const TGCSLPreSelector& right);

  TGCSLPreSelector(const TGCSectorLogic* sL=0);
  ~TGCSLPreSelector();

private:
  enum {MaxNumberOfSubSectorCluster = 19}; 
  TGCRPhiCoincidenceOut* m_coincidenceOut[MaxNumberOfSubSectorCluster]; 

  const TGCSectorLogic* m_sectorLogic; 
};


} //end of namespace bracket

#endif // TGCSLPreSelector_hh
