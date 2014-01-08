/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigitCSC_H
#define MuonRecDigitCSC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MuonRecDigit.h"
#include "MboyCoreEvt/MbIDTagCSC.h"

  /**
   @class MuonRecDigitCSC

   This class is for managing a CSC strip digit data

  @author samusog@cern.ch
  
  */

class MuonRecDigitCSC: public MuonRecDigit, public MbIDTagCSC{
public:
    MuonRecDigitCSC();
    virtual ~MuonRecDigitCSC();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set Associated deposited charge  */
   void SetDepositedCharge(const double&      DepositedCharge);

   /**Get Associated deposited charge  */
   double      GetDepositedCharge() const ;

private:
///////////////////////////////////

 double m_DepositedCharge ; //!< Associated deposited charge
 
};

#endif
