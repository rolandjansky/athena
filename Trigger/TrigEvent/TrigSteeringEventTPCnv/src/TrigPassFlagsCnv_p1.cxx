/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigPassFlags.h"
#include <cmath>
#include <iostream>
#include "TrigSteeringEventTPCnv/TrigPassFlags_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCnv_p1.h"
#include <stdint.h>

void TrigPassFlagsCnv_p1::persToTrans(const TrigPassFlags_p1* persObj, 
                                      TrigPassFlags* transObj, 
                                      MsgStream &log)
{
   
   if(log.level()<=MSG::DEBUG)
      log << MSG::DEBUG << "TrigPassFlagsCnv_p1::persToTrans called " << endreq;
   
   // reserve space in vector<vector<bool> > and set defaults to false
   transObj->m_flagsPerObject.clear();
   transObj->m_flagsPerObject.resize(persObj->m_nObjects, 
                                     std::vector<bool>(persObj->m_nFlags,false)); 
   
   const unsigned int bitsPerWord = sizeof(uint32_t)*8;
   const unsigned int flagsPerObject = persObj->m_nFlags;
   
   // loop over words and fill object/flags of transient
   unsigned int object = 0;
   unsigned int flag = 0;
   for(unsigned int word = 0; word < persObj->m_serialized.size();++word){
      
      // loop over bits in the word
      for(unsigned int bit = 0;bit < bitsPerWord;++bit){
         
         if(persObj->m_serialized[word] & (1 << bit) )
            transObj->m_flagsPerObject[object][flag] = true;
         
         flag++;
         
         // reset flag index and increment object index if max flags reached 
         if(flag >= flagsPerObject){
            flag = 0;
            object++;
         }
         
         
      }// end for(bit)
      
      
   }// end for(word)

   if(log.level()<=MSG::DEBUG) {
      for(unsigned int i=0; i<transObj->size(); i++) {
         int flag = HLT::FlagAs<int>(transObj->getFlag(i));
         std::cout << "   " << flag << "  [0x" << std::hex << flag << "]" << std::dec << std::endl;
      }
   }
}


void TrigPassFlagsCnv_p1::transToPers(const TrigPassFlags* transObj,
                                      TrigPassFlags_p1* persObj,
                                      MsgStream &log)
{
   if(log.level()<=MSG::DEBUG) {
      log << MSG::DEBUG << "TrigPassFlagsCnv_p1::transToPers called " << endreq;
      for(unsigned int i=0; i<transObj->size(); i++) {
         int flag = HLT::FlagAs<int>(transObj->getFlag(i));
         log << "   " << flag << "  [0x" << std::hex << flag << "]" << std::dec << std::endl;
      }
   }

   // record the number of container objects
   persObj->m_nObjects = transObj->m_flagsPerObject.size();
   // record the number of flags per object (should be the same for all so get it from first entry)
   persObj->m_nFlags = 0;
   if(persObj->m_nObjects > 0){
      persObj->m_nFlags = transObj->m_flagsPerObject[0].size();
      if(persObj->m_nFlags == 0){
         log << MSG::WARNING << "TrigPassFlagsCnv_p1::transToPers zero flags to save, this is an empty TrigPassFlags" << endreq;
         persObj->m_serialized.clear();
         return;
      }
   }
   else{
      log << MSG::WARNING << "TrigPassFlagsCnv_p1::transToPers zero objects to save, this is an empty TrigPassFlags" << endreq;
      persObj->m_serialized.clear();
      return;
   }
   
   
   
   
   const unsigned int bitsPerWord = sizeof(uint32_t)*8;
   const unsigned int word_count = (persObj->m_nFlags * persObj->m_nObjects) / bitsPerWord + 1;
   
   // reserve space in vector<uint32_t> to 0
   persObj->m_serialized.clear();
   persObj->m_serialized.resize(word_count, 0); // reserve the space and initialize to zero
   
   // loop over objects in transient object
   unsigned int word = 0;
   unsigned int bit = 0;
   for(unsigned int object = 0;object < transObj->m_flagsPerObject.size();++object){
      
      // loop over the object's flags
      for(unsigned int flag = 0;flag < transObj->m_flagsPerObject[object].size();++flag){
         
         // if the flag is true, set the bit in the vector of words
         if(transObj->m_flagsPerObject[object][flag])
            persObj->m_serialized[word] |= (1 << bit);
         
         bit++;
         
         // reset bit index and increment word index if we reach end of word
         if(bit >= bitsPerWord){
            bit = 0;
            word++;
         }
         
      }// end for(flag)
      
      
   }// end for(object)
}
