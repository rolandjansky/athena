/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MissingETGoodness_SelectionTemplate_H__
#define __MissingETGoodness_SelectionTemplate_H__

#include "MissingETGoodness/TSelectionSet.h"

namespace MET {
  class SelectionTemplate : public TSelectionSet
  {
   public:
    SelectionTemplate();
    virtual ~SelectionTemplate() {};
  
    Bool_t CreateSelection();
  
   protected:

    ClassDef(SelectionTemplate,0)
  };
}

#endif

