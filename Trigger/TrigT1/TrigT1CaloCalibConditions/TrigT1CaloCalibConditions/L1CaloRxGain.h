/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALORXGAIN_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALORXGAIN_H

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"

#include <iostream>
#include <vector>
#include <string>

/**
*  Transient conditions class for objects defined
*  by the online framework and retrieved from COOL
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

class L1CaloRxGain final {
	friend std::ostream& operator<<(std::ostream& output, const L1CaloRxGain& r);
public:

    L1CaloRxGain();
//    L1CaloRxGain( unsigned int channelId, unsigned int identifier, std::string crate, unsigned int slot, unsigned int channel, int gain, std::string comment );
//    L1CaloRxGain( const L1CaloRxCoolChannelId& channelId, unsigned int identifier, std::string crate, unsigned int slot, unsigned int channel, int gain, std::string comment );

    L1CaloRxGain( unsigned int channelId, int gain, const std::string& comment );
    L1CaloRxGain( const L1CaloRxCoolChannelId& channelId, int gain, const std::string& comment );


	~L1CaloRxGain() =default;

	// getters
	inline L1CaloRxCoolChannelId channelId(void) const { return m_channelId; };
	//inline unsigned int identifier(void) const { return m_identifier; };
	//inline std::string crate(void) const { return m_recCrate; };
	//inline unsigned int slot(void) const { return m_recSlot; };
	//inline unsigned int channel(void) const { return m_recChannel; };
	inline int gain(void) const { return m_gain; };
	inline std::string comment(void) const { return m_comment; };

private:
	L1CaloRxCoolChannelId m_channelId;
	//unsigned int m_identifier;
	//std::string m_recCrate;
	//unsigned int m_recSlot;
	//unsigned int m_recChannel;
	int m_gain;
	std::string m_comment;

};

#endif
