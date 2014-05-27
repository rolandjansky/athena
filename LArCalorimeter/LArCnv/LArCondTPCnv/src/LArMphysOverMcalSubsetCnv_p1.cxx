/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCondTPCnv/LArMphysOverMcalSubsetCnv_p1.h"

void
LArMphysOverMcalSubsetCnv_p1::persToTrans(const LArMpMcPersType* persObj, 
					  LArMpMcTransType* transObj, 
					  MsgStream & /*log*/) {
  // Copy conditions
  unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
  unsigned int nfebids          = persObj->m_subset.m_febIds.size();
  unsigned index                =0;
  // resize subset to with then number of febids
  transObj->m_subset.resize(nfebids);

  for (unsigned int i = 0; i < nfebids; ++i){
    // Set febid
    transObj->m_subset[i].first = persObj->m_subset.m_febIds[i];
    // Fill channels with empty pedestal vectors
    transObj->m_subset[i].second.resize(NCHANNELPERFEB);
    // Loop over channels in feb 
    for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){
      transObj->m_subset[i].second[j].m_data=persObj->m_values[index];
      //      std::cout << "WL Data: FEB=" << std::hex << febid << std::dec << " [" << i << "] Channel=" 
      //			  << j << " Payload=  << 
      ++index;
    }
  }    
   
  transObj->m_correctionVec.reserve(ncorrs);
  
  // Loop over corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
    transObj->m_correctionVec[i].second.m_data=persObj->m_values[index];
    ++index;
  }
  // Copy the rest
  transObj->m_gain          = persObj->m_subset.m_gain; 
  transObj->m_channel       = persObj->m_subset.m_channel;
  transObj->m_groupingType  = persObj->m_subset.m_groupingType;
  
  return;
}

 
void 
LArMphysOverMcalSubsetCnv_p1::transToPers(const LArMpMcTransType*, 
				    LArMpMcPersType*, 
				    MsgStream &log) {

  log << MSG::ERROR << "LArMphysOverMcalSubsetCnv_p1::transToPers is obsolete" << endreq;
  return;
}
 
