/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyIlineStore_H
#define MboyIlineStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MboyIline.h"

  /**
   @class MboyIlineStore

   This class is for managing a I line collection

  @author samusog@cern.ch
  
  */

class MboyIlineStore{
public:
    MboyIlineStore();
    virtual ~MboyIlineStore();

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
   void Add(MboyIline ToBeAdded);

   /**Get number of lines */
   int NberOfObjects() const;

   /**Get a line */
   const MboyIline* GetMboyIline(int MboyIlineNber) const;
   MboyIline* GetMboyIlineForUpdate(int MboyIlineNber) ;

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

   std::vector<MboyIline>  MboyIlineSet ; //!< Storage vector for line

   int m_Active ;

};


#endif
