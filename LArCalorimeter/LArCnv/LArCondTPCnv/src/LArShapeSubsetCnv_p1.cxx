/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArShapeSubsetCnv_p1.h"

void
LArShapeSubsetCnv_p1::persToTrans(const LArShapePersType1* persObj, 
                                  LArShapeTransType1* transObj, 
                                  MsgStream & log)
{
    // Copy conditions
    unsigned int ncorrs        = persObj->m_subset.m_corrChannels.size();
    unsigned int nfebids       = persObj->m_subset.m_febIds.size();
    unsigned int nShapes       = persObj->m_vShapeSize;
    unsigned int nShapeDers    = persObj->m_vShapeDerSize;
    unsigned int shapeIndex    = 0;
    unsigned int shapederIndex = 0;
    
    // resize subset to with then number of febids
    transObj->m_subset.resize(nfebids);

    // Loop over febs
    unsigned int febid        = 0;
    unsigned int ifebWithData = 0; // counter for febs with data

    for (unsigned int i = 0; i < nfebids; ++i){
        // Set febid
        febid = transObj->m_subset[i].first = persObj->m_subset.m_febIds[i];
        // Fill channels with empty shape vectors
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
                        
                // Channel has shapes - loop over shapes per channel
                // and copy to the persistent object

                // check indexes
                if (shapeIndex    >= persObj->m_vShape.size() ||
                    shapederIndex >= persObj->m_vShapeDer.size()) {
                    log << MSG::ERROR 
                        << "LArShapeSubsetCnv_p1::persToTrans - shape index too large: shape/size, shapeder/size " 
                        << shapeIndex << " " << persObj->m_vShape.size() << " " 
                        << shapederIndex << " " << persObj->m_vShapeDer.size() 
                        << endreq;
                    return;
                }

                // This channel has shapes, resize vectors
                transObj->m_subset[i].second[j].m_vShape.resize(nShapes);
                transObj->m_subset[i].second[j].m_vShapeDer.resize(nShapeDers);

                for (unsigned int k = 0; k < persObj->m_vShapeSize; ++k){
                    transObj->m_subset[i].second[j].m_vShape[k] = persObj->m_vShape[shapeIndex];
                    shapeIndex++;
                }
                // Loop over shapeders per channel
                for (unsigned int k = 0; k < persObj->m_vShapeDerSize; ++k){
                    transObj->m_subset[i].second[j].m_vShapeDer[k] = persObj->m_vShapeDer[shapederIndex];
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
    
    if (ncorrs) {
        // corrs exist - resize vector
        std::vector<float> vShape(nShapes,0.0);
        std::vector<float> vShapeDer(nShapeDers,0.0);
        LArShapeP1  larShapeP1(vShape, vShapeDer);

        transObj->m_correctionVec.resize(ncorrs, LArShapeTransType1::CorrectionPair(0, larShapeP1));
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
                << endreq;
            return;
        }
        transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
        // Loop over shapes per channel
        for (unsigned int k = 0; k < persObj->m_vShapeSize; ++k){
            transObj->m_correctionVec[i].second.m_vShape[k] = persObj->m_vShape[shapeIndex];
            shapeIndex++;
        }
        // Loop over shapeders per channel
        for (unsigned int k = 0; k < persObj->m_vShapeDerSize; ++k){
            transObj->m_correctionVec[i].second.m_vShapeDer[k] = persObj->m_vShapeDer[shapederIndex];
            shapederIndex++;
        }
    }

    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType;

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
    unsigned int nfebs            = transObj->m_subset.size();
    unsigned int nsubsetsNotEmpty = 0;
    unsigned int ncorrs           = transObj->m_correctionVec.size();
    unsigned int nchans           = 0;
    unsigned int nShapes          = 0;
    unsigned int nShapeDers       = 0;
    bool foundNShapes             = false;
    std::vector<unsigned int> febsWithSparseData;

    // Find the number of shapes/shapeders and check for sparse
    // conditions, e.g. MC conditions
    for (unsigned int i = 0; i < nfebs; ++i){

        unsigned int nfebChans = transObj->m_subset[i].second.size();

        if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
            log << MSG::ERROR 
                << "LArShapeSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans
                << endreq;
            return;
        }
        if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty subsets

        // Loop over channels and check if this subset has sparse data
        bool subsetIsSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) {
            const LArShapeP1& shape = transObj->m_subset[i].second[j];
            if (shape.m_vShape.size() == 0) {
                if (!subsetIsSparse) {
                    // save febids for sparse subsets
                    subsetIsSparse = true;
                    febsWithSparseData.push_back(transObj->m_subset[i].first);
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
    if (!foundNShapes) {
        // Save the number of shapes and derivatives from first
        // correct - couldn't find it from channels
            const LArShapeP1& shape = transObj->m_correctionVec[0].second;
            nShapes                 = shape.m_vShape.size();
            nShapeDers              = shape.m_vShapeDer.size();
    }
    if (nShapes == 0 && nShapeDers == 0) {
        log << MSG::ERROR 
            << "LArShapeSubsetCnv_p1::transToPers - cannot get number of shapes and shape derivatives" 
            << endreq;
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
            
        // Now loop over channels and save shapes/shapeders and
        // channel number
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
        for (unsigned int j = 0; j < nfebChans; ++j){

            bool saveShapes = true;
            if (isSparse) {
                // subset with sparse data

                if (transObj->m_subset[i].second[j].m_vShape.size() > 0) {
                    // store the channel number in bit map
                    if (j < chansOffset || (j - chansOffset) > 31) {
                        log << MSG::ERROR 
                            << "LArShapeSubsetCnv_p1::transToPers - incorrect channel indexing: j, chansOffset: " << j << " " << chansOffset
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
                // save shapes
                for (unsigned int k = 0; k < nShapes; ++k){
                    persObj->m_vShape.push_back(transObj->m_subset[i].second[j].m_vShape[k]);
                }
                // save shapeders
                for (unsigned int k = 0; k < nShapeDers; ++k){
                    persObj->m_vShapeDer.push_back(transObj->m_subset[i].second[j].m_vShapeDer[k]);
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
    for (unsigned int i = 0; i < ncorrs; ++i){
        // Save channel id in febid vector
        persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);
        // Shapes
        for (unsigned int k = 0; k < nShapes; ++k){
            persObj->m_vShape.push_back(transObj->m_correctionVec[i].second.m_vShape[k]);
        }
        // ShapeDers
        for (unsigned int k = 0; k < nShapeDers; ++k){
            persObj->m_vShapeDer.push_back(transObj->m_correctionVec[i].second.m_vShapeDer[k]);
        }
    }

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->m_gain; 
    persObj->m_subset.m_channel       = transObj->m_channel;
    persObj->m_subset.m_groupingType  = transObj->m_groupingType;
    
}
