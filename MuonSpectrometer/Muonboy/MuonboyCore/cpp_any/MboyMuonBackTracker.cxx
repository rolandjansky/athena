/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MboyMuonBackTracker.h"

#include "MboyCoreEvt/MuonRecDigiStore.h"
#include "MboyCoreEvt/MB_Track.h"
#include "MuonboyCore/MuonboyComcan.h"
#include "MuonboyCore/MuonboyCocyli.h"
#include "MuonboyCore/FillerMB_Track.h"
#include "MuonboyCore/FillerMbIOObjectStore.h"

//*Wrapped Fortran routine
extern "C" void baktrak_();
extern "C" void tramatcov_();

MboyMuonBackTracker::MboyMuonBackTracker(){}

MboyMuonBackTracker::~MboyMuonBackTracker(){}

void MboyMuonBackTracker::initialize(){}


void MboyMuonBackTracker::execute(
     MB_Track*  pMB_Track,
     MB_Track*& pMB_Track_New
){

   int DoDbg = 0 ;
   
   pMB_Track_New = 0 ;

   if ( pMB_Track == 0 ){
     if (DoDbg==1) std::cout << " MboyMuonBackTracker::execute pMB_Track is 0 " << std::endl; 
     return ;
   }else{
     if (DoDbg==1) std::cout << " MboyMuonBackTracker::execute OK pMB_Track is NOT 0 " << std::endl; 
   }

   if ( pMB_Track->IsValidMS() != 1 ){
     if (DoDbg==1) std::cout << " MboyMuonBackTracker::execute IsValidMS is not 1 " << std::endl; 
     return ;
   }else{
     if (DoDbg==1) std::cout << " MboyMuonBackTracker::execute OK IsValidMS is 1 " << std::endl; 
   }
   
   const MB_Track_MboyCyl* pMB_Track_MboyCyl = pMB_Track->pGetTrackAtSpectrometerEntrance() ;

   if ( pMB_Track_MboyCyl == 0 ){
     if (DoDbg==1) std::cout << " MboyMuonBackTracker::execute pMB_Track_MboyCyl is 0 " << std::endl; 
     return ;
   }else{
     if (DoDbg==1) std::cout << " MboyMuonBackTracker::execute OK pMB_Track_MboyCyl is NOT 0 " << std::endl; 
   }
   
// Extract data
   double ThetaX               = pMB_Track_MboyCyl->ThetaOfCrossingPoint()       ;
   double PhiX                 = pMB_Track_MboyCyl->PhiOfCrossingPoint()         ;
   double ThetaV               = pMB_Track_MboyCyl->ThetaOfUnitVector()          ;
   double PhiV                 = pMB_Track_MboyCyl->PhiOfUnitVector()            ;
   double InvPT_SpecEntr       = pMB_Track_MboyCyl->InverseMomentum()    /0.001  ;
   
   double Xcrpoint_SpecEntr    = pMB_Track_MboyCyl->XCrossingPoint()     /10.    ;
   double Ycrpoint_SpecEntr    = pMB_Track_MboyCyl->YCrossingPoint()     /10.    ;
   double Zcrpoint_SpecEntr    = pMB_Track_MboyCyl->ZCrossingPoint()     /10.    ;
   double XUnit                = pMB_Track_MboyCyl->XUnitVector()                ;
   double YUnit                = pMB_Track_MboyCyl->YUnitVector()                ;
   double ZUnit                = pMB_Track_MboyCyl->ZUnitVector()                ;
   double Momentum_SpecEntr    = pMB_Track_MboyCyl->Momentum()           /1000.  ;
   
   double Zhalfcylind_SpecEntr = fabs(pMB_Track_MboyCyl->ZHalfLengthOfCylinder()) /10. ;
   double Rcylind_SpecEntr     =      pMB_Track_MboyCyl->RadiusOfCylinder()       /10. ;

   double QualityFactor = pMB_Track->GetQualityFactor() ; 
   int    NberOfDOF     = pMB_Track->GetNberOfDOF()     ;
   double Chi2Typ0      = pMB_Track->GetChi2(0)         ;
   double Chi2Typ1      = pMB_Track->GetChi2(1)         ;
   double Chi2Typ2      = pMB_Track->GetChi2(2)         ;

// Take a MuonboyComcan and a MuonboyCocyli and set commons contents
   MuonboyComcan aMuonboyComcan;
   MuonboyCocyli aMuonboyCocyli;

   aMuonboyComcan.SetNBCAN(1);

   int TrackNber = 0 ;

   aMuonboyComcan.SetPAXCAN(TrackNber, 0, ThetaX         );
   aMuonboyComcan.SetPAXCAN(TrackNber, 1, PhiX           );
   aMuonboyComcan.SetPAXCAN(TrackNber, 2, ThetaV         );
   aMuonboyComcan.SetPAXCAN(TrackNber, 3, PhiV           );
   aMuonboyComcan.SetPAXCAN(TrackNber, 4, InvPT_SpecEntr );
                                                          
   aMuonboyComcan.SetVXXCAN(TrackNber,0, Xcrpoint_SpecEntr ) ;
   aMuonboyComcan.SetVXXCAN(TrackNber,1, Ycrpoint_SpecEntr ) ;
   aMuonboyComcan.SetVXXCAN(TrackNber,2, Zcrpoint_SpecEntr ) ;
   aMuonboyComcan.SetVXXCAN(TrackNber,3, XUnit             ) ;
   aMuonboyComcan.SetVXXCAN(TrackNber,4, YUnit             ) ;
   aMuonboyComcan.SetVXXCAN(TrackNber,5, ZUnit             ) ;
   aMuonboyComcan.SetVXXCAN(TrackNber,6, Momentum_SpecEntr ) ;
   double Charge = 1. ;
   if (InvPT_SpecEntr < 0. ) Charge = -1. ;
   aMuonboyComcan.SetVXXCAN(TrackNber,7, Charge ) ;

   double KEEP_ZCYLIN_2 = aMuonboyCocyli.ZCYLIN(2) ; aMuonboyCocyli.SetZCYLIN(2, Zhalfcylind_SpecEntr ) ; 
   double KEEP_RCYLIN_2 = aMuonboyCocyli.RCYLIN(2) ; aMuonboyCocyli.SetRCYLIN(2, Rcylind_SpecEntr     ) ; 

   if (DoDbg==1) {
     std::cout << " MboyMuonBackTracker::execute content of aMuonboyCocyli " << std::endl; 
     int NberCylinders = aMuonboyCocyli.NberOfCylinders();
     for (int CylinderNber=0; CylinderNber<NberCylinders; CylinderNber++){
       std::cout << "    Cylinder nber "
            << std::setw(4) << CylinderNber
            << "  RCYLIN = " << std::setw(12)<<std::setprecision(1)<<aMuonboyCocyli.RCYLIN(CylinderNber)
            << "  ZCYLIN = " << std::setw(12)<<std::setprecision(1)<<aMuonboyCocyli.ZCYLIN(CylinderNber)
            << "  RCYLI2 = " << std::setw(12)<<std::setprecision(1)<<aMuonboyCocyli.RCYLI2(CylinderNber)
 	  << std::endl;
     }
     std::cout << " MboyMuonBackTracker::execute content of aMuonboyComcan " << std::endl; 
     for (int Item=0; Item<7; Item++){
       std::cout << "    Item nber "
            << std::setw(4) << Item
            << "  VXXCAN = " << std::setw(12)<<std::setprecision(1)<<aMuonboyComcan.VXXCAN(TrackNber,Item)
 	  << std::endl;
     }
   }

   aMuonboyComcan.SetCHICAN(TrackNber  , QualityFactor ) ;
   aMuonboyComcan.SetNDFCAN(TrackNber  , NberOfDOF     ) ;
   aMuonboyComcan.SetCXXCAN(TrackNber,0, Chi2Typ0      ) ;
   aMuonboyComcan.SetCXXCAN(TrackNber,1, Chi2Typ1      ) ;
   aMuonboyComcan.SetCXXCAN(TrackNber,2, Chi2Typ2      ) ;

   for (int I=0; I<5; I++){
     for (int J=0; J<5; J++){
       double scale = 1. ;
       if (I==4) scale = scale /0.001 ;
       if (J==4) scale = scale /0.001 ;
       int Is = I + 1 ;
       int Js = J + 1 ;
       double vtemp = pMB_Track_MboyCyl->Covij(Is,Js) * scale ;
       aMuonboyComcan.SetCOXCAN(TrackNber,I,J,vtemp)  ;
     }
   }
   
   aMuonboyComcan.SetNBMSXX(TrackNber  ,0);
   aMuonboyComcan.SetNZXCAN(TrackNber  ,0);
   aMuonboyComcan.SetNSXCAN(TrackNber  ,0);
   aMuonboyComcan.SetNZOCAN(TrackNber  ,0);
   aMuonboyComcan.SetNHXCAN(TrackNber  ,0);
   aMuonboyComcan.SetISECAN(TrackNber  ,0);
   aMuonboyComcan.SetFLGCAN(TrackNber  ,0);
   

//BackTrack and compute Cov matrices
    baktrak_() ;
    tramatcov_() ;

//Return track
   int FlgCan  = aMuonboyComcan.FLGCAN(TrackNber);
   int strLine = FlgCan / 100;
   FlgCan      = FlgCan - strLine*100;       
   int reverse = FlgCan / 10;
   FlgCan      = FlgCan - reverse*10; 

   if ( FlgCan == 1 || FlgCan == 2 || FlgCan == 3 ){
   
     if (DoDbg==1) std::cout << " MboyMuonBackTracker::execute OK GOOD FlgCan is  " << FlgCan << std::endl; 
    
     FillerMB_Track aFillerMB_Track ;

//   Create a MB_Track_SpecEntr
     MB_Track_SpecEntr  aMB_Track_SpecEntr ;
     aFillerMB_Track.FillMB_Track_SpecEntr(&aMB_Track_SpecEntr,TrackNber);

//   Create a MB_Track_CaloEntr
     MB_Track_CaloEntr aMB_Track_CaloEntr ;
     aFillerMB_Track.FillMB_Track_CaloEntr(&aMB_Track_CaloEntr,TrackNber);

//   Create a MB_Track_Perigee
     MB_Track_Perigee aMB_Track_Perigee ;
     aFillerMB_Track.FillMB_Track_Perigee(&aMB_Track_Perigee,TrackNber);

//   Fake digits
     MuonRecDigiStore aMuonRecDigiStore ; aMuonRecDigiStore.ResetMuonRecDigiStore();
     
//   Collect indexes of digis
     MbIOObjectStore    aMbIOObjectStore ;
     aMbIOObjectStore.SetRunNber(aMuonRecDigiStore.GetRunNber());
     aMbIOObjectStore.SetEventNber(aMuonRecDigiStore.GetEventNber());

     FillerMbIOObjectStore aFillerMbIOObjectStore;
     aFillerMbIOObjectStore.FillMbIOObjectStore(
                                               &aMuonRecDigiStore,
                                               &aMbIOObjectStore,
                                               TrackNber
                                              );

//   Quality Factor, Nber of d.o.f and Chi2
     double QualityFactor = aMuonboyComcan.CHICAN(TrackNber);
     int    NberOfDOF     = aMuonboyComcan.NDFCAN(TrackNber);
     double Chi2Typ0      = aMuonboyComcan.CXXCAN(TrackNber,0);
     double Chi2Typ1      = aMuonboyComcan.CXXCAN(TrackNber,1);
     double Chi2Typ2      = aMuonboyComcan.CXXCAN(TrackNber,2);

//   Create a MB_Track 
     pMB_Track_New = new MB_Track(
      aMB_Track_SpecEntr,aMB_Track_CaloEntr,aMB_Track_Perigee,
      QualityFactor,NberOfDOF,
      Chi2Typ0,Chi2Typ1,Chi2Typ2,
      aMbIOObjectStore
     );
     pMB_Track->Setenmsxx(aMuonboyComcan.ENMSXX(TrackNber)) ;

   }else{
     if (DoDbg==1) std::cout << " MboyMuonBackTracker::execute BAD FlgCan is  " << FlgCan << std::endl; 
   }
    


//Reset Cylinders dimensions
   aMuonboyCocyli.SetZCYLIN(2, KEEP_ZCYLIN_2 ) ;
   aMuonboyCocyli.SetRCYLIN(2, KEEP_RCYLIN_2 ) ;

   return ;
}


void MboyMuonBackTracker::finalize(){}
