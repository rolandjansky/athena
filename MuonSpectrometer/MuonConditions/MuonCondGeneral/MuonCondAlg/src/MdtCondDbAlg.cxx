/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/MdtCondDbAlg.h"
#include "AthenaKernel/IOVInfiniteRange.h"

// constructor
MdtCondDbAlg::MdtCondDbAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
    AthReentrantAlgorithm(name, pSvcLocator),
    m_condSvc("CondSvc", name),
    m_condMapTool("MDT_MapConversion")
  {
 
    declareProperty("MDT_MapConversion", m_condMapTool);

    declareProperty("isOnline"         , m_isOnline               );
    declareProperty("isData"           , m_isData                 );
    declareProperty("isRun1"           , m_isRun1                 );
    declareProperty("useRun1SetPoints" , m_checkOnSetPoint = false);
}


// Initialize
StatusCode
MdtCondDbAlg::initialize(){

    ATH_MSG_DEBUG( "initializing " << name() );                
    ATH_CHECK(m_condSvc .retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_writeKey.initialize());
    ATH_CHECK(m_readKey_folder_da_pshv.initialize(!m_readKey_folder_da_pshv.empty() && m_isData && m_isRun1));
    ATH_CHECK(m_readKey_folder_da_psv0.initialize(!m_readKey_folder_da_psv0.empty() && m_isData && m_isRun1 && m_checkOnSetPoint));
    ATH_CHECK(m_readKey_folder_da_psv1.initialize(!m_readKey_folder_da_psv1.empty() && m_isData && m_isRun1 && m_checkOnSetPoint));
    ATH_CHECK(m_readKey_folder_da_pslv.initialize(!m_readKey_folder_da_pslv.empty() && m_isData && m_isRun1));
    ATH_CHECK(m_readKey_folder_da_hv  .initialize(!m_readKey_folder_da_hv.empty() && m_isData && !m_isRun1));
    ATH_CHECK(m_readKey_folder_da_lv  .initialize(!m_readKey_folder_da_lv.empty() && m_isData && !m_isRun1));
    ATH_CHECK(m_readKey_folder_da_droppedChambers.initialize(!m_readKey_folder_da_droppedChambers.empty() && m_isData && m_isRun1));
    ATH_CHECK(m_readKey_folder_mc_droppedChambers.initialize(!m_readKey_folder_mc_droppedChambers.empty() && !m_isData));
    ATH_CHECK(m_readKey_folder_mc_noisyChannels  .initialize(!m_readKey_folder_mc_noisyChannels.empty() && !m_isData));
    // The calls to the functions that use these two are commented out,
    // so don't declare a dependencies on them.
    ATH_CHECK(m_readKey_folder_mc_deadElements   .initialize(false/*!m_readKey_folder_mc_deadElements.empty() && !m_isData*/));
    ATH_CHECK(m_readKey_folder_mc_deadTubes      .initialize(false/*!m_readKey_folder_mc_deadTubes.empty() && !m_isData*/));

    if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
      ATH_MSG_FATAL("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}


// execute
StatusCode 
MdtCondDbAlg::execute(const EventContext& ctx) const {

    ATH_MSG_DEBUG( "execute " << name() );   

	if(m_isOnline) {
    	ATH_MSG_DEBUG( "IsOnline is set to True; nothing to do!" );   
		return StatusCode::SUCCESS;
	}

    // launching Write Cond Handle
	SG::WriteCondHandle<MdtCondDbData> writeHandle{m_writeKey,ctx};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
        	  << " In theory this should not be called, but may happen"
        	  << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS; 
    }
    std::unique_ptr<MdtCondDbData> writeCdo{std::make_unique<MdtCondDbData>()};

    // retrieving data
    if(m_isData && m_isRun1) {
      ATH_CHECK(loadDataPsHv(writeHandle, writeCdo.get(),ctx));
      ATH_CHECK(loadDataPsLv(writeHandle, writeCdo.get(),ctx));
      ATH_CHECK(loadDroppedChambers(writeHandle, writeCdo.get(),ctx,false));
    }
    else if(m_isData && !m_isRun1) {
      ATH_CHECK(loadDataHv(writeHandle, writeCdo.get(),ctx));
      ATH_CHECK(loadDataLv(writeHandle, writeCdo.get(),ctx));
    }
    else {
      ATH_CHECK(loadDroppedChambers(writeHandle, writeCdo.get(),ctx,true));
      ATH_CHECK(loadMcNoisyChannels(writeHandle, writeCdo.get(),ctx));
		//ATH_CHECK(loadMcDeadElements     (rangeW, writeCdo.get(),ctx));// keep for future development 
		//ATH_CEHCK(loadMcDeadTubes        (rangeW, writeCdo.get(),ctx));// keep for future development
    } 

    if (writeHandle.record(std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record MdtCondDbData " << writeHandle.key() 
		    << " with EventRange " << writeHandle.getRange()
		    << " into Conditions Store");
      return StatusCode::FAILURE;
    }		  
    ATH_MSG_DEBUG("Recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

    return StatusCode::SUCCESS;
}


// loadDataPsHv
StatusCode
MdtCondDbAlg::loadDataPsHv(writeHandle_t& wh, MdtCondDbData* writeCdo, const EventContext& ctx) const{
  
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_pshv,ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle);
    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << wh.getRange());


    std::vector<Identifier> cachedDeadMultiLayersId_standby;
    CondAttrListCollection::const_iterator itr;
    unsigned int chan_index=0; 
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

        unsigned int chanNum   = readCdo->chanNum (chan_index);
        const std::string& hv_payload = readCdo->chanName(chanNum   );
        std::string hv_name;
        itr = readCdo->chanAttrListPair(chanNum);
        const coral::AttributeList& atr = itr->second;

        if(atr.size()==1) {
            hv_name = *(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
            std::string delimiter = " ";
            std::vector<std::string> tokens;
            MuonCalib::MdtStringUtils::tokenize(hv_name, tokens, delimiter);
     
            std::string thename; 
            std::string delimiter2 = "_";
            std::vector<std::string> tokens2;
            MuonCalib::MdtStringUtils::tokenize(hv_payload, tokens2, delimiter2);

            if(tokens[0]!="ON" && tokens[0]!="STANDBY" && tokens[0]!="UNKNOWN"){
                int multilayer = atoi(const_cast<char*>(tokens2[3].c_str()));
                std::string chamber_name = tokens2[2];
                Identifier ChamberId     = m_condMapTool->ConvertToOffline(chamber_name);
                Identifier MultiLayerId  = m_idHelperSvc->mdtIdHelper().channelID(ChamberId,multilayer,1,1);
                thename = chamber_name+"_multilayer"+tokens2[3];
                writeCdo->setDeadMultilayer(thename, MultiLayerId);
                writeCdo->setDeadChamber   (ChamberId);
                cachedDeadMultiLayersId_standby.push_back(MultiLayerId);
            }
            if(tokens[0]=="STANDBY"){
                int multilayer = atoi(const_cast<char*>(tokens2[3].c_str()));
                std::string chamber_name = tokens2[2];
                Identifier ChamberId     = m_condMapTool->ConvertToOffline(chamber_name);
                Identifier MultiLayerId  = m_idHelperSvc->mdtIdHelper().channelID(ChamberId,multilayer,1,1);
                thename = chamber_name+"_multilayer"+tokens2[3];
                writeCdo->setDeadMultilayer(thename, MultiLayerId);
                writeCdo->setDeadChamber   (ChamberId);
                cachedDeadMultiLayersId_standby.push_back(MultiLayerId);
            }
        }
        chan_index++;
    }


    // moving on to SetPoints
    if(!m_checkOnSetPoint) return StatusCode::SUCCESS;

    std::map<Identifier, float> chamberML_V1;
    std::map<Identifier, float> chamberML_V0; 
    std::map<Identifier, std::string> mlname;
 
    // V0 handle
    SG::ReadCondHandle<CondAttrListCollection> readHandle_v0{m_readKey_folder_da_psv0,ctx};
    const CondAttrListCollection* readCdo_v0{*readHandle_v0}; 
    if(readCdo_v0==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle_v0);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle_v0.fullKey() << " readCdo->size()= " << readCdo_v0->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle_v0.getRange() << ", range of output is " << wh.getRange());

    // V1
    SG::ReadCondHandle<CondAttrListCollection> readHandle_v1{m_readKey_folder_da_psv1,ctx};
    const CondAttrListCollection* readCdo_v1{*readHandle_v1}; 
    if(readCdo_v1==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle_v1);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle_v1.fullKey() << " readCdo->size()= " << readCdo_v1->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle_v1.getRange() << ", range of output is " << wh.getRange());

    // V0 iteration
    CondAttrListCollection::const_iterator itr_v0;
    unsigned int chan_index_v0=0; 
    for(itr_v0 = readCdo_v0->begin(); itr_v0 != readCdo_v0->end(); ++itr_v0) {
      
        unsigned int chanNum            = readCdo_v0->chanNum  (chan_index_v0);
        std::string setPointsV0_payload = readCdo_v0->chanName(chanNum);
        
        float setPointsV0_name;
           
        itr_v0 = readCdo_v0->chanAttrListPair(chanNum);
        const coral::AttributeList& atr_v0 = itr_v0->second;
            
        if(atr_v0.size()==1){
            setPointsV0_name = *(static_cast<const float*>((atr_v0["readBackSettings_v0"]).addressOfData()));
            std::string delimiter2 = "_";
            std::vector<std::string> tokens2;
            MuonCalib::MdtStringUtils::tokenize(setPointsV0_payload, tokens2, delimiter2);
        
            int multilayer = atoi(const_cast<char*>(tokens2[3].c_str()));
            std::string chamber_name     = tokens2[2];
            std::string thename          = chamber_name+"_"+tokens2[3];
            Identifier ChamberId         = m_condMapTool->ConvertToOffline(chamber_name);
            Identifier MultiLayerId      = m_idHelperSvc->mdtIdHelper().channelID(ChamberId,multilayer,1,1);
            chamberML_V0[MultiLayerId]   = setPointsV0_name;
            mlname[MultiLayerId]         = thename;
      
        }
        chan_index_v0++;
    }

    // V1 iteration
    CondAttrListCollection::const_iterator itr_v1;
    unsigned int chan_index_v1=0; 
    for(itr_v1 = readCdo_v1->begin(); itr_v1 != readCdo_v1->end(); ++itr_v1) {
      
        unsigned int chanNum            = readCdo_v1->chanNum(chan_index_v1);
        std::string setPointsV1_payload = readCdo_v1->chanName(chanNum);
        float setPointsV1_name;
        itr_v1 = readCdo_v1-> chanAttrListPair(chanNum);
        const coral::AttributeList& atr_v1 = itr_v1->second;
    
        if(atr_v1.size()==1){
            setPointsV1_name = *(static_cast<const float*>((atr_v1["readBackSettings_v1"]).addressOfData()));
               
            std::string delimiter2 = "_";
            std::vector<std::string> tokens2;
            MuonCalib::MdtStringUtils::tokenize(setPointsV1_payload, tokens2, delimiter2);
           
            int multilayer = atoi(const_cast<char*>(tokens2[3].c_str()));
            std::string chamber_name     = tokens2[2];
            std::string thename          = chamber_name+"_"+tokens2[3];
            Identifier ChamberId         = m_condMapTool->ConvertToOffline(chamber_name);
            Identifier MultiLayerId      = m_idHelperSvc->mdtIdHelper().channelID(ChamberId,multilayer,1,1);
            chamberML_V1[MultiLayerId]   = setPointsV1_name;
            mlname[MultiLayerId]         = thename;
          
        }
        chan_index_v1++;
    }
    
    // check for chamber standby the correct value of Setpoint V0 vs SetpointV1
    // for chamber StandBy --> V0==V1 to be on
    for (unsigned int vect=0; vect<cachedDeadMultiLayersId_standby.size(); vect++){
        Identifier MultilayerId_ch = cachedDeadMultiLayersId_standby[vect];
        
        if (chamberML_V1.find(MultilayerId_ch)->second == chamberML_V0.find(MultilayerId_ch)->second){
            ATH_MSG_DEBUG( "Chamber has  correct Voltage V1 = "<< chamberML_V1.find(MultilayerId_ch)->second<<" V0=   " << chamberML_V0.find(MultilayerId_ch)->second );
        }
        else{
            ATH_MSG_DEBUG( "Chamber has  wrong correct Voltage V1 = "<< chamberML_V1.find(MultilayerId_ch)->second<<" V0=   " << chamberML_V0.find(MultilayerId_ch)->second);
            ATH_MSG_DEBUG( "Has to be masked!!!");
            writeCdo->setDeadMultilayer(mlname[MultilayerId_ch], MultilayerId_ch);
        }
    }

    return StatusCode::SUCCESS;
}



// loadDataPsLv
StatusCode 
MdtCondDbAlg::loadDataPsLv(writeHandle_t& wh, MdtCondDbData* writeCdo, const EventContext& ctx) const{
  
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_pslv,ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << wh.getRange());

    CondAttrListCollection::const_iterator itr;
    unsigned int chan_index=0; 
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

        unsigned int chanNum = readCdo->chanNum(chan_index);
        std::string hv_name;
        std::string hv_payload = readCdo->chanName(chanNum);
        
        itr = readCdo-> chanAttrListPair(chanNum);
        const coral::AttributeList& atr=itr->second;
    
        if(atr.size()){
            hv_name = *(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
            std::string delimiter = " ";
            std::vector<std::string> tokens;
            MuonCalib::MdtStringUtils::tokenize(hv_name, tokens, delimiter);
            std::string delimiter2 = "_";
            std::vector<std::string> tokens2;
            MuonCalib::MdtStringUtils::tokenize(hv_payload,tokens2,delimiter2);
      
            if(tokens[0]!="ON"){
                std::string chamber_name= tokens2[2];
	            Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                writeCdo->setDeadStation(chamber_name, ChamberId);
            }
        }
        chan_index++;
    }
  
    return StatusCode::SUCCESS;
}



// loadDataHv
StatusCode
MdtCondDbAlg::loadDataHv(writeHandle_t& wh, MdtCondDbData* writeCdo, const EventContext& ctx) const{
  
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_hv,ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << wh.getRange());

    CondAttrListCollection::const_iterator itr;
    unsigned int chan_index=0; 
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

        unsigned int chanNum   = readCdo->chanNum(chan_index);
        std::string hv_name_ml1, hv_name_ml2;
        std::string hv_payload = readCdo->chanName(chanNum);
        float hv_v0_ml1, hv_v0_ml2, hv_v1_ml1, hv_v1_ml2;
        itr = readCdo->chanAttrListPair(chanNum);
        const coral::AttributeList& atr = itr->second;

        if(atr.size()) {
            hv_name_ml1 = *(static_cast<const std::string*>((atr["fsmCurrentState_ML1"]).addressOfData()));
            hv_name_ml2 = *(static_cast<const std::string*>((atr["fsmCurrentState_ML2"]).addressOfData()));
            hv_v0_ml1   = *(static_cast<const float*>((atr["v0set_ML1"]).addressOfData()));
            hv_v1_ml1   = *(static_cast<const float*>((atr["v1set_ML1"]).addressOfData()));
            hv_v0_ml2   = *(static_cast<const float*>((atr["v0set_ML2"]).addressOfData()));
            hv_v1_ml2   = *(static_cast<const float*>((atr["v1set_ML2"]).addressOfData()));

            std::string thename;            
            std::string delimiter2 = "_";
            std::vector<std::string> tokens2;
            MuonCalib::MdtStringUtils::tokenize(hv_payload, tokens2, delimiter2);

            if(hv_name_ml1 !="ON" && hv_name_ml1 !="STANDBY" && hv_name_ml1 !="UNKNOWN"){
	            int multilayer = 1;
	            std::string chamber_name = tokens2[0];
	            Identifier ChamberId     = m_condMapTool->ConvertToOffline(chamber_name);
	            Identifier MultiLayerId  = m_idHelperSvc->mdtIdHelper().channelID(ChamberId,multilayer, 1, 1);
                thename = chamber_name+"_multilayer1";
                writeCdo->setDeadMultilayer(thename, MultiLayerId);
                writeCdo->setDeadChamber   (ChamberId);
            }
      
            if(hv_name_ml1=="STANDBY" && hv_v0_ml1 != hv_v1_ml1){
	            int multilayer = 1;
	            std::string chamber_name = tokens2[0];
	            Identifier ChamberId     = m_condMapTool->ConvertToOffline(chamber_name);
	            Identifier MultiLayerId  = m_idHelperSvc->mdtIdHelper().channelID(ChamberId, multilayer, 1, 1);
                thename = chamber_name+"_multilayer1";
                writeCdo->setDeadMultilayer(thename, MultiLayerId);
                writeCdo->setDeadChamber   (ChamberId);
            }

            if(hv_name_ml2 !="ON" && hv_name_ml2 !="STANDBY" && hv_name_ml2 !="UNKNOWN"){
	            int multilayer = 2;
	            std::string chamber_name = tokens2[0];
	            Identifier ChamberId     = m_condMapTool->ConvertToOffline(chamber_name);
	            Identifier MultiLayerId  = m_idHelperSvc->mdtIdHelper().channelID(ChamberId, multilayer, 1, 1);
                thename = chamber_name+"_multilayer2";
                writeCdo->setDeadMultilayer(thename, MultiLayerId);
                writeCdo->setDeadChamber   (ChamberId);
            }
      
            if(hv_name_ml2=="STANDBY" && hv_v0_ml2 != hv_v1_ml2){
	            int multilayer = 2;
	            std::string chamber_name = tokens2[0];
	            Identifier ChamberId     = m_condMapTool->ConvertToOffline(chamber_name);
	            Identifier MultiLayerId  = m_idHelperSvc->mdtIdHelper().channelID(ChamberId, multilayer, 1, 1);
                thename = chamber_name+"_multilayer2";
                writeCdo->setDeadMultilayer(thename, MultiLayerId);
                writeCdo->setDeadChamber   (ChamberId);
            }

            if(hv_name_ml2 !="ON" && hv_name_ml2 !="STANDBY" && hv_name_ml2 !="UNKNOWN" && hv_name_ml1 !="ON" && hv_name_ml1 !="STANDBY" && hv_name_ml1 !="UNKNOWN" ){
                std::string chamber_name = tokens2[0];
                Identifier ChamberId     = m_condMapTool->ConvertToOffline(chamber_name);
                writeCdo->setDeadStation(chamber_name, ChamberId);
            }
            if(hv_name_ml2=="STANDBY" && hv_v0_ml2 != hv_v1_ml2 && hv_name_ml1=="STANDBY" && hv_v0_ml1 != hv_v1_ml1){
                std::string chamber_name = tokens2[0];
                Identifier ChamberId     = m_condMapTool->ConvertToOffline(chamber_name);
                writeCdo->setDeadStation(chamber_name, ChamberId);
            }
        }  
        chan_index++;
    }
    return StatusCode::SUCCESS;
}


// loadDataLv
StatusCode 
MdtCondDbAlg::loadDataLv(writeHandle_t& wh, MdtCondDbData* writeCdo, const EventContext& ctx) const{

  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_da_lv,ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << wh.getRange());

    CondAttrListCollection::const_iterator itr;
    unsigned int chan_index=0; 
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {
    
        unsigned int chanNum = readCdo->chanNum(chan_index);
        std::string hv_name;
        std::string hv_payload = readCdo->chanName(chanNum);
        
        itr = readCdo->chanAttrListPair(chanNum);
        const coral::AttributeList& atr = itr->second;
    
        if(atr.size()){
            hv_name = *(static_cast<const std::string*>((atr["fsmCurrentState_LV"]).addressOfData()));
            std::string delimiter = " ";
            std::vector<std::string> tokens;
            MuonCalib::MdtStringUtils::tokenize(hv_name, tokens, delimiter);
            std::string delimiter2 = "_";
            std::vector<std::string> tokens2;
            MuonCalib::MdtStringUtils::tokenize(hv_payload,tokens2,delimiter2);
      
            if(tokens[0]!="ON"){
                std::string chamber_name= tokens2[0];
	            Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                writeCdo->setDeadStation(chamber_name, ChamberId);
            }
        }
        chan_index++;
    }
  
    return StatusCode::SUCCESS;
  
}



//loadDataDroppedChambers
StatusCode 
MdtCondDbAlg::loadDroppedChambers(writeHandle_t& wh, MdtCondDbData* writeCdo, const EventContext& ctx, bool isMC) const {

  SG::ReadCondHandle<CondAttrListCollection> readHandle{ (isMC ? m_readKey_folder_mc_droppedChambers : 
                                                                  m_readKey_folder_da_droppedChambers),
                                                                  ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle);
    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << wh.getRange());

    CondAttrListCollection::const_iterator itr;
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {
        const coral::AttributeList& atr = itr->second;
        std::string chamber_dropped;
        chamber_dropped = *(static_cast<const std::string*>((atr["Chambers_disabled"]).addressOfData()));
    
        std::string delimiter = " ";
        std::vector<std::string> tokens;
        MuonCalib::MdtStringUtils::tokenize(chamber_dropped,tokens,delimiter);
        for (unsigned int i=0; i<tokens.size(); i++) {
            if(tokens[i]!="0"){
	            std::string chamber_name = tokens[i];
	            Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                writeCdo->setDeadStation(chamber_name, ChamberId);
            }
        }
    }
    return StatusCode::SUCCESS;
}


// loadMcDeadElements
StatusCode
MdtCondDbAlg::loadMcDeadElements(writeHandle_t& wh, MdtCondDbData* writeCdo, const EventContext& ctx) const {

  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_mc_deadElements,ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << wh.getRange());

    CondAttrListCollection::const_iterator itr;
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {
    
        const coral::AttributeList& atr = itr->second;
        std::string chamber_name, list_mlayer, list_layer, list_tube;

        chamber_name = *(static_cast<const std::string*>((atr["Chambers_Name"  ]).addressOfData()));
        list_mlayer  = *(static_cast<const std::string*>((atr["Dead_multilayer"]).addressOfData()));
        list_layer   = *(static_cast<const std::string*>((atr["Dead_layer"     ]).addressOfData()));
        list_tube    = *(static_cast<const std::string*>((atr["Dead_tube"      ]).addressOfData()));
 
        std::string thename; 
        std::string delimiter = " ";
        std::vector<std::string> tokens;
        std::vector<std::string> tokens_mlayer;
        std::vector<std::string> tokens_layer;
        Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
        MuonCalib::MdtStringUtils::tokenize(list_tube  , tokens       , delimiter);
        MuonCalib::MdtStringUtils::tokenize(list_mlayer, tokens_mlayer, delimiter);
        MuonCalib::MdtStringUtils::tokenize(list_layer , tokens_layer , delimiter);
    
        for(unsigned int i=0; i<tokens.size(); i++){
            if(tokens[i]!="0"){
                int ml    = atoi(const_cast<char*>((tokens[i].substr(0,1)).c_str()));
                int layer = atoi(const_cast<char*>((tokens[i].substr(1,2)).c_str()));
                int tube  = atoi(const_cast<char*>((tokens[i].substr(2  )).c_str()));
                Identifier ChannelId = m_idHelperSvc->mdtIdHelper().channelID(ChamberId, ml, layer, tube);
                thename = chamber_name+"_"+tokens[i];
                writeCdo->setDeadTube   (thename, ChannelId);
                writeCdo->setDeadChamber(ChamberId);
            }
        }

        for(unsigned int i=0; i<tokens_mlayer.size(); i++){
            if(tokens_mlayer[i]!="0"){
                int ml = atoi(const_cast<char*>((tokens_mlayer[i].substr(0)).c_str()));
                Identifier ChannelId = m_idHelperSvc->mdtIdHelper().channelID(ChamberId,ml,1,1);
                thename = chamber_name+"_"+tokens[i];
                writeCdo->setDeadMultilayer(thename, ChannelId);
                writeCdo->setDeadChamber   (ChamberId);
            }
        }
      
        for(unsigned int i=0; i<tokens_layer.size(); i++) {
            if(tokens_layer[i]!="0"){
                int ml    = atoi(const_cast<char*>((tokens_layer[i].substr(0,1)).c_str()));
                int layer = atoi(const_cast<char*>((tokens_layer[i].substr(1  )).c_str()));
                Identifier ChannelId = m_idHelperSvc->mdtIdHelper().channelID(ChamberId, ml, layer, 1);
                thename = chamber_name+"_"+tokens[i];
                writeCdo->setDeadLayer  (thename, ChannelId);
                writeCdo->setDeadChamber(ChamberId);
            }
        }
    }

    return StatusCode::SUCCESS;
}



//loadMcDeadTubes
StatusCode 
MdtCondDbAlg::loadMcDeadTubes(writeHandle_t& wh, MdtCondDbData* writeCdo, const EventContext& ctx) const {

  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_mc_deadTubes,ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange()  << ", range of output is " << wh.getRange());

    CondAttrListCollection::const_iterator itr;
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

        const coral::AttributeList& atr=itr->second;
        
        std::string chamber_name;
        std::string dead_tube;
        std::string tube_list;
        dead_tube    = *(static_cast<const std::string*>((atr["DeadTube_List"]).addressOfData()));
        chamber_name = *(static_cast<const std::string*>((atr["Chamber_Name" ]).addressOfData()));

        std::string thename;
        std::vector<std::string> tokens;
        std::string delimiter = " ";
        MuonCalib::MdtStringUtils::tokenize(dead_tube, tokens, delimiter);
        Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);

        for (unsigned int i=0; i<tokens.size(); i++) {
            int ml    = atoi(const_cast<char*>((tokens[i].substr(0,1)).c_str()));
            int layer = atoi(const_cast<char*>((tokens[i].substr(1,2)).c_str()));
            int tube  = atoi(const_cast<char*>((tokens[i].substr(2  )).c_str()));
            thename   = chamber_name+"_"+tokens[i];
            tube_list = tokens[i]+".";
            Identifier ChannelId = m_idHelperSvc->mdtIdHelper().channelID(ChamberId, ml, layer, tube); 
		    writeCdo->setDeadTube(thename, ChannelId);
        }
		writeCdo->setDeadChamber(ChamberId);
    }
  
    return StatusCode::SUCCESS;
}



// loadMcNoisyChannels
StatusCode 
MdtCondDbAlg::loadMcNoisyChannels(writeHandle_t& wh, MdtCondDbData* writeCdo, const EventContext& ctx) const {

  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_mc_noisyChannels,ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
    wh.addDependency(readHandle);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << wh.getRange() );

    for(const auto& itr: *readCdo) {

        unsigned int chanNum = itr.first;
        std::string hv_name;
        std::string hv_payload = readCdo->chanName(chanNum);

        const coral::AttributeList& atr = itr.second;
        
        if(atr.size()){
            hv_name = *(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
            std::string delimiter = " ";
            std::vector<std::string> tokens;
            MuonCalib::MdtStringUtils::tokenize(hv_name, tokens, delimiter);
            std::string delimiter2 = "_";
            std::vector<std::string> tokens2;
            MuonCalib::MdtStringUtils::tokenize(hv_payload,tokens2,delimiter2);
      
            if(tokens[0]!="ON"){
                std::string chamber_name= tokens2[2];
	              Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                writeCdo->setDeadStation(chamber_name, ChamberId);
            }
        }
    }
	return StatusCode::SUCCESS;
}



