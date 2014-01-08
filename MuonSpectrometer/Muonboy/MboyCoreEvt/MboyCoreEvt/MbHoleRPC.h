/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbHoleRPC_H
#define MbHoleRPC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbHole.h"
#include "MboyCoreEvt/MbIDTagRPC.h"

  /**
   @class MbHoleRPC

   This class is for managing a RPC hole

  @author samusog@cern.ch
  
  */

class MbHoleRPC: public MbHole, public MbIDTagRPC{
public:
    MbHoleRPC();
    virtual ~MbHoleRPC();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;
   virtual void PrintCout()               const ;

};

#endif
