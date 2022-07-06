/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/NswCalibDbAlg.h"

#include "TTree.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "CoralUtilities/blobaccess.h"
#include "MuonCondData/NswCalibDbTimeChargeData.h"
#include "MuonCondData/NswCalibDbThresholdData.h"
#include "MuonNSWCommonDecode/NSWElink.h"
#include "MuonNSWCommonDecode/NSWResourceId.h"
#include "MuonNSWCommonDecode/NSWOfflineHelper.h"


// Initialize
StatusCode
NswCalibDbAlg::initialize(){

	// retrievals
	ATH_MSG_DEBUG( "initializing " << name() );                
	ATH_CHECK(m_condSvc	   .retrieve());
	ATH_CHECK(m_idHelperSvc.retrieve());

	// initialize read keys
	ATH_CHECK(m_readKey_mm_sidea_tdo  .initialize(!m_readKey_mm_sidea_tdo  .empty()             ));
	ATH_CHECK(m_readKey_mm_sidec_tdo  .initialize(!m_readKey_mm_sidec_tdo  .empty()             ));
	ATH_CHECK(m_readKey_mm_sidea_pdo  .initialize(!m_readKey_mm_sidea_pdo  .empty()             ));
	ATH_CHECK(m_readKey_mm_sidec_pdo  .initialize(!m_readKey_mm_sidec_pdo  .empty()             ));
	ATH_CHECK(m_readKey_mm_sidea_thr  .initialize(!m_readKey_mm_sidea_thr  .empty() && !m_isData));
	ATH_CHECK(m_readKey_mm_sidec_thr  .initialize(!m_readKey_mm_sidec_thr  .empty() && !m_isData));
	ATH_CHECK(m_readKey_stgc_sidea_tdo.initialize(!m_readKey_stgc_sidea_tdo.empty()             ));
	ATH_CHECK(m_readKey_stgc_sidec_tdo.initialize(!m_readKey_stgc_sidec_tdo.empty()             ));
	ATH_CHECK(m_readKey_stgc_sidea_pdo.initialize(!m_readKey_stgc_sidea_pdo.empty()             ));
	ATH_CHECK(m_readKey_stgc_sidec_pdo.initialize(!m_readKey_stgc_sidec_pdo.empty()             ));
	ATH_CHECK(m_readKey_stgc_sidea_thr.initialize(!m_readKey_stgc_sidea_thr.empty() && !m_isData));
	ATH_CHECK(m_readKey_stgc_sidec_thr.initialize(!m_readKey_stgc_sidec_thr.empty() && !m_isData));

	// write key for time/charge data
	ATH_CHECK(m_writeKey_tdopdo.initialize());

	// write key for threshold data
	ATH_CHECK(m_writeKey_thr.initialize(!m_isData));

	return StatusCode::SUCCESS;
}


// execute
StatusCode 
NswCalibDbAlg::execute(const EventContext& ctx) const {

	ATH_MSG_DEBUG( "execute " << name() );   

	if(processTdoPdoData(ctx).isFailure()) return StatusCode::FAILURE;
	if(processThrData   (ctx).isFailure()) return StatusCode::FAILURE;

	return StatusCode::SUCCESS;

}

// processTdoPdoData
StatusCode 
NswCalibDbAlg::processTdoPdoData(const EventContext& ctx) const {

	// set up write handles for time/charge data
	SG::WriteCondHandle<NswCalibDbTimeChargeData> wrHdl{m_writeKey_tdopdo, ctx};
	if (wrHdl.isValid()) {
		ATH_MSG_DEBUG("CondHandle " << wrHdl.fullKey() << " is already valid."
			  << " In theory this should not be called, but may happen"
			  << " if multiple concurrent events are being processed out of order.");
		return StatusCode::SUCCESS;
	}
	ATH_MSG_DEBUG("Range of time/charge output is " << wrHdl.getRange());
	std::unique_ptr<NswCalibDbTimeChargeData> wrCdo{std::make_unique<NswCalibDbTimeChargeData>(m_idHelperSvc->mmIdHelper(), m_idHelperSvc->stgcIdHelper())};

	// MM
	ATH_CHECK(loadTimeChargeData(ctx, m_readKey_mm_sidea_tdo  , TimeChargeTech::MM  , TimeChargeType::TDO, wrHdl, wrCdo.get()));
	ATH_CHECK(loadTimeChargeData(ctx, m_readKey_mm_sidec_tdo  , TimeChargeTech::MM  , TimeChargeType::TDO, wrHdl, wrCdo.get()));
	ATH_CHECK(loadTimeChargeData(ctx, m_readKey_mm_sidea_pdo  , TimeChargeTech::MM  , TimeChargeType::PDO, wrHdl, wrCdo.get()));
	ATH_CHECK(loadTimeChargeData(ctx, m_readKey_mm_sidec_pdo  , TimeChargeTech::MM  , TimeChargeType::PDO, wrHdl, wrCdo.get()));

	// sTGC
	ATH_CHECK(loadTimeChargeData(ctx, m_readKey_stgc_sidea_tdo, TimeChargeTech::STGC, TimeChargeType::TDO, wrHdl, wrCdo.get()));
	ATH_CHECK(loadTimeChargeData(ctx, m_readKey_stgc_sidec_tdo, TimeChargeTech::STGC, TimeChargeType::TDO, wrHdl, wrCdo.get()));
	ATH_CHECK(loadTimeChargeData(ctx, m_readKey_stgc_sidea_pdo, TimeChargeTech::STGC, TimeChargeType::PDO, wrHdl, wrCdo.get()));
	ATH_CHECK(loadTimeChargeData(ctx, m_readKey_stgc_sidec_pdo, TimeChargeTech::STGC, TimeChargeType::PDO, wrHdl, wrCdo.get()));

	// insert/write data for time/charge data
	if (wrHdl.record(std::move(wrCdo)).isFailure()) {
		ATH_MSG_FATAL("Could not record " << wrHdl.key() 
		      << " with EventRange " << wrHdl.getRange()
		      << " into Conditions Store");
		return StatusCode::FAILURE;
	}		  
	ATH_MSG_DEBUG("Recorded new " << wrHdl.key() << " with range " << wrHdl.getRange() << " into Conditions Store");

	return StatusCode::SUCCESS; // nothing to do

}


// processThrData
StatusCode 
NswCalibDbAlg::processThrData(const EventContext& ctx) const {

	if(m_isData) return StatusCode::SUCCESS; // nothing to do

	// set up write handles for threshold data
	SG::WriteCondHandle<NswCalibDbThresholdData> wrHdl{m_writeKey_thr, ctx};
	if (wrHdl.isValid()) {
		ATH_MSG_DEBUG("CondHandle " << wrHdl.fullKey() << " is already valid."
			  << " In theory this should not be called, but may happen"
			  << " if multiple concurrent events are being processed out of order.");
		return StatusCode::SUCCESS;
	}
	ATH_MSG_DEBUG("Range of threshold output is " << wrHdl.getRange());
	std::unique_ptr<NswCalibDbThresholdData> wrCdo{std::make_unique<NswCalibDbThresholdData>(m_idHelperSvc->mmIdHelper(), m_idHelperSvc->stgcIdHelper())};

	ATH_CHECK(loadThresholdData(ctx, m_readKey_mm_sidea_thr  , ThresholdTech::MM  , wrHdl, wrCdo.get()));
	ATH_CHECK(loadThresholdData(ctx, m_readKey_mm_sidec_thr  , ThresholdTech::MM  , wrHdl, wrCdo.get()));
	ATH_CHECK(loadThresholdData(ctx, m_readKey_stgc_sidea_thr, ThresholdTech::STGC, wrHdl, wrCdo.get()));
	ATH_CHECK(loadThresholdData(ctx, m_readKey_stgc_sidec_thr, ThresholdTech::STGC, wrHdl, wrCdo.get()));
	
	// insert/write data for threshold data
	if (wrHdl.record(std::move(wrCdo)).isFailure()) {
		ATH_MSG_FATAL("Could not record " << wrHdl.key() 
		      << " with EventRange " << wrHdl.getRange()
		      << " into Conditions Store");
		return StatusCode::FAILURE;
	}		  
	ATH_MSG_DEBUG("Recorded new " << wrHdl.key() << " with range " << wrHdl.getRange() << " into Conditions Store");

	return StatusCode::SUCCESS;
}


// loadThresholdData
StatusCode
NswCalibDbAlg::loadThresholdData(const EventContext& ctx, const readKey_t& readKey, const ThresholdTech tech, writeHandleThr_t& writeHandle, NswCalibDbThresholdData* writeCdo) const {

	// set up read handle
	SG::ReadCondHandle<CondAttrListCollection> readHandle{readKey, ctx};
	const CondAttrListCollection* readCdo{*readHandle}; 
	if(!readCdo){
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

		// retrieve blob
		const coral::AttributeList& atr = itr->second;
		if(atr["data"].specification().type() != typeid(coral::Blob)) {
			ATH_MSG_FATAL( "Data column is not of type blob!" );
			return StatusCode::FAILURE;
		}
		coral::Blob blob = atr["data"].data<coral::Blob>();
		std::unique_ptr<TTree> tree;
		if(!CoralUtilities::readBlobAsTTree(blob, tree)) {
			ATH_MSG_FATAL( "Cannot retrieve data from coral blob!" );
			return StatusCode::FAILURE;
		}

		// parse tree
		unsigned int elinkId{0}, vmm{0}, channel{0}; 
		float threshold{0.};
		tree->SetBranchAddress("vmm"           , &vmm           );
		tree->SetBranchAddress("channel"       , &channel       );
		tree->SetBranchAddress("elinkId"       , &elinkId       );
		tree->SetBranchAddress("threshold"     , &threshold     );

		// loop over channels
		unsigned int nChns = 0; 
		for(unsigned int iEvt=0; iEvt<tree->GetEntries(); ++iEvt){
			tree->GetEntry(iEvt);
			Identifier channelId;
			if(!buildChannelId(channelId, elinkId, vmm, channel)){
				ATH_MSG_WARNING("Could not find valid channelId for elink "<<elinkId);
				continue;
			}
			if(channelId.get_compact()==0){
				writeCdo->setZero(tech, threshold);
				++nChns;
				continue;
			}
			writeCdo->setData(channelId, threshold);
			++nChns;
		}
		ATH_MSG_VERBOSE("Retrieved data for "<<nChns<<" channels.");
		++nObjs;		
	}
	ATH_MSG_VERBOSE("Retrieved data for "<<nObjs<<" objects.");

	return StatusCode::SUCCESS;
}



// loadTimeChargeData
StatusCode
NswCalibDbAlg::loadTimeChargeData(const EventContext& ctx, const readKey_t& readKey, const TimeChargeTech tech, const TimeChargeType type, writeHandleTdoPdo_t& writeHandle, NswCalibDbTimeChargeData* writeCdo) const {

	// set up read handle
	SG::ReadCondHandle<CondAttrListCollection> readHandle{readKey, ctx};
	const CondAttrListCollection* readCdo{*readHandle}; 
	if(!readCdo){
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

		// retrieve blob
		const coral::AttributeList& atr = itr->second;
		if(atr["data"].specification().type() != typeid(coral::Blob)) {
			ATH_MSG_FATAL( "Data column is not of type blob!" );
			return StatusCode::FAILURE;
		}
		coral::Blob blob = atr["data"].data<coral::Blob>();
		std::unique_ptr<TTree> tree;
		if(!CoralUtilities::readBlobAsTTree(blob, tree)) {
			ATH_MSG_FATAL( "Cannot retrieve data from coral blob!" );
			return StatusCode::FAILURE;
		}
		// parse tree
		unsigned int elinkId{0}, vmm{0}, channel{0};
		float slope{0}, slopeError{0}, intercept{0},interceptError{0};	

		tree->SetBranchAddress("vmm"           , &vmm           );
		tree->SetBranchAddress("channel"       , &channel       );
		tree->SetBranchAddress("elinkId"       , &elinkId       );
		tree->SetBranchAddress("slope"         , &slope         );
		tree->SetBranchAddress("slopeError"    , &slopeError    );
		tree->SetBranchAddress("intercept"     , &intercept     );
		tree->SetBranchAddress("interceptError", &interceptError);
		

		// loop over channels
		unsigned int nChns = 0; 
		for(unsigned int iEvt=0; iEvt<tree->GetEntries(); ++iEvt){
			tree->GetEntry(iEvt);
			Identifier channelId{0};
			if(!buildChannelId(channelId, elinkId, vmm, channel)){
				ATH_MSG_WARNING("Could not find valid channelId for elink "<<elinkId);
				continue;
			}

			NswCalibDbTimeChargeData::CalibConstants calib_data{};
			calib_data.slope = slope;
			calib_data.slopeError = slopeError;
			calib_data.intercept = intercept;
			calib_data.interceptError = interceptError;
			
			if(!channelId.get_compact()){
				writeCdo->setZero(type, tech, std::move(calib_data));
				++nChns;
				continue;
			}

			writeCdo->setData(type, channelId, std::move(calib_data));
			++nChns;
		}
		ATH_MSG_VERBOSE("Retrieved data for "<<nChns<<" channels. "<<tree->GetName()<<" "<<tree->GetEntries());
		++nObjs;		
	}
	ATH_MSG_VERBOSE("Retrieved data for "<<nObjs<<" objects.");

	return StatusCode::SUCCESS;
}


// buildChannelId
bool
NswCalibDbAlg::buildChannelId(Identifier& channelId, unsigned int elinkId, unsigned int vmm, unsigned int channel) const {

	// return dummy Identifier
	if(elinkId==0){
		channelId = Identifier(0);
		return true;
	}

	// build NSWOfflineHelper
	std::unique_ptr<Muon::nsw::NSWResourceId> resId = std::make_unique<Muon::nsw::NSWResourceId>((uint32_t) elinkId);
	Muon::nsw::helper::NSWOfflineHelper helper(resId.get(), vmm, channel);
	
	std::string stationName;
	if(resId->detId () < eformat::MUON_STGC_ENDCAP_C_SIDE)
		stationName = resId->is_large_station () ? "MML" : "MMS";
	else
		stationName = resId->is_large_station () ? "STL" : "STS";
	
	int8_t   stationEta    = resId->station_eta ();
	uint8_t  stationPhi    = resId->station_phi ();
	uint8_t  multiLayer    = resId->multi_layer ();
	uint8_t  gasGap        = resId->gas_gap     ();
	
	uint8_t  channelType   = helper.channel_type  ();
	uint16_t channelNumber = helper.channel_number();

	ATH_MSG_VERBOSE("Station name="    << stationName 
	          << " Station eta="    << static_cast <int>          (stationEta)
	          << " Station phi="    << static_cast <unsigned int> (stationPhi)
	          << " Multilayer="     << static_cast <unsigned int> (multiLayer) 
	          << " Gas gap="        << static_cast <unsigned int> (gasGap)
	          << " Channel type="   << static_cast <unsigned int> (channelType)
	          << " Channel Number=" << channelNumber );


	// MM
	if(resId->detId () < eformat::MUON_STGC_ENDCAP_C_SIDE){
		Identifier chnlId = m_idHelperSvc->mmIdHelper().channelID(stationName, static_cast<int>(stationEta), static_cast<int>(stationPhi), static_cast<int>(multiLayer), static_cast<int>(gasGap), static_cast<int>(channelNumber));
		if(!chnlId.is_valid()){
			ATH_MSG_WARNING("Could not extract valid channelId for MM elink "<<elinkId);
			return false;
		}
		channelId = chnlId;
	}
	// sTGC
	else{
		Identifier chnlId = m_idHelperSvc->stgcIdHelper().channelID(stationName, static_cast<int>(stationEta), static_cast<int>(stationPhi), static_cast<int>(multiLayer), static_cast<int>(gasGap), static_cast<int>(channelType), static_cast<int>(channelNumber));
		if(!chnlId.is_valid()){
			ATH_MSG_WARNING("Could not extract valid channelId for STGC elink "<<elinkId);
			return false;
		}
		channelId = chnlId;
	}

	return true;
}

