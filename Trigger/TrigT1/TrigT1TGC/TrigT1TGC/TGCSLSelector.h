/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSLSelector_hh
#define TGCSLSelector_hh

namespace LVL1TGCTrigger {

class TGCSectorLogic;
class TGCSLSelectorOut;
class TGCSLPreSelectorOut;

class TGCSLSelector {
public:

  TGCSLSelector(const TGCSectorLogic* sL=0); 
  virtual ~TGCSLSelector(){} 

  TGCSLSelectorOut* select(TGCSLPreSelectorOut* PSOut, TGCSLSelectorOut* out);

private:

  const TGCSectorLogic* m_sectorLogic;

};


} //end of namespace bracket

#endif // TGCSLSelector_hh
