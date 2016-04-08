/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DetDescrConditions/DetCondCFloat.h"
#include "DetDescrCondTPCnv/DetCondCFloatCnv_p1.h"

void DetCondCFloatCnv_p1::transToPers(const DetCondCFloat* trans, DetCondCFloat_p1* pers, MsgStream &/*log*/) {

    pers->m_size = trans->size();
    pers->m_tag  = trans->tag();

    DetCondCFloat::BufferMap::const_iterator it   = trans->m_bufmap.begin();
    DetCondCFloat::BufferMap::const_iterator last = trans->m_bufmap.end();
    pers->m_bufKeyValue.reserve(2*trans->m_bufmap.size());
    for (; it != last; ++it) {
        unsigned int key = it->first.get_identifier32().get_compact();
        pers->m_bufKeyValue.push_back(key);
        pers->m_bufKeyValue.push_back(it->second);
    }
    pers->m_buf = trans->m_buf;

}

void DetCondCFloatCnv_p1::persToTrans(const DetCondCFloat_p1* pers, DetCondCFloat* trans, MsgStream &/*log*/)  {

    trans->m_size = pers->m_size;
    trans->m_tag  = pers->m_tag;

    // Copy stored vec into map 
    typedef DetCondCFloat::BufferMap::value_type value_type;
    DetCondCFloat::BufferMap::iterator it   = trans->m_bufmap.begin();
    for (unsigned int i = 0; i < pers->m_bufKeyValue.size(); i += 2) {
        it = trans->m_bufmap.insert(it, value_type(Identifier(pers->m_bufKeyValue[i]), pers->m_bufKeyValue[i + 1]));
    }
    trans->m_buf = pers->m_buf;

}

