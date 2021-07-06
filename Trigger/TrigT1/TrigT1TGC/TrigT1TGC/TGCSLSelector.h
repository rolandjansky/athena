/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCSLSelector_h
#define TrigT1TGC_TGCSLSelector_h

#include <memory>

namespace LVL1TGCTrigger {

class TGCSectorLogic;
class TGCSLSelectorOut;
class TGCSLPreSelectorOut;

class TGCSLSelector
{
 public:
  TGCSLSelector(const TGCSectorLogic* sL=0); 
  virtual ~TGCSLSelector();

  bool select(TGCSLPreSelectorOut* in, std::shared_ptr<TGCSLSelectorOut> out);

 private:
  const TGCSectorLogic* m_sectorLogic;
};


}   // end of namespace

#endif
