/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPRD_COLLECTION_P1_H
#define INDETPRD_COLLECTION_P1_H

#include <vector>
#include "Identifier/Identifier.h"

namespace InDet{
class InDetPRD_Collection_p1 
{
    
  
public:
  
    InDetPRD_Collection_p1()
            : 
            m_id(0),
            m_begin(0),
            m_end(0)
        { } ;

    // container cnvs do conversions
    friend class PixelClusterContainerCnv_p1;
    friend class SCT_ClusterContainerCnv_p1;
    friend class TRT_DriftCircleContainerCnv_p1;
    
    //private:
  
    // Identifier of this collection
    unsigned int m_id;
 
    //Identifier::value_type m_id;

    // Hash Identifier of this collection 
    // (write this one as well, so we don't rely on the IdHelper for it)   
    unsigned int m_hashId;    

    // Begin index into master collection
    unsigned int m_begin;

    // End index into master collection
    unsigned int m_end;
  
};

}

#endif


