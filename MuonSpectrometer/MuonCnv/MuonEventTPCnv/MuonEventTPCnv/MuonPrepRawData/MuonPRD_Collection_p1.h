/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRD_COLLECTION_P1_H
#define MUONPRD_COLLECTION_P1_H

#include <vector>
namespace Muon{
    class MuonPRD_Collection_p1 
{
public: 
    MuonPRD_Collection_p1()
            : 
            m_id(0),
            m_hashId(0),
            m_begin(0),
            m_end(0)
        { } ;

    // container cnvs do conversions
    friend class CscPrepDataContainerCnv_p1;
    friend class CscStripPrepDataContainerCnv_p1;
    friend class RpcPrepDataContainerCnv_p1;
    friend class TgcPrepDataContainerCnv_p1;
    friend class MdtPrepDataContainerCnv_p1;
    
    //private:
  
    /** Identifier of this collection. FIXME - do we need this? Is it not obvious from the context? EJWM. */
    unsigned int m_id;

    /** Hash Identifier of this collection 
    (write this one as well, so we don't rely on the IdHelper for it)  */ 
    unsigned int m_hashId;    

    /** Begin index into master collection*/
    unsigned int m_begin;

    /** End index into master collection*/
    unsigned int m_end;
  
};

}

#endif


