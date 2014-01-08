/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyBFAth_H
#define MuonboyBFAth_H

#include "MboyCoreEvt/MuonboyBFAbstract.h"

class AbstractMagneticField;

  /**
   @class MuonboyBFAth

   This class is insuring connection to what matters in standalone mode for field. 
   It derives from MuonboyBFAbstract
   
  @author samusog@cern.ch
  
  */
class MuonboyBFStand : public MuonboyBFAbstract{
public:
   MuonboyBFStand(AbstractMagneticField* mag);
   virtual ~MuonboyBFStand();

public:
///////////////////////////////////

   /**Get field */
   virtual void field(float* XYZ,float* BXYZ) ;

   /**Get field and gradient*/
   virtual void fieldd(float* XYZ,float* BXYZ) ;

   /**Get field */
   virtual void field_tesla_cm(float* XYZ,float* BXYZ) ;

   /**Get field and gradient*/
   virtual void fieldGradient_tesla_cm(float* XYZ,float* BXYZ) ;


private:
///////////////////////////////////
  AbstractMagneticField* m_mag_field;

};
#endif
