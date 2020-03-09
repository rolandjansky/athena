/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/TgcCondDbData.h"

// constructor
TgcCondDbData::TgcCondDbData(){
}


// --- writing identifiers -------

// setDeadStation
void
TgcCondDbData::setDeadStation(Identifier Id){
    if(std::binary_search(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), Id, Compare)) return;
    m_cachedDeadStationsId.push_back(Id  );
}


// --- reading identifiers -------

// getDeadStationsId
const 
std::vector<Identifier>& TgcCondDbData::getDeadStationsId() const{
    if(m_cachedDeadStationsId.size()!=0) return m_cachedDeadStationsId;
    return m_emptyIds;
}


// --- probing identifiers -------

// isGood
bool 
TgcCondDbData::isGood(const Identifier & Id) const{
    // probing id in all lists
    if(not isGoodStation   (Id)) return false;
    return true;
}

// isGoodStation
bool
TgcCondDbData::isGoodStation(const Identifier & Id) const{
    if(m_cachedDeadStationsId.size()==0) return true;
    bool found = std::binary_search(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), Id, Compare);
    return !found;
} 




