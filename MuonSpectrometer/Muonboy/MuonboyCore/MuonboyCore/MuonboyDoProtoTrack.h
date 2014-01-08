/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyDoProtoTrack_H
#define MuonboyDoProtoTrack_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MbDRAT ;
class MbProtoTrack;

class MuonRecDigiStore;
class MB_TrackStore;
class MB_TrackStoreMaker;

  /**
   @class MuonboyDoProtoTrack

  @author samusog@cern.ch
  
  */

class MuonboyDoProtoTrack{
public:
    MuonboyDoProtoTrack();
   ~MuonboyDoProtoTrack();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Do execution */
   MbProtoTrack*  find(
        MbProtoTrack*            pMbProtoTrack ,
        MbProtoTrack*            pTargetSegmentsProtoTrack 
     );
   MbProtoTrack*  refine(
        MbProtoTrack*           pMbProtoTrack,
        const MuonRecDigiStore* pMuonRecDigiStore,
        MB_TrackStore*          pMB_TrackStore
     );

   /**Do finalisation */
   void finalize();

private:
///////////////////////////////////
 
  void Add2Codrat(MbDRAT* pMbDRAT, int& KOUNTERDRA_i) ;

  MB_TrackStoreMaker*   p_MB_TrackStoreMaker   ; //!< Pointer on MB_TrackStoreMaker
 
  void ComputeDirAndPos(
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
                       );

};

#endif

