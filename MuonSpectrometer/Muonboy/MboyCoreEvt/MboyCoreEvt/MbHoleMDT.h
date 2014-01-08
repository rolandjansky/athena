/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbHoleMDT_H
#define MbHoleMDT_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbHole.h"
#include "MboyCoreEvt/MbIDTagMDT.h"

  /**
   @class MbHoleMDT

   This class is for managing a MDT hole

  @author samusog@cern.ch
  
  */

class MbHoleMDT: public MbHole, public MbIDTagMDT{
public:
    MbHoleMDT();
    virtual ~MbHoleMDT();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;
   virtual void PrintCout()               const ;

};

#endif
