/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArConditionsSubset.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p1.h"

void
LArShapeSubsetCnv_p1::persToTrans(const LArShapePersType1* persObj, 
                                  LArShapeTransType1* transObj, 
                                  MsgStream & log)
{
    transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

    // Copy conditions
    unsigned int nfebids       = persObj->m_subset.m_febIds.size();
    unsigned int nShapes       = persObj->m_vShapeSize;
    unsigned int nShapeDers    = persObj->m_vShapeDerSize;
    unsigned int shapeIndex    = 0;
    unsigned int shapederIndex = 0;
    
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
                        
                // Channel has shapes - loop over shapes per channel
                // and copy to the persistent object

                // check indexes
                if (shapeIndex    >= persObj->m_vShape.size() ||
                    shapederIndex >= persObj->m_vShapeDer.size()) {
                    log << MSG::ERROR 
                        << "LArShapeSubsetCnv_p1::persToTrans - shape index too large: shape/size, shapeder/size " 
                        << shapeIndex << " " << persObj->m_vShape.size() << " " 
                        << shapederIndex << " " << persObj->m_vShapeDer.size() 
                        << endmsg;
                    return;
                }

                // This channel has shapes, resize vectors
                subsetIt->second[j].m_vShape.resize(nShapes);
                subsetIt->second[j].m_vShapeDer.resize(nShapeDers);

                for (unsigned int k = 0; k < persObj->m_vShapeSize; ++k){
                    subsetIt->second[j].m_vShape[k] = persObj->m_vShape[shapeIndex];
                    shapeIndex++;
                }
                // Loop over shapeders per channel
                for (unsigned int k = 0; k < persObj->m_vShapeDerSize; ++k){
                    subsetIt->second[j].m_vShapeDer[k] = persObj->m_vShapeDer[shapederIndex];
                    shapederIndex++;
                }
            }

//             static unsigned int nch1 = 0;
//             ++nch1;
//             std::cout << "persToTrans - i, j, copy " << i << " " << j << " " 
//                       << copyChannel << " " << (chansSet & (1 << (j + chansOffset))) << " " 
//                       << (chansSet & (1 << (j - chansOffset))) << " " 
//                       << nch1 
//                       << " hasSparseData " << hasSparseData 
//                       << " chansSet, chansOffset, ifebWithData " << std::hex 
//                       << chansSet << std::dec << " " << chansOffset << " " << ifebWithData 
//                       << " febids " << febid
//                       << std::endl;

        }
    }

    // Copy corrections
    unsigned int ncorrs        = persObj->m_subset.m_corrChannels.size();
    LArShapeTransType1::CorrectionVec corrs;

    if (ncorrs) {
        // corrs exist - resize vector
        std::vector<float> vShape(nShapes,0.0);
        std::vector<float> vShapeDer(nShapeDers,0.0);
        LArShapeP1  larShapeP1(vShape, vShapeDer);

        corrs.resize(ncorrs, LArShapeTransType1::CorrectionPair(0, larShapeP1));
    }

    // Loop over corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
        if (shapeIndex    >= persObj->m_vShape.size() ||
            shapederIndex >= persObj->m_vShapeDer.size()) {
            log << MSG::ERROR 
                << "LArShapeSubsetCnv_p1::persToTrans - shape index too large: shape/size, shapeder/size " 
                << shapeIndex << " " << persObj->m_vShape.size() << " " 
                << shapederIndex << " " << persObj->m_vShapeDer.size() 
                << endmsg;
            return;
        }
        corrs[i].first = persObj->m_subset.m_corrChannels[i];
        // Loop over shapes per channel
        for (unsigned int k = 0; k < persObj->m_vShapeSize; ++k){
            corrs[i].second.m_vShape[k] = persObj->m_vShape[shapeIndex];
            shapeIndex++;
        }
        // Loop over shapeders per channel
        for (unsigned int k = 0; k < persObj->m_vShapeDerSize; ++k){
            corrs[i].second.m_vShapeDer[k] = persObj->m_vShapeDer[shapederIndex];
            shapederIndex++;
        }
    }
    transObj->insertCorrections (std::move (corrs));

    // Copy the rest
    transObj->setChannel       (persObj->m_subset.m_channel);
    transObj->setGroupingType  (persObj->m_subset.m_groupingType);
}


        
    
void
LArShapeSubsetCnv_p1::transToPers(const LArShapeTransType1* transObj, 
                                  LArShapePersType1* persObj, 
                                  MsgStream &log) 
{
    // Copy conditions

    // We copy all shapes into a single vector and the same for
    // shaperDers. 
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
    // m_subset.m_corrChannels and the shapes and shapeders in the
    // same vectors as the rest of the conditions data.
    //
    // For each channel with data, the number of shapes and shape
    // derivatives is assumed constant. This is calculated at the
    // beginning, along with whether a feb is sparse or not.
    //

    // Get the number of channels, corrections and the size of shape and shapeder
    unsigned int nsubsetsNotEmpty = 0;
    unsigned int ncorrs           = transObj->correctionVecSize();
    unsigned int nchans           = 0;
    unsigned int nShapes          = 0;
    unsigned int nShapeDers       = 0;
    bool foundNShapes             = false;
    std::vector<unsigned int> febsWithSparseData;

    // Find the number of shapes/shapeders and check for sparse
    // conditions, e.g. MC conditions
    const auto subsetEnd = transObj->subsetEnd();
    for (auto subsetIt = transObj->subsetBegin();
         subsetIt != subsetEnd;
         ++subsetIt)
    {
        unsigned int nfebChans = subsetIt->second.size();

        if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
            log << MSG::ERROR 
                << "LArShapeSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endmsg;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets

        // Loop over channels and check if this subset has sparse data
        bool subsetIsSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) {
            const LArShapeP1& shape = subsetIt->second[j];
            if (shape.m_vShape.size() == 0) {
                if (!subsetIsSparse) {
                    // save febids for sparse subsets
                    subsetIsSparse = true;
                    febsWithSparseData.push_back(subsetIt->first);
                }
            }
            else {
                nchans++; // count number of channels
                if (!foundNShapes) {
                    // Save the number of shapes and derivatives from first channels present
                    nShapes                 = shape.m_vShape.size();
                    nShapeDers              = shape.m_vShapeDer.size();
                    foundNShapes            = true;
                }
            }
        }
    }
    if (!foundNShapes && ncorrs > 0) {
        // Save the number of shapes and derivatives from first
        // correct - couldn't find it from channels
            const LArShapeP1& shape = transObj->correctionVecBegin()->second;
            nShapes                 = shape.m_vShape.size();
            nShapeDers              = shape.m_vShapeDer.size();
    }
    if (nShapes == 0 && nShapeDers == 0) {
        log << MSG::ERROR 
            << "LArShapeSubsetCnv_p1::transToPers - cannot get number of shapes and shape derivatives" 
            << endmsg;
        return;
    }
    
    // Save sizes
    persObj->m_vShapeSize    = nShapes;
    persObj->m_vShapeDerSize = nShapeDers;

    // Reserve space in vectors
    persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
    persObj->m_subset.m_corrChannels.reserve(ncorrs);
    unsigned int nShapesTot     = (nchans + ncorrs)*nShapes;
    unsigned int nShapeDersTot  = (nchans + ncorrs)*nShapeDers;
    persObj->m_vShape.reserve(nShapesTot);
    persObj->m_vShapeDer.reserve(nShapeDersTot);

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
            
        // Now loop over channels and save shapes/shapeders and
        // channel number
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
        for (unsigned int j = 0; j < nfebChans; ++j){

            bool saveShapes = true;
            if (isSparse) {
                // subset with sparse data

                if (subsetIt->second[j].m_vShape.size() > 0) {
                    // store the channel number in bit map
                    assert (j >= chansOffset && (j - chansOffset) <= 31);
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
                // save shapes
                for (unsigned int k = 0; k < nShapes; ++k){
                    persObj->m_vShape.push_back(subsetIt->second[j].m_vShape[k]);
                }
                // save shapeders
                for (unsigned int k = 0; k < nShapeDers; ++k){
                    persObj->m_vShapeDer.push_back(subsetIt->second[j].m_vShapeDer[k]);
                }
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
    const auto corrEnd = transObj->correctionVecEnd();
    for (auto corrIt = transObj->correctionVecBegin();
         corrIt != corrEnd;
         ++corrIt)
    {
        // Save channel id in febid vector
        persObj->m_subset.m_corrChannels.push_back(corrIt->first);
        // Shapes
        for (unsigned int k = 0; k < nShapes; ++k){
            persObj->m_vShape.push_back(corrIt->second.m_vShape[k]);
        }
        // ShapeDers
        for (unsigned int k = 0; k < nShapeDers; ++k){
            persObj->m_vShapeDer.push_back(corrIt->second.m_vShapeDer[k]);
        }
    }

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->gain(); 
    persObj->m_subset.m_channel       = transObj->channel();
    persObj->m_subset.m_groupingType  = transObj->groupingType();
}
