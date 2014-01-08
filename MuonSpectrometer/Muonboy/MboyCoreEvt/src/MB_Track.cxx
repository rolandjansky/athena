/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MB_Track.h"

MB_Track::MB_Track(){   
  m_QualityFactor = 0. ;  
  m_NberOfDOF     = 0  ; 
  m_Chi2Typ0      = 0. ; 
  m_Chi2Typ1      = 0. ; 
  m_Chi2Typ2      = 0. ; 

  m_enmsxx        = 0. ; 
}
MB_Track::MB_Track
       (const MB_Track_SpecEntr& SpectrometerEntrance,
        const MB_Track_CaloEntr& CalorimeterEntrance,
        const MB_Track_Perigee&  Perigee,
	                 double QualityFactor,
                         int    NberOfDOF ,
                         double Chi2Typ0 ,
                         double Chi2Typ1 ,
                         double Chi2Typ2 ,
			 MbIOObjectStore&  aMbIOObjectStore
):AccessorToMbIOObjectStore(aMbIOObjectStore){

  m_SpectrometerEntrance = SpectrometerEntrance ;
  m_CalorimeterEntrance  = CalorimeterEntrance  ;
  m_Perigee              = Perigee              ;

   m_QualityFactor = QualityFactor;
   m_NberOfDOF     = NberOfDOF;
   m_Chi2Typ0      = Chi2Typ0;
   m_Chi2Typ1      = Chi2Typ1;
   m_Chi2Typ2      = Chi2Typ2;

   m_enmsxx = 0 ;

}
MB_Track::~MB_Track(){}

// Get Tracks validities
int MB_Track::IsValidMS() const{ return m_SpectrometerEntrance.IsValid() ;}
int MB_Track::IsValidKA() const{ return m_CalorimeterEntrance.IsValid() ;}
int MB_Track::IsValidVT() const{ return m_Perigee.IsValid() ;}

int MB_Track::IsToBeReversed() const{ 

 if (IsValidVT() != 0 ) return m_Perigee.IsToBeReversed() ;
 if (IsValidKA() != 0 ) return m_CalorimeterEntrance.IsToBeReversed() ;
 if (IsValidMS() != 0 ) return m_SpectrometerEntrance.IsToBeReversed() ;
 return 0 ;

}
int MB_Track::IsStraightLine() const{ 

 if (IsValidVT() != 0 ) return m_Perigee.IsStraightLine() ;
 if (IsValidKA() != 0 ) return m_CalorimeterEntrance.IsStraightLine() ;
 if (IsValidMS() != 0 ) return m_SpectrometerEntrance.IsStraightLine() ;
 return 0 ;

}
// Get Tracks
MB_Track_SpecEntr MB_Track::GetTrackAtSpectrometerEntrance()  const{return m_SpectrometerEntrance;}
MB_Track_CaloEntr MB_Track::GetTrackAtCalorimeterEntrance()   const{return m_CalorimeterEntrance;}
MB_Track_Perigee  MB_Track::GetTrackAtPerigee()               const{return m_Perigee ;}

const MB_Track_SpecEntr* MB_Track::pGetTrackAtSpectrometerEntrance()  const{return &m_SpectrometerEntrance;}
const MB_Track_CaloEntr* MB_Track::pGetTrackAtCalorimeterEntrance()   const{return &m_CalorimeterEntrance;}
const MB_Track_Perigee*  MB_Track::pGetTrackAtPerigee()               const{return &m_Perigee ;}

// Get the Quality Factor
double MB_Track::GetQualityFactor()  const{return m_QualityFactor;}

// Get the Nber of d.o.f
int MB_Track::GetNberOfDOF()  const{return m_NberOfDOF;}

// Get the Chi2's
double MB_Track::GetChi2(int FitKind)  const{

  double ToBeReturned=0.;
  if (FitKind == 0) ToBeReturned = m_Chi2Typ0;
  if (FitKind == 1) ToBeReturned = m_Chi2Typ1;
  if (FitKind == 2) ToBeReturned = m_Chi2Typ2;
  return ToBeReturned;

}

void MB_Track::Setenmsxx(double enmsxx_i) { m_enmsxx = enmsxx_i ;}


// Get index of digits
double MB_Track::Getenmsxx()              const { return m_enmsxx            ;}
