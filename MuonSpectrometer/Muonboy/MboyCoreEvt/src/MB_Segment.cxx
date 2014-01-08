/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MB_Segment.h"

MB_Segment::MB_Segment(){  
 
   m_FittedT0  = 0. ;
   m_eFittedT0 = 0. ;
   
   m_Xpt = 0. ;
   m_Ypt = 0. ;
   m_Zpt = 0. ;

   m_Xvec = 0. ;
   m_Yvec = 0. ;
   m_Zvec = 0. ;

   m_X_dx = 0. ;
   m_Y_dx = 0. ;
   m_Z_dx = 0. ;
   m_X_dy = 0. ;
   m_Y_dy = 0. ;
   m_Z_dy = 0. ;

   m_E_Valid = 0  ; 
   m_E_dx    = 0. ;
   m_E_Axz   = 0. ;
   m_E_dxAxz = 0. ;
   m_E_dy    = 0. ;
   m_E_Ayz   = 0. ;
   m_E_dyAyz = 0. ;

   m_QualityFactor = 0. ;

   m_KounterMultiLayer = 0 ;

   m_TrackSharing = 0 ;

   m_StationName     =" XXX" ;
   m_ItypStationName = 0 ;

   m_isStrict = false ;
   
}
MB_Segment::MB_Segment(
      double Xpt,double Ypt,double Zpt,
      double Xvec,double Yvec,double Zvec,
      double X_dx,double Y_dx,double Z_dx,
      double X_dy,double Y_dy,double Z_dy,
      int E_Valid,
      double E_dx,double E_Axz,double E_dxAxz,
      double E_dy,double E_Ayz,double E_dyAyz,
      double QualityFactor,
      int NberOfMultiLayer,
      int TrackSharing,
      std::string StationName,
      int         ItypStationName,
      double FittedT0  ,
      double eFittedT0 ,
      MbIOObjectStore&  aMbIOObjectStore,
      bool isStrict
      
):AccessorToMbIOObjectStore(aMbIOObjectStore), m_isStrict(isStrict){

// Coordinates of Fitted T0 and error
    m_FittedT0  = FittedT0  ;
    m_eFittedT0 = eFittedT0 ;

// Coordinates of the Point
    m_Xpt  = Xpt;
    m_Ypt  = Ypt;
    m_Zpt  = Zpt;

//  Coordinates of the Vector
    m_Xvec = Xvec;
    m_Yvec = Yvec;
    m_Zvec = Zvec;

// Coordinates of the dx and dy Vectors
    m_X_dx = X_dx ;
    m_Y_dx = Y_dx ;
    m_Z_dx = Z_dx ;
    m_X_dy = X_dy ;
    m_Y_dy = Y_dy ;
    m_Z_dy = Z_dy ;

// Errors components
    m_E_Valid	   = E_Valid	  ;
    m_E_dx	   = E_dx	  ;
    m_E_Axz   = E_Axz   ;
    m_E_dxAxz = E_dxAxz ;
    m_E_dy	   = E_dy	  ;
    m_E_Ayz   = E_Ayz   ;
    m_E_dyAyz = E_dyAyz ;

// Quality factor
    m_QualityFactor = QualityFactor;

// Nber of multilayers
    m_KounterMultiLayer = NberOfMultiLayer;

// TrackSharing
    m_TrackSharing = TrackSharing;

// Station Name
   m_StationName     = StationName;
   m_ItypStationName = ItypStationName;

}

MB_Segment::~MB_Segment(){}

// Get Get Fitted T0 and error
void MB_Segment::GetFittedT0(double & FittedT0,double & eFittedT0 ) const{

  FittedT0  = m_FittedT0  ;
  eFittedT0 = m_eFittedT0 ;

}

// Get Segment Point
void MB_Segment::GetPoint(double & Xpt,double & Ypt,double & Zpt) const{

  Xpt = m_Xpt;
  Ypt = m_Ypt;
  Zpt = m_Zpt;

}

// Get Segment Direction
void MB_Segment::GetDirection(double & Xvec,double & Yvec,double & Zvec) const{

  Xvec = m_Xvec;
  Yvec = m_Yvec;
  Zvec = m_Zvec;

}

// Get dx and dy vectors
void MB_Segment::Getdx(double & X_dx,double & Y_dx,double & Z_dx) const{

  X_dx = m_X_dx ;
  Y_dx = m_Y_dx ;
  Z_dx = m_Z_dx ;

}
void MB_Segment::Getdy(double & X_dy,double & Y_dy,double & Z_dy) const{

  X_dy = m_X_dy ;
  Y_dy = m_Y_dy ;
  Z_dy = m_Z_dy ;

}
// Get errors components
int MB_Segment::ErrorValidity() const{ return m_E_Valid;}
double MB_Segment::Wij(int i, int j)const{

  int i2 = i ; int j2 = j ;
  if (i < j ) {
   i2 = j ; j2 = i ;
  }
  i = i2 ; j = j2 ;
  if ( i == 1 && j == 1 ) return m_E_dx;
  if ( i == 2 && j == 1 ) return m_E_dxAxz;
  if ( i == 2 && j == 2 ) return m_E_Axz;
  if ( i == 3 && j == 3 ) return m_E_dy;
  if ( i == 4 && j == 3 ) return m_E_dyAyz;
  if ( i == 4 && j == 4 ) return m_E_Ayz;
  return 0. ;

}
void MB_Segment::GetErrorsComponents(
    int & E_Valid ,
    double & E_dx, double & E_Axz , double & E_dxAxz,
    double & E_dy, double & E_Ayz , double & E_dyAyz) const{

  E_Valid = m_E_Valid ;
  E_dx    = m_E_dx    ;
  E_Axz   = m_E_Axz   ;
  E_dxAxz = m_E_dxAxz ;
  E_dy    = m_E_dy    ;
  E_Ayz   = m_E_Ayz   ;
  E_dyAyz = m_E_dyAyz ;

}

// Get Segment Quality Factor
double MB_Segment::GetQualityFactor() const{ return m_QualityFactor; }

// Get Nber of Multilayers
int MB_Segment::GetNberOfMultiLayer() const{ return m_KounterMultiLayer; }

// Get TrackSharing
int MB_Segment::GetTrackSharing() const{ return m_TrackSharing; }

// Get Station Name
std::string MB_Segment::GetStationName() const{ return m_StationName; }
int MB_Segment::GetItypStationName() const{ return m_ItypStationName; }

// Get Nber of precise coordinate Digis 
int MB_Segment::GetNberOfDigiCSCPrecise () const{
 int NberOfCSCPrecise = 0 ;
 int NberOfTotal = GetNberOfElements();
 for (int DigitNber=0; DigitNber<NberOfTotal; DigitNber++){
   const MbROTCSC*    pDiOutCSC    = LiIdGetMbROTCSC    (DigitNber);
   if (pDiOutCSC){
     int ItMeasuresPhi = pDiOutCSC->GetSGMeasuresPhi();
     if (ItMeasuresPhi == 0)
     NberOfCSCPrecise    = NberOfCSCPrecise    + 1 ;
   }
   const MbROTCSCclu*    pDiOutCSCclu    = LiIdGetMbROTCSCclu    (DigitNber);
   if (pDiOutCSCclu){
     int ItMeasuresPhi = pDiOutCSCclu->GetSGMeasuresPhi();
     if (ItMeasuresPhi == 0)
     NberOfCSCPrecise    = NberOfCSCPrecise    + 1 ;
   }
 }
 return NberOfCSCPrecise ;
}
