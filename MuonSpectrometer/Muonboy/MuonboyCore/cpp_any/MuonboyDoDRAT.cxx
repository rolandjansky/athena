/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyDoDRAT.h"

#include "MboyCoreEvt/MbDRAT.h"
#include "MboyCoreEvt/MbIDTagMDT.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"
#include "MuonboyCore/GetDecoIdentAMDC.h"

MuonboyDoDRAT::MuonboyDoDRAT(){}

MuonboyDoDRAT::~MuonboyDoDRAT(){}

void MuonboyDoDRAT::initialize(){}


void MuonboyDoDRAT::execute(
        MbDRAT*        pMbDRAT
){
 if (pMbDRAT == 0){
   std::cout << "MuonboyDoDRAT::execute pMbDRAT ==0  " << std::endl ;   
   return ;
 }

//  std::cout << "MuonboyDoDRAT::execute (pMbDRAT->m_VectorOfMbIDTagMDT).size()    " << (pMbDRAT->m_VectorOfMbIDTagMDT).size()    << std::endl ;   
//  std::cout << "MuonboyDoDRAT::execute (pMbDRAT->m_VectorOfMbIDTagCSCclu).size() " << (pMbDRAT->m_VectorOfMbIDTagCSCclu).size() << std::endl ;   

 int Test = (pMbDRAT->m_VectorOfMbIDTagMDT).size() * (pMbDRAT->m_VectorOfMbIDTagCSCclu).size() ;
 if (Test != 0 ) {
   std::cout << "MuonboyDoDRAT::execute pMbDRAT contains both MDT and CSC digits = > Give up " << std::endl ;   
   return ;
 }
 if ( (pMbDRAT->m_VectorOfMbIDTagMDT).size() == 0 && (pMbDRAT->m_VectorOfMbIDTagCSCclu).size() == 0 ) {
   std::cout << "MuonboyDoDRAT::execute pMbDRAT contains no MDT and no CSC digits = > Give up " << std::endl ;   
   return ;
 }

 double XpointL = pMbDRAT->m_Xpoint / 10. ;
 double YpointL = pMbDRAT->m_Ypoint / 10. ;
 double ZpointL = pMbDRAT->m_Zpoint / 10. ;


 double XdirL   = pMbDRAT->m_Xdir         ;
 double YdirL   = pMbDRAT->m_Ydir         ;
 double ZdirL   = pMbDRAT->m_Zdir         ;

 int Has2ndCoord = 0 ;
 
 std::vector< MbIDTagRPC* >::iterator itRPC       = (pMbDRAT->m_VectorOfMbIDTagRPC).begin() ; 
 std::vector< MbIDTagRPC* >::iterator itRPC_end   = (pMbDRAT->m_VectorOfMbIDTagRPC).end()   ;
 for(; itRPC!=itRPC_end;++itRPC ){ 
   if ( Has2ndCoord == 1 ) break ;
   if ( (*itRPC)->GetSGMeasuresPhi() == 1 ){
     Has2ndCoord = 1 ;
     break;
   }
 }
 std::vector< MbIDTagTGC* >::iterator itTGC       = (pMbDRAT->m_VectorOfMbIDTagTGC).begin() ; 
 std::vector< MbIDTagTGC* >::iterator itTGC_end   = (pMbDRAT->m_VectorOfMbIDTagTGC).end()   ;
 for(; itTGC!=itTGC_end;++itTGC ){ 
   if ( Has2ndCoord == 1 ) break ;
   if ( (*itTGC)->GetSGIsStrip() == 1 ){
     Has2ndCoord = 1 ;
     break;
   }
 }
 std::vector< MbIDTagCSCclu* >::iterator itCSCclu0       = (pMbDRAT->m_VectorOfMbIDTagCSCclu).begin() ; 
 std::vector< MbIDTagCSCclu* >::iterator itCSCclu0_end   = (pMbDRAT->m_VectorOfMbIDTagCSCclu).end()   ;
 std::vector< int >::iterator itStatus                  = (pMbDRAT->m_VectorOfStatusCSCclu).begin()  ; 
 for(; itCSCclu0!=itCSCclu0_end;++itCSCclu0 ){ 
   if ( Has2ndCoord == 1 ) break ;
   if ( (*itCSCclu0)->GetSGMeasuresPhi() == 1 ){
     Has2ndCoord = 1 ;
     break;
   }
 }
 
 MbIDTag* pMbIDTag = 0 ;

 if ( (pMbDRAT->m_VectorOfMbIDTagMDT).size() ){
   int DoneTpos1 = 0 ; double Tpos1 = 0. ; 
   int DoneTpos2 = 0 ; double Tpos2 = 0. ; 
   int IW1 = 0 ;
   int IW2 = 0 ;
   int IC1 = 0 ;
   int IC2 = 0 ;
   std::vector< MbIDTagMDT* >::iterator itMDT       = (pMbDRAT->m_VectorOfMbIDTagMDT).begin() ; 
   std::vector< MbIDTagMDT* >::iterator itMDT_end   = (pMbDRAT->m_VectorOfMbIDTagMDT).end()   ;
   for(; itMDT!=itMDT_end;++itMDT ){ 
     pMbIDTag = (*itMDT) ;
     pMbDRAT->m_GGDRAT = 0 ; 
     std::string StationName = pMbIDTag->GetSGStationName () ; 
     int StationEta          = pMbIDTag->GetSGStationEta  () ;
     int StationPhi          = pMbIDTag->GetSGStationPhi  () ;
     int Multilayer = (*itMDT)->GetSGMultilayer() ;  
     int TubeLayer  = (*itMDT)->GetSGTubeLayer () ;  
     int Tube       = (*itMDT)->GetSGTube      () ;  

//      std::cout << "MuonboyDoDRAT::execute Ids In   " 
//                << " " << std::setw(3) << StationName     
//                << " " << std::setw(3) << StationEta      
//                << " " << std::setw(3) << StationPhi      
//                << " " << std::setw(3) << Multilayer      
//                << " " << std::setw(3) << TubeLayer       
//                << " " << std::setw(3) << Tube        << std::endl ;   

     int Jtyp;
     int Jff ;
     int Jzz ;
     int Job ;
     int Jsl ;
     int Jtube;
     TranslateToAmdcMDT(
                        StationName ,StationEta,StationPhi,
                        Multilayer,TubeLayer,Tube,
                        Jtyp,Jff,Jzz,
                        Job,Jsl,Jtube
                       );

//      std::cout << "MuonboyDoDRAT::execute Ids Amdc " 
//                << " " <<  std::setw(3) << Jtyp      
//                << " " <<  std::setw(3) << Jzz       
//                << " " <<  std::setw(3) << Jff       
//                << " " <<  std::setw(3) << Job       
//                << " " <<  std::setw(3) << Jsl       
//                << " " <<  std::setw(3) << Jtube << std::endl ;   

     int ISC = GetISC(Jtyp) ; 
     int IC   ;                  
     int INOF ;                  
     int INOZ ;                  
     int ICT  ;                  
     GetICINOFINOZICT(Jtyp,Jff,Jzz,Job,IC,INOF,INOZ,ICT);
     int IGEOMA = GetIGEOMA(IC,INOF,INOZ) ;
     int IW = GetIWCHA(IC,INOF,INOZ) ;

//      std::cout << "MuonboyDoDRAT::execute Ids Mboy " 
//                << " " <<  std::setw(3) << ISC        
//                << " " <<  std::setw(3) << IC       
//                << " " <<  std::setw(3) << INOF       
//                << " " <<  std::setw(3) << INOZ       
//                << " " <<  std::setw(3) << ICT        
//                << " " <<  std::setw(6) << IGEOMA     
//                << " " <<  std::setw(3) << IW     << std::endl ;   
     
     pMbDRAT->m_ISC0   =  ISC ; 
     
     pMbDRAT->m_FFDRAT = GetFCECHAV(IGEOMA ) ;     
              
     pMbDRAT->m_GGDRAT = 0. ;
     
     pMbDRAT->m_JFDRAT = Jff ;     
     
     double Tpos = GetTCECHAV(IGEOMA ) ;
     if ( DoneTpos1 == 0 ) { DoneTpos1 = 1 ; Tpos1 = Tpos ; }
     if ( DoneTpos2 == 0 ) { DoneTpos2 = 1 ; Tpos2 = Tpos ; }
     if ( Tpos1 <= Tpos ) { 
       Tpos1 = Tpos ; 
       IW1   = IW   ; 
       IC1   = IC   ; 
     }
     if ( Tpos2 >= Tpos ) { 
       Tpos2 = Tpos ; 
       IW2   = IW   ; 
       IC2   = IC   ; 
     }
   }
   
   pMbDRAT->m_EZDRAT =  999. ;
   
   if (IC1 == IC2 ){
     Tpos1 = Tpos1 + 8. ; 
     Tpos2 = Tpos2 - 8. ; 
   }
   
   if (Tpos2/Tpos1 > 1. ){
     double temp_T  = Tpos1 ;
     int    temp_IW = IW1   ; 
     int    temp_IC = IC1   ; 
     Tpos1 = Tpos2   ;
     IW1   = IW2     ;
     IC1   = IC2     ;
     Tpos2 = temp_T  ;
     IW2   = temp_IW ;
     IC2   = temp_IC ;
   }
   
   if ( ( 2 * pMbDRAT->m_ISC0    ) != IC1 ) IW1 = 0 ;
   if ( ( 2 * pMbDRAT->m_ISC0 -1 ) != IC2 ) IW2 = 0 ;
   if ( Has2ndCoord == 0 ) {
     IW1 = - IW1 ;
     IW2 = - IW2 ;
   }

//    std::cout << "MuonboyDoDRAT::execute   " 
//              << " " <<  std::setw(3) << pMbDRAT->m_ISC0        
//              << " " <<  std::setw(3) << IC1        
//              << " " <<  std::setw(3) << IW1       
//              << " " <<  std::setw(3) << IC2       
//              << " " <<  std::setw(3) << IW2   << std::endl ;   

   pMbDRAT->m_IWDRAT1 = IW1 ;
   pMbDRAT->m_IWDRAT2 = IW2 ;
   
   double CFI  = cos(pMbDRAT->m_FFDRAT) ;
   double SFI  = sin(pMbDRAT->m_FFDRAT) ;
   
   double GTpoint = CFI * XpointL + SFI * YpointL ;
   double GSpoint =-SFI * XpointL + CFI * YpointL ;
   double GZpoint = ZpointL;
   
   double GTdir   = CFI * XdirL   + SFI * YdirL ;
   double GSdir   =-SFI * XdirL   + CFI * YdirL ;
   double GZdir   = ZdirL ;
   
   if ( pMbDRAT->m_ISC0 > GetNCOUBAS2() ){
     
     pMbDRAT->m_ZZDRAT1 = ( Tpos1                                         ) * 10. ;
     pMbDRAT->m_TTDRAT1 = ( GTpoint + GTdir * ( Tpos1 - GZpoint ) / GZdir ) * 10. ;
     pMbDRAT->m_SSDRAT1 = ( GSpoint + GSdir * ( Tpos1 - GZpoint ) / GZdir ) * 10. ;
     
     pMbDRAT->m_ZZDRAT2 = ( Tpos2                                         ) * 10. ;
     pMbDRAT->m_TTDRAT2 = ( GTpoint + GTdir * ( Tpos2 - GZpoint ) / GZdir ) * 10. ;
     pMbDRAT->m_SSDRAT2 = ( GSpoint + GSdir * ( Tpos2 - GZpoint ) / GZdir ) * 10. ;
   }else{
     
     pMbDRAT->m_TTDRAT1 = ( Tpos1                                         ) * 10. ;
     pMbDRAT->m_ZZDRAT1 = ( GZpoint + GZdir * ( Tpos1 - GTpoint ) / GTdir ) * 10. ;
     pMbDRAT->m_SSDRAT1 = ( GSpoint + GSdir * ( Tpos1 - GTpoint ) / GTdir ) * 10. ;
     
     pMbDRAT->m_TTDRAT2 = ( Tpos2                                         ) * 10. ;
     pMbDRAT->m_ZZDRAT2 = ( GZpoint + GZdir * ( Tpos2 - GTpoint ) / GTdir ) * 10. ;
     pMbDRAT->m_SSDRAT2 = ( GSpoint + GSdir * ( Tpos2 - GTpoint ) / GTdir ) * 10. ;
   }
   
 }

 if ( (pMbDRAT->m_VectorOfMbIDTagCSCclu).size() ){
   int ngoodz = 0 ;
   int IMDT  ;
   int DoneTpos1 = 0 ; double Tpos1 = 0. ; 
   int DoneTpos2 = 0 ; double Tpos2 = 0. ; 
   int IW1 = 0 ;
   int IW2 = 0 ;
   int IC1 = 0 ;
   int IC2 = 0 ;
   std::vector< MbIDTagCSCclu* >::iterator itCSCclu       = (pMbDRAT->m_VectorOfMbIDTagCSCclu).begin() ; 
   std::vector< MbIDTagCSCclu* >::iterator itCSCclu_end   = (pMbDRAT->m_VectorOfMbIDTagCSCclu).end()   ;
   std::vector< int >::iterator itStatus                  = (pMbDRAT->m_VectorOfStatusCSCclu).begin()  ; 
   for(; itCSCclu!=itCSCclu_end;++itCSCclu, ++itStatus ){ 
     pMbIDTag = (*itCSCclu) ; 
     std::string StationName = pMbIDTag->GetSGStationName () ; 
     int StationEta          = pMbIDTag->GetSGStationEta  () ;
     int StationPhi          = pMbIDTag->GetSGStationPhi  () ;
     int ChamberLayer = (*itCSCclu)->GetSGChamberLayer() ;   
     int WireLayer    = (*itCSCclu)->GetSGWireLayer   () ;   
     int MeasuresPhi  = (*itCSCclu)->GetSGMeasuresPhi () ;   
     int Strip        = (*itCSCclu)->GetSGStrip       () ;   

//      std::cout << "MuonboyDoDRAT::execute Ids In   " 
//                << " " << std::setw(3) << StationName      
//                << " " << std::setw(3) << StationEta       
//                << " " << std::setw(3) << StationPhi       
//                << " " << std::setw(3) << ChamberLayer     
//                << " " << std::setw(3) << WireLayer        
//                << " " << std::setw(3) << MeasuresPhi      
//                << " " << std::setw(3) << Strip        << std::endl ;   

     int Jtyp;
     int Jff;
     int Jzz;
     int Job;
     int Jsl;
     int Jtube;
     double SZflag;
     TranslateToAmdcCSC(
                        StationName ,StationEta,StationPhi,
                        ChamberLayer,WireLayer,MeasuresPhi,Strip,
                        Jtyp,Jff,Jzz,Job,
                        Jsl,Jtube,SZflag
                       );

//      std::cout << "MuonboyDoDRAT::execute Ids Amdc " 
//                << " " << std::setw(3) << Jtyp       
//                << " " << std::setw(3) << Jzz        
//                << " " << std::setw(3) << Jff        
//                << " " << std::setw(3) << Job        
//                << " " << std::setw(3) << Jsl        
//                << " " << std::setw(3) << Jtube      
//                << " " << "    "       
//                << " " << std::setw(3) << SZflag << std::endl ;   

     int ISC = GetISC(Jtyp) ; 
     int IC   ;                  
     int INOF ;                  
     int INOZ ;                  
     int ICT  ;                  
     GetICINOFINOZICT(Jtyp,Jff,Jzz,Job,IC,INOF,INOZ,ICT);
     int IGEOMA = GetIGEOMA(IC,INOF,INOZ) ;
     int IW = GetIWCHA(IC,INOF,INOZ) ;
     IMDT = GetISTMDTV(IGEOMA) ;

//      std::cout << "MuonboyDoDRAT::execute Ids Mboy " 
//                << " " <<  std::setw(3) << ISC        
//                << " " <<  std::setw(3) << IC        
//                << " " <<  std::setw(3) << INOF       
//                << " " <<  std::setw(3) << INOZ       
//                << " " <<  std::setw(3) << ICT        
//                << " " <<  std::setw(6) << IGEOMA     
//                << " " <<  std::setw(3) << IW     
//                << " " <<  std::setw(3) << IMDT    << std::endl ;   
     
     
     pMbDRAT->m_ISC0   =  ISC ; 

     pMbDRAT->m_FFDRAT = GetFCECHAV(IGEOMA ) ;   
     
     pMbDRAT->m_GGDRAT = GetGCECHAV(IGEOMA) ;
     
     pMbDRAT->m_JFDRAT = Jff ;     

     double Tpos = GetTCECHAV(IGEOMA ) ;
     if ( DoneTpos1 == 0 ) { DoneTpos1 = 1 ; Tpos1 = Tpos ; }
     if ( DoneTpos2 == 0 ) { DoneTpos2 = 1 ; Tpos2 = Tpos ; }
     if ( Tpos1 <= Tpos ) { 
       Tpos1 = Tpos ; 
       IW1   = IW   ; 
       IC1   = IC   ; 
     }
     if ( Tpos2 >= Tpos ) { 
       Tpos2 = Tpos ; 
       IW2   = IW   ; 
       IC2   = IC   ; 
     }
     
     if ( (*itStatus) == 0 || (*itStatus) == 10 ) ngoodz +=1 ;
   } 

//    std::cout << "MuonboyDoDRAT::execute Tpos   " << Tpos   << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute ngoodz " << ngoodz << std::endl ;

   if ( ngoodz >= 4 )  pMbDRAT->m_EZDRAT = 2.   * GetRESTUB(IMDT) ;
   if ( ngoodz == 3 )  pMbDRAT->m_EZDRAT = 3.   * GetRESTUB(IMDT) ;
   if ( ngoodz == 2 )  pMbDRAT->m_EZDRAT = 6.   * GetRESTUB(IMDT) ;
   if ( ngoodz >= 1 )  pMbDRAT->m_EZDRAT = 0.25 * GetPASTUB(IMDT) ;

   if (IC1 == IC2 ){
     Tpos1 = Tpos1 + 10. ; 
     Tpos2 = Tpos2 - 10. ; 
   }

   if (Tpos2/Tpos1 > 1. ){
     double temp_T  = Tpos1 ;
     int    temp_IW = IW1   ; 
     int    temp_IC = IC1   ; 
     Tpos1 = Tpos2   ;
     IW1   = IW2     ;
     IC1   = IC2     ;
     Tpos2 = temp_T  ;
     IW2   = temp_IW ;
     IC2   = temp_IC ;
   }
   
   if ( ( 2 * pMbDRAT->m_ISC0    ) != IC1 ) IW1 = 0 ;
   if ( ( 2 * pMbDRAT->m_ISC0 -1 ) != IC2 ) IW2 = 0 ;
   if ( Has2ndCoord == 0 ) {
     IW1 = - IW1 ;
     IW2 = - IW2 ;
   }

//    std::cout << "MuonboyDoDRAT::execute   " 
//              << " " <<  std::setw(3) << pMbDRAT->m_ISC0        
//              << " " <<  std::setw(3) << IC1        
//              << " " <<  std::setw(3) << IW1       
//              << " " <<  std::setw(3) << IC2       
//              << " " <<  std::setw(3) << IW2   << std::endl ;   

   pMbDRAT->m_IWDRAT1 = IW1  ;
   pMbDRAT->m_IWDRAT2 = IW2  ;

   double CFI  = cos(pMbDRAT->m_FFDRAT) ;
   double SFI  = sin(pMbDRAT->m_FFDRAT) ;
   
   double GTpoint =  CFI * XpointL + SFI * YpointL ;
   double GSpoint = -SFI * XpointL + CFI * YpointL ;
   double GZpoint =  ZpointL;
   
   double GTdir   =  CFI * XdirL   + SFI * YdirL ;
   double GSdir   = -SFI * XdirL   + CFI * YdirL ;
   double GZdir   =  ZdirL ;
 
   double CFA  = cos(pMbDRAT->m_GGDRAT) ;
   double SFA  = sin(pMbDRAT->m_GGDRAT) ;
   
   double cscZpoint = -SFA * GZpoint + CFA * GTpoint ;
   double cscTpoint = +CFA * GZpoint + SFA * GTpoint ;
   double cscSpoint =  GSpoint ;                      
   
   double cscZdir   = -SFA * GZdir   + CFA * GTdir   ;
   double cscTdir   = +CFA * GZdir   + SFA * GTdir   ;
   double cscSdir   =  GSdir   ;
   
   double cscZ1 = cscZpoint + cscZdir * ( Tpos1 - cscTpoint ) / cscTdir ;
   double cscS1 = cscSpoint + cscSdir * ( Tpos1 - cscTpoint ) / cscTdir ;
   double cscT1 = cscTpoint + cscTdir * ( Tpos1 - cscTpoint ) / cscTdir ;
   
   double cscZ2 = cscZpoint + cscZdir * ( Tpos2 - cscTpoint ) / cscTdir ;
   double cscS2 = cscSpoint + cscSdir * ( Tpos2 - cscTpoint ) / cscTdir ;
   double cscT2 = cscTpoint + cscTdir * ( Tpos2 - cscTpoint ) / cscTdir ;

//    std::cout << "MuonboyDoDRAT::execute*XpointL   " << XpointL   << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute YpointL   " << YpointL   << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute ZpointL   " << ZpointL   << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute*GTpoint   " << GTpoint   << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute GSpoint   " << GSpoint   << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute GZpoint   " << GZpoint   << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute*cscZpoint " << cscZpoint << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute cscTpoint " << cscTpoint << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute cscSpoint " << cscSpoint << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute*cscZ2     " << cscZ2     << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute cscS2     " << cscS2     << std::endl ;
//    std::cout << "MuonboyDoDRAT::execute cscT2     " << cscT2     << std::endl ;

   pMbDRAT->m_ZZDRAT1 = ( -SFA * cscZ1 + CFA * cscT1 ) * 10. ;
   pMbDRAT->m_TTDRAT1 = ( +CFA * cscZ1 + SFA * cscT1 ) * 10. ;
   pMbDRAT->m_SSDRAT1 = (        cscS1               ) * 10. ;                   
      
   pMbDRAT->m_ZZDRAT2 = ( -SFA * cscZ2 + CFA * cscT2 ) * 10. ;
   pMbDRAT->m_TTDRAT2 = ( +CFA * cscZ2 + SFA * cscT2 ) * 10. ;
   pMbDRAT->m_SSDRAT2 = (        cscS2               ) * 10. ;                      


 }

}
void MuonboyDoDRAT::execute(
        std::vector<MbDRAT*>*    pVectorOfMbDRAT
){

  std::vector< MbDRAT* >::iterator it     = pVectorOfMbDRAT->begin()   ;
  std::vector< MbDRAT* >::iterator it_end = pVectorOfMbDRAT->end()   ;
  for(; it!=it_end;++it )execute( (*it) );

}


void MuonboyDoDRAT::finalize(){}
