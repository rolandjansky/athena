/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHighPtBoardOut.hh"

namespace LVL1TGCTrigger {

TGCHighPtBoardOut::TGCHighPtBoardOut(TGCHighPtBoard* hpt, int bidIn):
  m_origin(hpt), m_bid(bidIn)
{
}

TGCHighPtBoardOut::TGCHighPtBoardOut():
  m_origin(0), m_bid(-1)
{
}

void TGCHighPtBoardOut::clear()
{
  int i,j;
  for( i=0; i<NumberOfChip ; i+=1)
    for( j=0; j<MaxNumberOfHPBData; j+=1){
      m_hit[i][j] = false;
      m_select[i][j] = 0;
    }
}

} //end of namespace bracket
