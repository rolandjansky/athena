/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCondTPCnv/LArMphysOverMcalSubsetCnv_p1.h"

void
LArMphysOverMcalSubsetCnv_p1::persToTrans(const LArMpMcPersType* persObj, 
					  LArMpMcTransType* transObj, 
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
      subsetIt->second[j].m_data=persObj->m_values[index];
      ++index;
    }
  }    

  unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
  LArMpMcTransType::CorrectionVec corrs;
  corrs.resize(ncorrs);
  
  // Loop over corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    corrs[i].first = persObj->m_subset.m_corrChannels[i];
    corrs[i].second.m_data=persObj->m_values[index];
    ++index;
  }
  transObj->insertCorrections (std::move (corrs));

  // Copy the rest
  transObj->setChannel       (persObj->m_subset.m_channel);
  transObj->setGroupingType  (persObj->m_subset.m_groupingType);
  
  return;
}

 
void 
LArMphysOverMcalSubsetCnv_p1::transToPers(const LArMpMcTransType*, 
				    LArMpMcPersType*, 
				    MsgStream &log) {

  log << MSG::ERROR << "LArMphysOverMcalSubsetCnv_p1::transToPers is obsolete" << endmsg;
  return;
}
 
