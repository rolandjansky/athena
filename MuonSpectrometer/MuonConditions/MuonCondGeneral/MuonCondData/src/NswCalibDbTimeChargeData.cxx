/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/NswCalibDbTimeChargeData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "Identifier/Identifier.h"

std::ostream& operator<<(std::ostream& ostr, const NswCalibDbTimeChargeData::CalibConstants& obj) {
    ostr<<" valid channel: "<< (obj.is_valid ? "si" : "no");
    ostr<<" slope: "<<std::setprecision(15)<<obj.slope<<" pm "<<std::setprecision(15)<<obj.slopeError;
    ostr<<" intercept: "<<std::setprecision(15)<<obj.intercept<<" pm "<<std::setprecision(15)<<obj.interceptError;
    return ostr;
}

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
NswCalibDbTimeChargeData::getChannelIds(const CalibDataType type, const std::string& tech, const std::string& side) const {
	if(m_data.find(type) == m_data.end()) {
        return {};
	}
    std::vector<Identifier> keys;
	keys.reserve(m_data.size());
	for (const auto& p : m_data.at(type)) {
		keys.emplace_back(p.first);
	}
	if(tech.empty() && side.empty()) return keys;
	std::vector<Identifier> chnls;
	for(unsigned int i=0; i<keys.size(); ++i){
		bool isMM = m_mmIdHelper.is_mm(keys[i]);
		bool isSTGC = m_stgcIdHelper.is_stgc(keys[i]);
		int eta = (isSTGC)? m_stgcIdHelper.stationEta(keys[i]) : m_mmIdHelper.stationEta(keys[i]);		
		if(!isSTGC && tech == "STGC")  continue;
		if(!isMM && tech == "MM") continue;
		if(eta<=0 && side == "A") continue;
		if(eta>=0 && side == "C") continue;	
		chnls.push_back(keys[i]);
	}
	return chnls;
}
const NswCalibDbTimeChargeData::CalibConstants& NswCalibDbTimeChargeData::getCalibForChannel(const CalibDataType type, const Identifier& channelId) const {
	// search for data for this channel
	std::map<CalibDataType, ChannelCalibMap>::const_iterator itr = m_data.find(type);
	if (itr != m_data.end()) {
		const ChannelCalibMap& channelMap = itr->second;
		ChannelCalibMap::const_iterator chan_itr = channelMap.find(channelId.get_compact());
		if (chan_itr != channelMap.end()) return chan_itr->second;
	}
	// search for data for channel zero
	const CalibTechType tech = (m_stgcIdHelper.is_stgc(channelId))? CalibTechType::STGC : CalibTechType::MM;
	return getZeroCalibChannel(type, tech);		

}
const NswCalibDbTimeChargeData::CalibConstants& NswCalibDbTimeChargeData::getZeroCalibChannel(const CalibDataType type, const CalibTechType tech) const{
	static const CalibConstants dummy{};
	std::map<CalibTechType, ZeroCalibMap>::const_iterator itr = m_zero.find(tech);
	if(itr != m_zero.end()) {
		const ZeroCalibMap& zeroMap = itr->second;
		ZeroCalibMap::const_iterator type_itr = zeroMap.find(type);
		if(type_itr != zeroMap.end()) return type_itr->second;
	}
	return dummy;
}


