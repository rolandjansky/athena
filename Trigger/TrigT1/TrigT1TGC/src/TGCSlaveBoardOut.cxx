/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSlaveBoardOut.hh"
#include "TrigT1TGC/TGCSlaveBoard.hh"

namespace LVL1TGCTrigger {

TGCSlaveBoardOut::TGCSlaveBoardOut(const TGCSlaveBoard* sb, int bidIn)
  :origin(sb), bid(bidIn), orgBoardType(-1), orgSBid(-1),
   numberOfData(0)
{
  for(int i=0; i<MaxNumberOfSBData; i++) {
    bpos[i]=0;
    dev[i] =0;
    pos[i] =0;
    hit[i] = false;
  }
  for(int i=0; i<2; i++) bdev[i]=0; // 2=TotalNumberOfOutputData[1]
  
  if (!origin) return;
  
  orgBoardType = origin->getType(); //type = 0,1,2,3 : WT,WD,ST,SD
  orgSBid      = origin->getId();
  
  for(int i=0; i < TotalNumberOfOutputData[orgBoardType]; i++) {
    bpos[i] = new TGCHitPattern(posSize[orgBoardType]);
  }
  if ((orgBoardType == SDSB) || (orgBoardType == WDSB)){
    for(int i=0; i<TotalNumberOfOutputData[1]; i++) {
      bdev[i] = new TGCHitPattern(1);
    }
  }
}

TGCSlaveBoardOut& TGCSlaveBoardOut::operator=(const TGCSlaveBoardOut& right)
{
  if (this != &right){
    for(int i=0; i<MaxNumberOfSBData; i++) {
      delete bpos[i];
      bpos[i]=0;
      dev[i] =0;
      pos[i] =0;
      hit[i] = false;
    }
    for(int i=0; i<2; i++) { // 2=TotalNumberOfOutputData[1]
      delete bdev[i];
      bdev[i]=0;
    } 

    origin = right.origin;
    bid    = right.bid;
    orgBoardType = right.orgBoardType;
    orgSBid      = right.orgSBid;
    numberOfData = right.numberOfData;
 
    if (orgBoardType >=0) {
      for(int i=0; i < TotalNumberOfOutputData[orgBoardType]; i++) {
        bpos[i] = new TGCHitPattern(*(right.bpos[i]));
        dev[i]  = right.dev[i];
        pos[i]  = right.pos[i];
        hit[i]  = right.hit[i];
      }
      if ((orgBoardType == SDSB) || (orgBoardType == WDSB)){
        for(int i=0; i<TotalNumberOfOutputData[1]; i++) {
          bdev[i] = new TGCHitPattern(*(right.bdev[i]));
        }  
      }
    }
  }
  return *this;
}

TGCSlaveBoardOut::TGCSlaveBoardOut(const TGCSlaveBoardOut& right)
  :origin(right.origin), bid(right.bid), 
   orgBoardType(-1), orgSBid(-1),
   numberOfData(right.numberOfData)
{
  for(int i=0; i<MaxNumberOfSBData; i++) {
    bpos[i]=0;
    dev[i] =0;
    pos[i] =0;
    hit[i] = false;
  }
  for(int i=0; i<2; i++) bdev[i]=0; // 2=TotalNumberOfOutputData[1]
  
  if (!origin) return;
  
  orgBoardType = origin->getType(); //type = 0,1,2,3 : WT,WD,ST,SD
  orgSBid      = origin->getId();
  
  for(int i=0; i < TotalNumberOfOutputData[orgBoardType]; i++) {
    bpos[i] = new TGCHitPattern(*(right.bpos[i]));
    dev[i]  = right.dev[i];
    pos[i]  = right.pos[i];
    hit[i]  = right.hit[i];
  }
  if ((orgBoardType == SDSB) || (orgBoardType == WDSB)){
    for(int i=0; i<TotalNumberOfOutputData[1]; i++) {
      bdev[i] = new TGCHitPattern(*(right.bdev[i]));
    }
  }
}

TGCSlaveBoardOut::TGCSlaveBoardOut()
  :origin(0), bid(-1), 
   orgBoardType(-1), orgSBid(-1),
   numberOfData(0)
{
  for(int i=0; i<MaxNumberOfSBData; i++) {
    bpos[i]=0;
    dev[i] =0;
    pos[i] =0;
    hit[i] = false;
  }
  for(int i=0; i<2/* 2=TotalNumberOfOutputData[1] */; i++) bdev[i]=0;
}

TGCSlaveBoardOut::~TGCSlaveBoardOut()
{
  for(int i=0; i<TotalNumberOfOutputData[orgBoardType]; i++) {
    if(bpos[i]) {
      delete bpos[i];
      bpos[i] = 0;
    }
  }
  for(int i=0; i<TotalNumberOfOutputData[1]; i++) {
    if(bdev[i]){
      delete bdev[i];
      bdev[i] = 0;
    }
  }
}



void TGCSlaveBoardOut::setbDev(int block, int sign, int dr)
{
  if (block >= TotalNumberOfOutputData[orgBoardType]){
    std::cerr << "TGCSlaveBoardOut::setbDev  : illegal block "
              << " Type=" << orgBoardType << " #block=" << TotalNumberOfOutputData[orgBoardType]
              << " block=" << block << std::endl;
    return; 
  }
  if((orgBoardType != SDSB) && (orgBoardType != WDSB)){
    std::cerr << "TGCSlaveBoardOut::setbDev  : illegal Type "
              << " Type=" << orgBoardType << " #block=" << TotalNumberOfOutputData[orgBoardType]
              << " block=" << block << std::endl;
    return;
  }
  
  TGCHitPattern tmp(3);
  tmp.dec2bin(dr);
#ifdef TGCDEBUG
  tmp.printb();
  std::cout << ":" << dr << " TGCSlaveBoardOut::setbDev" << std::endl;
#endif
  if (0 <= sign*dr) {
     bdev[block]->setChannel(0, true);
  } else {
     bdev[block]->setChannel(0, false);
  }
  bdev[block]->push_back(&tmp);
#ifdef TGCDEBUG
  bdev[block]->printb();
  std::cout << std::endl;
#endif
}

void TGCSlaveBoardOut::setPos(int iData, int posIn)
{
  if (MaxNumberOfSBData <= iData) {
     std::cerr << "internal error TGCSlaveBoardOut::setPos()" << std::endl;
  } else {
    pos[iData] = posIn;
  }
}


void TGCSlaveBoardOut::clear() 
{
  int i;
  numberOfData=0;
  for( i=0; i<MaxNumberOfSBData; i+=1)
    {
      hit[i]=false;
      pos[i]=-1;
      dev[i]=-99;
    }
}

void TGCSlaveBoardOut::print() const 
{
#ifdef TGCCOUT
  int OutPutBlock;
  std::cout<<" SlaveBoardOut:";
  std::cout<<" SBID:"<< orgSBid;
  
  switch (orgBoardType)
    {
    case WTSB:
      std::cout<<" SBType:" << "WTSB";
      break;
    case WDSB:
      std::cout<<" SBType:" << "WDSB";
      break;
    case STSB:
      std::cout<<" SBType:" << "STSB";
      break;
    case SDSB:
      std::cout<<" SBType:" << "SDSB";
      break;
    }
  std::cout<<" bid:" << bid;
  for( OutPutBlock=0; OutPutBlock<numberOfData; OutPutBlock+=1)
    if(hit[OutPutBlock]) std::cout << "  [OutPutBlock=" << OutPutBlock << " x=" << pos[OutPutBlock] << " d=" << dev[OutPutBlock] << "]";
  std::cout<<std::endl;
  
  int count = 0;
  for( OutPutBlock=0; OutPutBlock<numberOfData; OutPutBlock+=1)
    {
      if(hit[OutPutBlock] && bpos[OutPutBlock])
	{
	  count++;
	  std::cout << "  bit pattern [OutPutBlock=" << OutPutBlock << " x=";
	  bpos[OutPutBlock]->printb();
	  if(((orgBoardType == SDSB) || (orgBoardType == WDSB)) && bdev[OutPutBlock])
	    {
	      std::cout << " d=";
	      bdev[OutPutBlock]->printb();
            }
	  std::cout << "]";
        }
    }
  if(count)std::cout<<std::endl;
#endif
}

#ifdef TGCCOUT
void TGCSlaveBoardOut::print(int OutPutBlock )  const
#else
void TGCSlaveBoardOut::print(int /*OutPutBlock*/)  const
#endif
{
#ifdef TGCCOUT
  std::cout<<" SlaveBoardOut:";
  std::cout<<" SBID:"<< orgSBid;
#endif
  switch (orgBoardType)
    {
    case WTSB:
#ifdef TGCCOUT
      std::cout<<" SBType:" << "WTSB";
#endif
      break;
    case WDSB:
#ifdef TGCCOUT
      std::cout<<" SBType:" << "WDSB";
#endif
      break;
    case STSB:
#ifdef TGCCOUT
      std::cout<<" SBType:" << "STSB";
#endif
      break;
    case SDSB:
#ifdef TGCCOUT
      std::cout<<" SBType:" << "SDSB";
#endif
      break;
    }
#ifdef TGCCOUT
  std::cout<<" bid:" << bid;
  if(hit[OutPutBlock]) std::cout << "  [OutPutBlock=" << OutPutBlock << " x=" << pos[OutPutBlock] << " d=" << dev[OutPutBlock] << "]";

  if(hit[OutPutBlock] && bpos[OutPutBlock])
    {
      std::cout << "  bit pattern [OutPutBlock=" << OutPutBlock << " x=";
      bpos[OutPutBlock]->printb();
      if(((orgBoardType == SDSB) || (orgBoardType == WDSB)) && bdev[OutPutBlock])
	{
	  std::cout << " d=";
	  bdev[OutPutBlock]->printb();
        }
      std::cout << "]";
    }
  std::cout<<std::endl;
#endif
}

void TGCSlaveBoardOut::print(std::ofstream* ofs) const 
{
  int OutPutBlock;
  bool tHit = false;
  
  for(OutPutBlock = 0; OutPutBlock < numberOfData; OutPutBlock++)
    if(hit[OutPutBlock] && bpos[OutPutBlock])
      tHit = true;
  if(!tHit)return;
  
  *ofs << "#SlaveBoardOut:";
  *ofs << " SBID:" << orgSBid;
  
  switch (orgBoardType)
    {
    case WTSB:
      *ofs << " SBType:" << "WTSB";
      break;
    case WDSB:
      *ofs << " SBType:" << "WDSB";
      break;
    case STSB:
      *ofs << " SBType:" << "STSB";
      break;
    case SDSB:
      *ofs << " SBType:" << "SDSB";
      break;
    }
  *ofs << " bid:" << bid;
  for(OutPutBlock = 0; OutPutBlock < numberOfData; OutPutBlock++)
    if(hit[OutPutBlock] && bpos[OutPutBlock])
      {
	*ofs << "  [OutPutBlock=" << OutPutBlock << "/" << numberOfData-1 << " x=" << pos[OutPutBlock] << " d=" << dev[OutPutBlock] << "]";
	*ofs << "[OutPutBlock=" << OutPutBlock << " x=";
	bpos[OutPutBlock]->printb(ofs);
	if(((orgBoardType == SDSB) || (orgBoardType == WDSB)) && bdev[OutPutBlock])
	  {
	    *ofs << " d=";
	    bdev[OutPutBlock]->printb(ofs);
	  }
	*ofs << "]";
      }
  *ofs << std::endl;
}

void TGCSlaveBoardOut::print(std::ofstream* ofs, int OutPutBlock) const
{
  if(!(hit[OutPutBlock] && bpos[OutPutBlock]))return;
  
  *ofs << "#SlaveBoardOut:";
  *ofs << " SBID:" << orgSBid;
  
  switch (orgBoardType)
    {
    case WTSB:
      *ofs << " SBType:" << "WTSB";
      break;
    case WDSB:
      *ofs << " SBType:" << "WDSB";
      break;
    case STSB:
      *ofs << " SBType:" << "STSB";
      break;
    case SDSB:
      *ofs << " SBType:" << "SDSB";
      break;
    }
  *ofs << " bid:" << bid;
  if(hit[OutPutBlock] && bpos[OutPutBlock])
    {
      *ofs << "  [OutPutBlock=" << OutPutBlock << " x=" << pos[OutPutBlock] << " d=" << dev[OutPutBlock] << "]";
      *ofs << "[OutPutBlock=" << OutPutBlock;
      *ofs << " x=";
      bpos[OutPutBlock]->printb(ofs);
      if(((orgBoardType == SDSB) || (orgBoardType == WDSB)) && bdev[OutPutBlock])
	{
	  *ofs << " d=";
	  bdev[OutPutBlock]->printb(ofs);
        }
      *ofs << "]";
    }
  *ofs << std::endl;
}

void TGCSlaveBoardOut::printpattern(std::ofstream* ofs) const
{
  int OutPutBlock;
  
  for(OutPutBlock = 0; OutPutBlock < numberOfData; OutPutBlock++)
    if(hit[OutPutBlock] && bpos[OutPutBlock])
      {
	*ofs << " ";
	if((orgBoardType == WTSB)|| (orgBoardType == STSB))
	  {
	    *ofs << "1"; //hit
	  }
	bpos[OutPutBlock]->printb(ofs);
	if(((orgBoardType == SDSB) || (orgBoardType == WDSB)) && bdev[OutPutBlock])
	  {
	    *ofs << " ";
	    bdev[OutPutBlock]->printb(ofs);
	  }
      }
    else
      {
	if((orgBoardType == SDSB) || (orgBoardType == WDSB))
	  {
	    *ofs << " 00000 0000";
	  }
	else if(orgBoardType == WTSB)
	  {
	    *ofs << " 000000";
	  }
	else
	  {
	    *ofs << " 00000";
	  }
      }
}

void TGCSlaveBoardOut::printpattern(std::ofstream* ofs, int OutPutBlock) const
{
  if(hit[OutPutBlock] && bpos[OutPutBlock])
    {
      *ofs << " ";
      if((orgBoardType == WTSB) || (orgBoardType == STSB))
	{
	  *ofs << "1"; //hit
        }
      bpos[OutPutBlock]->printb(ofs);
      if(((orgBoardType == SDSB) || (orgBoardType == WDSB)) && bdev[OutPutBlock])
	{
	  *ofs << " ";
	  bdev[OutPutBlock]->printb(ofs);
        }
    }
  else
    {
      if((orgBoardType == SDSB) || (orgBoardType == WDSB))
	{
	  *ofs << " 00000 0000";
        }
      else if(orgBoardType == WTSB)
	{
	  *ofs << " 000000";
        }
      else
	{
	  *ofs << " 00000";
        }
    }
}

} //end of namespace bracket
