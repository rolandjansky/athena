/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"

void
LArAutoCorrSubsetCnv_p1::persToTrans(const LArAutoCorrPersType* persObj, 
                                  LArAutoCorrTransType* transObj, 
                                  MsgStream & log)
{
    // Copy conditions
    unsigned int ncorrs        = persObj->m_subset.m_corrChannels.size();
    unsigned int nfebids       = persObj->m_subset.m_febIds.size();
    unsigned int nAutoCorrs    = persObj->m_vAutoCorrSize;
    unsigned int autocorrIndex    = 0;
    
    // resize subset to with then number of febids
    transObj->m_subset.resize(nfebids);

    // Loop over febs
    unsigned int febid        = 0;
    unsigned int ifebWithData = 0; // counter for febs with data

    for (unsigned int i = 0; i < nfebids; ++i){
        // Set febid
        febid = transObj->m_subset[i].first = persObj->m_subset.m_febIds[i];
        // Fill channels with empty autocorr vectors
        transObj->m_subset[i].second.resize(NCHANNELPERFEB);
        bool hasSparseData       = false;
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
        if (ifebWithData < persObj->m_subset.m_febsWithSparseData.size() &&
            febid == persObj->m_subset.m_febsWithSparseData[ifebWithData] ) {
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
                        
                // Channel has autocorrs - loop over autocorrs per channel
                // and copy to the persistent object

                // check indexes
                if (autocorrIndex    >= persObj->m_vAutoCorr.size()) {
                    log << MSG::ERROR 
                        << "LArAutoCorrSubsetCnv_p1::persToTrans - autocorr index too large: autocorr/size " 
                        << autocorrIndex << " " << persObj->m_vAutoCorr.size() << " " 
                        << endreq;
                    return;
                }

                // This channel has autocorrs, resize vectors
                transObj->m_subset[i].second[j].m_vAutoCorr.resize(nAutoCorrs);

                for (unsigned int k = 0; k < persObj->m_vAutoCorrSize; ++k){
                    transObj->m_subset[i].second[j].m_vAutoCorr[k] = persObj->m_vAutoCorr[autocorrIndex];
                    autocorrIndex++;
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
        std::vector<float> vAutoCorr(nAutoCorrs,0.0);
        LArAutoCorrP1  larAutoCorrP1(vAutoCorr);

        transObj->m_correctionVec.resize(ncorrs, LArAutoCorrTransType::CorrectionPair(0, larAutoCorrP1));
    }

    // Loop over corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
        if (autocorrIndex    >= persObj->m_vAutoCorr.size()) {
            log << MSG::ERROR 
                << "LArAutoCorrSubsetCnv_p1::persToTrans - autocorr index too large: autocorr/size " 
                << autocorrIndex << " " << persObj->m_vAutoCorr.size() << " " 
                << endreq;
            return;
        }
        transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
        // Loop over autocorrs per channel
        for (unsigned int k = 0; k < persObj->m_vAutoCorrSize; ++k){
            transObj->m_correctionVec[i].second.m_vAutoCorr[k] = persObj->m_vAutoCorr[autocorrIndex];
            autocorrIndex++;
        }
    }

    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType;

}


        
    
void
LArAutoCorrSubsetCnv_p1::transToPers(const LArAutoCorrTransType* transObj, 
                                  LArAutoCorrPersType* persObj, 
                                  MsgStream &log) 
{
    
    // Copy conditions

    // We copy all autocorrs into a single vector. 
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
    // m_subset.m_corrChannels and the autocorrs in the same vectors as
    // the rest of the conditions data.
    //
    // For each channel with data, the number of autocorrs is assumed
    // constant. This is calculated at the beginning, along with
    // whether a feb is sparse or not.
    //

    // Get the number of channels, corrections and the size of autocorr 
    unsigned int nfebs            = transObj->m_subset.size();
    unsigned int nsubsetsNotEmpty = 0;
    unsigned int ncorrs           = transObj->m_correctionVec.size();
    unsigned int nchans           = 0;
    unsigned int nAutoCorrs       = 0;
    bool foundNAutoCorrs          = false;
    std::vector<unsigned int> febsWithSparseData;

    // Find the number of autocorrs and check for sparse conditions,
    // e.g. MC conditions
    for (unsigned int i = 0; i < nfebs; ++i){

        unsigned int nfebChans = transObj->m_subset[i].second.size();

        if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
            log << MSG::ERROR 
                << "LArAutoCorrSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endreq;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets

        // Loop over channels and check if this subset has sparse data
        bool subsetIsSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) {
            const LArAutoCorrP1& autocorr = transObj->m_subset[i].second[j];
            if (autocorr.m_vAutoCorr.size() == 0) {
                if (!subsetIsSparse) {
                    // save febids for sparse subsets
                    subsetIsSparse = true;
                    febsWithSparseData.push_back(transObj->m_subset[i].first);
                }
            }
            else {
                nchans++; // count number of channels
                if (!foundNAutoCorrs) {
                    // Save the number of autocorrs and derivatives from first channels present
                    nAutoCorrs        = autocorr.m_vAutoCorr.size();
                    foundNAutoCorrs   = true;
                }
            }
        }
    }
    if (!foundNAutoCorrs && ncorrs>0) {
        // Save the number of autocorrs and derivatives from first
        // correct - couldn't find it from channels
            const LArAutoCorrP1& autocorr = transObj->m_correctionVec[0].second;
            nAutoCorrs                    = autocorr.m_vAutoCorr.size();
    }
    // Save sizes
    persObj->m_vAutoCorrSize    = nAutoCorrs;

    // Reserve space in vectors
    persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
    persObj->m_subset.m_corrChannels.reserve(ncorrs);
    unsigned int nAutoCorrsTot     = (nchans + ncorrs)*nAutoCorrs;
    persObj->m_vAutoCorr.reserve(nAutoCorrsTot);

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
            
        // Now loop over channels and save autocorrs and channel number
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
        for (unsigned int j = 0; j < nfebChans; ++j){

            bool saveAutoCorrs = true;
            if (isSparse) {
                // subset with sparse data

                if (transObj->m_subset[i].second[j].m_vAutoCorr.size() > 0) {
                    // store the channel number in bit map
                    if (j < chansOffset || (j - chansOffset) > 31) {
                        log << MSG::ERROR 
                            << "LArAutoCorrSubsetCnv_p1::transToPers - incorrect channel indexing: j, chansOffset: " << j << " " << chansOffset
                            << endreq;
                    }
                    chansSet |= (1 << (j - chansOffset));
                }
                else {
                    saveAutoCorrs = false;
                }
                // Save chansSet
                if  (j == (chansOffset + 31)) {
                    persObj->m_subset.m_febsWithSparseData.push_back(chansSet);
                    chansSet    =   0;
                    chansOffset += 32;
                }
            }
            if (saveAutoCorrs) {
                // save autocorrs
 	        bool tooSmall=false;
                for (unsigned int k = 0; k < nAutoCorrs; ++k){
		  if (k>=transObj->m_subset[i].second[j].m_vAutoCorr.size()) {
		    persObj->m_vAutoCorr.push_back(0.);
		    tooSmall=true;
		  }
		  else
		    persObj->m_vAutoCorr.push_back(transObj->m_subset[i].second[j].m_vAutoCorr[k]);
                }
		if (tooSmall)
		  log << MSG::ERROR << "Feb 0x" << std::hex << febid << std::dec << " channel " << j <<": AutoCorr object too small. Expected " 
		      << nAutoCorrs << " entries. Padded with 0.0" << endreq;
	    }
//             static unsigned int nch = 0;
//             ++nch;
//             std::cout << "transToPers - i, j, save " << i << " " << j << " " 
//                       << saveAutoCorrs << " " << nch << " febid " << febid
//                       << " chansSet " << std::hex << chansSet << std::dec
//                       << " chansOffset " << chansOffset
//                       << std::endl;

        }
    }
    
    // Copy corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // Save channel id in febid vector
        persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);
        // AutoCorrs
	bool tooSmall=false;
        for (unsigned int k = 0; k < nAutoCorrs; ++k){
	  if (k>=transObj->m_correctionVec[i].second.m_vAutoCorr.size()) {
	    persObj->m_vAutoCorr.push_back(0.);
	    tooSmall=true;
	  }
	  else
            persObj->m_vAutoCorr.push_back(transObj->m_correctionVec[i].second.m_vAutoCorr[k]);
        }
	if (tooSmall)
	  log << MSG::ERROR << "Correction index "<< i <<"(channel 0x" << std::hex << transObj->m_correctionVec[i].first << std::dec <<  
	    "): AutoCorr object too small. Expected " << nAutoCorrs << " entries. Padded with 0.0" << endreq;
    }
    // Copy the rest
    persObj->m_subset.m_gain          = transObj->m_gain; 
    persObj->m_subset.m_channel       = transObj->m_channel;
    persObj->m_subset.m_groupingType  = transObj->m_groupingType;
    
}
