/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DetDescrConditions/DetStatusMap.h"
#include "DetDescrCondTPCnv/DetStatusMapCnv_p1.h"

void DetStatusMapCnv_p1::transToPers(const DetStatusMap* trans, DetStatusMap_p1* pers, MsgStream &/*log*/) {

    pers->m_keys.reserve(trans->size());
    pers->m_codes.reserve(trans->size());
    pers->m_deadfrac_thrust.reserve(2*trans->size());

    for (const DetStatusMap::DetStatusPair& pair : *trans) {
        const DetStatus& ds = pair.second;
        pers->m_keys.push_back(pair.first);
        pers->m_codes.push_back(ds.fullCode());
        pers->m_deadfrac_thrust.push_back(ds.deadFrac());
        pers->m_deadfrac_thrust.push_back(ds.deadThrust());
    }
}

void DetStatusMapCnv_p1::persToTrans(const DetStatusMap_p1* pers, DetStatusMap* trans, MsgStream &/*log*/)  {

    trans->clear();
    trans->m_statmap.reserve(pers->m_keys.size());
    unsigned int j = 0;
    for (unsigned int i = 0; i < pers->m_keys.size(); ++i) {
        DetStatus ds(pers->m_codes[i], pers->m_deadfrac_thrust[j], pers->m_deadfrac_thrust[j + 1]);
        trans->m_statmap.emplace_back (pers->m_keys[i], ds);
        j += 2;
    }
}

