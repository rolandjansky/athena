/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCCORE_AMDCBLINESTORE_H
#define AMDCCORE_AMDCBLINESTORE_H

#include "AmdcCore/AmdcBline.h"
#include <vector>
#include <iostream>

  /**
   @class AmdcBlineStore

   This class is for managing a B line collection

  @author samusog@cern.ch
  
  */

class AmdcBlineStore{
public:
    AmdcBlineStore();
    ~AmdcBlineStore();

public:
///////////////////////////////////

   /**Reset the Store */
   void Reset();

   /**Print summary */
   void PrintSummary() const;
   
   /**Print line data */
   void Print(std::ostream* out) const;
   void Print()                  const;

   /**Add a line */
   void Add(const AmdcBline& ToBeAdded);

   /**Get number of lines */
   int NberOfObjects() const;

   /**Get a line */
   const AmdcBline* GetAmdcBline(int AmdcBlineNber) const;
   AmdcBline* GetAmdcBlineForUpdate(int AmdcBlineNber);

   /**Set Activity*/
   void SetActive(int Active_i);
      
   /**Get Activity*/
   int GetActive();
      
   /**Sort the Store*/
   void Sort();
      
   /**Get Data*/
   int  getData(int JTYP,int JFF,int JZZ,int JOB);

private:
///////////////////////////////////
//Data

   std::vector<AmdcBline>  m_AmdcBlineSet ; //!< Storage vector for line

   int m_Active ;

};


#endif
