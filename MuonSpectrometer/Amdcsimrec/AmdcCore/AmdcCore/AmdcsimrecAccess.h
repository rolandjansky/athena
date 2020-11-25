/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCCORE_AMDCSIMRECACCESS_H
#define AMDCCORE_AMDCSIMRECACCESS_H

#include "AmdcCore/Amdcsimrec.h"

  /**
   @class AmdcsimrecAccess

   This class provides a static access to a Amdcsimrec interface
   
  @author samusog@cern.ch
  
  */

class AmdcsimrecAccess{
public:
   AmdcsimrecAccess();
  ~AmdcsimrecAccess();

public:
///////////////////////////////////

   /**Get Amdcsimrec interface*/
   static Amdcsimrec* GetAmdcsimrec();

   /**Set Amdcsimrec interface*/
   static void SetAmdcsimrec(Amdcsimrec* pAmdcsimrec);

   /**Reset Amdcsimrec interface*/
   static void ResetAmdcsimrec();

private:
///////////////////////////////////
// Data

   static Amdcsimrec* s_Amdcsimrec ; //!< Static Pointer on a Amdcsimrec interface
   
};
#endif

