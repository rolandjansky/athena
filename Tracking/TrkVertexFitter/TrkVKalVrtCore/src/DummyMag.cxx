/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Create object vkalMagFld which containg magnetic field.
//  It accepts external handlers with magnetic field ( function or object)
//  and takes data from them. If they are absent - default constants 
//  ATLAS magnetic field (2.084 tesla) is used
//
//   Single object myMagFld of vkalMagFld type is created in CFit.cxx
//
//   A structure vkalvrtbmag of type VKalVrtBMag is also created in CFit.cxx
//  Purpose is keep magnetic field at point of linearisation of vertex fit.
//  It stays constant during one iteration and after fit convergence
//   (contrary to myMagFld which changes during any propagation)
//-------------------------------------------------------------------------
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include <iostream>

namespace Trk {

extern VKalVrtBMag vkalvrtbmag;

vkalMagFld::vkalMagFld() {
   m_cnstBMAG=1.996;                    /* Const mag. field in Tesla */
   m_functionHandler=0;
   m_objectHandler=0;

//   vkalCnvMagFld = 0.0029979246;   /* For GeV and cm and Tesla*/
   m_vkalCnvMagFld = 0.29979246;       /* For MeV and mm and Tesla*/

   vkalvrtbmag.bmag=1.996;            /* Initial mag. mield in Tesla */
   m_saveXpos=-10000000000.;
   m_saveYpos=-20000000000.;
   m_saveZpos=-30000000000.;
   m_saveBX=0.;
   m_saveBY=0.;
   m_saveBZ=0.;
   m_mm=1.;
}

vkalMagFld::~vkalMagFld() {}


baseMagFld::baseMagFld() {}
baseMagFld::~baseMagFld() {}

void vkalMagFld::setMagHandler(addrMagHandler addr) {
   m_functionHandler=addr;
   m_objectHandler=0;
}

void vkalMagFld::setMagHandler(baseMagFld* addr) {
   m_objectHandler=addr;
   m_functionHandler=0;
}


void vkalMagFld::getMagFld(const double X,const double Y,const double Z,
                           double& bx, double& by, double& bz)
{
     if ( m_functionHandler == 0 && m_objectHandler==0 ){
        bx=0.;
        by=0.;
        bz=m_cnstBMAG;
     }
     if ( m_functionHandler != 0){
        m_functionHandler(X,Y,Z,bx,by,bz);
     }else if ( m_objectHandler   != 0) {
        m_objectHandler->getMagFld(X,Y,Z,bx,by,bz);
     }
//   std::cout<<" Mag.field"<<bx<<", "<<by<<", "<<bz<<" Shift="<<Shift<<
//                   " obj="<<m_objectHandler<<" func="<<m_functionHandler<<'\n';
}

double vkalMagFld::getCnvCst() {
   return this->m_vkalCnvMagFld;
}



}

