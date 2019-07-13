/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/TgcCondDbAlg.h"
#include <zlib.h>

// constructor
TgcCondDbAlg::TgcCondDbAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
    AthAlgorithm(name, pSvcLocator),
    m_condSvc("CondSvc", name),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
  {
 
    declareProperty("IdHelper"         , m_idHelper   );

    declareProperty("isOnline"                 , m_isOnline                                                );
    declareProperty("isData"                   , m_isData                                                  );
    declareProperty("isRun1"                   , m_isRun1                                                  );
}


// Initialize
StatusCode
TgcCondDbAlg::initialize(){

    ATH_MSG_DEBUG( "initializing " << name() );                
    ATH_CHECK(m_condSvc .retrieve());
    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_writeKey.initialize());
    ATH_CHECK(m_readKey_folder_detectorStatus.initialize());

    if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
      ATH_MSG_FATAL("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}


// finalize
StatusCode
TgcCondDbAlg::finalize(){
  return StatusCode::SUCCESS;
}


// execute
StatusCode 
TgcCondDbAlg::execute(){

    ATH_MSG_DEBUG( "execute " << name() );   

	if(m_isOnline) {
		ATH_MSG_DEBUG( "IsOnline is set to True; nothing to do!" );   
		return StatusCode::SUCCESS;
	}
 
    // launching Write Cond Handle
    SG::WriteCondHandle<TgcCondDbData> writeHandle{m_writeKey};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
        	  << " In theory this should not be called, but may happen"
        	  << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS; 
    }
    std::unique_ptr<TgcCondDbData> writeCdo{std::make_unique<TgcCondDbData>()};
    EventIDRange rangeW;
    StatusCode sc  = StatusCode::SUCCESS;
    StatusCode sc0 = StatusCode::SUCCESS;

    // retrieving data
    sc0 = loadDetectorStatus(rangeW, writeCdo); if(sc0.isFailure()) {sc = sc0;}
    if(sc.isFailure()){
        ATH_MSG_WARNING("Could not read data from the DB");
        return StatusCode::FAILURE;
    }

    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record TgcCondDbData " << writeHandle.key() 
  		  << " with EventRange " << rangeW
  		  << " into Conditions Store");
      return StatusCode::FAILURE;
    }		  
    ATH_MSG_INFO("Recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

    return StatusCode::SUCCESS;
}


// loadDetectorStatus
StatusCode
TgcCondDbAlg::loadDetectorStatus(EventIDRange & rangeW, std::unique_ptr<TgcCondDbData>& writeCdo){
  
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_detectorStatus};
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

		const coral::AttributeList& atr=itr->second;
		int detector_status;
		
		detector_status=*(static_cast<const int*>((atr["detector_status"]).addressOfData()));
		ATH_MSG_DEBUG("TGC detector status is " << detector_status);
		
		if (detector_status!=0){
		    int channum=itr->first;
		    Identifier chamberId = m_idHelper->tgcIdHelper().elementID(Identifier(channum));
			writeCdo->setDeadStation(chamberId);
		}
    }

    return StatusCode::SUCCESS;
}


