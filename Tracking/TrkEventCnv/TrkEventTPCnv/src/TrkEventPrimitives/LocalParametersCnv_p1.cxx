/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalParametersCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include  <stdexcept>

void LocalParametersCnv_p1 :: persToTrans( const Trk :: LocalParameters_p1 *persObj,
                                                 Trk :: LocalParameters    *transObj,
                                                 MsgStream&)
{
    transObj->m_parameterkey = persObj->m_parameterKey;
    // transObj->readArray(persObj->m_vec);
    unsigned int size = persObj->m_vec.size();
    transObj->resize(size);
    for (unsigned int i=0; i<size;++i)
      (*transObj)(i)=persObj->m_vec[i];
}

void LocalParametersCnv_p1 :: transToPers( const Trk :: LocalParameters    *transObj,
                                                 Trk :: LocalParameters_p1 *persObj,
                                                 MsgStream& )
{
    persObj->m_parameterKey = transObj->m_parameterkey;
    unsigned int size=transObj->size();
    persObj->m_vec.resize(size);
    // persObj->m_vec.insert(persObj->m_vec.begin(),transObj->array(),transObj->array()+size);
    // std::transform(transObj->array(), transObj->array()+size,
    //                    std::back_inserter(persObj->m_vec),
    //                    castToFloat());
    
    for (unsigned int i=0; i<size;++i)
      persObj->m_vec[i]=(*transObj)(i);
}
