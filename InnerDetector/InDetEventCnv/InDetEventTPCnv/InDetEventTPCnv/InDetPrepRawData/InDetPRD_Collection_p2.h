/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPRD_COLLECTION_P2_H
#define INDETPRD_COLLECTION_P2_H

#include <vector>
namespace InDet{
class InDetPRD_Collection_p2
{
    
  
public:
  
    InDetPRD_Collection_p2()
      : 
      //      m_idDelta(0),
      m_hashId(0),
      m_size(0)
        { } ;

    // container cnvs do conversions
    friend class PixelClusterContainerCnv_p2;
    friend class SCT_ClusterContainerCnv_p2;
    friend class TRT_DriftCircleContainerCnv_p1;
    
    //private:
  
    // Identifier distance of this collection from the previous one
    //    unsigned short m_idDelta;

    // Hash Identifier of this collection 
    // (write this one as well, so we don't rely on the IdHelper for it)   
    unsigned short m_hashId;    

    //  Collection size into master collection
    //  Note I use a short. If a collection has more than 2^16 elements (unlikely) it 
    //  has to be split into two separate collections
    unsigned short m_size;
  
};

}

#endif


