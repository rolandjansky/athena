/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCCORE_AMDCILINESTORE_H
#define AMDCCORE_AMDCILINESTORE_H

#include <vector>
#include <iostream>

#include "AmdcCore/AmdcIline.h"

  /**
   @class AmdcIlineStore

   This class is for managing a A line collection

  @author samusog@cern.ch
  
  */

class AmdcIlineStore{
public:
    AmdcIlineStore();
    virtual ~AmdcIlineStore();

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
   void Add(const AmdcIline& ToBeAdded);

   /**Get number of lines */
   int NberOfObjects() const;

   /**Get a line */
   const AmdcIline* GetAmdcIline(int AmdcIlineNber) const;
   AmdcIline* GetAmdcIlineForUpdate(int AmdcIlineNber);

   /**Set Activity*/
   void SetActive(int Active_i);
      
   /**Get Activity*/
   int GetActive();
      
   /**Sort the Store*/
   void Sort();
      
   /**Get Data*/
   int  getData(int JTYP,int JFF,int JZZ,int JOB,int JLAY);
   
private:
///////////////////////////////////
//Data

   std::vector<AmdcIline>  m_AmdcIlineSet ; //!< Storage vector for line

   int m_Active ;

};


#endif
