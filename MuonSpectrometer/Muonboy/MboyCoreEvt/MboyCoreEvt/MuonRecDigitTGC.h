/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigitTGC_H
#define MuonRecDigitTGC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MuonRecDigit.h"
#include "MboyCoreEvt/MbIDTagTGC.h"

  /**
   @class MuonRecDigitTGC

   This class is for managing a TGC digit data

  @author samusog@cern.ch
  
  */

class MuonRecDigitTGC: public MuonRecDigit, public MbIDTagTGC{
public:
    MuonRecDigitTGC();
    virtual ~MuonRecDigitTGC();
    
public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

};

#endif
