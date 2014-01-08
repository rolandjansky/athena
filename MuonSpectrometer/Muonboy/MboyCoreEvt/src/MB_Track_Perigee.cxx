/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MB_Track_Perigee.h"

MB_Track_Perigee::MB_Track_Perigee():MB_Track_Base(){}
MB_Track_Perigee::~MB_Track_Perigee(){}

void MB_Track_Perigee::SetTrackPerigee(
      double ImpactParameterAtPerigee,
       double ZAtPerigee,
        double PhiAtPerigee,
	 double CotgAtPerigee,
	  double InverseTransverseMomentum,
      double Cov11, double Cov21, double Cov22,
      double Cov31, double Cov32, double Cov33,
      double Cov41, double Cov42, double Cov43,
      double Cov44, double Cov51, double Cov52,
      double Cov53, double Cov54, double Cov55,
      double XPerigeePoint ,double YPerigeePoint ,double ZPerigeePoint,
      double UXUnitVector ,double UYUnitVector ,double UZUnitVector,
      double Momentum)
{

// Set Track parameters
   SetTrack(
            ImpactParameterAtPerigee,
             ZAtPerigee,
              PhiAtPerigee,
	       CotgAtPerigee,
	        InverseTransverseMomentum,
            Cov11, Cov21, Cov22,
            Cov31, Cov32, Cov33,
            Cov41, Cov42, Cov43,
            Cov44, Cov51, Cov52,
            Cov53, Cov54, Cov55,
            XPerigeePoint ,YPerigeePoint ,ZPerigeePoint,
            UXUnitVector ,UYUnitVector ,UZUnitVector,
            Momentum);

}

// Get
// Tracks Parameters
double MB_Track_Perigee::ImpactParameterAtPerigee()  const {return m_P1;}
double MB_Track_Perigee::ZAtPerigee()                const {return m_P2;}
double MB_Track_Perigee::PhiAtPerigee()              const {return m_P3;}
double MB_Track_Perigee::CotgAtPerigee()             const {return m_P4;}
double MB_Track_Perigee::InverseTransverseMomentum() const {return m_P5;}

// Coordinates of the Point on track at the Perigee
double MB_Track_Perigee::XPerigeePoint() const {return XCrossingPoint();}
double MB_Track_Perigee::YPerigeePoint() const {return YCrossingPoint();}
double MB_Track_Perigee::ZPerigeePoint() const {return ZCrossingPoint();}
