/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCore_VKalVrtBMag_H
#define _TrkVKalVrtCore_VKalVrtBMag_H
namespace Trk {

/* Structure for keeping magnetic field in the fitting point.          */
/* This field is used for mass, momentum and constraints calculation   */
/* in the package.                                                     */
/*  The field here is set BEFORE starting of propagation of all tracks */
/*  to the estimated vertex position on each iteration of the fit      */
/*     Used for error matrix calculation AFTER fit also!!!             */
/***********************************************************************/

  struct VKalVrtBMag{
    double bmag, bmagx, bmagy, bmagz;
  };




/* Now general magnetic field in any point definition*/
/*     Changeable at any moment!!!                   */
/*---------------------------------------------------*/

  typedef void (*addrMagHandler)(double,double,double, double& ,double& , double& );

  class baseMagFld {
  public:
     baseMagFld();
     virtual ~baseMagFld();
     virtual void getMagFld(const double,const double,const double,double&,double&,double&)=0;
  };


  class vkalMagFld {
  public:
     vkalMagFld();
    ~vkalMagFld();

     void getMagFld(const double,const double,const double,double&,double&,double&);
     void setMagHandler(addrMagHandler);
     void setMagHandler(baseMagFld*);
     double getCnvCst();
  
   private:
     double m_cnstBMAG;
     addrMagHandler m_functionHandler;
     double m_vkalCnvMagFld;
     baseMagFld*    m_objectHandler;   
     double m_saveXpos;
     double m_saveYpos;
     double m_saveZpos;
     double m_saveBX;
     double m_saveBY;
     double m_saveBZ;
     double m_mm;
   };

}
#endif
