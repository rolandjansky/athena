/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigVKalFitter/TrigVKalFitter.h"
#include "TrigVKalFitter/VKalVrtAtlas.h"
//-------------------------------------------------
#include "CLHEP/Units/SystemOfUnits.h"
#include<iostream>

 namespace Trk {
 extern vkalPropagator  myPropagator;
}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//                  ATLAS magnetic field access
  TrigAtlasMagFld::TrigAtlasMagFld(){
     m_VKalAthenaField=0;
     m_FIXED_ATLAS_FIELD=1.997;
  }
  TrigAtlasMagFld::~TrigAtlasMagFld(){}
  void TrigAtlasMagFld::setAtlasMag( MagField::IMagFieldSvc *  pnt)
  {
     m_VKalAthenaField = pnt; 
  }
  void TrigAtlasMagFld::setAtlasMag(const double  field)
  {   m_FIXED_ATLAS_FIELD = field; }
  
  void TrigAtlasMagFld::getMagFld(const double x, const double y, const double z, 
                                    double &bx, double &by, double &bz)
  {   
      double fieldXYZ[3];  double BField[3];
      fieldXYZ[0]=x;
      fieldXYZ[1]=y;
      fieldXYZ[2]=z;
      if( m_VKalAthenaField != 0) {
         m_VKalAthenaField->getField(fieldXYZ,BField); 
         bx = BField[0]/CLHEP::tesla;
         by = BField[1]/CLHEP::tesla;   // Field in TESLA!!!!
         bz = BField[2]/CLHEP::tesla;
//std::cout<<" Exact mag"<<'\n';
      }else{
         bx = 0.;
         by = 0.;
         bz = m_FIXED_ATLAS_FIELD;
//std::cout<<" Fix mag"<<'\n';
       }
   }
//--------------------------------------------------------------------------
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//  Setting interface
  void TrigVKalFitter::setAthenaField(MagField::IMagFieldSvc*  pnt)
  {
     m_fitField->setAtlasMag(pnt);
     double Bx,By,Bz;
     m_fitField->getMagFld(0.,0.,0.,Bx,By,Bz);
     m_fitField->setAtlasMag(Bz);
     if(m_PropagatorType == 0) m_PropagatorType = 1;  // set up Runge-Kutta propagator from Core
  }

  void TrigVKalFitter::setAthenaField(const double  Field)
  {
     m_fitField->setAtlasMag( Field );
     if(m_PropagatorType == 1) m_PropagatorType = 0;  // set up constant field propagator if Runge-Kutta was
                                                      // used before. Otherwise use what is set.
  }
