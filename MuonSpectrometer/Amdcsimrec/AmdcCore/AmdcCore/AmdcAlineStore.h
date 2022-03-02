/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCCORE_AMDCALINESTORE_H
#define AMDCCORE_AMDCALINESTORE_H

#include "AmdcCore/AmdcAline.h"
#include <vector>
#include <iostream>

  /**
   @class AmdcAlineStore

   This class is for managing a A line collection

  @author samusog@cern.ch
  
  */

class AmdcAlineStore{
public:
    AmdcAlineStore();
    virtual ~AmdcAlineStore();

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
   void Add(const AmdcAline& ToBeAdded);

   /**Get number of lines */
   int NberOfObjects() const;

   /**Get a line */
   const AmdcAline* GetAmdcAline(int AmdcAlineNber) const;
   AmdcAline* GetAmdcAlineForUpdate(int AmdcAlineNber);

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

   std::vector<AmdcAline>  m_AmdcAlineSet ; //!< Storage vector for line

   int m_Active ;

};


#endif
