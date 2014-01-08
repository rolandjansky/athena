/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCollectMatter.h"

#include "MuonboyCore/MuonboyBFAccess.h"

#include "MuonboyCore/MuonboyComusc.h"
#include "MuonboyCore/MuonboyCocyli.h"
#include "MuonboyCore/MuonboyCofiti.h"
#include "MuonboyCore/MuonboyTrmusc.h"
#include "MuonboyCore/MuonboyPosmusc.h"
#include "MuonboyCore/MuonboyControl.h"
#include "MboyCoreEvt/MbMuSc.h"

MuonboyCollectMatter::MuonboyCollectMatter(){}
MuonboyCollectMatter::~MuonboyCollectMatter(){}

void MuonboyCollectMatter::initialize(){}

void MuonboyCollectMatter::execute(
                MuonboyBFAbstract* pMuonboyBFAbstract,
                int AtCosmMatter,
                double XposI,
                double YposI,
                double ZposI,
                double XmomI,
                double YmomI,
                double ZmomI,
                double Charge,
                std::vector<MbMuSc*>* pVectorOfMbMuSc
){
//Minimal action
  pVectorOfMbMuSc->clear();

//Minimal action
  std::vector<MbMuSc> VectorLocalOfMbMuSc ;
  VectorLocalOfMbMuSc.clear();

//Set Field
  MuonboyBFAccess::SetMuonboyBFAbstract(pMuonboyBFAbstract);

//Basic objects
  MuonboyComusc  aMuonboyComusc;
  MuonboyCocyli  aMuonboyCocyli;
  MuonboyCofiti  aMuonboyCofiti;
  MuonboyTrmusc  aMuonboyTrmusc;
  MuonboyPosmusc aMuonboyPosmusc;
  MuonboyControl aMuonboyControl;
  int     IndexMax ;

//Save Old AtCosm value 
  int AtCosm_Old = aMuonboyControl.GetAtCosm() ;

//Take new AtCosm value 
//std::cout << " MuonboyCollectMatter::execute AtCosm AVANT " <<  aMuonboyControl.GetAtCosm() << std::endl ;
  aMuonboyControl.SetAtCosm(AtCosmMatter) ;

//Save Old [R/Z]CHAM[A/I] values 
  double RCHAMA_Old = aMuonboyCofiti.GetRCHAMA() ;
  double RCHAMI_Old = aMuonboyCofiti.GetRCHAMI() ;
  double ZCHAMA_Old = aMuonboyCofiti.GetZCHAMA() ;
  double ZCHAMI_Old = aMuonboyCofiti.GetZCHAMI() ;
  
//Take new [R/Z]CHAM[A/I] values 
  double RCHAMA_New = aMuonboyCocyli.RCYLIN(3) ;
  double RCHAMI_New = -100. ;
  double ZCHAMA_New = aMuonboyCocyli.ZCYLIN(3) ;
  double ZCHAMI_New = -100. ;

//Set new [R/Z]CHAM[A/I] values 
  aMuonboyCofiti.SetRCHAMA(RCHAMA_New);
  aMuonboyCofiti.SetRCHAMI(RCHAMI_New);
  aMuonboyCofiti.SetZCHAMA(ZCHAMA_New);
  aMuonboyCofiti.SetZCHAMI(ZCHAMI_New); 

//Reformat inputs
  double Xpos = XposI / 10. ;
  double Ypos = YposI / 10. ;
  double Zpos = ZposI / 10. ;
  double Pmom =sqrt( XmomI*XmomI + YmomI*YmomI + ZmomI*ZmomI ) ;
  double uX = XmomI/Pmom ;
  double uY = YmomI/Pmom ;
  double uZ = ZmomI/Pmom ;
  Pmom = Pmom / 1000.;

//Initialize
  aMuonboyComusc.ResetMBCOMUSC();

//Collect matter crossings
  int Nsens = 11 ;
  aMuonboyTrmusc.DoIt(Xpos, Ypos,  Zpos, uX, uY, uZ, Pmom, Charge, Nsens );
  aMuonboyTrmusc.DoIt(Xpos, Ypos,  Zpos, uX, uY, uZ, Pmom, Charge, Nsens );

//debug
//std::cout << "*Before aMuonboyPosmusc" << std::endl ; aMuonboyComusc.Print1();

//Find track positions and directions at MuSc Centers
  aMuonboyPosmusc.DoIt(Xpos, Ypos, Zpos, uX, uY, uZ, Pmom, Charge );

//debug
//std::cout << "*After aMuonboyPosmusc" << std::endl ; aMuonboyComusc.Print2();

//Collect MuSc planes parameters
  IndexMax = aMuonboyComusc.NBMASC();
  for (int Index= 0 ; Index <IndexMax ; Index++){
    if (aMuonboyComusc.IValMASC(Index) != 0) {
    if ( fabs( aMuonboyComusc.PmomMASC(Index) ) >= 0.010) {
      MbMuSc aMbMuSc ;
      CopyMuonboyComusc2MbMuSc(&aMuonboyComusc,&aMbMuSc,Pmom,Index);
      VectorLocalOfMbMuSc.push_back(aMbMuSc) ;
    }   
    }   
  }   

//Initialize
  aMuonboyComusc.ResetMBCOMUSC();

//Take back Last point
  Xpos = aMuonboyCofiti.GetVEND( 0 ) ;
  Ypos = aMuonboyCofiti.GetVEND( 1 ) ;
  Zpos = aMuonboyCofiti.GetVEND( 2 ) ;
  uX   = aMuonboyCofiti.GetVEND( 3 ) ;
  uY   = aMuonboyCofiti.GetVEND( 4 ) ;
  uZ   = aMuonboyCofiti.GetVEND( 5 ) ;
  Pmom = aMuonboyCofiti.GetVEND( 6 ) ;
//   std::cout << "*Last Point" << std::endl ; 
//   std::cout << "* Xpos " << Xpos << std::endl ; 
//   std::cout << "* Ypos " << Ypos << std::endl ; 
//   std::cout << "* Zpos " << Zpos << std::endl ; 
//   std::cout << "* uX   " << uX   << std::endl ; 
//   std::cout << "* uY   " << uY   << std::endl ; 
//   std::cout << "* uZ   " << uZ   << std::endl ; 
//   std::cout << "* Pmom " << Pmom << std::endl ; 
  

//Collect matter crossings
  Nsens = 1 ;
  aMuonboyTrmusc.DoIt(Xpos, Ypos,  Zpos, uX, uY, uZ, Pmom, Charge, Nsens );
  aMuonboyTrmusc.DoIt(Xpos, Ypos,  Zpos, uX, uY, uZ, Pmom, Charge, Nsens );

//debug
//std::cout << "*Before aMuonboyPosmusc 2 " << std::endl ; aMuonboyComusc.Print1();

//Find track positions and directions at MuSc Centers
  aMuonboyPosmusc.DoIt(Xpos, Ypos, Zpos, uX, uY, uZ, Pmom, Charge );

//debug
//std::cout << "*After aMuonboyPosmusc 2 " << std::endl ; aMuonboyComusc.Print2();

//Collect MuSc planes parameters
  IndexMax = aMuonboyComusc.NBMASC();
  for (int Index= 0 ; Index <IndexMax ; Index++){
    if (aMuonboyComusc.IValMASC(Index) != 0) {
    if ( fabs( aMuonboyComusc.PmomMASC(Index) ) >= 0.010) {
      MbMuSc aMbMuSc ;
      CopyMuonboyComusc2MbMuSc(&aMuonboyComusc,&aMbMuSc,Pmom,Index);
      VectorLocalOfMbMuSc.push_back(aMbMuSc) ;
    }   
    }   
  }   

 SelectAndOrder(
                XposI,
                YposI,
                ZposI,
                VectorLocalOfMbMuSc,
                pVectorOfMbMuSc
               );

//Reset [R/Z]CHAM[A/I] to old values 
  aMuonboyCofiti.SetRCHAMA(RCHAMA_Old);
  aMuonboyCofiti.SetRCHAMI(RCHAMI_Old);
  aMuonboyCofiti.SetZCHAMA(ZCHAMA_Old);
  aMuonboyCofiti.SetZCHAMI(ZCHAMI_Old); 

//Reset AtCosm to old valuevalue 
  aMuonboyControl.SetAtCosm(AtCosm_Old) ;
//std::cout << " MuonboyCollectMatter::execute AtCosm APRES " <<  aMuonboyControl.GetAtCosm() << std::endl ;

}

void MuonboyCollectMatter::finalize(){}


void MuonboyCollectMatter::CopyMuonboyComusc2MbMuSc(MuonboyComusc* pMuonboyComusc,MbMuSc* pMbMuSc,double /*Pmom*/,int Index){

    double RRMASC_V   = pMuonboyComusc->RRMASC(Index)*10.     ;

    int    LLMASC_V   = pMuonboyComusc->LLMASC(Index)         ;
//Overwritte to allow both Eloss and Mu Sc
    LLMASC_V   = 7 ;

    double X0MASC_V   = pMuonboyComusc->X0MASC(Index)         ;

    double S1MASC_V   = pMuonboyComusc->S1MASC(Index)         ;
    double S2MASC_V   = pMuonboyComusc->S2MASC(Index)         ;
    double THMASC_V   = pMuonboyComusc->THMASC(Index)         ;

    double ALMASC_V   = pMuonboyComusc->ALMASC(Index) - 1.    ;
    double BLMASC_V   = pMuonboyComusc->BLMASC(Index)*1000.   ;
    double XposMASC_V = pMuonboyComusc->XposMASC(Index)*10.   ;
    double YposMASC_V = pMuonboyComusc->YposMASC(Index)*10.   ;
    double ZposMASC_V = pMuonboyComusc->ZposMASC(Index)*10.   ;
    double XdirMASC_V = pMuonboyComusc->XdirMASC(Index)       ;
    double YdirMASC_V = pMuonboyComusc->YdirMASC(Index)       ;
    double ZdirMASC_V = pMuonboyComusc->ZdirMASC(Index)       ;
    double PmomMASC_V = pMuonboyComusc->PmomMASC(Index)*1000. ;
    
    double ChgeMASC_V = pMuonboyComusc->ChgeMASC(Index)       ;

    pMbMuSc->SetRRMASC( RRMASC_V );
    
    pMbMuSc->SetLLMASC( LLMASC_V );

    pMbMuSc->SetX0MASC( X0MASC_V );

    pMbMuSc->SetS1MASC( S1MASC_V );
    pMbMuSc->SetS2MASC( S2MASC_V );
    pMbMuSc->SetTHMASC( THMASC_V );

/////////////////////////////////////////////////////// 
// BE AWARE:
// IN MUONBOY             D_E = A * E + B
// USUAL  CONVENTION IS   D_E = A + B * E
// SO ONE MUST EXCHANGE A AND B ROLES
    pMbMuSc->SetALMASC( BLMASC_V );
    pMbMuSc->SetBLMASC( ALMASC_V );
/////////////////////////////////////////////////////// 
    
    pMbMuSc->SetPosDirMASC( 
                                     XposMASC_V , 
                                     YposMASC_V ,
                                     ZposMASC_V ,
                                     XdirMASC_V ,
                                     YdirMASC_V ,
                                     ZdirMASC_V
                                    );
    
    pMbMuSc->SetPmomMASC( PmomMASC_V );
    
    pMbMuSc->SetChgeMASC( ChgeMASC_V );

}

void MuonboyCollectMatter::SelectAndOrder(
                      double XrefStart,
                      double YrefStart,
                      double ZrefStart,
                      std::vector<MbMuSc>& VectorLocalOfMbMuSc,
                      std::vector<MbMuSc*>* pVectorOfMbMuSc
){

  int TotalNberOfElements=VectorLocalOfMbMuSc.size(); 
  
  std::vector<double> PointXdca ;
  std::vector<double> PointYdca ;
  std::vector<double> PointZdca ;
  std::vector<int>    PointInde ;

  for (int Index=0; Index<TotalNberOfElements; Index++){
    PointXdca.push_back( VectorLocalOfMbMuSc[Index].GetXposMASC() );
    PointYdca.push_back( VectorLocalOfMbMuSc[Index].GetYposMASC() );
    PointZdca.push_back( VectorLocalOfMbMuSc[Index].GetZposMASC() );
    PointInde.push_back( Index);
  }

  std::vector<int>  LinearIndexSorted ;
  LinearIndexSorted.clear();
  
  int    iNew = 1 ;
  int    IndexKeep = 0 ;
  double XrefStartNew = 0. ;
  double YrefStartNew = 0. ;
  double ZrefStartNew = 0. ;
  double DistanceToRefMAX =-99999999. ; 

  int TotalNberOfElementsTOBESORTED = PointXdca.size();
  for (int Index=0; Index<TotalNberOfElementsTOBESORTED; Index++){
    double Xdca = PointXdca[Index] ;
    double Ydca = PointYdca[Index] ;
    double Zdca = PointZdca[Index] ;
   
    double DistanceToZero = sqrt ( 
                                  (XrefStart-Xdca)*(XrefStart-Xdca)
                                + (YrefStart-Ydca)*(YrefStart-Ydca)
                                + (ZrefStart-Zdca)*(ZrefStart-Zdca)
                                );
    if ( DistanceToZero > DistanceToRefMAX )DistanceToRefMAX = DistanceToZero;

    int Index2Start = Index + 1 ;
    for (int Index2=Index2Start; Index2<TotalNberOfElementsTOBESORTED; Index2++){
      double Xdca2 = PointXdca[Index2] ;
      double Ydca2 = PointYdca[Index2] ;
      double Zdca2 = PointZdca[Index2] ;
      
      double DistanceInter= sqrt ( 
                                    (Xdca2-Xdca)*(Xdca2-Xdca)
                                  + (Ydca2-Ydca)*(Ydca2-Ydca)
                                  + (Zdca2-Zdca)*(Zdca2-Zdca)
                                  );
      if ( DistanceInter > DistanceToRefMAX )DistanceToRefMAX = DistanceInter;
      
    }
    
  }

  DistanceToRefMAX = 2. * DistanceToRefMAX ;
    
  double DistanceToRefmin = DistanceToRefMAX ; 
    
  while ( iNew == 1 ){
    iNew = 0 ; 
    int iStarted = 0 ; 
    for (int Index=0; Index<TotalNberOfElementsTOBESORTED; Index++){
      int IndexTrue = PointInde[Index];
      int IsExcluded = 0 ;
      int TotalNberOfElementsSorted = LinearIndexSorted.size(); 
      for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
        if ( LinearIndexSorted[iSorted] == IndexTrue ) IsExcluded = 1 ;
        if ( LinearIndexSorted[iSorted] == IndexTrue ) break ;
      }
      if ( IsExcluded == 0 ){
        iNew = 1 ;

        double Xdca = PointXdca[Index] ;
        double Ydca = PointYdca[Index] ;
        double Zdca = PointZdca[Index] ;
        double DistanceToRef = sqrt ( 
                                      (XrefStart-Xdca)*(XrefStart-Xdca)
                                    + (YrefStart-Ydca)*(YrefStart-Ydca)
                                    + (ZrefStart-Zdca)*(ZrefStart-Zdca)
                                    );
        if ( iStarted == 0 || DistanceToRef < DistanceToRefmin ){
          iStarted = 1 ;
          DistanceToRefmin = DistanceToRef ;
          IndexKeep = IndexTrue ;
          XrefStartNew = Xdca ;
          YrefStartNew = Ydca ;
          ZrefStartNew = Zdca ;
        }
      }
    }
    if ( iNew == 1 ){
      LinearIndexSorted.push_back(IndexKeep) ;
      XrefStart = XrefStartNew ;
      YrefStart = YrefStartNew ;
      ZrefStart = ZrefStartNew ;
      DistanceToRefmin = DistanceToRefMAX ; 
    }
  }
  
  int TotalNberOfElementsSorted = LinearIndexSorted.size(); 
  for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
    pVectorOfMbMuSc->push_back( new MbMuSc(VectorLocalOfMbMuSc[ LinearIndexSorted[iSorted] ] ) ) ;
  }
  
}
