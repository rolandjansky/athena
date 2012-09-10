/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNEL_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNEL_H

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <iostream>

/**
*  Transient conditions class for objects defined
*  by the online framework and retrieved from COOL
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

class string;

class L1CaloPpmDeadChannel {
	friend std::ostream& operator<<(std::ostream& output, const L1CaloPpmDeadChannel& r);

public:

    L1CaloPpmDeadChannel();
    L1CaloPpmDeadChannel(unsigned int channelId, ChanDeadErrorCode errorCode, std::string comment);
    L1CaloPpmDeadChannel(const L1CaloCoolChannelId& channelId, ChanDeadErrorCode errorCode, std::string comment);

	virtual ~L1CaloPpmDeadChannel() {};

	// getters
	inline L1CaloCoolChannelId channelId(void) const { return m_channelId; };
	inline ChanDeadErrorCode errorCode(void) const { return m_errorCode; };
	inline std::string comment(void) const { return m_comment; };


private:
	L1CaloCoolChannelId m_channelId;
	ChanDeadErrorCode m_errorCode;
	std::string m_comment;
};

#endif
