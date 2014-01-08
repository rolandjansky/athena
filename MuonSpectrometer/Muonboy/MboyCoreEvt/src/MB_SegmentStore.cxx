/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MB_SegmentStore.h"
 
MB_SegmentStore::MB_SegmentStore(){}
 
MB_SegmentStore::~MB_SegmentStore(){}

// Reset the Store
void MB_SegmentStore::ResetMB_SegmentStore(){MB_SegmentSet.clear();}
 
// Add a MB_Segment
void MB_SegmentStore::Add(const MB_Segment& ToBeAdded){MB_SegmentSet.push_back(ToBeAdded);}
 
// Nber of MB_Segment
int MB_SegmentStore::NberOfMB_Segment() const{return MB_SegmentSet.size();}
 
// Get a Pointer on a MB_Segment of Nber MB_SegmentNber
const MB_Segment* MB_SegmentStore::GetMB_Segment(int MB_SegmentNber) const{
 const MB_Segment* ToBeReturned = 0 ;
 if (MB_SegmentNber < 0 || MB_SegmentNber > NberOfMB_Segment()) return ToBeReturned;
 return &MB_SegmentSet[MB_SegmentNber];
}

// Print Out the Store
void MB_SegmentStore::PrintSummary() const{

//*Print out Total Nber of MB_Segment
   std::cout << "*  The Nber of MB_Segment in the MB_SegmentStore is "
             << NberOfMB_Segment()
             << std::endl;

}

void MB_SegmentStore::PrintSegments(
 std::ostream* out,
 int EventNber
) const{

  int NberOfSegments = NberOfMB_Segment();

  *out << "--------------------------------"
       << std::endl;
  *out << "Evt.Seg.Tra."
       << std::setw(10)<< EventNber
       << std::setw(5)<< NberOfSegments
       << std::endl;

  for (int SegmentNber=0; SegmentNber<NberOfSegments; SegmentNber++){
    const MB_Segment* pMB_Segment = GetMB_Segment(SegmentNber);

    int NberOfMDT    = pMB_Segment->GetNberOfDigiMDT () ;
    int NberOfRPC    = pMB_Segment->GetNberOfDigiRPC () ;
    int NberOfTGC    = pMB_Segment->GetNberOfDigiTGC () ;
    int NberOfCSC    = pMB_Segment->GetNberOfDigiCSC() ;
    int NberOfCSCclu = pMB_Segment->GetNberOfDigiCSCclu() ;
    int NberOfElements = pMB_Segment->GetNberOfElements() ;   

    int NberOfMbMuSc    = pMB_Segment->GetNberOfMbMuSc   () ;

    int NberOfHoleMDT    = pMB_Segment->GetNberOfHoleMDT   () ;
    int NberOfHoleRPC    = pMB_Segment->GetNberOfHoleRPC   () ;
    int NberOfHoleTGC    = pMB_Segment->GetNberOfHoleTGC   () ;
    int NberOfHoleCSC    = pMB_Segment->GetNberOfHoleCSC   () ;
    int NberOfHoleCSCclu = pMB_Segment->GetNberOfHoleCSCclu() ;
    int NberOfDigi       = pMB_Segment->GetNberOfDigi() ;

    double Xpt     =0.;
    double Ypt     =0.;
    double Zpt     =0.;
    pMB_Segment->GetPoint(Xpt,Ypt,Zpt); 
    double Xvec    =0.;
    double Yvec    =0.;
    double Zvec    =0.;
    pMB_Segment->GetDirection(Xvec,Yvec,Zvec) ;
    double      QualityFactor    = pMB_Segment->GetQualityFactor() ;
    int         NbreOfMultiLayer = pMB_Segment->GetNberOfMultiLayer() ;
    int         TrackSharing = pMB_Segment->GetTrackSharing() ;
    std::string StaName          = pMB_Segment->GetStationName();

    double X_dx  =0.;
    double Y_dx  =0.;
    double Z_dx  =0.;
    pMB_Segment->Getdx(X_dx,Y_dx,Z_dx);
    double X_dy  =0.;
    double Y_dy  =0.;
    double Z_dy  =0.;
    pMB_Segment->Getdy(X_dy,Y_dy,Z_dy);
    int    E_Valid   = 0  ;
    double E_dx      = 0. ;
    double E_Axz     = 0. ;
    double E_dxAxz   = 0. ;
    double E_dy      = 0. ; 
    double E_Ayz     = 0. ; 
    double E_dyAyz   = 0. ; 
    pMB_Segment->GetErrorsComponents(
     E_Valid,
     E_dx, 
     E_Axz , 
     E_dxAxz,
     E_dy, 
     E_Ayz , 
     E_dyAyz);
     double FittedT0, eFittedT0;
     pMB_Segment->GetFittedT0(FittedT0,eFittedT0 );

    Xpt     =Xpt/10.;
    Ypt     =Ypt/10.;
    Zpt     =Zpt/10.;

    *out << " Seg.------>"
         << std::setw(5)<< SegmentNber+1
         << std::setw(5)<< NbreOfMultiLayer
         << std::setw(5)<< NberOfMDT
         << std::setw(5)<< NberOfRPC
         << std::setw(5)<< TrackSharing
         << std::setw(10)<<std::setprecision(4)<< QualityFactor
         << "  " << std::setw(3)<< StaName
	<< std::endl;
     if ( pMB_Segment->isStrict() ) {
       *out << " Strict Segment "
	    << std::endl;
     }else{
       *out << " Loose Segment  "
	    << std::endl;
     }
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

    *out << std::setw(12)<<std::setprecision(4)<< FittedT0
         << std::setw(12)<<std::setprecision(4)<< eFittedT0
	<< std::endl;
        
    *out << std::setw(12)<<std::setprecision(4)<< Xpt
         << std::setw(12)<<std::setprecision(4)<< Ypt
         << std::setw(12)<<std::setprecision(4)<< Zpt
         << std::setw(12)<<std::setprecision(6)<< Xvec
         << std::setw(12)<<std::setprecision(6)<< Yvec
         << std::setw(12)<<std::setprecision(6)<< Zvec
	<< std::endl;
    *out << " Error validity " << std::setw(12)<< E_Valid
	<< std::endl;
    if ( E_Valid !=0 ) {
    *out << "dx Axz mm/mrad         "
         << std::setw(12)<<std::setprecision(4)<< 1./sqrt(E_dx)
         << std::setw(12)<<std::setprecision(4)<< 1000./sqrt(E_Axz)
         << std::setw(12)<<std::setprecision(4)<< E_dxAxz/(sqrt(E_dx)*sqrt(E_Axz))
	<< std::endl;
    *out << "dy Axz micron/microrad "
         << std::setw(12)<<std::setprecision(4)<< 1000./sqrt(E_dy)
         << std::setw(12)<<std::setprecision(4)<< 1000000./sqrt(E_Ayz)
         << std::setw(12)<<std::setprecision(4)<< E_dyAyz/(sqrt(E_dy)*sqrt(E_Ayz))
	<< std::endl;
    }

    int kountMbROTMDT = 0 ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbROTMDT*    pMbROTMDT    = pMB_Segment->LiIdGetMbROTMDT    (ElementNber);
      if (pMbROTMDT) {
        kountMbROTMDT =kountMbROTMDT  + 1 ;
        *out << std::setw(5)<< kountMbROTMDT ;
        pMbROTMDT->Print(out);
      }
    }

    int kountMbROTRPC = kountMbROTMDT ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbROTRPC*    pMbROTRPC    = pMB_Segment->LiIdGetMbROTRPC    (ElementNber);
      if (pMbROTRPC) {
        kountMbROTRPC =kountMbROTRPC  + 1 ;
        *out << std::setw(5)<< kountMbROTRPC ;
        pMbROTRPC->Print(out);
      }
    }

    int kountMbROTTGC = kountMbROTRPC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbROTTGC*    pMbROTTGC    = pMB_Segment->LiIdGetMbROTTGC    (ElementNber);
      if (pMbROTTGC) {
        kountMbROTTGC =kountMbROTTGC  + 1 ;
        *out << std::setw(5)<< kountMbROTTGC ;
        pMbROTTGC->Print(out);
      }
    }

    int kountMbROTCSC = kountMbROTTGC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbROTCSC*    pMbROTCSC    = pMB_Segment->LiIdGetMbROTCSC    (ElementNber);
      if (pMbROTCSC) {
        kountMbROTCSC =kountMbROTCSC  + 1 ;
        *out << std::setw(5)<< kountMbROTCSC ;
        pMbROTCSC->Print(out);
      }
    }

    int kountMbROTCSCclu = kountMbROTCSC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbROTCSCclu* pMbROTCSCclu = pMB_Segment->LiIdGetMbROTCSCclu (ElementNber);
      if (pMbROTCSCclu) {
        kountMbROTCSCclu =kountMbROTCSCclu  + 1 ;
        *out << std::setw(5)<< kountMbROTCSCclu ;
        pMbROTCSCclu->Print(out);
      }
    }


    int kountMbHoleMDT = kountMbROTCSCclu ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleMDT* pMbHoleMDT = pMB_Segment->LiIdGetMbHoleMDT (ElementNber);
      if (pMbHoleMDT) {
        kountMbHoleMDT =kountMbHoleMDT  + 1 ;
        *out << std::setw(5)<< kountMbHoleMDT ;
        pMbHoleMDT->Print(out);
      }
    }
  
    int kountMbHoleRPC = kountMbHoleMDT ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleRPC*    pMbHoleRPC    = pMB_Segment->LiIdGetMbHoleRPC    (ElementNber);
      if (pMbHoleRPC) {
        kountMbHoleRPC =kountMbHoleRPC  + 1 ;
        *out << std::setw(5)<< kountMbHoleRPC ;
        pMbHoleRPC->Print(out);
      }
    }

    int kountMbHoleTGC = kountMbHoleRPC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleTGC*    pMbHoleTGC    = pMB_Segment->LiIdGetMbHoleTGC    (ElementNber);
      if (pMbHoleTGC) {
        kountMbHoleTGC =kountMbHoleTGC  + 1 ;
        *out << std::setw(5)<< kountMbHoleTGC ;
        pMbHoleTGC->Print(out);
      }
    }

    int kountMbHoleCSC = kountMbHoleTGC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleCSC*    pMbHoleCSC    = pMB_Segment->LiIdGetMbHoleCSC    (ElementNber);
      if (pMbHoleCSC) {
        kountMbHoleCSC =kountMbHoleCSC  + 1 ;
        *out << std::setw(5)<< kountMbHoleCSC ;
        pMbHoleCSC->Print(out);
      }
    }

    int kountMbHoleCSCclu = kountMbHoleCSC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleCSCclu*    pMbHoleCSCclu   = pMB_Segment->LiIdGetMbHoleCSCclu    (ElementNber);
      if (pMbHoleCSCclu) {
        kountMbHoleCSCclu =kountMbHoleCSCclu  + 1 ;
        *out << std::setw(5)<< kountMbHoleCSCclu ;
        pMbHoleCSCclu->Print(out);
      }
    }

    int kountMbMuSc = kountMbHoleCSCclu ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbMuSc*    pMbMuSc   = pMB_Segment->LiIdGetMbMuSc    (ElementNber);
      if (pMbMuSc) {
        kountMbMuSc =kountMbMuSc  + 1 ;
        *out << std::setw(5)<< kountMbMuSc ;
        pMbMuSc->Print(out);
      }
    }

    pMB_Segment->PrintSortedElements(out);

  }

}
 
