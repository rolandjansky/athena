/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#ifndef MUONCOINDATACOLLECTION_P1_H
#define MUONCOINDATACOLLECTION_P1_H

#include <vector>
namespace Muon{
class MuonCoinDataCollection_p1 
  {
    
    
public:
  
    MuonCoinDataCollection_p1()
            : 
            m_id(0),
            m_hashId(0),
            m_begin(0),
            m_end(0)
        { } ;

    // container cnvs do conversions
    friend class TgcCoinDataContainerCnv_p1;
    
    //private:
  
    /** Identifier of this collection*/
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


