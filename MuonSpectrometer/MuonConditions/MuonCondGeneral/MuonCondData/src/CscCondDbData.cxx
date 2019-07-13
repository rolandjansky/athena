/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/CscCondDbData.h"

//STL includes
#include <vector>
#include <string>


using namespace std;


// constructor
CscCondDbData::CscCondDbData() {
}


// --- writing identifiers -------

// setChannelStatus
void
CscCondDbData::setChannelStatus(IdentifierHash hash, int status){
    m_cachedChannelsStatus[(unsigned int) hash] = status;
    if((status & 0x1) || ((status >> 1) & 0x1)) setDeadChannelHash(hash);
}

// setDeadChannelHash
void
CscCondDbData::setDeadChannelHash(IdentifierHash hash){
    if(std::find(m_cachedDeadChannelsHash.begin(), m_cachedDeadChannelsHash.end(), (unsigned int) hash) != m_cachedDeadChannelsHash.end()) return;
    m_cachedDeadChannelsHash.push_back((unsigned int) hash);
}

// setDeadLayer
void
CscCondDbData::setDeadLayer(std::string name, Identifier Id){
    if(std::find(m_cachedDeadLayersId.begin(), m_cachedDeadLayersId.end(), Id)!=m_cachedDeadLayersId.end()) return;
    m_cachedDeadLayers  .push_back(name);
    m_cachedDeadLayersId.push_back(Id  );
}

// setDeadStation
void
CscCondDbData::setDeadStation(std::string name, Identifier Id){
    if(std::find(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), Id)!=m_cachedDeadStationsId.end()) return;
    m_cachedDeadStations  .push_back(name);
    m_cachedDeadStationsId.push_back(Id  );
}





// --- reading identifiers -------

// getDeadLayers
const 
std::vector<std::string>& CscCondDbData::getDeadLayers() const{
    if(m_cachedDeadLayers.size()!=0) return m_cachedDeadLayers;
    return m_emptyNames;
}

// getDeadStations
const 
std::vector<std::string>& CscCondDbData::getDeadStations() const{
    if(m_cachedDeadStations.size()!=0) return m_cachedDeadStations;
    return m_emptyNames;
}



// getDeadLayersId
const 
std::vector<Identifier>& CscCondDbData::getDeadLayersId() const{
    if(m_cachedDeadLayersId.size()!=0) return m_cachedDeadLayersId;
    return m_emptyIds;
}

// getDeadStationsId
const 
std::vector<Identifier>& CscCondDbData::getDeadStationsId() const{
    if(m_cachedDeadStationsId.size()!=0) return m_cachedDeadStationsId;
    return m_emptyIds;
}



// getDeadChannelsHashs
const 
std::vector<unsigned int>& CscCondDbData::getDeadChannelsHash() const{
    if(m_cachedDeadChannelsHash.size()!=0) return m_cachedDeadChannelsHash;
    return m_emptyHashs;
}



// --- stored info for all -------

// getChannelStatus
const
int& CscCondDbData::getChannelStatus(IdentifierHash hash) const{
	if(m_cachedChannelsStatus.find((unsigned int) hash)==m_cachedChannelsStatus.end()) return m_nillint;
	return m_cachedChannelsStatus.find((unsigned int) hash)->second;
}




// --- probing identifiers -------

// isGood
bool 
CscCondDbData::isGood(const Identifier & Id) const{
    // probing id in all lists
    if(not isGoodLayer     (Id)) return false;
    if(not isGoodStation   (Id)) return false;
    return true;
}

// isGoodChannelHash
bool 
CscCondDbData::isGoodChannelHash(const IdentifierHash & hash) const{
    if(m_cachedDeadChannelsHash.size()==0) return true;
    bool found = std::find(m_cachedDeadChannelsHash.begin(), m_cachedDeadChannelsHash.end(), (unsigned int) hash) != m_cachedDeadChannelsHash.end();
    return !found;
}

// isGoodLayer
bool
CscCondDbData::isGoodLayer(const Identifier & Id) const{
    if(m_cachedDeadLayersId.size()==0) return true;
    bool found = std::find(m_cachedDeadLayersId.begin(), m_cachedDeadLayersId.end(), Id)!=m_cachedDeadLayersId.end();
    return !found;
} 

// isGoodStation
bool
CscCondDbData::isGoodStation(const Identifier & Id) const{
    if(m_cachedDeadStationsId.size()==0) return true;
    bool found = std::find(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), Id)!=m_cachedDeadStationsId.end();
    return !found;
} 



