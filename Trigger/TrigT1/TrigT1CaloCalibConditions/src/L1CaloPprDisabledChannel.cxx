/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannel.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

L1CaloPprDisabledChannel::L1CaloPprDisabledChannel():
	m_channelId(0),
	m_bMasked(false),
	m_noiseCut(0),
	m_disabledBits(0)
{
}

L1CaloPprDisabledChannel::L1CaloPprDisabledChannel( unsigned int channelId ):
	m_channelId(channelId),
	m_bMasked(false),
	m_noiseCut(0),
	m_disabledBits(0)
{
}

L1CaloPprDisabledChannel::L1CaloPprDisabledChannel( const L1CaloCoolChannelId& channelId ):
	m_channelId(channelId),
	m_bMasked(false),
	m_noiseCut(0),
	m_disabledBits(0)
{
}

L1CaloPprDisabledChannel::L1CaloPprDisabledChannel( unsigned int channelId, ChanDeadErrorCode deadErrorCode, ChanCalibErrorCode calibErrorCode, bool isMasked, unsigned int noiseCut, unsigned int disabledBits):
	m_channelId(channelId),
	m_calibErrorCode(calibErrorCode),
	m_deadErrorCode(deadErrorCode),
	m_bMasked(isMasked),
	m_noiseCut(noiseCut),
	m_disabledBits(disabledBits)
{
}

L1CaloPprDisabledChannel::L1CaloPprDisabledChannel( const L1CaloCoolChannelId& channelId, ChanDeadErrorCode deadErrorCode, ChanCalibErrorCode calibErrorCode, bool isMasked, unsigned int noiseCut, unsigned int disabledBits):
	m_channelId(channelId),
	m_calibErrorCode(calibErrorCode),
	m_deadErrorCode(deadErrorCode),
	m_bMasked(isMasked),
	m_noiseCut(noiseCut),
	m_disabledBits(disabledBits)
{
}

std::ostream& operator<<(std::ostream& output, const  L1CaloPprDisabledChannel& r) {
    output << "channelID: " << std::hex << r.m_channelId.id() << std::dec << ", " << "deadErrorCode: " << r.m_deadErrorCode<< ", " << "calibErrorCode: " << r.m_calibErrorCode << ", " << "masked: " << r.m_bMasked << ", noiseCut: " << r.m_noiseCut << ", disabledBits: " << r.m_disabledBits;
    return output;
}
