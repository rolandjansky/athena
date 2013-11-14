/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyAthEvt/MB_TrackStoreAth.h"

#include "MboyAthEvt/MuonRecDigiStoreAth.h"

MB_TrackStoreAth::MB_TrackStoreAth():MB_TrackStore(){}

MB_TrackStoreAth::~MB_TrackStoreAth(){}

// Reset the Store
void MB_TrackStoreAth::ResetMB_TrackStoreAth(){

  ResetMB_TrackStore();

  MB_TrackAthSet.clear();

}

// Consistentify
void MB_TrackStoreAth::Consistentify
 (const MuonRecDigiStoreAth* pMuonRecDigiStoreAth){

  int NberOfTracks = NberOfMB_Track();
  for (int TrackNber=0; TrackNber<NberOfTracks; TrackNber++){
   MB_TrackAth ToBeAdded(*GetMB_Track(TrackNber),pMuonRecDigiStoreAth);
   MB_TrackAthSet.push_back(ToBeAdded);
  }

}

// Get a Pointer on a MB_TrackAth of Nber MB_TrackNber
const MB_TrackAth* MB_TrackStoreAth::GetMB_TrackAth(int MB_TrackNber) const{
 const MB_TrackAth* ToBeReturned = 0 ;
 if (MB_TrackNber < 0 || MB_TrackNber >= NberOfMB_Track()) return ToBeReturned;
 ToBeReturned =  &MB_TrackAthSet[MB_TrackNber];
 return ToBeReturned;
}
