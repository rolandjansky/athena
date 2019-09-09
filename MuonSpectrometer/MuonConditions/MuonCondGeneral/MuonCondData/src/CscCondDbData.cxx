/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/CscCondDbData.h"

// constructor
CscCondDbData::CscCondDbData() :
    m_maxChanHash(61440),
    m_maxChamberCoolChannel(32),
    m_maxLayerHash(255){

    // prepare contexts
    m_moduleContext  = m_idHelper->module_context();
    m_channelContext = m_idHelper->channel_context();

    //prepare layer hash array
    int hash = 0;

    for(int stationName  = 0; stationName < 2; stationName++){
      for(int stationEta =0; stationEta <2; stationEta++){
        for(int stationPhi = 0; stationPhi <8; stationPhi++){
          for(int wireLayer = 0; wireLayer <4; wireLayer++){
            for(int measuresPhi = 0; measuresPhi <2; measuresPhi++){
              Identifier id = m_idHelper->channelID(
                  stationName+1,
                  (stationEta? 1:-1),
                  stationPhi+1,
                  2,//only installed chamber layer
                  wireLayer+1,
                  measuresPhi,
                  1 //channel doesn't matter. We'll just use first
                  );
              unsigned int onlineId;
              if(!offlineToOnlineId(id, onlineId).isSuccess()) {
                std::cout << "Failed at geting online id!" << std::endl;
              }
              else {
                m_onlineChannelIdsFromLayerHash.push_back(onlineId);
                m_layerHashes[stationName][stationEta][stationPhi][wireLayer][measuresPhi] = hash++;
              }
            }
          }
        }
      }
    }

    *(const_cast<unsigned int*>(&m_maxLayerHash)) = hash -1; //-1 because hash overshoots in loop


    //prepare chamberCoolChannel. This is similar to the hash array in CscIdHelper.
    //Putting it here because cool channels are dependent on this hash, and it can't
    //ever change, even if idHelper's definition changes
    hash = 1;
    for(int stationName  = 0; stationName < 2; stationName++){
      for(int stationEta =0; stationEta <2; stationEta++){
        for(int stationPhi = 0; stationPhi <8; stationPhi++){
          Identifier id = m_idHelper->channelID(
              stationName+1,
              (stationEta? 1:-1),
              stationPhi+1,
              2,//only installed chamber layer 2
              1,//wirelayer doesn't matter, we'll just use first
              0,//measures phi doesn't matter, we'll just use precision
              1 //channel doesn't matter. We'll just use first
              );
          unsigned int onlineId;
          if(!offlineToOnlineId(id, onlineId).isSuccess()) {
            std::cout << "Failed at geting online id!" << std::endl;
          }
          else {
            m_onlineChannelIdsFromChamberCoolChannel.push_back(onlineId);
            m_chamberCoolChannels[stationName][stationEta][stationPhi] = hash++;
          }
        }
      }
    }
    *(const_cast<unsigned int*>(&m_maxChamberCoolChannel)) = hash - 1; //-1 because hash overshoots in loop
    *(const_cast<unsigned int*>(&m_maxChanHash)) = m_idHelper->channel_hash_max() - 1;


    std::cout << "Maximum Chamber COOL Channel is " << m_maxChamberCoolChannel << std::endl;
    std::cout << "Maximum Layer hash is " << m_maxLayerHash << std::endl;
    std::cout << "Maximum Channel hash is " << m_maxChanHash << std::endl;
}

void
CscCondDbData::setParameters(bool onlineOfflinePhiFlip){
    m_onlineOfflinePhiFlip = onlineOfflinePhiFlip;
}




// --- writing identifiers -------


// setChannelF001
void
CscCondDbData::setChannelF001(IdentifierHash hash, float f001){
    m_cachedChannelsF001[(unsigned int) hash] = f001;
}

// setChannelNoise
void
CscCondDbData::setChannelNoise(IdentifierHash hash, float noise){
    m_cachedChannelsNoise[(unsigned int) hash] = noise;
}

// setChannelPed
void
CscCondDbData::setChannelPed(IdentifierHash hash, float ped){
    m_cachedChannelsPed[(unsigned int) hash] = ped;
}

// setChannelPSlope
void
CscCondDbData::setChannelPSlope(IdentifierHash hash, float pslope){
    m_cachedChannelsPSlope[(unsigned int) hash] = pslope;
}

// setChannelRMS
void
CscCondDbData::setChannelRMS(IdentifierHash hash, float rms){
    m_cachedChannelsRMS[(unsigned int) hash] = rms;
}

// setChannelStatus
void
CscCondDbData::setChannelStatus(IdentifierHash hash, int status){
    m_cachedChannelsStatus[(unsigned int) hash] = status;
    if((status & 0x1) || ((status >> 1) & 0x1)) setDeadChannelHash(hash);
}

// setChannelT0Base
void
CscCondDbData::setChannelT0Base(IdentifierHash hash, float t0base){
    m_cachedChannelsT0Base[(unsigned int) hash] = t0base;
}

// setChannelT0Phase
void
CscCondDbData::setChannelT0Phase(IdentifierHash hash, bool t0phase){
    m_cachedChannelsT0Phase[(unsigned int) hash] = t0phase;
}



// --- writing dead channels -----

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

// hasChannelT0Phase
bool CscCondDbData::hasChannelT0Phase(IdentifierHash hash) const{
	if(m_cachedChannelsT0Phase.find((unsigned int) hash)==m_cachedChannelsT0Phase.end()) return false;
	return true;
}

// getChannelF001
const
float& CscCondDbData::getChannelF001(IdentifierHash hash) const{
	if(m_cachedChannelsF001.find((unsigned int) hash)==m_cachedChannelsF001.end()) return m_nillfloat;
	return m_cachedChannelsF001.find((unsigned int) hash)->second;
}

// getChannelNoise
const
float& CscCondDbData::getChannelNoise(IdentifierHash hash) const{
	if(m_cachedChannelsNoise.find((unsigned int) hash)==m_cachedChannelsNoise.end()) return m_nillfloat;
	return m_cachedChannelsNoise.find((unsigned int) hash)->second;
}

// getChannelPed
const
float& CscCondDbData::getChannelPed(IdentifierHash hash) const{
	if(m_cachedChannelsPed.find((unsigned int) hash)==m_cachedChannelsPed.end()) return m_nillfloat;
	return m_cachedChannelsPed.find((unsigned int) hash)->second;
}

// getChannelPSlope
const
float& CscCondDbData::getChannelPSlope(IdentifierHash hash) const{
	if(m_cachedChannelsPSlope.find((unsigned int) hash)==m_cachedChannelsPSlope.end()) return m_nillfloat;
	return m_cachedChannelsPSlope.find((unsigned int) hash)->second;
}

// getChannelRMS
const
float& CscCondDbData::getChannelRMS(IdentifierHash hash) const{
	if(m_cachedChannelsRMS.find((unsigned int) hash)==m_cachedChannelsRMS.end()) return m_nillfloat;
	return m_cachedChannelsRMS.find((unsigned int) hash)->second;
}

// getChannelStatus
const
int& CscCondDbData::getChannelStatus(IdentifierHash hash) const{
	if(m_cachedChannelsStatus.find((unsigned int) hash)==m_cachedChannelsStatus.end()) return m_nillint;
	return m_cachedChannelsStatus.find((unsigned int) hash)->second;
}

// getChannelT0Base
const
float& CscCondDbData::getChannelT0Base(IdentifierHash hash) const{
	if(m_cachedChannelsT0Base.find((unsigned int) hash)==m_cachedChannelsT0Base.end()) return m_nillfloat;
	return m_cachedChannelsT0Base.find((unsigned int) hash)->second;
}

// getChannelT0Phase
const
bool& CscCondDbData::getChannelT0Phase(IdentifierHash hash) const{
	if(m_cachedChannelsT0Phase.find((unsigned int) hash)==m_cachedChannelsT0Phase.end()) return m_nillbool;
	return m_cachedChannelsT0Phase.find((unsigned int) hash)->second;
}

// readChannelF001
StatusCode CscCondDbData::readChannelF001(IdentifierHash hash, float& val) const{
    val = getChannelF001(hash);
    if(val==-1) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

// readChannelNoise
StatusCode CscCondDbData::readChannelNoise(IdentifierHash hash, float& val) const{
    val = getChannelNoise(hash);
    if(val==-1) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

// readChannelPed
StatusCode CscCondDbData::readChannelPed(IdentifierHash hash, float& val) const{
    val = getChannelPed(hash);
    if(val==-1) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

// readChannelPSlope
StatusCode CscCondDbData::readChannelPSlope(IdentifierHash hash, float& val) const{
    val = getChannelPSlope(hash);
    if(val==-1) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

// readChannelRMS
StatusCode CscCondDbData::readChannelRMS(IdentifierHash hash, float& val) const{
    val = getChannelRMS(hash);
    if(val==-1) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

// readChannelStatus
StatusCode CscCondDbData::readChannelStatus(IdentifierHash hash, int& val) const{
    val = getChannelStatus(hash);
    if(val==-1) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

// readChannelT0Base
StatusCode CscCondDbData::readChannelT0Base(IdentifierHash hash, float& val) const{
    val = getChannelT0Base(hash);
    if(val==-1) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

// readChannelT0Phase
StatusCode CscCondDbData::readChannelT0Phase(IdentifierHash hash, bool& val) const{
    if(!hasChannelT0Phase(hash)) return StatusCode::FAILURE;
    val = getChannelT0Phase(hash);
    return StatusCode::SUCCESS;
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



// ID HELPER FUNCTIONS BELOW ------------------------------


// indexToStringId
StatusCode 
CscCondDbData::indexToStringId(const unsigned int & index, const std::string & cat, std::string & idString) const {
    // copy-paste from CscCoolStrSvc

    //There is no string id for the CSC category.
    if(cat == "CSC") {
        idString = "";
        return StatusCode::SUCCESS;
    } 
    if(cat == "ENDCAP") {
        if(index == 0)
            idString = "-1";
        if(index == 1)
            idString = "1";
        else {
            std::cout << "Requested index " << index << " can't be converted to a string Id for the category " << cat << std::endl;
            return StatusCode::RECOVERABLE;
        }
    }

    //remaining categories need online identifiers
    unsigned int onlineId = 0;
    std::stringstream ss;
    if(cat == "CHAMBER"){
        Identifier chamberId;
        m_idHelper->get_id(IdentifierHash(index), chamberId, &m_moduleContext);
        if(!offlineElementToOnlineId(chamberId, onlineId).isSuccess()) {
            std::cout << "Failed converting chamber identifier to online id during stringId gen." << std::endl;
            return StatusCode::RECOVERABLE;
        }
    } 
    else if(cat == "LAYER"){
        unsigned int onlineId;
        if(!layerHashToOnlineId(index, onlineId)){
            std::cout << "Failed at getting online id from layer hash during stringId gen." << std::endl; 
        }
    }
    else if(cat == "CHANNEL"){
        Identifier channelId;
        m_idHelper->get_id(IdentifierHash(index), channelId, &m_channelContext);
        if(!offlineToOnlineId(channelId, onlineId).isSuccess()) {
            std::cout << "Failed converting chamber identifier to online id during stringId gen." << std::endl;
            return StatusCode::RECOVERABLE;
        }
    }

    ss << std::hex << std::setfill('0') << std::setw(5) << onlineId << std::dec;
    idString = ss.str();
    return StatusCode::SUCCESS;
}


// layerHashToOnlineId
StatusCode
CscCondDbData::layerHashToOnlineId(const unsigned int & layerHash, unsigned int & onlineId) const {
    // copy-paste from CscCoolStrSvc

    if(layerHash > m_onlineChannelIdsFromLayerHash.size()) {
		std::cout << "Tried to lookup online id from layer hash " << layerHash <<". Max is " << m_onlineChannelIdsFromLayerHash.size() << std::endl;
        return StatusCode::SUCCESS;
    }
    onlineId = m_onlineChannelIdsFromLayerHash[layerHash];
    return StatusCode::SUCCESS;
}


// offlineElementToOnlineId
StatusCode 
CscCondDbData::offlineElementToOnlineId(const Identifier & id, unsigned int &onlineId) const {
    // copy-paste from CscCoolStrSvc

    onlineId = 0;
    //Phi,wireLayer,and strip all are offset by one between the two schemes.
    //Also, station name is 50 or 51 in Identifiers, but only 0 or 1 in 
    //the online id.
    int stationName  	((m_idHelper->stationName(id) -50)&0x1 );		// 0001 0000 0000 0000 0000
    int phi =   		(m_idHelper->stationPhi(id) - 1)&0x7  ;		// 0000 1110 0000 0000 0000
    int eta = 		((m_idHelper->stationEta(id) == 1) ? 1:0) &0x1;  	// 0000 0001 0000 0000 0000
    int chamLay = 		1;		// 0000 0000 1000 0000 0000
    int wireLay = 		0;		// 0000 0000 0110 0000 0000
    int measuresPhi = 0;		// 0000 0000 0001 0000 0000
    int strip = 		0;     	// 0000 0000 0000 1111 1111

    onlineId 	+= (stationName << 16);	// 0001 0000 0000 0000 0000
    onlineId        += (phi << 13) ;	// 0000 1110 0000 0000 0000
    onlineId	+= (eta <<12);  	      // 0000 0001 0000 0000 0000
    onlineId 	+= (chamLay <<11);	    // 0000 0000 1000 0000 0000
    onlineId	+= (wireLay << 9);	    // 0000 0000 0110 0000 0000
    onlineId	+= (measuresPhi << 8);	// 0000 0000 0001 0000 0000
    onlineId	+= strip ;     	      	// 0000 0000 0000 1111 1111
    return StatusCode::SUCCESS;
}


// offlineToOnlineId
StatusCode 
CscCondDbData::offlineToOnlineId(const Identifier & id, unsigned int &onlineId) const {
    // copy-paste from CscCoolStrSvc

    onlineId = 0;
    //Phi,wireLayer,and strip all are offset by one between the two schemes.
    //Also, station name is 50 or 51 in Identifiers, but only 0 or 1 in 
    //the online id.
    int stationName  	((m_idHelper->stationName(id) -50)&0x1 );		// 0001 0000 0000 0000 0000
    int phi =   		(m_idHelper->stationPhi(id) - 1)&0x7  ;		    // 0000 1110 0000 0000 0000
    int eta = 		((m_idHelper->stationEta(id) == 1) ? 1:0) &0x1;  // 0000 0001 0000 0000 0000
    int chamLay = 		(m_idHelper->chamberLayer(id)-1) &0x1;		    // 0000 0000 1000 0000 0000
    int wireLay = 		(m_idHelper->wireLayer(id)-1) &0x3;		      // 0000 0000 0110 0000 0000
    int measuresPhi = 	(m_idHelper->measuresPhi(id) &0x1);		    // 0000 0000 0001 0000 0000
    int strip;     		                                          // 0000 0000 0000 1111 1111

    //Online and offline phi ids are flipped on A wheel
    if(m_onlineOfflinePhiFlip && measuresPhi && eta == 1){
        strip = (48 - (m_idHelper->strip(id))) & 0xff;  
    }
    else {
        strip = (m_idHelper->strip(id)-1) & 0xff;     		     
    }


    onlineId 	+= (stationName << 16);	// 0001 0000 0000 0000 0000
    onlineId        += (phi << 13) ;	// 0000 1110 0000 0000 0000
    onlineId	+= (eta <<12);  	      // 0000 0001 0000 0000 0000
    onlineId 	+= (chamLay <<11);	    // 0000 0000 1000 0000 0000
    onlineId	+= (wireLay << 9);	    // 0000 0000 0110 0000 0000
    onlineId	+= (measuresPhi << 8);	// 0000 0000 0001 0000 0000
    onlineId	+= strip ;     		      // 0000 0000 0000 1111 1111
    return StatusCode::SUCCESS;
}


// onlineToOfflineElementId
StatusCode 
CscCondDbData::onlineToOfflineElementId(const unsigned int & onlineId, Identifier &elementId) const {
    // copy-paste from CscCoolStrSvc

    int stationName =       ((onlineId >> 16)&0x1) + 50;
    int phi =               ((onlineId >> 13)&0x7)+1;
    int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);

    elementId = m_idHelper->elementID(stationName,eta,phi);
    return StatusCode::SUCCESS;
}


// onlineToOfflineChannelId
StatusCode 
CscCondDbData::onlineToOfflineChannelId(const unsigned int & onlineId, Identifier &chanId) const {
    // copy-paste from CscCoolStrSvc

    int stationName =       ((onlineId >> 16)&0x1) + 50;
    int phi =               ((onlineId >> 13)&0x7)+1;
    int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);
    int chamLay =           ((onlineId>>11)&0x1) +1;
    int wireLay =           ((onlineId>>9)&0x3) +1;
    int measuresPhi =       ((onlineId >> 8)&0x1);
    int strip;

    //Online and offline phi ids are flipped on A wheel
    if(m_onlineOfflinePhiFlip && measuresPhi && eta == 1){
        strip = 48 - ((onlineId)&0xff) ; //equivalent: 49 -( onlineId&0xff +1)
    }
    else {
        strip = ((onlineId)&0xff) +1;
    }

    chanId = m_idHelper->channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);
    return StatusCode::SUCCESS;
}


// onlineToOfflineIds
StatusCode 
CscCondDbData::onlineToOfflineIds(const unsigned int & onlineId, Identifier &elementId, Identifier &channelId) const {
    // copy-paste from CscCoolStrSvc

    int stationName =       ((onlineId >> 16)&0x1) + 50;
    int phi =               ((onlineId >> 13)&0x7)+1;
    int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);
    int chamLay =           ((onlineId>>11)&0x1) +1;
    int wireLay =           ((onlineId>>9)&0x3) +1;
    int measuresPhi =       ((onlineId >> 8)&0x1);
    int strip;
  
    //Online and offline phi ids are flipped on A wheel
    if(m_onlineOfflinePhiFlip && measuresPhi && eta == 1){
      strip = 48 - ((onlineId)&0xff) ; //equivalent: 49 -( onlineId&0xff +1)
    }
    else {
      strip = ((onlineId)&0xff) +1;
    }
  
    elementId = m_idHelper->elementID(stationName,eta,phi);
    channelId = m_idHelper->channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);
  
    return StatusCode::SUCCESS;
}



