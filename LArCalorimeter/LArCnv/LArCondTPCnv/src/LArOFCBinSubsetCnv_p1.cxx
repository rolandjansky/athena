/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCondTPCnv/LArOFCBinSubsetCnv_p1.h"

void
LArOFCBinSubsetCnv_p1::persToTrans(const OFCBinPersType* persObj, 
				   OFCBinTransType* transObj, 
				   MsgStream & /*log*/)
{
  transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

  // Copy conditions
  unsigned int nfebids          = persObj->m_subset.m_febIds.size();
  const unsigned int nChannelsPerFeb  = persObj->m_subset.subsetSize();
  unsigned index                =0;

  auto subsetIt = transObj->subsetBegin();
  for (unsigned int i = 0; i < nfebids; ++i, ++subsetIt){
    // Loop over channels in feb 
    for (unsigned int j = 0; j < nChannelsPerFeb; ++j){
      (*subsetIt).second[j].m_bin=persObj->m_values[index];
      //      std::cout << "WL Data: FEB=" << std::hex << febid << std::dec << " [" << i << "] Channel=" 
      //			  << j << " Payload=  << 
      ++index;
    }
  }    

  unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
  OFCBinTransType::CorrectionVec corrs;
  corrs.resize(ncorrs);
  
  // Loop over corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    corrs[i].first = persObj->m_subset.m_corrChannels[i];
    corrs[i].second.m_bin=persObj->m_values[index];
    ++index;
  }
  transObj->insertCorrections (std::move (corrs));

  // Copy the rest
  transObj->setChannel       (persObj->m_subset.m_channel);
  transObj->setGroupingType  (persObj->m_subset.m_groupingType);
}

  
        
    

void 
LArOFCBinSubsetCnv_p1::transToPers(const OFCBinTransType* transObj, 
				   OFCBinPersType* persObj, 
				   MsgStream &log) 
{
  // Get the number of channels, corrections and the size of pedestal and pedestalrms
  unsigned int ncorrs           = transObj->correctionVecSize();
  const unsigned int nChannelsPerFeb  = transObj->channelVectorSize();
  unsigned int nsubsetsNotEmpty = 0;
 
  const auto subsetEnd = transObj->subsetEnd();
  for (auto subsetIt = transObj->subsetBegin();
       subsetIt != subsetEnd;
       ++subsetIt)
  {
    unsigned int nfebChans = (*subsetIt).second.size();
    if (nfebChans != 0 && nfebChans != nChannelsPerFeb) {
      log << MSG::ERROR 
	  << "LArOFCBinSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
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
    unsigned int nfebChans = (*subsetIt).second.size();
    if (nfebChans==0) continue;
    unsigned int febid = (*subsetIt).first;
    persObj->m_subset.m_febIds.push_back(febid);
    for (unsigned int j=0; j<nfebChans;++j)  //Loop over channels in FEB
      persObj->m_values.push_back((*subsetIt).second[j].m_bin);
  }

  // Copy corrections
  const auto corrEnd = transObj->correctionVecEnd();
  for (auto corrIt = transObj->correctionVecBegin();
       corrIt != corrEnd;
       ++corrIt)
  {
    persObj->m_subset.m_corrChannels.push_back(corrIt->first);
    persObj->m_values.push_back(corrIt->second.m_bin);
  }

  // Copy the rest
  persObj->m_subset.m_gain          = transObj->gain(); 
  persObj->m_subset.m_channel       = transObj->channel();
  persObj->m_subset.m_groupingType  = transObj->groupingType();
}
