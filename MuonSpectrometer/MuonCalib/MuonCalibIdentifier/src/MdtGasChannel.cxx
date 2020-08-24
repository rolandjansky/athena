/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibIdentifier/MdtGasChannel.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibIdentifier/OfflineOnlineIdConversion.h"
#include "PathResolver/PathResolver.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace MuonCalib {

const MdtGasChannel::GasChannel MdtGasChannel::INVALID_CHANNEL = MdtGasChannel::GasChannel(-99, -99);


bool MdtGasChannel::readFile()
	{
	std::string channel_file = PathResolver::find_file("mdt_gas_distribution.txt","DATAPATH");
	if(!channel_file.size())
		{
		return false;
		}
	m_channel_map.clear();
	std::ifstream inf(channel_file.c_str());
	if(!inf.good())
		{
		return false;
		}
	std::string line;
	while(!getline(inf, line).eof())
		{
		size_t pos;
		if((pos=line.find_first_not_of(" \t\n"))==std::string::npos)
			continue;
		if(line[pos]=='#')
			continue;
		std::istringstream lnstr(line);
		std::string chan, stations;
		lnstr>>chan;
		if(lnstr.eof()) continue;
		lnstr>>stations;
		if((pos=chan.find("/"))==std::string::npos) continue;
		chan[pos]=' ';
		std::istringstream chanstr(chan);
		GasChannel c;
		chanstr>>c.first;
		chanstr>>c.second;
		while((pos=stations.find(","))!=std::string::npos)
			{
			stations[pos]=' ';
			}
		std::istringstream st_stream(stations);
		std::string station;
		while(1)
			{
			if(st_stream.eof()) break;
			st_stream>>station;
			MuonFixedId ml=OnlineToOffline(station);
			if(!ml.isValid()) continue;
			m_channel_map[ml]=c;
		//treat connected chambers
			if(ml.stationName()==15 && std::abs(ml.eta())==5 && (ml.phi()==1 || ml.phi()==5))
				{
				ml.setStationEta(ml.eta()==-5?-4:4);
				m_channel_map[ml]=c;
				}
			else if (ml.stationName()==15 && std::abs(ml.eta())==2)
				{
				ml.setStationEta(ml.eta()==-2?-3:3);
				m_channel_map[ml]=c;
				}
			}
		}
	return true;
	}

const  MdtGasChannel::GasChannel & MdtGasChannel::GetGasChannel(const MuonFixedId &id) const
	{
	std::map<MuonFixedId, GasChannel>::const_iterator it=m_channel_map.find(id);
	if(it==m_channel_map.end())
		{
		if(m_warning_printed.find(id)==m_warning_printed.end()) {
			MsgStream log(Athena::getMessageSvc(),"MdtGasChannel");
			log<<MSG::WARNING<<"Invalid Gas channel for "<<id.stationNameString()<<" "<<id.phi()<<" "<<id.eta()<<" "<<id.mdtMultilayer()<<endmsg;
			m_warning_printed.insert(id);
		}
		return INVALID_CHANNEL;
		}
	return it->second;
	}

}
