/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArRampSubsetCnv_p1.h"

void
LArRampSubsetCnv_p1::persToTrans(const LArRampPersType* persObj, 
                                  LArRampTransType* transObj, 
                                  MsgStream & log)
{
    // Copy conditions
    unsigned int ncorrs       = persObj->m_subset.m_corrChannels.size();
    unsigned int nfebids      = persObj->m_subset.m_febIds.size();
    unsigned int nRamps       = persObj->m_vRampSize;
    unsigned int rampIndex    = 0;
    
    // resize subset to with then number of febids
    transObj->m_subset.resize(nfebids);

    // Loop over febs
    unsigned int febid        = 0;
    unsigned int ifebWithData = 0; // counter for febs with data

    for (unsigned int i = 0; i < nfebids; ++i){
        // Set febid
        febid = transObj->m_subset[i].first = persObj->m_subset.m_febIds[i];
        // Fill channels with empty ramp vectors
        transObj->m_subset[i].second.resize(NCHANNELPERFEB);
        bool hasSparseData       = false;
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
        if (ifebWithData < persObj->m_subset.m_febsWithSparseData.size() &&
            febid == persObj->m_subset.m_febsWithSparseData[ifebWithData]) {
            // Found feb with sparse data
            hasSparseData = true;
            ifebWithData++;
            chansSet    = persObj->m_subset.m_febsWithSparseData[ifebWithData];
            chansOffset = 0;
            ifebWithData++;
        }
            
        // Loop over channels in feb - only some channels are filled
        for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){

            bool copyChannel = true;
            if (hasSparseData) {
                if (!(chansSet & (1 << (j - chansOffset)))) {
                    // Channel is missing data - skip
                    copyChannel = false;
                }
                if (j%32 == 31 && j < 126) {
                    chansSet     = persObj->m_subset.m_febsWithSparseData[ifebWithData];
                    chansOffset += 32;
                    ifebWithData++;
                }
            }
            if (copyChannel) {
                        
                // Channel has ramps - loop over ramps per channel
                // and copy to the persistent object

                // check indexes
                if (rampIndex    >= persObj->m_vRamp.size()) {
                    log << MSG::ERROR 
                        << "LArRampSubsetCnv_p1::persToTrans - ramp index too large: ramp/size " 
                        << rampIndex << " " << persObj->m_vRamp.size() << " " 
                        << endreq;
                    return;
                }

                // This channel has ramps, resize vectors
                transObj->m_subset[i].second[j].m_vRamp.resize(nRamps);

                for (unsigned int k = 0; k < persObj->m_vRampSize; ++k){
                    transObj->m_subset[i].second[j].m_vRamp[k] = persObj->m_vRamp[rampIndex];
                    rampIndex++;
                }
            }

//             static unsigned int nch1 = 0;
//             ++nch1;
//             std::cout << "persToTrans - i, j, copy " << i << " " << j << " " 
//                       << copyChannel << " " << nch1 
//                       << " hasSparseData " << hasSparseData 
//                       << " chansSet, chansOffset, ifebWithData " << std::hex 
//                       << chansSet << std::dec << " " << chansOffset << " " << ifebWithData 
//                       << " febids " << febid << " " 
//                       << sparseFebid
//                       << std::endl;

        }
    }
    
    // Copy corrections
    
    if (ncorrs) {
        // corrs exist - resize vector
        std::vector<float> vRamp(nRamps,0.0);
        LArRampP1  larRampP1(vRamp);

        transObj->m_correctionVec.resize(ncorrs, LArRampTransType::CorrectionPair(0, larRampP1));
    }

    // Loop over corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
        if (rampIndex    >= persObj->m_vRamp.size()) {
            log << MSG::ERROR 
                << "LArRampSubsetCnv_p1::persToTrans - ramp index too large: ramp/size " 
                << rampIndex << " " << persObj->m_vRamp.size() << " " 
                << endreq;
            return;
        }
        transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
        // Loop over ramps per channel
        for (unsigned int k = 0; k < persObj->m_vRampSize; ++k){
            transObj->m_correctionVec[i].second.m_vRamp[k] = persObj->m_vRamp[rampIndex];
            rampIndex++;
        }
    }

    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType;

}


        
    
void
LArRampSubsetCnv_p1::transToPers(const LArRampTransType* transObj, 
                                  LArRampPersType* persObj, 
                                  MsgStream &log) 
{
    
    // Copy conditions

    // We copy all ramps into a single vector. 
    // For the conditions, there are two situations to treat: 
    //   1) dense data: normal conditions where each feb has 128
    //      channels and all channels have data,
    //   2) sparse data: conditions data where some channels are
    //      missing data. This is true for MC conditions (only some
    //      channels have data, and symmetry is used to obtain
    //      conditions for the rest of the channels), as well for
    //      'normal' conditions it may happen that some channels may
    //      be missing data.
    //
    // Treating 1) is straight-forward. For 2) we need to keep track
    // of which channels are present. We do so with
    // m_subset.m_febsWithSparseData where we store the febid followed by
    // four unsigned ints which contain the full bit pattern of the
    // channels set (i.e. bits 0-127).
    //
    // Note that one may also have a subset with all channels missing
    // data. In this case, we do not write out the empty subset.
    //
    // Finally, for corrections, we save the channel ids in
    // m_subset.m_corrChannels and the ramps in the same vectors as
    // the rest of the conditions data.
    //
    // For each channel with data, the number of ramps is assumed
    // constant. This is calculated at the beginning, along with
    // whether a feb is sparse or not.
    //

    // Get the number of channels, corrections and the size of ramp 
    unsigned int nfebs            = transObj->m_subset.size();
    unsigned int nsubsetsNotEmpty = 0;
    unsigned int ncorrs           = transObj->m_correctionVec.size();
    unsigned int nchans           = 0;
    unsigned int nRamps           = 0;
    bool foundNRamps              = false;
    std::vector<unsigned int> febsWithSparseData;

    // Find the number of ramps and check for sparse conditions,
    // e.g. MC conditions
    for (unsigned int i = 0; i < nfebs; ++i){

        unsigned int nfebChans = transObj->m_subset[i].second.size();

        if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
            log << MSG::ERROR 
                << "LArRampSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endreq;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets

        // Loop over channels and check if this subset has sparse data
        bool subsetIsSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) {
            const LArRampP1& ramp = transObj->m_subset[i].second[j];
            if (ramp.m_vRamp.size() == 0) {
                if (!subsetIsSparse) {
                    // save febids for sparse subsets
                    subsetIsSparse = true;
                    febsWithSparseData.push_back(transObj->m_subset[i].first);
                }
            }
            else {
                nchans++; // count number of channels
                if (!foundNRamps) {
                    // Save the number of ramps and derivatives from first channels present
                    nRamps        = ramp.m_vRamp.size();
                    foundNRamps   = true;
                }
            }
        }
    }
    if (!foundNRamps && ncorrs>0) {
        // Save the number of ramps and derivatives from first
        // correct - couldn't find it from channels
      const LArRampP1& ramp = transObj->m_correctionVec[0].second;
      nRamps = ramp.m_vRamp.size();
    }

    
    // Save sizes
    persObj->m_vRampSize    = nRamps;

    // Reserve space in vectors
    persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
    persObj->m_subset.m_corrChannels.reserve(ncorrs);
    unsigned int nRampsTot     = (nchans + ncorrs)*nRamps;
    persObj->m_vRamp.reserve(nRampsTot);

    // For subsets with sparse data, reserve space for identifying
    // channels written out:
    //   1 - febid
    //   4 - for 128 bits (4*32)
    if (febsWithSparseData.size())
        persObj->m_subset.m_febsWithSparseData.reserve(febsWithSparseData.size()*5);

    // Copy conditions in subset
    unsigned int isparse = 0;
    for (unsigned int i = 0; i < nfebs; ++i){

        unsigned int nfebChans = transObj->m_subset[i].second.size();

        // skip subsets without any channels
        if (nfebChans == 0) continue;
        
        unsigned int febid = transObj->m_subset[i].first;
        persObj->m_subset.m_febIds.push_back(febid);


        bool isSparse = false;
        if (isparse < febsWithSparseData.size() && 
            febsWithSparseData[isparse] == febid) {
            // sparse subset, save channels with data
            isparse++;
            isSparse = true;
            // save febid
            persObj->m_subset.m_febsWithSparseData.push_back(febid);            
        }
            
        // Now loop over channels and save ramps and channel number
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
        for (unsigned int j = 0; j < nfebChans; ++j){

            bool saveRamps = true;
            if (isSparse) {
                // subset with sparse data

                if (transObj->m_subset[i].second[j].m_vRamp.size() > 0) {
                    // store the channel number in bit map
                    if (j < chansOffset || (j - chansOffset) > 31) {
                        log << MSG::ERROR 
                            << "LArRampSubsetCnv_p1::transToPers - incorrect channel indexing: j, chansOffset: " << j << " " << chansOffset
                            << endreq;
                    }
                    chansSet |= (1 << (j - chansOffset));
                }
                else {
                    saveRamps = false;
                }
                // Save chansSet
                if  (j == (chansOffset + 31)) {
                    persObj->m_subset.m_febsWithSparseData.push_back(chansSet);
                    chansSet    =   0;
                    chansOffset += 32;
                }
            }
            if (saveRamps) {
                // save ramps
	        bool tooSmall=false;
                for (unsigned int k = 0; k < nRamps; ++k){
		  if (k>=transObj->m_subset[i].second[j].m_vRamp.size()) {
		    tooSmall=true;
		    persObj->m_vRamp.push_back(0.0);
		  }
		  else
                    persObj->m_vRamp.push_back(transObj->m_subset[i].second[j].m_vRamp[k]);
                }
		if (tooSmall)
		  log << MSG::ERROR << "Feb 0x" << std::hex << febid << std::dec << " channel " << j <<": Ramp object too small. Expected a polynom of degree "
		      << nRamps << ". Padded with 0.0" << endreq; 
            }

//             static unsigned int nch = 0;
//             ++nch;
//             std::cout << "transToPers - i, j, save " << i << " " << j << " " 
//                       << saveRamps << " " << nch << " febid " << febid
//                       << " chansSet " << std::hex << chansSet << std::dec
//                       << " chansOffset " << chansOffset
//                       << std::endl;

        }
    }
    
    // Copy corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // Save channel id in febid vector
        bool tooSmall=false;
        persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);
        // Ramps
        for (unsigned int k = 0; k < nRamps; ++k){
	  if (k>=transObj->m_correctionVec[i].second.m_vRamp.size()) {
	    tooSmall=true;
	    persObj->m_vRamp.push_back(0.0);
	  }
	  else
            persObj->m_vRamp.push_back(transObj->m_correctionVec[i].second.m_vRamp[k]);
        }
	if (tooSmall)
	  log << MSG::ERROR << "Correction index "<< i <<"(channel 0x" << std::hex << transObj->m_correctionVec[i].first << std::dec <<  
	    "): Ramp object too small. Expected a polynom of degree " << nRamps << ". Padded with 0.0" <<  endreq;
    }

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->m_gain; 
    persObj->m_subset.m_channel       = transObj->m_channel;
    persObj->m_subset.m_groupingType  = transObj->m_groupingType;
    
}
