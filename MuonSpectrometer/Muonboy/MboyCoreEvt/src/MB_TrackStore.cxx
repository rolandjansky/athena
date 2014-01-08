/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MB_TrackStore.h"
#include <cmath>
MB_TrackStore::MB_TrackStore(){}

MB_TrackStore::~MB_TrackStore(){}

// Reset the Store
void MB_TrackStore::ResetMB_TrackStore(){MB_TrackSet.clear();}

// Add a MB_Track
void MB_TrackStore::Add(const MB_Track& ToBeAdded){MB_TrackSet.push_back(ToBeAdded);}

// Nber of MB_Track
int MB_TrackStore::NberOfMB_Track() const{return MB_TrackSet.size();}

// Get a Pointer on a MB_Track of Nber MB_TrackNber
const MB_Track* MB_TrackStore::GetMB_Track(int MB_TrackNber) const{
 const MB_Track* ToBeReturned = 0 ;
 if (MB_TrackNber < 0 || MB_TrackNber >= NberOfMB_Track()) return ToBeReturned;
 return &MB_TrackSet[MB_TrackNber];
}

// Print Out the Store
void MB_TrackStore::PrintSummary() const{

//*Print out Total Nber of MB_Track
   std::cout << "*  The Nber of MB_Track in the MB_TrackStore is "
             << NberOfMB_Track()
             << std::endl;

}

void MB_TrackStore::PrintTracks(
 std::ostream* out,
 int EventNber
) const{

  *out << "Event Nber: " << EventNber << std::endl;

//*Loop on all tracks
   int NberOfTracks = NberOfMB_Track();
   *out << "* There are "
        << std::setw(5)<< NberOfTracks
	<< " tracks "
	<< std::endl;
   for (int TrackNber=0; TrackNber<NberOfTracks; TrackNber++){

     const MB_Track*   TheTrack = GetMB_Track(TrackNber);
     const MB_Track_MboyCyl* pMB_Track_MboyCyl_MS = TheTrack->pGetTrackAtSpectrometerEntrance();
     const MB_Track_MboyCyl* pMB_Track_MboyCyl_KA = TheTrack->pGetTrackAtCalorimeterEntrance();
     const MB_Track_Perigee* pMB_Track_Perigee_VT = TheTrack->pGetTrackAtPerigee();


     *out << "*   For track nber "
          << std::setw(4) << TrackNber << std::endl;

     *out << "*    Eloss = "
          << std::setw(12)<<std::setprecision(5)
	  << pMB_Track_MboyCyl_KA->Momentum() - pMB_Track_MboyCyl_MS->Momentum()
	  << std::endl;

     *out << "*     Track validities at "
	  << std::endl;
     *out << "*     Muon System entrance:  " << std::setw(5)<< TheTrack->IsValidMS()
	  << std::endl;
     *out << "*     Calorimeter entrance:  " << std::setw(5)<< TheTrack->IsValidKA()
	  << std::endl;
     *out << "*     at vertex           :  " << std::setw(5)<< TheTrack->IsValidVT()
	  << std::endl;

     *out << "*     Track parameters at "
	  << std::endl;

     *out << "*     Muon System entrance:  "
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->ThetaOfCrossingPoint()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->PhiOfCrossingPoint()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->ThetaOfUnitVector()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->PhiOfUnitVector()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->InverseMomentum()
          <<  std::setw(12)<<std::setprecision(5)<<1./pMB_Track_MboyCyl_MS->InverseMomentum()
	  << std::endl;
     *out << "*                       +/-  "
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov11())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov22())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov33())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov44())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov55())
          <<  std::setw(12)<<std::setprecision(5)
	  <<sqrt(pMB_Track_MboyCyl_MS->Cov55())/std::pow(pMB_Track_MboyCyl_MS->InverseMomentum(),2)
	  << std::endl;

     *out << "*     Calorimeter entrance:  "
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->ThetaOfCrossingPoint()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->PhiOfCrossingPoint()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->ThetaOfUnitVector()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->PhiOfUnitVector()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->InverseMomentum()
          <<  std::setw(12)<<std::setprecision(5)<<1./pMB_Track_MboyCyl_KA->InverseMomentum()
	  << std::endl;
     *out << "*                       +/-  "
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov11())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov22())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov33())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov44())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov55())
          <<  std::setw(12)<<std::setprecision(5)
	  <<sqrt(pMB_Track_MboyCyl_KA->Cov55())/std::pow(pMB_Track_MboyCyl_KA->InverseMomentum(),2)
	  << std::endl;

     *out << "*     at vertex           :  "
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->ImpactParameterAtPerigee()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->ZAtPerigee()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->PhiAtPerigee()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->CotgAtPerigee()
          <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->InverseTransverseMomentum()
          <<  std::setw(12)<<std::setprecision(5)<<1./pMB_Track_Perigee_VT->InverseTransverseMomentum()
	  << std::endl;
     *out << "*                       +/-  "
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov11())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov22())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov33())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov44())
          <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov55())
	  << std::endl;

     *out << "*    Covariance matrice at "
	  << std::endl;

     *out << "*     Muon System entrance:  "
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,1)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(1,1))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,2)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(2,2))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,3)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(3,3))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,4)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(4,4))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,5)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(5,5))
	  << std::endl;
     for (int i1=2; i1<=5; i1++){
      *out << "*                            "
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,1)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(1,1))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,2)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(2,2))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,3)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(3,3))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,4)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(4,4))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,5)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(5,5))
	   << std::endl;
     }

     *out << "*     Calorimeter entrance:  "
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,1)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(1,1))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,2)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(2,2))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,3)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(3,3))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,4)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(4,4))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,5)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(5,5))
	  << std::endl;
     for (int i1=2; i1<=5; i1++){
      *out << "*                            "
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,1)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(1,1))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,2)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(2,2))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,3)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(3,3))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,4)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(4,4))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,5)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(5,5))
	   << std::endl;
     }

     *out << "*     at vertex           :  "
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,1)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(1,1))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,2)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(2,2))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,3)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(3,3))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,4)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(4,4))
          <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,5)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(5,5))
	  << std::endl;
     for (int i1=2; i1<=5; i1++){
      *out << "*                            "
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,1)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(1,1))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,2)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(2,2))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,3)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(3,3))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,4)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(4,4))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,5)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(5,5))
	   << std::endl;
     }

     *out << "*    PerigeePoint            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->XPerigeePoint()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->YPerigeePoint()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->ZPerigeePoint()
          << std::endl;
     *out << "*    Momentum                "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->XUnitVector()*pMB_Track_Perigee_VT->Momentum()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->YUnitVector()*pMB_Track_Perigee_VT->Momentum()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->ZUnitVector()*pMB_Track_Perigee_VT->Momentum()
          << std::endl;
     *out << "*    MuonSysEntrancePoint    "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->XCrossingPoint()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->YCrossingPoint()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->ZCrossingPoint()
          << std::endl;
     *out << "*    Momentum                "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->XUnitVector()*pMB_Track_MboyCyl_MS->Momentum()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->YUnitVector()*pMB_Track_MboyCyl_MS->Momentum()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->ZUnitVector()*pMB_Track_MboyCyl_MS->Momentum()
          << std::endl;
     
     *out << "*    CaloEntrancePoint       "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->XCrossingPoint()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->YCrossingPoint()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->ZCrossingPoint()
          << std::endl;
     *out << "*    Momentum                "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->XUnitVector()*pMB_Track_MboyCyl_KA->Momentum()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->YUnitVector()*pMB_Track_MboyCyl_KA->Momentum()
          << std::endl;
     *out << "*                            "
          <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->ZUnitVector()*pMB_Track_MboyCyl_KA->Momentum()
          << std::endl;
    
     *out << "*     List of Digits           :  "
	  << std::endl;

     int NberOfMDT    = TheTrack->GetNberOfDigiMDT   ();
     int NberOfRPC    = TheTrack->GetNberOfDigiRPC   ();
     int NberOfTGC    = TheTrack->GetNberOfDigiTGC   ();
     int NberOfCSC    = TheTrack->GetNberOfDigiCSC   ();
     int NberOfCSCclu = TheTrack->GetNberOfDigiCSCclu();
     int NberOfElements = TheTrack->GetNberOfElements();

     int NberOfMbMuSc    = TheTrack->GetNberOfMbMuSc   () ;

     int NberOfHoleMDT    = TheTrack->GetNberOfHoleMDT   () ;
     int NberOfHoleRPC    = TheTrack->GetNberOfHoleRPC   () ;
     int NberOfHoleTGC    = TheTrack->GetNberOfHoleTGC   () ;
     int NberOfHoleCSC    = TheTrack->GetNberOfHoleCSC   () ;
     int NberOfHoleCSCclu = TheTrack->GetNberOfHoleCSCclu() ;
     int NberOfDigi       = TheTrack->GetNberOfDigi() ;

     *out << "*      First Coordinate "
	  << std::endl;

     double Chi2Typ1 = TheTrack->GetChi2(1);
     double Xcrpoint_SpecEntr = pMB_Track_MboyCyl_MS->XCrossingPoint() ;
     double Ycrpoint_SpecEntr = pMB_Track_MboyCyl_MS->YCrossingPoint() ;
     double Zcrpoint_SpecEntr = pMB_Track_MboyCyl_MS->ZCrossingPoint() ;
     double Xunivect_SpecEntr = pMB_Track_MboyCyl_MS->XUnitVector()    ;
     double Yunivect_SpecEntr = pMB_Track_MboyCyl_MS->YUnitVector()    ;
     double Zunivect_SpecEntr = pMB_Track_MboyCyl_MS->ZUnitVector()    ;
     double Momentum_SpecEntr = pMB_Track_MboyCyl_MS->Momentum()       ;
     double Charge_SpecEntr   = 1.                       ;
     if (pMB_Track_MboyCyl_MS->InverseMomentum() <= 0. ) Charge_SpecEntr = -1. ;

     Xcrpoint_SpecEntr = Xcrpoint_SpecEntr/10.  ;
     Ycrpoint_SpecEntr = Ycrpoint_SpecEntr/10.  ;
     Zcrpoint_SpecEntr = Zcrpoint_SpecEntr/10.  ;
     Momentum_SpecEntr = Momentum_SpecEntr/1000.;

     *out << " Tra.------>"
          << std::setw(5)<< TrackNber+1
          << "     "
          << std::setw(5)<< NberOfMDT
          << std::setw(10)<<std::setprecision(4)<< Chi2Typ1
          << std::endl;
     *out << std::setw(5)<< NberOfMDT   
          << std::setw(5)<< NberOfRPC   
          << std::setw(5)<< NberOfTGC   
          << std::setw(5)<< NberOfCSC   
          << std::setw(5)<< NberOfCSCclu
          << std::endl;
     *out << std::setw(5)<< NberOfHoleMDT   
          << std::setw(5)<< NberOfHoleRPC   
          << std::setw(5)<< NberOfHoleTGC   
          << std::setw(5)<< NberOfHoleCSC   
          << std::setw(5)<< NberOfHoleCSCclu
          << std::setw(5)<< NberOfDigi      
          << std::endl;
     *out << std::setw(5)<< NberOfMbMuSc   
          << std::endl;

     *out << std::setw(12)<<std::setprecision(4)<< Xcrpoint_SpecEntr
          << std::setw(12)<<std::setprecision(4)<< Ycrpoint_SpecEntr
          << std::setw(12)<<std::setprecision(4)<< Zcrpoint_SpecEntr
          << std::setw(12)<<std::setprecision(6)<< Xunivect_SpecEntr
          << std::setw(12)<<std::setprecision(6)<< Yunivect_SpecEntr
          << std::setw(12)<<std::setprecision(6)<< Zunivect_SpecEntr
          << std::setw(12)<<std::setprecision(3)<< Momentum_SpecEntr
          << std::setw(12)<<std::setprecision(1)<< Charge_SpecEntr
          << std::endl;

     int kountMbROTMDT = 0 ;
     for (int ItemEls=0; ItemEls<NberOfElements; ItemEls++){
       const MbROTMDT*    pMbROTMDT    = TheTrack->LiIdGetMbROTMDT    (ItemEls);
       if (pMbROTMDT) {
         kountMbROTMDT =kountMbROTMDT  + 1 ;
         *out << std::setw(5)<< kountMbROTMDT ;
         pMbROTMDT->Print(out);
       }
     }

     int kountMbROTRPC = kountMbROTMDT ;
     for (int ItemEls=0; ItemEls<NberOfElements; ItemEls++){
       const MbROTRPC*    pMbROTRPC    = TheTrack->LiIdGetMbROTRPC    (ItemEls);
       if (pMbROTRPC) {
         kountMbROTRPC =kountMbROTRPC  + 1 ;
         *out << std::setw(5)<< kountMbROTRPC ;
         pMbROTRPC->Print(out);
       }
     }

     int kountMbROTTGC = kountMbROTRPC ;
     for (int ItemEls=0; ItemEls<NberOfElements; ItemEls++){
       const MbROTTGC*    pMbROTTGC    = TheTrack->LiIdGetMbROTTGC    (ItemEls);
       if (pMbROTTGC) {
         kountMbROTTGC =kountMbROTTGC  + 1 ;
         *out << std::setw(5)<< kountMbROTTGC ;
         pMbROTTGC->Print(out);
       }
     }
     
     int kountMbROTCSC = kountMbROTTGC ;
     for (int ItemEls=0; ItemEls<NberOfElements; ItemEls++){
       const MbROTCSC*    pMbROTCSC    = TheTrack->LiIdGetMbROTCSC    (ItemEls);
       if (pMbROTCSC) {
         kountMbROTCSC =kountMbROTCSC  + 1 ;
         *out << std::setw(5)<< kountMbROTCSC ;
         pMbROTCSC->Print(out);
       }
     }

     int kountMbROTCSCclu = kountMbROTCSC ;
     for (int ItemEls=0; ItemEls<NberOfElements; ItemEls++){
       const MbROTCSCclu* pMbROTCSCclu = TheTrack->LiIdGetMbROTCSCclu (ItemEls);
       if (pMbROTCSCclu) {
         kountMbROTCSCclu =kountMbROTCSCclu  + 1 ;
         *out << std::setw(5)<< kountMbROTCSCclu ;
         pMbROTCSCclu->Print(out);
       }
     }
   
  
  
    int kountMbHoleMDT = kountMbROTCSCclu ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleMDT* pMbHoleMDT = TheTrack->LiIdGetMbHoleMDT (ElementNber);
      if (pMbHoleMDT) {
        kountMbHoleMDT =kountMbHoleMDT  + 1 ;
        *out << std::setw(5)<< kountMbHoleMDT ;
        pMbHoleMDT->Print(out);
      }
    }
  
     int kountMbHoleRPC = kountMbHoleMDT ;
     for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
       const MbHoleRPC*    pMbHoleRPC    = TheTrack->LiIdGetMbHoleRPC    (ElementNber);
       if (pMbHoleRPC) {
         kountMbHoleRPC =kountMbHoleRPC  + 1 ;
         *out << std::setw(5)<< kountMbHoleRPC ;
         pMbHoleRPC->Print(out);
       }
     }

     int kountMbHoleTGC = kountMbHoleRPC ;
     for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
       const MbHoleTGC*    pMbHoleTGC    = TheTrack->LiIdGetMbHoleTGC    (ElementNber);
       if (pMbHoleTGC) {
         kountMbHoleTGC =kountMbHoleTGC  + 1 ;
         *out << std::setw(5)<< kountMbHoleTGC ;
         pMbHoleTGC->Print(out);
       }
     }

     int kountMbHoleCSC = kountMbHoleTGC ;
     for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
       const MbHoleCSC*    pMbHoleCSC    = TheTrack->LiIdGetMbHoleCSC    (ElementNber);
       if (pMbHoleCSC) {
         kountMbHoleCSC =kountMbHoleCSC  + 1 ;
         *out << std::setw(5)<< kountMbHoleCSC ;
         pMbHoleCSC->Print(out);
       }
     }

     int kountMbHoleCSCclu = kountMbHoleCSC ;
     for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
       const MbHoleCSCclu*    pMbHoleCSCclu   = TheTrack->LiIdGetMbHoleCSCclu    (ElementNber);
       if (pMbHoleCSCclu) {
         kountMbHoleCSCclu =kountMbHoleCSCclu  + 1 ;
         *out << std::setw(5)<< kountMbHoleCSCclu ;
         pMbHoleCSCclu->Print(out);
       }
     }

     int kountMbMuSc = kountMbHoleCSCclu ;
     for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
       const MbMuSc*    pMbMuSc   = TheTrack->LiIdGetMbMuSc    (ElementNber);
       if (pMbMuSc) {
         kountMbMuSc =kountMbMuSc  + 1 ;
         *out << std::setw(5)<< kountMbMuSc ;
         pMbMuSc->Print(out);
       }
     }

     TheTrack->PrintSortedElements(out);

   }

}
