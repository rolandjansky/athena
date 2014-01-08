/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyGenTrackStore.h"
 
MboyGenTrackStore::MboyGenTrackStore(){}
 
MboyGenTrackStore::~MboyGenTrackStore(){}

// Reset the Store
void MboyGenTrackStore::Reset(){MboyGenTrackSet.clear();}
 
// Print Out the Store
void MboyGenTrackStore::PrintSummary() const{

//*Print out Total Nber of MboyGenTrack
   std::cout << "*  The Nber of MboyGenTrack in the MboyGenTrackStore is "
             << NberOfMboyGenTrack()
             << std::endl;

}
void MboyGenTrackStore::Print
  (std::ostream* out) const{

  int NberOfGenTracks = NberOfMboyGenTrack();
  for (int GenTrackNber=0; GenTrackNber<NberOfGenTracks; GenTrackNber++){
   GetMboyGenTrack(GenTrackNber)->Print(out) ;
  }

}
 
// Add a MboyGenTrack
void MboyGenTrackStore::Add(MboyGenTrack ToBeAdded){MboyGenTrackSet.push_back(ToBeAdded);}
 
// Nber of MboyGenTrack
int MboyGenTrackStore::NberOfMboyGenTrack() const{return MboyGenTrackSet.size();}
 
// Get a Pointer on a MboyGenTrack of Nber MboyGenTrackNber
const MboyGenTrack* MboyGenTrackStore::GetMboyGenTrack(int MboyGenTrackNber) const{
 const MboyGenTrack* ToBeReturned = 0 ;
 if (MboyGenTrackNber < 0 || MboyGenTrackNber > NberOfMboyGenTrack()) return ToBeReturned;
 return &MboyGenTrackSet[MboyGenTrackNber];
}
