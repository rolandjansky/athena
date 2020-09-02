/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArConditionsSubset.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"

void
LArAutoCorrSubsetCnv_p1::persToTrans(const LArAutoCorrPersType* persObj, 
                                  LArAutoCorrTransType* transObj, 
                                  MsgStream & log)
{
  transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

  unsigned int nfebids          = persObj->m_subset.m_febIds.size();
  unsigned int nAutoCorrs       = persObj->m_vAutoCorrSize;
  unsigned int autocorrIndex    = 0;

  unsigned int ifebWithData = 0; // counter for febs with data
  auto subsetIt = transObj->subsetBegin();
  for (unsigned int i = 0; i < nfebids; ++i, ++subsetIt){
    unsigned int febid = subsetIt->first;
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
        if (autocorrIndex + nAutoCorrs > persObj->m_vAutoCorr.size()) {
          log << MSG::ERROR 
              << "LArAutoCorrSubsetCnv_p1::persToTrans - autocorr index too large: autocorr/size " 
              << autocorrIndex << " " << persObj->m_vAutoCorr.size() << " " 
              << endmsg;
          return;
        }

        subsetIt->second[j].m_vAutoCorr.assign
          (persObj->m_vAutoCorr.begin() + autocorrIndex,
           persObj->m_vAutoCorr.begin() + autocorrIndex + nAutoCorrs);
        autocorrIndex += nAutoCorrs;
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

  unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
  LArAutoCorrTransType::CorrectionVec corrs;
  corrs.resize(ncorrs);
  
  // Loop over corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    // check indexes
    if (autocorrIndex + nAutoCorrs > persObj->m_vAutoCorr.size()) {
      log << MSG::ERROR 
          << "LArAutoCorrSubsetCnv_p1::persToTrans - autocorr index too large: autocorr/size " 
          << autocorrIndex << " " << persObj->m_vAutoCorr.size() << " " 
          << endmsg;
      return;
    }

    corrs[i].first = persObj->m_subset.m_corrChannels[i];

    corrs[i].second.m_vAutoCorr.assign
      (persObj->m_vAutoCorr.begin() + autocorrIndex,
       persObj->m_vAutoCorr.begin() + autocorrIndex + nAutoCorrs);
    autocorrIndex += nAutoCorrs;
  }
  transObj->insertCorrections (std::move (corrs));

  // Copy the rest
  transObj->setChannel       (persObj->m_subset.m_channel);
  transObj->setGroupingType  (persObj->m_subset.m_groupingType);
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
    unsigned int nsubsetsNotEmpty = 0;
    unsigned int ncorrs           = transObj->correctionVecSize();
    unsigned int nchans           = 0;
    unsigned int nAutoCorrs       = 0;
    bool foundNAutoCorrs          = false;
    std::vector<unsigned int> febsWithSparseData;

    // Find the number of autocorrs and check for sparse conditions,
    // e.g. MC conditions
    const auto subsetEnd = transObj->subsetEnd();
    for (auto subsetIt = transObj->subsetBegin();
         subsetIt != subsetEnd;
         ++subsetIt)
    {
        unsigned int nfebChans = subsetIt->second.size();

        if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
            log << MSG::ERROR 
                << "LArAutoCorrSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endmsg;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets

        // Loop over channels and check if this subset has sparse data
        bool subsetIsSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) {
            const LArAutoCorrP1& autocorr = subsetIt->second[j];
            if (autocorr.m_vAutoCorr.size() == 0) {
                if (!subsetIsSparse) {
                    // save febids for sparse subsets
                    subsetIsSparse = true;
                    febsWithSparseData.push_back(subsetIt->first);
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
            const LArAutoCorrP1& autocorr = transObj->correctionVecBegin()->second;
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
            
        // Now loop over channels and save autocorrs and channel number
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
        for (unsigned int j = 0; j < nfebChans; ++j){

            bool saveAutoCorrs = true;
            if (isSparse) {
                // subset with sparse data

                if (subsetIt->second[j].m_vAutoCorr.size() > 0) {
                    // store the channel number in bit map
                    assert (j >= chansOffset && (j - chansOffset) <= 31);
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
		  if (k>=subsetIt->second[j].m_vAutoCorr.size()) {
		    persObj->m_vAutoCorr.push_back(0.);
		    tooSmall=true;
		  }
		  else
		    persObj->m_vAutoCorr.push_back(subsetIt->second[j].m_vAutoCorr[k]);
                }
		if (tooSmall)
		  log << MSG::ERROR << "Feb 0x" << std::hex << febid << std::dec << " channel " << j <<": AutoCorr object too small. Expected " 
		      << nAutoCorrs << " entries. Padded with 0.0" << endmsg;
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

    const auto corrEnd = transObj->correctionVecEnd();
    for (auto corrIt = transObj->correctionVecBegin();
         corrIt != corrEnd;
         ++corrIt)
    {
        // Save channel id in febid vector
        persObj->m_subset.m_corrChannels.push_back(corrIt->first);
        // AutoCorrs
	bool tooSmall=false;
        for (unsigned int k = 0; k < nAutoCorrs; ++k){
	  if (k>=corrIt->second.m_vAutoCorr.size()) {
	    persObj->m_vAutoCorr.push_back(0.);
	    tooSmall=true;
	  }
	  else
            persObj->m_vAutoCorr.push_back(corrIt->second.m_vAutoCorr[k]);
        }
	if (tooSmall)
	  log << MSG::ERROR << "Correction (channel 0x" << std::hex << corrIt->first << std::dec <<  
	    "): AutoCorr object too small. Expected " << nAutoCorrs << " entries. Padded with 0.0" << endmsg;
    }

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->gain(); 
    persObj->m_subset.m_channel       = transObj->channel();
    persObj->m_subset.m_groupingType  = transObj->groupingType();
    
}
