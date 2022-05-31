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
	std::vector<double> empty; // storing as vector is not optimal, but keep for now in case we'll add more data in the future
	m_data[channelId] = empty;
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
	std::vector<Identifier> chnls;
	std::vector<Identifier> keys;
        for (const auto& p : m_data) {
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

// getThreshold
bool
NswCalibDbThresholdData::getThreshold(const Identifier& chnlId, double& threshold) const {
	unsigned long long channelId = chnlId.get_compact();
	if(m_data.find(channelId) == m_data.end()) {
        // if channelId doesn't exist in buffer, use 0 channel data ("all channels"), if exists
        ThrsldTechType tech = (m_stgcIdHelper.is_stgc(chnlId))? ThrsldTechType::STGC : ThrsldTechType::MM;
        if(m_zero.find(tech) == m_zero.end()) return false;
        threshold = m_zero.at(tech);
        return true;
	}
	if(m_data.at(channelId).size() != 1           ) return false;
	threshold = m_data.at(channelId).at(0);
	return true;
}



