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
NswCalibDbThresholdData::setData(const Identifier* chnlId, const double threshold) {
	unsigned long long channelId = chnlId->get_compact();
	if(m_data.find(channelId) != m_data.end()) return;
	std::vector<double> empty; // storing as vector is not optimal, but keep for now in case we'll add more data in the future
	m_data[channelId] = empty;
	m_data[channelId].push_back(threshold);
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
		int tec = m_mmIdHelper.technology(keys[i]);
		int eta = m_mmIdHelper.stationEta(keys[i]);
		if(strcmp(tech.c_str(), "STGC")==0 && tec!=4) continue;
		if(strcmp(tech.c_str(), "MM"  )==0 && tec!=5) continue;
		if(strcmp(side.c_str(), "A"   )==0 && eta<=0) continue;
		if(strcmp(side.c_str(), "C"   )==0 && eta>=0) continue;
		chnls.push_back(keys[i]);
	}
	return chnls;
}

// getThreshold
bool
NswCalibDbThresholdData::getThreshold(const Identifier* chnlId, double& threshold) const {
	unsigned long long channelId = chnlId->get_compact();
	if(m_data.find(channelId)      == m_data.end()) return false;
	if(m_data.at(channelId).size() != 1           ) return false;
	threshold = m_data.at(channelId).at(0);
	return true;
}



