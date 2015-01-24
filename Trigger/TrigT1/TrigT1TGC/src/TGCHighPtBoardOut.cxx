/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHighPtBoardOut.hh"

namespace LVL1TGCTrigger {

TGCHighPtBoardOut::TGCHighPtBoardOut(TGCHighPtBoard* hpt, int bidIn):
  origin(hpt), bid(bidIn)
{
  TGCHBChip chip;
  TGCHPBBlock block;
  for(chip = ChipA; chip < TotalNumTGCHBChip; chip++){
    for(block = HPBBlockA; block < TotalNumTGCHPBBlock; block++){
      bpos[chip][block] = 0;
      bdev[chip][block] = 0;
    }
  }
}

TGCHighPtBoardOut::TGCHighPtBoardOut():
  origin(0), bid(-1)
{
  TGCHBChip chip;
  TGCHPBBlock block;
  for(chip = ChipA; chip < TotalNumTGCHBChip; chip++){
    for(block = HPBBlockA; block < TotalNumTGCHPBBlock; block++){
      bpos[chip][block] = 0;
      bdev[chip][block] = 0;
    }
  }
}

TGCHighPtBoardOut::~TGCHighPtBoardOut()
{
  TGCHBChip chip;
  TGCHPBBlock block;
  
  origin=0;
  
  for(chip = ChipA; chip < TotalNumTGCHBChip; chip++){
    for(block = HPBBlockA; block < TotalNumTGCHPBBlock; block++){
      if(bpos[chip][block]){
	delete bpos[chip][block];
	bpos[chip][block] = 0;
      }
      if(bdev[chip][block]){
	delete bdev[chip][block];
	bdev[chip][block] = 0;
      }
    }
  }
}
  

void TGCHighPtBoardOut::setbDev(TGCHBChip chip, TGCHPBBlock block, int sign, int dr)
{
  TGCHitPattern tmp(3);
  tmp.dec2bin(dr);
  tmp.printb();
  std::cout << ":" << dr << " TGCSlaveBoardOut::setbDev" << std::endl;
  if(0 <= sign*dr){
    bdev[chip][block]->setChannel(0, true);
  }else{
    bdev[chip][block]->setChannel(0, false);
  }
  bdev[chip][block]->push_back(&tmp);
  bdev[chip][block]->printb();
  std::cout << std::endl;
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
