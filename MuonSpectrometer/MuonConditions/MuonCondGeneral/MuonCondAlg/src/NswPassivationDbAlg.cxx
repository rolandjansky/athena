/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/NswPassivationDbAlg.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "CoralUtilities/blobaccess.h"
#include "MuonReadoutGeometry/NswPassivationDbData.h"


// Initialize
StatusCode
NswPassivationDbAlg::initialize(){

	// retrievals
	ATH_MSG_DEBUG( "initializing " << name() );				
	ATH_CHECK(m_condSvc	.retrieve());
	ATH_CHECK(m_idHelperSvc.retrieve());

	// read keys
	ATH_CHECK(m_readKey_data_mm.initialize(!m_readKey_data_mm.empty()));

	// write keys	
	ATH_CHECK(m_writeKey.initialize());

	// register write handles
	if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
	  ATH_MSG_FATAL("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
	  return StatusCode::FAILURE;
	}

	return StatusCode::SUCCESS;
}


// execute
StatusCode 
NswPassivationDbAlg::execute(const EventContext& ctx) const {

	ATH_MSG_DEBUG( "execute " << name() );   
	
	// retrieving data
	ATH_CHECK(loadData(ctx));

	// return	
	return StatusCode::SUCCESS;
}


// loadData
StatusCode
NswPassivationDbAlg::loadData(const EventContext& ctx) const {

	// set up write handle
	SG::WriteCondHandle<NswPassivationDbData> writeHandle{m_writeKey, ctx};
	if (writeHandle.isValid()) {
		ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
			  << " In theory this should not be called, but may happen"
			  << " if multiple concurrent events are being processed out of order.");
		return StatusCode::SUCCESS; 
	}
	std::unique_ptr<NswPassivationDbData> writeCdo{std::make_unique<NswPassivationDbData>(m_idHelperSvc->mmIdHelper())};

	// set up read handle
	SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_data_mm, ctx};
	const CondAttrListCollection* readCdo{*readHandle}; 
	if(readCdo==nullptr){
	  ATH_MSG_ERROR("Null pointer to the read conditions object");
	  return StatusCode::FAILURE; 
	} 
	writeHandle.addDependency(readHandle);
	ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
	ATH_MSG_DEBUG("Range of input is " << readHandle.getRange() << ", range of output is " << writeHandle.getRange());

	// iterate through data
	CondAttrListCollection::const_iterator itr;
	unsigned int nObjs = 0;
	for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

		// retrieve data
		const coral::AttributeList& atr = itr->second;
		std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

		// unwrap the json and process the data
		unsigned int nChns = 0; 
		nlohmann::json yy = nlohmann::json::parse(data);
		for (auto &kt : yy.items()) {
			nlohmann::json jt = kt.value();
			Identifier channelId = buildChannelId(jt["stationName"], jt["stationEta"], jt["stationPhi"], jt["multiLayer"], jt["gasGap"], jt["pcbPos"]);
			writeCdo->setData(channelId, jt["pcbPos"], jt["indiv"], jt["extra"], jt["position"]);
			++nChns;
		}
		ATH_MSG_VERBOSE("Retrieved data for "<<nChns<<" channels.");
		++nObjs;
	}
	ATH_MSG_VERBOSE("Retrieved data for "<<nObjs<<" objects.");

	// insert/write data
	if (writeHandle.record(std::move(writeCdo)).isFailure()) {
		ATH_MSG_FATAL("Could not record NswPassivationDbData " << writeHandle.key() 
			  << " with EventRange " << writeHandle.getRange()
			  << " into Conditions Store");
		return StatusCode::FAILURE;
	}		  
	ATH_MSG_DEBUG("Recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

	return StatusCode::SUCCESS;
}


// buildChannelId
Identifier
NswPassivationDbAlg::buildChannelId(const int station, const int stationEta, const int stationPhi, const int multiLayer, const int gasGap, const int pcbPos) const {

	// channelNumber is counted 1-5120 for PCBs 1-5, and 1-3072 for PCBs 6-8
	int pcb  = abs(stationEta)==1 ? pcbPos : pcbPos-5;
	int chnl = (pcb-1)*1024+1;

	Identifier chnlId = m_idHelperSvc->mmIdHelper().channelID(station, stationEta, stationPhi, multiLayer, gasGap, chnl);

	return chnlId;
}

