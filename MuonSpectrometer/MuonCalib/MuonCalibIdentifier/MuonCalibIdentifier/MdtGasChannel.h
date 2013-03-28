/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_MdtGasChannel_h
#define  MuonCalib_MdtGasChannel_h

#include "string"
#include "map"
#include "set"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "iostream"

namespace MuonCalib {

//class MuonFixedId;

class MdtGasChannel
	{
	public:
		typedef std::pair<int, int> GasChannel;
		inline MdtGasChannel() 
			{
			if(!readFile())
				{
				std::cout<<"ERROR: Gas channel file not found!"<<std::endl;
				}
			}
		const GasChannel & GetGasChannel(const MuonFixedId &id) const;
		static const GasChannel INVALID_CHANNEL;
	private:
		bool readFile();
		std::map<MuonFixedId, GasChannel> m_channel_map;
		mutable std::set<MuonFixedId> m_warning_printed;
	};

}

#endif
