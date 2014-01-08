/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyBlineStore_H
#define MboyBlineStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MboyBline.h"

  /**
   @class MboyBlineStore

   This class is for managing a B line collection

  @author samusog@cern.ch
  
  */

class MboyBlineStore{
public:
    MboyBlineStore();
    virtual ~MboyBlineStore();

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
   void Add(MboyBline ToBeAdded);

   /**Get number of lines */
   int NberOfObjects() const;

   /**Get a line */
   const MboyBline* GetMboyBline(int MboyBlineNber) const;

   /**Set Activity*/
   void SetActive(int Active_i);
      
   /**Get Activity*/
   int GetActive();
      
   /**Sort the Store*/
   void Sort();
      
   /**Get Data*/
   int  getData(int JTYP,int JFF,int JZZ);
      
private:
///////////////////////////////////
//Data

   std::vector<MboyBline>  MboyBlineSet ; //!< Storage vector for line

   int m_Active ;

};


#endif
