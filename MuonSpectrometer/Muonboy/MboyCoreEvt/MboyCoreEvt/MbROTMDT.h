/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbROTMDT_H
#define MbROTMDT_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbROT.h"
#include "MboyCoreEvt/MbIDTagMDT.h"
#include "MboyCoreEvt/MuonRecDigitMDT.h"

  /**
   @class MbROTMDT

   This class is for managing a MDT digit-rec data

  @author samusog@cern.ch
  
  */

class MbROTMDT: public MbROT, public MbIDTagMDT{
public:
    MbROTMDT();
    virtual ~MbROTMDT();

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
   void SetMuonRecDigit(const MuonRecDigitMDT& MuonRecDigitObj) ;

   /**Get the digit */
   const MuonRecDigitMDT* GetMuonRecDigit() const;

   /** Set R from simulation */
   void SetRZsim(double RZsim) ;

   /** Get R from simulation */
   double GetRZsim() const;

   /** Set error from fit */
   void SetEZfit(double EZfit) ;

   /** Get error from fit */
   double GetEZfit() const;

   /** Set R from drift time */
   void SetRdigi(double Rdigi);

   /** Get R from drift time */
   double GetRdigi() const;

   /** Set R from track */
   void SetRtrak(double Rtrak);

   /** Get R from track */
   double GetRtrak() const;

private:
///////////////////////////////////

 MuonRecDigitMDT m_MuonRecDigitObj ; //!< Digit Object

 double m_RZsim ; //!< R from simulation
 double m_EZfit ; //!< R from fit
 double m_Rdigi ; //!< R from drift time
 double m_Rtrak ; //!< R from track

};

#endif
