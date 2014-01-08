/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/AandBlinesLoader.h"

#include "MboyCoreEvt/MboyAlineStore.h"
#include "MboyCoreEvt/MboyBlineStore.h"
#include "MboyCoreEvt/MboyIlineStore.h"
#include "MboyCoreEvt/MboyAline.h"
#include "MboyCoreEvt/MboyBline.h"
#include "MboyCoreEvt/MboyIline.h"

#include "AmdcStand/setablines.h"
#include "load_position_adjustment.h"

AandBlinesLoader::AandBlinesLoader(){}
AandBlinesLoader::~AandBlinesLoader(){}

void AandBlinesLoader::initialize(){}


int AandBlinesLoader::execute(
               MboyAlineStore* pMboyAlineStore,
               MboyBlineStore* pMboyBlineStore,
               MboyIlineStore* pMboyIlineStore 
){

  if  ( pMboyAlineStore == 0 ) return 0;
  if  ( pMboyBlineStore == 0 ) return 0;
  if  ( pMboyIlineStore == 0 ) return 0;
  if  ( pMboyAlineStore ->GetActive() != 1
  &&    pMboyBlineStore ->GetActive() != 1  
  &&    pMboyIlineStore ->GetActive() != 1 ) return 0;

  std::string StationType ; 
  int         jff         ;
  int         jzz         ;
  int         job         ;
  int         jlay        ;
  char STANAME[3];

  int ierr ;

//A lines
  double DataAline[6];
  double s    ;
  double z    ;
  double t    ;
  double rotS ;
  double rotZ ;
  double rotT ;
  
  jff  = 0 ;
  jzz  = 0 ;
  job  = 0 ;
  ierr = 0 ;

  STANAME[0] = 'Z' ;
  STANAME[1] = 'Z' ;
  STANAME[2] = 'Z' ;

  if ( pMboyAlineStore->GetActive() == 1 ){
    int NberOfA= pMboyAlineStore->NberOfObjects();
//  if (NberOfA >= 1 ) stasetalines_(STANAME, jff, jzz, job, DataAline, ierr);
    stasetalines_(STANAME, jff, jzz, job, DataAline, ierr);
    for (int ItemNber=0; ItemNber<NberOfA ; ItemNber++){
      const MboyAline* pMboyAline=pMboyAlineStore->GetMboyAline(ItemNber);
      
      StationType = pMboyAline->GetStationType();
      jff         = pMboyAline->Getjff        ();
      jzz         = pMboyAline->Getjzz        ();
      job         = pMboyAline->Getjob        ();
      pMboyAline->GetStationNameChar(STANAME);

      s   = pMboyAline->Gets    () ;
      z   = pMboyAline->Getz    () ;
      t   = pMboyAline->Gett    () ;
      rotS = pMboyAline->GetrotS () ;
      rotZ = pMboyAline->GetrotZ () ;
      rotT = pMboyAline->GetrotT () ;

      DataAline[0] = s    ;
      DataAline[1] = z    ;
      DataAline[2] = t    ;
      DataAline[3] = rotS ;
      DataAline[4] = rotZ ;
      DataAline[5] = rotT ;
      stasetalines_(STANAME, jff, jzz, job, DataAline, ierr);
      if ( ierr != 0 ) {
        std::cout << "stasetalines_ failed on " << std::endl;
        std::cout << "  StationType " << StationType << std::endl;
        std::cout << "  jff         " << jff         << std::endl;
        std::cout << "  jzz         " << jzz         << std::endl;
        std::cout << "  job         " << job         << std::endl;
        std::cout << "  s    " << s    << std::endl;
        std::cout << "  z    " << z    << std::endl;
        std::cout << "  t    " << t    << std::endl;
        std::cout << "  rotS " << rotS << std::endl;
        std::cout << "  rotZ " << rotZ << std::endl;
        std::cout << "  rotT " << rotT << std::endl;
        return ierr;
      }

    }
  }
  

//B lines
  double DataBline[11];
  double bz ;
  double bp ;
  double bn ;
  double sp ;
  double sn ;
  double tw ;
  double pg ;
  double tr ;
  double eg ;
  double ep ;
  double en ;

  jff  = 0 ;
  jzz  = 0 ;
  job  = 0 ;
  ierr = 0 ;

  STANAME[0] = 'Z' ;
  STANAME[1] = 'Z' ;
  STANAME[2] = 'Z' ;

  if ( pMboyBlineStore->GetActive() == 1 ){
    int NberOfB= pMboyBlineStore->NberOfObjects();
//  if (NberOfB >= 1 ) stasetblines_(STANAME, jff, jzz, job, DataBline, ierr);
    stasetblines_(STANAME, jff, jzz, job, DataBline, ierr);
    for (int ItemNber=0; ItemNber<NberOfB ; ItemNber++){
      const MboyBline* pMboyBline=pMboyBlineStore->GetMboyBline(ItemNber);
      
      StationType = pMboyBline->GetStationType();
      jff         = pMboyBline->Getjff        ();
      jzz         = pMboyBline->Getjzz        ();
      job         = pMboyBline->Getjob        ();
      pMboyBline->GetStationNameChar(STANAME);

      bz = pMboyBline->Getbz () ;
      bp = pMboyBline->Getbp () ;
      bn = pMboyBline->Getbn () ;
      sp = pMboyBline->Getsp () ;
      sn = pMboyBline->Getsn () ;
      tw = pMboyBline->Gettw () ;
      pg = pMboyBline->Getpg () ;
      tr = pMboyBline->Gettr () ;
      eg = pMboyBline->Geteg () ;
      ep = pMboyBline->Getep () ;
      en = pMboyBline->Geten () ;

      DataBline[0]  = bz ;
      DataBline[1]  = bp ;
      DataBline[2]  = bn ;
      DataBline[3]  = sp ;
      DataBline[4]  = sn ;
      DataBline[5]  = tw ;
      DataBline[6]  = pg ;
      DataBline[7]  = tr ;
      DataBline[8]  = eg ;
      DataBline[9]  = ep ;
      DataBline[10] = en ;
      stasetblines_(STANAME, jff, jzz, job, DataBline, ierr);
      if ( ierr != 0 ) {
        std::cout << "stasetblines_ failed on " << std::endl;
        std::cout << "  StationType " << StationType << std::endl;
        std::cout << "  jff         " << jff         << std::endl;
        std::cout << "  jzz         " << jzz         << std::endl;
        std::cout << "  job         " << job         << std::endl;
        std::cout << "  bz " << bz << std::endl;
        std::cout << "  bp " << bp << std::endl;
        std::cout << "  bn " << bn << std::endl;
        std::cout << "  sp " << sp << std::endl;
        std::cout << "  sn " << sn << std::endl;
        std::cout << "  tw " << tw << std::endl;
        std::cout << "  pg " << pg << std::endl;
        std::cout << "  tr " << tr << std::endl;
        std::cout << "  eg " << eg << std::endl;
        std::cout << "  ep " << ep << std::endl;
        std::cout << "  en " << en << std::endl;
        return ierr;
      }

    }
  }

//I lines
  double DataIline[6];
  
  jff  = 0 ;
  jzz  = 0 ;
  job  = 0 ;
  jlay = 0 ;
  ierr = 0 ;

  STANAME[0] = 'Z' ;
  STANAME[1] = 'Z' ;
  STANAME[2] = 'Z' ;

  if ( pMboyIlineStore->GetActive() == 1 ){
    int NberOfA= pMboyIlineStore->NberOfObjects();
//  if (NberOfA >= 1 ) stasetilines_(STANAME, jff, jzz, job,  jlay, DataIline, ierr);
    stasetilines_(STANAME, jff, jzz, job, jlay, DataIline, ierr);
    for (int ItemNber=0; ItemNber<NberOfA ; ItemNber++){
      const MboyIline* pMboyIline=pMboyIlineStore->GetMboyIline(ItemNber);
      
      StationType = pMboyIline->GetStationType();
      jff         = pMboyIline->Getjff        ();
      jzz         = pMboyIline->Getjzz        ();
      job         = pMboyIline->Getjob        ();
      jlay        = pMboyIline->Getjlay       ();
      pMboyIline->GetStationNameChar(STANAME);

      s   = pMboyIline->Gets    () ;
      z   = pMboyIline->Getz    () ;
      t   = pMboyIline->Gett    () ;
      rotS = pMboyIline->GetrotS () ;
      rotZ = pMboyIline->GetrotZ () ;
      rotT = pMboyIline->GetrotT () ;

      DataIline[0] = s    ;
      DataIline[1] = z    ;
      DataIline[2] = t    ;
      DataIline[3] = rotS ;
      DataIline[4] = rotZ ;
      DataIline[5] = rotT ;
      stasetilines_(STANAME, jff, jzz, job, jlay, DataIline, ierr);
      if ( ierr != 0 ) {
        std::cout << "stasetilines_ failed on " << std::endl;
        std::cout << "  StationType " << StationType << std::endl;
        std::cout << "  jff         " << jff         << std::endl;
        std::cout << "  jzz         " << jzz         << std::endl;
        std::cout << "  job         " << job         << std::endl;
        std::cout << "  jlay        " << jlay        << std::endl;
        std::cout << "  s    " << s    << std::endl;
        std::cout << "  z    " << z    << std::endl;
        std::cout << "  t    " << t    << std::endl;
        std::cout << "  rotS " << rotS << std::endl;
        std::cout << "  rotZ " << rotZ << std::endl;
        std::cout << "  rotT " << rotT << std::endl;
        return ierr;
      }

    }
    seticsc_() ;
  }
     
//Loading A and B lines
//  if ( NberOfA != 0 || NberOfB != 0 ){
    ierr = 0 ;
    load_position_adjustment_(ierr);
    if ( ierr != 0 ) {
      std::cout << "load_position_adjustment_ failed " << std::endl;
      return ierr;
    }
//  }
  
  return 0;
  
}


void AandBlinesLoader::finalize(){}
