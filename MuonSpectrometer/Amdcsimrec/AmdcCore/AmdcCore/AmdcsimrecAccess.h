/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

   /**Get Amdcsimrec interface*/
   static Amdcsimrec* GetAmdcsimrec();
};
#endif

