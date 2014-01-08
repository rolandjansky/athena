/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyRTAccess_H
#define MuonboyRTAccess_H


#include "MuonboyCore/MuonboyCoreMisc.h"

#include "MboyCoreEvt/MuonboyRTAbstract.h"

  /**
   @class MuonboyRTAccess

   this class manages connection to RT

  @author samusog@cern.ch
  
  */

class MuonboyRTAccess{
public:
   MuonboyRTAccess();
  ~MuonboyRTAccess();

public:
///////////////////////////////////

   /**Get RT connection */
   static MuonboyRTAbstract* GetMuonboyRTAbstract();

   /**Set RT connection */
   static void SetMuonboyRTAbstract(MuonboyRTAbstract* pMuonboyRTAbstract);

   /**Reset RT connection */
   static void ResetMuonboyRTAbstract();

private:
///////////////////////////////////

   static MuonboyRTAbstract* p_MuonboyRTAbstract ; //!< Pointer on MuonboyRTAbstract

};
#endif

