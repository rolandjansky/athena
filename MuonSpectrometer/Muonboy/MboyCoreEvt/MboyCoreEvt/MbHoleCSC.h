/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbHoleCSC_H
#define MbHoleCSC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbHole.h"
#include "MboyCoreEvt/MbIDTagCSC.h"

  /**
   @class MbHoleCSC

   This class is for managing a CSC strip hole

  @author samusog@cern.ch
  
  */

class MbHoleCSC: public MbHole, public MbIDTagCSC{
public:
    MbHoleCSC();
    virtual ~MbHoleCSC();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;
   virtual void PrintCout()               const ;

};

#endif
