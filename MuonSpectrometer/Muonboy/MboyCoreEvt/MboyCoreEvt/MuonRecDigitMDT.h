/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigitMDT_H
#define MuonRecDigitMDT_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MuonRecDigit.h"
#include "MboyCoreEvt/MbIDTagMDT.h"

  /**
   @class MuonRecDigitMDT

   This class is for managing a MDT digit data

  @author samusog@cern.ch
  
  */

class MuonRecDigitMDT: public MuonRecDigit, public MbIDTagMDT{
public:
    MuonRecDigitMDT();
    virtual ~MuonRecDigitMDT();

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set Associated TDC count  */
   void SetTDCcount       (const int&	      TDCcount	     );

   /**Get Associated TDC count  */
   int         GetTDCcount	  () const ; 

   /**Set Associated ADC count  */
   void SetADCcount       (const int&	      ADCcount	     );

   /**Get Associated ADC count  */
   int         GetADCcount	  () const ;

   /**Set Associated drift time   */
   void SetSignalDriftTime(const double& DriftTime);

   /**Get Associated drift time   */
   double      GetDriftTime  () const ;

   /**Set string for R vs S function   */
   void SetRsLine(std::string RsLine);

   /**Get string for R vs S function   */
   std::string GetRsLine     () const ;

   /**Set Associated Status  */
   void SetStatus       (const int&	      Status	     );

   /**Get Associated Status  */
   int         GetStatus	  () const ;

private:
///////////////////////////////////

 int          m_TDCcount  ; //!< Associated TDC count

 int          m_ADCcount  ; //!< Associated ADC count

 double       m_DriftTime ; //!< Associated drift time 

 std::string  m_RsLine    ; //!< string for R vs S function

 int          m_Status    ; //!< Associated Status

};

#endif
