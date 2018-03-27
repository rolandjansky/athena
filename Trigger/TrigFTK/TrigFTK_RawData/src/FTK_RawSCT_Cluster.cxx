/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"
#include <iostream>

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

using namespace std;

FTK_RawSCT_Cluster::FTK_RawSCT_Cluster() :
  m_word(0), 
  m_barcode(0)
{
  // nothing to do
}


FTK_RawSCT_Cluster::FTK_RawSCT_Cluster(uint32_t word)
{
  m_word        = word;
  m_barcode     = 0;
}

FTK_RawSCT_Cluster::FTK_RawSCT_Cluster(uint32_t word, signed long barcode)
{
  m_word        = word;
  m_barcode     = barcode;
}


FTK_RawSCT_Cluster::~FTK_RawSCT_Cluster()
{
  // nothing to do
}

  void FTK_RawSCT_Cluster::setWord(uint32_t word){ 
    m_word = word;
  }

float FTK_RawSCT_Cluster::getHitCoord() const{

  // Hit coordinate is 11 bits, starting at bit 16

  uint32_t hit = (m_word  & (0x7ff << 16)) >> 16;

  //  return 0.5 * (float)hit;
  return (float)hit;
}

void FTK_RawSCT_Cluster::setHitCoord( float hit_coord ){

  // TopBit 23 BottomBit 13  (11 bits) Max 2048 

  uint32_t hit = uint32_t (hit_coord);
  //  uint32_t hit = uint32_t (hit_coord*2.);

  if ( hit < 2048 ){
    hit = hit << 16 ;
    m_word = (hit | m_word);
  }
  return;
}

unsigned int FTK_RawSCT_Cluster::getHitWidth() const{

  // width is 3 bits, starting at bit 28
  unsigned int width = (m_word & 0x70000000)>> 28;

  return (width+1);  // transform from packed width=0 to 7 to width 1-8

}

void FTK_RawSCT_Cluster::setHitWidth( unsigned int hit_width ){

  // TopBit 26 BottomBit 24  (3 bits) Max 8

  uint32_t width = hit_width;

  if (width>0) width-=1; // transform from width=1 to 8 to packed width 0-7

  if(width > 7) width = 7;
  width = width << 28;

  m_word = (width | m_word);
  
  return;

}

// JAA modified this to use 13 bits
unsigned int FTK_RawSCT_Cluster::getModuleID() const{
  
  // First 13 bits
  uint32_t module_id = (m_word & 0x1fff);
  return module_id;

}
// JAA modified this to use 13 bits
void FTK_RawSCT_Cluster::setModuleID( unsigned int module_id ){

  uint32_t id = module_id;

  if (id <  8192) m_word = (id | m_word);
  
  return;
}



#if defined(__MAKECINT__)
#pragma link C++ class FTK_RawSCT_Cluster;
#endif
