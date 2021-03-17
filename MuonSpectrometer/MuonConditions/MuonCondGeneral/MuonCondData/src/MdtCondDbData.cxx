/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/MdtCondDbData.h"
#include "MuonIdHelpers/MdtIdHelper.h"

// --- writing identifiers -------
MdtCondDbData::MdtCondDbData(const MdtIdHelper& id_helper):
    m_id_helper(id_helper){}
// setDeadTube
void
MdtCondDbData::setDeadTube(const std::string& id_name, Identifier Id){
    if(std::find(m_cachedDeadTubesId.begin(), m_cachedDeadTubesId.end(), Id)!=m_cachedDeadTubesId.end()) return;
    m_cachedDeadTubes  .push_back(id_name);
    m_cachedDeadTubesId.push_back(Id  );
}

// setDeadLayer
void
MdtCondDbData::setDeadLayer(const std::string& id_name, Identifier Id){
    if(std::find(m_cachedDeadLayersId.begin(), m_cachedDeadLayersId.end(), Id)!=m_cachedDeadLayersId.end()) return;
    m_cachedDeadLayers  .push_back(id_name);
    m_cachedDeadLayersId.push_back(Id  );
}

// setDeadMultilayer
void
MdtCondDbData::setDeadMultilayer(const std::string& id_name, Identifier Id){
    if(std::find(m_cachedDeadMultilayersId.begin(), m_cachedDeadMultilayersId.end(), Id)!=m_cachedDeadMultilayersId.end()) return;
    m_cachedDeadMultilayers  .push_back(id_name);
    m_cachedDeadMultilayersId.push_back(Id  );
}

// setDeadStation (= a chamber dead by itself)
void
MdtCondDbData::setDeadStation(const std::string& id_name, Identifier Id){
    if(std::find(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), Id)!=m_cachedDeadStationsId.end()) return;
    m_cachedDeadStations  .push_back(id_name);
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

const std::vector<std::string>& MdtCondDbData::getDeadTubes() const{return m_cachedDeadTubes;}
const std::vector<std::string>& MdtCondDbData::getDeadLayers() const{return m_cachedDeadLayers;}
const std::vector<std::string>& MdtCondDbData::getDeadMultilayers() const{ return m_cachedDeadMultilayers;}
const std::vector<std::string>& MdtCondDbData::getDeadStations() const{ return m_cachedDeadStations;}
const std::vector<std::string>& MdtCondDbData::getDeadChambers() const{return m_cachedDeadChambers;}



const std::vector<Identifier>& MdtCondDbData::getDeadTubesId() const{ return m_cachedDeadTubesId;}
const std::vector<Identifier>& MdtCondDbData::getDeadLayersId() const{ return m_cachedDeadLayersId; }
const std::vector<Identifier>& MdtCondDbData::getDeadMultilayersId() const{ return m_cachedDeadMultilayersId;}
const std::vector<Identifier>& MdtCondDbData::getDeadStationsId() const{ return m_cachedDeadStationsId;}
const std::vector<Identifier>& MdtCondDbData::getDeadChambersId() const{ return m_cachedDeadChambersId;}



const std::vector<std::string>& MdtCondDbData::getNoisyTubes() const{ return m_cachedNoisyTubes;}
const std::vector<std::string>& MdtCondDbData::getNoisyLayers() const{ return m_cachedNoisyLayers;}
const std::vector<std::string>& MdtCondDbData::getNoisyMultilayers() const{return m_cachedNoisyMultilayers;}
const std::vector<std::string>& MdtCondDbData::getNoisyStations() const{ return m_cachedNoisyStations;}
const std::vector<std::string>& MdtCondDbData::getNoisyChambers() const{ return m_cachedNoisyChambers;}



const std::vector<Identifier>& MdtCondDbData::getNoisyTubesId() const{ return m_cachedNoisyTubesId;}
const std::vector<Identifier>& MdtCondDbData::getNoisyLayersId() const{return m_cachedNoisyLayersId;}
const std::vector<Identifier>& MdtCondDbData::getNoisyMultilayersId() const{return m_cachedNoisyMultilayersId;}
const std::vector<Identifier>& MdtCondDbData::getNoisyStationsId() const{ return m_cachedNoisyStationsId;}
const std::vector<Identifier>& MdtCondDbData::getNoisyChambersId() const{return m_cachedNoisyChambersId;}



// --- probing identifiers -------

bool 
MdtCondDbData::isGood(const Identifier & Id) const{
    // probing id in all lists
    const Identifier multilayerId = m_id_helper.multilayerID(Id); 
    const Identifier chamberId    = m_id_helper.elementID   (Id); 
    if(not isGoodStation   (chamberId   )) return false;
    if(not isGoodMultilayer(multilayerId)) return false;
    if(not isGoodTube      (Id          )) return false;
    return true;
}
bool MdtCondDbData::isGoodChannel(const MdtIdHelper* , const Identifier & Id) const{ return isGood(Id);}

bool MdtCondDbData::isGoodTube(const Identifier & Id) const{
    if(m_cachedDeadTubesId.empty()) return true;
    bool found = std::find(m_cachedDeadTubesId.begin(), m_cachedDeadTubesId.end(), Id)!=m_cachedDeadTubesId.end();
    return !found;
} 

bool MdtCondDbData::isGoodLayer(const Identifier & Id) const{
    if(m_cachedDeadLayersId.empty()) return true;
    bool found = std::find(m_cachedDeadLayersId.begin(), m_cachedDeadLayersId.end(), Id)!=m_cachedDeadLayersId.end();
    return !found;
} 

bool MdtCondDbData::isGoodMultilayer(const Identifier & Id) const{
    if(m_cachedDeadMultilayersId.empty()) return true;
    bool found = std::find(m_cachedDeadMultilayersId.begin(), m_cachedDeadMultilayersId.end(), Id)!=m_cachedDeadMultilayersId.end();
    return !found;
} 

bool MdtCondDbData::isGoodStation(const Identifier & Id) const{
    if(m_cachedDeadStationsId.empty()) return true;
    bool found = std::find(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), Id)!=m_cachedDeadStationsId.end();
    return !found;
} 

// isGoodChamber
/// this method probably doesn't do what you expect it to - it returns whether there is a
/// bad multilayer/tube inside the chamber, not if the full chamber is disabled. For the
/// latter, you need to use isGoodStation.
bool
MdtCondDbData::isGoodChamber(const Identifier & Id) const{
    if(m_cachedDeadChambersId.empty()) return true;
    bool found = std::find(m_cachedDeadChambersId.begin(), m_cachedDeadChambersId.end(), Id)!=m_cachedDeadChambersId.end();
    return !found;
} 



