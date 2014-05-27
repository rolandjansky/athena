/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArShapeSubsetCnv_p2.h"

void
LArShapeSubsetCnv_p2::persToTrans(const LArShapePersType2* persObj, 
                                  LArShapeTransType2* transObj, 
                                  MsgStream & log)
{
    // Copy conditions
    unsigned int ncorrs     = persObj->m_subset.m_corrChannels.size();
    unsigned int nfebids    = persObj->m_subset.m_febIds.size();
    unsigned int nPhases    = persObj->m_nPhases;
    unsigned int nSamples   = persObj->m_nSamples;
    unsigned int dataIndex  = 0;
    unsigned int timeIndex  = 0;

    // resize subset to with then number of febids
    transObj->m_subset.resize(nfebids);

    // Loop over febs
    unsigned int febid        = 0;
    unsigned int ifebWithData = 0; // counter for febs with data

    for (unsigned int i = 0; i < nfebids; ++i){
        // Set febid
        febid = transObj->m_subset[i].first = persObj->m_subset.m_febIds[i];
        // Fill channels with empty vectors
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
                        
                // Channel has shapes - loop over shapes per channel and
                // copy to the persistent object

                // check indexes
                if (dataIndex  >= persObj->m_vShape.size()    ||
                    dataIndex  >= persObj->m_vShapeDer.size() ||
                    timeIndex >= persObj->m_timeOffset.size() ||
                    timeIndex >= persObj->m_timeBinWidth.size()) {
                    log << MSG::ERROR 
                        << "LArShapeSubsetCnv_p2::persToTrans - shape index too large: shapeIndex size shape, size shapeDer, timeIndex timeOffset size, timeBinWidth size " 
                        << dataIndex  << " " << persObj->m_vShape.size() << " " 
                        << persObj->m_vShapeDer.size() << " " << timeIndex << " "
                        << persObj->m_timeOffset.size() << " " 
                        << persObj->m_timeBinWidth.size()
                        << endreq;
                    return;
                }

                LArShapeTransType2::Reference shape =
                  transObj->m_subset[i].second[j];
                LArShapeP2 tmp (persObj->m_timeOffset[timeIndex],
                                persObj->m_timeBinWidth[timeIndex],
                                nPhases,
                                nSamples,
                                persObj->m_vShape,
                                persObj->m_vShapeDer,
                                dataIndex);
                shape.assign (tmp);
                ++timeIndex;
                dataIndex += nPhases * nSamples;
            }
        }
    }
    
    // Copy corrections
    
    if (ncorrs) {
        // corrs exist - resize vector
        std::vector<float>               vSamples(nSamples, 0.0);
        std::vector<std::vector<float> > vShape(nPhases, vSamples);
        LArShapeP2  larShapeP2(0.0, 0.0, vShape, vShape);

        transObj->m_correctionVec.resize(ncorrs, LArShapeTransType2::CorrectionPair(0, larShapeP2));
    }

    // Loop over corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
        if (dataIndex  >= persObj->m_vShape.size()    ||
            dataIndex  >= persObj->m_vShapeDer.size() ||
            timeIndex >= persObj->m_timeOffset.size() ||
            timeIndex >= persObj->m_timeBinWidth.size()) {
            log << MSG::ERROR 
                << "LArShapeSubsetCnv_p2::persToTrans - data index too large: dataIndex size shape, size shapeDer, timeIndex timeOffset size, timeBinWidth size " 
                << dataIndex  << " " << persObj->m_vShape.size() << " " 
                << persObj->m_vShapeDer.size() << " " << timeIndex << " "
                << persObj->m_timeOffset.size() << " " 
                << persObj->m_timeBinWidth.size()
            << endreq;
            return;
        }

        // copy channel id
        transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];

        LArShapeP2& shape = transObj->m_correctionVec[i].second;
        LArShapeP2 tmp (persObj->m_timeOffset[timeIndex],
                        persObj->m_timeBinWidth[timeIndex],
                        nPhases,
                        nSamples,
                        persObj->m_vShape,
                        persObj->m_vShapeDer,
                        dataIndex);
        shape.setFrom (tmp);
        ++timeIndex;
        dataIndex += nPhases * nSamples;
    }

    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType;

    transObj->m_subset.trim();
}


        
    
void
LArShapeSubsetCnv_p2::transToPers(const LArShapeTransType2* transObj, 
				  LArShapePersType2* persObj, 
				  MsgStream &log) 
{
    
    // Copy conditions

    // We copy all shapes into a few simple vectors. 
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
    // m_subset.m_corrChannels and the shapes in the same vectors as the
    // rest of the conditions data.
    //
    // For each channel with data, the number of shapes is assumed
    // constant. This is calculated at the beginning, along with
    // whether a feb is sparse or not.
    //

    // Get the number of channels, corrections and the size of shape vectors
    unsigned int nfebs            = transObj->m_subset.size();
    unsigned int nsubsetsNotEmpty = 0;
    unsigned int ncorrs           = transObj->m_correctionVec.size();
    unsigned int nchans           = 0;
    unsigned int nPhases          = 0;
    unsigned int nSamples         = 0;
    bool foundShapes                = false;
    std::vector<unsigned int> febsWithSparseData;

    // Find the number of shapes and check for sparse conditions,
    // e.g. MC conditions
    for (unsigned int i = 0; i < nfebs; ++i){

        unsigned int nfebChans = transObj->m_subset[i].second.size();

        if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
            log << MSG::ERROR 
                << "LArShapeSubsetCnv_p2::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endreq;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets

        // Loop over channels and check if this subset has sparse data
        bool subsetIsSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) {
            LArShapeTransType2::ConstReference shapes =
              transObj->m_subset[i].second[j];
            if (shapes.shapeSize() == 0) {
                if (!subsetIsSparse) {
                    // save febids for sparse subsets
                    subsetIsSparse = true;
                    febsWithSparseData.push_back(transObj->m_subset[i].first);
                }
            }
            else {
                nchans++; // count number of channels
                if (!foundShapes) {
                    // Save the number of phases and samples for each
                    // shape from first channels present
                    nPhases       = shapes.shapeSize();
                    nSamples      = shapes.shape(0).size();
                    foundShapes     = true;
                }
            }
        }
    }
    if (!foundShapes && ncorrs>0) {
        // Save the number of phases and samples for each shape from
        // first correction - couldn't find it from channels
        const LArShapeP2& shapes = transObj->m_correctionVec[0].second;
        nPhases             = shapes.shapeSize();
        nSamples            = shapes.shape(0).size();
    }
    
    // Save sizes
    persObj->m_nPhases    = nPhases;
    persObj->m_nSamples   = nSamples;

    // Reserve space in vectors
    persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
    persObj->m_subset.m_corrChannels.reserve(ncorrs);
    unsigned int ndataTot  = (nchans + ncorrs)*nPhases*nSamples;
    unsigned int nTime     = (nchans + ncorrs);
    persObj->m_vShape.reserve(ndataTot);
    persObj->m_vShapeDer.reserve(ndataTot);
    persObj->m_timeOffset.reserve(nTime);
    persObj->m_timeBinWidth.reserve(nTime);

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
            
        // Now loop over channels 
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
        for (unsigned int j = 0; j < nfebChans; ++j){

            bool saveShapes = true;
            if (isSparse) {
                // subset with sparse data
	      if (transObj->m_subset[i].second[j].shapeSize() > 0) {
                    // store the channel number in bit map
                    if (j < chansOffset || (j - chansOffset) > 31) {
                        log << MSG::ERROR 
                            << "LArShapeSubsetCnv_p2::transToPers - incorrect channel indexing: j, chansOffset: " << j << " " << chansOffset
                            << endreq;
                    }
                    chansSet |= (1 << (j - chansOffset));
                }
                else {
		  saveShapes = false;
                }
                // Save chansSet
                if  (j == (chansOffset + 31)) {
                    persObj->m_subset.m_febsWithSparseData.push_back(chansSet);
                    chansSet    =   0;
                    chansOffset += 32;
                }
            }
            if (saveShapes) {
	        bool tooSmall=false;
                // Loop over phases and samples per channel
                for (unsigned int k = 0; k < nPhases; ++k) {
                    for (unsigned int l = 0; l < nSamples; ++l) {
		      //check if data object is big enough
		      if (k>=transObj->m_subset[i].second[j].shapeSize() || l>=transObj->m_subset[i].second[j].shape(k).size()) {
			tooSmall=true;
			persObj->m_vShape.push_back(0.);
			persObj->m_vShapeDer.push_back(0.);
		      }
		      else {
                        persObj->m_vShape.push_back(transObj->m_subset[i].second[j].shape(k)[l]);
                        persObj->m_vShapeDer.push_back(transObj->m_subset[i].second[j].shapeDer(k)[l]);
//                      std::cout << "WL Data: FEB=" << std::hex << febid << std::dec << " [" << i << "] Channel=" 
// 				  << j << " Phase="<< k<< " Sample " << l << " Shape=" 
// 				  << transObj->m_subset[i].second[j].m_vShape[k][l] << std::endl;
		      }
                    }
                }
                // set time offset and binwidth
                persObj->m_timeOffset.push_back(transObj->m_subset[i].second[j].timeOffset());
                persObj->m_timeBinWidth.push_back(transObj->m_subset[i].second[j].timeBinWidth());
		if (tooSmall)
		  log << MSG::ERROR << "Feb 0x" << std::hex << febid << std::dec << " channel " << j <<": Shape object too small. Expected " 
		      << nPhases << " phases and " << nSamples << " samples. Padded with 0.0" << endreq;
            }

//             static unsigned int nch = 0;
//             ++nch;
//             std::cout << "transToPers - i, j, save " << i << " " << j << " " 
//                       << saveShapes << " " << nch << " febid " << febid
//                       << " chansSet " << std::hex << chansSet << std::dec
//                       << " chansOffset " << chansOffset
//                       << std::endl;

        }
    }
    
    // Copy corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // Save channel id in febid vector
        persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);
        // Shapes
	bool tooSmall=false;
        // Loop over phases and samples per channel
        for (unsigned int k = 0; k < nPhases; ++k) {
            for (unsigned int l = 0; l < nSamples; ++l) {
	      if (k>=transObj->m_correctionVec[i].second.shapeSize() || l>=transObj->m_correctionVec[i].second.shape(k).size()) {
		tooSmall=true;
		persObj->m_vShape.push_back(0.);
		persObj->m_vShapeDer.push_back(0.);
	      }
	      else {
                persObj->m_vShape.push_back(transObj->m_correctionVec[i].second.shape(k)[l]);
                persObj->m_vShapeDer.push_back(transObj->m_correctionVec[i].second.shapeDer(k)[l]);
	      }
            }
        }
        // set time offset and binwidth
        persObj->m_timeOffset.push_back(transObj->m_correctionVec[i].second.timeOffset());
        persObj->m_timeBinWidth.push_back(transObj->m_correctionVec[i].second.timeBinWidth());
	if (tooSmall)
	  log << MSG::ERROR << "Correction index "<< i <<"(channel 0x" << std::hex << transObj->m_correctionVec[i].first << std::dec <<  
	    "): Shape object too small. Expected " << nPhases << " phases and " << nSamples << " samples. Padded with 0.0" << endreq;
    }

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->m_gain; 
    persObj->m_subset.m_channel       = transObj->m_channel;
    persObj->m_subset.m_groupingType  = transObj->m_groupingType;
    
}
