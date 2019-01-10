/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHighPtBoardOut.hh"

namespace LVL1TGCTrigger {

TGCHighPtBoardOut::TGCHighPtBoardOut(TGCHighPtBoard* hpt, int bidIn):
  origin(hpt), bid(bidIn)
{
}

TGCHighPtBoardOut::TGCHighPtBoardOut():
  origin(0), bid(-1)
{
}

void TGCHighPtBoardOut::clear()
{
  int i,j;
  for( i=0; i<NumberOfChip ; i+=1)
    for( j=0; j<MaxNumberOfHPBData; j+=1){
      hit[i][j] = false;
      select[i][j] = 0;
    }
}

} //end of namespace bracket
