/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/NswCalibDbThresholdData.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "Identifier/Identifier.h"


// general functions ---------------------------------
NswCalibDbThresholdData::NswCalibDbThresholdData(const MmIdHelper& mmIdHelper, const sTgcIdHelper& stgcIdHelper):
    m_mmIdHelper(mmIdHelper),
    m_stgcIdHelper(stgcIdHelper)
{
}


// setting functions ---------------------------------

// setData
void
NswCalibDbThresholdData::setData(const Identifier& chnlId, const double threshold) {
	unsigned long long channelId = chnlId.get_compact();
	if(m_data.find(channelId) != m_data.end()) return;
	// storing as vector is not optimal, but keep for now in case we'll add more data in the future
	m_data[channelId].push_back(threshold);
}

// setZero
void
NswCalibDbThresholdData::setZero(ThrsldTechType tech, const double threshold) {
    m_zero[tech] = threshold;
}


// retrieval functions -------------------------------

// getChannelIds
std::vector<Identifier>
NswCalibDbThresholdData::getChannelIds(const std::string tech, const std::string side) const {
	std::vector<Identifier> keys;
	keys.reserve(m_data.size());
	for (const auto& p : m_data) {
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

// getThreshold
bool
NswCalibDbThresholdData::getThreshold(const Identifier& chnlId, double& threshold) const {
	unsigned long long channelId = chnlId.get_compact();
	ChannelMap::const_iterator chan_itr = m_data.find(channelId);
	/// For the moment require that there is only one channel per identifier
	if(chan_itr != m_data.end() && chan_itr->second.size() == 1) {
		threshold = chan_itr->second[0];
		return true;
	}
	// if channelId doesn't exist in buffer, use 0 channel data ("all channels"), if exists
    const ThrsldTechType tech = (m_stgcIdHelper.is_stgc(chnlId))? ThrsldTechType::STGC : ThrsldTechType::MM;
    
	ZeroMap::const_iterator zero_itr = m_zero.find(tech);
	if (zero_itr != m_zero.end()) {
		threshold = zero_itr->second;
		return true;
	}
	threshold = 0.;
	return false;
}



