/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __H_FTK_HITMASKVECTOR
#define __H_FTK_HITMASKVECTOR

#include <inttypes.h>
#include <vector>
#include <utility>
#include <cstring>
#include <iostream>
#include <iomanip>

typedef uint8_t FTK_HitMask;

class FTK_HitMaskIterator {
 public:
   FTK_HitMaskIterator(FTK_HitMask *ptr=0) : m_ptr(ptr) { }
   void setLayer(uint8_t layer) { m_mask=1<<layer; }
   inline void update4(int delta4) { m_ptr += delta4<<2; }
   inline void update(int delta) { m_ptr += delta; }
   inline uint8_t getChange(FTK_HitMask xorMask) {
      return (((uint8_t)(xorMask-1))>>(8*sizeof(FTK_HitMask)-1))-1;
   }
   inline uint8_t process(void) {
      FTK_HitMask m0=*m_ptr;
      FTK_HitMask m1=m0|m_mask;
      *m_ptr=m1;
      return getChange(m0^m1);
   }
   inline uint8_t getNHit(int i) const {
      FTK_HitMask data= *(m_ptr+i);
      uint8_t nhit=
         (data & 0x11)+
         ((data>>1)&0x11)+
         ((data>>2)&0x11)+
         ((data>>3)&0x11);
      nhit=(nhit&0xf)+(nhit>>4);
      return nhit;
   }
   inline FTK_HitMask getHitMask(void) const {
      return *m_ptr;
   }
   inline FTK_HitMask const *getPtr(void) const { return m_ptr; }
 protected:
   FTK_HitMask __restrict *m_ptr;
   FTK_HitMask m_mask;
};

class FTK_HitMaskVector {
public:
   inline void setNPattern(uint32_t nPattern) {
      m_data.reserve(nPattern+3); m_data.resize(nPattern); }
   inline uint32_t memoryEstimate(void) const {
      return sizeof(FTK_HitMaskVector)+m_data.capacity()*sizeof(FTK_HitMask);
   }
   inline void init(uint32_t first,uint32_t last,FTK_HitMask data) {
      memset(m_data.data()+first,data,last+1-first);
   }
   inline FTK_HitMaskIterator getPointer(uint32_t pattern) {
      return FTK_HitMaskIterator(m_data.data()+pattern);
   }
   inline uint32_t getPatternID(FTK_HitMaskIterator const &ptr) {
      return ptr.getPtr()-m_data.data();
   }
 protected:
   std::vector<FTK_HitMask> m_data;
};

#endif
