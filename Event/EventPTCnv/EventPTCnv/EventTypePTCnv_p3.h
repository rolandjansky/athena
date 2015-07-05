/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTPTCNV_EVENTTYPEPTCNV_P3_H
#define EVENTPTCNV_EVENTTYPEPTCNV_P3_H
/**
 * @file EventTypePTCnv_p3.h
 *
 * @brief Header file for in-file metadata getters (file peeking without Athena)
 *
 * @author  <vaniachine@anl.gov>
 */

#define private public
#include "EventTPCnv/EventType_p3.h"
#undef private

class EventTypePTCnv_p3 : EventType_p3 {
public:
    static std::vector<unsigned int> bit_mask (const ::EventType_p3& etp)
    {
        return std::vector<unsigned int> (etp.m_bit_mask.begin(), etp.m_bit_mask.end());
    };
};

#endif // EVENTPTCNV_EVENTTYPEPTCNV_P3_H
