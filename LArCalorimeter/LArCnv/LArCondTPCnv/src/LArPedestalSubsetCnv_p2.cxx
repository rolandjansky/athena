/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArConditionsSubset.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p2.h"
#include "CopyFloatWithErrorcode.h"

void
LArPedestalSubsetCnv_p2::persToTrans(const LArPedestalSubset_p2* persObj, 
				     LArPedestalTransType* transObj, 
				     MsgStream & log)
{
    // Copy basic metadata
    transObj->setChannel       (persObj->m_subset.m_channel);
    transObj->setGroupingType  (persObj->m_subset.m_groupingType);

    transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

    // Copy conditions
    const unsigned int nfebids          = persObj->m_subset.m_febIds.size();
    const unsigned int nChannelsPerFeb  = persObj->m_subset.subsetSize();
    unsigned int pedestalIndex    = 0;
    // Loop over febs
    auto subsetIt = transObj->subsetBegin();
    for (unsigned int i = 0; i < nfebids; ++i, ++subsetIt){
        // Loop over channels in feb
        for (unsigned int j = 0; j < nChannelsPerFeb; ++j){
	  //Copy Pedestal
	  subsetIt->second[j].m_Pedestal=copyFloatPT(persObj->m_vPedestal[pedestalIndex]);
	  //Copy RMS
	  subsetIt->second[j].m_PedestalRMS=copyFloatPT(persObj->m_vPedestalRMS[pedestalIndex]);
	  ++pedestalIndex;
	}//end loop over channel in feb
    }//end loop over febs

    // Copy corrections    
    unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
    LArPedestalTransType::CorrectionVec corrs;
    if (ncorrs) {
      // corrs exist - resize vector
      corrs.resize(ncorrs);
      // Loop over corrections
      for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
	if (pedestalIndex    >= persObj->m_vPedestal.size()) {
	  log << MSG::ERROR 
	      << "LArPedestalSubsetCnv_p2::persToTrans - pedestal index too large: pedestal/size " 
	      << pedestalIndex << " " << persObj->m_vPedestal.size() << endmsg;
	  return;
	}

	corrs[i].first = persObj->m_subset.m_corrChannels[i];
	//Copy Pedestal
	corrs[i].second.m_Pedestal=copyFloatPT(persObj->m_vPedestal[pedestalIndex]);
	//Copy RMS
	corrs[i].second.m_PedestalRMS=copyFloatPT(persObj->m_vPedestalRMS[pedestalIndex]);
	++pedestalIndex;
      }//end loop over corrections
    }//end if ncorrs
    transObj->insertCorrections (std::move (corrs));

}


        
    
void
LArPedestalSubsetCnv_p2::transToPers(const LArPedestalTransType* transObj, 
				     LArPedestalSubset_p2* persObj, 
				     MsgStream &log) 
{
    // Copy conditions
    // Get the number of channels, corrections and the size of pedestal and pedestalrms
    const unsigned int ncorrs           = transObj->correctionVecSize();
    unsigned int nsubsetsNotEmpty = 0;
    const unsigned int nChannelsPerFeb  = transObj->channelVectorSize();
    // Find the number of pedestals/pedestalrmss and check for sparse
    // conditions, e.g. MC conditions
    const auto subsetEnd = transObj->subsetEnd();
    for (auto subsetIt = transObj->subsetBegin();
         subsetIt != subsetEnd;
         ++subsetIt)
    {
        unsigned int nfebChans = subsetIt->second.size();

        if (nfebChans != 0 && nfebChans != nChannelsPerFeb) {
            log << MSG::ERROR 
                << "LArPedestalSubsetCnv_p2::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endmsg;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets
    }
    // Reserve space in vectors
    persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
    persObj->m_subset.m_corrChannels.reserve(ncorrs);
    persObj->m_vPedestal.reserve(ncorrs+nsubsetsNotEmpty*nChannelsPerFeb);
    persObj->m_vPedestalRMS.reserve(ncorrs+nsubsetsNotEmpty*nChannelsPerFeb);

    // Copy conditions in subset
    for (auto subsetIt = transObj->subsetBegin();
         subsetIt != subsetEnd;
         ++subsetIt)
    {
        unsigned int nfebChans = subsetIt->second.size();
        // skip subsets without any channels
        if (nfebChans == 0) continue;
        
        unsigned int febid = subsetIt->first;
        persObj->m_subset.m_febIds.push_back(febid);
	for (unsigned int j=0; j<nfebChans;++j) {  //Loop over channels in FEB
	  persObj->m_vPedestal.push_back(subsetIt->second[j].m_Pedestal);
	  persObj->m_vPedestalRMS.push_back(subsetIt->second[j].m_PedestalRMS);
	}//End loop over channels in feb
    }//end loop over febs

    // Copy corrections
    const auto corrEnd = transObj->correctionVecEnd();
    for (auto corrIt = transObj->correctionVecBegin();
         corrIt != corrEnd;
         ++corrIt)
    {
        // Save channel id in febid vector
        persObj->m_subset.m_corrChannels.push_back(corrIt->first);
	persObj->m_vPedestal.push_back(corrIt->second.m_Pedestal);
	persObj->m_vPedestalRMS.push_back(corrIt->second.m_PedestalRMS);
    }//End loop over corrections

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->gain(); 
    persObj->m_subset.m_channel       = transObj->channel();
    persObj->m_subset.m_groupingType  = transObj->groupingType();
}
