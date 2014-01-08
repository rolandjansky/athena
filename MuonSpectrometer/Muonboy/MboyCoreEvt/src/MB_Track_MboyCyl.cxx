/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MB_Track_MboyCyl.h"

MB_Track_MboyCyl::MB_Track_MboyCyl():MB_Track_Base(){

 m_ZHalfLengthOfCylinder = -9999.;
 m_RadiusOfCylinder      = -9999.;

}
MB_Track_MboyCyl::~MB_Track_MboyCyl(){}

void MB_Track_MboyCyl::SetMboyCyl(
      double ThetaOfCrossingPoint,
       double PhiOfCrossingPoint,
        double ThetaOfUnitVector,
	 double PhiOfUnitVector,
	  double InverseMomentum,
      double Cov11, double Cov21, double Cov22,
      double Cov31, double Cov32, double Cov33,
      double Cov41, double Cov42, double Cov43,
      double Cov44, double Cov51, double Cov52,
      double Cov53, double Cov54, double Cov55,
      double XCrossingPoint ,double YCrossingPoint ,double ZCrossingPoint,
      double UXUnitVector ,double UYUnitVector ,double UZUnitVector,
      double Momentum,
      double ZHalfLengthOfCylinder,double RadiusOfCylinder)
{

// Set Track parameters
   SetTrack(
            ThetaOfCrossingPoint,
             PhiOfCrossingPoint,
              ThetaOfUnitVector,
      	       PhiOfUnitVector,
	        InverseMomentum,
            Cov11, Cov21, Cov22,
            Cov31, Cov32, Cov33,
            Cov41, Cov42, Cov43,
            Cov44, Cov51, Cov52,
            Cov53, Cov54, Cov55,
            XCrossingPoint ,YCrossingPoint ,ZCrossingPoint,
            UXUnitVector ,UYUnitVector ,UZUnitVector,
            Momentum);

// Cylinder Dimensions
   m_ZHalfLengthOfCylinder = ZHalfLengthOfCylinder;
   m_RadiusOfCylinder      = RadiusOfCylinder     ;

}

// Get
// Tracks Parameters
double MB_Track_MboyCyl::ThetaOfCrossingPoint() const {return m_P1;}
double MB_Track_MboyCyl::PhiOfCrossingPoint()   const {return m_P2;}
double MB_Track_MboyCyl::ThetaOfUnitVector()    const {return m_P3;}
double MB_Track_MboyCyl::PhiOfUnitVector()      const {return m_P4;}
double MB_Track_MboyCyl::InverseMomentum()      const {return m_P5;}

// Cylinder Dimensions
double MB_Track_MboyCyl::ZHalfLengthOfCylinder() const {return m_ZHalfLengthOfCylinder;}
double MB_Track_MboyCyl::RadiusOfCylinder()      const {return m_RadiusOfCylinder;}
