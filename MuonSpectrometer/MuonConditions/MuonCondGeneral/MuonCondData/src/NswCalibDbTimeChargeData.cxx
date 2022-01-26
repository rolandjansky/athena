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
NswCalibDbTimeChargeData::setData(const std::string& type, const Identifier* chnlId, const double slope, const double slopeError, const double intercept, const double interceptError) {
	if(type!="TDO" && type!="PDO") return;
	if(m_data.find(type) == m_data.end()){
		std::map<unsigned long long, std::vector<double> > empty;
		m_data[type] = empty;
	}
	unsigned long long channelId = chnlId->get_compact();
	if(m_data[type].find(channelId) != m_data[type].end()) return;
	std::vector<double> empty;
	m_data[type][channelId] = empty;
	m_data[type][channelId].push_back(slope         );
	m_data[type][channelId].push_back(slopeError    );
	m_data[type][channelId].push_back(intercept     );
	m_data[type][channelId].push_back(interceptError);
}



// retrieval functions -------------------------------

// getChannelIds
std::vector<Identifier>
NswCalibDbTimeChargeData::getChannelIds(const std::string& type, const std::string tech, const std::string side) const {
	std::vector<Identifier> chnls;
	if(type!="TDO" && type!="PDO") return chnls;
	if(m_data.find(type) == m_data.end()) return chnls;
	std::vector<Identifier> keys;
        for (const auto& p : m_data.at(type)) {
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

// getSlope
bool
NswCalibDbTimeChargeData::getSlope(const std::string& type, const Identifier* chnlId, double& slope) const {
	if(type!="TDO" && type!="PDO") return false;
	if(m_data.find(type) == m_data.end()) return false;
	unsigned long long channelId = chnlId->get_compact();
	if(m_data.at(type).find(channelId)      == m_data.at(type).end()) return false;
	if(m_data.at(type).at(channelId).size() != 4                    ) return false;
	slope = m_data.at(type).at(channelId).at(0);
	return true;
}

// getSlopeError
bool
NswCalibDbTimeChargeData::getSlopeError(const std::string& type, const Identifier* chnlId, double& slopeError) const {
	if(type!="TDO" && type!="PDO") return false;
	if(m_data.find(type) == m_data.end()) return false;
	unsigned long long channelId = chnlId->get_compact();
	if(m_data.at(type).find(channelId)      == m_data.at(type).end()) return false;
	if(m_data.at(type).at(channelId).size() != 4                    ) return false;
	slopeError = m_data.at(type).at(channelId).at(1);
	return true;
}

// getIntercept
bool
NswCalibDbTimeChargeData::getIntercept(const std::string& type, const Identifier* chnlId, double& intercept) const {
	if(type!="TDO" && type!="PDO") return false;
	if(m_data.find(type) == m_data.end()) return false;
	unsigned long long channelId = chnlId->get_compact();
	if(m_data.at(type).find(channelId)      == m_data.at(type).end()) return false;
	if(m_data.at(type).at(channelId).size() != 4                    ) return false;
	intercept = m_data.at(type).at(channelId).at(2);
	return true;
}

// getInterceptError
bool
NswCalibDbTimeChargeData::getInterceptError(const std::string& type, const Identifier* chnlId, double& interceptError) const {
	if(type!="TDO" && type!="PDO") return false;
	if(m_data.find(type) == m_data.end()) return false;
	unsigned long long channelId = chnlId->get_compact();
	if(m_data.at(type).find(channelId)      == m_data.at(type).end()) return false;
	if(m_data.at(type).at(channelId).size() != 4                    ) return false;
	interceptError = m_data.at(type).at(channelId).at(3);
	return true;
}



