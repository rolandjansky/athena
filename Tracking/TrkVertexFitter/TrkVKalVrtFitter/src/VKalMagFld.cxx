/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  AtlasMagFld object inherits from Trk::baseMagFld.  So pointer
//  AtlasMagFld*  is transferred to VKalVrtCore through Trk::myMagFld.setMagHandler().
//  Then this field is used in CORE for fitting
//
//
//  MAGNETIC FIELD IS ALWAYS GIVEN AT GLOBAL REFERENCE FRAME !!!
//    ref.frame origin (m_refFrameX, m_refFrameY, m_refFrameZ) is always added to coordinates
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
//
//  ATHENA  object "m_fitRotatedField" of AtlasMagFld type is created in VKalVrtFitter during init.
//  This object is provided to VKalVrtCore when ROTATED reference frame ( Z axis is along magnetic field )
//  is used. It gives constant value (magnitude) at given point
//-----------------------------------------------------------------------------------------------

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
//-------------------------------------------------
#include<iostream>

 namespace Trk {
 extern vkalPropagator  myPropagator;
}

namespace Trk{
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//                  ATLAS magnetic field access
  VKalAtlasMagFld::VKalAtlasMagFld(): m_mm(1.) {
     m_VKalAthenaField=0; 
     m_FIXED_ATLAS_FIELD=1.997;
     m_magFrameX=0.;
     m_magFrameY=0.;
     m_magFrameZ=0.;
     m_saveXpos=-10000000000.;
     m_saveYpos=-20000000000.;
     m_saveZpos=-30000000000.;
     m_saveBX=0.;
     m_saveBY=0.;
     m_saveBZ=0.;
  }
  VKalAtlasMagFld::~VKalAtlasMagFld(){}
//
//  Setting of parameters
//
  void VKalAtlasMagFld::setAtlasMag(MagField::IMagFieldSvc*  pnt)
  {   m_VKalAthenaField = pnt;  }
     
  void VKalAtlasMagFld::setAtlasMag(const double  field)
  {   m_FIXED_ATLAS_FIELD = field; }
  
  void VKalAtlasMagFld::setAtlasMagRefFrame( double x, double y, double z)
  {   m_magFrameX=x; m_magFrameY=y; m_magFrameZ=z; }
  
//
// Coordinates here are in mm. - default for TrkVKalVrtCore
//
  void VKalAtlasMagFld::getMagFld(const double x, const double y, const double z, 
                                    double &bx, double &by, double &bz)
  {   
      double fieldXYZ[3];  double BField[3];
      fieldXYZ[0]= (x +m_magFrameX) *m_mm;
      fieldXYZ[1]= (y +m_magFrameY) *m_mm;
      fieldXYZ[2]= (z +m_magFrameZ) *m_mm;
      if( m_VKalAthenaField ) {
	 double Shift= (m_saveXpos-fieldXYZ[0])*(m_saveXpos-fieldXYZ[0])
	              +(m_saveYpos-fieldXYZ[1])*(m_saveYpos-fieldXYZ[1])
	              +(m_saveZpos-fieldXYZ[2])*(m_saveZpos-fieldXYZ[2]);
         if(Shift < -1.*1.*m_mm*m_mm){   //17.03.2010 VK no caching - mag.field is steplike
	   bx=m_saveBX;
	   by=m_saveBY;
	   bz=m_saveBZ;
         }else{	 	 
           m_VKalAthenaField->getField(fieldXYZ,BField); 
           bx = BField[0]/CLHEP::tesla;
           by = BField[1]/CLHEP::tesla;   // Field in TESLA!!!!
           bz = BField[2]/CLHEP::tesla;
//	     if(bz<0.2)bz=0.2;     //some protection  !!VK NO PROTECTION!!!
           m_saveXpos=fieldXYZ[0];
           m_saveYpos=fieldXYZ[1];
           m_saveZpos=fieldXYZ[2];
	   m_saveBX=bx;
	   m_saveBY=by;
	   m_saveBZ=bz;
         }
//std::cout<<" Exact mag="<<bz<<" at "<<fieldXYZ[0]<<", "<<fieldXYZ[1]<<", "<<fieldXYZ[2]<<", "<<'\n';
      }else{
         bx = 0.;
         by = 0.;
         bz = m_FIXED_ATLAS_FIELD;
//std::cout<<" Fix mag="<<bz<<'\n';
       }
   }
//--------------------------------------------------------------------------
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//  Setting interface
  void TrkVKalVrtFitter::setAthenaField(MagField::IMagFieldSvc * pnt)
  {
     m_fitField->setAtlasMag(pnt);
     //double Bx,By,Bz;                             //22.01.2009 - moved to SetFitOption due to callback
     //m_fitField->getMagFld(0.,0.,0.,Bx,By,Bz);
     //m_fitField->setAtlasMag(Bz);
     if(m_PropagatorType == 0) m_PropagatorType = 1;  // set up Runge-Kutta propagator from Core
     m_isFieldInitialized = true;   //  to signal end of mag.field init procedure 
  }


  void TrkVKalVrtFitter::setAthenaField(const double  Field)
  {
     m_fitField->setAtlasMag( Field );
     if(m_PropagatorType == 1) m_PropagatorType = 0;  // set up constant field propagator if Runge-Kutta was
                                                      // used before. Otherwise use what is set.
     m_isFieldInitialized = true;   //  to signal end of mag.field init procedure 
  }
}
