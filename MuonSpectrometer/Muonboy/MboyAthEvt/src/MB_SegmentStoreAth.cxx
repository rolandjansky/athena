/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyAthEvt/MB_SegmentStoreAth.h"
 
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

MB_SegmentStoreAth::MB_SegmentStoreAth():MB_SegmentStore(){}
MB_SegmentStoreAth::~MB_SegmentStoreAth(){}
 
// Reset the Store
void MB_SegmentStoreAth::ResetMB_SegmentStoreAth(){

  ResetMB_SegmentStore() ;

  MB_SegmentAthSet.clear();

}
 
// Consistentify
void MB_SegmentStoreAth::Consistentify
 (const MuonRecDigiStoreAth* pMuonRecDigiStoreAth){

  int NberOfSegments = NberOfMB_Segment();
  for (int SegmentNber=0; SegmentNber<NberOfSegments; SegmentNber++){
   MB_SegmentAth ToBeAdded(*GetMB_Segment(SegmentNber),pMuonRecDigiStoreAth);
   MB_SegmentAthSet.push_back(ToBeAdded);
  }

}

// Get a Pointer on a MB_SegmentAth of Nber MB_SegmentNber
const MB_SegmentAth* MB_SegmentStoreAth::GetMB_SegmentAth(int MB_SegmentNber) const{
 const MB_SegmentAth* ToBeReturned = 0 ;
 if (MB_SegmentNber < 0 || MB_SegmentNber > NberOfMB_Segment()) return ToBeReturned;
 ToBeReturned =  &MB_SegmentAthSet[MB_SegmentNber];
 return ToBeReturned;
}
