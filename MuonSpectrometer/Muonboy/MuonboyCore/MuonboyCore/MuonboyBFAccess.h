/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyBFAccess_H
#define MuonboyBFAccess_H


#include "MuonboyCore/MuonboyCoreMisc.h"

class MuonboyBFAbstract;

  /**
   @class MuonboyBFAccess

   this class manages connection to B Field

  @author samusog@cern.ch
  
  */

class MuonboyBFAccess{
public:
   MuonboyBFAccess();
  ~MuonboyBFAccess();

public:
///////////////////////////////////

   /**Get B Field connection */
   static MuonboyBFAbstract* GetMuonboyBFAbstract();

   /**Set B Field connection */
   static void SetMuonboyBFAbstract(MuonboyBFAbstract* pMuonboyBFAbstract);

   /**Reset B Field connection */
   static void ResetMuonboyBFAbstract();

   /**Get field */
   static void field(float* XYZ,float* BXYZ) ;

   /**Get field and gradient*/
   static void fieldd(float* XYZ,float* BXYZ) ;

   /**Get field */
   static void field_tesla_cm(float* XYZ,float* BXYZ) ;

   /**Get field and gradient*/
   static void fieldGradient_tesla_cm(float* XYZ,float* BXYZ) ;

private:
///////////////////////////////////

   static MuonboyBFAbstract* p_MuonboyBFAbstract ; //!< Pointer on MuonboyBFAbstract

};
#endif

