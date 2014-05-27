/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArPedestalSubsetCnv_p2.h"
#include "CopyFloatWithErrorcode.h"

void
LArPedestalSubsetCnv_p2::persToTrans(const LArPedestalSubset_p2* persObj, 
				     LArPedestalTransType* transObj, 
				     MsgStream & log)
{
    // Copy conditions
    unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
    unsigned int nfebids          = persObj->m_subset.m_febIds.size();
    unsigned int pedestalIndex    = 0;
    
    // resize subset to with then number of febids
    transObj->m_subset.resize(nfebids);

    // Loop over febs
    for (unsigned int i = 0; i < nfebids; ++i){
        // Set febid
        transObj->m_subset[i].first = persObj->m_subset.m_febIds[i];
        // Fill channels with empty pedestal vectors
        transObj->m_subset[i].second.resize(NCHANNELPERFEB);
            
        // Loop over channels in feb
        for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){
	  //Copy Pedestal
	  transObj->m_subset[i].second[j].m_Pedestal=copyFloatPT(persObj->m_vPedestal[pedestalIndex]);
	  //Copy RMS
	  transObj->m_subset[i].second[j].m_PedestalRMS=copyFloatPT(persObj->m_vPedestalRMS[pedestalIndex]);
	  ++pedestalIndex;
	}//end loop over channel in feb
    }//end loop over febs

    // Copy corrections    
    if (ncorrs) {
      // corrs exist - resize vector
      transObj->m_correctionVec.resize(ncorrs);
      // Loop over corrections
      for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
	if (pedestalIndex    >= persObj->m_vPedestal.size()) {
	  log << MSG::ERROR 
	      << "LArPedestalSubsetCnv_p2::persToTrans - pedestal index too large: pedestal/size " 
	      << pedestalIndex << " " << persObj->m_vPedestal.size() << endreq;
	  return;
	}

	transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
	//Copy Pedestal
	transObj->m_correctionVec[i].second.m_Pedestal=copyFloatPT(persObj->m_vPedestal[pedestalIndex]);
	//Copy RMS
	transObj->m_correctionVec[i].second.m_PedestalRMS=copyFloatPT(persObj->m_vPedestalRMS[pedestalIndex]);
	++pedestalIndex;
      }//end loop over corrections
    }//end if ncorrs
    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType; 
}


        
    
void
LArPedestalSubsetCnv_p2::transToPers(const LArPedestalTransType* transObj, 
				     LArPedestalSubset_p2* persObj, 
				     MsgStream &log) 
{
    
    // Copy conditions
    // Get the number of channels, corrections and the size of pedestal and pedestalrms
    unsigned int nfebs            = transObj->m_subset.size();
    unsigned int ncorrs           = transObj->m_correctionVec.size();
    unsigned int nsubsetsNotEmpty = 0;

    // Find the number of pedestals/pedestalrmss and check for sparse
    // conditions, e.g. MC conditions
    for (unsigned int i = 0; i < nfebs; ++i){
        unsigned int nfebChans = transObj->m_subset[i].second.size();

        if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
            log << MSG::ERROR 
                << "LArPedestalSubsetCnv_p2::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endreq;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets
    }
    // Reserve space in vectors
    persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
    persObj->m_subset.m_corrChannels.reserve(ncorrs);
    persObj->m_vPedestal.reserve(ncorrs+nsubsetsNotEmpty*NCHANNELPERFEB);
    persObj->m_vPedestalRMS.reserve(ncorrs+nsubsetsNotEmpty*NCHANNELPERFEB);
	
    // Copy conditions in subset
    for (unsigned int i = 0; i < nfebs; ++i){
        unsigned int nfebChans = transObj->m_subset[i].second.size();
        // skip subsets without any channels
        if (nfebChans == 0) continue;
        
        unsigned int febid = transObj->m_subset[i].first;
        persObj->m_subset.m_febIds.push_back(febid);
	for (unsigned int j=0; j<nfebChans;++j) {  //Loop over channels in FEB
	  persObj->m_vPedestal.push_back(transObj->m_subset[i].second[j].m_Pedestal);
	  persObj->m_vPedestalRMS.push_back(transObj->m_subset[i].second[j].m_PedestalRMS);
	}//End loop over channels in feb
    }//end loop over febs
    
    // Copy corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // Save channel id in febid vector
        persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);
	persObj->m_vPedestal.push_back(transObj->m_correctionVec[i].second.m_Pedestal);
	persObj->m_vPedestalRMS.push_back(transObj->m_correctionVec[i].second.m_PedestalRMS);
    }//End loop over corrections

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->m_gain; 
    persObj->m_subset.m_channel       = transObj->m_channel;
    persObj->m_subset.m_groupingType  = transObj->m_groupingType;
    
}
