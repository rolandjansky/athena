/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHighPtChipOut.hh"
#include "TrigT1TGC/TGCHighPtBoard.hh"

namespace LVL1TGCTrigger {

TGCHighPtChipOut::TGCHighPtChipOut(TGCHighPtBoard* hpt, int bidIn)
  :origin(hpt), bid(bidIn)
{
  clear();
}

TGCHighPtChipOut::TGCHighPtChipOut()
  :origin(0), bid(-1)
{
  clear();
}

TGCHighPtChipOut::TGCHighPtChipOut(const TGCHighPtChipOut& right )
  :origin(right.origin), bid(right.bid)
{
  for(int i=0; i<NumberOfChip ; i+=1) {
    for(int j=0; j<NHitInTrackSelector; j+=1){
      pt[i][j]     = right.pt[i][j];
      pos[i][j]    = right.pos[i][j];
      hitID[i][j]  = right.hitID[i][j];
      dev[i][j]    = right.dev[i][j];
      select[i][j] = right.select[i][j];
    }
  }
}

TGCHighPtChipOut& TGCHighPtChipOut::operator=(const TGCHighPtChipOut& right )
{
  if (this != &right){
    origin = right.origin;
    bid    = right.bid;
    for(int i=0; i<NumberOfChip ; i+=1) {
      for(int j=0; j<NHitInTrackSelector; j+=1){
        pt[i][j]     = right.pt[i][j];
        pos[i][j]    = right.pos[i][j];
        hitID[i][j]  = right.hitID[i][j];
        dev[i][j]    = right.dev[i][j];
        select[i][j] = right.select[i][j];
      }
    }
  }
  return *this;
}
TGCHighPtChipOut::~TGCHighPtChipOut()
{
  origin=0;
  bid = -1;
  clear();
}


void TGCHighPtChipOut::clear()
{
  for(int i=0; i<NumberOfChip ; i+=1) {
    for(int j=0; j<NHitInTrackSelector; j+=1){
      pt[i][j]     = 0;
      pos[i][j]    = 0;
      hitID[i][j]  = 0;
      dev[i][j]    = 0;
      select[i][j] = 0;
    }
  }
}

} //end of namespace bracket
