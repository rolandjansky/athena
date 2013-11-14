/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------//
#include "TrkPrepRawData/PrepRawData.h"

//----------------------------------------------------------------//
#include "TrkSurfaces/Surface.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"

#include "MuonPrepRawData/CscStripPrepData.h"

#include "MboyAthEvt/MB_TrackAth.h"

#include "MboyAthEvt/MuonRecDigiStoreAth.h"

MB_TrackAth::MB_TrackAth():MB_Track(){}
MB_TrackAth::MB_TrackAth(
                         const MB_Track & oMB_Track,
                         const MuonRecDigiStoreAth* pMuonRecDigiStoreAth)
  :MB_Track(oMB_Track){

  int TotalNberOfEls = GetNberOfElements();
  for (int ItemNber=0; ItemNber<TotalNberOfEls ; ItemNber++){

    int LinearNber = -1 ;
    const MbROT* pMbROT = LiIdGetMbROT(ItemNber);
    if (pMbROT) LinearNber = pMbROT->GetLinkPrd() ;
    
    MdtPrepDataVector.push_back(pMuonRecDigiStoreAth->LiIdGetMdtPrepData   (LinearNber));
    RpcPrepDataVector.push_back(pMuonRecDigiStoreAth->LiIdGetRpcPrepData   (LinearNber));
    TgcPrepDataVector.push_back(pMuonRecDigiStoreAth->LiIdGetTgcPrepData   (LinearNber));
    CscStripPrepDataVector.push_back(pMuonRecDigiStoreAth->LiIdGetCscStripPrepData   (LinearNber));
    CscCluPrepDataVector.push_back(pMuonRecDigiStoreAth->LiIdGetCscClusterPrepData (LinearNber));

  }

}

MB_TrackAth::~MB_TrackAth(){}

/** Get  Surface */
const Trk::Surface* MB_TrackAth::LiIdGetSurface        (int LinearNber) const{
  const Trk::PrepRawData* pPrepRawData = LiIdGetPrepRawData(LinearNber);
  if ( pPrepRawData ) return &(pPrepRawData->detectorElement()->surface(pPrepRawData->identify())) ;
  return 0;
}

/** For a Linear Nber LinearNber get a typed Pointer */
const Trk::PrepRawData* MB_TrackAth::LiIdGetPrepRawData(int LinearNber) const{

  const Muon::MdtPrepData*      pMdtPrepData         = LiIdGetMdtPrepData(LinearNber)            ; if ( pMdtPrepData         ) return pMdtPrepData         ;

  const Muon::RpcPrepData*      pRpcPrepData         = LiIdGetRpcPrepData(LinearNber)            ; if ( pRpcPrepData         ) return pRpcPrepData         ;

  const Muon::TgcPrepData*      pTgcPrepData         = LiIdGetTgcPrepData(LinearNber)            ; if ( pTgcPrepData         ) return pTgcPrepData         ;

  const Muon::CscStripPrepData* pCscStripPrepData    = LiIdGetCscStripPrepData(LinearNber)       ; if ( pCscStripPrepData    ) return pCscStripPrepData    ;

  const Muon::CscPrepData*      pCscPrepDataClu =      LiIdGetCscClusterPrepData(LinearNber)     ; if ( pCscPrepDataClu      ) return pCscPrepDataClu      ;     

  return 0 ;
    
}
const Muon::MdtPrepData* MB_TrackAth::LiIdGetMdtPrepData(int LinearNber) const{
  if (LinearNber < 0 || LinearNber >= GetNberOfElements()) return 0;
  return  MdtPrepDataVector[LinearNber];
}
const Muon::RpcPrepData* MB_TrackAth::LiIdGetRpcPrepData(int LinearNber) const{
  if (LinearNber < 0 || LinearNber >= GetNberOfElements()) return 0;
  return  RpcPrepDataVector[LinearNber];
}
const Muon::TgcPrepData* MB_TrackAth::LiIdGetTgcPrepData(int LinearNber) const{
  if (LinearNber < 0 || LinearNber >= GetNberOfElements()) return 0;
  return  TgcPrepDataVector[LinearNber];
}
const Muon::CscStripPrepData* MB_TrackAth::LiIdGetCscStripPrepData(int LinearNber) const{
  if (LinearNber < 0 || LinearNber >= GetNberOfElements()) return 0;
  return  CscStripPrepDataVector[LinearNber];
}
const Muon::CscPrepData* MB_TrackAth::LiIdGetCscClusterPrepData(int LinearNber) const{
  if (LinearNber < 0 || LinearNber >= GetNberOfElements()) return 0;
  return  CscCluPrepDataVector[LinearNber];
}
