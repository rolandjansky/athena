/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MB_Track_Base.h"

MB_Track_Base::MB_Track_Base(){

// Validity flag
   m_IsValid = 0;

// Tracks Parameters
   m_P1 = -9999.;
   m_P2 = -9999.;
   m_P3 = -9999.;
   m_P4 = -9999.;
   m_P5 = -9999.;

// Covariance Matrix
   m_Cov11 = -9999.;  m_Cov21 = -9999.;  m_Cov22 = -9999.;
   m_Cov31 = -9999.;  m_Cov32 = -9999.;  m_Cov33 = -9999.;
   m_Cov41 = -9999.;  m_Cov42 = -9999.;  m_Cov43 = -9999.;
   m_Cov44 = -9999.;  m_Cov51 = -9999.;  m_Cov52 = -9999.;
   m_Cov53 = -9999.;  m_Cov54 = -9999.;  m_Cov55 = -9999.;

// Coordinates of the Point on the track
   m_XPointOnTrack = -9999. ;
   m_YPointOnTrack = -9999. ;
   m_ZPointOnTrack = -9999. ;

// Unit vector coordinates at the Point on the track
   m_UXPointOnTrack = -9999. ;
   m_UYPointOnTrack = -9999. ;
   m_UZPointOnTrack = -9999. ;

// Momentum at the Point on the track
   m_Momentum = -9999. ;

// To be Reversed flag
   m_IsToBeReversed = 0;

// Is StraightLine
   m_IsStraightLine = 0;

}

MB_Track_Base::~MB_Track_Base(){}

void MB_Track_Base::SetTrack(
      double P1,double P2,double P3,double P4,double P5,
      double Cov11, double Cov21, double Cov22,
      double Cov31, double Cov32, double Cov33,
      double Cov41, double Cov42, double Cov43,
      double Cov44, double Cov51, double Cov52,
      double Cov53, double Cov54, double Cov55,
      double XPointOnTrack ,double YPointOnTrack ,double ZPointOnTrack,
      double UXPointOnTrack ,double UYPointOnTrack ,double UZPointOnTrack,
      double Momentum
  ){

// Validity flag
   m_IsValid = 1;

// Tracks Parameters
   m_P1 = P1;
   m_P2 = P2;
   m_P3 = P3;
   m_P4 = P4;
   m_P5 = P5;

// Covariance Matrix
   m_Cov11 = Cov11;  m_Cov21 = Cov21;  m_Cov22 = Cov22;
   m_Cov31 = Cov31;  m_Cov32 = Cov32;  m_Cov33 = Cov33;
   m_Cov41 = Cov41;  m_Cov42 = Cov42;  m_Cov43 = Cov43;
   m_Cov44 = Cov44;  m_Cov51 = Cov51;  m_Cov52 = Cov52;
   m_Cov53 = Cov53;  m_Cov54 = Cov54;  m_Cov55 = Cov55;

// Coordinates of the Point on the track
   m_XPointOnTrack = XPointOnTrack ;
   m_YPointOnTrack = YPointOnTrack ;
   m_ZPointOnTrack = ZPointOnTrack ;

// Unit vector coordinates at the Point on the track
   m_UXPointOnTrack = UXPointOnTrack ;
   m_UYPointOnTrack = UYPointOnTrack ;
   m_UZPointOnTrack = UZPointOnTrack ;

// Momentum at the Point on the track
   m_Momentum = Momentum ;

}

double MB_Track_Base::Covij(int i, int j) const{

  int i2 = i ; int j2 = j ;
  if (i < j ) {
   i2 = j ; j2 = i ;
  }
  i = i2 ; j = j2 ;
  if ( i == 1 && j == 1 ) return m_Cov11;
  if ( i == 2 && j == 1 ) return m_Cov21;
  if ( i == 3 && j == 1 ) return m_Cov31;
  if ( i == 4 && j == 1 ) return m_Cov41;
  if ( i == 5 && j == 1 ) return m_Cov51;
  if ( i == 2 && j == 2 ) return m_Cov22;
  if ( i == 3 && j == 2 ) return m_Cov32;
  if ( i == 4 && j == 2 ) return m_Cov42;
  if ( i == 5 && j == 2 ) return m_Cov52;
  if ( i == 3 && j == 3 ) return m_Cov33;
  if ( i == 4 && j == 3 ) return m_Cov43;
  if ( i == 5 && j == 3 ) return m_Cov53;
  if ( i == 4 && j == 4 ) return m_Cov44;
  if ( i == 5 && j == 4 ) return m_Cov54;
  if ( i == 5 && j == 5 ) return m_Cov55;
  return 0 ;

}

int MB_Track_Base::IsValid() const{ return m_IsValid ;}

void MB_Track_Base::SetIsToBeReversed (int IsToBeReversed ) { m_IsToBeReversed  = IsToBeReversed ;}
int  MB_Track_Base::IsToBeReversed() const{ return m_IsToBeReversed ;}

void MB_Track_Base::SetIsStraightLine (int IsStraightLine ) { m_IsStraightLine  = IsStraightLine ;}
int  MB_Track_Base::IsStraightLine() const{ return m_IsStraightLine ;}

// Covariance Matrix
double MB_Track_Base::Cov11() const{ return m_Cov11; }
double MB_Track_Base::Cov21() const{ return m_Cov21; }
double MB_Track_Base::Cov22() const{ return m_Cov22; }
double MB_Track_Base::Cov31() const{ return m_Cov31; }
double MB_Track_Base::Cov32() const{ return m_Cov32; }
double MB_Track_Base::Cov33() const{ return m_Cov33; }
double MB_Track_Base::Cov41() const{ return m_Cov41; }
double MB_Track_Base::Cov42() const{ return m_Cov42; }
double MB_Track_Base::Cov43() const{ return m_Cov43; }
double MB_Track_Base::Cov44() const{ return m_Cov44; }
double MB_Track_Base::Cov51() const{ return m_Cov51; }
double MB_Track_Base::Cov52() const{ return m_Cov52; }
double MB_Track_Base::Cov53() const{ return m_Cov53; }
double MB_Track_Base::Cov54() const{ return m_Cov54; }
double MB_Track_Base::Cov55() const{ return m_Cov55; }

// Coordinates of the Crossing Point
double MB_Track_Base::XCrossingPoint() const {return m_XPointOnTrack;}
double MB_Track_Base::YCrossingPoint() const {return m_YPointOnTrack;}
double MB_Track_Base::ZCrossingPoint() const {return m_ZPointOnTrack;}

// Unit vector coordinates at the Crossing Point
double MB_Track_Base::XUnitVector() const {return m_UXPointOnTrack;}
double MB_Track_Base::YUnitVector() const {return m_UYPointOnTrack;}
double MB_Track_Base::ZUnitVector() const {return m_UZPointOnTrack;}

// Momentum at the Crossing Point
double MB_Track_Base::Momentum() const {return m_Momentum;}

