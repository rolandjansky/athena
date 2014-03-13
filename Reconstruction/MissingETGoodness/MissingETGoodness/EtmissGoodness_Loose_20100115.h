/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MissingETGoodness_EtmissGoodness_Loose_20100115_H__
#define __MissingETGoodness_EtmissGoodness_Loose_20100115_H__

#include "MissingETGoodness/TSelectionSet.h"

namespace MET {
  class EtmissGoodness_Loose_20100115 : public TSelectionSet
  {
   public:
    EtmissGoodness_Loose_20100115();
    virtual ~EtmissGoodness_Loose_20100115() {};
  
    Bool_t CreateSelection();
  
   protected:

    ClassDef(EtmissGoodness_Loose_20100115,0)
  };
}

#endif

