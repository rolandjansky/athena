/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTKSIM_FTK_COMPRESSEDPATTERNLIST_H
#define TRIGFTKSIM_FTK_COMPRESSEDPATTERNLIST_H

#include <inttypes.h>
#include <set>
#include <vector>
#include <TObject.h>
#include "CxxUtils/restrict.h"

class FTK_CompressedPatternList {
 public:
   // crompression schemes
   enum ENCODING_SCHEME {
      ENCODING_MIN=1,
      ENCODING_U32=1,
      ENCODING_U7=2,
      ENCODING_DELTA=3,
      ENCODING_BIT4=4,
      ENCODING_DELTA2=5,
      ENCODING_MAX=5
   };

   // create
   FTK_CompressedPatternList(uint32_t firstPattern,uint32_t nPattern,
                             uint32_t nData,uint32_t encoding,uint8_t *data)
      : m_NPattern(nPattern) {
      setData(firstPattern,encoding,data,nData);
   }
   //
   inline void setData(uint32_t firstPattern,uint32_t encoding,
                       std::vector<uint8_t> &data) {
      setData(firstPattern,encoding,data.data(),data.size()); }
   void setData(uint32_t firstPattern,uint32_t encoding,
                uint8_t const *dataPtr,uint32_t nData);
   inline void setDataPointer(uint8_t const *data) { m_data=data; }
   // pattern range
   std::pair<uint32_t,uint32_t> getMinMaxPattern(void) const;
   // number of patterns
   inline uint32_t getNPattern(void) const { return m_NPattern; }
   // encoding scheme
   inline uint32_t getEncoding(void) const { return m_encoding; }
   // select encoding scheme
   uint32_t selectEncoding(uint32_t defaultMode) const;
   // change to different encoding scheme
   // returns: new data size if successful
   //          -1 if new encoding is worse than before
   void encode(uint32_t encoding,uint32_t *firstPattern,
               std::vector<uint8_t> &data) const;
   // export data (for TTree)
   void exportData(Int_t *firstPattern,Int_t *nPattern,Int_t *encoding,
                   std::vector<Int_t> &deltaData) const;
   // unpack and loop over all patterns
   template<class LOOP> inline void patternLoop(LOOP &loop) const {
      // start pattern ID
      loop.init(m_FirstPattern);
      uint8_t const * ATH_RESTRICT ptr=m_data;
      if(m_encoding == ENCODING_U7) {
         loop.process();
         for(int nPattern=m_loopHelper;nPattern>0;nPattern--) {
            uint32_t delta;
            delta=nextByte(ptr);
            loop.update(delta+1);
            loop.process();
         }
         for(int nPattern=m_NPattern-1-m_loopHelper;nPattern>0;nPattern--) {
            uint32_t delta;
            delta=nextByte(ptr);
            if(delta & 0x80) {
               uint32_t up=0;
               do {
                  up=(up|(delta&0x7f))<<7;
                  delta=nextByte(ptr);
               } while(delta & 0x80);
               delta |= up;
            }
            loop.update(delta+1);
            loop.process();
         }
      } else if(m_encoding == ENCODING_DELTA) {
         // delta scheme: first pattern is processed directly
         loop.process();
         //for(int nPattern=m_NPattern-1;nPattern>0;nPattern--) {
         for(int iLoop=m_loopHelper;iLoop>0;iLoop--) {
            uint32_t delta=nextByte(ptr)+1;
            if(delta>0x80) {
               if(delta>0xf0) {
                  delta -=0xef; // original 0xf0-> f1 ->2
                  //nPattern -= delta-1;
                  do {
                      loop.update(1);
                      loop.process();
                      delta--;
                  } while(delta>1);
               } else {
                  uint32_t d=delta-1;
                  delta=1;
                  do {
                     // pattern with more than 7 bits
                     //  add non-zero high bits in groups of 4 bits
                     int shift=((d>>2)&0x3c)-25;
                     delta += (d&0xf)<<shift;
                     d=nextByte(ptr);
                  } while(d & 0x80);
                  delta += d;
               }
            }
            loop.update(delta);
            loop.process();
         }
      } else if(m_encoding == ENCODING_BIT4) {
         // BIT4 scheme
         for(int nData=m_nData;nData>0;nData--) {
            uint8_t bit4=nextByte(ptr);
            if(bit4<0x80) {
               // 00..0x7f  : skip 4*N+4 patterns
               loop.update((bit4<<2)+4);
            } else if(bit4<=0xf0) {
               // 0x80..0xf0  : set 4*(N-0x7f) patterns
               for(bit4=bit4-0x7f ; bit4;bit4--) {
                  loop.process(); loop.update(1);
                  loop.process(); loop.update(1);
                  loop.process(); loop.update(1);
                  loop.process(); loop.update(1);
               }
            } else {
               // 0xf1..0xff  : set bits
               if(bit4&0x1) loop.process();
               loop.update(1);
               if(bit4&0x2) loop.process();
               loop.update(1);
               if(bit4&0x4) loop.process();
               loop.update(1);
               if(bit4&0x8) loop.process();
               loop.update(1);
            }
         }
      } else if(m_encoding == ENCODING_DELTA2) {
         // DELTA2 scheme, similar DELTA but make better use of
         //  all characters
         // delta scheme: first pattern is processed directly
         loop.process();
         //for(int nPattern=m_NPattern-1;nPattern>0;nPattern--) {
         for(int iLoop=m_loopHelper;iLoop>0;iLoop--) {
            uint32_t delta;
            delta=nextByte(ptr);
            if(delta & 0x80) {
               if(delta<0xc0) {
                  uint32_t up=0;
                  do {
                     up=(up|(delta&0x7f))<<6;
                     delta=nextByte(ptr);
                  } while(delta & 0x80);
                  delta |= (up<<1);
               } else {
                  delta-=0xbf;
                  do {
                      loop.update(1);
                      loop.process();
                      --delta;
                  } while(delta>0);
               }
            }
            loop.update(delta+1);
            loop.process();
         }
      } else if(m_encoding == ENCODING_U32) {
         loop.process();
         for(int nPattern=m_NPattern-1;nPattern>0;nPattern--) {
            uint32_t delta;
            delta=nextByte(ptr);
            delta |= ((uint32_t)nextByte(ptr))<<8;
            delta |= ((uint32_t)nextByte(ptr))<<16;
            delta |= ((uint32_t)nextByte(ptr))<<24;
            loop.update(delta);
            loop.process();
         }
      }
   }
 protected:
   inline uint8_t nextByte(uint8_t const * ATH_RESTRICT &ptr) const {
      return *(ptr++);
   }
   //
   // first pattern ID
   uint32_t m_FirstPattern;
   //
   // number of patterns (unpacked)
   uint32_t m_NPattern;
   //
   // number of bytes
   uint32_t m_nData;
   //
   // compression scheme
   uint32_t m_encoding;
   //
   // helper to speed up loops
   uint32_t m_loopHelper;
   //
   // pointer to data
   uint8_t const * ATH_RESTRICT m_data;
};

class FTK_CompressedPatternListLoopWithCounter_base {
 public:
   inline virtual void init(uint32_t basePattern) { m_pattern=basePattern; }
   inline void update(uint32_t delta) { m_pattern += delta; }
   inline uint32_t getPattern(void) const { return m_pattern; }
 private:
   uint32_t m_pattern;
};

class FTK_CompressedPatternListEncoder_base
: public FTK_CompressedPatternListLoopWithCounter_base {
 public:
   virtual ~FTK_CompressedPatternListEncoder_base(void) { }
   // empty encoder
   // user may implement storeFirst()
   // user has to implement storeNext()
   FTK_CompressedPatternListEncoder_base(std::vector<uint8_t> &data)
      : m_data(data),m_firstPattern(0),m_nPattern(0) {
      m_data.resize(0); }
   virtual void storeFirst(void) { }
   virtual void storeNext(void) =0;
   void process(void) {
      if(m_nPattern==0) {
         m_firstPattern=getPattern();
         storeFirst();
      } else {
         storeNext();
      }
      m_nPattern++;
      m_previousPattern=getPattern();
   }
   uint32_t getFirstPattern(void) const { return m_firstPattern; }
 protected:
   std::vector<uint8_t> &m_data;
   uint32_t m_firstPattern;
   uint32_t m_nPattern;
   uint32_t m_previousPattern;
};

class FTK_CompressedPatternListBIT4Encoder
   : public FTK_CompressedPatternListEncoder_base {
public:
   FTK_CompressedPatternListBIT4Encoder(std::vector<uint8_t> &data)
      : FTK_CompressedPatternListEncoder_base(data) { }
   virtual void storeFirst(void) {
      m_data.push_back((1<<(m_firstPattern & 0x03))|0xf0);
      m_firstPattern &= 0xfffffffc;
   }
   virtual void storeNext(void) {
      uint32_t bit=getPattern()-(m_previousPattern & 0xfffffffc);
      if(bit>3) {
         // insert zeros, then start new byte
         uint32_t nZero4=(bit>>2)-1;
         while(nZero4>0) {
            if(nZero4>0x80) {
               m_data.push_back(0x7f);
               nZero4 -=0x80;
            } else {
               m_data.push_back(nZero4-1);
               nZero4=0;
            }
         }
         m_data.push_back((1<<(bit &3))|0xf0);
      } else {
         // add bit to previous
         uint32_t n1=m_data.size()-1;
         uint8_t data=m_data[n1]|(1<<bit);
         m_data[n1]=data;
         if(data==0xff) {
            // possibly merge with previous
            if(n1>0) {
               uint8_t &prev=m_data[n1-1];
               if((prev>=0x80)&&(prev<0xf0)) {
                  prev++;
                  m_data.resize(n1);
               }
            }
         }
      }
   }
};

class FTK_CompressedPatternListU32Encoder
   : public FTK_CompressedPatternListEncoder_base {
public:
   FTK_CompressedPatternListU32Encoder(std::vector<uint8_t> &data)
      : FTK_CompressedPatternListEncoder_base(data) { }
   virtual void storeNext(void) {
      uint32_t delta=getPattern()-m_previousPattern;
      m_data.push_back(delta);
      m_data.push_back(delta>>8);
      m_data.push_back(delta>>16);
      m_data.push_back(delta>>24);
   }
};

class FTK_CompressedPatternListU7Encoder
   : public FTK_CompressedPatternListEncoder_base {
public:
   FTK_CompressedPatternListU7Encoder(std::vector<uint8_t> &data)
      : FTK_CompressedPatternListEncoder_base(data) { }
   virtual void storeNext(void) {
      uint32_t delta=getPattern()-m_previousPattern-1;
      if(delta & 0xf0000000) m_data.push_back((delta>>28)|0x80);
      if(delta & 0xffe00000) m_data.push_back((delta>>21)|0x80);
      if(delta & 0xffffc000) m_data.push_back((delta>>14)|0x80);
      if(delta & 0xffffff80) m_data.push_back((delta>>7)|0x80);
      m_data.push_back(delta & 0x7f);
   }
};

class FTK_CompressedPatternListDeltaEncoder
   : public FTK_CompressedPatternListEncoder_base {
public:
   FTK_CompressedPatternListDeltaEncoder(std::vector<uint8_t> &data)
      : FTK_CompressedPatternListEncoder_base(data) { }
   virtual void storeNext(void) {
      uint32_t delta=getPattern()-m_previousPattern-1;
      uint32_t mask4=0x780;
      int shift=7;
      // check whether some bits >=7 are nonzero
      //  -> need extra bytes to store this delta
      for(uint32_t mask=0xffffff80;mask;mask<<=4) {
         if((delta & mask)==0) break;
         uint32_t bits=delta & mask4;
         if(bits) m_data.push_back( (bits>>shift)|((shift+25)<<2) );
         mask4 <<=4;
         shift+=4;
      }
      if(!delta) {
         uint8_t prev=0xff;
         uint8_t pprev=0;
         uint32_t n1=m_data.size()-1;
         if(m_data.size()>0) {
            prev=m_data[n1];
            if(n1>0) pprev=m_data[n1-1];
         }
         if((prev==0x00)&&(pprev<0x80)) {
            // repeat 2x
            m_data[n1]=0xf0;
         } else if((prev>=0xF0)&&(prev<0xFF)) {
            // repeat 3+x times
            m_data[n1]=prev+1;
         } else {
            // normal byte
            m_data.push_back(0);
         }
      } else {
         m_data.push_back(delta & 0x7f);
      }
   }
};

class FTK_CompressedPatternListDelta2Encoder
   : public FTK_CompressedPatternListEncoder_base {
public:
   FTK_CompressedPatternListDelta2Encoder(std::vector<uint8_t> &data)
      : FTK_CompressedPatternListEncoder_base(data) { }
   virtual void storeNext(void) {
      uint32_t delta=getPattern()-m_previousPattern-1;
      if(delta==0) {
         int n1=m_data.size()-1;
         uint8_t pprev=1;
         uint8_t prev=1;
         if(n1>=0) {
            prev=m_data[n1];
            if(n1>0) pprev=m_data[n1-1];
         }
         if((prev==0)&&(pprev<0x80)) {
            // merge zero -> c0
            m_data[n1]=0xc0;
         } else if((prev>=0xc0)&&(prev<0xff)) {
            m_data[n1]++; // c0->c1 ... fe->ff
         } else {
            // no packing for this byte
            m_data.push_back(0);
         }
      } else {
         if(delta & 0x80000000) m_data.push_back(((delta>>31)&0x3f)|0x80);
         if(delta & 0xfe000000) m_data.push_back(((delta>>25)&0x3f)|0x80);
         if(delta & 0xfff80000) m_data.push_back(((delta>>19)&0x3f)|0x80);
         if(delta & 0xffffe000) m_data.push_back(((delta>>13)&0x3f)|0x80);
         if(delta & 0xffffff80) m_data.push_back(((delta>>7)&0x3f)|0x80);
         m_data.push_back(delta & 0x7f);
      }
   }
};


#endif

