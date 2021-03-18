/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCTMDB.h"

namespace LVL1TGCTrigger {

TGCTMDB::TGCTMDB()
{
  for (int side=0; side < 2; side++) {
    for (int mod=0; mod < NumberOfTileModule; mod++) {
      m_buffer[side*NumberOfTileModule + mod] = new TGCTMDBOut(side, mod); 
    }
  }
}

TGCTMDB::~TGCTMDB()
{
  for (int idx=0; idx<2*NumberOfTileModule; idx++){
    delete m_buffer[idx]; 
  }
}

TGCTMDB::TGCTMDB(const TGCTMDB& right)
{
  for (int idx=0; idx<2*NumberOfTileModule; idx++){
    m_buffer[idx] = 0; 
  }
  *this= right;
}

TGCTMDB& TGCTMDB::operator = (const TGCTMDB& right)
{
  if (this != &right) {
    for (int idx=0; idx<2*NumberOfTileModule; idx++){
      delete m_buffer[idx];
      m_buffer[idx] = new TGCTMDBOut(*(right.m_buffer[idx]));
    }
  }
  return *this;
}
  
const TGCTMDBOut* TGCTMDB::getOutput(int side, int mod) const
{
  if ( (side<0)||(side>1) ) return 0;
  if ( (mod<0)||(mod>=NumberOfTileModule) ) return 0;
  return m_buffer[side*NumberOfTileModule + mod] ;
}

const TGCTMDBOut* TGCTMDB::getOutput(int side, int sector, int mod) const
{
  if ((side<0)||(side>1)) return 0;
  if ((sector<0)||(sector>47)) return 0;
  if ((mod<0)||(mod>3)) return 0;
  int octant = sector / 6;
  int sec = sector % 6;
  int offset = 0;
  if      (sec==0) offset = -4;
  else if (sec==1) offset = -4; // same SL board as sec#0   
  else if (sec==2) offset =  0;
  else if (sec==3) offset =  0; // same SL board as sec#1
  else if (sec==4) offset =  2;
  else if (sec==5) offset =  2; // same SL board as sec#2
  int moduleID = (octant*(NumberOfTileModule/8) + offset + NumberOfTileModule) % NumberOfTileModule;
  moduleID = (moduleID + mod) % NumberOfTileModule;
  return m_buffer[side*NumberOfTileModule + moduleID];
}

void TGCTMDB::setOutput(const int side, const int module,
                        const TGCTMDBOut::TileModuleHit hit56,
                        const TGCTMDBOut::TileModuleHit hit6)
{
  if ( (side<0)||(side>1) ) return;
  if ( (module<0)||(module>=NumberOfTileModule) ) return;
  m_buffer[side*NumberOfTileModule +module]->setHit56(hit56);
  m_buffer[side*NumberOfTileModule +module]->setHit6(hit6);
}

void TGCTMDB::eraseOutput()
{
  for (int idx=0; idx<2*NumberOfTileModule; idx++){
    m_buffer[idx]->clear(); 
  }
}

int TGCTMDB::getInnerTileBits(int side, int sector) const
{
  int inner_tile = 0;  

  for (int ii = 0; ii < 4; ii++) {
    TGCTMDBOut::TileModuleHit hit56 = getOutput(side, sector, ii)->getHit56();
    TGCTMDBOut::TileModuleHit hit6  = getOutput(side, sector, ii)->getHit6();

    int tmp_56 = (hit56 == TGCTMDBOut::TM_LOW || hit56 == TGCTMDBOut::TM_HIGH) ? 1 : 0;
    int tmp_6  = (hit6  == TGCTMDBOut::TM_LOW || hit6  == TGCTMDBOut::TM_HIGH) ? 1 : 0;
    
    int tmp_all = (tmp_6 << 1) | (tmp_56);

    inner_tile |= (tmp_all << (ii*2));
  }

  return inner_tile;
}

void TGCTMDB::print() const
{
  for (int idx=0; idx<2*NumberOfTileModule; idx++){
    m_buffer[idx]->print(); 
  }
}
  

}   // end of namespace
