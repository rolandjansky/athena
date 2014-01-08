/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbROTCSCclu_H
#define MbROTCSCclu_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbROT.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"
#include "MboyCoreEvt/MuonRecDigitCSCClu.h"

  /**
   @class MbROTCSCclu

   This class is for managing a CSC cluster digit-rec data

  @author samusog@cern.ch
  
  */

class MbROTCSCclu: public MbROT, public MbIDTagCSCclu{
public:
    MbROTCSCclu();
    virtual ~MbROTCSCclu();

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
   void SetMuonRecDigit(const MuonRecDigitCSCClu& MuonRecDigitObj) ;

   /**Get the digit */
   const MuonRecDigitCSCClu* GetMuonRecDigit() const;

   /** Set R from simulation */
   void SetRsim(double Rsim) ;

   /** Get R from simulation */
   double GetRsim() const;

   /** Set error from fit */
   void SetEfit(double Efit) ;

   /** Get error from fit */
   double GetEfit() const;

   /** Set R from cluster */
   void SetRdigi(double Rdigi);

   /** Get R from cluster */
   double GetRdigi() const;

   /** Set R from track */
   void SetRtrak(double Rtrak);

   /** Get R from track */
   double GetRtrak() const;

private:
///////////////////////////////////

 MuonRecDigitCSCClu m_MuonRecDigitObj ; //!< Digit Object

 double m_Rsim ; //!< R from simulation
 double m_Efit ; //!< R from fit
 double m_Rdigi; //!< R from cluster
 double m_Rtrak; //!< R from track

};

#endif
