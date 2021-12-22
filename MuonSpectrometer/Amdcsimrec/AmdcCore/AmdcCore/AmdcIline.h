/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCCORE_AMDCILINE_H
#define AMDCCORE_AMDCILINE_H

#include <string>
#include <iostream>

  /**
   @class AmdcIline

   This class is for managing a I line data

  @author samusog@cern.ch
  
  */

class AmdcIline{
public:
   AmdcIline();
   ~AmdcIline();

public:
///////////////////////////////////
 
   /**Prints */
   void Print(std::ostream* out) const;
   void Print()                  const;
   void SuperPrint(std::ostream* out) const;
   void SuperPrint()                  const;

   /**Set station name */
   void SetStationType(const std::string& StationType );

   /**Set jff index */
   void Setjff(int jff  );

   /**Set jzz index */
   void Setjzz(int jzz  );

   /**Set job index */
   void Setjob(int job  );

   /**Set jlay index */
   void Setjlay(int jlay );

   /**Get station name */
   std::string GetStationType() const ;

   /**Get station name Nber*/
   int         Getjtyp () const ;

   /**Get jff index */
   int         Getjff  () const ;

   /**Get jzz index */
   int         Getjzz  () const ;

   /**Get job index */
   int         Getjob  () const ;

   /**Get jlay index */
   int         Getjlay () const ;

   /**Get station name as character array*/
   void        GetStationNameChar (char* NameI) const ;

   /**Set s    parameter */
   void Sets    (double s    );

   /**Set z    parameter */
   void Setz    (double z    );

   /**Set t    parameter */
   void Sett    (double t    );

   /**Set rotS parameter */
   void SetrotS (double rotS );

   /**Set rotZ parameter */
   void SetrotZ (double rotZ );

   /**Set rotT parameter */
   void SetrotT (double rotT );

   /**Get s    parameter */
   double Gets    () const ;

   /**Get z    parameter */
   double Getz    () const ;

   /**Get t    parameter */
   double Gett    () const ;

   /**Get rotS parameter */
   double GetrotS () const ;

   /**Get rotZ parameter */
   double GetrotZ () const ;

   /**Get rotT parameter */
   double GetrotT () const ;

private:
///////////////////////////////////
  std::string m_StationType ; //!< station name
  int         m_jff         ; //!< jff index
  int         m_jzz         ; //!< jzz index
  int         m_job         ; //!< job index
  int         m_jlay        ; //!< jlay index

  double m_s    ; //!< s    parameter
  double m_z    ; //!< z    parameter
  double m_t    ; //!< t    parameter
  double m_rotS ; //!< rotS parameter
  double m_rotZ ; //!< rotZ parameter
  double m_rotT ; //!< rotT parameter

  char m_StationNameChar[3]{} ; //!< station name as character array

};
#endif
