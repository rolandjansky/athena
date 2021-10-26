/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCondTPCnv/LArDSPThresholdsSubsetCnv_p1.h"

void
LArDSPThresholdsSubsetCnv_p1::persToTrans(const DSPThresholdsPersType* persObj, 
					  DSPThresholdsTransType* transObj, 
					  MsgStream & /*log*/)
{
  transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

  unsigned int nfebids          = persObj->m_subset.m_febIds.size();
  unsigned index                =0;
  const unsigned int nChannelsPerFeb  = persObj->m_subset.subsetSize();
  auto subsetIt = transObj->subsetBegin();
  for (unsigned int i = 0; i < nfebids; ++i, ++subsetIt){
    // Loop over channels in feb 
    for (unsigned int j = 0; j < nChannelsPerFeb; ++j){
      subsetIt->second[j].set(persObj->m_tQThr[index],persObj->m_samplesThr[index],persObj->m_trigSumThr[index]);
      ++index;
    }
  }    

  unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
  DSPThresholdsTransType::CorrectionVec corrs;
  corrs.resize(ncorrs);
  
  // Loop over corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    corrs[i].first = persObj->m_subset.m_corrChannels[i];
    corrs[i].second.set(persObj->m_tQThr[index],persObj->m_samplesThr[index],persObj->m_trigSumThr[index]);
    ++index;
  }
  transObj->insertCorrections (std::move (corrs));

  // Copy the rest
  transObj->setChannel       (persObj->m_subset.m_channel);
  transObj->setGroupingType  (persObj->m_subset.m_groupingType);
  
  return;
}

  
        
    

void 
LArDSPThresholdsSubsetCnv_p1::transToPers(const DSPThresholdsTransType* transObj, 
					DSPThresholdsPersType* persObj, 
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
	  << "LArDSPThresholdsSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
	  << endmsg;
      return;
    }
    if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets
  }

  // Reserve space in vectors
  persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
  persObj->m_subset.m_corrChannels.reserve(ncorrs);
  persObj->m_tQThr.reserve(ncorrs+nsubsetsNotEmpty*nChannelsPerFeb);
  persObj->m_samplesThr.reserve(ncorrs+nsubsetsNotEmpty*nChannelsPerFeb);
  persObj->m_trigSumThr.reserve(ncorrs+nsubsetsNotEmpty*nChannelsPerFeb);
    
   //Copy subsets
  for (auto subsetIt = transObj->subsetBegin();
       subsetIt != subsetEnd;
       ++subsetIt)
  {
    unsigned int nfebChans = subsetIt->second.size();
    if (nfebChans==0) continue;
    unsigned int febid = subsetIt->first;
    persObj->m_subset.m_febIds.push_back(febid);
    for (unsigned int j=0; j<nfebChans;++j) {  //Loop over channels in FEB
      persObj->m_tQThr.push_back(subsetIt->second[j].tQThr());
      persObj->m_samplesThr.push_back(subsetIt->second[j].samplesThr());
      persObj->m_trigSumThr.push_back(subsetIt->second[j].trigSumThr());
    }
  }

  // Copy corrections
  const auto corrEnd = transObj->correctionVecEnd();
  for (auto corrIt = transObj->correctionVecBegin();
       corrIt != corrEnd;
       ++corrIt)
  {
    persObj->m_subset.m_corrChannels.push_back(corrIt->first);
    persObj->m_tQThr.push_back(corrIt->second.tQThr());
    persObj->m_samplesThr.push_back(corrIt->second.samplesThr());
    persObj->m_trigSumThr.push_back(corrIt->second.trigSumThr());
  }

  // Copy the rest
  persObj->m_subset.m_gain          = transObj->gain(); 
  persObj->m_subset.m_channel       = transObj->channel();
  persObj->m_subset.m_groupingType  = transObj->groupingType();
}
