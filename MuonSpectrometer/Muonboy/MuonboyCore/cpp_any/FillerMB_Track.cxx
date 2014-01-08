/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/FillerMB_Track.h"

#include "MboyCoreEvt/MB_Track.h"

#include "MuonboyCore/MuonboyComcan.h"
#include "MuonboyCore/MuonboyCocyli.h"

FillerMB_Track::FillerMB_Track(){}
FillerMB_Track::~FillerMB_Track(){}

void FillerMB_Track::FillMB_Track_SpecEntr (MB_Track_SpecEntr* pMB_Track_SpecEntr,int TrackNber)
{
   if ( pMB_Track_SpecEntr == 0 ){
     std::cout << " FillerMB_Track::execute pMB_Track_SpecEntr is 0 " << std::endl; 
     return ;
   }

// Take a MuonboyComcan and a MuonboyCocyli
   MuonboyComcan aMuonboyComcan;
   MuonboyCocyli aMuonboyCocyli;

   int FlgCan  = aMuonboyComcan.FLGCAN(TrackNber);
   int strLine = FlgCan / 100;
   FlgCan      = FlgCan - strLine*100;       
   int reverse = FlgCan / 10;
   FlgCan      = FlgCan - reverse*10; 

   if ( FlgCan == 2 || FlgCan == 3 || FlgCan == 4 ){
//   Change cm-->mm, GeV-->MeV

     double InvPT_SpecEntr = aMuonboyComcan.PAXCAN(TrackNber,4)   *0.001 ;
     double Cov51_SpecEntr = aMuonboyComcan.COXCAN(TrackNber,4,0) *0.001 ;
     double Cov52_SpecEntr = aMuonboyComcan.COXCAN(TrackNber,4,1) *0.001 ;
     double Cov53_SpecEntr = aMuonboyComcan.COXCAN(TrackNber,4,2) *0.001 ;
     double Cov54_SpecEntr = aMuonboyComcan.COXCAN(TrackNber,4,3) *0.001 ;
     double Cov55_SpecEntr = aMuonboyComcan.COXCAN(TrackNber,4,4) *0.001*0.001 ;
     double Xcrpoint_SpecEntr = aMuonboyComcan.VXXCAN(TrackNber,0)*10. ;
     double Ycrpoint_SpecEntr = aMuonboyComcan.VXXCAN(TrackNber,1)*10. ;
     double Zcrpoint_SpecEntr = aMuonboyComcan.VXXCAN(TrackNber,2)*10. ;
     double Momentum_SpecEntr = aMuonboyComcan.VXXCAN(TrackNber,6)*1000. ;
     double Zhalfcylind_SpecEntr = aMuonboyCocyli.ZCYLIN(2)       *10. ;
     double Rcylind_SpecEntr = aMuonboyCocyli.RCYLIN(2)           *10. ;

     pMB_Track_SpecEntr->SetIsToBeReversed(reverse) ;
     pMB_Track_SpecEntr->SetIsStraightLine(strLine) ;
     
     pMB_Track_SpecEntr->SetTrackSpecEntr(
       aMuonboyComcan.PAXCAN(TrackNber,0),
          aMuonboyComcan.PAXCAN(TrackNber,1),
             aMuonboyComcan.PAXCAN(TrackNber,2),
                aMuonboyComcan.PAXCAN(TrackNber,3),
                                      InvPT_SpecEntr,
       aMuonboyComcan.COXCAN(TrackNber,0,0),
          aMuonboyComcan.COXCAN(TrackNber,1,0),
          aMuonboyComcan.COXCAN(TrackNber,1,1),
             aMuonboyComcan.COXCAN(TrackNber,2,0),
             aMuonboyComcan.COXCAN(TrackNber,2,1),
             aMuonboyComcan.COXCAN(TrackNber,2,2),
                aMuonboyComcan.COXCAN(TrackNber,3,0),
                aMuonboyComcan.COXCAN(TrackNber,3,1),
                aMuonboyComcan.COXCAN(TrackNber,3,2),
                aMuonboyComcan.COXCAN(TrackNber,3,3),
                   Cov51_SpecEntr,
                   Cov52_SpecEntr,
                   Cov53_SpecEntr,
                   Cov54_SpecEntr,
                   Cov55_SpecEntr,
       Xcrpoint_SpecEntr,
          Ycrpoint_SpecEntr,
             Zcrpoint_SpecEntr,   
       aMuonboyComcan.VXXCAN(TrackNber,3),
          aMuonboyComcan.VXXCAN(TrackNber,4),
             aMuonboyComcan.VXXCAN(TrackNber,5),
       Momentum_SpecEntr,
          Zhalfcylind_SpecEntr,  
             Rcylind_SpecEntr);
   }   

}

void FillerMB_Track::FillMB_Track_CaloEntr (MB_Track_CaloEntr* pMB_Track_CaloEntr,int TrackNber)
{
   if ( pMB_Track_CaloEntr == 0 ){
     std::cout << " FillerMB_Track::execute pMB_Track_CaloEntr is 0 " << std::endl; 
     return ;
   }

// Take a MuonboyComcan and a MuonboyCocyli
   MuonboyComcan aMuonboyComcan;
   MuonboyCocyli aMuonboyCocyli;

   int FlgCan  = aMuonboyComcan.FLGCAN(TrackNber);
   int strLine = FlgCan / 100;
   FlgCan      = FlgCan - strLine*100;       
   int reverse = FlgCan / 10;
   FlgCan      = FlgCan - reverse*10; 

   if ( FlgCan == 3 ){
//   Change cm-->mm, GeV-->MeV

     double InvPT_CaloEntr = aMuonboyComcan.PAYCAN(TrackNber,4)   *0.001 ;
     double Cov51_CaloEntr = aMuonboyComcan.COYCAN(TrackNber,4,0) *0.001 ;
     double Cov52_CaloEntr = aMuonboyComcan.COYCAN(TrackNber,4,1) *0.001 ;
     double Cov53_CaloEntr = aMuonboyComcan.COYCAN(TrackNber,4,2) *0.001 ;
     double Cov54_CaloEntr = aMuonboyComcan.COYCAN(TrackNber,4,3) *0.001 ;
     double Cov55_CaloEntr = aMuonboyComcan.COYCAN(TrackNber,4,4) *0.001*0.001 ;
     double Xcrpoint_CaloEntr = aMuonboyComcan.VYYCAN(TrackNber,0)*10. ;
     double Ycrpoint_CaloEntr = aMuonboyComcan.VYYCAN(TrackNber,1)*10. ;
     double Zcrpoint_CaloEntr = aMuonboyComcan.VYYCAN(TrackNber,2)*10. ;
     double Momentum_CaloEntr = aMuonboyComcan.VYYCAN(TrackNber,6)*1000. ;
     double Zhalfcylind_CaloEntr = aMuonboyCocyli.ZCYLIN(1)       *10. ;
     double Rcylind_CaloEntr = aMuonboyCocyli.RCYLIN(1)           *10. ;

     pMB_Track_CaloEntr->SetIsToBeReversed(reverse) ;
     pMB_Track_CaloEntr->SetIsStraightLine(strLine) ;
     
     pMB_Track_CaloEntr->SetTrackCalo(
      aMuonboyComcan.PAYCAN(TrackNber,0),
         aMuonboyComcan.PAYCAN(TrackNber,1),
             aMuonboyComcan.PAYCAN(TrackNber,2),
                aMuonboyComcan.PAYCAN(TrackNber,3),
                   InvPT_CaloEntr,
       aMuonboyComcan.COYCAN(TrackNber,0,0),
          aMuonboyComcan.COYCAN(TrackNber,1,0),
          aMuonboyComcan.COYCAN(TrackNber,1,1),
             aMuonboyComcan.COYCAN(TrackNber,2,0),
             aMuonboyComcan.COYCAN(TrackNber,2,1),
             aMuonboyComcan.COYCAN(TrackNber,2,2),
                aMuonboyComcan.COYCAN(TrackNber,3,0),
                aMuonboyComcan.COYCAN(TrackNber,3,1),
                aMuonboyComcan.COYCAN(TrackNber,3,2),
                aMuonboyComcan.COYCAN(TrackNber,3,3),
                   Cov51_CaloEntr,
                   Cov52_CaloEntr,
                   Cov53_CaloEntr,
                   Cov54_CaloEntr,
                   Cov55_CaloEntr,
       Xcrpoint_CaloEntr,
          Ycrpoint_CaloEntr,
             Zcrpoint_CaloEntr,  
       aMuonboyComcan.VYYCAN(TrackNber,3),
          aMuonboyComcan.VYYCAN(TrackNber,4),
             aMuonboyComcan.VYYCAN(TrackNber,5),
       Momentum_CaloEntr,
          Zhalfcylind_CaloEntr,  
             Rcylind_CaloEntr); 

   }   

}

void FillerMB_Track::FillMB_Track_Perigee (MB_Track_Perigee* pMB_Track_Perigee,int TrackNber)
{
   if ( pMB_Track_Perigee == 0 ){
     std::cout << " FillerMB_Track::execute pMB_Track_Perigee is 0 " << std::endl; 
     return ;
   }

// Take a MuonboyComcan and a MuonboyCocyli
   MuonboyComcan aMuonboyComcan;
   MuonboyCocyli aMuonboyCocyli;

   int FlgCan  = aMuonboyComcan.FLGCAN(TrackNber);
   int strLine = FlgCan / 100;
   FlgCan      = FlgCan - strLine*100;       
   int reverse = FlgCan / 10;
   FlgCan      = FlgCan - reverse*10; 

   if ( FlgCan == 1 || FlgCan == 2 || FlgCan == 3 ){
//   Change cm-->mm, GeV-->MeV

     double IPRPHI_Perigee = aMuonboyComcan.PAZCAN(TrackNber,0) *10. ;
     double IPZ_Perigee = aMuonboyComcan.PAZCAN(TrackNber,1)    *10. ;
     double InvPT_Perigee = aMuonboyComcan.PAZCAN(TrackNber,4)  *0.001 ;
     double Cov11_Perigee = aMuonboyComcan.COZCAN(TrackNber,0,0)*10.*10. ;
     double Cov21_Perigee = aMuonboyComcan.COZCAN(TrackNber,1,0)*10.*10. ;
     double Cov22_Perigee = aMuonboyComcan.COZCAN(TrackNber,1,1)*10.*10. ;
     double Cov31_Perigee = aMuonboyComcan.COZCAN(TrackNber,2,0)*10. ;
     double Cov32_Perigee = aMuonboyComcan.COZCAN(TrackNber,2,1)*10. ;
     double Cov41_Perigee = aMuonboyComcan.COZCAN(TrackNber,3,0)*10. ;
     double Cov42_Perigee = aMuonboyComcan.COZCAN(TrackNber,3,1)*10. ;
     double Cov51_Perigee = aMuonboyComcan.COZCAN(TrackNber,4,0)*10.*0.001 ;
     double Cov52_Perigee = aMuonboyComcan.COZCAN(TrackNber,4,1)*10.*0.001 ;
     double Cov53_Perigee = aMuonboyComcan.COZCAN(TrackNber,4,2)*0.001 ;
     double Cov54_Perigee = aMuonboyComcan.COZCAN(TrackNber,4,3)*0.001 ;
     double Cov55_Perigee = aMuonboyComcan.COZCAN(TrackNber,4,4)*0.001*0.001 ;
     double X_Perigee = aMuonboyComcan.VZZCAN(TrackNber,0)      *10. ;
     double Y_Perigee = aMuonboyComcan.VZZCAN(TrackNber,1)      *10. ;
     double Z_Perigee = aMuonboyComcan.VZZCAN(TrackNber,2)      *10. ;
     double Mom_Perigee = aMuonboyComcan.VZZCAN(TrackNber,6)    *1000. ;

     pMB_Track_Perigee->SetIsToBeReversed(reverse) ;
     pMB_Track_Perigee->SetIsStraightLine(strLine) ;
     
     pMB_Track_Perigee->SetTrackPerigee(
       IPRPHI_Perigee, 
          IPZ_Perigee, 
             aMuonboyComcan.PAZCAN(TrackNber,2),
                aMuonboyComcan.PAZCAN(TrackNber,3),
                   InvPT_Perigee, 
       Cov11_Perigee, 
          Cov21_Perigee,
             Cov22_Perigee,
       Cov31_Perigee,
          Cov32_Perigee,
             aMuonboyComcan.COZCAN(TrackNber,2,2),
       Cov41_Perigee,
          Cov42_Perigee,
                aMuonboyComcan.COZCAN(TrackNber,3,2),
                   aMuonboyComcan.COZCAN(TrackNber,3,3),
       Cov51_Perigee,
          Cov52_Perigee,
             Cov53_Perigee,
                Cov54_Perigee,
                   Cov55_Perigee,
       X_Perigee,
          Y_Perigee,
             Z_Perigee,
       aMuonboyComcan.VZZCAN(TrackNber,3),
          aMuonboyComcan.VZZCAN(TrackNber,4),
             aMuonboyComcan.VZZCAN(TrackNber,5),
       Mom_Perigee); 
   }   

}
