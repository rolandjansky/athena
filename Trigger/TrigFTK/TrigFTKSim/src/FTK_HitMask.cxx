#include "TrigFTKSim/FTK_HitMask.h"

#ifdef USE_32BIT_CONTAINER

void FTK_HitMaskVector::init(uint32_t first,uint32_t last,FTK_HitMask data) {
   uint32_t w0=first>>2;
   uint32_t w1=w0+1;
   uint32_t w2=last>>2;
   uint8_t firstWordFirstBit=first &3;
   uint8_t lastWordLastBit=last &3;
   uint8_t firstWordLastBit=3;
   if(w0==w2) {
      // first word=last word
      firstWordLastBit=lastWordLastBit;
   }
   // construct mask and data for bit number zero 
   FTK_HitMask4 mask1=0x11111111;
   FTK_HitMask4 data1=0;
   for(uint8_t bit=0;bit<8;bit++) {
      if(data & (1<<bit)) data1 |= 1<<(bit<<2);
   }
   // construct mask and data for first word
   int bitsFirstWord = (1<<(firstWordLastBit+1))
      -(1<<firstWordFirstBit);
   // example:
   //  firstWordFirstBit=1
   //  firstWordLastBit=3
   // -> bitsFirstWord = (1<<4)-(1<<2) = 0x0e  -> binary 1110
   //   (bits 1,2,3 are set, i.e. in the range [1,3])

   // clear bits which are not in mask1, set bits in data1
   m_data4[w0] = (m_data4[w0]& ~(mask1*bitsFirstWord))|(data1*bitsFirstWord);

   if(w0!=w2) {
      // construct mask for last word
      int bitsLastWord = (1<<(lastWordLastBit+1))-(1<<0);
      m_data4[w2] = (m_data4[w2]& ~(mask1*bitsLastWord))|(data1*bitsLastWord);

      // handle bulk of data -> all bits used
      FTK_HitMask4 dataF=data1*0xf;
      for(uint32_t w=w1;w<w2;w++) m_data4[w]=dataF;
   }
}

#endif
