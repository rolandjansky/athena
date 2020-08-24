/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/MdtCondDbData.h"
#include "MuonIdHelpers/MdtIdHelper.h"

// --- writing identifiers -------

// setDeadTube
void
MdtCondDbData::setDeadTube(std::string name, Identifier Id){
    if(std::find(m_cachedDeadTubesId.begin(), m_cachedDeadTubesId.end(), Id)!=m_cachedDeadTubesId.end()) return;
    m_cachedDeadTubes  .push_back(name);
    m_cachedDeadTubesId.push_back(Id  );
}

// setDeadLayer
void
MdtCondDbData::setDeadLayer(std::string name, Identifier Id){
    if(std::find(m_cachedDeadLayersId.begin(), m_cachedDeadLayersId.end(), Id)!=m_cachedDeadLayersId.end()) return;
    m_cachedDeadLayers  .push_back(name);
    m_cachedDeadLayersId.push_back(Id  );
}

// setDeadMultilayer
void
MdtCondDbData::setDeadMultilayer(std::string name, Identifier Id){
    if(std::find(m_cachedDeadMultilayersId.begin(), m_cachedDeadMultilayersId.end(), Id)!=m_cachedDeadMultilayersId.end()) return;
    m_cachedDeadMultilayers  .push_back(name);
    m_cachedDeadMultilayersId.push_back(Id  );
}

// setDeadStation (= a chamber dead by itself)
void
MdtCondDbData::setDeadStation(std::string name, Identifier Id){
    if(std::find(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), Id)!=m_cachedDeadStationsId.end()) return;
    m_cachedDeadStations  .push_back(name);
    m_cachedDeadStationsId.push_back(Id  );
}

// setDeadChamber (= a chamber with dead channels)
void
MdtCondDbData::setDeadChamber(Identifier Id){
    if(std::find(m_cachedDeadChambersId.begin(), m_cachedDeadChambersId.end(), Id)!=m_cachedDeadChambersId.end()) return;
    m_cachedDeadChambersId.push_back(Id  );
}

// setNoisyTube
void
MdtCondDbData::setNoisyTube(Identifier Id){
    if(std::find(m_cachedNoisyTubesId.begin(), m_cachedNoisyTubesId.end(), Id)!=m_cachedNoisyTubesId.end()) return;
    m_cachedNoisyTubesId.push_back(Id  );
}

// setNoisyLayer
void
MdtCondDbData::setNoisyLayer(Identifier Id){
    if(std::find(m_cachedNoisyLayersId.begin(), m_cachedNoisyLayersId.end(), Id)!=m_cachedNoisyLayersId.end()) return;
    m_cachedNoisyLayersId.push_back(Id  );
}

// setNoisyMultilayer
void
MdtCondDbData::setNoisyMultilayer(Identifier Id){
    if(std::find(m_cachedNoisyMultilayersId.begin(), m_cachedNoisyMultilayersId.end(), Id)!=m_cachedNoisyMultilayersId.end()) return;
    m_cachedNoisyMultilayersId.push_back(Id  );
}

// setNoisyStation
void
MdtCondDbData::setNoisyStation(Identifier Id){
    if(std::find(m_cachedNoisyStationsId.begin(), m_cachedNoisyStationsId.end(), Id)!=m_cachedNoisyStationsId.end()) return;
    m_cachedNoisyStationsId.push_back(Id  );
}

// setNoisyChamber
void
MdtCondDbData::setNoisyChamber(Identifier Id){
    if(std::find(m_cachedNoisyChambersId.begin(), m_cachedNoisyChambersId.end(), Id)!=m_cachedNoisyChambersId.end()) return;
    m_cachedNoisyChambersId.push_back(Id  );
}





// --- reading identifiers -------

// getDeadTubes
const 
std::vector<std::string>& MdtCondDbData::getDeadTubes() const{
    if(m_cachedDeadTubes.size()!=0) return m_cachedDeadTubes;
    return m_emptyNames;
}

// getDeadLayers
const 
std::vector<std::string>& MdtCondDbData::getDeadLayers() const{
    if(m_cachedDeadLayers.size()!=0) return m_cachedDeadLayers;
    return m_emptyNames;
}

// getDeadMultilayers
const 
std::vector<std::string>& MdtCondDbData::getDeadMultilayers() const{
    if(m_cachedDeadMultilayers.size()!=0) return m_cachedDeadMultilayers;
    return m_emptyNames;
}

// getDeadStations
const 
std::vector<std::string>& MdtCondDbData::getDeadStations() const{
    if(m_cachedDeadStations.size()!=0) return m_cachedDeadStations;
    return m_emptyNames;
}

// getDeadChambers
const 
std::vector<std::string>& MdtCondDbData::getDeadChambers() const{
    if(m_cachedDeadChambers.size()!=0) return m_cachedDeadChambers;
    return m_emptyNames;
}



// getDeadTubesId
const 
std::vector<Identifier>& MdtCondDbData::getDeadTubesId() const{
    if(m_cachedDeadTubesId.size()!=0) return m_cachedDeadTubesId;
    return m_emptyIds;
}

// getDeadLayersId
const 
std::vector<Identifier>& MdtCondDbData::getDeadLayersId() const{
    if(m_cachedDeadLayersId.size()!=0) return m_cachedDeadLayersId;
    return m_emptyIds;
}

// getDeadMultilayersId
const 
std::vector<Identifier>& MdtCondDbData::getDeadMultilayersId() const{
    if(m_cachedDeadMultilayersId.size()!=0) return m_cachedDeadMultilayersId;
    return m_emptyIds;
}

// getDeadStationsId
const 
std::vector<Identifier>& MdtCondDbData::getDeadStationsId() const{
    if(m_cachedDeadStationsId.size()!=0) return m_cachedDeadStationsId;
    return m_emptyIds;
}

// getDeadChambersId
const 
std::vector<Identifier>& MdtCondDbData::getDeadChambersId() const{
    if(m_cachedDeadChambersId.size()!=0) return m_cachedDeadChambersId;
    return m_emptyIds;
}



// getNoisyTubes
const 
std::vector<std::string>& MdtCondDbData::getNoisyTubes() const{
    if(m_cachedNoisyTubes.size()!=0) return m_cachedNoisyTubes;
    return m_emptyNames;
}

// getNoisyLayers
const 
std::vector<std::string>& MdtCondDbData::getNoisyLayers() const{
    if(m_cachedNoisyLayers.size()!=0) return m_cachedNoisyLayers;
    return m_emptyNames;
}

// getNoisyMultilayers
const 
std::vector<std::string>& MdtCondDbData::getNoisyMultilayers() const{
    if(m_cachedNoisyMultilayers.size()!=0) return m_cachedNoisyMultilayers;
    return m_emptyNames;
}

// getNoisyStations
const 
std::vector<std::string>& MdtCondDbData::getNoisyStations() const{
    if(m_cachedNoisyStations.size()!=0) return m_cachedNoisyStations;
    return m_emptyNames;
}

// getNoisyChambers
const 
std::vector<std::string>& MdtCondDbData::getNoisyChambers() const{
    if(m_cachedNoisyChambers.size()!=0) return m_cachedNoisyChambers;
    return m_emptyNames;
}



// getNoisyTubesId
const 
std::vector<Identifier>& MdtCondDbData::getNoisyTubesId() const{
    if(m_cachedNoisyTubesId.size()!=0) return m_cachedNoisyTubesId;
    return m_emptyIds;
}

// getNoisyLayersId
const 
std::vector<Identifier>& MdtCondDbData::getNoisyLayersId() const{
    if(m_cachedNoisyLayersId.size()!=0) return m_cachedNoisyLayersId;
    return m_emptyIds;
}

// getNoisyMultilayersId
const 
std::vector<Identifier>& MdtCondDbData::getNoisyMultilayersId() const{
    if(m_cachedNoisyMultilayersId.size()!=0) return m_cachedNoisyMultilayersId;
    return m_emptyIds;
}

// getNoisyStationsId
const 
std::vector<Identifier>& MdtCondDbData::getNoisyStationsId() const{
    if(m_cachedNoisyStationsId.size()!=0) return m_cachedNoisyStationsId;
    return m_emptyIds;
}

// getNoisyChambersId
const 
std::vector<Identifier>& MdtCondDbData::getNoisyChambersId() const{
    if(m_cachedNoisyChambersId.size()!=0) return m_cachedNoisyChambersId;
    return m_emptyIds;
}



// --- probing identifiers -------

// isGood
bool 
MdtCondDbData::isGood(const Identifier & Id) const{
    // probing id in all lists
    if(not isGoodTube      (Id)) return false;
    if(not isGoodLayer     (Id)) return false;
    if(not isGoodMultilayer(Id)) return false;
    if(not isGoodChamber   (Id)) return false;
    if(not isGoodStation   (Id)) return false;
    return true;
}

// isGoodChannel
bool 
MdtCondDbData::isGoodChannel(const MdtIdHelper* idHelper, const Identifier & Id) const{
    // probing channel and all associated entities
    //Identifier layerId      = idHelper->layerID     (Id); 
    Identifier multilayerId = idHelper->multilayerID(Id); 
    Identifier chamberId    = idHelper->elementID   (Id); 
    //Identifier stationId    = idHelper->stationID   (Id); 
    if(not isGoodTube      (Id          )) return false;
    //if(not isGoodLayer     (layerId     )) return false;
    if(not isGoodMultilayer(multilayerId)) return false;
    // if(not isGoodChamber   (chamberId   )) return false; // will fail if any channel in same chamber is off
    if(not isGoodStation   (chamberId   )) return false;
    return true;
}

// isGoodTube
bool
MdtCondDbData::isGoodTube(const Identifier & Id) const{
    if(m_cachedDeadTubesId.size()==0) return true;
    bool found = std::find(m_cachedDeadTubesId.begin(), m_cachedDeadTubesId.end(), Id)!=m_cachedDeadTubesId.end();
    return !found;
} 

// isGoodLayer
bool
MdtCondDbData::isGoodLayer(const Identifier & Id) const{
    if(m_cachedDeadLayersId.size()==0) return true;
    bool found = std::find(m_cachedDeadLayersId.begin(), m_cachedDeadLayersId.end(), Id)!=m_cachedDeadLayersId.end();
    return !found;
} 

// isGoodMultilayer
bool
MdtCondDbData::isGoodMultilayer(const Identifier & Id) const{
    if(m_cachedDeadMultilayersId.size()==0) return true;
    bool found = std::find(m_cachedDeadMultilayersId.begin(), m_cachedDeadMultilayersId.end(), Id)!=m_cachedDeadMultilayersId.end();
    return !found;
} 

// isGoodStation
bool
MdtCondDbData::isGoodStation(const Identifier & Id) const{
    if(m_cachedDeadStationsId.size()==0) return true;
    bool found = std::find(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), Id)!=m_cachedDeadStationsId.end();
    return !found;
} 

// isGoodChamber
/// this method probably doesn't do what you expect it to - it returns whether there is a
/// bad multilayer/tube inside the chamber, not if the full chamber is disabled. For the
/// latter, you need to use isGoodStation.
bool
MdtCondDbData::isGoodChamber(const Identifier & Id) const{
    if(m_cachedDeadChambersId.size()==0) return true;
    bool found = std::find(m_cachedDeadChambersId.begin(), m_cachedDeadChambersId.end(), Id)!=m_cachedDeadChambersId.end();
    return !found;
} 



