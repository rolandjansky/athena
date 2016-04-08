/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCondTPCnv/LArSingleFloatSubsetCnv_p1.h"

#include "LArElecCalib/LArCalibErrorCode.h"
#include "CopyFloatWithErrorcode.h"

void
LArSingleFloatSubsetCnv_p1::persToTrans(const SingleFloatPersType* persObj, 
					SingleFloatTransType* transObj, 
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
    // Fill channels with empty vectors
    transObj->m_subset[i].second.resize(NCHANNELPERFEB);
    // Loop over channels in feb 
    for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){
      //transObj->m_subset[i].second[j].m_data=persObj->m_values[index];
      const float& data=persObj->m_values[index];
      if (data<=1.0+LArElecCalib::ERRORCODE) 
	transObj->m_subset[i].second[j].m_data=LArElecCalib::ERRORCODE;
      else
	transObj->m_subset[i].second[j].m_data=data;
	  
      //      std::cout << "WL Data: FEB=" << std::hex << febid << std::dec << " [" << i << "] Channel=" 
      //			  << j << " Payload=  << 
      ++index;
    }
  }    
   
  transObj->m_correctionVec.reserve(ncorrs);
  
  // Loop over corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
    //transObj->m_correctionVec[i].second.m_data=persObj->m_values[index];
    const float& data=persObj->m_values[index];
      if (data<=1.0+LArElecCalib::ERRORCODE) 
	transObj->m_correctionVec[i].second.m_data=LArElecCalib::ERRORCODE;
      else
	transObj->m_correctionVec[i].second.m_data=data;
    ++index;
  }
  // Copy the rest
  transObj->m_gain          = persObj->m_subset.m_gain; 
  transObj->m_channel       = persObj->m_subset.m_channel;
  transObj->m_groupingType  = persObj->m_subset.m_groupingType;
  
  return;
}

  
        
    

void 
LArSingleFloatSubsetCnv_p1::transToPers(const SingleFloatTransType* transObj, 
					SingleFloatPersType* persObj, 
					MsgStream &log) 
{
  // Get the number of channels, corrections and the size of pedestal and pedestalrms
  unsigned int nfebs            = transObj->m_subset.size();
  unsigned int ncorrs           = transObj->m_correctionVec.size();
  unsigned int nsubsetsNotEmpty = 0;
 
  for (unsigned int i = 0; i < nfebs; ++i){
    unsigned int nfebChans = transObj->m_subset[i].second.size();
    if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
      log << MSG::ERROR 
	  << "LArSingleFloatSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
	  << endreq;
      return;
    }
    if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets
  }

  // Reserve space in vectors
  persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
  persObj->m_subset.m_corrChannels.reserve(ncorrs);
  persObj->m_values.reserve(ncorrs+nsubsetsNotEmpty*NCHANNELPERFEB);
    
   //Copy subsets
  for (unsigned int i = 0; i < nfebs; ++i) {//Loop over FEBs
    unsigned int nfebChans = transObj->m_subset[i].second.size();
    if (nfebChans==0) continue;
    unsigned int febid = transObj->m_subset[i].first;
    persObj->m_subset.m_febIds.push_back(febid);
    for (unsigned int j=0; j<nfebChans;++j)  //Loop over channels in FEB
      persObj->m_values.push_back(transObj->m_subset[i].second[j].m_data);
  }

  // Copy corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);
    persObj->m_values.push_back(transObj->m_correctionVec[i].second.m_data);
  }

  // Copy the rest
  persObj->m_subset.m_gain          = transObj->m_gain; 
  persObj->m_subset.m_channel       = transObj->m_channel;
  persObj->m_subset.m_groupingType  = transObj->m_groupingType;
}
