// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRDISABLEDCHANNEL_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRDISABLEDCHANNEL_H

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"
#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <iostream>
#include <string>

/**
 *  Transient conditions class for objects defined
 *  by the online framework and retrieved from COOL
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloPprDisabledChannel {
    friend std::ostream& operator<<(std::ostream& output, const L1CaloPprDisabledChannel& r);

 public:

    L1CaloPprDisabledChannel();
    L1CaloPprDisabledChannel( unsigned int channelId );
    L1CaloPprDisabledChannel( const L1CaloCoolChannelId& channelId );
    L1CaloPprDisabledChannel( unsigned int channelId, ChanDeadErrorCode deadErrorCode, ChanCalibErrorCode calibErrorCode, bool isMasked, unsigned int noiseCut, unsigned int disabledBits);
    L1CaloPprDisabledChannel( const L1CaloCoolChannelId& channelId, ChanDeadErrorCode deadErrorCode, ChanCalibErrorCode calibErrorCode, bool isMasked, unsigned int noiseCut, unsigned int disabledBits);

    virtual ~L1CaloPprDisabledChannel() {};

    // getters
    inline L1CaloCoolChannelId channelId(void) const { return m_channelId; };

    inline ChanCalibErrorCode calibErrorCode(void) const { return m_calibErrorCode; };
    inline void calibErrorCode(const ChanCalibErrorCode& calibErrorCode) { m_calibErrorCode = calibErrorCode; };

    inline ChanDeadErrorCode deadErrorCode(void) const { return m_deadErrorCode; };
    inline void deadErrorCode(const ChanDeadErrorCode& deadErrorCode) { m_deadErrorCode = deadErrorCode; };

    inline bool isMasked(void) const { return m_bMasked; };
    inline void isMasked(bool isMasked) { m_bMasked = isMasked; };

    inline bool isDead(void) const { return m_deadErrorCode.chanValid(); };
    inline bool hasCalib(void) const { return m_calibErrorCode.chanValid(); };

    inline unsigned int noiseCut() const { return m_noiseCut; }
    inline void setNoiseCut(unsigned int noiseCut) { m_noiseCut = noiseCut; }

    inline unsigned int disabledBits() const { return m_disabledBits; }
    inline void setDisabledBits(unsigned int disabledBits) { m_disabledBits = disabledBits; }

 private:
    L1CaloCoolChannelId m_channelId;
    ChanCalibErrorCode m_calibErrorCode;
    ChanDeadErrorCode m_deadErrorCode;
    bool m_bMasked;
    unsigned int m_noiseCut;
    unsigned int m_disabledBits;
};

#endif
