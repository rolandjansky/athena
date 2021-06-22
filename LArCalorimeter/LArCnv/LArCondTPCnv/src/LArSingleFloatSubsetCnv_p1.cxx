/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCondTPCnv/LArSingleFloatSubsetCnv_p1.h"

#include "LArElecCalib/LArCalibErrorCode.h"
#include "CopyFloatWithErrorcode.h"

void
LArSingleFloatSubsetCnv_p1::persToTrans(const SingleFloatPersType* persObj, 
					SingleFloatTransType* transObj, 
					MsgStream & /*log*/)
{
  transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

  unsigned int nfebids          = persObj->m_subset.m_febIds.size();
  const unsigned int nChannelsPerFeb  = persObj->m_subset.subsetSize();
  unsigned index                =0;

  auto subsetIt = transObj->subsetBegin();
  for (unsigned int i = 0; i < nfebids; ++i, ++subsetIt){
    // Loop over channels in feb 
    for (unsigned int j = 0; j < nChannelsPerFeb; ++j){
      const float& data=persObj->m_values[index];
      if (data<=1.0+LArElecCalib::ERRORCODE) 
	subsetIt->second[j].m_data=LArElecCalib::ERRORCODE;
      else
	subsetIt->second[j].m_data=data;
      ++index;
    }
  }    

  unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
  SingleFloatTransType::CorrectionVec corrs;
  corrs.resize(ncorrs);
  
  // Loop over corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    corrs[i].first = persObj->m_subset.m_corrChannels[i];
    const float& data=persObj->m_values[index];
    if (data<=1.0+LArElecCalib::ERRORCODE) 
      corrs[i].second.m_data=LArElecCalib::ERRORCODE;
    else
      corrs[i].second.m_data=data;
    ++index;
  }
  transObj->insertCorrections (std::move (corrs));

  // Copy the rest
  transObj->setChannel       (persObj->m_subset.m_channel);
  transObj->setGroupingType  (persObj->m_subset.m_groupingType);
  
  return;
}

  
        
    

void 
LArSingleFloatSubsetCnv_p1::transToPers(const SingleFloatTransType* transObj, 
					SingleFloatPersType* persObj, 
					MsgStream &log) 
{
  // Get the number of channels, corrections and the size of pedestal and pedestalrms
  unsigned int ncorrs           = transObj->correctionVecSize();
  unsigned int nsubsetsNotEmpty = 0;
  const unsigned int nChannelsPerFeb  = transObj->channelVectorSize();
 
  const auto subsetEnd = transObj->subsetEnd();
  for (auto subsetIt = transObj->subsetBegin();
       subsetIt != subsetEnd;
       ++subsetIt)
  {
    unsigned int nfebChans = subsetIt->second.size();
    if (nfebChans != 0 && nfebChans != nChannelsPerFeb) {
      log << MSG::ERROR 
	  << "LArSingleFloatSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
	  << endmsg;
      return;
    }
    if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets
  }

  // Reserve space in vectors
  persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
  persObj->m_subset.m_corrChannels.reserve(ncorrs);
  persObj->m_values.reserve(ncorrs+nsubsetsNotEmpty*nChannelsPerFeb);
    
   //Copy subsets
  for (auto subsetIt = transObj->subsetBegin();
       subsetIt != subsetEnd;
       ++subsetIt)
  {
    unsigned int nfebChans = subsetIt->second.size();
    if (nfebChans==0) continue;
    unsigned int febid = subsetIt->first;
    persObj->m_subset.m_febIds.push_back(febid);
    for (unsigned int j=0; j<nfebChans;++j)  //Loop over channels in FEB
      persObj->m_values.push_back(subsetIt->second[j].m_data);
  }

  // Copy corrections
  const auto corrEnd = transObj->correctionVecEnd();
  for (auto corrIt = transObj->correctionVecBegin();
       corrIt != corrEnd;
       ++corrIt)
  {
    persObj->m_subset.m_corrChannels.push_back(corrIt->first);
    persObj->m_values.push_back(corrIt->second.m_data);
  }

  // Copy the rest
  persObj->m_subset.m_gain          = transObj->gain(); 
  persObj->m_subset.m_channel       = transObj->channel();
  persObj->m_subset.m_groupingType  = transObj->groupingType();
}
