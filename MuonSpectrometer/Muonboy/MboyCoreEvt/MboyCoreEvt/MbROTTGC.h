/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbROTTGC_H
#define MbROTTGC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbROT.h"
#include "MboyCoreEvt/MbIDTagTGC.h"
#include "MboyCoreEvt/MuonRecDigitTGC.h"

  /**
   @class MbROTTGC

   This class is for managing a TGC digit-rec data

  @author samusog@cern.ch
  
  */

class MbROTTGC: public MbROT, public MbIDTagTGC{
public:
    MbROTTGC();
    virtual ~MbROTTGC();

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
   void SetMuonRecDigit(const MuonRecDigitTGC& MuonRecDigitObj) ;

   /**Get the digit */
   const MuonRecDigitTGC* GetMuonRecDigit() const;

   /** Set R from simulation */
   void SetRSsim(double RSsim) ;

   /** Get R from simulation */
   double GetRSsim() const;

   /** Set error from fit */
   void SetESfit(double ESfit) ;

   /** Get error from fit */
   double GetESfit() const;

   /** Set R from strip */
   void SetRdigi(double Rdigi);

   /** Get R from strip */
   double GetRdigi() const;
   
private:
///////////////////////////////////

 MuonRecDigitTGC m_MuonRecDigitObj ; //!< Digit Object

 double m_RSsim ; //!< R from simulation
 double m_ESfit ; //!< R from fit
 double m_Rdigi ; //!< R from strip

};

#endif
