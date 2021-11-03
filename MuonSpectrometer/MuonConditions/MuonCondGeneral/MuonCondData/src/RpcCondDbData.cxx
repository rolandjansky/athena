/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/RpcCondDbData.h"

// constructor
RpcCondDbData::RpcCondDbData(){
}


// --- writing identifiers -------

// setDeadPanel
void
RpcCondDbData::setDeadPanel(const std::string& name, Identifier Id){
    if(std::find(m_cachedDeadPanelsId.begin(), m_cachedDeadPanelsId.end(), Id)!=m_cachedDeadPanelsId.end()) return;
    m_cachedDeadPanels  .push_back(name);
    m_cachedDeadPanelsId.push_back(Id  );
}

// setDeadStrip
void
RpcCondDbData::setDeadStrip(Identifier stripId){
    if(std::find(m_cachedDeadStrips.begin(), m_cachedDeadStrips.end(), stripId)!=m_cachedDeadStrips.end()) return;
    m_cachedDeadStrips.push_back(stripId);
}

// setDeadStripInt
void
RpcCondDbData::setDeadStripInt(Identifier chamberId, int striplist){
    if(m_cachedDeadStripListInt.count(chamberId)) return;
    m_cachedDeadStripListInt[chamberId] = striplist;
}

// setDeadStripList
void
RpcCondDbData::setDeadStripList(Identifier chamberId, const std::string& striplist){
    if(m_cachedDeadStripList.count(chamberId)) return;
    m_cachedDeadStripList[chamberId] = std::move(striplist);
}

// setEfficiency
void
RpcCondDbData::setEfficiency(Identifier chamberId, double efficiency){
    if(m_cachedEfficiency.count(chamberId)) return;
    m_cachedEfficiency[chamberId] = efficiency;
}

// setFracClusterSize1
void
RpcCondDbData::setFracClusterSize1(Identifier chamberId, double fcs){
    if(m_cachedFracClusterSize1.count(chamberId)) return;
    m_cachedFracClusterSize1[chamberId] = fcs;
}

// setFracClusterSize2
void
RpcCondDbData::setFracClusterSize2(Identifier chamberId, double fcs){
    if(m_cachedFracClusterSize2.count(chamberId)) return;
    m_cachedFracClusterSize2[chamberId] = fcs;
}

// setFracClusterSize3
void
RpcCondDbData::setFracClusterSize3(Identifier chamberId, double fcs){
    if(m_cachedFracClusterSize3.count(chamberId)) return;
    m_cachedFracClusterSize3[chamberId] = fcs;
}

// setFracDeadStrip
void
RpcCondDbData::setFracDeadStrip(Identifier chamberId, double fds){
    if(m_cachedFracDeadStrip.count(chamberId)) return;
    m_cachedFracDeadStrip[chamberId] = fds;
}

// setGapEfficiency
void
RpcCondDbData::setGapEfficiency(Identifier chamberId, double efficiency){
    if(m_cachedGapEfficiency.count(chamberId)) return;
    m_cachedGapEfficiency[chamberId] = efficiency;
}

// setLowEffPanel
void
RpcCondDbData::setLowEffPanel(Identifier chamberId){
    if(std::find(m_cachedLowEffPanels.begin(), m_cachedLowEffPanels.end(), chamberId)!=m_cachedLowEffPanels.end()) return;
    m_cachedLowEffPanels.push_back(chamberId);
}

// setMeanClusterSize
void
RpcCondDbData::setMeanClusterSize(Identifier chamberId, double mcs){
    if(m_cachedMeanClusterSize.count(chamberId)) return;
    m_cachedMeanClusterSize[chamberId] = mcs;
}

// setOffPanel
void
RpcCondDbData::setOffPanel(const std::string& name, Identifier Id){
    if(std::find(m_cachedOffPanelsId.begin(), m_cachedOffPanelsId.end(), Id)!=m_cachedOffPanelsId.end()) return;
    m_cachedOffPanels  .push_back(name);
    m_cachedOffPanelsId.push_back(Id  );
}

// setProjectedTrack
void
RpcCondDbData::setProjectedTrack(Identifier chamberId, int projectedTracks){
    if(m_cachedProjectedTracks.count(chamberId)) return;
    m_cachedProjectedTracks[chamberId] = projectedTracks;
}

// setStripTime
void
RpcCondDbData::setStripTime(Identifier stripId, const std::vector<double>& time){
    if(m_cachedStripTime.count(stripId)) return;
    m_cachedStripTime[stripId] = std::move(time);
}






// --- reading identifiers -------


// getDeadPanels
const std::vector<std::string>& 
RpcCondDbData::getDeadPanels() const{
    if(m_cachedDeadPanels.size()!=0) return m_cachedDeadPanels;
    return m_emptyNames;
}

// getDeadPanelsId
const std::vector<Identifier>& 
RpcCondDbData::getDeadPanelsId() const{
    if(m_cachedDeadPanelsId.size()!=0) return m_cachedDeadPanelsId;
    return m_emptyIds;
}

// getDeadStrips
const std::vector<Identifier>& 
RpcCondDbData::getDeadStrips() const{
    if(m_cachedDeadStrips.size()!=0) return m_cachedDeadStrips;
    return m_emptyIds;
}

// getLowEffPanels
const std::vector<Identifier>& 
RpcCondDbData::getLowEffPanels() const{
    if(m_cachedLowEffPanels.size()!=0) return m_cachedLowEffPanels;
    return m_emptyIds;
}

// getOffPanels
const std::vector<std::string>& 
RpcCondDbData::getOffPanels() const{
    if(m_cachedOffPanels.size()!=0) return m_cachedOffPanels;
    return m_emptyNames;
}

// getOffPanelsId
const std::vector<Identifier>& 
RpcCondDbData::getOffPanelsId() const{
    if(m_cachedOffPanelsId.size()!=0) return m_cachedOffPanelsId;
    return m_emptyIds;
}





// getDeadStripInt
const int&
RpcCondDbData::getDeadStripInt(const Identifier & Id) {
	if(m_cachedDeadStripListInt.find(Id)==m_cachedDeadStripListInt.end()) return m_nillint;
	return m_cachedDeadStripListInt[Id];
} 

// getDeadStripList
const std::string&
RpcCondDbData::getDeadStripList(const Identifier & Id) {
	if(m_cachedDeadStripList.find(Id)==m_cachedDeadStripList.end()) return m_nillstr;
	return m_cachedDeadStripList[Id];
} 

// getEfficiency
const double&
RpcCondDbData::getEfficiency(const Identifier & Id) {
	if(m_cachedEfficiency.find(Id)==m_cachedEfficiency.end()) return m_nill;
	return m_cachedEfficiency[Id];
} 

// getFracClusterSize1
const double&
RpcCondDbData::getFracClusterSize1(const Identifier & Id) {
	if(m_cachedFracClusterSize1.find(Id)==m_cachedFracClusterSize1.end()) return m_nill;
	return m_cachedFracClusterSize1[Id];
} 

// getFracClusterSize2
const double&
RpcCondDbData::getFracClusterSize2(const Identifier & Id) {
	if(m_cachedFracClusterSize2.find(Id)==m_cachedFracClusterSize2.end()) return m_nill;
	return m_cachedFracClusterSize2[Id];
} 

// getFracClusterSize3
const double&
RpcCondDbData::getFracClusterSize3(const Identifier & Id) {
	if(m_cachedFracClusterSize3.find(Id)==m_cachedFracClusterSize3.end()) return m_nill;
	return m_cachedFracClusterSize3[Id];
} 

// getFracDeadStrip
const double&
RpcCondDbData::getFracDeadStrip(const Identifier & Id) {
	if(m_cachedFracDeadStrip.find(Id)==m_cachedFracDeadStrip.end()) return m_nill;
	return m_cachedFracDeadStrip[Id];
} 

// getGapEfficiency
const double&
RpcCondDbData::getGapEfficiency(const Identifier & Id) {
	if(m_cachedGapEfficiency.find(Id)==m_cachedGapEfficiency.end()) return m_nill;
	return m_cachedGapEfficiency[Id];
} 

// getMeanClusterSize
const double&
RpcCondDbData::getMeanClusterSize(const Identifier & Id) {
	if(m_cachedMeanClusterSize.find(Id)==m_cachedMeanClusterSize.end()) return m_nill;
	return m_cachedMeanClusterSize[Id];
} 

// getProjectedTrack
const int&
RpcCondDbData::getProjectedTrack(const Identifier & Id) {
	if(m_cachedProjectedTracks.find(Id)==m_cachedProjectedTracks.end()) return m_nillint;
	return m_cachedProjectedTracks[Id];
} 

// getStripTime
const std::vector<double>& 
RpcCondDbData::getStripTime(const Identifier & Id) {
	if(m_cachedStripTime.find(Id)==m_cachedStripTime.end()) return m_emptyDoubles;
	return m_cachedStripTime[Id];
} 




// --- probing identifiers -------

// isGood
bool 
RpcCondDbData::isGood(const Identifier & Id) const{
    // probing id in all lists
    if(not isGoodPanel(Id)) return false;
    if(    isOffPanel (Id)) return false;
    if(not isGoodStrip(Id)) return false;
    return true;
}

// isGoodPanel
bool 
RpcCondDbData::isGoodPanel(const Identifier & Id) const{
    if(m_cachedDeadPanelsId.size()==0) return true;
    bool found = std::find(m_cachedDeadPanelsId.begin(), m_cachedDeadPanelsId.end(), Id)!=m_cachedDeadPanelsId.end();
    return !found;
} 

// isGoodStrip
bool 
RpcCondDbData::isGoodStrip(const Identifier & Id) const{
    if(m_cachedDeadStrips.size()==0) return true;
    bool found = std::find(m_cachedDeadStrips.begin(), m_cachedDeadStrips.end(), Id)!=m_cachedDeadStrips.end();
    return !found;
} 

// isOffPanel
bool 
RpcCondDbData::isOffPanel(const Identifier & Id) const{
    if(m_cachedOffPanelsId.size()==0) return true;
    bool found = std::find(m_cachedOffPanelsId.begin(), m_cachedOffPanelsId.end(), Id)!=m_cachedOffPanelsId.end();
    return found;
} 

// isEffPanel
bool
RpcCondDbData::isEffPanel(const Identifier & Id) const{
    if(m_cachedLowEffPanels.size()==0) return true;
    bool found = std::find(m_cachedLowEffPanels.begin(), m_cachedLowEffPanels.end(), Id)!=m_cachedLowEffPanels.end();
    return !found;
} 


