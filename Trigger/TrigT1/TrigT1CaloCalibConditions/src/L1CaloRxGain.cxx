/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRxGain.h"

#include <sstream>
#include <iostream>

// AttributeList
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/MsgStream.h"


//L1CaloRxGain::L1CaloRxGain():
//	m_channelId(0),
//	m_identifier(0),
//	m_recCrate(""),
//	m_recSlot(0),
//	m_recChannel(0),
//	m_gain(0),
//	m_comment("")
//
//{
//}
//
//L1CaloRxGain::L1CaloRxGain( const L1CaloRxCoolChannelId& channelId, unsigned int identifier, std::string crate, unsigned int slot, unsigned int channel, int gain, std::string comment ):
//	m_channelId(channelId),
//	m_identifier(identifier),
//	m_recCrate(crate),
//	m_recSlot(slot),
//	m_recChannel(channel),
//	m_gain(gain),
//	m_comment(comment)
//{
//}


L1CaloRxGain::L1CaloRxGain():
	m_channelId(0),
	m_gain(0),
	m_comment("")
{
}

L1CaloRxGain::L1CaloRxGain( unsigned int channelId, int gain, std::string comment ):
	m_channelId(channelId),
	m_gain(gain),
	m_comment(comment)
{
}

L1CaloRxGain::L1CaloRxGain( const L1CaloRxCoolChannelId& channelId, int gain, std::string comment ):
	m_channelId(channelId),
	m_gain(gain),
	m_comment(comment)
{
}

std::ostream& operator<<(std::ostream& output, const  L1CaloRxGain& r) {
	//output << "channelID: " << r.m_channelId.id() << ", " << "identifier: " << r.m_identifier<< ", " << "crate: " << r.m_recCrate<< ", " << "slot: " << r.m_recSlot<< ", " << "channel: " << r.m_recChannel << ", " << "gain: " << r.m_gain << ", " << "comment: " << r.m_comment.c_str();
	output << "channelID: " << r.m_channelId.id() << ", " << "gain: " << r.m_gain << ", " << "comment: " << r.m_comment.c_str();
    return output;
}
