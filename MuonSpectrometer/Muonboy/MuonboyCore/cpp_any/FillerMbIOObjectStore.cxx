/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/FillerMbIOObjectStore.h"

#include "MboyCoreEvt/MuonRecDigiStore.h"
#include "MboyCoreEvt/MbIOObjectStore.h"

#include "MuonboyCore/MuonboyComcan.h"
#include "MuonboyCore/GetDecoIdentAMDC.h"
#include "MboyCoreEvt/MbMuSc.h"

FillerMbIOObjectStore::FillerMbIOObjectStore(){}
FillerMbIOObjectStore::~FillerMbIOObjectStore(){}

void FillerMbIOObjectStore::FillMbIOObjectStore(
     const MuonRecDigiStore* pMuonRecDigiStore,
     MbIOObjectStore* pMbIOObjectStore,
     int TrackNber     
){
   if ( pMbIOObjectStore == 0 ){
     std::cout << " FillerMbIOObjectStore::FillMbIOObjectStore pMbIOObjectStore is 0 " << std::endl; 
     return ;
   }
// Take a MuonboyComcan
   MuonboyComcan aMuonboyComcan;

   int FlgCan  = aMuonboyComcan.FLGCAN(TrackNber);
   int strLine = FlgCan / 100;
   FlgCan      = FlgCan - strLine*100;       
   int reverse = FlgCan / 10;
   FlgCan      = FlgCan - reverse*10; 

//ROTs
   int NberOf1stCo = aMuonboyComcan.NZXCAN(TrackNber);
   for (int index=0; index<NberOf1stCo; index++){
     int IdDigit = aMuonboyComcan.Index1stCo(TrackNber,index);
     double Xdca  = aMuonboyComcan.XDCCAN(TrackNber,index)*10.;
     double Ydca  = aMuonboyComcan.YDCCAN(TrackNber,index)*10.;
     double Zdca  = aMuonboyComcan.ZDCCAN(TrackNber,index)*10.;
     double Xdir  = aMuonboyComcan.XDCdCAN(TrackNber,index)*10.;
     double Ydir  = aMuonboyComcan.YDCdCAN(TrackNber,index)*10.;
     double Zdir  = aMuonboyComcan.ZDCdCAN(TrackNber,index)*10.;
     double Ener  = aMuonboyComcan.ENCCAN(TrackNber,index)*1000.;
     double Sdca  = aMuonboyComcan.SDCCAN(TrackNber,index)*10.;
     double Reso  = aMuonboyComcan.RDCCAN(TrackNber,index)*10.;
     double Rdigi = aMuonboyComcan.RZXCAN(TrackNber,index)*10.;
     double Rtrak = aMuonboyComcan.RZICAN(TrackNber,index)*10.;
     double RZsim = aMuonboyComcan.RZISIM(TrackNber,index)*10.;
     double EZfit = aMuonboyComcan.EZIFIT(TrackNber,index)*10.;
     const MuonRecDigitMDT*    pMuonRecDigitMDT    = pMuonRecDigiStore->LiIdGetMuonRecDigitMDT   (IdDigit) ;
     if (pMuonRecDigitMDT) {
       MbROTMDT aMbROTMDT;
       
       aMbROTMDT.SetIsToBeReversed(reverse) ;
       
       aMbROTMDT.SetMuonRecDigit(*pMuonRecDigitMDT) ;
       aMbROTMDT.SetLinkPrd(IdDigit) ;
       aMbROTMDT.SetXdca (Xdca );
       aMbROTMDT.SetYdca (Ydca );
       aMbROTMDT.SetZdca (Zdca );
       aMbROTMDT.SetXdir (Xdir );
       aMbROTMDT.SetYdir (Ydir );
       aMbROTMDT.SetZdir (Zdir );
       aMbROTMDT.SetEner (Ener );
       aMbROTMDT.SetSdca (Sdca );
       aMbROTMDT.SetReso (Reso );
       aMbROTMDT.SetRZsim(RZsim);
       aMbROTMDT.SetEZfit(EZfit);
       aMbROTMDT.SetRdigi(Rdigi);
       aMbROTMDT.SetRtrak(Rtrak);
       pMbIOObjectStore->Add(aMbROTMDT);
     }else{
       const MuonRecDigitCSCClu* pMuonRecDigitCSCClu = pMuonRecDigiStore->LiIdGetMuonRecDigitCSCClu(IdDigit) ;
       if (pMuonRecDigitCSCClu) {
         MbROTCSCclu aMbROTCSCclu;
       
         aMbROTCSCclu.SetIsToBeReversed(reverse) ;
       
         aMbROTCSCclu.SetMuonRecDigit(*pMuonRecDigitCSCClu) ;
         aMbROTCSCclu.SetLinkPrd(IdDigit) ;
         aMbROTCSCclu.SetXdca (Xdca ) ;
         aMbROTCSCclu.SetYdca (Ydca ) ;
         aMbROTCSCclu.SetZdca (Zdca ) ;
         aMbROTCSCclu.SetXdir (Xdir );
         aMbROTCSCclu.SetYdir (Ydir );
         aMbROTCSCclu.SetZdir (Zdir );
         aMbROTCSCclu.SetEner (Ener );
         aMbROTCSCclu.SetSdca (Sdca ) ;
         aMbROTCSCclu.SetReso (Reso ) ;
         aMbROTCSCclu.SetRsim (RZsim) ;
         aMbROTCSCclu.SetEfit (EZfit) ;
         aMbROTCSCclu.SetRdigi(Rdigi);
         aMbROTCSCclu.SetRtrak(Rtrak);
         pMbIOObjectStore->Add(aMbROTCSCclu);
       }else{
         const MuonRecDigitCSC*    pMuonRecDigitCSC    = pMuonRecDigiStore->LiIdGetMuonRecDigitCSC   (IdDigit) ;
         if (pMuonRecDigitCSC) {
           MbROTCSC aMbROTCSC;
       
           aMbROTCSC.SetIsToBeReversed(reverse) ;
       
           aMbROTCSC.SetMuonRecDigit(*pMuonRecDigitCSC) ;
           aMbROTCSC.SetLinkPrd(IdDigit) ;
           aMbROTCSC.SetXdca (Xdca );
           aMbROTCSC.SetYdca (Ydca );
           aMbROTCSC.SetZdca (Zdca );
           aMbROTCSC.SetXdir (Xdir );
           aMbROTCSC.SetYdir (Ydir );
           aMbROTCSC.SetZdir (Zdir );
           aMbROTCSC.SetEner (Ener );
           aMbROTCSC.SetSdca (Sdca );
           aMbROTCSC.SetReso (Reso );
           aMbROTCSC.SetRZsim(RZsim);
           aMbROTCSC.SetEZfit(EZfit);
           pMbIOObjectStore->Add(aMbROTCSC);
         }
       }
     }
   }
   int NberOf2ndCo = aMuonboyComcan.NSXCAN(TrackNber);
   for (int index=0; index<NberOf2ndCo; index++){
     int IdDigit = aMuonboyComcan.Index2ndCo(TrackNber,index);
     double Xdca  = aMuonboyComcan.XSDCCAN(TrackNber,index)*10.;
     double Ydca  = aMuonboyComcan.YSDCCAN(TrackNber,index)*10.;
     double Zdca  = aMuonboyComcan.ZSDCCAN(TrackNber,index)*10.;
     double Xdir  = aMuonboyComcan.XSDCdCAN(TrackNber,index)*10.;
     double Ydir  = aMuonboyComcan.YSDCdCAN(TrackNber,index)*10.;
     double Zdir  = aMuonboyComcan.ZSDCdCAN(TrackNber,index)*10.;
     double Ener  = aMuonboyComcan.ESNCCAN(TrackNber,index)*1000.;
     double Reso  = aMuonboyComcan.RSDCCAN(TrackNber,index)*10.;
     double Rdigi = aMuonboyComcan.RSXCAN(TrackNber,index)*10.;
     double RSsim = aMuonboyComcan.RSISIM(TrackNber,index)*10.;
     double ESfit = aMuonboyComcan.ESIFIT(TrackNber,index)*10.;
     const MuonRecDigitRPC*    pMuonRecDigitRPC    = pMuonRecDigiStore->LiIdGetMuonRecDigitRPC(IdDigit) ;
     if (pMuonRecDigitRPC) {
       MbROTRPC aMbROTRPC;
       
       aMbROTRPC.SetIsToBeReversed(reverse) ;
       
       aMbROTRPC.SetMuonRecDigit(*pMuonRecDigitRPC) ;
       aMbROTRPC.SetLinkPrd(IdDigit) ;
       aMbROTRPC.SetXdca (Xdca ) ;
       aMbROTRPC.SetYdca (Ydca ) ;
       aMbROTRPC.SetZdca (Zdca ) ;
       aMbROTRPC.SetXdir (Xdir );
       aMbROTRPC.SetYdir (Ydir );
       aMbROTRPC.SetZdir (Zdir );
       aMbROTRPC.SetEner (Ener );
       aMbROTRPC.SetReso (Reso ) ;
       aMbROTRPC.SetRSsim(RSsim) ;
       aMbROTRPC.SetESfit(ESfit) ;
       aMbROTRPC.SetRdigi(Rdigi);
       pMbIOObjectStore->Add(aMbROTRPC);
     }else{
       const MuonRecDigitTGC*    pMuonRecDigitTGC    = pMuonRecDigiStore->LiIdGetMuonRecDigitTGC(IdDigit) ;
       if (pMuonRecDigitTGC) {
         MbROTTGC aMbROTTGC;
       
         aMbROTTGC.SetIsToBeReversed(reverse) ;
       
         aMbROTTGC.SetMuonRecDigit(*pMuonRecDigitTGC) ;
         aMbROTTGC.SetLinkPrd(IdDigit) ;
         aMbROTTGC.SetXdca (Xdca ) ;
         aMbROTTGC.SetYdca (Ydca ) ;
         aMbROTTGC.SetZdca (Zdca ) ;
         aMbROTTGC.SetXdir (Xdir );
         aMbROTTGC.SetYdir (Ydir );
         aMbROTTGC.SetZdir (Zdir );
         aMbROTTGC.SetEner (Ener );
         aMbROTTGC.SetReso (Reso ) ;
         aMbROTTGC.SetRSsim(RSsim) ;
         aMbROTTGC.SetESfit(ESfit) ;
         aMbROTTGC.SetRdigi(Rdigi);
         pMbIOObjectStore->Add(aMbROTTGC);
       }
     }
   }

//ROTs Outliers
   int NberOf1stCoOutliersStart = aMuonboyComcan.NZXCAN(TrackNber)  ;
   int NberOf1stCoOutliers      = aMuonboyComcan.NZXCAN(TrackNber) 
                                + aMuonboyComcan.NZOCAN(TrackNber) ;
   for (int index=NberOf1stCoOutliersStart; index<NberOf1stCoOutliers; index++){
     int IdDigit = aMuonboyComcan.Index1stCo(TrackNber,index);
     double Xdca  = aMuonboyComcan.XDCCAN(TrackNber,index)*10.;
     double Ydca  = aMuonboyComcan.YDCCAN(TrackNber,index)*10.;
     double Zdca  = aMuonboyComcan.ZDCCAN(TrackNber,index)*10.;
     double Xdir  = aMuonboyComcan.XDCdCAN(TrackNber,index)*10.;
     double Ydir  = aMuonboyComcan.YDCdCAN(TrackNber,index)*10.;
     double Zdir  = aMuonboyComcan.ZDCdCAN(TrackNber,index)*10.;
     double Ener  = aMuonboyComcan.ENCCAN(TrackNber,index)*1000.;
     double Sdca  = aMuonboyComcan.SDCCAN(TrackNber,index)*10.;
     double Reso  = aMuonboyComcan.RDCCAN(TrackNber,index)*10.;
     double Rdigi = aMuonboyComcan.RZXCAN(TrackNber,index)*10.;
     double Rtrak = aMuonboyComcan.RZICAN(TrackNber,index)*10.;
     double RZsim = aMuonboyComcan.RZISIM(TrackNber,index)*10.;
     double EZfit = aMuonboyComcan.EZIFIT(TrackNber,index)*10.;
     const MuonRecDigitMDT*    pMuonRecDigitMDT    = pMuonRecDigiStore->LiIdGetMuonRecDigitMDT   (IdDigit) ;
     if (pMuonRecDigitMDT) {
       MbROTMDT aMbROTMDT;

       aMbROTMDT.SetOutlier();          
       
       aMbROTMDT.SetIsToBeReversed(reverse) ;
       
       aMbROTMDT.SetMuonRecDigit(*pMuonRecDigitMDT) ;
       aMbROTMDT.SetLinkPrd(IdDigit) ;
       aMbROTMDT.SetXdca (Xdca );
       aMbROTMDT.SetYdca (Ydca );
       aMbROTMDT.SetZdca (Zdca );
       aMbROTMDT.SetXdir (Xdir );
       aMbROTMDT.SetYdir (Ydir );
       aMbROTMDT.SetZdir (Zdir );
       aMbROTMDT.SetEner (Ener );
       aMbROTMDT.SetSdca (Sdca );
       aMbROTMDT.SetReso (Reso );
       aMbROTMDT.SetRZsim(RZsim);
       aMbROTMDT.SetEZfit(EZfit);
       aMbROTMDT.SetRdigi(Rdigi);
       aMbROTMDT.SetRtrak(Rtrak);
       pMbIOObjectStore->Add(aMbROTMDT);
     }
   }
       
//Holes     
//RPCHOLEWARNING      
   int DoRpcHole = 0 ;
   int NberOfHoles = aMuonboyComcan.NHXCAN(TrackNber);
   for (int index=0; index<NberOfHoles; index++){
     double Xdca  = aMuonboyComcan.XHACAN(TrackNber,index)*10.;
     double Ydca  = aMuonboyComcan.YHACAN(TrackNber,index)*10.;
     double Zdca  = aMuonboyComcan.ZHACAN(TrackNber,index)*10.;
     double Xdir  = aMuonboyComcan.XHdCAN(TrackNber,index)*10.;
     double Ydir  = aMuonboyComcan.YHdCAN(TrackNber,index)*10.;
     double Zdir  = aMuonboyComcan.ZHdCAN(TrackNber,index)*10.;
     double Ener  = aMuonboyComcan.ENHCAN(TrackNber,index)*1000.;
//MDT
     if (aMuonboyComcan.TYHCAN(TrackNber,index) == 1 ){
       MbHoleMDT aMbHoleMDT;
       
       aMbHoleMDT.SetIsToBeReversed(reverse) ;
       
       aMbHoleMDT.SetXdca ( Xdca ) ;
       aMbHoleMDT.SetYdca ( Ydca ) ;
       aMbHoleMDT.SetZdca ( Zdca ) ;
       aMbHoleMDT.SetXdir ( Xdir ) ;
       aMbHoleMDT.SetYdir ( Ydir ) ;
       aMbHoleMDT.SetZdir ( Zdir ) ;
       aMbHoleMDT.SetEner ( Ener ) ;
       int IdentEncoded = aMuonboyComcan.IHXCAN(TrackNber,index) ;
       int Jtyp                ;
       int Jff                 ;
       int Jzz                 ;
       int Job                 ;
       std::string StationName ;
       int StationNber         ;
       int StationEta          ;
       int StationPhi          ;

       int JSL                 ;
       int JTUBE               ;
       
       int Multilayer          ;
       int TubeLayer           ;
       int Tube                ;

       GetDecoIdentAMDCMDT(IdentEncoded, 
                           StationName ,
                           Jtyp, Jff, Jzz, Job, 
                           JSL, JTUBE,
                           StationEta, StationPhi, 
                           Multilayer, TubeLayer, Tube
                          );
                          
//Wanrning JFL samedi 11 avril 2009, 15:39:35 (UTC+0200): COULD BE A WEAK POINT
       StationNber = Jtyp - 1 ;
       aMbHoleMDT.SetAreAmdbIds ( 1 ) ;
       aMbHoleMDT.SetSGStationNber( StationNber ) ;
       aMbHoleMDT.SetSGStationName( StationName ) ;
       aMbHoleMDT.SetSGStationEta ( StationEta  ) ;
       aMbHoleMDT.SetSGStationPhi ( StationPhi  ) ;
       aMbHoleMDT.SetJTYP         ( Jtyp        ) ;
       aMbHoleMDT.SetJFF          ( Jff         ) ;
       aMbHoleMDT.SetJZZ          ( Jzz         ) ;
       aMbHoleMDT.SetJOB          ( Job         ) ;
       
       aMbHoleMDT.SetSGMultilayer ( Multilayer  ) ;
       aMbHoleMDT.SetSGTubeLayer  ( TubeLayer   ) ;
       aMbHoleMDT.SetSGTube       ( Tube        ) ;
       
       aMbHoleMDT.SetJSL          ( JSL         ) ;
       aMbHoleMDT.SetJTUBE        ( JTUBE       ) ;
       
       pMbIOObjectStore->Add(aMbHoleMDT);
     }
//RPC
     if ( DoRpcHole == 1 && aMuonboyComcan.TYHCAN(TrackNber,index) == 2 ){
       MbHoleRPC aMbHoleRPC;
       
       aMbHoleRPC.SetIsToBeReversed(reverse) ;
       
       aMbHoleRPC.SetXdca ( Xdca ) ;
       aMbHoleRPC.SetYdca ( Ydca ) ;
       aMbHoleRPC.SetZdca ( Zdca ) ;
       aMbHoleRPC.SetXdir ( Xdir ) ;
       aMbHoleRPC.SetYdir ( Ydir ) ;
       aMbHoleRPC.SetZdir ( Zdir ) ;
       aMbHoleRPC.SetEner ( Ener ) ;
       int IdentEncoded = aMuonboyComcan.IHXCAN(TrackNber,index) ;
       int Jtyp                ;
       int Jff                 ;
       int Jzz                 ;
       int Job                 ;
       std::string StationName ;
       int StationNber         ;
       int StationEta          ;
       int StationPhi          ;

       int JSPLI               ;
       int JSL                 ;
       int JSZ                 ;
       int JSTRI               ;
    
       int DoubletR            ;
       int DoubletZ            ;
       int DoubletPhi          ;
       int GasGap              ;
       int MeasuresPhi         ;
       int Strip               ;

       GetDecoIdentAMDCRPC(IdentEncoded, 
                           StationName ,
                           Jtyp, Jff, Jzz, Job, 
                           JSPLI, JSL, JSZ, JSTRI,
                           StationEta, StationPhi, 
                           DoubletR, DoubletZ, DoubletPhi, GasGap, MeasuresPhi, Strip
                          );
                          
//Wanrning JFL samedi 11 avril 2009, 15:39:35 (UTC+0200): COULD BE A WEAK POINT
       StationNber = Jtyp - 1 ;
       aMbHoleRPC.SetAreAmdbIds ( 1 ) ;
       aMbHoleRPC.SetSGStationNber( StationNber ) ;
       aMbHoleRPC.SetSGStationName( StationName ) ;
       aMbHoleRPC.SetSGStationEta ( StationEta  ) ;
       aMbHoleRPC.SetSGStationPhi ( StationPhi  ) ;
       aMbHoleRPC.SetJTYP         ( Jtyp        ) ;
       aMbHoleRPC.SetJFF          ( Jff         ) ;
       aMbHoleRPC.SetJZZ          ( Jzz         ) ;
       aMbHoleRPC.SetJOB          ( Job         ) ;

       aMbHoleRPC.SetSGDoubletR   ( DoubletR    ) ;
       aMbHoleRPC.SetSGDoubletZ   ( DoubletZ    ) ;
       aMbHoleRPC.SetSGDoubletPhi ( DoubletPhi  ) ;
       aMbHoleRPC.SetSGGasGap     ( GasGap      ) ;
       aMbHoleRPC.SetSGMeasuresPhi( MeasuresPhi ) ;
       aMbHoleRPC.SetSGStrip      ( Strip       ) ;

       aMbHoleRPC.SetJSPLI        ( JSPLI       ) ;
       aMbHoleRPC.SetJSL          ( JSL         ) ;
       aMbHoleRPC.SetJSZ          ( JSZ         ) ;
       aMbHoleRPC.SetJSTRI        ( JSTRI       ) ;
       
       pMbIOObjectStore->Add(aMbHoleRPC);
     }
//TGC
     if (aMuonboyComcan.TYHCAN(TrackNber,index) == 3 ){
       MbHoleTGC aMbHoleTGC;
       
       aMbHoleTGC.SetIsToBeReversed(reverse) ;
       
       aMbHoleTGC.SetXdca ( Xdca ) ;
       aMbHoleTGC.SetYdca ( Ydca ) ;
       aMbHoleTGC.SetZdca ( Zdca ) ;
       aMbHoleTGC.SetXdir ( Xdir ) ;
       aMbHoleTGC.SetYdir ( Ydir ) ;
       aMbHoleTGC.SetZdir ( Zdir ) ;
       aMbHoleTGC.SetEner ( Ener ) ;
       int IdentEncoded = aMuonboyComcan.IHXCAN(TrackNber,index) ;
       int Jtyp                ;
       int Jff                 ;
       int Jzz                 ;
       int Job                 ;
       std::string StationName ;
       int StationNber         ;
       int StationEta          ;
       int StationPhi          ;

       int JSPLI               ;
       int JSL                 ;
       int JSZ                 ;
       int JSTRI               ;
       
       int GasGap              ;
       int IsStrip             ;
       int Channel             ;

       GetDecoIdentAMDCTGC(IdentEncoded, 
                           StationName ,
                           Jtyp, Jff, Jzz, Job, 
                           JSPLI,JSL, JSZ, JSTRI,
                           StationEta, StationPhi, 
                           GasGap, IsStrip, Channel
                          );
                          
//Wanrning JFL samedi 11 avril 2009, 15:39:35 (UTC+0200): COULD BE A WEAK POINT
       StationNber = Jtyp - 1 ;
       aMbHoleTGC.SetAreAmdbIds ( 1 ) ;
       aMbHoleTGC.SetSGStationNber( StationNber ) ;
       aMbHoleTGC.SetSGStationName( StationName ) ;
       aMbHoleTGC.SetSGStationEta ( StationEta  ) ;
       aMbHoleTGC.SetSGStationPhi ( StationPhi  ) ;
       aMbHoleTGC.SetJTYP         ( Jtyp        ) ;
       aMbHoleTGC.SetJFF          ( Jff         ) ;
       aMbHoleTGC.SetJZZ          ( Jzz         ) ;
       aMbHoleTGC.SetJOB          ( Job         ) ;
       
       aMbHoleTGC.SetSGGasGap     ( GasGap      ) ;
       aMbHoleTGC.SetSGIsStrip    ( IsStrip     ) ;
       aMbHoleTGC.SetSGChannel    ( Channel     ) ;
       
       aMbHoleTGC.SetJSPLI        ( JSPLI       ) ;
       aMbHoleTGC.SetJSL          ( JSL         ) ;
       aMbHoleTGC.SetJSZ          ( JSZ         ) ;
       aMbHoleTGC.SetJSTRI        ( JSTRI       ) ;
       
       pMbIOObjectStore->Add(aMbHoleTGC);
     }
//CSC
     if (aMuonboyComcan.TYHCAN(TrackNber,index) == 4 ){
       MbHoleCSC aMbHoleCSC;
       
       aMbHoleCSC.SetIsToBeReversed(reverse) ;
       
       aMbHoleCSC.SetXdca ( Xdca ) ;
       aMbHoleCSC.SetYdca ( Ydca ) ;
       aMbHoleCSC.SetZdca ( Zdca ) ;
       aMbHoleCSC.SetXdir ( Xdir ) ;
       aMbHoleCSC.SetYdir ( Ydir ) ;
       aMbHoleCSC.SetZdir ( Zdir ) ;
       aMbHoleCSC.SetEner ( Ener ) ;
       int IdentEncoded = aMuonboyComcan.IHXCAN(TrackNber,index) ;
       int Jtyp                ;
       int Jff                 ;
       int Jzz                 ;
       int Job                 ;
       std::string StationName ;
       int StationNber         ;
       int StationEta          ;
       int StationPhi          ;

       int    JSL              ;
       int    JSTRI            ;
       double SZFLAG           ;
       
       int ChamberLayer        ;
       int WireLayer           ;
       int MeasuresPhi         ;
       int Strip               ;

       GetDecoIdentAMDCCSC(IdentEncoded, 
                           StationName ,
                           Jtyp, Jff, Jzz, Job, 
                           JSL, JSTRI, SZFLAG,
                           StationEta, StationPhi, 
                           ChamberLayer, WireLayer, MeasuresPhi, Strip
                          );
                          
//Wanrning JFL samedi 11 avril 2009, 15:39:35 (UTC+0200): COULD BE A WEAK POINT
       StationNber = Jtyp - 1 ;
       aMbHoleCSC.SetAreAmdbIds ( 1 ) ;
       aMbHoleCSC.SetSGStationNber( StationNber ) ;
       aMbHoleCSC.SetSGStationName( StationName ) ;
       aMbHoleCSC.SetSGStationEta ( StationEta  ) ;
       aMbHoleCSC.SetSGStationPhi ( StationPhi  ) ;
       aMbHoleCSC.SetJTYP         ( Jtyp        ) ;
       aMbHoleCSC.SetJFF          ( Jff         ) ;
       aMbHoleCSC.SetJZZ          ( Jzz         ) ;
       aMbHoleCSC.SetJOB          ( Job         ) ;
       
       aMbHoleCSC.SetSGChamberLayer ( ChamberLayer) ;
       aMbHoleCSC.SetSGWireLayer    ( WireLayer   ) ;
       aMbHoleCSC.SetSGMeasuresPhi  ( MeasuresPhi ) ;
       aMbHoleCSC.SetSGStrip        ( Strip       ) ;
       
       aMbHoleCSC.SetJSL          ( JSL         ) ;
       aMbHoleCSC.SetJSTRI        ( JSTRI       ) ;
       aMbHoleCSC.SetSZFLAG       ( SZFLAG      ) ;
       
       pMbIOObjectStore->Add(aMbHoleCSC);
     }
   }
       
// Store Nu Sca
   int NbMasc = aMuonboyComcan.NBMSXX(TrackNber);
   for (int index=0; index<NbMasc; index++){
//JFL: jeudi 25 juin 2009, 15:34:12 (UTC+0200)
//Remove mu sca only points 
//for the rest allow both Eloss and Mu Sc
     int TempoLLMSXX = aMuonboyComcan.LLMSXX(TrackNber,index) ;
     if ( TempoLLMSXX != 6 ){
       int TempoLLMSXX = 7 ;
       MbMuSc aMbMuSc ;
       
       aMbMuSc.SetIsToBeReversed(reverse) ;
       
       aMbMuSc.SetRRMASC( aMuonboyComcan.RRMSXX(TrackNber,index) );
       aMbMuSc.SetLLMASC( TempoLLMSXX );
       aMbMuSc.SetX0MASC( aMuonboyComcan.X0MSXX(TrackNber,index) );
       aMbMuSc.SetS1MASC( aMuonboyComcan.S1MSXX(TrackNber,index) );
       aMbMuSc.SetS2MASC( aMuonboyComcan.S2MSXX(TrackNber,index) );
       aMbMuSc.SetTHMASC( aMuonboyComcan.THMSXX(TrackNber,index) );
/////////////////////////////////////////////////////// 
//      BE AWARE:
//      IN MUONBOY             D_E = A * E + B
//      USUAL  CONVENTION IS   D_E = A + B * E
//      SO ONE MUST EXCHANGE A AND B ROLES
       double ALMASC_V   = aMuonboyComcan.EAMSXX(TrackNber,index) - 1.    ;
       double BLMASC_V   = aMuonboyComcan.EBMSXX(TrackNber,index)*1000.   ;
       aMbMuSc.SetALMASC( BLMASC_V );
       aMbMuSc.SetBLMASC( ALMASC_V );
/////////////////////////////////////////////////////// 
       double XposMASC_in  = aMuonboyComcan.VinVouXX(TrackNber,index,0) * 10. ;
       double YposMASC_in  = aMuonboyComcan.VinVouXX(TrackNber,index,1) * 10. ;
       double ZposMASC_in  = aMuonboyComcan.VinVouXX(TrackNber,index,2) * 10. ;
       double XdirMASC_in  = aMuonboyComcan.VinVouXX(TrackNber,index,3) * 10. ;
       double YdirMASC_in  = aMuonboyComcan.VinVouXX(TrackNber,index,4) * 10. ;
       double ZdirMASC_in  = aMuonboyComcan.VinVouXX(TrackNber,index,5) * 10. ;
       double Xdir2MASC_in = aMuonboyComcan.VinVouXX2(TrackNber,index,3) * 10. ;
       double Ydir2MASC_in = aMuonboyComcan.VinVouXX2(TrackNber,index,4) * 10. ;
       double Zdir2MASC_in = aMuonboyComcan.VinVouXX2(TrackNber,index,5) * 10. ;
       aMbMuSc.SetPosDirMASC( 
                             XposMASC_in  , 
                             YposMASC_in  ,
                             ZposMASC_in  ,
                             XdirMASC_in  ,
                             YdirMASC_in  ,
                             ZdirMASC_in 
                            );
       aMbMuSc.SetDir2MASC( 
                           Xdir2MASC_in  ,
                           Ydir2MASC_in  ,
                           Zdir2MASC_in 
                          );
       double PmomMASC_in = aMuonboyComcan.VinVouXX(TrackNber,index,6) * 1000. ;
       aMbMuSc.SetPmomMASC( PmomMASC_in );
       double Pmom2MASC_in = aMuonboyComcan.VinVouXX2(TrackNber,index,6) * 1000. ;
       aMbMuSc.SetPmom2MASC( Pmom2MASC_in );
       aMbMuSc.SetChgeMASC( aMuonboyComcan.VinVouXX(TrackNber,index,7) );
       pMbIOObjectStore->Add(aMbMuSc);
     }
   }
      
// Sort the MbIOObjectStore
   double XrefStart = aMuonboyComcan.VXXCAN(TrackNber,0)*10. ;
   double YrefStart = aMuonboyComcan.VXXCAN(TrackNber,1)*10. ;
   double ZrefStart = aMuonboyComcan.VXXCAN(TrackNber,2)*10. ;
   if ( FlgCan == 1 ){
     XrefStart = aMuonboyComcan.VZZCAN(TrackNber,0)*10. ;
     YrefStart = aMuonboyComcan.VZZCAN(TrackNber,1)*10. ;
     ZrefStart = aMuonboyComcan.VZZCAN(TrackNber,2)*10. ;
   }
   XrefStart = aMuonboyComcan.VZZCAN(TrackNber,0)*10. ;
   YrefStart = aMuonboyComcan.VZZCAN(TrackNber,1)*10. ;
   ZrefStart = aMuonboyComcan.VZZCAN(TrackNber,2)*10. ;
   pMbIOObjectStore->SortElements(XrefStart,YrefStart,ZrefStart);
   
}
