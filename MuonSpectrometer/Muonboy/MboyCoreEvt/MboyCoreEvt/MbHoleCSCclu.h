/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbHoleCSCclu_H
#define MbHoleCSCclu_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbHole.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"

  /**
   @class MbHoleCSCclu

   This class is for managing a CSC cluster hole

  @author samusog@cern.ch
  
  */

class MbHoleCSCclu: public MbHole, public MbIDTagCSCclu{
public:
    MbHoleCSCclu();
    virtual ~MbHoleCSCclu();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;
   virtual void PrintCout()               const ;

};

#endif
