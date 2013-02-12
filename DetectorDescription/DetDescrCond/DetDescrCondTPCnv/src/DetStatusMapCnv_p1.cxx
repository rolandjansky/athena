/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "DetDescrConditions/DetStatusMap.h"
#undef private
#undef protected

#include "DetDescrCondTPCnv/DetStatusMapCnv_p1.h"

void DetStatusMapCnv_p1::transToPers(const DetStatusMap* trans, DetStatusMap_p1* pers, MsgStream &/*log*/) {

    pers->m_keys.reserve(trans->m_statmap.size());
    pers->m_codes.reserve(trans->m_statmap.size());
    pers->m_deadfrac_thrust.reserve(2*trans->m_statmap.size());

    for (unsigned int i = 0; i < trans->m_statmap.size(); ++i) {
        const DetStatus& ds = trans->m_statmap[i].second;
        pers->m_keys.push_back(trans->m_statmap[i].first);
        pers->m_codes.push_back(ds.m_code);
        pers->m_deadfrac_thrust.push_back(ds.m_deadfrac);
        pers->m_deadfrac_thrust.push_back(ds.m_deadthrust);
    }
}

void DetStatusMapCnv_p1::persToTrans(const DetStatusMap_p1* pers, DetStatusMap* trans, MsgStream &/*log*/)  {

    trans->m_statmap.reserve(pers->m_keys.size());
    unsigned int j = 0;
    for (unsigned int i = 0; i < pers->m_keys.size(); ++i) {
        DetStatus ds(pers->m_codes[i], pers->m_deadfrac_thrust[j], pers->m_deadfrac_thrust[j + 1]);
        trans->m_statmap.push_back(DetStatusMap::DetStatusPair(pers->m_keys[i], ds));
        j += 2;
    }
}

