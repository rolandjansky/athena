/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCCORE_AMDCBLINE_H
#define AMDCCORE_AMDCBLINE_H

#include <string>
#include <iostream>

  /**
   @class AmdcBline

   This class is for managing a B line data

  @author samusog@cern.ch
  
  */

class AmdcBline{
public:
   AmdcBline();
   ~AmdcBline();

public:
///////////////////////////////////
 
   /**Prints */
   void Print(std::ostream* out) const;
   void Print()                  const;
   void SuperPrint(std::ostream* out) const;
   void SuperPrint()                  const;

   /**Set station name */
   void SetStationType(const std::string& StationType );

   /**Get station name Nber*/
   int         Getjtyp       () const ;

   /**Set jff index */
   void Setjff        (int	   jff         );

   /**Set jzz index */
   void Setjzz        (int	   jzz         );

   /**Set job index */
   void Setjob        (int	   job         );

   /**Get station name */
   std::string GetStationType() const ;

   /**Get jff index */
   int         Getjff	     () const ;

   /**Get jzz index */
   int         Getjzz	     () const ;

   /**Get job index */
   int         Getjob	     () const ;

   /**Get station name as character array*/
   void        GetStationNameChar (char* NameI) const ;

   /**Set bz parameter*/
   void Setbz (double bz);

   /**Set bp parameter*/
   void Setbp (double bp);

   /**Set bn parameter*/
   void Setbn (double bn);

   /**Set sp parameter*/
   void Setsp (double sp);

   /**Set sn parameter*/
   void Setsn (double sn);

   /**Set tw parameter*/
   void Settw (double tw);

   /**Set pg parameter*/
   void Setpg (double pg);

   /**Set tr parameter*/
   void Settr (double tr);

   /**Set eg parameter*/
   void Seteg (double eg);

   /**Set ep parameter*/
   void Setep (double ep);

   /**Set en parameter*/
   void Seten (double en);

   /**Get bz parameter*/
   double Getbz() const ;

   /**Get bp parameter*/
   double Getbp() const ;

   /**Get bn parameter*/
   double Getbn() const ;

   /**Get sp parameter*/
   double Getsp() const ;

   /**Get sn parameter*/
   double Getsn() const ;

   /**Get tw parameter*/
   double Gettw() const ;

   /**Get pg parameter*/
   double Getpg() const ;

   /**Get tr parameter*/
   double Gettr() const ;

   /**Get eg parameter*/
   double Geteg() const ;

   /**Get ep parameter*/
   double Getep() const ;

   /**Get en parameter*/
   double Geten() const ;

private:
///////////////////////////////////
  std::string m_StationType ; //!< station name
  int         m_jff         ; //!< jff index
  int         m_jzz         ; //!< jzz index
  int         m_job         ; //!< job index

  double m_bz; //!< tube bow in plane
  double m_bp; //!< tube bow out of plane
  double m_bn; //!< tube bow out of plane
  double m_sp; //!< cross plate sag out of plane
  double m_sn; //!< cross plate sag out of plane
  double m_tw; //!< twist
  double m_pg; //!< parallelogram
  double m_tr; //!< trapezoid
  double m_eg; //!< global expansion
  double m_ep; //!< local expansion
  double m_en; //!< local expansion

  char m_StationNameChar[3]{} ; //!< station name as character array

};
#endif
