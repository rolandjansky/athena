/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyBFAbstract_H
#define MuonboyBFAbstract_H


#include "MboyCoreEvt/MboyCoreEvtMisc.h"

  /**
   @class MuonboyBFAbstract

   This class is the base class to connect to B Field services

  @author samusog@cern.ch
  
  */

class MuonboyBFAbstract{
public:
   MuonboyBFAbstract();
   virtual ~MuonboyBFAbstract();

public:
///////////////////////////////////

   /**Get field */
   virtual void field(float* XYZ,float* BXYZ) = 0 ;

   /**Get field and gradient*/
   virtual void fieldd(float* XYZ,float* BXYZ) = 0 ;

   /**Get field */
   virtual void field_tesla_cm(float* XYZ,float* BXYZ) = 0 ;

   /**Get field and gradient*/
   virtual void fieldGradient_tesla_cm(float* XYZ,float* BXYZ) = 0 ;

};
#endif

