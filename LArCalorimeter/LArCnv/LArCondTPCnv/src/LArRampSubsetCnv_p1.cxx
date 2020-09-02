/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArConditionsSubset.h"

#include "LArCondTPCnv/LArRampSubsetCnv_p1.h"

void
LArRampSubsetCnv_p1::persToTrans(const LArRampPersType* persObj, 
                                  LArRampTransType* transObj, 
                                  MsgStream & log)
{
    transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

    // Copy conditions
    unsigned int nfebids      = persObj->m_subset.m_febIds.size();
    unsigned int nRamps       = persObj->m_vRampSize;
    unsigned int rampIndex    = 0;
    
    // Loop over febs
    unsigned int ifebWithData = 0; // counter for febs with data

    auto subsetIt = transObj->subsetBegin();
    for (unsigned int i = 0; i < nfebids; ++i, ++subsetIt){
        // Set febid
        unsigned int febid = subsetIt->first;

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
                if (rampIndex + persObj->m_vRampSize > persObj->m_vRamp.size()) {
                    log << MSG::ERROR 
                        << "LArRampSubsetCnv_p1::persToTrans - ramp index too large: ramp/size " 
                        << rampIndex << " " << persObj->m_vRamp.size() << " " 
                        << endmsg;
                    return;
                }

                // This channel has ramps, resize vectors
                subsetIt->second[j].m_vRamp.resize(nRamps);

                for (unsigned int k = 0; k < persObj->m_vRampSize; ++k){
                    subsetIt->second[j].m_vRamp[k] = persObj->m_vRamp[rampIndex];
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
    unsigned int ncorrs       = persObj->m_subset.m_corrChannels.size();
    LArRampTransType::CorrectionVec corrs;
    if (ncorrs) {
        // corrs exist - resize vector
        std::vector<float> vRamp(nRamps,0.0);
        LArRampP1  larRampP1(vRamp);

        corrs.resize(ncorrs, LArRampTransType::CorrectionPair(0, larRampP1));
    }

    // Loop over corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
        if (rampIndex + persObj->m_vRampSize > persObj->m_vRamp.size()) {
            log << MSG::ERROR 
                << "LArRampSubsetCnv_p1::persToTrans - ramp index too large: ramp/size " 
                << rampIndex << " " << persObj->m_vRamp.size() << " " 
                << endmsg;
            return;
        }
        corrs[i].first = persObj->m_subset.m_corrChannels[i];
        // Loop over ramps per channel
        for (unsigned int k = 0; k < persObj->m_vRampSize; ++k){
            corrs[i].second.m_vRamp[k] = persObj->m_vRamp[rampIndex];
            rampIndex++;
        }
    }
    transObj->insertCorrections (std::move (corrs));

    // Copy the rest
    transObj->setChannel       (persObj->m_subset.m_channel);
    transObj->setGroupingType  (persObj->m_subset.m_groupingType);
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
    unsigned int nsubsetsNotEmpty = 0;
    unsigned int ncorrs           = transObj->correctionVecSize();
    unsigned int nchans           = 0;
    unsigned int nRamps           = 0;
    bool foundNRamps              = false;
    std::vector<unsigned int> febsWithSparseData;

    // Find the number of ramps and check for sparse conditions,
    // e.g. MC conditions
    const auto subsetEnd = transObj->subsetEnd();
    for (auto subsetIt = transObj->subsetBegin();
         subsetIt != subsetEnd;
         ++subsetIt)
    {
        unsigned int nfebChans = subsetIt->second.size();

        if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
            log << MSG::ERROR 
                << "LArRampSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endmsg;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets

        // Loop over channels and check if this subset has sparse data
        bool subsetIsSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) {
            const LArRampP1& ramp = subsetIt->second[j];
            if (ramp.m_vRamp.size() == 0) {
                if (!subsetIsSparse) {
                    // save febids for sparse subsets
                    subsetIsSparse = true;
                    febsWithSparseData.push_back(subsetIt->first);
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
      const LArRampP1& ramp = transObj->correctionVecBegin()->second;
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
    for (auto subsetIt = transObj->subsetBegin();
         subsetIt != subsetEnd;
         ++subsetIt)
    {
        unsigned int nfebChans = subsetIt->second.size();

        // skip subsets without any channels
        if (nfebChans == 0) continue;
        
        unsigned int febid = subsetIt->first;
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

                if (subsetIt->second[j].m_vRamp.size() > 0) {
                    // store the channel number in bit map
                    assert (j >= chansOffset && (j - chansOffset) <= 31);
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
		  if (k>=subsetIt->second[j].m_vRamp.size()) {
		    tooSmall=true;
		    persObj->m_vRamp.push_back(0.0);
		  }
		  else
                    persObj->m_vRamp.push_back(subsetIt->second[j].m_vRamp[k]);
                }
		if (tooSmall)
		  log << MSG::ERROR << "Feb 0x" << std::hex << febid << std::dec << " channel " << j <<": Ramp object too small. Expected a polynom of degree "
		      << nRamps << ". Padded with 0.0" << endmsg; 
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
    const auto corrEnd = transObj->correctionVecEnd();
    for (auto corrIt = transObj->correctionVecBegin();
         corrIt != corrEnd;
         ++corrIt)
    {
        // Save channel id in febid vector
        bool tooSmall=false;
        persObj->m_subset.m_corrChannels.push_back(corrIt->first);
        // Ramps
        for (unsigned int k = 0; k < nRamps; ++k){
	  if (k>=corrIt->second.m_vRamp.size()) {
	    tooSmall=true;
	    persObj->m_vRamp.push_back(0.0);
	  }
	  else
            persObj->m_vRamp.push_back(corrIt->second.m_vRamp[k]);
        }
	if (tooSmall)
	  log << MSG::ERROR << "Correction (channel 0x" << std::hex << corrIt->first << std::dec <<  
	    "): Ramp object too small. Expected a polynom of degree " << nRamps << ". Padded with 0.0" <<  endmsg;
    }

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->gain(); 
    persObj->m_subset.m_channel       = transObj->channel();
    persObj->m_subset.m_groupingType  = transObj->groupingType();
}
