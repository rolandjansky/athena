/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbHole_H
#define MbHole_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbPOT.h"


  /**
   @class MbHole

   This class is an absract class for managing a hole

  @author samusog@cern.ch
  
  */

class MbHole: public MbPOT{
public:
    MbHole();
    virtual ~MbHole();

};

#endif
