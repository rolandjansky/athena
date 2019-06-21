/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __H_FTK_COMPRESSEDSECTORPATTERNLIST
#define __H_FTK_COMPRESSEDSECTORPATTERNLIST

#include "TrigFTKSim/FTK_CompressedPatternList.h"
#include <map>
#include <vector>

class FTK_CompressedSectorPatternList
: public std::map<uint32_t,FTK_CompressedPatternList> {
 public:
   FTK_CompressedSectorPatternList(void);
   ~FTK_CompressedSectorPatternList();
   // import patterns for one sector
   void importPatternSet(uint32_t sector,std::set<uint32_t> const &patterns,
                         int encoding);
   // finalize patterns after importing them
   void finalize(int compressionType);
   // query total number of patterns
   uint32_t getNPattern(void) const;
   // query maximum sector number
   inline uint32_t maxSector(void) const { return (*rbegin()).first; }
   // get memory estimate
   uint32_t getMemoryEstimate(void) const;

   // loop over all sectors and patterns
   template<class LOOP> inline void patternLoop(LOOP &loop) const {
      for( std::map<uint32_t,FTK_CompressedPatternList>::const_iterator
              iSector=begin();iSector!=end();iSector++) {
         if(loop.setSector((*iSector).first)) {
            (*iSector).second.patternLoop(loop);
         }
      }
   }
   
   // import data from TTree
   void importData(uint32_t sector,uint32_t firstPattern,
                   uint32_t nPattern,uint32_t encoding,uint32_t nData,
                        Int_t const *intData);

 protected:
   size_t m_total;
   uint8_t *m_compressedData;
   std::map<uint32_t,std::vector<uint8_t> > *m_tempBuffer;
};

#endif
