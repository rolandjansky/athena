/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigitCSCClu_H
#define MuonRecDigitCSCClu_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MuonRecDigit.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"

  /**
   @class MuonRecDigitCSCClu

   This class is for managing a CSC cluster digit data

  @author samusog@cern.ch
  
  */

class MuonRecDigitCSCClu: public MuonRecDigit, public MbIDTagCSCclu{
public:
    MuonRecDigitCSCClu();
    virtual ~MuonRecDigitCSCClu();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set Associated position error  */
   void SetError(const double&      Error);

   /**Get Associated position error  */
   double      GetError() const ;

   /**Set Associated Deposited Charge  */
   void SetDepositedCharge(const double&      DepositedCharge);

   /**Get Associated Deposited Charge  */
   double      GetDepositedCharge() const ;

   /**Set Associated Time  */
   void SetTime(const double&      Time);

   /**Get Associated Time  */
   double      GetTime() const ;

   /**Set Associated Status  */
   void SetStatus(const int&      Status);

   /**Get Associated Status  */
   int      GetStatus() const ;

   /**Set Associated TimeStatus  */
   void SetTimeStatus(const int&      TimeStatus);

   /**Get Associated TimeStatus  */
   int      GetTimeStatus() const ;

private:
///////////////////////////////////

 double m_Error           ; //!< Associated position error  

 double m_DepositedCharge ; //!< Associated Deposited Charge  

 double m_Time            ; //!< Associated Time  

 int    m_TimeStatus      ; //!< Associated Time Status  

 int    m_Status          ; //!< Associated Status  

};

#endif
