/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTPCNV_LARRAWCHANNELCOLLECTION_P2_H
#define LARTPCNV_LARRAWCHANNELCOLLECTION_P2_H

#include "LArTPCnv/LArRawChannel_p1.h"

#include <vector>
/**
@class LArRawChannelCollection_p2
@brief Persistent LArRawChannelCollection, version p2
*/
class LArRawChannelCollection_p2 
{
    
  
public:
  
    LArRawChannelCollection_p2() 
            : 
            m_id(0),
            m_begin(0),
            m_end(0)
        { } ;

    // container cnv does conversion
    friend class LArRawChannelContainerCnv_p2;
    
private:
  
    // Identifier of this collection
    unsigned int m_id;

    // Begin index into master collection
    unsigned int m_begin;

    // End index into master collection
    unsigned int m_end;
  
};

#endif
