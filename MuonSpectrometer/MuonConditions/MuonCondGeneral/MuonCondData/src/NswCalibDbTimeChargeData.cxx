/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/NswCalibDbTimeChargeData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "Identifier/Identifier.h"



// general functions ---------------------------------
NswCalibDbTimeChargeData::NswCalibDbTimeChargeData(const MmIdHelper& mmIdHelper, const sTgcIdHelper& stgcIdHelper):
    m_mmIdHelper(mmIdHelper),
    m_stgcIdHelper(stgcIdHelper)
{
}


// setting functions ---------------------------------

// setData
void
NswCalibDbTimeChargeData::setData(CalibDataType type, const Identifier& chnlId,  CalibConstants constants) {

	unsigned long long channelId = chnlId.get_compact();
    ChannelCalibMap& calibMap = m_data[type];    
	if(calibMap.find(channelId) != calibMap.end()) return;
    constants.is_valid = true;
    calibMap.insert(std::make_pair(channelId, std::move(constants)));	
}

// setZeroData
void
NswCalibDbTimeChargeData::setZero(CalibDataType type, CalibTechType tech,  CalibConstants constants) {
    ZeroCalibMap& calibMap = m_zero[tech];
    constants.is_valid = true;
    calibMap.insert(std::make_pair(type, std::move(constants)));
}



// retrieval functions -------------------------------

// getChannelIds
std::vector<Identifier>
NswCalibDbTimeChargeData::getChannelIds(CalibDataType type, const std::string tech, const std::string side) const {
	std::vector<Identifier> chnls;
	if(m_data.find(type) == m_data.end()) return chnls;
	std::vector<Identifier> keys;
        for (const auto& p : m_data.at(type)) {
		keys.emplace_back(p.first);
	}
	if(tech=="" && side=="") return keys;
	for(unsigned int i=0; i<keys.size(); ++i){
		bool isMM = m_mmIdHelper.is_mm(keys[i]);
		bool isSTGC = m_stgcIdHelper.is_stgc(keys[i]);
		int eta = (isSTGC)? m_stgcIdHelper.stationEta(keys[i]) : m_mmIdHelper.stationEta(keys[i]);
		if(strcmp(tech.c_str(), "STGC")==0 && !isSTGC) continue;
		if(strcmp(tech.c_str(), "MM"  )==0 && !isMM) continue;
		if(strcmp(side.c_str(), "A"   )==0 && eta<=0) continue;
		if(strcmp(side.c_str(), "C"   )==0 && eta>=0) continue;
		chnls.push_back(keys[i]);
	}
	return chnls;
}
const NswCalibDbTimeChargeData::CalibConstants& NswCalibDbTimeChargeData::getCalibForChannel(CalibDataType type, const Identifier& channelId) const {
	// search for data for this channel
	static const CalibConstants dummy{};
	std::map<CalibDataType, ChannelCalibMap>::const_iterator itr = m_data.find(type);
	if (itr == m_data.end()) {
		// search for data for channel 0
		CalibTechType tech = (m_stgcIdHelper.is_stgc(channelId))? CalibTechType::STGC : CalibTechType::MM;
		std::map<CalibTechType, ZeroCalibMap>::const_iterator ztr = m_zero.find(tech);
		if(ztr == m_zero.end()) return dummy;
		const ZeroCalibMap& zeroMap = ztr->second;
		ZeroCalibMap::const_iterator type_ztr = zeroMap.find(type);
		if(type_ztr != zeroMap.end()) return type_ztr->second;
		return dummy;   
	}
	const ChannelCalibMap& channelMap = itr->second;
	ChannelCalibMap::const_iterator chan_itr = channelMap.find(channelId.get_compact());
	if (chan_itr != channelMap.end()) return chan_itr->second;
	return dummy;   
}


