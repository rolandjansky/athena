/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRD_COLLECTION_P2_H
#define MUONPRD_COLLECTION_P2_H

#include <vector>
namespace Muon{
    /** @brief Class to hold the persistent representation of MuonPRD_Collection.
    It's similar in design to the InDet classes, and therefore uses a very different structure to the transient class.
    No actual PRDs are contained in here, as the PRDs are contained in the container. Instead, here we just have the size 
    of this collection in the container's collection of PRDs, along with the hash of this collection. 
    See the convertor documentations for more information, e.g. 
       - CscPrepDataContainerCnv_p2, 
       - CscStripPrepDataContainerCnv_p2, 
       - RpcPrepDataContainerCnv_p2,
       - TgcPrepDataContainerCnv_p2, 
       - MdtPrepDataContainerCnv_p2
    */
    class MuonPRD_Collection_p2 
{
public: 
    MuonPRD_Collection_p2()
        : 
        m_id(0),
        m_hashId(0),
        m_size(0)
    {} 

    //@{ container cnvs do conversions
    friend class CscPrepDataContainerCnv_p2;
    friend class CscStripPrepDataContainerCnv_p2;
    friend class RpcPrepDataContainerCnv_p2;
    friend class TgcPrepDataContainerCnv_p2;
    friend class TgcPrepDataContainerCnv_p3;
    friend class MdtPrepDataContainerCnv_p2;
    //}@

    // 
    /** Identifier of this collection.
    @todo Ideally we wouldn't write this out, and would get the Id from the hash, but this doesn't seem to be easy to do with the current MdtIdHelper.*/ 
    unsigned int   m_id;

    /** Hash Identifier of this collection */ 
    unsigned int m_hashId;    

    /** Collection size into master collection
        Note I use a short. If a collection has more than 2^16 elements (unlikely) it 
        has to be split into two separate collections*/
    unsigned short m_size;
};

}

#endif


