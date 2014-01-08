/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyDoProtoTrack.h"

#include "MboyCoreEvt/MbDRAT.h"
#include "MboyCoreEvt/MbProtoTrack.h"

#include "MuonboyCore/GetDecoIdentAMDC.h"
#include "MuonboyCore/MuonboyCofiti.h"
#include "MuonboyCore/DealWithFit.h"

#include "MboyCoreEvt/MuonRecDigiStore.h"
#include "MboyCoreEvt/MB_TrackStore.h"
#include "MuonboyCore/MB_TrackStoreMaker.h"
#include "inirunendmuonboy.h"
#include "MuonboyCore/MuonboyCofito.h"
#include "MuonboyCore/GetDecoIdentAMDC.h"

MuonboyDoProtoTrack::MuonboyDoProtoTrack()
{
   p_MB_TrackStoreMaker = 0 ;
}

MuonboyDoProtoTrack::~MuonboyDoProtoTrack(){}

void MuonboyDoProtoTrack::initialize(){

//*Create p_MB_TrackStoreMaker and Initialize it
   p_MB_TrackStoreMaker = new MB_TrackStoreMaker() ;
   p_MB_TrackStoreMaker->initialize();     

}


MbProtoTrack*  MuonboyDoProtoTrack::find(
        MbProtoTrack*            pMbProtoTrack ,
        MbProtoTrack*            pTargetSegmentsProtoTrack 
){

  if ( (pMbProtoTrack->m_VectorOfMbDRAT).size() == 0 ){
       std::cout << "MuonboyDoProtoTrack::find pMbProtoTrack contains no MbDRAT = > Give up " << std::endl ;   
       return 0 ;
  }
  if ( (pTargetSegmentsProtoTrack->m_VectorOfMbDRAT).size() == 0 ){
       std::cout << "MuonboyDoProtoTrack::find pTargetSegmentsProtoTrack contains no MbDRAT = > Give up " << std::endl ;   
       return 0 ;
  }

  int ThereIsAnAppropriateSeeSegment = 0 ;
  std::vector< MbDRAT* >::iterator it     = (pMbProtoTrack->m_VectorOfMbDRAT).begin()   ;
  std::vector< MbDRAT* >::iterator it_end = (pMbProtoTrack->m_VectorOfMbDRAT).end()   ;
  for(; it!=it_end;++it ){
    MbDRAT* pMbDRAT = (*it) ;
    int    IWDRAT1 = pMbDRAT->m_IWDRAT1       ;
    int    IWDRAT2 = pMbDRAT->m_IWDRAT2       ;
    if ( IWDRAT1 != 0 ||  IWDRAT2 != 0 ) {
      ThereIsAnAppropriateSeeSegment = 1 ;
      break ;
    }
  }
  if ( ThereIsAnAppropriateSeeSegment == 0 ) {
       std::cout << "MuonboyDoProtoTrack::find There is no appropriate seed segment = > Give up " << std::endl ;   
       return 0;
  }

//Reset
  resetdealwithfit_();

//Load Codrat stuff
  int KOUNTERDRA_i = -1 ;
  std::vector<MbDRAT*> VectorOfMbDRATLocal ; VectorOfMbDRATLocal.push_back(0) ; VectorOfMbDRATLocal.clear() ;
  VectorOfMbDRATLocal.reserve( (pMbProtoTrack->m_VectorOfMbDRAT).size() + (pTargetSegmentsProtoTrack->m_VectorOfMbDRAT).size() ); 

  it     = (pMbProtoTrack->m_VectorOfMbDRAT).begin()   ;
  it_end = (pMbProtoTrack->m_VectorOfMbDRAT).end()   ;
  for(; it!=it_end;++it ){
    MbDRAT* pMbDRAT = (*it) ;

    VectorOfMbDRATLocal.push_back(pMbDRAT);
    KOUNTERDRA_i += 1;
    
    Add2Codrat(pMbDRAT,KOUNTERDRA_i);

  }
  it     = (pTargetSegmentsProtoTrack->m_VectorOfMbDRAT).begin()   ;
  it_end = (pTargetSegmentsProtoTrack->m_VectorOfMbDRAT).end()   ;
  for(; it!=it_end;++it ){
    MbDRAT* pMbDRAT = (*it) ;

    VectorOfMbDRATLocal.push_back(pMbDRAT);
    KOUNTERDRA_i += 1;
    
    Add2Codrat(pMbDRAT,KOUNTERDRA_i);

  }

//Set cofito (segment in prototrack) stuff
  KOUNTERDRA_i = -1 ;  
  it     = (pMbProtoTrack->m_VectorOfMbDRAT).begin()   ;
  it_end = (pMbProtoTrack->m_VectorOfMbDRAT).end()   ;
  for(; it!=it_end;++it ){

    KOUNTERDRA_i += 1;
    int ISCI = (*it)->m_ISC0 ;
    addtocofito_(ISCI,KOUNTERDRA_i) ;

  }

//Set cofiti (initial parameters of the fit) stuff
  MuonboyCofiti aMuonboyCofiti ;

  int    MboyStatusInput = pMbProtoTrack->m_MboyStatus ;  
                                        
  int    ISCI = pMbProtoTrack->m_ISC0           ; aMuonboyCofiti.SetISCI( ISCI ) ;
                                        
  double CFI  = pMbProtoTrack->m_CFI            ; aMuonboyCofiti.SetCFI ( CFI  ) ;
  double SFI  = pMbProtoTrack->m_SFI            ; aMuonboyCofiti.SetSFI ( SFI  ) ;
  double CFA  = pMbProtoTrack->m_CFA            ; aMuonboyCofiti.SetCFA ( CFA  ) ;
  double SFA  = pMbProtoTrack->m_SFA            ; aMuonboyCofiti.SetSFA ( SFA  ) ;
                                        
  double SMU  = pMbProtoTrack->m_SMU  * 100000. ; aMuonboyCofiti.SetSMU ( SMU  ) ;

  double DZT1 = pMbProtoTrack->m_DZT1 / 10.     ; aMuonboyCofiti.SetDZT1( DZT1 ) ;
  double DS1  = pMbProtoTrack->m_DS1  / 10.     ; aMuonboyCofiti.SetDS1 ( DS1  ) ;
  double DZT2 = pMbProtoTrack->m_DZT2 / 10.     ; aMuonboyCofiti.SetDZT2( DZT2 ) ;
  double DS2  = pMbProtoTrack->m_DS2  / 10.     ; aMuonboyCofiti.SetDS2 ( DS2  ) ;
                                        
  double Z1   = pMbProtoTrack->m_Z1   / 10.     ; aMuonboyCofiti.SetZ1  ( Z1   ) ;
  double T1   = pMbProtoTrack->m_T1   / 10.     ; aMuonboyCofiti.SetT1  ( T1   ) ;
  double S1   = pMbProtoTrack->m_S1   / 10.     ; aMuonboyCofiti.SetS1  ( S1   ) ;
                                        
  double Z2   = pMbProtoTrack->m_Z2   / 10.     ; aMuonboyCofiti.SetZ2  ( Z2   ) ;
  double T2   = pMbProtoTrack->m_T2   / 10.     ; aMuonboyCofiti.SetT2  ( T2   ) ;
  double S2   = pMbProtoTrack->m_S2   / 10.     ; aMuonboyCofiti.SetS2  ( S2   ) ;

  double X1   = CFI * T1 - SFI * S1             ; aMuonboyCofiti.SetX1  ( X1   ) ;
  double Y1   = SFI * T1 + CFI * S1             ; aMuonboyCofiti.SetY1  ( Y1   ) ;
  double X2   = CFI * T2 - SFI * S2             ; aMuonboyCofiti.SetX2  ( X2   ) ;
  double Y2   = SFI * T2 + CFI * S2             ; aMuonboyCofiti.SetY2  ( Y2   ) ;
 
//If the origin of the input track is unknown, 
// assume that it is coming from main fit
// but if it contains only one segment, in which case it is assumed to have never been reconstructed
  if ( MboyStatusInput == -1 ) {
    MboyStatusInput = 3 ;
    if ( (pMbProtoTrack->m_VectorOfMbDRAT).size() == 1 ) MboyStatusInput = 0 ;
  }

//Keep the station index of the first "fixed" segment 
  it     = (pMbProtoTrack->m_VectorOfMbDRAT).begin()   ;
  int ISC0 = (*it)->m_ISC0 ;
  
//Keep the station index of the first "free" segment 
  it     = (pTargetSegmentsProtoTrack->m_VectorOfMbDRAT).begin()   ;
  int ISC1 = (*it)->m_ISC0 ;
  
//Do the fit
  int iok = 1 ;
  dothefit_(MboyStatusInput,ISC0,ISC1,iok);
//   std::cout << "MuonboyDoProtoTrack::find MboyStatusInput " << MboyStatusInput << std::endl ;
//   std::cout << "MuonboyDoProtoTrack::find iok             " << iok             << std::endl ;
  if ( iok != 0 ) return 0 ;
 

//One can have iok 0 and 1 fixed segment ??????????????????????
  MuonboyCofito aMuonboyCofito ; 
  int NBFIX_o = aMuonboyCofito.GetNBFIXE() ;
//std::cout << " NBFIX_o " << NBFIX_o << std::endl;
  if ( NBFIX_o <= (int)((pMbProtoTrack->m_VectorOfMbDRAT).size()) ) return 0 ;

//Take back new Proto track
  MbProtoTrack*  New_pMbProtoTrack = new MbProtoTrack();
  
//Take back new Proto track parameters
  int MboyStatusOutput =  MboyStatusInput + 1;
  if ( MboyStatusOutput > 3 ) MboyStatusOutput = 3 ;
  New_pMbProtoTrack->m_MboyStatus = MboyStatusOutput ;
  
  ISCI = aMuonboyCofiti.GetISCI () ; New_pMbProtoTrack->m_ISC0 = ISCI           ;
                                                             
  CFI  = aMuonboyCofiti.GetCFI  () ; New_pMbProtoTrack->m_CFI  = CFI            ;
  SFI  = aMuonboyCofiti.GetSFI  () ; New_pMbProtoTrack->m_SFI  = SFI            ;
  CFA  = aMuonboyCofiti.GetCFA  () ; New_pMbProtoTrack->m_CFA  = CFA            ;
  SFA  = aMuonboyCofiti.GetSFA  () ; New_pMbProtoTrack->m_SFA  = SFA            ;
  
  SMU  = aMuonboyCofiti.GetSMU  () ; New_pMbProtoTrack->m_SMU  = SMU  / 100000. ;

  DZT1 = aMuonboyCofiti.GetDZT1 () ; New_pMbProtoTrack->m_DZT1 = DZT1 * 10.     ;
  DS1  = aMuonboyCofiti.GetDS1  () ; New_pMbProtoTrack->m_DS1  = DS1  * 10.     ;
  DZT2 = aMuonboyCofiti.GetDZT2 () ; New_pMbProtoTrack->m_DZT2 = DZT2 * 10.     ;
  DS2  = aMuonboyCofiti.GetDS2  () ; New_pMbProtoTrack->m_DS2  = DS2  * 10.     ;
  
  Z1   = aMuonboyCofiti.GetZ1   () ; New_pMbProtoTrack->m_Z1   = Z1   * 10.     ;
  T1   = aMuonboyCofiti.GetT1   () ; New_pMbProtoTrack->m_T1   = T1   * 10.     ;
  S1   = aMuonboyCofiti.GetS1   () ; New_pMbProtoTrack->m_S1   = S1   * 10.     ;
  
  Z2   = aMuonboyCofiti.GetZ2   () ; New_pMbProtoTrack->m_Z2   = Z2   * 10.     ;
  T2   = aMuonboyCofiti.GetT2   () ; New_pMbProtoTrack->m_T2   = T2   * 10.     ;
  S2   = aMuonboyCofiti.GetS2   () ; New_pMbProtoTrack->m_S2   = S2   * 10.     ;
 
//Take back kinematics
  double Xdir = 0. ; 
  double Ydir = 0. ; 
  double Zdir = 0. ;
  double Xpos = 0. ; 
  double Ypos = 0. ; 
  double Zpos = 0. ;
  double Mome = 0. ; 
  double Charge = 0. ;
  ComputeDirAndPos(
                   ISCI ,
                   CFI  ,
                   SFI  ,
                   CFA  ,
                   SFA  ,
                   SMU  ,
                   DZT1 ,
                   DS1  ,
                   DZT2 ,
                   DS2  ,
                   Z1   ,
                   T1   ,
                   S1   ,
                   Z2   ,
                   T2   ,
                   S2   ,
                   Xdir , 
                   Ydir , 
                   Zdir ,
                   Xpos , 
                   Ypos , 
                   Zpos ,
                   Mome , 
                   Charge
                  );
  New_pMbProtoTrack->m_Xdir   = Xdir           ;
  New_pMbProtoTrack->m_Ydir   = Ydir           ;
  New_pMbProtoTrack->m_Zdir   = Zdir           ;
  New_pMbProtoTrack->m_Xpoint = Xpos   * 10.   ;
  New_pMbProtoTrack->m_Ypoint = Ypos   * 10.   ;
  New_pMbProtoTrack->m_Zpoint = Zpos   * 10.   ;
  New_pMbProtoTrack->m_Mome   = Mome   * 1000. ;
  New_pMbProtoTrack->m_Charge = Charge         ;

//Take back new Proto track associated segments
  (New_pMbProtoTrack->m_VectorOfMbDRAT).clear(); 
  for (int Iseg = 1 ; Iseg<=NBFIX_o; ++Iseg){
    int IsegM1 = Iseg -1 ;
    int ID           = aMuonboyCofito.GetIDFIXE(IsegM1)  - 1 ;
    int ISC          = aMuonboyCofito.GetISCFIXE(IsegM1) - 1 ;
    int KOUNTERDRA_o = aMuonboyCofito.GetKOUNTERDRA(ID,ISC) ;
    MbDRAT* pMbDRAT = VectorOfMbDRATLocal[KOUNTERDRA_o] ;
    (New_pMbProtoTrack->m_VectorOfMbDRAT).push_back( pMbDRAT ) ;
    MbDRAT* pMbDRAT_New = new MbDRAT( *pMbDRAT );
    pMbDRAT_New->m_ISC0   = ISC  ;
    pMbDRAT_New->m_CCDRAT = aMuonboyCofito.GetCCDRAT(ID,ISC) ;
    pMbDRAT_New->m_FFDRAT = aMuonboyCofito.GetFFDRAT(ID,ISC) ;
    pMbDRAT_New->m_GGDRAT = aMuonboyCofito.GetGGDRAT(ID,ISC) ;
    pMbDRAT_New->m_EZDRAT = aMuonboyCofito.GetEZDRAT(ID,ISC) ;
    pMbDRAT_New->m_JFDRAT = aMuonboyCofito.GetJFDRAT(ID,ISC) ;
    pMbDRAT_New->m_ZZDRAT1 = aMuonboyCofito.GetZZTRAKDO(ID,1,ISC) * 10 ;
    pMbDRAT_New->m_TTDRAT1 = aMuonboyCofito.GetTTTRAKDO(ID,1,ISC) * 10 ;
    pMbDRAT_New->m_SSDRAT1 = aMuonboyCofito.GetSSTRAKDO(ID,1,ISC) * 10 ;
    pMbDRAT_New->m_IWDRAT1 = aMuonboyCofito.GetIWDRAT(ID,1,ISC)        ;
    pMbDRAT_New->m_ZZDRAT2 = aMuonboyCofito.GetZZTRAKDO(ID,2,ISC) * 10 ;
    pMbDRAT_New->m_TTDRAT2 = aMuonboyCofito.GetTTTRAKDO(ID,2,ISC) * 10 ;
    pMbDRAT_New->m_SSDRAT2 = aMuonboyCofito.GetSSTRAKDO(ID,2,ISC) * 10 ;
    pMbDRAT_New->m_IWDRAT2 = aMuonboyCofito.GetIWDRAT(ID,2,ISC)        ;
    
    ISC  = pMbDRAT_New->m_ISC0 ;
    CFI  = cos(pMbDRAT_New->m_FFDRAT) ;
    SFI  = sin(pMbDRAT_New->m_FFDRAT) ;
    CFA  = cos(pMbDRAT->m_GGDRAT) ;
    SFA  = sin(pMbDRAT->m_GGDRAT) ;
    SMU  = aMuonboyCofiti.GetSMU  () ;
    DZT1 = 0.     ;
    DS1  = 0.     ;
    DZT2 = 0.     ;
    DS2  = 0.     ;
    Z1   = pMbDRAT_New->m_ZZDRAT1 / 10. ;
    T1   = pMbDRAT_New->m_TTDRAT1 / 10. ;
    S1   = pMbDRAT_New->m_SSDRAT1 / 10. ;
    Z2   = pMbDRAT_New->m_ZZDRAT2 / 10. ;
    T2   = pMbDRAT_New->m_TTDRAT2 / 10. ;
    S2   = pMbDRAT_New->m_SSDRAT2 / 10. ;
    ComputeDirAndPos(
                   ISCI ,
                   CFI  ,
                   SFI  ,
                   CFA  ,
                   SFA  ,
                   SMU  ,
                   DZT1 ,
                   DS1  ,
                   DZT2 ,
                   DS2  ,
                   Z1   ,
                   T1   ,
                   S1   ,
                   Z2   ,
                   T2   ,
                   S2   ,
                   Xdir , 
                   Ydir , 
                   Zdir ,
                   Xpos , 
                   Ypos , 
                   Zpos ,
                   Mome , 
                   Charge
                  );
    pMbDRAT_New->m_Xdir   = Xdir           ;
    pMbDRAT_New->m_Ydir   = Ydir           ;
    pMbDRAT_New->m_Zdir   = Zdir           ;
    pMbDRAT_New->m_Xpoint = Xpos   * 10.   ;
    pMbDRAT_New->m_Ypoint = Ypos   * 10.   ;
    pMbDRAT_New->m_Zpoint = Zpos   * 10.   ;
    pMbDRAT_New->m_Mome   = Mome   * 1000. ;
    pMbDRAT_New->m_Charge = Charge         ;
    (New_pMbProtoTrack->m_VectorOfMbDRATTRAKDO).push_back( pMbDRAT_New ) ;

  }

 return New_pMbProtoTrack ;
 
}
void MuonboyDoProtoTrack::ComputeDirAndPos(
                        int&    ISCI ,
                        double& CFI  , 
                        double& SFI  , 
                        double& CFA  , 
                        double& SFA  , 
                        double& SMU  ,
                        double& DZT1 , 
                        double& DS1  , 
                        double& DZT2 ,
                        double& DS2  ,
                        double& Z1   , 
                        double& T1   ,
                        double& S1   , 
                        double& Z2   , 
                        double& T2   , 
                        double& S2   ,
                        double& Xdir , 
                        double& Ydir , 
                        double& Zdir ,
                        double& Xpos , 
                        double& Ypos , 
                        double& Zpos ,
                        double& Mome , 
                        double& Charge
){
  double X1 = CFI * T1 - SFI * S1 ;
  double Y1 = SFI * T1 + CFI * S1 ;
  double X2 = CFI * T2 - SFI * S2 ;
  double Y2 = SFI * T2 + CFI * S2 ;
  
  double XA = 0. ;
  double YA = 0. ;
  double ZA = 0. ;
  double XB = 0. ;
  double YB = 0. ;
  double ZB = 0. ;
  
  if ( ISCI <= GetNCOUBAS2() ) {
    XA = X1 - SFI * DS1 ;
    YA = Y1 + CFI * DS1 ;
    ZA = Z1 + DZT1      ;
    XB = X2 - SFI * DS2 ;
    YB = Y2 + CFI * DS2 ;
    ZB = Z2 + DZT2      ;
  }else{
    XA = X1 - SFI * DS1 + CFI * CFA * DZT1 ;
    YA = Y1 + CFI * DS1 + SFI * CFA * DZT1 ;
    ZA = Z1                   - SFA * DZT1 ;
    XB = X2 - SFI * DS2 + CFI * CFA * DZT2 ;
    YB = Y2 + CFI * DS2 + SFI * CFA * DZT2 ;
    ZB = Z2                   - SFA * DZT2 ;
  }

//Compute dir  
  double OBMA = 1. / sqrt( (XB-XA)*(XB-XA) + (YB-YA)*(YB-YA) + (ZB-ZA)*(ZB-ZA) ) ;
  Xdir = (XB-XA) * OBMA ;
  Ydir = (YB-YA) * OBMA ;
  Zdir = (ZB-ZA) * OBMA ;

//Compute pos  
  Xpos = (XB+XA) / 2.   ;
  Ypos = (YB+YA) / 2.   ;
  Zpos = (ZB+ZA) / 2.   ;
  
//Compute momentum and Charge  
  double ASMU = 0.000001 ; if ( fabs(SMU) >= ASMU ) ASMU = fabs(SMU) ;
  Mome = 100. / ASMU ; 
  Charge = 1. ; if ( SMU <= 0. ) Charge = -1. ;

//And reverse
  Xdir = -Xdir ;
  Ydir = -Ydir ;
  Zdir = -Zdir ;
  Charge = -Charge ;

}

void MuonboyDoProtoTrack::finalize(){}

void MuonboyDoProtoTrack::Add2Codrat(MbDRAT* pMbDRAT, int& KOUNTERDRA_i){

  int    ISC0    = pMbDRAT->m_ISC0          ;
  double CCDRAT  = pMbDRAT->m_CCDRAT        ;
  double FFDRAT  = pMbDRAT->m_FFDRAT        ; 
  double GGDRAT  = pMbDRAT->m_GGDRAT        ;
  double EZDRAT  = pMbDRAT->m_EZDRAT        ;
  double SSDRAT1 = pMbDRAT->m_SSDRAT1 / 10. ; 
  double ZZDRAT1 = pMbDRAT->m_ZZDRAT1 / 10. ;  
  double TTDRAT1 = pMbDRAT->m_TTDRAT1 / 10. ;  
  int    IWDRAT1 = pMbDRAT->m_IWDRAT1       ;
  double SSDRAT2 = pMbDRAT->m_SSDRAT2 / 10. ;  
  double ZZDRAT2 = pMbDRAT->m_ZZDRAT2 / 10. ;  
  double TTDRAT2 = pMbDRAT->m_TTDRAT2 / 10. ;  
  int    IWDRAT2 = pMbDRAT->m_IWDRAT2       ;
  int    JFDRAT  = pMbDRAT->m_JFDRAT        ;

  if ( IWDRAT1 == 0 &&  IWDRAT2 == 0 ) return ;
  addtocodrat_(        
             ISC0    ,
             CCDRAT  ,
             FFDRAT  , 
             GGDRAT  ,
             EZDRAT  ,
             SSDRAT1 , 
             ZZDRAT1 , 
             TTDRAT1 , 
             IWDRAT1 ,
             SSDRAT2 , 
             ZZDRAT2 , 
             TTDRAT2 , 
             IWDRAT2 ,
             JFDRAT  ,
             KOUNTERDRA_i
            );

}

MbProtoTrack*  MuonboyDoProtoTrack::refine(
        MbProtoTrack*           pMbProtoTrack,
        const MuonRecDigiStore* pMuonRecDigiStore,
        MB_TrackStore*          pMB_TrackStore
){

  if ( (pMbProtoTrack->m_VectorOfMbDRAT).size() == 0 ){
       std::cout << "MuonboyDoProtoTrack::find pMbProtoTrack contains no MbDRAT = > Give up " << std::endl ;   
       return 0 ;
  }

//Reset
  resetdealwithfit_();
  
//Load Codrat stuff
  int KOUNTERDRA_i = -1 ;
  std::vector<MbDRAT*> VectorOfMbDRATLocal ; VectorOfMbDRATLocal.push_back(0) ; VectorOfMbDRATLocal.clear() ;
  VectorOfMbDRATLocal.reserve( (pMbProtoTrack->m_VectorOfMbDRAT).size() ); 

  std::vector< MbDRAT* >::iterator it     = (pMbProtoTrack->m_VectorOfMbDRAT).begin()   ;
  std::vector< MbDRAT* >::iterator it_end = (pMbProtoTrack->m_VectorOfMbDRAT).end()   ;
  for(; it!=it_end;++it ){
    MbDRAT* pMbDRAT = (*it) ;

    VectorOfMbDRATLocal.push_back(pMbDRAT);
    KOUNTERDRA_i += 1;
    
    Add2Codrat(pMbDRAT,KOUNTERDRA_i);

  }
  
//Set cofito (segment in prototrack) stuff
  KOUNTERDRA_i = -1 ;  
  it     = (pMbProtoTrack->m_VectorOfMbDRAT).begin()   ;
  it_end = (pMbProtoTrack->m_VectorOfMbDRAT).end()   ;
  for(; it!=it_end;++it ){

    KOUNTERDRA_i += 1;
    int ISCI = (*it)->m_ISC0 ;
    addtocofito_(ISCI,KOUNTERDRA_i) ;

  }

//Set cofiti (initial parameters of the fit) stuff
  MuonboyCofiti aMuonboyCofiti ;

  int    ISCI = pMbProtoTrack->m_ISC0           ; aMuonboyCofiti.SetISCI( ISCI ) ;
                                        
  double CFI  = pMbProtoTrack->m_CFI            ; aMuonboyCofiti.SetCFI ( CFI  ) ;
  double SFI  = pMbProtoTrack->m_SFI            ; aMuonboyCofiti.SetSFI ( SFI  ) ;
  double CFA  = pMbProtoTrack->m_CFA            ; aMuonboyCofiti.SetCFA ( CFA  ) ;
  double SFA  = pMbProtoTrack->m_SFA            ; aMuonboyCofiti.SetSFA ( SFA  ) ;
                                        
  double SMU  = pMbProtoTrack->m_SMU  * 100000. ; aMuonboyCofiti.SetSMU ( SMU  ) ;

  double DZT1 = pMbProtoTrack->m_DZT1 / 10.     ; aMuonboyCofiti.SetDZT1( DZT1 ) ;
  double DS1  = pMbProtoTrack->m_DS1  / 10.     ; aMuonboyCofiti.SetDS1 ( DS1  ) ;
  double DZT2 = pMbProtoTrack->m_DZT2 / 10.     ; aMuonboyCofiti.SetDZT2( DZT2 ) ;
  double DS2  = pMbProtoTrack->m_DS2  / 10.     ; aMuonboyCofiti.SetDS2 ( DS2  ) ;
                                        
  double Z1   = pMbProtoTrack->m_Z1   / 10.     ; aMuonboyCofiti.SetZ1  ( Z1   ) ;
  double T1   = pMbProtoTrack->m_T1   / 10.     ; aMuonboyCofiti.SetT1  ( T1   ) ;
  double S1   = pMbProtoTrack->m_S1   / 10.     ; aMuonboyCofiti.SetS1  ( S1   ) ;
                                        
  double Z2   = pMbProtoTrack->m_Z2   / 10.     ; aMuonboyCofiti.SetZ2  ( Z2   ) ;
  double T2   = pMbProtoTrack->m_T2   / 10.     ; aMuonboyCofiti.SetT2  ( T2   ) ;
  double S2   = pMbProtoTrack->m_S2   / 10.     ; aMuonboyCofiti.SetS2  ( S2   ) ;

  double X1   = CFI * T1 - SFI * S1             ; aMuonboyCofiti.SetX1  ( X1   ) ;
  double Y1   = SFI * T1 + CFI * S1             ; aMuonboyCofiti.SetY1  ( Y1   ) ;
  double X2   = CFI * T2 - SFI * S2             ; aMuonboyCofiti.SetX2  ( X2   ) ;
  double Y2   = SFI * T2 + CFI * S2             ; aMuonboyCofiti.SetY2  ( Y2   ) ;

//Do the fit
  int MboyStatusInput = 100 ; int ISC0 = 0 ; int ISC1 = 0 ; int iok = 1 ;
  dothefit_(MboyStatusInput, ISC0, ISC1,iok);
//   std::cout << "MuonboyDoProtoTrack::refine iok             " << iok             << std::endl ;
  if ( iok != 0 ) return 0 ;

//*Execute p_MB_TrackStoreMaker
   p_MB_TrackStoreMaker->execute(pMuonRecDigiStore,pMB_TrackStore);

//Take back new Proto track
  MbProtoTrack*  New_pMbProtoTrack = new MbProtoTrack();
  
//Take back new Proto track parameters  
  New_pMbProtoTrack->m_MboyStatus = 4 ;

  New_pMbProtoTrack->m_ISC0 = aMuonboyCofiti.GetISCI ()           ;

  New_pMbProtoTrack->m_CFI  = aMuonboyCofiti.GetCFI  ()           ;
  New_pMbProtoTrack->m_SFI  = aMuonboyCofiti.GetSFI  ()           ;
  New_pMbProtoTrack->m_CFA  = aMuonboyCofiti.GetCFA  ()           ;
  New_pMbProtoTrack->m_SFA  = aMuonboyCofiti.GetSFA  ()           ;
  
  New_pMbProtoTrack->m_SMU  = aMuonboyCofiti.GetSMU  () / 100000. ;

  New_pMbProtoTrack->m_DZT1 = aMuonboyCofiti.GetDZT1 () * 10.     ;
  New_pMbProtoTrack->m_DS1  = aMuonboyCofiti.GetDS1  () * 10.     ;
  New_pMbProtoTrack->m_DZT2 = aMuonboyCofiti.GetDZT2 () * 10.     ;
  New_pMbProtoTrack->m_DS2  = aMuonboyCofiti.GetDS2  () * 10.     ;
  
  New_pMbProtoTrack->m_Z1   = aMuonboyCofiti.GetZ1   () * 10.     ;
  New_pMbProtoTrack->m_T1   = aMuonboyCofiti.GetT1   () * 10.     ;
  New_pMbProtoTrack->m_S1   = aMuonboyCofiti.GetS1   () * 10.     ;
  
  New_pMbProtoTrack->m_Z2   = aMuonboyCofiti.GetZ2   () * 10.     ;
  New_pMbProtoTrack->m_T2   = aMuonboyCofiti.GetT2   () * 10.     ;
  New_pMbProtoTrack->m_S2   = aMuonboyCofiti.GetS2   () * 10.     ;
  
//Take back new Proto track associated segments
  (New_pMbProtoTrack->m_VectorOfMbDRAT).clear(); 
  it     = (pMbProtoTrack->m_VectorOfMbDRAT).begin()   ;
  it_end = (pMbProtoTrack->m_VectorOfMbDRAT).end()   ;
  for(; it!=it_end;++it ){
    (New_pMbProtoTrack->m_VectorOfMbDRAT).push_back( (*it) ) ;
  }

 return New_pMbProtoTrack ;
 
}
