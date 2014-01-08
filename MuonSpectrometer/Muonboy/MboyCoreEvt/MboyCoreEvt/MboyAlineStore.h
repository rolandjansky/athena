/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyAlineStore_H
#define MboyAlineStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MboyAline.h"

  /**
   @class MboyAlineStore

   This class is for managing a A line collection

  @author samusog@cern.ch
  
  */

class MboyAlineStore{
public:
    MboyAlineStore();
    virtual ~MboyAlineStore();

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
   void Add(MboyAline ToBeAdded);

   /**Get number of lines */
   int NberOfObjects() const;

   /**Get a line */
   const MboyAline* GetMboyAline(int MboyAlineNber) const;
   MboyAline* GetMboyAlineForUpdate(int MboyAlineNber) ;

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

   std::vector<MboyAline>  MboyAlineSet ; //!< Storage vector for line

   int m_Active ;

};


#endif
