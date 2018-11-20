/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigVKalFitter/TrigVKalFitter.h"
#include "TrigVKalFitter/VKalVrtAtlas.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
//-------------------------------------------------
#include "CLHEP/Units/SystemOfUnits.h"
#include<iostream>


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
                                    double &bx, double &by, double &bz) const
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
  }

  void TrigVKalFitter::setAthenaField(const double  Field)
  {
     m_fitField->setAtlasMag( Field );
  }
