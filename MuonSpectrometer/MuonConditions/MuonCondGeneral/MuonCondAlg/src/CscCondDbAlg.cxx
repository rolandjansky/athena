/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/CscCondDbAlg.h"
#include <zlib.h>

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
    ATH_CHECK(m_readKey_folder_da_hv  .initialize());
    ATH_CHECK(m_readKey_folder_da_stat.initialize());

    if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
      ATH_MSG_FATAL("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}


// finalize
StatusCode
CscCondDbAlg::finalize(){
  return StatusCode::SUCCESS;
}


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
    EventIDRange rangeW;
    StatusCode sc  = StatusCode::SUCCESS;
    StatusCode sc0 = StatusCode::SUCCESS;

    // retrieving data
    if(m_isData) {
        //sc0 = loadDataHv  (rangeW, writeCdo); if(sc0.isFailure()) {sc = sc0;}
        sc0 = loadDataStat(rangeW, writeCdo); if(sc0.isFailure()) {sc = sc0;}
    }
    else {
        sc0 = loadDataStat(rangeW, writeCdo); if(sc0.isFailure()) {sc = sc0;}
    }

    if(sc.isFailure() || sc0.isFailure()){
        ATH_MSG_WARNING("Could not read data from the DB");
        return StatusCode::FAILURE;
    }

    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record CscCondDbData " << writeHandle.key() 
  		  << " with EventRange " << rangeW
  		  << " into Conditions Store");
      return StatusCode::FAILURE;
    }		  
    ATH_MSG_INFO("Recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

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
  
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of input is " << rangeW);

    CondAttrListCollection::const_iterator itr;
	std::map<Identifier, int> layerMap;
    int hv_state, lv_state, hv_setpoint0, hv_setpoint1;

    unsigned int chan_index = 0; 
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

        unsigned int chanNum      = readCdo->chanNum (chan_index);
        std::string csc_chan_name = readCdo->chanName(chanNum);

        //itr=readCdo->chanAttrListPair(chanNum);
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



// loadDataStat
StatusCode
CscCondDbAlg::loadDataStat(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
  
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_stat};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
       return StatusCode::FAILURE; 
    } 
  
    if ( !readHandle.range(rangeW) ) {
       ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
  
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of input is " << rangeW);
 
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
            if(!cacheVersion1(data, writeCdo).isSuccess()) {
                ATH_MSG_ERROR("Failed caching from COOL string version 1");
                return StatusCode::FAILURE;
            }
        }
        else if(version == "02-00") {
            if(!cacheVersion2(data, writeCdo).isSuccess()) {
                ATH_MSG_ERROR("Failed caching from COOL string version 02-00");
                return StatusCode::FAILURE;
            }
        }
        else {
            // Old version was treated as an actual number rather than string. It was always
            // set to 1 or sometimes 1.00000, so we convert to integer here and check
            ATH_MSG_WARNING("Don't recognize CSC COOL string version " << version << ". Will treat as default version " << m_defaultDatabaseReadVersion);
            if(m_defaultDatabaseReadVersion == "1"){
                if(!cacheVersion1(data, writeCdo).isSuccess()) {
                    ATH_MSG_ERROR("Failed caching from COOL string version 1");
                    return StatusCode::FAILURE;
                }
            }
            else if(m_defaultDatabaseReadVersion == "02-00"){
                if(!cacheVersion2(data, writeCdo).isSuccess()) {
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
CscCondDbAlg::cacheVersion1(std::string data, std::unique_ptr<CscCondDbData>& writeCdo){

	// careful, this is untested yet!

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
			IdContext context = m_idHelper->cscIdHelper().channel_context();
            m_idHelper->cscIdHelper().get_id((IdentifierHash)index, chanId, &context);
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
CscCondDbAlg::cacheVersion2(std::string data, std::unique_ptr<CscCondDbData>& writeCdo) {

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

		// next element is the status bit
        unsigned int token;
        iss >> token;

		ATH_MSG_DEBUG("channel address: " << chanAddress << " and token " << token);

        //remaining categories need offline identifiers
        Identifier chamberId;
        Identifier channelId;
        if(!onlineToOfflineIds(chanAddress, chamberId, channelId).isSuccess())
            ATH_MSG_ERROR("Cannon get offline Ids from online Id" << std::hex << chanAddress << std::dec);
		
        IdentifierHash chanHash;
        m_idHelper->cscIdHelper().get_channel_hash(channelId, chanHash);
        writeCdo->setChannelStatus(chanHash, token);
        chanAddress = 0;

    }


    return StatusCode::SUCCESS;
}



// onlineToOfflineIds
StatusCode 
CscCondDbAlg::onlineToOfflineIds(const unsigned int & onlineId, Identifier &elementId, Identifier &channelId) const {
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




//// loadDataDeadChambers
//StatusCode
//CscCondDbAlg::loadDataDeadChambers(EventIDRange & rangeW, std::unique_ptr<CscCondDbData>& writeCdo){
//  
//    ATH_CHECK(m_readKey_folder_da_chambers.initialize());
//    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_chambers};
//    const CondAttrListCollection* readCdo{*readHandle}; 
//    if(readCdo==0){
//      ATH_MSG_ERROR("Null pointer to the read conditions object");
//      return StatusCode::FAILURE; 
//    } 
//  
//    if ( !readHandle.range(rangeW) ) {
//      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
//      return StatusCode::FAILURE;
//    } 
//  
//    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
//    ATH_MSG_INFO("Range of input is " << rangeW);
//
//    std::vector<std::string> goodChambers;
//    std::vector<std::string> deadChambers;
//
//    CondAttrListCollection::const_iterator itr;
//    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {
//
//        const coral::AttributeList& atr = itr->second;
//        std::string chamber_enabled = *(static_cast<const std::string*>((atr["enabledChambers"]).addressOfData()));
//    
//        std::string delimiter = " ";
//        std::vector<std::string> tokens;
//        MuonCalib::MdtStringUtils::tokenize(chamber_enabled,tokens,delimiter);
//    
//        for (unsigned int i=0; i<tokens.size(); i++) goodChambers.push_back(tokens[i]);
//    }
//  
//    std::string chamber_all[] = {"A01","A02","A03","A04","A05","A06","A07","A08","A09","A10","A11","A12","A13","A14","A15","A16",
//                                 "C01","C02","C03","C04","C05","C06","C07","C08","C09","C10","C11","C12","C13","C14","C15","C16"};
//    std::vector<std::string> chamber_v(chamber_all,chamber_all+32);
//    sort(chamber_v.begin(), chamber_v.end());
//    
//    for(unsigned int i=0; i<chamber_v.size(); ++i){
//        if(!binary_search(goodChambers.begin(), goodChambers.end(), chamber_v[i]))
//            deadChambers.push_back(chamber_v[i]);
//    } 
//
//    for(unsigned int i=0; i<deadChambers.size(); ++i){
//        int eta = 0; 
//        std::string eta_side = deadChambers[i].substr(0,1);
//        if(eta_side == "A") eta = +1;
//        if(eta_side == "C") eta = -1;
//
//        int phi = 0;
//        std::string sector_side = deadChambers[i].substr(2,4);
//        if(sector_side == "01" || sector_side == "02") phi=1;
//        if(sector_side == "03" || sector_side == "04") phi=2;
//        if(sector_side == "05" || sector_side == "06") phi=3;
//        if(sector_side == "07" || sector_side == "08") phi=4;
//        if(sector_side == "09" || sector_side == "10") phi=5;
//        if(sector_side == "11" || sector_side == "12") phi=6;
//        if(sector_side == "13" || sector_side == "14") phi=7;
//        if(sector_side == "15" || sector_side == "16") phi=8;
//
//        std::string chamber_name = "";
//        if(sector_side == "01" || sector_side == "03" ||sector_side == "05" ||sector_side == "07" || sector_side == "09" || sector_side == "11" || sector_side == "13" || sector_side == "15" ) chamber_name = "CSL";
//        if(sector_side == "02" || sector_side == "04" || sector_side == "06"|| sector_side == "08" || sector_side == "10"|| sector_side == "12"|| sector_side == "14"|| sector_side == "16") chamber_name = "CSS";
//
//        Identifier ChamberId = m_idHelper->cscIdHelper().elementID(chamber_name, eta, phi);
//		writeCdo->setDeadStation(deadChambers[i], ChamberId);
//    }
//
//    return StatusCode::SUCCESS;
//}


