/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_MdtGasChannel_h
#define  MuonCalib_MdtGasChannel_h

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "string"
#include "map"
#include "set"
#include "iostream"

namespace MuonCalib {

class MdtGasChannel
	{
	public:
		typedef std::pair<int, int> GasChannel;
		inline MdtGasChannel() {
			if(!readFile()) {
				MsgStream log(Athena::getMessageSvc(),"MdtGasChannel");
				log<<MSG::WARNING<<"Gas channel file not found!"<<endmsg;
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
