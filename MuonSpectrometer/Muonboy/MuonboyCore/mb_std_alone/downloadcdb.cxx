/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "downloadcdb.h"
#include "getcdbvar.h"
#include "MuonboyCore/GetSetMuonboyVariable.h"
#include "Align/MuonAlignCorrDownloader.h"
#include "MuonboyCore/MuonboyInterface.h"
#include "MboyCoreEvt/MboyAlineStore.h"
#include "MboyCoreEvt/MboyBlineStore.h"
#include "MboyCoreEvt/MboyIlineStore.h"
#include "AmdcStand/AmdcsimrecStand.h"

int FirstPassage = 1 ;
MuonboyInterfaceAbstract* p_MuonboyInterfaceAbstract = 0 ;
MboyAlineStore*           p_MboyAlineStoreOriginal   = 0 ;
MboyBlineStore*           p_MboyBlineStoreOriginal   = 0 ;
Amdcsimrec*               p_Amdcsimrec               = 0 ;

extern "C"
void  downloadcdb_(){

  downloadali_();

}

extern "C"
void  downloadali_(){
  
//Store the A and B line present in input Amdc file
  if ( FirstPassage == 1 ) {
    FirstPassage += 1 ;

    p_MboyAlineStoreOriginal   = new MboyAlineStore   ;
    p_MboyBlineStoreOriginal   = new MboyBlineStore   ;
    p_Amdcsimrec               = new AmdcsimrecStand  ;
    p_MuonboyInterfaceAbstract = new MuonboyInterface ;
    
    std::cout << "*At the first passage, the A and B lines from input Amdc file are: " << std::endl;
    std::cout << "  NBadjust " << p_Amdcsimrec->NBadjust() << std::endl;
    std::cout << "  NBdeform " << p_Amdcsimrec->NBdeform() << std::endl;
    
    int DB_Jadjust_Max = p_Amdcsimrec->NBadjust();
    for (int DB_Jadjust=1 ; DB_Jadjust<=DB_Jadjust_Max ; DB_Jadjust++){
      int    DB_JTYP ;
      int    DB_JFF  ;
      int    DB_JZZ  ;
      int    DB_JOB  ;
      double Amdc_TraS ;
      double Amdc_TraZ ;
      double Amdc_TraT ;
      double Amdc_RotS ;
      double Amdc_RotZ ;
      double Amdc_RotT ;
      p_Amdcsimrec->GetStationDisplacementFromAMDCJadjust(
                                                          DB_Jadjust,
                                                          DB_JTYP,
                                                          DB_JFF, 
                                                          DB_JZZ,
                                                          DB_JOB,
                                                          Amdc_TraS, 
                                                          Amdc_TraZ, 
                                                          Amdc_TraT,
                                                          Amdc_RotS, 
                                                          Amdc_RotZ, 
                                                          Amdc_RotT 
                                                         );
      MboyAline aMboyAline;
      aMboyAline.SetStationType(p_Amdcsimrec->StationName(DB_JTYP));
      aMboyAline.Setjff  ( DB_JFF    );
      aMboyAline.Setjzz  ( DB_JZZ    );
      aMboyAline.Setjob  ( DB_JOB    );
      aMboyAline.Sets    ( Amdc_TraS );
      aMboyAline.Setz    ( Amdc_TraZ );
      aMboyAline.Sett    ( Amdc_TraT );
      aMboyAline.SetrotS ( Amdc_RotS );
      aMboyAline.SetrotZ ( Amdc_RotZ );
      aMboyAline.SetrotT ( Amdc_RotT );
      p_MboyAlineStoreOriginal->Add(aMboyAline);
    }
    int DB_Jdeform_Max = p_Amdcsimrec->NBdeform();
    for (int DB_Jdeform=1 ; DB_Jdeform<=DB_Jdeform_Max ; DB_Jdeform++){

      int    DB_JTYP ;
      int    DB_JFF  ;
      int    DB_JZZ  ;
      int    DB_JOB  ;
      double Amdc_bz ;
      double Amdc_bp ;
      double Amdc_bn ;
      double Amdc_sp ;
      double Amdc_sn ;
      double Amdc_tw ;
      double Amdc_pg ;
      double Amdc_tr ;
      double Amdc_eg ;
      double Amdc_ep ;
      double Amdc_en ;
      p_Amdcsimrec->GetStationDeformationFromAMDCJdeform(
                                                         DB_Jdeform,
                                                         DB_JTYP,
                                                         DB_JFF, 
                                                         DB_JZZ,
                                                         DB_JOB,
                                                         Amdc_bz,
                                                         Amdc_bp,
                                                         Amdc_bn,
                                                         Amdc_sp,
                                                         Amdc_sn,
                                                         Amdc_tw,
                                                         Amdc_pg,
                                                         Amdc_tr,
                                                         Amdc_eg,
                                                         Amdc_ep,
                                                         Amdc_en  
                                                        );
      MboyBline aMboyBline;
      aMboyBline.SetStationType(p_Amdcsimrec->StationName(DB_JTYP));
      aMboyBline.Setjff ( DB_JFF  );
      aMboyBline.Setjzz ( DB_JZZ  );
      aMboyBline.Setjob ( DB_JOB  );
      aMboyBline.Setbz  ( Amdc_bz );
      aMboyBline.Setbp  ( Amdc_bp );
      aMboyBline.Setbn  ( Amdc_bn );
      aMboyBline.Setsp  ( Amdc_sp );
      aMboyBline.Setsn  ( Amdc_sn );
      aMboyBline.Settw  ( Amdc_tw );
      aMboyBline.Setpg  ( Amdc_pg );
      aMboyBline.Settr  ( Amdc_tr );
      aMboyBline.Seteg  ( Amdc_eg );
      aMboyBline.Setep  ( Amdc_ep );
      aMboyBline.Seten  ( Amdc_en );
      p_MboyBlineStoreOriginal->Add(aMboyBline);
    }
  }
  
//Store the A and B line from cdb
  MboyAlineStore aMboyAlineStore ; aMboyAlineStore.Reset(); aMboyAlineStore.SetActive(1);
  MboyBlineStore aMboyBlineStore ; aMboyBlineStore.Reset(); aMboyBlineStore.SetActive(1);
  MboyIlineStore aMboyIlineStore ; aMboyIlineStore.Reset(); aMboyIlineStore.SetActive(1);

  double timestamp  = (double) GetMuboIntVar ("timestamp",-1,-1,-1);
  int    ncdbalitag = GetCDBIntVar("ncdbalitag",-1,-1,-1);

  for (int Item= 0 ; Item <ncdbalitag ; Item++){

    std::vector< MuonAlignCorr::Correction > corrections;
    std::string folderPath = GetCDBCharVar("cdbalifoldername",Item,-1,-1) ;
    std::string tag        = GetCDBCharVar("cdbalitagname",Item,-1,-1)   ;
//   int         port       = 3128   ;
    int port = persint::NetworkSettings::instance().getProxy("CERN");
    MuonAlignCorr::Downloader::getCorrections(folderPath,timestamp,tag, corrections,port);

    int NberOfCorrections = corrections.size();
    std::cout << "=> Nber of corrections from cdb  " << NberOfCorrections << std::endl;
    for (int Item2= 0 ; Item2 <NberOfCorrections ; Item2++){
      MboyAline aMboyAline;
      aMboyAline.SetStationType( corrections[Item2].type         );
      aMboyAline.Setjff        ( corrections[Item2].jff          );
      aMboyAline.Setjzz        ( corrections[Item2].jzz          );
      aMboyAline.Setjob        ( corrections[Item2].job          );
      aMboyAline.Sets          ( corrections[Item2].aline.svalue );
      aMboyAline.Setz          ( corrections[Item2].aline.zvalue );
      aMboyAline.Sett          ( corrections[Item2].aline.tvalue );
      aMboyAline.SetrotS       ( corrections[Item2].aline.tsv    );
      aMboyAline.SetrotZ       ( corrections[Item2].aline.tzv    );
      aMboyAline.SetrotT       ( corrections[Item2].aline.ttv    );
      aMboyAlineStore.Add(aMboyAline);
      MboyBline aMboyBline;
      aMboyBline.SetStationType( corrections[Item2].type     );
      aMboyBline.Setjff        ( corrections[Item2].jff      );
      aMboyBline.Setjzz        ( corrections[Item2].jzz      );
      aMboyBline.Setjob        ( corrections[Item2].job      );
      aMboyBline.Setbz         ( corrections[Item2].bline.bz );
      aMboyBline.Setbp         ( corrections[Item2].bline.bp );
      aMboyBline.Setbn         ( corrections[Item2].bline.bn );
      aMboyBline.Setsp         ( corrections[Item2].bline.sp );
      aMboyBline.Setsn         ( corrections[Item2].bline.sn );
      aMboyBline.Settw         ( corrections[Item2].bline.tw );
      aMboyBline.Setpg         ( corrections[Item2].bline.pg );
      aMboyBline.Settr         ( corrections[Item2].bline.tr );
      aMboyBline.Seteg         ( corrections[Item2].bline.eg );
      aMboyBline.Setep         ( corrections[Item2].bline.ep );
      aMboyBline.Seten         ( corrections[Item2].bline.en );
      aMboyBlineStore.Add(aMboyBline);
    }

  }

//Add to the A and B lines from cdb, the lines originally present in input Admc file
  int NTotMboyAlineStoreOriginal = p_MboyAlineStoreOriginal->NberOfObjects(); 
  for (int Item=0 ; Item<NTotMboyAlineStoreOriginal ; Item++){
    const MboyAline* pMboyAline=p_MboyAlineStoreOriginal->GetMboyAline(Item);
      MboyAline aMboyAline;
      aMboyAline.SetStationType( pMboyAline->GetStationType() );
      aMboyAline.Setjff        ( pMboyAline->Getjff        () );
      aMboyAline.Setjzz        ( pMboyAline->Getjzz        () );
      aMboyAline.Setjob        ( pMboyAline->Getjob        () );
      aMboyAline.Sets          ( pMboyAline->Gets          () );
      aMboyAline.Setz          ( pMboyAline->Getz          () );
      aMboyAline.Sett          ( pMboyAline->Gett          () );
      aMboyAline.SetrotS       ( pMboyAline->GetrotS       () );
      aMboyAline.SetrotZ       ( pMboyAline->GetrotZ       () );
      aMboyAline.SetrotT       ( pMboyAline->GetrotT       () );
      aMboyAlineStore.Add(aMboyAline);
  }
  int NTotMboyBlineStoreOriginal = p_MboyBlineStoreOriginal->NberOfObjects(); 
  for (int Item=0 ; Item<NTotMboyBlineStoreOriginal ; Item++){
    const MboyBline* pMboyBline=p_MboyBlineStoreOriginal->GetMboyBline(Item);
      MboyBline aMboyBline;
      aMboyBline.SetStationType( pMboyBline->GetStationType() );
      aMboyBline.Setjff        ( pMboyBline->Getjff        () );
      aMboyBline.Setjzz        ( pMboyBline->Getjzz        () );
      aMboyBline.Setjob        ( pMboyBline->Getjob        () );
      aMboyBline.Setbz         ( pMboyBline->Getbz         () );
      aMboyBline.Setbp         ( pMboyBline->Getbp         () );
      aMboyBline.Setbn         ( pMboyBline->Getbn         () );
      aMboyBline.Setsp         ( pMboyBline->Getsp         () );
      aMboyBline.Setsn         ( pMboyBline->Getsn         () );
      aMboyBline.Settw         ( pMboyBline->Gettw         () );
      aMboyBline.Setpg         ( pMboyBline->Getpg         () );
      aMboyBline.Settr         ( pMboyBline->Gettr         () );
      aMboyBline.Seteg         ( pMboyBline->Geteg         () );
      aMboyBline.Setep         ( pMboyBline->Getep         () );
      aMboyBline.Seten         ( pMboyBline->Geten         () );
      aMboyBlineStore.Add(aMboyBline);
  }
  


//Print out form debug
//Sort the A line store  
//    aMboyAlineStore.Sort() ;
//    std::cout << " aMboyAlineStore activity " << aMboyAlineStore.GetActive() << std::endl;
//    aMboyAlineStore.Print();
//   std::cout << " aMboyBlineStore activity " << aMboyBlineStore.GetActive() << std::endl;
//   aMboyBlineStore.Print();
//   std::cout << " aMboyIlineStore activity " << aMboyIlineStore.GetActive() << std::endl;
//   aMboyIlineStore.Print();

//Reset B lines store to NOT load B lines in Mboy
  aMboyBlineStore.Reset(); aMboyBlineStore.SetActive(0);

//Load the A and B lines in Mboy
  std::cout << "*Before Loading " << std::endl;
  std::cout << "  NBadjust " << p_Amdcsimrec->NBadjust() << std::endl;
  std::cout << "  NBdeform " << p_Amdcsimrec->NBdeform() << std::endl;

  int StatusSetAliLine = p_MuonboyInterfaceAbstract->SetAliLine(&aMboyAlineStore,&aMboyBlineStore,&aMboyIlineStore);
  if ( StatusSetAliLine !=0 ) std::cout << " p_MuonboyInterfaceAbstract->SetAliLine failed " <<  std::endl;

  std::cout << "*After Loading " << std::endl;
  std::cout << "  NBadjust " << p_Amdcsimrec->NBadjust() << std::endl;
  std::cout << "  NBdeform " << p_Amdcsimrec->NBdeform() << std::endl;

}
