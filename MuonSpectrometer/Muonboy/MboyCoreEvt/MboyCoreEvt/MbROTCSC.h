/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbROTCSC_H
#define MbROTCSC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbROT.h"
#include "MboyCoreEvt/MbIDTagCSC.h"
#include "MboyCoreEvt/MuonRecDigitCSC.h"

  /**
   @class MbROTCSC

   This class is for managing a CSC digit-rec data

  @author samusog@cern.ch
  
  */

class MbROTCSC: public MbROT, public MbIDTagCSC{
public:
    MbROTCSC();
    virtual ~MbROTCSC();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;
   virtual void PrintCout()               const ;

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set the digit */
   void SetMuonRecDigit(const MuonRecDigitCSC& MuonRecDigitObj) ;

   /**Get the digit */
   const MuonRecDigitCSC* GetMuonRecDigit() const;

   /** Set R from simulation */
   void SetRZsim(double RZsim) ;

   /** Get R from simulation */
   double GetRZsim() const;

   /** Set error from fit */
   void SetEZfit(double EZfit) ;

   /** Get error from fit */
   double GetEZfit() const;

private:
///////////////////////////////////

 MuonRecDigitCSC m_MuonRecDigitObj ; //!< Digit Object

 double m_RZsim ; //!< R from simulation
 double m_EZfit ; //!< R from fit

};

#endif
