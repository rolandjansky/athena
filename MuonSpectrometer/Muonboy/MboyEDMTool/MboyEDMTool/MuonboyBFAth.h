/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyBFAth_H
#define MuonboyBFAth_H


#include "MboyEDMTool/MboyEDMToolMisc.h"

#include "MboyCoreEvt/MuonboyBFAbstract.h"

namespace MagField {
  class IMagFieldSvc;
}
  /**
   @class MuonboyBFAth

   This class is insuring connection to IMagFieldAthenaSvc. 
   It derives from MuonboyBFAbstract
   
  @author samusog@cern.ch
  
  */

class MuonboyBFAth: public MuonboyBFAbstract{
public:
   MuonboyBFAth();
   virtual ~MuonboyBFAth();

public:
///////////////////////////////////

   /**Set Ready */
   virtual void SetReady() ;

   /**Get field */
   virtual void field(float* XYZ,float* BXYZ) ;

   /**Get field and gradient*/
   virtual void fieldd(float* XYZ,float* BXYZ) ;

   /**Get field */
   virtual void field_tesla_cm(float* XYZ,float* BXYZ) ;

   /**Get field and gradient*/
   virtual void fieldGradient_tesla_cm(float* XYZ,float* BXYZ) ;

public:
///////////////////////////////////
// Data

  MagField::IMagFieldSvc*                m_fieldService;

};
#endif

