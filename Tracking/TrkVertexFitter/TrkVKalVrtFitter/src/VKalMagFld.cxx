/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//  AtlasMagFld object inherits from Trk::baseMagFld.  So pointer
//  AtlasMagFld*  is transferred to VKalVrtCore through VKalVrtControlBase object
//  Then this field is used in CORE for fitting
//
//
//  MAGNETIC FIELD IS ALWAYS GIVEN AT GLOBAL REFERENCE FRAME !!!
//    ref.frame origin (state.m_refFrameX, state.m_refFrameY, state.m_refFrameZ) is always added to coordinates
//    supplied to getMagFld().
//    default of ref.frame origin is (0,0,0) but may be changed if input tracks have different frame.
// 
//
//  ATHENA  object "m_fitField" of AtlasMagFld type is created in VKalVrtFitter during init.
//  By default it gives constant magnetic field value of which may be changed through 
//  jobOption "baseMagneticField" during initialization.
//   Magnetic field at (0,0,0)
//       before 13.1.0   - 2.084 tesla
//       after  13.1.0   - 1.964 tesla
//       after  15.1.0   - 1.997 tesla
//  
//  Currently works with both TrackingMagField and AtlasMagField
//
//-----------------------------------------------------------------------------------------------

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
#include "CLHEP/Units/SystemOfUnits.h"
//-------------------------------------------------
#include<iostream>

namespace Trk{
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//                  ATLAS magnetic field access
  VKalAtlasMagFld::VKalAtlasMagFld():
     m_magFrameX(0.),
     m_magFrameY(0.),
     m_magFrameZ(0.)
     {}

  VKalAtlasMagFld::~VKalAtlasMagFld()= default;
//
//  Setting of parameters
//
  void VKalAtlasMagFld::setAtlasField(MagField::AtlasFieldCache * pnt )
  {   
     m_VKalAthenaField = pnt;
  }
     
  void VKalAtlasMagFld::setAtlasField(const double  field)
  {   m_FIXED_ATLAS_FIELD = field; }
  
  void VKalAtlasMagFld::setAtlasMagRefFrame( double x, double y, double z)
  {   m_magFrameX=x; m_magFrameY=y; m_magFrameZ=z; }
  
//
// Coordinates here are in mm. - default for TrkVKalVrtCore
//
  void VKalAtlasMagFld::getMagFld(const double x, const double y, const double z, 
                                    double &bx, double &by, double &bz) const
  {   
      double fieldXYZ[3];  double BField[3];
      fieldXYZ[0]= (x +m_magFrameX) * CLHEP::mm;
      fieldXYZ[1]= (y +m_magFrameY) * CLHEP::mm;
      fieldXYZ[2]= (z +m_magFrameZ) * CLHEP::mm;
      if( m_VKalAthenaField ) {
         m_VKalAthenaField->getField(fieldXYZ,BField); 
         bx = BField[0]/CLHEP::tesla;
         by = BField[1]/CLHEP::tesla;   // Field in TESLA!!!!
         bz = BField[2]/CLHEP::tesla;
//std::cout<<" Exact mag.="<<bz<<" at "<<fieldXYZ[0]<<", "<<fieldXYZ[1]<<", "<<fieldXYZ[2]<<", "<<'\n';
      }else{
         bx = 0.;
         by = 0.;
         bz = m_FIXED_ATLAS_FIELD;
//std::cout<<" Fix mag.="<<bz<<'\n';
       }
   }
}
