/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/CscCondDbAlg.h"

// constructor
CscCondDbAlg::CscCondDbAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
    AthAlgorithm(name, pSvcLocator),
    m_condSvc("CondSvc", name),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_defaultDatabaseReadVersion("02-00")
  {
 
    declareProperty("IdHelper"                  , m_idHelper                           );
    declareProperty("defaultDatabaseReadVersion", m_defaultDatabaseReadVersion, "02-00");
    declareProperty("phiSwapVersion1Strings"    , m_phiSwapVersion1Strings = true      );
    declareProperty("onlineOfflinePhiFlip"      , m_onlineOfflinePhiFlip = true        );

    declareProperty("isOnline"                  , m_isOnline                           );
    declareProperty("isData"                    , m_isData                             );
    declareProperty("isRun1"                    , m_isRun1                             );
}


// Initialize
StatusCode
CscCondDbAlg::initialize(){

    ATH_MSG_DEBUG( "initializing " << name() );                
    ATH_CHECK(m_condSvc .retrieve());
    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_writeKey.initialize());
    ATH_CHECK(m_readKey_folder_da_hv     .initialize());
    ATH_CHECK(m_readKey_folder_da_f001   .initialize());
    ATH_CHECK(m_readKey_folder_da_noise  .initialize());
    ATH_CHECK(m_readKey_folder_da_ped    .initialize());
    ATH_CHECK(m_readKey_folder_da_pslope .initialize());
    ATH_CHECK(m_readKey_folder_da_rms    .initialize());
    ATH_CHECK(m_readKey_folder_da_status .initialize());
    ATH_CHECK(m_readKey_folder_da_t0base .initialize());
    ATH_CHECK(m_readKey_folder_da_t0phase.initialize());

    if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
      ATH_MSG_FATAL("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

/*

// prepareCollections
StatusCode
CscCondDbAlg::prepareCollections() const {

    // prepare contexts
    m_moduleContext  = m_idHelper->cscIdHelper().module_context();
    m_channelContext = m_idHelper->cscIdHelper().channel_context();

    //prepare layer hash array
    int hash = 0;

    for(int stationName  = 0; stationName < 2; stationName++){
      for(int stationEta =0; stationEta <2; stationEta++){
        for(int stationPhi = 0; stationPhi <8; stationPhi++){
          for(int wireLayer = 0; wireLayer <4; wireLayer++){
            for(int measuresPhi = 0; measuresPhi <2; measuresPhi++){
              Identifier id = m_idHelper->cscIdHelper().channelID(
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
                ATH_MSG_ERROR("Failed at geting online id!");
                return StatusCode::RECOVERABLE;
              }
              m_onlineChannelIdsFromLayerHash.push_back(onlineId);
              m_layerHashes[stationName][stationEta][stationPhi][wireLayer][measuresPhi] = hash++;
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
          Identifier id = m_idHelper->cscIdHelper().channelID(
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
            ATH_MSG_ERROR("Failed at geting online id!");
            return StatusCode::RECOVERABLE;
          }

          m_onlineChannelIdsFromChamberCoolChannel.push_back(onlineId);
          m_chamberCoolChannels[stationName][stationEta][stationPhi] = hash++;
        }
      }
    }
    *(const_cast<unsigned int*>(&m_maxChamberCoolChannel)) = hash - 1; //-1 because hash overshoots in loop
    *(const_cast<unsigned int*>(&m_maxChanHash)) = m_idHelper->cscIdHelper().channel_hash_max() - 1;


    ATH_MSG_DEBUG("Maximum Chamber COOL Channel is " << m_maxChamberCoolChannel);
    ATH_MSG_DEBUG("Maximum Layer hash is " << m_maxLayerHash);
    ATH_MSG_DEBUG("Maximum Channel hash is " << m_maxChanHash);

}
*/


// execute
StatusCode 
CscCondDbAlg::execute(){

    ATH_MSG_DEBUG( "execute " << name() );   

	if(m_isOnline) {
		ATH_MSG_DEBUG( "IsOnline is set to True; nothing to do!" );   
		return StatusCode::SUCCESS;
	}
 
    // launching Write Cond Handle
    SG::WriteCondHandle<CscCondDbData> writeHandle{m_writeKey};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
        	  << " In theory this should not be called, but may happen"
        	  << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS; 
    }
    std::unique_ptr<CscCondDbData> writeCdo{std::make_unique<CscCondDbData>()};
    writeCdo->setParameters(m_onlineOfflinePhiFlip);

    EventIDRange rangeW;
    StatusCode sc  = StatusCode::SUCCESS;

    // retrieving data only
    if(m_isData) {
        //if(loadDataHv  (rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; // keep for future development
    }

    // both data and MC
    if(loadDataF001   (rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; 
    if(loadDataNoise  (rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; 
    if(loadDataPed    (rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; 
    if(loadDataPSlope (rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; 
    if(loadDataRMS    (rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; 
    if(loadDataStatus (rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; 
    if(loadDataT0Base (rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; 
    if(loadDataT0Phase(rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; 

    if(sc.isFailure()){
        ATH_MSG_WARNING("Could not read data from the DB");
        return StatusCode::FAILURE;
    }

    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record CscCondDbData " << writeHandle.key() 
  		  << " with EventRange " << rangeW
  		  << " into Conditions Store");
      return StatusCode::FAILURE;
    }		  
    ATH_MSG_DEBUG("Recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

    return StatusCode::SUCCESS;
}


// loadDataHv
StatusCode
CscCondDbAlg::loadDataHv(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
  
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_hv};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
  
    if ( !readHandle.range(rangeW) ) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
  
    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << rangeW);

    CondAttrListCollection::const_iterator itr;
	std::map<Identifier, int> layerMap;
    int hv_state, lv_state, hv_setpoint0, hv_setpoint1;

    unsigned int chan_index = 0; 
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

        unsigned int chanNum      = readCdo->chanNum (chan_index);
        std::string csc_chan_name = readCdo->chanName(chanNum);

        const coral::AttributeList& atr = itr->second;
    
        if(atr.size()){
    
            hv_state     = *(static_cast<const int*>((atr["HVState"    ]).addressOfData()));
            lv_state     = *(static_cast<const int*>((atr["LVState"    ]).addressOfData()));
            hv_setpoint0 = *(static_cast<const int*>((atr["HVSetpoint0"]).addressOfData()));
            hv_setpoint1 = *(static_cast<const int*>((atr["HVSetpoint1"]).addressOfData()));
    
            std::string delimiter = "_";
            std::vector<std::string> tokens;
            MuonCalib::MdtStringUtils::tokenize(csc_chan_name,tokens,delimiter);

            if((hv_state!=1 or lv_state!=1 or hv_setpoint0 <1000 or hv_setpoint1 <1000) && tokens.size()!=0){
      
                std::string layer = tokens[1];
                std::string number_layer = tokens[1].substr(1,2);   
                int wirelayer = atoi(const_cast<char*>(number_layer.c_str()));

                int eta=0; 
                std::string eta_side = tokens[0].substr(0,1);
                if (eta_side == "A") eta = +1;
                if (eta_side == "C") eta = -1;

                std::string chamber_name;
                std::string size_side = tokens[0].substr(1,1);
                if (size_side == "L") chamber_name = "CSL";
                if (size_side == "S") chamber_name = "CSS";

                int phi=0;
                std::string sector_side = tokens[0].substr(2,4);
                if (sector_side == "01" || sector_side == "02") phi=1;
                if (sector_side == "03" || sector_side == "04") phi=2;
                if (sector_side == "05" || sector_side == "06") phi=3;
                if (sector_side == "07" || sector_side == "08") phi=4;
                if (sector_side == "09" || sector_side == "10") phi=5;
                if (sector_side == "11" || sector_side == "12") phi=6;
                if (sector_side == "13" || sector_side == "14") phi=7;
                if (sector_side == "15" || sector_side == "16") phi=8;
      
                Identifier ChamberId   = m_idHelper->cscIdHelper().elementID(chamber_name, eta, phi);
                Identifier WireLayerId = m_idHelper->cscIdHelper().channelID(ChamberId, 1, wirelayer,1,1);
                std::string WireLayerstring = chamber_name+"_"+eta_side+"_"+sector_side+"_"+layer;  

                writeCdo->setDeadLayer(WireLayerstring, WireLayerId);
                if(layerMap.count(ChamberId)==0) layerMap[ChamberId] = 0;
                ++layerMap[ChamberId];
                if(layerMap[ChamberId]==3) writeCdo->setDeadStation(chamber_name, ChamberId);
            }
        }
        chan_index++;
    }

    return StatusCode::SUCCESS;
}


// loadDataF001
StatusCode
CscCondDbAlg::loadDataF001(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_f001};
	return loadData(rangeW, writeCdo, readHandle, "f001");
}

// loadDataNoise
StatusCode
CscCondDbAlg::loadDataNoise(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_noise};
	return loadData(rangeW, writeCdo, readHandle, "noise");
}

// loadDataPed
StatusCode
CscCondDbAlg::loadDataPed(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_ped};
	return loadData(rangeW, writeCdo, readHandle, "ped");
}

// loadDataPSlope
StatusCode
CscCondDbAlg::loadDataPSlope(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_pslope};
	return loadData(rangeW, writeCdo, readHandle, "pslope");
}

// loadDataRMS
StatusCode
CscCondDbAlg::loadDataRMS(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_rms};
	return loadData(rangeW, writeCdo, readHandle, "rms");
}

// loadDataStatus
StatusCode
CscCondDbAlg::loadDataStatus(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_status};
	return loadData(rangeW, writeCdo, readHandle, "status");
}

// loadDataT0Base
StatusCode
CscCondDbAlg::loadDataT0Base(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_t0base};
	return loadData(rangeW, writeCdo, readHandle, "t0base");
}

// loadDataT0Phase
StatusCode
CscCondDbAlg::loadDataT0Phase(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_t0phase};
	return loadData(rangeW, writeCdo, readHandle, "t0phase", true);
}


// loadData
StatusCode
CscCondDbAlg::loadData(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo, SG::ReadCondHandle<CondAttrListCollection> readHandle, const std::string parName, bool parAsm) {

    const CondAttrListCollection* readCdo{*readHandle}; 

    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
       return StatusCode::FAILURE; 
    } 
  
    if ( !readHandle.range(rangeW) ) {
       ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
  
    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << rangeW);
 
    CondAttrListCollection::const_iterator itr;

    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

        const coral::AttributeList& atr = itr->second;
        std::string data = atr["Data"].data<std::string>();
        ATH_MSG_DEBUG("Data is: "<<data);
 	
        std::istringstream ss(data);	

	    if(!ss.good()) {
            ATH_MSG_WARNING("Failed forming stringstream during caching");
            continue;
        }

        std::string version;
		ss >> version;

        if(version == "1" || atoi(version.c_str()) == 1){
            if(!cacheVersion1(data, writeCdo, parName).isSuccess()) {
                ATH_MSG_ERROR("Failed caching from COOL string version 1");
                return StatusCode::FAILURE;
            }
        }
        else if(version == "02-00" && parAsm) {
            if(!cacheVersion2ASM(data, writeCdo, parName).isSuccess()) {
                ATH_MSG_ERROR("Failed caching from COOL string version 02-00 in ASM format");
                return StatusCode::FAILURE;
            }
        }
        else if(version == "02-00") {
            if(!cacheVersion2(data, writeCdo, parName).isSuccess()) {
                ATH_MSG_ERROR("Failed caching from COOL string version 02-00");
                return StatusCode::FAILURE;
            }
        }
        else {
            // Old version was treated as an actual number rather than string. It was always
            // set to 1 or sometimes 1.00000, so we convert to integer here and check
            ATH_MSG_WARNING("Don't recognize CSC COOL string version " << version << ". Will treat as default version " << m_defaultDatabaseReadVersion);
            if(m_defaultDatabaseReadVersion == "1"){
                if(!cacheVersion1(data, writeCdo, parName).isSuccess()) {
                    ATH_MSG_ERROR("Failed caching from COOL string version 1");
                    return StatusCode::FAILURE;
                }
            }
            else if(m_defaultDatabaseReadVersion == "02-00" && parAsm) {
                if(!cacheVersion2ASM(data, writeCdo, parName).isSuccess()) {
                    ATH_MSG_ERROR("Failed caching from COOL string version 02-00 in ASM format");
                    return StatusCode::FAILURE;
                }
            }
            else if(m_defaultDatabaseReadVersion == "02-00"){
                if(!cacheVersion2(data, writeCdo, parName).isSuccess()) {
                    ATH_MSG_ERROR("Failed caching from COOL string version 02-00");
                    return StatusCode::FAILURE;
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}


// cacheVersion1
StatusCode 
CscCondDbAlg::cacheVersion1(std::string data, std::unique_ptr<CscCondDbData>& writeCdo, const std::string parName){

	// ATTENTION: careful, this is untested yet! (lack of data)
	ATH_MSG_WARNING("Running cacheVersion1 function for parameter "<<parName<<", which is not properly implemented for folders other than /CSC/STAT/ nor properly tested!");

    std::string valueStr;
	std::istringstream ss(data);

	bool started = false;
    while(ss.good()) {
        ss >> valueStr;
  
		if(valueStr == "END") break;
 
        if(valueStr == "<BEGIN_DATA>"){
            started = true;
			continue;
		}
		if(!started) continue;

		ATH_MSG_DEBUG("current element " << valueStr);

        int index = atoi(valueStr.c_str());

		// swapping version 1 strings
        if(m_phiSwapVersion1Strings){
            Identifier chanId;
            IdContext channelContext = m_idHelper->cscIdHelper().channel_context();
            m_idHelper->cscIdHelper().get_id((IdentifierHash)index, chanId, &channelContext);
            int stationEta  = m_idHelper->cscIdHelper().stationEta (chanId); // +1 Wheel A   -1 Wheel C
            int measuresPhi = m_idHelper->cscIdHelper().measuresPhi(chanId); // 0 eta 1 phi

            if(stationEta > 0 && measuresPhi ==1){
                int stationName  = m_idHelper->cscIdHelper().stationName (chanId); // CSL or CSS
                int stationPhi   = m_idHelper->cscIdHelper().stationPhi  (chanId); // PhiSector from 1-8
                int chamberLayer = m_idHelper->cscIdHelper().chamberLayer(chanId); // Either 1 or 2 (but always 2)
                int wireLayer    = m_idHelper->cscIdHelper().wireLayer   (chanId); // layer in chamber 1-4
                int strip        = 49 - m_idHelper->cscIdHelper().strip  (chanId);

                Identifier newId = m_idHelper->cscIdHelper().channelID(stationName,stationEta,stationPhi,chamberLayer, wireLayer,measuresPhi,strip);
                IdentifierHash hash ;
                m_idHelper->cscIdHelper().get_channel_hash(newId, hash);

				ATH_MSG_VERBOSE("Swapped phi strip "<< m_idHelper->cscIdHelper().show_to_string(chanId) << 
                                " (" << index << ") to " << m_idHelper->cscIdHelper().show_to_string(newId) << 
                                " (" << hash << ")");
                index = hash;
            }
            else {
                ATH_MSG_VERBOSE("Not swapping " << m_idHelper->cscIdHelper().show_to_string(chanId));
            }
        }

        writeCdo->setDeadChannelHash((IdentifierHash) index);

    }
    return StatusCode::SUCCESS;
}



// cacheVersion2
StatusCode 
CscCondDbAlg::cacheVersion2(std::string data, std::unique_ptr<CscCondDbData>& writeCdo, const std::string parName) {

	std::istringstream ss(data);
    std::string valueStr;
    unsigned int chanAddress = 0;

	bool started = false;
    while(ss.good()) {
        ss >> valueStr;

		if(valueStr == "<END_DATA>") break;
   
        if(valueStr == "<BEGIN_DATA>"){
            started = true;
			continue;
		}
		if(!started) continue;
		ATH_MSG_DEBUG("current element " << valueStr);
        std::istringstream iss(valueStr);


		// use new iss to translate the hex
        if(chanAddress==0){
            iss >> std::hex >> chanAddress;
            continue;
        }

        // record parameter
        if(recordParameter(chanAddress, valueStr, writeCdo, parName).isFailure())
        	return StatusCode::FAILURE;

        // reset the address
        chanAddress = 0;
    }

    return StatusCode::SUCCESS;
}


// cacheVersion2ASM
StatusCode 
CscCondDbAlg::cacheVersion2ASM(std::string data, std::unique_ptr<CscCondDbData>& writeCdo, const std::string parName) {

	std::istringstream ss(data);
    std::string valueStr;
    std::string chanAddress;
    bool setAddress = false;

	bool started = false;
    while(ss.good()) {
        ss >> valueStr;

		if(valueStr == "<END_DATA>") break;
   
        if(valueStr == "<BEGIN_DATA>"){
            started = true;
			continue;
		}
		if(!started) continue;
		ATH_MSG_DEBUG("current element " << valueStr);
        std::istringstream iss(valueStr);


		// use new iss to translate the hex
        if(!setAddress){
            iss >> chanAddress;
            setAddress = true;
            continue;
        }

        // chanAddress is the ASM tag, need to do something with it
        // format: ASM[#:1-5]_[StationEtaString:AorC][stationPhi:1-8]_[stationName:50-51]
        //         xxx   3   x                  5                6   x             x9
        int asmNum = atoi(chanAddress.substr(3,1).c_str());

        int stationEta = 0;
        if     (chanAddress[5] == 'A') stationEta =  1;
        else if(chanAddress[5] == 'C') stationEta = -1;   
        else{
            ATH_MSG_FATAL("Bad ASMID String in CSC COOL database \"" << chanAddress << "\" (wheel " << chanAddress[5] << " doesn't exist!");
            return StatusCode::FAILURE;
        }
         
        int stationPhi  = atoi(chanAddress.substr(6,1).c_str());
        int stationName = atoi(chanAddress.substr(8,2).c_str());
        
        if(stationPhi < 1 || stationPhi > 8 || stationName < 50 || stationName > 51){
            ATH_MSG_FATAL("Bad ASMID String in CSC COOL database: \"" << chanAddress << "\"");
            ATH_MSG_FATAL("Read station phi: " << stationPhi << ", stationName " << stationName);
            return StatusCode::FAILURE;
        }

        int chamberLayer = 2;  // chamberLayer1 was never built.

        int measuresPhi  = 0;
        int layerSince   = 0;
        int layerUntil   = 0;
        int stripSince   = 0;
        int stripUntil   = 0;
        if(!getAsmScope(asmNum, measuresPhi, layerSince, layerUntil, stripSince, stripUntil).isSuccess()){ 
            ATH_MSG_FATAL("Failure of getAsmScope in cacheVersion2.");
            return StatusCode::FAILURE;
        }

        //  Now for given asmID, loop over strip and layer
        unsigned int index = 0;
        Identifier chanId;
        IdentifierHash hashIdentifier;
        for(int iStrip = stripSince; iStrip < stripUntil; iStrip++){ 
            for(int iLayer = layerSince; iLayer < layerUntil; iLayer++){ 
                chanId = m_idHelper->cscIdHelper().channelID(stationName, stationEta, stationPhi, 
                                                             chamberLayer, iLayer, measuresPhi, iStrip);
                m_idHelper->cscIdHelper().get_channel_hash(chanId, hashIdentifier);
                index = (int) hashIdentifier;

                ATH_MSG_VERBOSE("[cache version 2 (ASM)] Recording "
                                    << valueStr << " at index " << index 
                                    << "\nstationName " << stationName
                                    <<"\nstationEta " << stationEta
                                    << "\nstationPhi " << stationPhi 
                                    << "\nchamberLayer " << chamberLayer
                                    << "\niLayer " << iLayer
                                    << "\nmeasuresPhi " << measuresPhi
                                    << "\niStrip " << iStrip); 

                // record parameter
                if(recordParameter(hashIdentifier, valueStr, writeCdo, parName).isFailure())
                	return StatusCode::FAILURE;
            }
        }

        // reset the address
        setAddress  = false;
        chanAddress = "";
    }

    return StatusCode::SUCCESS;
}


// getAsmScope
StatusCode 
CscCondDbAlg::getAsmScope(int asmNum, int &measuresPhi,  int & layerSince, int & layerUntil, int & stripSince , int & stripUntil){
    // copy-paste from CscCoolStrSvc

    if(asmNum == 1 ){
        stripSince = 1;  //inclusive
        stripUntil = 97; //exclusive
        layerSince = 1; //inclusive
        layerUntil = 3; //exclusive
        measuresPhi = 0;
    }
    else if(asmNum == 2){
        stripSince = 1;  //inclusive
        stripUntil = 97; //exclusive
        layerSince = 3; //inclusive
        layerUntil = 5; //exclusive
        measuresPhi = 0;
    }
    else if(asmNum == 3){
        stripSince = 97; 
        stripUntil = 193;
        layerSince = 1;
        layerUntil = 3;
        measuresPhi = 0;
    }
    else if( asmNum == 4){
        stripSince = 97; 
        stripUntil = 193;
        layerSince = 3;
        layerUntil = 5;
        measuresPhi = 0;
    }
    else if(asmNum == 5){
        stripSince = 1;
        stripUntil = 49;
        layerSince = 1;
        layerUntil = 5;
        measuresPhi = 1;
    }
    else {
        ATH_MSG_FATAL("ASM  number  \"" << asmNum << "\" is invalid. It needs to end in a number from 1-5.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}



// recordParameter
StatusCode
CscCondDbAlg::recordParameter(unsigned int chanAddress, std::string data, std::unique_ptr<CscCondDbData>& writeCdo, const std::string parName){

    // retrieve channel hash
    Identifier chamberId;
    Identifier channelId;
    if(!writeCdo->onlineToOfflineIds(chanAddress, chamberId, channelId).isSuccess())
        ATH_MSG_ERROR("Cannon get offline Ids from online Id" << std::hex << chanAddress << std::dec);
    
    IdentifierHash chanHash;
    m_idHelper->cscIdHelper().get_channel_hash(channelId, chanHash);
    
    // record parameter
    return recordParameter(chanHash, data, writeCdo, parName);   
}


// recordParameter
StatusCode
CscCondDbAlg::recordParameter(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo, const std::string parName){
    
    // record parameter
    StatusCode sc = StatusCode::FAILURE;
    if     (parName == "f001"   ) sc = recordParameterF001   (chanHash, data, writeCdo);
    else if(parName == "noise"  ) sc = recordParameterNoise  (chanHash, data, writeCdo);
    else if(parName == "ped"    ) sc = recordParameterPed    (chanHash, data, writeCdo);
    else if(parName == "pslope" ) sc = recordParameterPSlope (chanHash, data, writeCdo);
    else if(parName == "rms"    ) sc = recordParameterRMS    (chanHash, data, writeCdo);
    else if(parName == "status" ) sc = recordParameterStatus (chanHash, data, writeCdo);
    else if(parName == "t0base" ) sc = recordParameterT0Base (chanHash, data, writeCdo);
    else if(parName == "t0phase") sc = recordParameterT0Phase(chanHash, data, writeCdo);

    if(!sc.isSuccess())
        ATH_MSG_ERROR("Cannot extract parameter " <<parName<< " for channel hash " <<chanHash<< " from data string '" <<data << "'");
    return sc;
}


// recordParameterF001
StatusCode
CscCondDbAlg::recordParameterF001(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelF001(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterNoise
StatusCode
CscCondDbAlg::recordParameterNoise(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelNoise(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterPed
StatusCode
CscCondDbAlg::recordParameterPed(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelPed(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterPSlope
StatusCode
CscCondDbAlg::recordParameterPSlope(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelPSlope(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterRMS
StatusCode
CscCondDbAlg::recordParameterRMS(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelRMS(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterStatus
StatusCode
CscCondDbAlg::recordParameterStatus(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

    unsigned int token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelStatus(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterT0Base
StatusCode
CscCondDbAlg::recordParameterT0Base(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelT0Base(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterT0Phase
StatusCode
CscCondDbAlg::recordParameterT0Phase(IdentifierHash chanHash, std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

    bool token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelT0Phase(chanHash, token);
    return StatusCode::SUCCESS;
}



// ID HELPER FUNCTIONS BELOW ------------------------------

/*
// indexToStringId
StatusCode 
CscCondDbAlg::indexToStringId(const unsigned int & index, const std::string & cat, std::string & idString) const {
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
            ATH_MSG_ERROR("Requested index " << index << " can't be converted to a string Id for the category " << cat);
            return StatusCode::RECOVERABLE;
        }
    }

    //remaining categories need online identifiers
    unsigned int onlineId = 0;
    stringstream ss;
    if(cat == "CHAMBER"){
        Identifier chamberId;
        m_idHelper->cscIdHelper().get_id(IdentifierHash(index), chamberId, &m_moduleContext);
        if(!offlineElementToOnlineId(chamberId, onlineId).isSuccess()) {
            ATH_MSG_ERROR("Failed converting chamber identifier to online id during stringId gen.");
            return StatusCode::RECOVERABLE;
        }
    } 
    else if(cat == "LAYER"){
        unsigned int onlineId;
        if(!layerHashToOnlineId(index, onlineId)){
            ATH_MSG_ERROR("Failed at getting online id from layer hash during stringId gen."); 
        }
    }
    else if(cat == "CHANNEL"){
        Identifier channelId;
        m_idHelper->cscIdHelper().get_id(IdentifierHash(index), channelId, &m_channelContext);
        if(!offlineToOnlineId(channelId, onlineId).isSuccess()) {
            ATH_MSG_ERROR("Failed converting chamber identifier to online id during stringId gen.");
            return StatusCode::RECOVERABLE;
        }
    }

    ss << hex << setfill('0') << setw(5) << onlineId << dec;
    idString = ss.str();
    return StatusCode::SUCCESS;
}


// layerHashToOnlineId
StatusCode
CscCondDbAlg::layerHashToOnlineId(const unsigned int & layerHash, unsigned int & onlineId) const {
    // copy-paste from CscCoolStrSvc

    if(layerHash > m_onlineChannelIdsFromLayerHash.size()) {
		ATH_MSG_ERROR("Tried to lookup online id from layer hash " << layerHash <<". Max is " << m_onlineChannelIdsFromLayerHash.size());
        return StatusCode::SUCCESS;
    }
    onlineId = m_onlineChannelIdsFromLayerHash[layerHash];
    return StatusCode::SUCCESS;
}


// offlineElementToOnlineId
StatusCode 
CscCondDbAlg::offlineElementToOnlineId(const Identifier & id, unsigned int &onlineId) const {
    // copy-paste from CscCoolStrSvc

    onlineId = 0;
    //Phi,wireLayer,and strip all are offset by one between the two schemes.
    //Also, station name is 50 or 51 in Identifiers, but only 0 or 1 in 
    //the online id.
    int stationName  	((m_idHelper->cscIdHelper().stationName(id) -50)&0x1 );		// 0001 0000 0000 0000 0000
    int phi =   		(m_idHelper->cscIdHelper().stationPhi(id) - 1)&0x7  ;		// 0000 1110 0000 0000 0000
    int eta = 		((m_idHelper->cscIdHelper().stationEta(id) == 1) ? 1:0) &0x1;  	// 0000 0001 0000 0000 0000
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
CscCondDbAlg::offlineToOnlineId(const Identifier & id, unsigned int &onlineId) const {
    // copy-paste from CscCoolStrSvc

    onlineId = 0;
    //Phi,wireLayer,and strip all are offset by one between the two schemes.
    //Also, station name is 50 or 51 in Identifiers, but only 0 or 1 in 
    //the online id.
    int stationName  	((m_idHelper->cscIdHelper().stationName(id) -50)&0x1 );		// 0001 0000 0000 0000 0000
    int phi =   		(m_idHelper->cscIdHelper().stationPhi(id) - 1)&0x7  ;		    // 0000 1110 0000 0000 0000
    int eta = 		((m_idHelper->cscIdHelper().stationEta(id) == 1) ? 1:0) &0x1;  // 0000 0001 0000 0000 0000
    int chamLay = 		(m_idHelper->cscIdHelper().chamberLayer(id)-1) &0x1;		    // 0000 0000 1000 0000 0000
    int wireLay = 		(m_idHelper->cscIdHelper().wireLayer(id)-1) &0x3;		      // 0000 0000 0110 0000 0000
    int measuresPhi = 	(m_idHelper->cscIdHelper().measuresPhi(id) &0x1);		    // 0000 0000 0001 0000 0000
    int strip;     		                                          // 0000 0000 0000 1111 1111

    //Online and offline phi ids are flipped on A wheel
    if(m_onlineOfflinePhiFlip && measuresPhi && eta == 1){
        strip = (48 - (m_idHelper->cscIdHelper().strip(id))) & 0xff;  
    }
    else {
        strip = (m_idHelper->cscIdHelper().strip(id)-1) & 0xff;     		     
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
CscCondDbAlg::onlineToOfflineElementId(const unsigned int & onlineId, Identifier &elementId) const {
    // copy-paste from CscCoolStrSvc

    int stationName =       ((onlineId >> 16)&0x1) + 50;
    int phi =               ((onlineId >> 13)&0x7)+1;
    int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);

    elementId = m_idHelper->cscIdHelper().elementID(stationName,eta,phi);
    return StatusCode::SUCCESS;
}


// onlineToOfflineChannelId
StatusCode 
CscCondDbAlg::onlineToOfflineChannelId(const unsigned int & onlineId, Identifier &chanId) const {
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

    chanId = m_idHelper->cscIdHelper().channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);
    return StatusCode::SUCCESS;
}


// onlineToOfflineIds
StatusCode 
CscCondDbAlg::onlineToOfflineIds(const unsigned int & onlineId, Identifier &elementId, Identifier &channelId) const {
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
  
    elementId = m_idHelper->cscIdHelper().elementID(stationName,eta,phi);
    channelId = m_idHelper->cscIdHelper().channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);
  
    return StatusCode::SUCCESS;
}

*/


/*
keep for future development:

// loadDataDeadChambers
StatusCode
CscCondDbAlg::loadDataDeadChambers(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
  
    ATH_CHECK(m_readKey_folder_da_chambers.initialize());
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_chambers};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
  
    if ( !readHandle.range(rangeW) ) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
  
    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << rangeW);

    std::vector<std::string> goodChambers;
    std::vector<std::string> deadChambers;

    CondAttrListCollection::const_iterator itr;
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

        const coral::AttributeList& atr = itr->second;
        std::string chamber_enabled = *(static_cast<const std::string*>((atr["enabledChambers"]).addressOfData()));
    
        std::string delimiter = " ";
        std::vector<std::string> tokens;
        MuonCalib::MdtStringUtils::tokenize(chamber_enabled,tokens,delimiter);
    
        for (unsigned int i=0; i<tokens.size(); i++) goodChambers.push_back(tokens[i]);
    }
  
    std::string chamber_all[] = {"A01","A02","A03","A04","A05","A06","A07","A08","A09","A10","A11","A12","A13","A14","A15","A16",
                                 "C01","C02","C03","C04","C05","C06","C07","C08","C09","C10","C11","C12","C13","C14","C15","C16"};
    std::vector<std::string> chamber_v(chamber_all,chamber_all+32);
    sort(chamber_v.begin(), chamber_v.end());
    
    for(unsigned int i=0; i<chamber_v.size(); ++i){
        if(!binary_search(goodChambers.begin(), goodChambers.end(), chamber_v[i]))
            deadChambers.push_back(chamber_v[i]);
    } 

    for(unsigned int i=0; i<deadChambers.size(); ++i){
        int eta = 0; 
        std::string eta_side = deadChambers[i].substr(0,1);
        if(eta_side == "A") eta = +1;
        if(eta_side == "C") eta = -1;

        int phi = 0;
        std::string sector_side = deadChambers[i].substr(2,4);
        if(sector_side == "01" || sector_side == "02") phi=1;
        if(sector_side == "03" || sector_side == "04") phi=2;
        if(sector_side == "05" || sector_side == "06") phi=3;
        if(sector_side == "07" || sector_side == "08") phi=4;
        if(sector_side == "09" || sector_side == "10") phi=5;
        if(sector_side == "11" || sector_side == "12") phi=6;
        if(sector_side == "13" || sector_side == "14") phi=7;
        if(sector_side == "15" || sector_side == "16") phi=8;

        std::string chamber_name = "";
        if(sector_side == "01" || sector_side == "03" ||sector_side == "05" ||sector_side == "07" || sector_side == "09" || sector_side == "11" || sector_side == "13" || sector_side == "15" ) chamber_name = "CSL";
        if(sector_side == "02" || sector_side == "04" || sector_side == "06"|| sector_side == "08" || sector_side == "10"|| sector_side == "12"|| sector_side == "14"|| sector_side == "16") chamber_name = "CSS";

        Identifier ChamberId = m_idHelper->cscIdHelper().elementID(chamber_name, eta, phi);
		writeCdo->setDeadStation(deadChambers[i], ChamberId);
    }

    return StatusCode::SUCCESS;
}
*/

