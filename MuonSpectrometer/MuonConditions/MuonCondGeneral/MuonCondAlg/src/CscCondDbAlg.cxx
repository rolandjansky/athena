/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/CscCondDbAlg.h"
#include "AthenaKernel/IOVInfiniteRange.h"

// constructor
CscCondDbAlg::CscCondDbAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
    AthReentrantAlgorithm(name, pSvcLocator),
    m_condSvc("CondSvc", name)
{
}


// Initialize
StatusCode
CscCondDbAlg::initialize(){

    ATH_MSG_DEBUG( "initializing " << name() );
    ATH_CHECK(m_condSvc .retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_writeKey.initialize());

    if (m_pslopeFromDB) {
        ATH_MSG_WARNING("You have activated the retrieval of the pslope per CSC channel from the COOL database. "
            << "Please make sure that a correct PSLOPE database is in place which uses geometrical CSC hashes in hex format "
            << "as keys and different values of the pslopes for the different CSC channels as values, otherwise please run "
            << "with the ReadPSlopeFromDatabase property set to false");
    } else {
        if (!(m_pslope>0 && m_pslope<1)) {
            ATH_MSG_FATAL("The Pslope cannot be set to a value <=0 or >=1");
            return StatusCode::FAILURE;
        } else if (m_pslope != m_DEFAULT_PSLOPE) {
            ATH_MSG_WARNING("You have manually set the Pslope property (to " << m_pslope << "). Please check whether this is really intended.");
        }
    }

    ATH_CHECK(m_readKey_folder_da_hv     .initialize(!m_readKey_folder_da_hv.empty() && m_isData));
    ATH_CHECK(m_readKey_folder_da_f001   .initialize());
    ATH_CHECK(m_readKey_folder_da_noise  .initialize());
    ATH_CHECK(m_readKey_folder_da_ped    .initialize());
    ATH_CHECK(m_readKey_folder_da_pslope .initialize(m_pslopeFromDB));
    ATH_CHECK(m_readKey_folder_da_rms    .initialize());
    ATH_CHECK(m_readKey_folder_da_status .initialize());
    ATH_CHECK(m_readKey_folder_da_t0base .initialize(!m_readKey_folder_da_t0base.empty()));
    ATH_CHECK(m_readKey_folder_da_t0phase.initialize(!m_readKey_folder_da_t0phase.empty()));

    if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
      ATH_MSG_FATAL("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }



    return StatusCode::SUCCESS;
}

// execute
StatusCode 
CscCondDbAlg::execute(const EventContext& ctx) const {

    ATH_MSG_DEBUG( "execute " << name() );   
 
    // launching Write Cond Handle
    SG::WriteCondHandle<CscCondDbData> writeHandle{m_writeKey, ctx};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
        	  << " In theory this should not be called, but may happen"
        	  << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS; 
    }
    std::unique_ptr<CscCondDbData> writeCdo{std::make_unique<CscCondDbData>()};

    writeCdo->loadParameters(&m_idHelperSvc->cscIdHelper());
    writeCdo->setParameters(m_onlineOfflinePhiFlip);

    // data only
    if(m_isData) {
        //ATH_CHECK(loadDataHv(writeHandle, writeCdo.get(), ctx)); // keep for future development
    }

    // both data and MC
    ATH_CHECK(loadDataF001   (writeHandle, writeCdo.get(), ctx)); 
    ATH_CHECK(loadDataNoise  (writeHandle, writeCdo.get(), ctx)); 
    ATH_CHECK(loadDataPed    (writeHandle, writeCdo.get(), ctx)); 
    if (m_pslopeFromDB) ATH_CHECK(loadDataPSlope (writeHandle, writeCdo.get(), ctx)); 
    ATH_CHECK(loadDataRMS    (writeHandle, writeCdo.get(), ctx)); 
    ATH_CHECK(loadDataStatus (writeHandle, writeCdo.get(), ctx)); 

	if(!m_isOnline) {
        ATH_CHECK(loadDataT0Base (writeHandle, writeCdo.get(), ctx));
        ATH_CHECK(loadDataT0Phase(writeHandle, writeCdo.get(), ctx));
    }

    if (writeHandle.record(std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record CscCondDbData " << writeHandle.key() 
		    << " with EventRange " << writeHandle.getRange()
		    << " into Conditions Store");
      return StatusCode::FAILURE;
    }		  
    ATH_MSG_DEBUG("Recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

    return StatusCode::SUCCESS;
}


// loadDataHv
StatusCode
CscCondDbAlg::loadDataHv(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
  
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_hv, ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    writeHandle.addDependency(readHandle);
 
    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << writeHandle.getRange());

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
      
                Identifier ChamberId   = m_idHelperSvc->cscIdHelper().elementID(chamber_name, eta, phi);
                Identifier WireLayerId = m_idHelperSvc->cscIdHelper().channelID(ChamberId, 1, wirelayer,1,1);
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
CscCondDbAlg::loadDataF001(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_f001, ctx};
    writeHandle.addDependency(readHandle);
    return loadData(writeCdo, *readHandle, "f001");
}

// loadDataNoise
StatusCode
CscCondDbAlg::loadDataNoise(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_noise, ctx};
    writeHandle.addDependency(readHandle);
    return loadData(writeCdo, *readHandle, "noise");
}

// loadDataPed
StatusCode
CscCondDbAlg::loadDataPed(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_ped, ctx};
  writeHandle.addDependency(readHandle);
  return loadData(writeCdo, *readHandle, "ped");
}

// loadDataPSlope
StatusCode
CscCondDbAlg::loadDataPSlope(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_pslope, ctx};
    writeHandle.addDependency(readHandle);
    return loadData(writeCdo, *readHandle, "pslope");
}

// loadDataRMS
StatusCode
CscCondDbAlg::loadDataRMS(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_rms, ctx};
    writeHandle.addDependency(readHandle);
    return loadData(writeCdo, *readHandle, "rms");
}

// loadDataStatus
StatusCode
CscCondDbAlg::loadDataStatus(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_status, ctx};
    writeHandle.addDependency(readHandle);
    return loadData(writeCdo, *readHandle, "status");
}

// loadDataT0Base
StatusCode
CscCondDbAlg::loadDataT0Base(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_t0base, ctx};
  writeHandle.addDependency(readHandle);
  return loadData(writeCdo, *readHandle, "t0base");
}

// loadDataT0Phase
StatusCode
CscCondDbAlg::loadDataT0Phase(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_t0phase, ctx};
  writeHandle.addDependency(readHandle);
  return loadData(writeCdo, *readHandle, "t0phase", true);
}


// loadData
StatusCode
CscCondDbAlg::loadData(CscCondDbData* writeCdo, const CondAttrListCollection* readCdo, const std::string parName, bool parAsm) const {

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

        if(version == "02-00" && parAsm) {
            if(!cacheASM(data, writeCdo, parName).isSuccess()) {
                ATH_MSG_ERROR("Failed caching from COOL string version 02-00 in ASM format");
                return StatusCode::FAILURE;
            }
        }
        else if(version == "02-00") {
            if(!cache(data, writeCdo, parName).isSuccess()) {
                ATH_MSG_ERROR("Failed caching from COOL string version 02-00");
                return StatusCode::FAILURE;
            }
        }
        else {
            // Old version was treated as an actual number rather than string. It was always
            // set to 1 or sometimes 1.00000, so we convert to integer here and check
            ATH_MSG_WARNING("Don't recognize CSC COOL string version " << version << ". Will treat as default version " << m_defaultDatabaseReadVersion);
            if(m_defaultDatabaseReadVersion == "02-00" && parAsm) {
                if(!cacheASM(data, writeCdo, parName).isSuccess()) {
                    ATH_MSG_ERROR("Failed caching from COOL string version 02-00 in ASM format");
                    return StatusCode::FAILURE;
                }
            }
            else if(m_defaultDatabaseReadVersion == "02-00"){
                if(!cache(data, writeCdo, parName).isSuccess()) {
                    ATH_MSG_ERROR("Failed caching from COOL string version 02-00");
                    return StatusCode::FAILURE;
                }
            }
            else {
                ATH_MSG_FATAL("Did not recognize CSC COOL string version " << version << ". Currently, only version 02-00 is supported. The keys of the database have to be geometrical CSC hashes in hex format.");
                return StatusCode::FAILURE;
            }
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode 
CscCondDbAlg::cache(std::string data, CscCondDbData* writeCdo, const std::string parName) const {

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
		ATH_MSG_VERBOSE("cache() - current element " << valueStr);
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

StatusCode 
CscCondDbAlg::cacheASM(std::string data, CscCondDbData* writeCdo, const std::string parName) const {

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
		ATH_MSG_VERBOSE("cacheASM() - current element " << valueStr);
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
            ATH_MSG_FATAL("Failure of getAsmScope in cacheASM.");
            return StatusCode::FAILURE;
        }

        //  Now for given asmID, loop over strip and layer
        unsigned int index = 0;
        Identifier chanId;
        IdentifierHash hashIdentifier;
        for(int iStrip = stripSince; iStrip < stripUntil; iStrip++){ 
            for(int iLayer = layerSince; iLayer < layerUntil; iLayer++){
                // The following call of channelID with check=true ensures that the identifier is checked to be physically valid.
                // This is currently required to be checked when running with layouts which do not contain all CSCs anymore, since the
                // CSCCool database contains still all CSCs. A clean fix would be to have a dedicated database for every layout.
                bool isValid = true;
                chanId = m_idHelperSvc->cscIdHelper().channelID(stationName, stationEta, stationPhi, 
                                                             chamberLayer, iLayer, measuresPhi, iStrip, true, &isValid);
                static bool conversionFailPrinted = false;
                if (!isValid) {
                    if (!conversionFailPrinted) {
                        ATH_MSG_WARNING("Failed to retrieve offline identifier from ASM cool string " << chanAddress
                                              << ". This is likely due to the fact that the CSCCool database contains "
                                              << "more entries than the detector layout.");
                        conversionFailPrinted = true;
                    }
                    continue;
                }
                if (m_idHelperSvc->cscIdHelper().get_channel_hash(chanId, hashIdentifier)) {
                    ATH_MSG_WARNING("Failed to retrieve channel hash for Identifier " << chanId.get_compact());
                }
                index = (int) hashIdentifier;
                if (index==UINT_MAX) continue;

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
CscCondDbAlg::getAsmScope(int asmNum, int &measuresPhi,  int & layerSince, int & layerUntil, int & stripSince , int & stripUntil) const {
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
CscCondDbAlg::recordParameter(unsigned int chanAddress, std::string data, CscCondDbData* writeCdo, const std::string parName) const {

    // retrieve channel hash
    Identifier chamberId;
    Identifier channelId;
    if(!writeCdo->onlineToOfflineIds(&m_idHelperSvc->cscIdHelper(), chanAddress, chamberId, channelId).isSuccess()) {
        // if onlineToOfflineIds does not return SUCCESS, the underlying reason was alrady printed there, so no need to also print a WARNING here
        return StatusCode::SUCCESS;
    }
    
    IdentifierHash chanHash;
    if (m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, chanHash)) {
        ATH_MSG_WARNING("recordParameter(): Failed to retrieve channel Identifier hash for Identifier "<<channelId.get_compact()<<". Not recording parameter...");
        return StatusCode::SUCCESS;
    }
    
    // record parameter
    return recordParameter(chanHash, data, writeCdo, parName);   
}


// recordParameter
StatusCode
CscCondDbAlg::recordParameter(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo, const std::string parName) const {
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
CscCondDbAlg::recordParameterF001(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo) const {

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelF001(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterNoise
StatusCode
CscCondDbAlg::recordParameterNoise(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo) const {

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelNoise(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterPed
StatusCode
CscCondDbAlg::recordParameterPed(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo) const {

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelPed(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterPSlope
StatusCode
CscCondDbAlg::recordParameterPSlope(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo) const {
    if (m_pslopeFromDB) {
        float token;
        if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
        writeCdo->setChannelPSlope(chanHash, token);
    } else {
        // just set plsope to m_pslope for every channel
        writeCdo->setChannelPSlope(chanHash, m_pslope);
    }
    return StatusCode::SUCCESS;
}


// recordParameterRMS
StatusCode
CscCondDbAlg::recordParameterRMS(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo) const {

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelRMS(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterStatus
StatusCode
CscCondDbAlg::recordParameterStatus(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo) const {

    unsigned int token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelStatus(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterT0Base
StatusCode
CscCondDbAlg::recordParameterT0Base(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo) const {

    float token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelT0Base(chanHash, token);
    return StatusCode::SUCCESS;
}


// recordParameterT0Phase
StatusCode
CscCondDbAlg::recordParameterT0Phase(IdentifierHash chanHash, std::string data, CscCondDbData* writeCdo) const {

    bool token;
    if(getParameter(chanHash, data, token).isFailure()) return StatusCode::FAILURE;
    writeCdo->setChannelT0Phase(chanHash, token);
    return StatusCode::SUCCESS;
}


/*
keep for future development:

// loadDataDeadChambers
StatusCode
CscCondDbAlg::loadDataDeadChambers(writeHandle_t & writeHandle, CscCondDbData* writeCdo, const EventContext& ctx) const {
  
    ATH_CHECK(m_readKey_folder_da_chambers.initialize());
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_chambers, ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
  
    EventIDRange range; 
    if ( !readHandle.range(range) ) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 

    //intersect validity range of this obj with the validity of already-loaded objs
    rangeW = EventIDRange::intersect(range, rangeW);
  
    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << range << ", range of output is " << rangeW);

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

        Identifier ChamberId = m_idHelperSvc->cscIdHelper().elementID(chamber_name, eta, phi);
		writeCdo->setDeadStation(deadChambers[i], ChamberId);
    }

    return StatusCode::SUCCESS;
}
*/

