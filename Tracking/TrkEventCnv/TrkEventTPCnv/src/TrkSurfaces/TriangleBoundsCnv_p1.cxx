/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/TrkSurfaces/TriangleBoundsCnv_p1.h"
#include <algorithm>

void TriangleBoundsCnv_p1 :: persToTrans( const Trk :: TriangleBounds_p1 *persObj,
                                                 Trk :: TriangleBounds    *transObj,
                                                 MsgStream                 & )
{
    transObj->m_boundValues.resize(persObj->m_vertices.size());
    std::vector<std::pair<double,double> >::const_iterator it=persObj->m_vertices.begin(), itEnd=persObj->m_vertices.end();
    for (;it!=itEnd;++it) {
      transObj->m_boundValues.push_back(it->first);
      transObj->m_boundValues.push_back(it->second);
    }
}

void TriangleBoundsCnv_p1 :: transToPers( const Trk :: TriangleBounds    * /**transObj*/,
                                                 Trk :: TriangleBounds_p1 * /**persObj*/,
                                                 MsgStream                 & ) 
{
    throw std::runtime_error("TriangleBoundsCnv_p1::transToPers is deprecated!");
}
