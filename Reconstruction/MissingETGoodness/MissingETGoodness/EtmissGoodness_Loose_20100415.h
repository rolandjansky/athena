/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MissingETGoodness_EtmissGoodness_Loose_20100415_H__
#define __MissingETGoodness_EtmissGoodness_Loose_20100415_H__

#include "MissingETGoodness/TSelectionSet.h"

namespace MET {
  class EtmissGoodness_Loose_20100415 : public TSelectionSet
  {
   public:
    EtmissGoodness_Loose_20100415();
    virtual ~EtmissGoodness_Loose_20100415() {};
  
    Bool_t CreateSelection();
  
   protected:

    ClassDef(EtmissGoodness_Loose_20100415,0)
  };
}

#endif

