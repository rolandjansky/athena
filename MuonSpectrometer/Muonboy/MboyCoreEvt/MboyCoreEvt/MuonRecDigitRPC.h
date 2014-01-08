/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigitRPC_H
#define MuonRecDigitRPC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MuonRecDigit.h"
#include "MboyCoreEvt/MbIDTagRPC.h"

  /**
   @class MuonRecDigitRPC

   This class is for managing a RPC digit data

  @author samusog@cern.ch
  
  */

class MuonRecDigitRPC: public MuonRecDigit, public MbIDTagRPC{
public:
    MuonRecDigitRPC();
    virtual ~MuonRecDigitRPC();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set Associated time*/
   void SetRpcTime     (const double&	   RpcTime     );

   /**Get Associated time*/
   double      GetRpcTime         () const ;

private:
///////////////////////////////////

 double m_RpcTime ;//!< Associated time

};

#endif
