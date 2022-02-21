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
	ATH_CHECK(m_condSvc    .retrieve());
	ATH_CHECK(m_idHelperSvc.retrieve());

    // read keys
    ATH_CHECK(m_readKey_data_mm_sidea_tdo  .initialize(!m_readKey_data_mm_sidea_tdo  .empty() && m_isData));
    ATH_CHECK(m_readKey_data_mm_sidec_tdo  .initialize(!m_readKey_data_mm_sidec_tdo  .empty() && m_isData));
    ATH_CHECK(m_readKey_data_mm_sidea_pdo  .initialize(!m_readKey_data_mm_sidea_pdo  .empty() && m_isData));
    ATH_CHECK(m_readKey_data_mm_sidec_pdo  .initialize(!m_readKey_data_mm_sidec_pdo  .empty() && m_isData));
    ATH_CHECK(m_readKey_data_mm_sidea_vmm  .initialize(!m_readKey_data_mm_sidea_vmm  .empty() && m_isData));
    ATH_CHECK(m_readKey_data_mm_sidec_vmm  .initialize(!m_readKey_data_mm_sidec_vmm  .empty() && m_isData));
    ATH_CHECK(m_readKey_data_stgc_sidea_tdo.initialize(!m_readKey_data_stgc_sidea_tdo.empty() && m_isData));
    ATH_CHECK(m_readKey_data_stgc_sidec_tdo.initialize(!m_readKey_data_stgc_sidec_tdo.empty() && m_isData));
    ATH_CHECK(m_readKey_data_stgc_sidea_pdo.initialize(!m_readKey_data_stgc_sidea_pdo.empty() && m_isData));
    ATH_CHECK(m_readKey_data_stgc_sidec_pdo.initialize(!m_readKey_data_stgc_sidec_pdo.empty() && m_isData));
    ATH_CHECK(m_readKey_data_stgc_sidea_vmm.initialize(!m_readKey_data_stgc_sidea_vmm.empty() && m_isData));
    ATH_CHECK(m_readKey_data_stgc_sidec_vmm.initialize(!m_readKey_data_stgc_sidec_vmm.empty() && m_isData));

	// write keys	
	ATH_CHECK(m_writeKey_tdopdo.initialize());
	ATH_CHECK(m_writeKey_vmm   .initialize());

	// register write handles
    if(m_condSvc->regHandle(this, m_writeKey_tdopdo).isFailure()) {
      ATH_MSG_FATAL("Unable to register WriteCondHandle " << m_writeKey_tdopdo.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
    if(m_condSvc->regHandle(this, m_writeKey_vmm   ).isFailure()) {
      ATH_MSG_FATAL("Unable to register WriteCondHandle " << m_writeKey_vmm   .fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}


// execute
StatusCode 
NswCalibDbAlg::execute(const EventContext& ctx) const {

	ATH_MSG_DEBUG( "execute " << name() );   

	// nothing to do when online	
	if(m_isOnline) {
		ATH_MSG_DEBUG( "IsOnline is set to True; nothing to do!" );   
		return StatusCode::SUCCESS;
	}
	
	// retrieving data (for future: add calls to separate methods here, if any)
	if(m_isData) {
		ATH_CHECK(loadDataCalibMm  (ctx));
		ATH_CHECK(loadDataCalibStgc(ctx));
	}
	else {
		// keep for now: place to drop MC-only methods, if any
	} 
	

	// return	
	return StatusCode::SUCCESS;
}


// loadDataCalibMm
StatusCode
NswCalibDbAlg::loadDataCalibMm(const EventContext& ctx) const{

	if(loadTimeChargeData(ctx, m_readKey_data_mm_sidea_tdo, "TDO").isFailure()) return StatusCode::FAILURE;
	if(loadTimeChargeData(ctx, m_readKey_data_mm_sidec_tdo, "TDO").isFailure()) return StatusCode::FAILURE;
	if(loadTimeChargeData(ctx, m_readKey_data_mm_sidea_pdo, "PDO").isFailure()) return StatusCode::FAILURE;
	if(loadTimeChargeData(ctx, m_readKey_data_mm_sidec_pdo, "PDO").isFailure()) return StatusCode::FAILURE;
	if(loadThresholdData (ctx, m_readKey_data_mm_sidea_vmm       ).isFailure()) return StatusCode::FAILURE;
	if(loadThresholdData (ctx, m_readKey_data_mm_sidec_vmm       ).isFailure()) return StatusCode::FAILURE;
	return StatusCode::SUCCESS;
}
	

// loadDataPdo
StatusCode
NswCalibDbAlg::loadDataCalibStgc(const EventContext& ctx) const {
	if(loadTimeChargeData(ctx, m_readKey_data_stgc_sidea_tdo, "TDO").isFailure()) return StatusCode::FAILURE;
	if(loadTimeChargeData(ctx, m_readKey_data_stgc_sidec_tdo, "TDO").isFailure()) return StatusCode::FAILURE;
	if(loadTimeChargeData(ctx, m_readKey_data_stgc_sidea_pdo, "PDO").isFailure()) return StatusCode::FAILURE;
	if(loadTimeChargeData(ctx, m_readKey_data_stgc_sidec_pdo, "PDO").isFailure()) return StatusCode::FAILURE;
	if(loadThresholdData (ctx, m_readKey_data_stgc_sidea_vmm       ).isFailure()) return StatusCode::FAILURE;
	if(loadThresholdData (ctx, m_readKey_data_stgc_sidec_vmm       ).isFailure()) return StatusCode::FAILURE;
	return StatusCode::SUCCESS;
}


// loadThresholdData
StatusCode
NswCalibDbAlg::loadThresholdData(const EventContext& ctx, const readKey_t& readKey) const {

	// set up write handle
	SG::WriteCondHandle<NswCalibDbThresholdData> writeHandle{m_writeKey_vmm, ctx};
	if (writeHandle.isValid()) {
		ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
			  << " In theory this should not be called, but may happen"
			  << " if multiple concurrent events are being processed out of order.");
		return StatusCode::SUCCESS; 
	}
	std::unique_ptr<NswCalibDbThresholdData> writeCdo{std::make_unique<NswCalibDbThresholdData>(m_idHelperSvc->mmIdHelper(), m_idHelperSvc->stgcIdHelper())};

	// set up read handle
	SG::ReadCondHandle<CondAttrListCollection> readHandle{readKey, ctx};
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

		// retrieve blob
		const coral::AttributeList& atr = itr->second;
		if(atr["data"].specification().type() != typeid(coral::Blob)) {
			ATH_MSG_FATAL( "Data column is not of type blob!" );
			return StatusCode::FAILURE;
		}
		coral::Blob blob = atr["data"].data<coral::Blob>();
		TTree* tree = nullptr;
		if(!CoralUtilities::readBlobAsTTree(blob, tree)) {
			ATH_MSG_FATAL( "Cannot retrieve data from coral blob!" );
			return StatusCode::FAILURE;
		}

		// parse tree
		unsigned int elinkId;
		unsigned int vmm; 
		unsigned int channel;
		float threshold;

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
			writeCdo->setData(&channelId, threshold);
        	++nChns;
		}
		ATH_MSG_VERBOSE("Retrieved data for "<<nChns<<" channels.");
        ++nObjs;
    }
	ATH_MSG_VERBOSE("Retrieved data for "<<nObjs<<" objects.");

	// insert/write data
	if (writeHandle.record(std::move(writeCdo)).isFailure()) {
		ATH_MSG_FATAL("Could not record NswCalibDbTimeChargeData " << writeHandle.key() 
		      << " with EventRange " << writeHandle.getRange()
		      << " into Conditions Store");
		return StatusCode::FAILURE;
	}		  
	ATH_MSG_DEBUG("Recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

    return StatusCode::SUCCESS;
}



// loadTimeChargeData
StatusCode
NswCalibDbAlg::loadTimeChargeData(const EventContext& ctx, const readKey_t& readKey, const std::string& type) const {

	// set up write handle
	SG::WriteCondHandle<NswCalibDbTimeChargeData> writeHandle{m_writeKey_tdopdo, ctx};
	if (writeHandle.isValid()) {
		ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
			  << " In theory this should not be called, but may happen"
			  << " if multiple concurrent events are being processed out of order.");
		return StatusCode::SUCCESS; 
	}
	std::unique_ptr<NswCalibDbTimeChargeData> writeCdo{std::make_unique<NswCalibDbTimeChargeData>(m_idHelperSvc->mmIdHelper(), m_idHelperSvc->stgcIdHelper())};

	// set up read handle
	SG::ReadCondHandle<CondAttrListCollection> readHandle{readKey, ctx};
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

		// retrieve blob
		const coral::AttributeList& atr = itr->second;
		if(atr["data"].specification().type() != typeid(coral::Blob)) {
			ATH_MSG_FATAL( "Data column is not of type blob!" );
			return StatusCode::FAILURE;
		}
		coral::Blob blob = atr["data"].data<coral::Blob>();
		TTree* tree = nullptr;
		if(!CoralUtilities::readBlobAsTTree(blob, tree)) {
			ATH_MSG_FATAL( "Cannot retrieve data from coral blob!" );
			return StatusCode::FAILURE;
		}

		// parse tree
		unsigned int elinkId;
		unsigned int vmm; 
		unsigned int channel;
		float slope;
		float slopeError;
		float intercept;
		float interceptError;
		float chi2;
		float prob;

		tree->SetBranchAddress("vmm"           , &vmm           );
		tree->SetBranchAddress("channel"       , &channel       );
		tree->SetBranchAddress("elinkId"       , &elinkId       );
		tree->SetBranchAddress("slope"         , &slope         );
		tree->SetBranchAddress("slopeError"    , &slopeError    );
		tree->SetBranchAddress("intercept"     , &intercept     );
		tree->SetBranchAddress("interceptError", &interceptError);
		tree->SetBranchAddress("chi2"          , &chi2          );
		tree->SetBranchAddress("prob"          , &prob          );

		// loop over channels
		unsigned int nChns = 0; 
		for(unsigned int iEvt=0; iEvt<tree->GetEntries(); ++iEvt){
			tree->GetEntry(iEvt);
			Identifier channelId;
			if(!buildChannelId(channelId, elinkId, vmm, channel)){
				ATH_MSG_WARNING("Could not find valid channelId for elink "<<elinkId);
				continue;
			}
			writeCdo->setData(type, &channelId, slope, slopeError, intercept, interceptError);
        	++nChns;
		}
		ATH_MSG_VERBOSE("Retrieved data for "<<nChns<<" channels.");
        ++nObjs;
    }
	ATH_MSG_VERBOSE("Retrieved data for "<<nObjs<<" objects.");

	// insert/write data
	if (writeHandle.record(std::move(writeCdo)).isFailure()) {
		ATH_MSG_FATAL("Could not record NswCalibDbTimeChargeData " << writeHandle.key() 
		      << " with EventRange " << writeHandle.getRange()
		      << " into Conditions Store");
		return StatusCode::FAILURE;
	}		  
	ATH_MSG_DEBUG("Recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

    return StatusCode::SUCCESS;
}


// buildChannelId
bool
NswCalibDbAlg::buildChannelId(Identifier& channelId, unsigned int elinkId, unsigned int vmm, unsigned int channel) const {

	// build NSWOfflineHelper
	Muon::nsw::NSWResourceId* resId = new Muon::nsw::NSWResourceId((uint32_t) elinkId);
	Muon::nsw::helper::NSWOfflineHelper helper(resId, vmm, channel);
	
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

	/* keep for debugging
	std::cout << "Station name="    << stationName 
	          << " Station eta="    << static_cast <int>          (stationEta)
	          << " Station phi="    << static_cast <unsigned int> (stationPhi)
	          << " Multilayer="     << static_cast <unsigned int> (multiLayer) 
	          << " Gas gap="        << static_cast <unsigned int> (gasGap)
	          << " Channel type="   << static_cast <unsigned int> (channelType)
	          << " Channel Number=" << channelNumber << std::endl;
	*/

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

