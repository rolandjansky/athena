/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSlaveBoardOut.hh"
#include "TrigT1TGC/TGCSlaveBoard.hh"

namespace LVL1TGCTrigger {

TGCSlaveBoardOut::TGCSlaveBoardOut(const TGCSlaveBoard* sb, int bidIn)
  :m_origin(sb), m_bid(bidIn), m_orgBoardType(-1), m_orgSBid(-1),
   m_numberOfData(0)
{
  for(int i=0; i<MaxNumberOfSBData; i++) {
    m_bpos[i]=0;
    m_dev[i] =0;
    m_pos[i] =0;
    m_hit[i] = false;
  }
  for(int i=0; i<2; i++) m_bdev[i]=0; // 2=TotalNumberOfOutputData[1]
  
  if (!m_origin) return;
  
  m_orgBoardType = m_origin->getType(); //type = 0,1,2,3 : WT,WD,ST,SD
  m_orgSBid      = m_origin->getId();
  
  for(int i=0; i < TotalNumberOfOutputData[m_orgBoardType]; i++) {
    m_bpos[i] = new TGCHitPattern(posSize[m_orgBoardType]);
  }
  if ((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)){
    for(int i=0; i<TotalNumberOfOutputData[1]; i++) {
      m_bdev[i] = new TGCHitPattern(1);
    }
  }
}

TGCSlaveBoardOut& TGCSlaveBoardOut::operator=(const TGCSlaveBoardOut& right)
{
  if (this != &right){
    for(int i=0; i<MaxNumberOfSBData; i++) {
      delete m_bpos[i];
      m_bpos[i]=0;
      m_dev[i] =0;
      m_pos[i] =0;
      m_hit[i] = false;
    }
    for(int i=0; i<2; i++) { // 2=TotalNumberOfOutputData[1]
      delete m_bdev[i];
      m_bdev[i]=0;
    } 

    m_origin = right.m_origin;
    m_bid    = right.m_bid;
    m_orgBoardType = right.m_orgBoardType;
    m_orgSBid      = right.m_orgSBid;
    m_numberOfData = right.m_numberOfData;
 
    if (m_orgBoardType >=0) {
      for(int i=0; i < TotalNumberOfOutputData[m_orgBoardType]; i++) {
        m_bpos[i] = new TGCHitPattern(*(right.m_bpos[i]));
        m_dev[i]  = right.m_dev[i];
        m_pos[i]  = right.m_pos[i];
        m_hit[i]  = right.m_hit[i];
      }
      if ((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)){
        for(int i=0; i<TotalNumberOfOutputData[1]; i++) {
          m_bdev[i] = new TGCHitPattern(*(right.m_bdev[i]));
        }  
      }
    }
  }
  return *this;
}

TGCSlaveBoardOut::TGCSlaveBoardOut(const TGCSlaveBoardOut& right)
  :m_origin(right.m_origin), m_bid(right.m_bid), 
   m_orgBoardType(-1), m_orgSBid(-1),
   m_numberOfData(right.m_numberOfData)
{
  for(int i=0; i<MaxNumberOfSBData; i++) {
    m_bpos[i]=0;
    m_dev[i] =0;
    m_pos[i] =0;
    m_hit[i] = false;
  }
  for(int i=0; i<2; i++) m_bdev[i]=0; // 2=TotalNumberOfOutputData[1]
  
  if (!m_origin) return;
  
  m_orgBoardType = m_origin->getType(); //type = 0,1,2,3 : WT,WD,ST,SD
  m_orgSBid      = m_origin->getId();
  
  for(int i=0; i < TotalNumberOfOutputData[m_orgBoardType]; i++) {
    m_bpos[i] = new TGCHitPattern(*(right.m_bpos[i]));
    m_dev[i]  = right.m_dev[i];
    m_pos[i]  = right.m_pos[i];
    m_hit[i]  = right.m_hit[i];
  }
  if ((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)){
    for(int i=0; i<TotalNumberOfOutputData[1]; i++) {
      m_bdev[i] = new TGCHitPattern(*(right.m_bdev[i]));
    }
  }
}

TGCSlaveBoardOut::TGCSlaveBoardOut()
  :m_origin(0), m_bid(-1), 
   m_orgBoardType(-1), m_orgSBid(-1),
   m_numberOfData(0)
{
  for(int i=0; i<MaxNumberOfSBData; i++) {
    m_bpos[i]=0;
    m_dev[i] =0;
    m_pos[i] =0;
    m_hit[i] = false;
  }
  for(int i=0; i<2/* 2=TotalNumberOfOutputData[1] */; i++) m_bdev[i]=0;
}

TGCSlaveBoardOut::~TGCSlaveBoardOut()
{
  for(int i=0; i<TotalNumberOfOutputData[m_orgBoardType]; i++) {
    if(m_bpos[i]) {
      delete m_bpos[i];
      m_bpos[i] = 0;
    }
  }
  for(int i=0; i<TotalNumberOfOutputData[1]; i++) {
    if(m_bdev[i]){
      delete m_bdev[i];
      m_bdev[i] = 0;
    }
  }
}



void TGCSlaveBoardOut::setbDev(int block, int sign, int dr)
{
  if (block >= TotalNumberOfOutputData[m_orgBoardType]){
    std::cerr << "TGCSlaveBoardOut::setbDev  : illegal block "
              << " Type=" << m_orgBoardType << " #block=" << TotalNumberOfOutputData[m_orgBoardType]
              << " block=" << block << std::endl;
    return; 
  }
  if((m_orgBoardType != SDSB) && (m_orgBoardType != WDSB)){
    std::cerr << "TGCSlaveBoardOut::setbDev  : illegal Type "
              << " Type=" << m_orgBoardType << " #block=" << TotalNumberOfOutputData[m_orgBoardType]
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
     m_bdev[block]->setChannel(0, true);
  } else {
     m_bdev[block]->setChannel(0, false);
  }
  m_bdev[block]->push_back(&tmp);
#ifdef TGCDEBUG
  m_bdev[block]->printb();
  std::cout << std::endl;
#endif
}

void TGCSlaveBoardOut::setPos(int iData, int posIn)
{
  if (MaxNumberOfSBData <= iData) {
     std::cerr << "internal error TGCSlaveBoardOut::setPos()" << std::endl;
  } else {
    m_pos[iData] = posIn;
  }
}


void TGCSlaveBoardOut::clear() 
{
  int i;
  m_numberOfData=0;
  for( i=0; i<MaxNumberOfSBData; i+=1)
    {
      m_hit[i]=false;
      m_pos[i]=-1;
      m_dev[i]=-99;
    }
}

void TGCSlaveBoardOut::print() const 
{
#ifdef TGCCOUT
  int OutPutBlock;
  std::cout<<" SlaveBoardOut:";
  std::cout<<" SBID:"<< m_orgSBid;
  
  switch (m_orgBoardType)
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
  std::cout<<" bid:" << m_bid;
  for( OutPutBlock=0; OutPutBlock<m_numberOfData; OutPutBlock+=1)
    if(m_hit[OutPutBlock]) std::cout << "  [OutPutBlock=" << OutPutBlock << " x=" << m_pos[OutPutBlock] << " d=" << m_dev[OutPutBlock] << "]";
  std::cout<<std::endl;
  
  int count = 0;
  for( OutPutBlock=0; OutPutBlock<m_numberOfData; OutPutBlock+=1)
    {
      if(m_hit[OutPutBlock] && m_bpos[OutPutBlock])
	{
	  count++;
	  std::cout << "  bit pattern [OutPutBlock=" << OutPutBlock << " x=";
	  m_bpos[OutPutBlock]->printb();
	  if(((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)) && m_bdev[OutPutBlock])
	    {
	      std::cout << " d=";
	      m_bdev[OutPutBlock]->printb();
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
  std::cout<<" SBID:"<< m_orgSBid;
#endif
  switch (m_orgBoardType)
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
  std::cout<<" bid:" << m_bid;
  if(m_hit[OutPutBlock]) std::cout << "  [OutPutBlock=" << OutPutBlock << " x=" << m_pos[OutPutBlock] << " d=" << m_dev[OutPutBlock] << "]";

  if(m_hit[OutPutBlock] && m_bpos[OutPutBlock])
    {
      std::cout << "  bit pattern [OutPutBlock=" << OutPutBlock << " x=";
      m_bpos[OutPutBlock]->printb();
      if(((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)) && m_bdev[OutPutBlock])
	{
	  std::cout << " d=";
	  m_bdev[OutPutBlock]->printb();
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
  
  for(OutPutBlock = 0; OutPutBlock < m_numberOfData; OutPutBlock++)
    if(m_hit[OutPutBlock] && m_bpos[OutPutBlock])
      tHit = true;
  if(!tHit)return;
  
  *ofs << "#SlaveBoardOut:";
  *ofs << " SBID:" << m_orgSBid;
  
  switch (m_orgBoardType)
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
  *ofs << " bid:" << m_bid;
  for(OutPutBlock = 0; OutPutBlock < m_numberOfData; OutPutBlock++)
    if(m_hit[OutPutBlock] && m_bpos[OutPutBlock])
      {
	*ofs << "  [OutPutBlock=" << OutPutBlock << "/" << m_numberOfData-1 << " x=" << m_pos[OutPutBlock] << " d=" << m_dev[OutPutBlock] << "]";
	*ofs << "[OutPutBlock=" << OutPutBlock << " x=";
	m_bpos[OutPutBlock]->printb(ofs);
	if(((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)) && m_bdev[OutPutBlock])
	  {
	    *ofs << " d=";
	    m_bdev[OutPutBlock]->printb(ofs);
	  }
	*ofs << "]";
      }
  *ofs << std::endl;
}

void TGCSlaveBoardOut::print(std::ofstream* ofs, int OutPutBlock) const
{
  if(!(m_hit[OutPutBlock] && m_bpos[OutPutBlock]))return;
  
  *ofs << "#SlaveBoardOut:";
  *ofs << " SBID:" << m_orgSBid;
  
  switch (m_orgBoardType)
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
  *ofs << " bid:" << m_bid;
  if(m_hit[OutPutBlock] && m_bpos[OutPutBlock])
    {
      *ofs << "  [OutPutBlock=" << OutPutBlock << " x=" << m_pos[OutPutBlock] << " d=" << m_dev[OutPutBlock] << "]";
      *ofs << "[OutPutBlock=" << OutPutBlock;
      *ofs << " x=";
      m_bpos[OutPutBlock]->printb(ofs);
      if(((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)) && m_bdev[OutPutBlock])
	{
	  *ofs << " d=";
	  m_bdev[OutPutBlock]->printb(ofs);
        }
      *ofs << "]";
    }
  *ofs << std::endl;
}

void TGCSlaveBoardOut::printpattern(std::ofstream* ofs) const
{
  int OutPutBlock;
  
  for(OutPutBlock = 0; OutPutBlock < m_numberOfData; OutPutBlock++)
    if(m_hit[OutPutBlock] && m_bpos[OutPutBlock])
      {
	*ofs << " ";
	if((m_orgBoardType == WTSB)|| (m_orgBoardType == STSB))
	  {
	    *ofs << "1"; //m_hit
	  }
	m_bpos[OutPutBlock]->printb(ofs);
	if(((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)) && m_bdev[OutPutBlock])
	  {
	    *ofs << " ";
	    m_bdev[OutPutBlock]->printb(ofs);
	  }
      }
    else
      {
	if((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB))
	  {
	    *ofs << " 00000 0000";
	  }
	else if(m_orgBoardType == WTSB)
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
  if(m_hit[OutPutBlock] && m_bpos[OutPutBlock])
    {
      *ofs << " ";
      if((m_orgBoardType == WTSB) || (m_orgBoardType == STSB))
	{
	  *ofs << "1"; //m_hit
        }
      m_bpos[OutPutBlock]->printb(ofs);
      if(((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB)) && m_bdev[OutPutBlock])
	{
	  *ofs << " ";
	  m_bdev[OutPutBlock]->printb(ofs);
        }
    }
  else
    {
      if((m_orgBoardType == SDSB) || (m_orgBoardType == WDSB))
	{
	  *ofs << " 00000 0000";
        }
      else if(m_orgBoardType == WTSB)
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
