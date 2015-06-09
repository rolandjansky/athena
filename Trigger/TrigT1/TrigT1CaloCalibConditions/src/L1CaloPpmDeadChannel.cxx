/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannel.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

L1CaloPpmDeadChannel::L1CaloPpmDeadChannel():
	m_channelId(0),
	m_comment("")
{
}

L1CaloPpmDeadChannel::L1CaloPpmDeadChannel( unsigned int channelId, ChanDeadErrorCode errorCode, std::string comment ):
	m_channelId(channelId),
	m_errorCode(errorCode),
	m_comment(comment)
{
}

L1CaloPpmDeadChannel::L1CaloPpmDeadChannel( const L1CaloCoolChannelId& channelId, ChanDeadErrorCode errorCode, std::string comment ):
	m_channelId(channelId),
	m_errorCode(errorCode),
	m_comment(comment)
{
}

std::ostream& operator<<(std::ostream& output, const  L1CaloPpmDeadChannel& r) {
	output << "channelID: " << std::hex << r.m_channelId.id() << std::dec << ", " << "errorCode: " << r.m_errorCode<< ", " << "comment: " << r.m_comment;
    return output;
}
