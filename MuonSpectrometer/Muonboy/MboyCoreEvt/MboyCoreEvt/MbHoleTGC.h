/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbHoleTGC_H
#define MbHoleTGC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbHole.h"
#include "MboyCoreEvt/MbIDTagTGC.h"

  /**
   @class MbHoleTGC

   This class is for managing a TGC hole

  @author samusog@cern.ch
  
  */

class MbHoleTGC: public MbHole, public MbIDTagTGC{
public:
    MbHoleTGC();
    virtual ~MbHoleTGC();
    
public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;
   virtual void PrintCout()               const ;

};

#endif
