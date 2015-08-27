/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetEventTPCnv/JetMomentMapCnv_p6.h"

#include "JetEvent/JetCollection.h"

void
JetMomentMapCnv_p6::transToPers(const JetMomentMap* transObj,
				JetMomentMap_p6* persObj,
				MsgStream &reporter )
{
  reporter << MSG::DEBUG << "JetMomentMapConverterBase toPers() Template _p6" << endreq;
  
  persObj->m_momentNum.clear();
  size_t njet = transObj->m_store.size();
  persObj->m_momentNum.resize(njet);
  persObj->m_jetIds.resize(njet);
  persObj->m_moments.reserve(njet*10); // just a guess 
  persObj->m_momentStatus.reserve(njet*10); // just a guess 


  JetMomentMap::map_t::iterator mapIt = transObj->m_store.begin();
  JetMomentMap::map_t::iterator mapItE = transObj->m_store.end();
  size_t i=0;
  for(; mapIt != mapItE; ++mapIt){
    // check validity of vectors
    if( ! (*mapIt).second.isValid() ){ // in principle this should not happen
      persObj->m_momentNum[i] = 0;
      persObj->m_jetIds[i] = (*mapIt).first;
      continue ; // don't save anything.
    }

    JetMomentMap::record_t & moments = *(*mapIt).second.get();
    size_t jetId= (*mapIt).first;

    size_t momNum = moments.size();
    if ( momNum >= JetMomentMap_p6::s_maxNumMoment ){  
      reporter << MSG::ERROR << " Attempt to save  "<< momNum <<" jet moments failed. Limiting this number to "<< JetMomentMap_p6::s_maxNumMoment-1<< endreq;
      momNum = JetMomentMap_p6::s_maxNumMoment -1;
    }
    persObj->m_momentNum[i] = momNum;
    persObj->m_jetIds[i] = jetId;
    for( size_t j=0; j<momNum;j++){
      bool used = moments[j].tag();
      if(used) persObj->m_moments.push_back(moments[j].moment() );
      persObj->m_momentStatus.push_back(used);
    }
    i++;

  }
  reporter << MSG::DEBUG << "JetMomentMapConverterBase toPers() Template _p6 Ending" << endreq;
  
}

void
JetMomentMapCnv_p6::persToTrans(const JetMomentMap_p6* persObj,
				JetMomentMap* transObj,
				MsgStream &reporter )
{
  reporter << MSG::DEBUG << "JetMomentMapConverterBase toTrans() Template _p6" << endreq;

  transObj->clear();
  size_t njet = persObj->m_momentNum.size() ;
  size_t offset = 0;
  size_t valueOffset = 0;

  for( size_t i=0; i<njet;i++){
    size_t momNum = persObj->m_momentNum[i];
    
    // create a new record, assign it to the map
    JetMomentMap::record_t * moments_ptr = new JetMomentMap::record_t(momNum);
    JetMomentMap::record_t & moments = *moments_ptr;
    transObj->m_store[ persObj->m_jetIds[i] ] = JetMomentMap::record_ptr_t(moments_ptr);

    // fill the records from pers object
    for( size_t j=0; j<momNum;j++){ 
      if( persObj->m_momentStatus[offset+j] ){
        moments[j] = JetMoment::MomentData(persObj->m_moments[valueOffset], true) ;
        valueOffset++;
      }else{
        moments[j] = JetMoment::MomentData(0, false) ;
      }
      
    }
    
    offset += momNum;
  }
}
