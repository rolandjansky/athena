#include "TrigFTKSim/FTK_CompressedPatternList.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>

std::pair<uint32_t,uint32_t> FTK_CompressedPatternList::getMinMaxPattern(void)
   const {
   class GetMinMax : public FTK_CompressedPatternListLoopWithCounter_base {
   public:
      GetMinMax(void) { minMaxPattern.first=1; minMaxPattern.second=0; }
      void process(void) {
         if(minMaxPattern.first>minMaxPattern.second) {
            minMaxPattern.first=getPattern();
            minMaxPattern.second=getPattern();
         } else {
            minMaxPattern.second=getPattern(); 
         }
      }
   std::pair<uint32_t,uint32_t> minMaxPattern;
   } getMinMax;
   patternLoop(getMinMax);
   return getMinMax.minMaxPattern;
}

void FTK_CompressedPatternList::setData(uint32_t firstPattern,uint32_t encoding,
                                        uint8_t const *dataPtr,uint32_t nData) {
   m_FirstPattern=firstPattern;
   m_encoding=encoding;
   m_nData=nData;
   m_loopHelper=0;
   setDataPointer(dataPtr);
   if(m_encoding==ENCODING_DELTA) {
      // initialize loop helper, a trick to optimize the pattern loop
      // (the loop is optimized by determining the (fixed) number of loops)
      for(uint32_t i=0;i<m_nData;i++) {
         if((m_data[i]<0x80)||(m_data[i]>=0xf0)) m_loopHelper++;
      }
   } else if(m_encoding==ENCODING_DELTA2) {
      // initialize loop helper, a trick to optimize the pattern loop
      // (the loop is optimized by determining the (fixed) number of loops)
      for(uint32_t i=0;i<m_nData;i++) {
         if((m_data[i]<0x80)||(m_data[i]>=0xc0)) m_loopHelper++;
      }
   } else if(m_encoding==ENCODING_U7) {
      // initialize loop helper
      // (the loop is optimized by splitting off a first look which goes
      //  without if statement)
      m_loopHelper=m_nData;
      for(uint32_t i=0;i<m_nData;i++) {
         if(m_data[i]>0x80) {
            m_loopHelper=i;
            break;
         }
      }
   }
}

void FTK_CompressedPatternList::exportData
(Int_t *firstPattern,Int_t *nPattern,Int_t *encoding,
 std::vector<Int_t> &deltaData) const {
   *firstPattern=m_FirstPattern;
   *nPattern=m_NPattern;
   *encoding=m_encoding;
   deltaData.reserve(m_nData);
   deltaData.resize(0);
   for(size_t i=0;i<m_nData;i++) {
      deltaData.push_back(m_data[i]);
   }
}

uint32_t FTK_CompressedPatternList::selectEncoding(uint32_t defaultMode) const {
   uint32_t r=defaultMode;
   if((r<ENCODING_MIN)||(r>ENCODING_MAX)) {
      r=m_encoding;
      // analyze delta data
      // quick analysis of multiplicity data
      if(m_encoding==ENCODING_DELTA) {
         int n[4];
         int nLong=0;
         n[0]=1;
         for(size_t i=1;i<4;i++) n[i]=0;
         for(uint32_t i=0;i<m_nData;i++) {
            uint8_t d=m_data[i];
            if(d<0x80) n[0]++;
            else if(d>=0xf0) {
               d-=0xee;
               if(d>3) {
                  nLong+=d;
               }
               while(d>3) {
                  d-=4;
                  n[3]++;
               }
               if(d) n[d-1]++;
            }
         }
         uint32_t sum=n[0]+n[1]+n[2]+n[3];
         double multiplicity=(n[0]+2*n[1]+3*n[2]+4*n[3])/(double)sum;
         if(multiplicity>=2) {
            // BIT4 is expected to be good (?) if multiplicity is very high
            r=ENCODING_BIT4;
         } else if(nLong<n[0]) {
            // U7 is expected to be good if
            // long sequences are suppressed compared to single patterns
            r=ENCODING_U7;
         } else {
            // DELTA
            // fair compromize of everyting
         }
      }
   }
   // few patterns only -> use U7
   if((getNPattern()<=10)&&(r==ENCODING_BIT4)) {
      r=ENCODING_U7;
   }
   return r;
}

void FTK_CompressedPatternList::encode
(uint32_t encoding,uint32_t *firstPattern,std::vector<uint8_t> &data) const {
   // loop over all patterns

   if(encoding==ENCODING_BIT4) {
      FTK_CompressedPatternListBIT4Encoder encoder(data);
      patternLoop(encoder);
      *firstPattern=encoder.getFirstPattern();
   } else if(encoding==ENCODING_DELTA2) {
      FTK_CompressedPatternListDelta2Encoder encoder(data);
      patternLoop(encoder);
      *firstPattern=encoder.getFirstPattern();
   } else if(encoding==ENCODING_U32) {
      FTK_CompressedPatternListU32Encoder encoder(data);
      patternLoop(encoder);
      *firstPattern=encoder.getFirstPattern();
   } else if(encoding==ENCODING_U7) {
      FTK_CompressedPatternListU7Encoder encoder(data);
      patternLoop(encoder);
      *firstPattern=encoder.getFirstPattern();
   } else {
      FTK_CompressedPatternListDeltaEncoder encoder(data);
      patternLoop(encoder);
      *firstPattern=encoder.getFirstPattern();
   }
}
