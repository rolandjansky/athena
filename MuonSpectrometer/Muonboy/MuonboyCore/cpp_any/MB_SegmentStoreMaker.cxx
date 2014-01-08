/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MB_SegmentStoreMaker.h"

#include "MboyCoreEvt/MuonRecDigiStore.h"
#include "MboyCoreEvt/MB_SegmentStore.h"

#include "MuonboyCore/MuonboyCointr.h"
#include "MuonboyCore/GetDecoIdentAMDC.h"
#include "MuonboyCore/MuonboyCodrat.h"
 
MB_SegmentStoreMaker::MB_SegmentStoreMaker(){}
MB_SegmentStoreMaker::~MB_SegmentStoreMaker(){}

 
void MB_SegmentStoreMaker::initialize(){}  
 
void MB_SegmentStoreMaker::execute
  (const MuonRecDigiStore* pMuonRecDigiStore,
         MB_SegmentStore*  pMB_SegmentStore){

   if ( pMB_SegmentStore == 0 ){
     std::cout << " MB_SegmentStoreMaker::execute pMB_SegmentStore is 0 " << std::endl; 
     return ;
   }
   if ( pMuonRecDigiStore == 0 ){
     std::cout << " MB_SegmentStoreMaker::execute pMuonRecDigiStore is 0 " << std::endl; 
     return ;
   }  

// //*Take a MuonboyCodrat
//   MuonboyCodrat aMuonboyCodrat;
//   int o_NMATMA = aMuonboyCodrat.GetNMATMA();
//   for (int ISC_item=0; ISC_item<o_NMATMA; ISC_item++){
//     int o_NBDRAT = aMuonboyCodrat.GetNBDRAT(ISC_item);
//     for (int ID_item=0; ID_item<o_NBDRAT; ID_item++){
//     
//                     int    m_ISC0 = ISC_item + 1 ;
//       int J12 = 0 ; int    m_IWDRAT1 = aMuonboyCodrat.GetIWDRAT(ID_item,J12,ISC_item) ;
//           J12 = 1 ; int    m_IWDRAT2 = aMuonboyCodrat.GetIWDRAT(ID_item,J12,ISC_item) ;
//                     int    m_JFDRAT  = aMuonboyCodrat.GetJFDRAT(ID_item,    ISC_item) ;
//                     double m_FFDRAT  = aMuonboyCodrat.GetFFDRAT(ID_item,    ISC_item) ;
//                     double m_GGDRAT  = aMuonboyCodrat.GetGGDRAT(ID_item,    ISC_item) ;
//                     double m_EZDRAT  = aMuonboyCodrat.GetEZDRAT(ID_item,    ISC_item) ;
//           J12 = 0 ; double m_SSDRAT1 = aMuonboyCodrat.GetSSDRAT(ID_item,J12,ISC_item) * 10. ;
//           J12 = 0 ; double m_ZZDRAT1 = aMuonboyCodrat.GetZZDRAT(ID_item,J12,ISC_item) * 10. ;
//           J12 = 0 ; double m_TTDRAT1 = aMuonboyCodrat.GetTTDRAT(ID_item,J12,ISC_item) * 10. ;
//           J12 = 1 ; double m_SSDRAT2 = aMuonboyCodrat.GetSSDRAT(ID_item,J12,ISC_item) * 10. ;
//           J12 = 1 ; double m_ZZDRAT2 = aMuonboyCodrat.GetZZDRAT(ID_item,J12,ISC_item) * 10. ;
//           J12 = 1 ; double m_TTDRAT2 = aMuonboyCodrat.GetTTDRAT(ID_item,J12,ISC_item) * 10. ;
//       
//       std::cout <<  "***********************************" << std::endl ;
//       std::cout <<  "                              m_ISC0    " << m_ISC0     
//              << " m_IWDRAT  " << m_IWDRAT1 << " " << m_IWDRAT2  
//              << " m_JFDRAT  " << m_JFDRAT  << std::endl ;
//       std::cout <<  "                              m_FFDRAT   m_GGDRAT " << m_FFDRAT << " " << m_GGDRAT  
//              << " m_EZDRAT  " << m_EZDRAT  << std::endl ;
//       std::cout <<  "                              m_SS/ZZ/TTDRAT 1 " << m_SSDRAT1 << " " << m_ZZDRAT1  << " " << m_TTDRAT1 << std::endl ;
//       std::cout <<  "                              m_SS/ZZ/TTDRAT 2 " << m_SSDRAT2 << " " << m_ZZDRAT2  << " " << m_TTDRAT2 << std::endl ;
//       std::cout <<  "***********************************" << std::endl ;
// 
//     }
//   }

//*Take a MuonboyCointr
  MuonboyCointr aMuonboyCointr;

//*Loop on Segments candidates
   int NberOfSegments = aMuonboyCointr.NBINTR();
   
   for (int SegmentNber=0; SegmentNber<NberOfSegments; SegmentNber++){


//   Convert from cm to mm
     double X_Segment = aMuonboyCointr.PTINTR(SegmentNber,0)*10. ;
     double Y_Segment = aMuonboyCointr.PTINTR(SegmentNber,1)*10. ;
     double Z_Segment = aMuonboyCointr.PTINTR(SegmentNber,2)*10. ;

//   Coordinates of the Vector
     double Xvec = aMuonboyCointr.VEINTR(SegmentNber,0);
     double Yvec = aMuonboyCointr.VEINTR(SegmentNber,1);
     double Zvec = aMuonboyCointr.VEINTR(SegmentNber,2);

//   Coordinates of the dx vector
     double X_dx = aMuonboyCointr.DXINTR(SegmentNber,0);
     double Y_dx = aMuonboyCointr.DXINTR(SegmentNber,1);
     double Z_dx = aMuonboyCointr.DXINTR(SegmentNber,2);

//   Coordinates of the dy vector
     double X_dy = aMuonboyCointr.DYINTR(SegmentNber,0);
     double Y_dy = aMuonboyCointr.DYINTR(SegmentNber,1);
     double Z_dy = aMuonboyCointr.DYINTR(SegmentNber,2);

//   Coordinates of the dz vector
//   double X_dz = aMuonboyCointr.DZINTR(SegmentNber,0);
//   double Y_dz = aMuonboyCointr.DZINTR(SegmentNber,1);
//   double Z_dz = aMuonboyCointr.DZINTR(SegmentNber,2);

//   Errors components
     int    E_Valid = aMuonboyCointr.IERINTR(SegmentNber);
     double E_dx    = aMuonboyCointr.ERINTR(SegmentNber,0)*0.01;
     double E_Axz   = aMuonboyCointr.ERINTR(SegmentNber,1);
     double E_dxAxz = aMuonboyCointr.ERINTR(SegmentNber,2)*0.1;
     double E_dy    = aMuonboyCointr.ERINTR(SegmentNber,3)*0.01;
     double E_Ayz   = aMuonboyCointr.ERINTR(SegmentNber,4);
     double E_dyAyz = aMuonboyCointr.ERINTR(SegmentNber,5)*0.1;

//   Quality factor
     double QualityFactor = aMuonboyCointr.CHINTR(SegmentNber);

//   Fitted T0 and its error
     double fittedt0 = aMuonboyCointr.T0INTR(SegmentNber);
     double errort0  = aMuonboyCointr.ET0INTR(SegmentNber);

//   Nber of multilayers
     int NberOfMultiLayer = aMuonboyCointr.MlNber(SegmentNber);
//   TrackSharing
     int TrackSharing = 0;

//   Station Name
     std::string StationName = aMuonboyCointr.StationName(SegmentNber);
     int ItypStationName = aMuonboyCointr.ItypStationName(SegmentNber);

//   Collect indexes of digis
     MbIOObjectStore    aMbIOObjectStore ;
     aMbIOObjectStore.SetRunNber(pMuonRecDigiStore->GetRunNber());
     aMbIOObjectStore.SetEventNber(pMuonRecDigiStore->GetEventNber());

     int NberOfTubeDigits  = aMuonboyCointr.NUINTR(SegmentNber);
     for (int index=0; index<NberOfTubeDigits; index++){
       int IdDigit = aMuonboyCointr.LinearIndex(SegmentNber,index);
       double Xdca  = aMuonboyCointr.XDINTR(SegmentNber,index)*10.;
       double Ydca  = aMuonboyCointr.YDINTR(SegmentNber,index)*10.;
       double Zdca  = aMuonboyCointr.ZDINTR(SegmentNber,index)*10.;
       double Reso  = aMuonboyCointr.RDINTR(SegmentNber,index)*10.;
       double Xdir  = aMuonboyCointr.XDdINTR(SegmentNber,index)*10.;
       double Ydir  = aMuonboyCointr.YDdINTR(SegmentNber,index)*10.;
       double Zdir  = aMuonboyCointr.ZDdINTR(SegmentNber,index)*10.;
//       
//JFL mercredi 8 avril 2009, 15:03:21 (UTC+0200)
       double ProdSca = Xvec * Xdir
                      + Yvec * Ydir
                      + Zvec * Zdir ;
       if ( ProdSca < 0. ) {
         Xdir = - Xdir ;
         Ydir = - Ydir ;
         Zdir = - Zdir ;
       }
//JFL mercredi 8 avril 2009, 15:03:21 (UTC+0200)
//       
       double Rdigi = aMuonboyCointr.Rdigi(SegmentNber,index) *10.;
       double Rtrak = aMuonboyCointr.Rtrak(SegmentNber,index) *10.;
       double RZsim =  -1.;
       double EZfit =  -1.;
       double Rsim  =  -1.;
       double Efit  =  -1.;
       const MuonRecDigitMDT*    pMuonRecDigitMDT    = pMuonRecDigiStore->LiIdGetMuonRecDigitMDT    (IdDigit) ;
       if (pMuonRecDigitMDT) {
         MbROTMDT aMbROTMDT;
         aMbROTMDT.SetMuonRecDigit(*pMuonRecDigitMDT) ;
         aMbROTMDT.SetLinkPrd(IdDigit) ;
         aMbROTMDT.SetXdca (Xdca );
         aMbROTMDT.SetYdca (Ydca );
         aMbROTMDT.SetZdca (Zdca );
         aMbROTMDT.SetXdir (Xdir );
         aMbROTMDT.SetYdir (Ydir );
         aMbROTMDT.SetZdir (Zdir );
         aMbROTMDT.SetReso (Reso );
         aMbROTMDT.SetRZsim(RZsim);
         aMbROTMDT.SetEZfit(EZfit);
         aMbROTMDT.SetRdigi(Rdigi);
         aMbROTMDT.SetRtrak(Rtrak);
         aMbIOObjectStore.Add(aMbROTMDT);
       }else{
         const MuonRecDigitCSCClu* pMuonRecDigitCSCClu = pMuonRecDigiStore->LiIdGetMuonRecDigitCSCClu    (IdDigit) ;
         if (pMuonRecDigitCSCClu) {
           MbROTCSCclu aMbROTCSCclu;
           aMbROTCSCclu.SetMuonRecDigit(*pMuonRecDigitCSCClu) ;
           aMbROTCSCclu.SetLinkPrd(IdDigit) ;
           aMbROTCSCclu.SetXdca (Xdca );
           aMbROTCSCclu.SetYdca (Ydca );
           aMbROTCSCclu.SetZdca (Zdca );
           aMbROTCSCclu.SetXdir (Xdir );
           aMbROTCSCclu.SetYdir (Ydir );
           aMbROTCSCclu.SetZdir (Zdir );
           aMbROTCSCclu.SetReso (Reso );
           aMbROTCSCclu.SetRsim (Rsim);
           aMbROTCSCclu.SetEfit (Efit);
           aMbIOObjectStore.Add(aMbROTCSCclu);
         }else{
           const MuonRecDigitCSC*    pMuonRecDigitCSC    = pMuonRecDigiStore->LiIdGetMuonRecDigitCSC    (IdDigit) ;
           if (pMuonRecDigitCSC) {
             MbROTCSC aMbROTCSC;
             aMbROTCSC.SetMuonRecDigit(*pMuonRecDigitCSC) ;
             aMbROTCSC.SetLinkPrd(IdDigit) ;
             aMbROTCSC.SetXdca (Xdca );
             aMbROTCSC.SetYdca (Ydca );
             aMbROTCSC.SetZdca (Zdca );
             aMbROTCSC.SetXdir (Xdir );
             aMbROTCSC.SetYdir (Ydir );
             aMbROTCSC.SetZdir (Zdir );
             aMbROTCSC.SetReso (Reso );
             aMbROTCSC.SetRZsim(RZsim);
             aMbROTCSC.SetEZfit(EZfit);
             aMbIOObjectStore.Add(aMbROTCSC);
           }
         }
       }
     }
     
     int NberOfStripDigits = aMuonboyCointr.NAINTR(SegmentNber);
     for (int index=NberOfTubeDigits; index<NberOfTubeDigits+NberOfStripDigits; index++){
       int IdDigit = aMuonboyCointr.LinearIndex(SegmentNber,index);
       double Xdca  = aMuonboyCointr.XDINTR(SegmentNber,index)*10.;
       double Ydca  = aMuonboyCointr.YDINTR(SegmentNber,index)*10.;
       double Zdca  = aMuonboyCointr.ZDINTR(SegmentNber,index)*10.;
       double Reso  = aMuonboyCointr.RDINTR(SegmentNber,index)*10.;
       double Xdir  = aMuonboyCointr.XDdINTR(SegmentNber,index)*10.;
       double Ydir  = aMuonboyCointr.YDdINTR(SegmentNber,index)*10.;
       double Zdir  = aMuonboyCointr.ZDdINTR(SegmentNber,index)*10.;
//       
//JFL mercredi 8 avril 2009, 15:03:21 (UTC+0200)
       double ProdSca = Xvec * Xdir
                      + Yvec * Ydir
                      + Zvec * Zdir ;
       if ( ProdSca < 0. ) {
         Xdir = - Xdir ;
         Ydir = - Ydir ;
         Zdir = - Zdir ;
       }
//JFL mercredi 8 avril 2009, 15:03:21 (UTC+0200)
//       
       double RSsim =  -1.;
       double ESfit =  -1.;
       const MuonRecDigitRPC*    pMuonRecDigitRPC    = pMuonRecDigiStore->LiIdGetMuonRecDigitRPC    (IdDigit) ;
       if (pMuonRecDigitRPC) {
         MbROTRPC aMbROTRPC;
         aMbROTRPC.SetMuonRecDigit(*pMuonRecDigitRPC) ;
         aMbROTRPC.SetLinkPrd(IdDigit) ;
         aMbROTRPC.SetXdca (Xdca ) ;
         aMbROTRPC.SetYdca (Ydca ) ;
         aMbROTRPC.SetZdca (Zdca ) ;
         aMbROTRPC.SetXdir (Xdir );
         aMbROTRPC.SetYdir (Ydir );
         aMbROTRPC.SetZdir (Zdir );
         aMbROTRPC.SetReso (Reso ) ;
         aMbROTRPC.SetRSsim(RSsim) ;
         aMbROTRPC.SetESfit(ESfit) ;
         aMbIOObjectStore.Add(aMbROTRPC);
       }else{
         const MuonRecDigitTGC*    pMuonRecDigitTGC    = pMuonRecDigiStore->LiIdGetMuonRecDigitTGC    (IdDigit) ;
         if (pMuonRecDigitTGC) {
           MbROTTGC aMbROTTGC;
           aMbROTTGC.SetMuonRecDigit(*pMuonRecDigitTGC) ;
           aMbROTTGC.SetLinkPrd(IdDigit) ;
           aMbROTTGC.SetXdca (Xdca ) ;
           aMbROTTGC.SetYdca (Ydca ) ;
           aMbROTTGC.SetZdca (Zdca ) ;
           aMbROTTGC.SetXdir (Xdir );
           aMbROTTGC.SetYdir (Ydir );
           aMbROTTGC.SetZdir (Zdir );
           aMbROTTGC.SetReso (Reso ) ;
           aMbROTTGC.SetRSsim(RSsim) ;
           aMbROTTGC.SetESfit(ESfit) ;
           aMbIOObjectStore.Add(aMbROTTGC);
         }
       }
     }

//Holes     
//RPCHOLEWARNING      
     int DoRpcHole = 0 ;
     int NberOfHoles = aMuonboyCointr.NHINTR(SegmentNber);
     for (int index=0; index<NberOfHoles; index++){
       double Xdca  = aMuonboyCointr.XHINTR(SegmentNber,index)*10.;
       double Ydca  = aMuonboyCointr.YHINTR(SegmentNber,index)*10.;
       double Zdca  = aMuonboyCointr.ZHINTR(SegmentNber,index)*10.;
       double Xdir  = aMuonboyCointr.XHdINTR(SegmentNber,index)*10.;
       double Ydir  = aMuonboyCointr.YHdINTR(SegmentNber,index)*10.;
       double Zdir  = aMuonboyCointr.ZHdINTR(SegmentNber,index)*10.;
//       
//JFL mercredi 8 avril 2009, 15:03:21 (UTC+0200)
       double ProdSca = Xvec * Xdir
                      + Yvec * Ydir
                      + Zvec * Zdir ;
       if ( ProdSca < 0. ) {
         Xdir = - Xdir ;
         Ydir = - Ydir ;
         Zdir = - Zdir ;
       }
//JFL mercredi 8 avril 2009, 15:03:21 (UTC+0200)
//       
//MDT
       if (aMuonboyCointr.TYHINTR(SegmentNber,index) == 1 ) {
         MbHoleMDT aMbHoleMDT;
         aMbHoleMDT.SetXdca ( Xdca ) ;
         aMbHoleMDT.SetYdca ( Ydca ) ;
         aMbHoleMDT.SetZdca ( Zdca ) ;
         aMbHoleMDT.SetXdir ( Xdir ) ;
         aMbHoleMDT.SetYdir ( Ydir ) ;
         aMbHoleMDT.SetZdir ( Zdir ) ;
         int IdentEncoded = aMuonboyCointr.IHINTR(SegmentNber,index) ;
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

         aMbIOObjectStore.Add(aMbHoleMDT);
       }
//RPC
       if ( DoRpcHole == 1 && aMuonboyCointr.TYHINTR(SegmentNber,index) == 2 ){
         MbHoleRPC aMbHoleRPC;
         aMbHoleRPC.SetXdca ( Xdca ) ;
         aMbHoleRPC.SetYdca ( Ydca ) ;
         aMbHoleRPC.SetZdca ( Zdca ) ;
         aMbHoleRPC.SetXdir ( Xdir ) ;
         aMbHoleRPC.SetYdir ( Ydir ) ;
         aMbHoleRPC.SetZdir ( Zdir ) ;
         int IdentEncoded = aMuonboyCointr.IHINTR(SegmentNber,index) ;
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
         
         aMbIOObjectStore.Add(aMbHoleRPC);
       }
//TGC
       if (aMuonboyCointr.TYHINTR(SegmentNber,index) == 3 ){
         MbHoleTGC aMbHoleTGC;
         aMbHoleTGC.SetXdca ( Xdca ) ;
         aMbHoleTGC.SetYdca ( Ydca ) ;
         aMbHoleTGC.SetZdca ( Zdca ) ;
         aMbHoleTGC.SetXdir ( Xdir ) ;
         aMbHoleTGC.SetYdir ( Ydir ) ;
         aMbHoleTGC.SetZdir ( Zdir ) ;
         int IdentEncoded = aMuonboyCointr.IHINTR(SegmentNber,index) ;
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
         
         aMbIOObjectStore.Add(aMbHoleTGC);
       }
//CSC
       if (aMuonboyCointr.TYHINTR(SegmentNber,index) == 4 ){
         MbHoleCSC aMbHoleCSC;
         aMbHoleCSC.SetXdca ( Xdca ) ;
         aMbHoleCSC.SetYdca ( Ydca ) ;
         aMbHoleCSC.SetZdca ( Zdca ) ;
         aMbHoleCSC.SetXdir ( Xdir ) ;
         aMbHoleCSC.SetYdir ( Ydir ) ;
         aMbHoleCSC.SetZdir ( Zdir ) ;
         int IdentEncoded = aMuonboyCointr.IHINTR(SegmentNber,index) ;
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
         
         aMbIOObjectStore.Add(aMbHoleCSC);
       }
     }

//*  Sort the MbIOObjectStore
     double XrefStart = 0. ;
     double YrefStart = 0. ;
     double ZrefStart = 0. ;
     aMbIOObjectStore.SortElements(XrefStart,YrefStart,ZrefStart);

//Losseness
     bool IsStrict =  true ;
     if (  aMuonboyCointr.IKINTR(SegmentNber) < 0 ) IsStrict =  false ;
     
//*  Create a MB_Segment and store it in the store
     MB_Segment aMB_Segment(
      X_Segment,Y_Segment,Z_Segment,
      Xvec,Yvec,Zvec,
      X_dx,Y_dx,Z_dx,
      X_dy,Y_dy,Z_dy,
      E_Valid,
      E_dx,E_Axz,E_dxAxz,
      E_dy,E_Ayz,E_dyAyz,
      QualityFactor,
      NberOfMultiLayer,
      TrackSharing,
      StationName,
      ItypStationName,
      fittedt0,
      errort0,
      aMbIOObjectStore,
      IsStrict
     );
     pMB_SegmentStore->Add(aMB_Segment);
  }

}
 
void MB_SegmentStoreMaker::finalize(){}

