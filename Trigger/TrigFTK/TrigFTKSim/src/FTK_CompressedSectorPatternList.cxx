#include "TrigFTKSim/FTK_CompressedSectorPatternList.h"
#include <iostream>
#include <iomanip>

// #define COMPRESSION_CHECK

FTK_CompressedSectorPatternList::FTK_CompressedSectorPatternList(void) {
   m_tempBuffer=0;
   m_compressedData=0;
   m_total=0;
}

FTK_CompressedSectorPatternList::~FTK_CompressedSectorPatternList() {
   if(m_tempBuffer) delete m_tempBuffer;
   if(m_compressedData) delete [] m_compressedData;
}

uint32_t FTK_CompressedSectorPatternList::getNPattern(void) const {
   uint32_t r=0;
   for(const_iterator iSector=begin();iSector!=end();iSector++) {
      r += (*iSector).second.getNPattern();
   }
   return r;
}

uint32_t FTK_CompressedSectorPatternList::getMemoryEstimate(void) const {
   uint32_t r=sizeof(*this)+sizeof(uint8_t)*m_total+
      size()*sizeof(FTK_CompressedPatternList);
   if(m_tempBuffer) {
      for(std::map<uint32_t,std::vector<uint8_t> >::const_iterator i=
             m_tempBuffer->begin();i!=m_tempBuffer->end();i++) {
         r += (*i).second.size();
      }
   }
   return r;
}

void FTK_CompressedSectorPatternList::importData
(uint32_t sector,uint32_t firstPattern,uint32_t nPattern,uint32_t encoding,
 uint32_t nData,Int_t const *intData) {
   if(!m_tempBuffer) {
      m_tempBuffer=new std::map<uint32_t,std::vector<uint8_t> >;
   }
   std::vector<uint8_t> &data8((*m_tempBuffer)[sector]);
   data8.reserve(nData);
   for(size_t i=0;i<nData;i++) {
      data8.push_back(intData[i]);
   }
   insert(std::make_pair(sector,FTK_CompressedPatternList
                         (firstPattern,nPattern,nData,encoding,data8.data())));
}

void FTK_CompressedSectorPatternList::importPatternSet
(uint32_t sector,std::set<uint32_t> const &patterns,int encoding) {
   if(!m_tempBuffer) {
      m_tempBuffer=new std::map<uint32_t,std::vector<uint8_t> >;
   }
   if((encoding<FTK_CompressedPatternList::ENCODING_MIN)||
      (encoding>=FTK_CompressedPatternList::ENCODING_MAX)) {
      encoding=FTK_CompressedPatternList::ENCODING_DELTA;
   }
   uint32_t nPattern=patterns.size();
   std::vector<uint8_t> &data((*m_tempBuffer)[sector]);
   data.reserve(4*nPattern);
   FTK_CompressedPatternListEncoder_base *encoder=0;
   if(encoding==FTK_CompressedPatternList::ENCODING_BIT4) {
      encoder=new FTK_CompressedPatternListBIT4Encoder(data);
   } else if(encoding==FTK_CompressedPatternList::ENCODING_DELTA2) {
      encoder=new FTK_CompressedPatternListDelta2Encoder(data);
   } else if(encoding==FTK_CompressedPatternList::ENCODING_U32) {
      encoder=new FTK_CompressedPatternListU32Encoder(data);
   } else if(encoding==FTK_CompressedPatternList::ENCODING_U7) {
      encoder=new FTK_CompressedPatternListU7Encoder(data);
   } else {
      encoder=new FTK_CompressedPatternListDeltaEncoder(data);
   }
   std::set<uint32_t>::const_iterator i=patterns.begin(),i0;
   encoder->init(*i);
   encoder->process();
   for(i0=i++;i!=patterns.end();i0=i++) {
      encoder->update((*i)-(*i0));
      encoder->process();
   }
   data.shrink_to_fit();
   insert(std::make_pair(sector,FTK_CompressedPatternList
                         (encoder->getFirstPattern(),nPattern,data.size(),
                          encoding,data.data()))); 
   delete encoder;
}


void FTK_CompressedSectorPatternList::finalize(int compressionMode) {
   if(m_tempBuffer) {
      if(m_compressedData) {
         // should never happen
         std::cout<<"FTK_CompressedSectorPatternList packed and non-packed data"
            " are mixed\n";
         exit(12);
      }
      // enforce a specific compression scheme
      uint32_t maxSize=0;
      for(iterator plist=begin();plist!=end();plist++) {
         if((*plist).second.getEncoding()!=(uint32_t)compressionMode) {
            if((*plist).second.getNPattern()>maxSize) {
               maxSize=(*plist).second.getNPattern();
            }
         }
      }
      std::vector<uint8_t> tmpData;
      tmpData.reserve(maxSize);
#ifdef COMPRESSION_CHECK
      std::vector<uint8_t> test1;
      std::vector<uint8_t> test2;
      std::vector<uint8_t> orig;
      test1.reserve(maxSize);
      test2.reserve(maxSize);
      orig.reserve(maxSize);
#endif
      for(iterator plist=begin();plist!=end();plist++) {
         uint32_t compressionType=(*plist).second.selectEncoding
            (compressionMode);
         if((*plist).second.getEncoding()!=(uint32_t)compressionType) {
            uint32_t firstPattern;
#ifdef COMPRESSION_CHECK
            uint32_t first1,first2;
            (*plist).second.encode(FTK_CompressedPatternList::ENCODING_U32,
                                   &first1,test1);
#endif
            (*plist).second.encode(compressionType,&firstPattern,tmpData);
            std::map<uint32_t,std::vector<uint8_t> >::iterator iBuffer=
               m_tempBuffer->find((*plist).first);
            if(iBuffer!=m_tempBuffer->end()) {
               size_t n=tmpData.size();
#ifdef COMPRESSION_CHECK
               orig.swap((*iBuffer).second);
#endif
               (*iBuffer).second.reserve(n);
               (*iBuffer).second.resize(n);
               if((*iBuffer).second.size()) {
                  memcpy((*iBuffer).second.data(),tmpData.data(),n);
               }
               (*plist).second.setData
                  (firstPattern,compressionType,(*iBuffer).second);
            }
#ifdef COMPRESSION_CHECK
            (*plist).second.encode(FTK_CompressedPatternList::ENCODING_U32,
                                   &first2,test2);
            uint32_t error=0;
            if(test1.size()!=test2.size()) {
               error++;
            }
            if(first1!=first2) {
               error++;
            }
            for(size_t k=0;(k<test1.size())&&(k<test2.size());k++) {
               if(test1[k]!=test2[k]) error++;
            }
            if(error) {
               std::cout<<"first: "<<first1<<" "<<first2<<"\n";
               std::cout<<"size: "<<test1.size()<<" "<<test2.size()<<"\n";
               for(size_t k=0;(k<test1.size())||(k<test2.size());k+=4) {
                  std::cout<<" "<<k;
                  uint32_t x1=0,x2=0;
                  int s=0;
                  for(size_t l=0;l<3;l++) {
                     if(k+l<test1.size()) x1 |= ((uint32_t)test1[k+l])<<s;
                     if(k+l<test2.size()) x2 |= ((uint32_t)test2[k+l])<<s;
                     s+=8;
                  }
                  std::cout<<" "<<x1<<" "<<x2
                           <<std::setbase(16)<<" 0x"<<x1<<" 0x"<<x2
                           <<std::setbase(10)<<"\n";
               }
               std::cout<<"original encoded data\n";
               for(size_t k=0;k<orig.size();k++) {
                  std::cout<<std::setbase(16)<<" 0x"<<(int)orig[k];
               }
               std::cout<<"\n";
               std::cout<<"newly encoded data\n";
               for(size_t k=0;k<tmpData.size();k++) {
                  std::cout<<std::setbase(16)<<" 0x"<<(int)tmpData[k];
               }
               std::cout<<"\n";
               exit(111);
            }
#endif
         }
      }

      // calculate total size
      size_t total=0;
      for(std::map<uint32_t,std::vector<uint8_t> >::const_iterator
             iSector=m_tempBuffer->begin();iSector!=m_tempBuffer->end();
          iSector++) {
         total += (*iSector).second.size();
      }
      // reserve continuous memory block
      m_compressedData=new uint8_t[total];
      m_total=total;
      uint8_t *ptr=m_compressedData;
      // copy data to continuous memory block
      for(std::map<uint32_t,std::vector<uint8_t> >::const_iterator
             iSector=m_tempBuffer->begin();iSector!=m_tempBuffer->end();
          iSector++) {
         if((*iSector).second.size()==0) continue;
         iterator plistPtr=find((*iSector).first);
         if(plistPtr!=end()) {
            (*plistPtr).second.setDataPointer(ptr);
            memcpy(ptr,(*iSector).second.data(),(*iSector).second.size());
            ptr += (*iSector).second.size();
         }
      }
      // delete temporary space
      delete m_tempBuffer;
      m_tempBuffer=0;
   }
}
