/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtRdoToPrepDataToolCore.h"

#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MdtRDO_Decoder.h"
#include "MuonCalibEvent/MdtCalibHit.h"
#include "MdtCalibSvc/MdtCalibrationTool.h"
#include "MdtCalibSvc/MdtCalibrationSvcSettings.h"
#include "MdtCalibSvc/MdtCalibrationSvcInput.h"
#include "MuonPrepRawData/MdtTwinPrepData.h"
#include "GeoModelUtilities/GeoGetIds.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <vector>
#include <algorithm>

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

namespace {
  static constexpr double const& inverseSpeedOfLight = 1 / Gaudi::Units::c_light; // need 1/299.792458
}

Muon::MdtRdoToPrepDataToolCore::MdtRdoToPrepDataToolCore(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_muonMgr(nullptr),
  m_mdtCalibSvcSettings(new MdtCalibrationSvcSettings()),
  m_calibratePrepData(true),
  m_fullEventDone(false),
  m_BMEpresent(false),
  m_BMGpresent(false),
  m_BMEid(-1),
  m_BMGid(-1)
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);

  //  template for property decalration
  declareProperty("CalibratePrepData",   m_calibratePrepData = true );
  declareProperty("DecodeData",          m_decodeData = true ); 
  
  // + TWIN TUBE
  declareProperty("UseTwin",                 m_useTwin = true);
  declareProperty("UseAllBOLTwin",           m_useAllBOLTwin = false);        
  declareProperty("Use1DPrepDataTwin",       m_use1DPrepDataTwin = false);
  declareProperty("TwinCorrectSlewing",      m_twinCorrectSlewing = false);
  declareProperty("DiscardSecondaryHitTwin", m_discardSecondaryHitTwin = false);
  // - TWIN TUBE
  declareProperty("TimeWindowLowerBound",   m_mdtCalibSvcSettings->windowLowerBound );
  declareProperty("TimeWindowUpperBound",   m_mdtCalibSvcSettings->windowUpperBound );
  declareProperty("TimeWindowSetting",      m_mdtCalibSvcSettings->windowSetting = 2 );
  declareProperty("DoTofCorrection",        m_mdtCalibSvcSettings->doTof  = true );
  declareProperty("DoPropagationCorrection",m_mdtCalibSvcSettings->doProp = false );
  // DataHandle
  declareProperty("RDOContainer",	m_rdoContainerKey = std::string("MDTCSM"),"MdtCsmContainer to retrieve");
  declareProperty("OutputCollection",	m_mdtPrepDataContainerKey = std::string("MDT_DriftCircles"),"Muon::MdtPrepDataContainer to record");
}

StatusCode Muon::MdtRdoToPrepDataToolCore::initialize() {
  ATH_CHECK(AthAlgTool::initialize());
  ATH_CHECK(detStore()->retrieve(m_muonMgr));
  ATH_CHECK(m_calibrationTool.retrieve());
  ATH_MSG_VERBOSE("MdtCalibrationTool retrieved with pointer = "<<m_calibrationTool);
  ATH_CHECK(m_idHelperSvc.retrieve());
  // Retrieve the RDO decoder 
  ATH_CHECK(m_mdtDecoder.retrieve());

  // + TWIN TUBES
  // make an array of [multilayer][layer][twin-pair]; 2 multilayers, 3 layer per multilayer, 36 twin-pairs per layer 
  if(m_useTwin){
    for(int i=0; i<2; i++){
      for(int j=0; j<3; j++){
	for(int k=0; k<36; k++){
          // fill m_twin_chamber array with unique numbers  
	  m_twin_chamber[i][j][k] = 1000*i + 100*j + k;
          // for secondary hits we need to make a second array with unique numbers 
          // (i+1 is used in the expression, so numbers are always different from m_twin_chamber array)
	  m_secondaryHit_twin_chamber[i][j][k] = 10000*(i+1) + 100*j + k;
	}
      }
    }
  }// end if(m_useTwin){
  // - TWIN TUBES

  m_mdtCalibSvcSettings->initialize();

  // check if the layout includes elevator chambers
  m_BMEpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME") != -1;
  if(m_BMEpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BME chambers.");
    m_BMEid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME");
  }
  m_BMGpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG") != -1;
  if(m_BMGpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BMG chambers.");
    m_BMGid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG");
    for(int phi=6; phi<8; phi++) { // phi sectors
      for(int eta=1; eta<4; eta++) { // eta sectors
        for(int side=-1; side<2; side+=2) { // side
          if( !m_muonMgr->getMuonStation("BMG", side*eta, phi) ) continue;
          for(int roe=1; roe<=( m_muonMgr->getMuonStation("BMG", side*eta, phi) )->nMuonReadoutElements(); roe++) { // iterate on readout elemets
            const MdtReadoutElement* mdtRE =
                  dynamic_cast<const MdtReadoutElement*> ( ( m_muonMgr->getMuonStation("BMG", side*eta, phi) )->getMuonReadoutElement(roe) ); // has to be an MDT
            if(mdtRE) initDeadChannels(mdtRE);
          }
        }
      }
    }
  }

  // check if initializing of DataHandle objects success
  ATH_CHECK(m_rdoContainerKey.initialize()); 
  ATH_CHECK(m_mdtPrepDataContainerKey.initialize());
  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_muDetMgrKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRdoToPrepDataToolCore::decode( const std::vector<uint32_t>& robIds )
{    
  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
  const MuonMDT_CablingMap* readCdo{*readHandle};
  if(!readCdo){
    ATH_MSG_ERROR("nullptr to the read conditions object");
    return StatusCode::FAILURE;
  }
  const std::vector<IdentifierHash>& chamberHashInRobs = readCdo->getChamberHashVec(robIds);
  return decode(chamberHashInRobs);
}

const MdtCsmContainer* Muon::MdtRdoToPrepDataToolCore::getRdoContainer() {
  auto rdoContainerHandle = SG::makeHandle(m_rdoContainerKey); 
  if(rdoContainerHandle.isValid()) {
    ATH_MSG_DEBUG("MdtgetRdoContainer success");
    return rdoContainerHandle.cptr();  
  }
  ATH_MSG_WARNING("Retrieval of Mdt RDO container failed !");
  return nullptr;
}

StatusCode Muon::MdtRdoToPrepDataToolCore::decode( const std::vector<IdentifierHash>& chamberHashInRobs )
{
  // setup output container
  SetupMdtPrepDataContainerStatus containerRecordStatus = setupMdtPrepDataContainer();
  if( containerRecordStatus == FAILED ){
    return StatusCode::FAILURE;
  }
  
  if( !m_decodeData ) {
    ATH_MSG_DEBUG("Stored empty container. Decoding MDT RDO into MDT PrepRawData is switched off");
    return StatusCode::SUCCESS;
  }
  
  //left unused, needed by other decode function and further down the code.
  std::vector<IdentifierHash> idWithDataVect;
  processPRDHashes(chamberHashInRobs,idWithDataVect);

  return StatusCode::SUCCESS;
}//end decode

void Muon::MdtRdoToPrepDataToolCore::processPRDHashes( const std::vector<IdentifierHash>& chamberHashInRobs, std::vector<IdentifierHash>& idWithDataVect ){
  // get RDO container
  const MdtCsmContainer* rdoContainer = getRdoContainer();
  if(!rdoContainer || rdoContainer->size()==0) {
    return;
  }                 

  for( auto it = chamberHashInRobs.begin(); it != chamberHashInRobs.end(); ++it ){
    if(!handlePRDHash(*it,*rdoContainer,idWithDataVect) ) {
      ATH_MSG_DEBUG("Failed to process hash " << *it );
    }
  }//ends loop over chamberhash  
}

void Muon::MdtRdoToPrepDataToolCore::processRDOContainer( std::vector<IdentifierHash>& idWithDataVect ){
  
  // get RDO container
  const MdtCsmContainer* rdoContainer = getRdoContainer();
  if(!rdoContainer || rdoContainer->size()==0) {
    return;                                        
  }    

  // It is more practical to loop through all the hashes rather than all RDO elements
  // as we benefit from handling the 2 RDO to 1 PRD special case
  for(unsigned int iHash = 0; iHash < m_idHelperSvc->mdtIdHelper().module_hash_max(); iHash++){
    handlePRDHash( IdentifierHash(iHash), *rdoContainer, idWithDataVect);
  }
  //for (MdtCsmContainer::const_iterator rdoColl = rdoContainer->begin(); rdoColl!=rdoContainer->end(); ++rdoColl) {
//
  //  handlePRDHash( (*rdoColl)->identifyHash(), *rdoContainer, idWithDataVect);
  //}

}

bool Muon::MdtRdoToPrepDataToolCore::handlePRDHash( IdentifierHash hash, const MdtCsmContainer& rdoContainer, std::vector<IdentifierHash>& idWithDataVect ) {
  
  // Check PRD container for the hash, if it exists, we already decoded fully
  if( m_mdtPrepDataContainer->tryAddFromCache(hash) ){
    ATH_MSG_DEBUG("RDO hash " << hash << " already decoded and inside PRD container cache");
    return true;
  }

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  IdentifierHash rdoHash = hash; // before BMEs were installed, RDOs were indexed by offline hashes (same as PRD)
  if (m_BMEpresent) { // after BMEs were installed, the RDOs are indexed by the detectorElement hash of a multilayer
    Identifier elementId;
    IdContext tmp_context = m_idHelperSvc->mdtIdHelper().module_context();
    if(m_idHelperSvc->mdtIdHelper().get_id(hash, elementId, &tmp_context)){
      ATH_MSG_DEBUG("Found invalid elementId");
      return false;
    }

    Identifier multilayerId = m_idHelperSvc->mdtIdHelper().multilayerID(elementId, 1); //first multilayer
    IdentifierHash multilayerHash;
    m_idHelperSvc->mdtIdHelper().get_detectorElement_hash(multilayerId, multilayerHash);
    rdoHash = multilayerHash;

    // for BMEs there are 2 CSMs per chamber, registered with the hashes of the 2 multilayers
    // If this is BMEid then we handle two RDO at once, else just one
    if (m_idHelperSvc->mdtIdHelper().stationName(elementId) == m_BMEid) {
      Identifier multilayerId2 = m_idHelperSvc->mdtIdHelper().multilayerID(elementId, 2); //second multilayer
      IdentifierHash multilayerHash2;
      m_idHelperSvc->mdtIdHelper().get_detectorElement_hash(multilayerId2, multilayerHash2);
      IdentifierHash rdoHash2 = multilayerHash2;
      // Retrieve the two RDO
      auto rdoColl  = rdoContainer.indexFindPtr(rdoHash);
      auto rdoColl2 = rdoContainer.indexFindPtr(rdoHash2);
      if( rdoColl != nullptr && rdoColl2 != nullptr ) {
        // Handle both at once
        if(processCsm(rdoColl, idWithDataVect, muDetMgr, rdoColl2).isFailure()){
          ATH_MSG_WARNING("processCsm failed for RDO id " 
            << (unsigned long long)(rdoColl->identify().get_compact()) << " and " 
            << (unsigned long long)(rdoColl2->identify().get_compact()));
          return false;
        }
      }
      else if(rdoColl != nullptr){
        // Handle just one
        ATH_MSG_DEBUG("Only one RDO container was found for hash " << hash << " despite BME - Missing " << rdoHash2 );
        if ( processCsm(rdoColl, idWithDataVect, muDetMgr).isFailure() ) {
          ATH_MSG_WARNING("processCsm failed for RDO id " << (unsigned long long)(rdoColl->identify().get_compact()));
          return false;
        }
      }
      else if(rdoColl2 != nullptr){
        // Handle just one
        ATH_MSG_DEBUG("Only one RDO container was found for hash " << hash << " despite BME - Missing " << rdoHash );
        if ( processCsm(rdoColl2, idWithDataVect, muDetMgr).isFailure() ) {
          ATH_MSG_WARNING("processCsm failed for RDO id " << (unsigned long long)(rdoColl->identify().get_compact()));
          return false;
        }
      }
      else{
        ATH_MSG_DEBUG("handlePRDHash: hash id " << hash << " not found in RDO container, and is BME - Missing " << rdoHash << " " << rdoHash2);
      }
    } // End of m_BMEpresent
    else{
      // process CSM if data was found
      auto rdoColl = rdoContainer.indexFindPtr(rdoHash);
      if( rdoColl != nullptr ) {
        if ( processCsm(rdoColl, idWithDataVect, muDetMgr).isFailure() ) {
          ATH_MSG_WARNING("processCsm failed for RDO id " << (unsigned long long)(rdoColl->identify().get_compact()));
          return false;
        }
      } else ATH_MSG_DEBUG("handlePRDHash: hash id " << (unsigned int)(hash) << " not found in RDO container");
    } 
  } else { // using pre-BME data
    // process CSM if data was found
    auto rdoColl = rdoContainer.indexFindPtr(rdoHash);
    if( rdoColl != nullptr ) {
      if ( processCsm(rdoColl, idWithDataVect, muDetMgr).isFailure() ) {
        ATH_MSG_WARNING("processCsm failed for RDO id " << (unsigned long long)(rdoColl->identify().get_compact()));
        return false;
      }
    } else ATH_MSG_DEBUG("handlePRDHash: hash id " << (unsigned int)(hash) << " not found in RDO container");
  }

  return true;
}


StatusCode Muon::MdtRdoToPrepDataToolCore::decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect )
{

  // clear output vector of selected data collections containing data 
  idWithDataVect.clear();

  // setup output container
  SetupMdtPrepDataContainerStatus containerRecordStatus = setupMdtPrepDataContainer();
  if( containerRecordStatus == FAILED ){
    return StatusCode::FAILURE;
  }

  if( !m_decodeData ) {
    ATH_MSG_DEBUG("Stored empty container. Decoding MDT RDO into MDT PrepRawData is switched off");
    return StatusCode::SUCCESS;
  }
  
  // check if event already decoded
  if( m_fullEventDone ){
    ATH_MSG_DEBUG("Whole event has already been decoded; nothing to do");
    return StatusCode::SUCCESS;
  }

  unsigned int sizeVectorRequested = idVect.size();
  ATH_MSG_DEBUG("decodeMdtRDO for "<<sizeVectorRequested<<" offline collections called");
  
  // if requesting full event, set the full event done flag to true
  if (sizeVectorRequested == 0) m_fullEventDone=true;

  // seeded or unseeded decoding
  if (sizeVectorRequested != 0) {
    processPRDHashes(idVect,idWithDataVect);
  }  else { 
    processRDOContainer(idWithDataVect);
  }
  
  return StatusCode::SUCCESS;
}

// dump the RDO in input
void Muon::MdtRdoToPrepDataToolCore::printInputRdo()
{

  ATH_MSG_DEBUG("******************************************************************************************");
  ATH_MSG_DEBUG("***************** Listing MdtCsmContainer collections content ********************************");

  const MdtCsmContainer* rdoContainer = getRdoContainer();

  if (rdoContainer->size()==0) ATH_MSG_DEBUG("MdtCsmContainer is Empty");

  ATH_MSG_DEBUG("-----------------------------------------------------------------------------");

  MdtCsmContainer::const_iterator it = rdoContainer->begin();

  int ncsm = 0;
  int namt = 0;
  // loop on the MdtCsm collections
  for (  ; it != rdoContainer->end() ; ++it ) {

    ncsm++;
    const MdtCsm* mdtColl = (*it);
    // get the online Id of the collection:
    uint16_t subdet = mdtColl->SubDetId();
    uint16_t mrodid = mdtColl->MrodId();
    uint16_t csmid  = mdtColl->CsmId();

    ATH_MSG_DEBUG("**** MdtCsm with online Id: subdetector: " << MSG::hex 
                 << subdet << MSG::dec << "  mrod: " << MSG::hex << mrodid 
                 << MSG::dec << "  csmid: " << MSG::hex << csmid << MSG::dec);
    ATH_MSG_DEBUG("  number of mdt hits: " << mdtColl->size());
  
    // loop on the hits of the CSM
    MdtCsm::const_iterator it_amt = mdtColl->begin();
    for ( ; it_amt != mdtColl->end() ; ++it_amt ) {

      const MdtAmtHit* amtHit = (*it_amt);
      
      namt++;
      uint16_t tdcId=amtHit->tdcId();
      uint16_t channelId=amtHit->channelId();
      uint16_t fine=amtHit->fine();
      uint16_t coarse=amtHit->coarse();
      uint16_t width=amtHit->width();
      
      ATH_MSG_DEBUG(">> AmtHit in tdc: " << MSG::hex << tdcId << MSG::dec
                   << "  channel: " << MSG::hex << channelId << MSG::dec
                   << "  fine time: " << fine
                   << "  coarse time: " << coarse
                   << "  width: " << width); 
      
    }

  }

  ATH_MSG_DEBUG("*** Event Summary: csm collections:" << ncsm << "  amt hits: " << namt);

  return;
}

void Muon::MdtRdoToPrepDataToolCore::printPrepData(  )
{
  // Dump info about PRDs
  ATH_MSG_DEBUG("******************************************************************************************");
  ATH_MSG_DEBUG("***************** Listing MdtPrepData collections content ********************************");
  
  if (m_mdtPrepDataContainer->size() <= 0) ATH_MSG_DEBUG("No MdtPrepRawData collections found");
  int ncoll = 0;
  int nhits = 0;
  ATH_MSG_DEBUG("--------------------------------------------------------------------------------------------");
  for (IdentifiableContainer<Muon::MdtPrepDataCollection>::const_iterator mdtColli = m_mdtPrepDataContainer->begin();
       mdtColli!=m_mdtPrepDataContainer->end(); ++mdtColli)
    {
      const Muon::MdtPrepDataCollection* mdtColl = *mdtColli;
      
      int nhitcoll = 0;
      if ( mdtColl->size() > 0 ) 
        {            
          ATH_MSG_DEBUG("PrepData Collection ID "<<m_idHelperSvc->toString(mdtColl->identify()));
          MdtPrepDataCollection::const_iterator it_mdtPrepData;
          for (it_mdtPrepData=mdtColl->begin(); it_mdtPrepData != mdtColl->end(); it_mdtPrepData++) {
            nhitcoll++;
            nhits++;
            ATH_MSG_DEBUG(" in this coll. "<<nhitcoll<<" prepData id = "
                         <<m_idHelperSvc->toString((*it_mdtPrepData)->identify())
                         <<" tdc/adc ="<<(*it_mdtPrepData)->tdc()<<"/"<< (*it_mdtPrepData)->adc());
          }
          ncoll++;
          ATH_MSG_DEBUG("*** Collection "<<ncoll<<" Summary: N. hits = "<<nhitcoll);
          ATH_MSG_DEBUG("--------------------------------------------------------------------------------------------");
        }
    }
  ATH_MSG_DEBUG("*** Event  Summary: "
               <<ncoll <<" Collections / "
               <<nhits<<" hits  ");
  ATH_MSG_DEBUG("--------------------------------------------------------------------------------------------");
  
}

StatusCode Muon::MdtRdoToPrepDataToolCore::processCsm(const MdtCsm* rdoColl, std::vector<IdentifierHash>& idWithDataVect, const MuonGM::MuonDetectorManager* muDetMgr, const MdtCsm* rdoColl2) {
  
  // first handle the case of twin tubes
  if(m_useTwin){
    // two chambers in ATLAS are installed with Twin Tubes; in detector coordinates BOL4A13 & BOL4C13; only INNER multilayer(=1) is with twin tubes
    // implement twin tube writing to prepData either for all BOL (m_useAllBOLTwin = true) _OR_ only for two chambers really installed
    Identifier elementId = rdoColl->identify();
    MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(elementId);
    if( chIndex == MuonStationIndex::BOL &&
        ( m_useAllBOLTwin || (std::abs(m_idHelperSvc->mdtIdHelper().stationEta(elementId)) == 4 && m_idHelperSvc->mdtIdHelper().stationPhi(elementId) == 7) ) ) { 
      return processCsmTwin(rdoColl, idWithDataVect, muDetMgr);
    }
  }
  
  ATH_MSG_DEBUG(" ***************** Start of processCsm");

  // Make some checks if we are in special circumstances
  if(rdoColl2){
    Identifier elementId1 = m_idHelperSvc->mdtIdHelper().parentID(rdoColl->identify());
    Identifier elementId2 = m_idHelperSvc->mdtIdHelper().parentID(rdoColl2->identify());
    IdContext mdtContext  = m_idHelperSvc->mdtIdHelper().module_context();
    IdentifierHash hash1, hash2;
    m_idHelperSvc->mdtIdHelper().get_hash(elementId1, hash1, &mdtContext);
    m_idHelperSvc->mdtIdHelper().get_hash(elementId2, hash2, &mdtContext);
    ATH_MSG_DEBUG("Special case with 2 RDO going into 1 PRD - ID1 " << elementId1 
      << " ID2 " << elementId2 << " hash 1 " << hash1 << " hash 2 " << hash2 );
    // These should have the same PRD MDT hash otherwise we need to understand why we are here
    if(hash1 != hash2){
      ATH_MSG_ERROR("Hash1 and Hash2 are different for special case : " << hash1 << " " << hash2);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG(" Number of AmtHit in this Csm (1) " << rdoColl->size()  <<" CSM id is "<<m_idHelperSvc->toString(elementId1));
    ATH_MSG_DEBUG(" Number of AmtHit in this Csm (2) " << rdoColl2->size() <<" CSM id is "<<m_idHelperSvc->toString(elementId2));
  }

  /// MDT hit context
  //IdContext hitContext = m_idHelperSvc->mdtIdHelper().channel_context();
  Identifier elementId = m_idHelperSvc->mdtIdHelper().parentID(rdoColl->identify());
  
  // prepare the special case of two chamber connected to the same CSM
  // create objects but do not initialize them
  bool doubleCsmFound = false;
  bool doubleCsm = false;

  Identifier secondElementId;
  IdentifierHash secondMdtHashId;

  if(!rdoColl2)
    ATH_MSG_DEBUG(" Number of AmtHit in this Csm " << rdoColl->size() <<" CSM id is "<<m_idHelperSvc->toString(elementId));

  uint16_t subdetId = rdoColl->SubDetId();
  uint16_t mrodId = rdoColl->MrodId();
  uint16_t csmId = rdoColl->CsmId();
  ATH_MSG_VERBOSE("Identifier = "<<m_idHelperSvc->toString(elementId)
                  <<" subdetId/ mrodId/ csmId = "<<subdetId<<" / "<<mrodId<<" / "<<csmId);

  // Create prepdata collection
  IdContext mdtContext = m_idHelperSvc->mdtIdHelper().module_context();
  IdentifierHash mdtHashId;
  if(m_idHelperSvc->mdtIdHelper().get_hash(elementId, mdtHashId, &mdtContext)) {
    ATH_MSG_FATAL("Unable to get MDT hash id from MDT digit collection "
                  << "context begin_index = " << mdtContext.begin_index()
                  << " context end_index  = " << mdtContext.end_index()
                  << " the identifier is ");
    elementId.show();
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("HashId = "<<(int)mdtHashId);

  std::unique_ptr<MdtPrepDataCollection> driftCircleColl = nullptr;
  std::unique_ptr<MdtPrepDataCollection> secondDriftCircleColl = nullptr;

  // Check the IDC cache (no write handles here)
  if( m_mdtPrepDataContainer->tryAddFromCache(mdtHashId) ){
    // The collection is in the container so we should not process anything (true for elevator chambers)
    ATH_MSG_DEBUG("In ProcessCSM - collection already contained in IDC " << elementId << " " << mdtHashId);
    // But instead of returning, we will process in case this RDO has two possible PRD and we only decoded one
  }
  // Create new collection 
  else{
    driftCircleColl.reset( new MdtPrepDataCollection( mdtHashId ) );
    idWithDataVect.push_back(mdtHashId);
    driftCircleColl->setIdentifier(elementId);
    ATH_MSG_DEBUG("Created MdtPrepDataCollection (not found in cache) " << (unsigned int)mdtHashId);
  }

  // for each Csm, loop over AmtHit, converter AmtHit to digit
  // retrieve/create digit collection, and insert digit into collection
  MdtCsm::const_iterator itD   = rdoColl->begin();
  MdtCsm::const_iterator itD_e = rdoColl->end();

  // Note that for 2 RDO -> 1 PRD, the first RDO _can_ be empty, so we should check before we loop
  if(rdoColl2){
    // Check finished the first collection (second pass is handled by for-loop end condition)
    if(itD == rdoColl->end()){
      ATH_MSG_DEBUG("RDO collection 1 is empty - Iterators moving to RDO collection 2");
      // Update our iterators and continue processing
      itD   = rdoColl2->begin();
      itD_e = rdoColl2->end();

      elementId = m_idHelperSvc->mdtIdHelper().parentID(rdoColl2->identify());
      subdetId = rdoColl2->SubDetId();
      mrodId   = rdoColl2->MrodId();
      csmId    = rdoColl2->CsmId();
      ATH_MSG_VERBOSE("Identifier = "<<m_idHelperSvc->toString(elementId)
                  <<" subdetId/ mrodId/ csmId = "<<subdetId<<" / "<<mrodId<<" / "<<csmId);
    }
  }

  int mc = 0;
  for (; itD!=itD_e; ) {
    mc++;
    const MdtAmtHit * amtHit = (*itD);

    // set to false the decision of whether it's a special case of two chambers connected to a 
    // single CSM or not
    doubleCsm = false;

    // FIXME: Still use the digit class.
    ATH_MSG_VERBOSE("Amt Hit n. "<<mc<<" tdcId = "<< amtHit->tdcId());

    MdtDigit * newDigit = m_mdtDecoder->getDigit(amtHit,subdetId,
                                                 mrodId,csmId);

    if (!newDigit) {
      ATH_MSG_WARNING("Found issue MDT RDO decoder for subdetId/mrodId/csmId "
                      <<subdetId<<"/"<<mrodId<<"/"<<csmId<<" amtHit channelId/tdcId ="
                      <<amtHit->channelId()<<"/"<<amtHit->tdcId());
      itD++;
      continue;
    }

    // Do something with it
    Identifier     channelId   = newDigit->identify();
    Identifier     parentId    = m_idHelperSvc->mdtIdHelper().parentID(channelId);
    if( m_idHelperSvc->mdtIdHelper().stationName(parentId) == m_BMGid && m_BMGpresent) {
      std::map<Identifier, std::vector<Identifier> >::iterator myIt = m_DeadChannels.find(muDetMgr->getMdtReadoutElement(channelId)->identify());
      if( myIt != m_DeadChannels.end() ){
        if( std::find( (myIt->second).begin(), (myIt->second).end(), channelId) != (myIt->second).end() ) {
          ATH_MSG_DEBUG("processCsm : Deleting BMG digit with identifier" << m_idHelperSvc->mdtIdHelper().show_to_string(channelId) );
          delete newDigit;
	  itD++;
          continue;
        }
      }
    }

    // check if the module ID of this channel is different from the CSM one
    // If it's the first case, create the additional collection

    if ( parentId != elementId ) {

      int stationName = m_idHelperSvc->mdtIdHelper().stationName(channelId);
      // check that at least the station name is the same, otherwise it must be an error
      if ( stationName == m_idHelperSvc->mdtIdHelper().stationName(elementId) ) {

        doubleCsm = true;

        if ( !doubleCsmFound ) {

          doubleCsmFound = true;
          secondElementId = parentId;

          ATH_MSG_DEBUG("A special case of single CSM reading two chambers has been found");

          ATH_MSG_DEBUG("First station: " << m_idHelperSvc->mdtIdHelper().stationNameString(stationName) << " eta: " 
                        << m_idHelperSvc->mdtIdHelper().stationEta(elementId) << " phi: " << m_idHelperSvc->mdtIdHelper().stationPhi(elementId) );

          ATH_MSG_DEBUG("Second station: " << m_idHelperSvc->mdtIdHelper().stationNameString(stationName) << " eta: " 
                        << m_idHelperSvc->mdtIdHelper().stationEta(secondElementId) << " phi: " 
                        << m_idHelperSvc->mdtIdHelper().stationPhi(secondElementId) );

          //
          // create the collection and initialize the hash Id
          //
          if(m_idHelperSvc->mdtIdHelper().get_hash(secondElementId, secondMdtHashId, &mdtContext)) {
            ATH_MSG_FATAL("Unable to get MDT hash id from MDT digit collection "
                          << "context begin_index = " << mdtContext.begin_index()
                          << " context end_index  = " << mdtContext.end_index()
                          << " the identifier is ");
            secondElementId.show();
            return StatusCode::FAILURE;
          }
          ATH_MSG_VERBOSE("HashId = "<<(int) secondMdtHashId);

          // If we got to here we need to inspect the cache and create or nullptr
          if( m_mdtPrepDataContainer->tryAddFromCache(secondMdtHashId) ){
            ATH_MSG_DEBUG("In ProcessCSM - collection already contained in IDC");
            // Proceed with the nullptr
          }
          else{
            secondDriftCircleColl.reset( new MdtPrepDataCollection(secondMdtHashId) );
            idWithDataVect.push_back(secondMdtHashId);
            secondDriftCircleColl->setIdentifier(secondElementId);
            ATH_MSG_DEBUG("Created second MdtPrepDataCollection (not found in cache) " << secondMdtHashId);
          }
        } 
      }
    }

    IdentifierHash channelHash = newDigit->identifyHash();

    ATH_MSG_VERBOSE("got digit with id ext / hash "<<m_idHelperSvc->toString(channelId)<<" / "<<(int)channelHash);

    double radius(0.); double errRadius(0.);
    Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;

    // do lookup once
    const MdtReadoutElement* descriptor = muDetMgr->getMdtReadoutElement(channelId);
    if (!descriptor){
      ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
                      <<m_idHelperSvc->toString(channelId)<<">  =>>ignore this hit");
      delete newDigit;
      itD++;
      continue;
    }
    if (!descriptor->containsId(channelId)) {
      ATH_MSG_WARNING("Detector Element "<<m_idHelperSvc->toString(descriptor->identify())
                      <<" does not contains candidate prd Identifier <"
                      <<m_idHelperSvc->toString(channelId)<<">  =>>ignore this hit");
      delete newDigit;
      itD++;
      continue;
    }
      

    if (newDigit->is_masked()) {
      digitStatus = Muon::MdtStatusMasked;
    }else{
      digitStatus = getMdtDriftRadius(newDigit, radius, errRadius, descriptor, muDetMgr);
      if( radius < -999 ) {
        ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
                        << " Id is: "<<m_idHelperSvc->toString(channelId));
          
      }
    }

    Amg::Vector2D driftRadius(radius,0);
    Amg::MatrixX* cov = new Amg::MatrixX(1,1);
    (*cov)(0,0) = errRadius*errRadius;
    // Create new PrepData

    // Check that the hit does not belong to the second chamber which might be connected to
    // this CSM
    MdtPrepDataCollection* collectionToBeUpdated;
    IdentifierHash hashId;

    if ( (!doubleCsmFound) || (!doubleCsm) ) {
      collectionToBeUpdated = driftCircleColl.get();
      hashId = mdtHashId;
    }
    else {
      collectionToBeUpdated = secondDriftCircleColl.get();
      hashId = secondMdtHashId;
    }

    // We can make the MdtPrepData, but only if our collection is being used (ie not nullptr)
    if(collectionToBeUpdated){
      MdtPrepData* newPrepData = new MdtPrepData(channelId,
                                                 hashId,
                                                 driftRadius,
                                                 cov,
                                                 descriptor,
                                                 newDigit->tdc(),
                                                 newDigit->adc(),
                                                 digitStatus);

      newPrepData->setHashAndIndex(collectionToBeUpdated->identifyHash(), collectionToBeUpdated->size());
      collectionToBeUpdated->push_back(newPrepData);
    }

    delete newDigit;

    // Special case when we need to process two RDO for one PRD in a single pass
    ++itD;
    // Check if we have two containers
    if(rdoColl2){
      // Check finished the first collection (second pass is handled by for-loop end condition)
      if(itD == rdoColl->end()){
        ATH_MSG_DEBUG("RDO collection 1 is processed - Iterators moving to RDO collection 2");
        // Update our iterators and continue processing
        itD   = rdoColl2->begin();
        itD_e = rdoColl2->end();
        mc = 0; // for logging
        // Update element information for next RDO
        elementId = m_idHelperSvc->mdtIdHelper().parentID(rdoColl2->identify());
        subdetId = rdoColl2->SubDetId();
        mrodId   = rdoColl2->MrodId();
        csmId    = rdoColl2->CsmId();
        ATH_MSG_VERBOSE("Identifier = "<<m_idHelperSvc->toString(elementId)
          <<" subdetId/ mrodId/ csmId = "<<subdetId<<" / "<<mrodId<<" / "<<csmId);
      }
    }
  }
  // At the end, we finally tell the cache we will be putting collections into it 
  // Note we did not skip empty RDO in case of 2 RDO -> 1 PRD processing
  // Record PRD if it is not empty
  if(driftCircleColl && !driftCircleColl->empty() ){
    MdtPrepDataContainer::IDC_WriteHandle lock = m_mdtPrepDataContainer->getWriteHandle( mdtHashId );
    if( !lock.alreadyPresent() ){
      StatusCode status_lock = lock.addOrDelete(std::move( driftCircleColl ));
      if (status_lock.isFailure()) {
        ATH_MSG_ERROR ( "Could not insert MdtCsmPrepdataCollection into MdtCsmPrepdataContainer..." );
        return StatusCode::FAILURE;
      }
    }
    else{
      ATH_MSG_DEBUG("MdtCsmPrepdataCollection already inside MdtCsmPrepdataContainer " << mdtHashId);
    }
  }
  // Special treatment for empty PRD containers
  if(driftCircleColl && driftCircleColl->empty() && !secondDriftCircleColl){
    // Do not record any empty PRD containers if RDO was empty
    ATH_MSG_DEBUG("MdtCsmPrepdataCollection was empty as input RDO is empty " << mdtHashId);
  }
  if(driftCircleColl && driftCircleColl->empty() && secondDriftCircleColl){
    // If RDO was not empty, we need to record all PRD containers (even if one is empty)
    ATH_MSG_DEBUG("Input RDO was not empty but 1 RDO -> 2 PRD left one MdtCsmPrepdataCollection empty");
    ATH_MSG_DEBUG("Recording empty MdtCsmPrepdataCollection " << mdtHashId);
    MdtPrepDataContainer::IDC_WriteHandle lock = m_mdtPrepDataContainer->getWriteHandle( mdtHashId );
    if( !lock.alreadyPresent() ){
      StatusCode status_lock = lock.addOrDelete(std::move( driftCircleColl ));
      if (status_lock.isFailure()) {
        ATH_MSG_ERROR ( "Could not insert MdtCsmPrepdataCollection into MdtCsmPrepdataContainer..." );
        return StatusCode::FAILURE;
      }
    }
    else{
      ATH_MSG_DEBUG("MdtCsmPrepdataCollection already inside MdtCsmPrepdataContainer " << mdtHashId);
    }
  }
  // Second PRD only exists if RDO was not empty and 1 RDO -> 2 PRD is activated
  if(secondDriftCircleColl && !secondDriftCircleColl->empty() ){
    MdtPrepDataContainer::IDC_WriteHandle lock = m_mdtPrepDataContainer->getWriteHandle( secondMdtHashId );
    if( !lock.alreadyPresent() ){
      StatusCode status_lock = lock.addOrDelete(std::move( secondDriftCircleColl ));
      if (status_lock.isFailure()) {
        ATH_MSG_ERROR ( "Could not insert MdtCsmPrepdataCollection into MdtCsmPrepdataContainer..." );
        return StatusCode::FAILURE;
      }
    }
    else{
      ATH_MSG_DEBUG("MdtCsmPrepdataCollection already inside MdtCsmPrepdataContainer " << secondMdtHashId);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRdoToPrepDataToolCore::processCsmTwin(const MdtCsm* rdoColl, std::vector<IdentifierHash>& idWithDataVect, const MuonGM::MuonDetectorManager* muDetMgr)
{
  ATH_MSG_DEBUG(" ***************** Start of processCsmTwin");
  
  ATH_MSG_DEBUG(" Number of AmtHit in this Csm "
                << rdoColl->size());
  /// MDT hit context
  Identifier elementId = m_idHelperSvc->mdtIdHelper().parentID(rdoColl->identify());
    
  uint16_t subdetId = rdoColl->SubDetId();
  uint16_t mrodId = rdoColl->MrodId();
  uint16_t csmId = rdoColl->CsmId();
  ATH_MSG_VERBOSE("Identifier = "<<m_idHelperSvc->toString(elementId)
                  <<" subdetId/ mrodId/ csmId = "<<subdetId<<" / "
                  <<mrodId<<" / "<<csmId);
    

    
  // Create prepdata collection
  IdContext mdtContext = m_idHelperSvc->mdtIdHelper().module_context();
  IdentifierHash mdtHashId;
  if(m_idHelperSvc->mdtIdHelper().get_hash(elementId, mdtHashId, &mdtContext)) {
    ATH_MSG_FATAL("Unable to get MDT hash id from MDT digit collection "
                  << "context begin_index = " << mdtContext.begin_index()
                  << " context end_index  = " << mdtContext.end_index()
                  << " the identifier is ");
    elementId.show();
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("HashId = "<<(int)mdtHashId);

  // Check the IDC cache (no write handles here)
  MdtPrepDataContainer::IDC_WriteHandle lock = m_mdtPrepDataContainer->getWriteHandle( mdtHashId );
  if( lock.alreadyPresent() ){
    ATH_MSG_DEBUG("MdtPrepDataCollection already contained in IDC " << elementId << " " << mdtHashId);
    return StatusCode::SUCCESS;
  }  
  
  std::unique_ptr<MdtPrepDataCollection> driftCircleColl ( new MdtPrepDataCollection( mdtHashId ) );
  idWithDataVect.push_back(mdtHashId);
  driftCircleColl->setIdentifier(elementId);
  ATH_MSG_DEBUG("Created MdtPrepDataCollection (not found in cache) " << mdtHashId);

  // for each Csm, loop over AmtHit, converter AmtHit to digit
  // retrieve/create digit collection, and insert digit into collection
  MdtCsm::const_iterator itD   = rdoColl->begin();
  MdtCsm::const_iterator itD_e = rdoColl->end();
    
  // make a map to be filled for every twin-pair
  //   std::map<int, std::vector<MdtDigit*> > mdtDigitColl;
  std::map<int, std::pair<MdtDigit*, MdtDigit*> > mdtDigitColl;

  for (; itD!=itD_e; ++itD){
    const MdtAmtHit * amtHit = (*itD);
      
    MdtDigit * newDigit = m_mdtDecoder->getDigit(amtHit,subdetId,mrodId,csmId);
    
    if (!newDigit) {
      ATH_MSG_WARNING("Error in MDT RDO decoder for subdetId/mrodId/csmId "<<subdetId<<"/"<<mrodId<<"/"<<csmId<<" amtHit channelId/tdcId ="<<amtHit->channelId()<<"/"<<amtHit->tdcId());
      continue;
    }

    // make an Identifier
    Identifier channelId = newDigit->identify();
    //IdentifierHash channelHash = newDigit->identifyHash();

    if( m_idHelperSvc->mdtIdHelper().stationName(channelId) == m_BMGid && m_BMGpresent) {
      std::map<Identifier, std::vector<Identifier> >::iterator myIt = m_DeadChannels.find(muDetMgr->getMdtReadoutElement(channelId)->identify());
      if( myIt != m_DeadChannels.end() ){
        if( std::find( (myIt->second).begin(), (myIt->second).end(), channelId) != (myIt->second).end() ) {
          ATH_MSG_DEBUG("processCsm : Deleting BMG digit with identifier" << m_idHelperSvc->mdtIdHelper().show_to_string(channelId) );
          delete newDigit;
          continue;
        }
      }
    }

    // get tube params
    int tube = m_idHelperSvc->mdtIdHelper().tube(channelId);
    int layer = m_idHelperSvc->mdtIdHelper().tubeLayer(channelId);
    int multilayer = m_idHelperSvc->mdtIdHelper().multilayer(channelId);
      
    // find the correct twin-pair (tube-1 & tube-3 are twin pair 1, tube-2 & tube-4 are twin pair 2)
    int twinPair = -1;
    if( tube%4 == 1){ twinPair = (tube + 1)/2 ;}
    else if( tube%4 == 3){ twinPair = (tube - 1)/2 ;}
    else if( tube%4 == 2){ twinPair = (tube + 2)/2 ;}
    else { twinPair = tube/2 ;} // tube%4 == 0

    // fill the digitColl map
    if( mdtDigitColl[ m_twin_chamber[multilayer-1][layer-1][twinPair-1] ].first == 0){
      mdtDigitColl[ m_twin_chamber[multilayer-1][layer-1][twinPair-1] ].first = newDigit;
    }
    else if( mdtDigitColl[ m_twin_chamber[multilayer-1][layer-1][twinPair-1] ].second == 0){
      mdtDigitColl[ m_twin_chamber[multilayer-1][layer-1][twinPair-1] ].second = newDigit;
    }
    // if a secondary hit appears in a tube add it to mdtDigitColl, unless m_discardSecondaryHitTwin flag is true
    else{
      ATH_MSG_VERBOSE(" TWIN TUBES: found a secondary(not twin) hit in a twin tube");
 
      if(!m_discardSecondaryHitTwin){
        if( mdtDigitColl[ m_secondaryHit_twin_chamber[multilayer-1][layer-1][twinPair-1] ].first == 0){
          mdtDigitColl[ m_secondaryHit_twin_chamber[multilayer-1][layer-1][twinPair-1] ].first = newDigit;
        }
        else if( mdtDigitColl[ m_secondaryHit_twin_chamber[multilayer-1][layer-1][twinPair-1] ].second == 0){
          mdtDigitColl[ m_secondaryHit_twin_chamber[multilayer-1][layer-1][twinPair-1] ].second = newDigit;
        }
        else{ ATH_MSG_VERBOSE(" TWIN TUBES: found a tertiary hit in a twin tube in one RdoCollection for "
                              << m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(channelId))
                              << " eta = " << m_idHelperSvc->mdtIdHelper().stationEta(channelId) << " phi = " << m_idHelperSvc->mdtIdHelper().stationPhi(channelId)
                              << "  ml =  "  <<m_idHelperSvc->mdtIdHelper().multilayer(channelId)  << "  layer = " << m_idHelperSvc->mdtIdHelper().tubeLayer(channelId) 
                              << " tube = " <<  m_idHelperSvc->mdtIdHelper().tube(channelId) << " with adc  = " << newDigit->adc() << "  tdc = " << newDigit->tdc());
        }
      } //end --   if(!m_discardSecondaryHitTwin){
      else{
        ATH_MSG_DEBUG(" TWIN TUBES: discarding secondary(non-twin) hit in a twin tube as flag m_discardSecondaryHitTwin is set to true");
      }
    }
  }// end for-loop over rdoColl  
 
  // make iterator over mdtDigitColl map
  std::map<int, std::pair<MdtDigit*, MdtDigit*> >::iterator iter_map;

  //iterate over mdtDigitColl
  for( iter_map = mdtDigitColl.begin(); iter_map != mdtDigitColl.end(); iter_map++ ) {

    // get the twin hits from mdtDigitColl
    MdtDigit* digit = iter_map->second.first;
    MdtDigit* second_digit = iter_map->second.second;

    if (!digit) {
      ATH_MSG_FATAL("nullptr to a digit ");
      return StatusCode::FAILURE;
    }
      
    // Do something with it
    Identifier     channelId   = digit->identify();
    IdentifierHash channelHash = digit->identifyHash();

    int multilayer = m_idHelperSvc->mdtIdHelper().multilayer(channelId);
  
    //check if the hit is in multilayer=1	    
    // two chambers in ATLAS are installed with Twin Tubes; in detector coordinates BOL4A13 & BOL4C13; only INNER multilayer(=1) is with twin tubes
    if(multilayer==1){
      // if no twin hit present in data, use standard PRD making 
      if(!second_digit){
	  
	  
        ATH_MSG_VERBOSE("got digit with id ext / hash "
                        <<m_idHelperSvc->toString(channelId)<<" / "
                        <<(int)channelHash);
	  
        double radius(0.); double errRadius(0.);
        Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;
	  
        // do lookup once
        const MdtReadoutElement * descriptor = muDetMgr->getMdtReadoutElement(channelId);
        if (!descriptor){
          ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
                          <<m_idHelperSvc->toString(channelId)<<">  =>>ignore this hit");
          delete digit;
          continue;
        }
	  
        if (digit->is_masked()) {
          digitStatus = Muon::MdtStatusMasked;
        }else{
          digitStatus = getMdtDriftRadius(digit, radius, errRadius, descriptor, muDetMgr);
          if( radius < -999 ) {
            ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
                            << " Id is: "<<m_idHelperSvc->toString(channelId));
          }
        }
	  
        Amg::Vector2D driftRadius(radius,0);
        Amg::MatrixX* cov = new Amg::MatrixX(1,1);
        (*cov)(0,0) = errRadius*errRadius;
	  
        // Create new PrepData
        MdtPrepData *newPrepData = new MdtPrepData(channelId,
                                                   mdtHashId,
                                                   driftRadius,
                                                   cov,
                                                   descriptor,
                                                   digit->tdc(),
                                                   digit->adc(),
                                                   digitStatus);
	  
        newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
        driftCircleColl->push_back(newPrepData);
	  
        ATH_MSG_DEBUG(" MADE ORIGINAL PREPDATA " << m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(channelId)) 
                      << "  eta = " << m_idHelperSvc->mdtIdHelper().stationEta(channelId)
                      << "  phi = " << m_idHelperSvc->mdtIdHelper().stationPhi(channelId)
                      << "  ml = " << m_idHelperSvc->mdtIdHelper().multilayer(channelId)
                      << "  layer = " << m_idHelperSvc->mdtIdHelper().tubeLayer(channelId) 
                      << "  tube = " <<  m_idHelperSvc->mdtIdHelper().tube(channelId) 
                      << "  radius = " << radius << " +- " << errRadius);
	  
        delete digit;
      }// end if(!second_digit){
      else{
	  
        // define twin position and error
        double zTwin(0.); double errZTwin(0.); bool secondHitIsPrompt(false);
        // define drift-radius and error
        double radius(0.); double errRadius(0.);
        Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;
        // call the function to calculate radii and twin coordinate
        digitStatus = getMdtTwinPosition(digit, second_digit, radius, errRadius, zTwin, errZTwin, secondHitIsPrompt, muDetMgr);
        if( zTwin <-99999 ) {   ATH_MSG_WARNING("MDT Twin PrepData with very large, negative twin coordinate " << zTwin
                                                << " Id is: "<<m_idHelperSvc->toString(digit->identify())
                                                << " Twin Id is: "<<m_idHelperSvc->toString(second_digit->identify())); }
	  
        // set the properties of PrepData-object to the tube that was PROMPT (= hit by the muon)
        Identifier promptHit_channelId = digit->identify();
        IdentifierHash promptHit_channelHash = digit->identifyHash();
        MdtDigit* promptHit_Digit = digit; 
        Identifier twinHit_channelId = second_digit->identify();
        IdentifierHash twinHit_channelHash = second_digit->identifyHash();
        MdtDigit* twinHit_Digit = second_digit; 
	  
        if(secondHitIsPrompt){
          promptHit_channelId = second_digit->identify();
          promptHit_channelHash = second_digit->identifyHash();
          promptHit_Digit = second_digit;
          twinHit_channelId = digit->identify();
          twinHit_channelHash = digit->identifyHash();
          twinHit_Digit = digit; 
        }
	  
        // do lookup once
        const MdtReadoutElement * descriptor = muDetMgr->getMdtReadoutElement(promptHit_channelId);
        if (!descriptor){
          ATH_MSG_WARNING("Detector Element not found for Identifier from the DetManager <"
                          <<m_idHelperSvc->toString(promptHit_channelId)<<">  =>>ignore this hit");
          delete digit;
          continue;
        }
	  
        // check if digit is masked
        if (promptHit_Digit->is_masked()) {
          digitStatus = Muon::MdtStatusMasked;
        }else if( radius < -999 ) {
          ATH_MSG_WARNING("MDT Twin PrepData with very large, negative radius "
                          << " Id is: "<<m_idHelperSvc->toString(promptHit_channelId));
        }
	  
          
        Amg::Vector2D driftRadiusZTwin(radius,zTwin);
        // make a 2x2 matrix with all values initialized at 0
        Amg::MatrixX* cov = new Amg::MatrixX(2,2);
        (*cov)(0,0) = errRadius*errRadius;
        (*cov)(1,1) = errZTwin*errZTwin;
        (*cov)(0,1) = 0;
        (*cov)(1,0) = 0;
          
          
        // Create new PrepData either w/ or w/o twin hit info depending on m_use1DPrepDataTwin flag
        if(!m_use1DPrepDataTwin){
          MdtTwinPrepData *twin_newPrepData = new MdtTwinPrepData(promptHit_channelId,
                                                                  //promptHit_channelHash,
                                                                  mdtHashId,  
                                                                  driftRadiusZTwin,
                                                                  cov,
                                                                  descriptor,
                                                                  promptHit_Digit->tdc(),
                                                                  promptHit_Digit->adc(),
                                                                  twinHit_Digit->tdc(),
                                                                  twinHit_Digit->adc(),
                                                                  digitStatus);
	    
          ATH_MSG_DEBUG(" MADE A 2D TWINPREPDATA " << m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(promptHit_channelId))
                        << "  eta = " << m_idHelperSvc->mdtIdHelper().stationEta(promptHit_channelId)
                        << "  phi = " << m_idHelperSvc->mdtIdHelper().stationPhi(promptHit_channelId)
                        << "  ml = " << m_idHelperSvc->mdtIdHelper().multilayer(promptHit_channelId)
                        << "  layer = " << m_idHelperSvc->mdtIdHelper().tubeLayer(promptHit_channelId) << " tube = " <<  m_idHelperSvc->mdtIdHelper().tube(promptHit_channelId) 
                        << "  zTwin = " << zTwin << " +- " << errZTwin 
                        << "  radius = " << radius << " +- " << errRadius);
	    
          Amg::Vector3D gpos_centertube = twin_newPrepData->globalPosition();
          const MdtReadoutElement* detEl = muDetMgr->getMdtReadoutElement(promptHit_channelId);
          Amg::Vector3D locpos_centertube = Amg::Vector3D(0.,0.,zTwin);
          Amg::Vector3D gpos_twin = detEl->localToGlobalCoords(locpos_centertube, promptHit_channelId);
	    
          ATH_MSG_DEBUG(" global pos center tube  x = " << gpos_centertube.x() << " y = " << gpos_centertube.y() << " z = " << gpos_centertube.z());
          ATH_MSG_DEBUG(" local pos center tube w/ TWIN INFO  x = " << locpos_centertube.x() << " y = " << locpos_centertube.y() << " z = " << locpos_centertube.z());
          ATH_MSG_DEBUG(" global pos w/ TWIN INFO  x = " << gpos_twin.x() << " y = " << gpos_twin.y() << " z = " << gpos_twin.z());
	    
          twin_newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
          driftCircleColl->push_back(twin_newPrepData);
	    
	    
        } // end if(!m_use1DPrepDataTwin){
        else{
          Amg::Vector2D driftRadius(radius,0);
          // make a 2x2 matrix with all values initialized at 0
          Amg::MatrixX* cov = new Amg::MatrixX(1,1);
          (*cov)(0,0) = errRadius*errRadius;
	    
          MdtPrepData *twin_newPrepData = new MdtPrepData(promptHit_channelId,
                                                          promptHit_channelHash,  
                                                          driftRadius,
                                                          cov,
                                                          descriptor,
                                                          promptHit_Digit->tdc(),
                                                          promptHit_Digit->adc(),
                                                          digitStatus);
	    
          ATH_MSG_DEBUG(" MADE A 1D(=original) PREPDATA OUT OF TWINPAIR "
                        << "   TWIN COORDINATE IS NOT STORED IN PREPDATA "
                        << m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(promptHit_channelId))
                        << "  eta = " << m_idHelperSvc->mdtIdHelper().stationEta(promptHit_channelId)
                        << "  phi = " << m_idHelperSvc->mdtIdHelper().stationPhi(promptHit_channelId)
                        << "  ml = " << m_idHelperSvc->mdtIdHelper().multilayer(promptHit_channelId)
                        << "  layer = " << m_idHelperSvc->mdtIdHelper().tubeLayer(promptHit_channelId) << " tube = " <<  m_idHelperSvc->mdtIdHelper().tube(promptHit_channelId) 
                        << "  zTwin = " << zTwin << " +- " << errZTwin 
                        << "  radius = " << radius << " +- " << errRadius);
	    
          twin_newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
          driftCircleColl->push_back(twin_newPrepData);
	    
        }// end else --  if(!m_use1DPrepDataTwin){
	  
        delete digit;
        delete second_digit;
	  
      }// end else --  if(!second_digit){
    } // end -- if(multilayer==1)
    else if(multilayer==2){
      // if multilayer=2, then treat every hit as a separate hit, no twin hit should be present here as the hardware is not installed
        
      if(!second_digit){

        ATH_MSG_VERBOSE("got digit with id ext / hash "
                        <<m_idHelperSvc->toString(channelId)<<" / "
                        <<(int)channelHash);
	  
        double radius(0.); double errRadius(0.);
        Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;
	  
        // do lookup once
        const MdtReadoutElement * descriptor = muDetMgr->getMdtReadoutElement(channelId);
        if (!descriptor){
          ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
                          <<m_idHelperSvc->toString(channelId)<<">  =>>ignore this hit");
          delete digit;
          continue;
        }
	  
        if (digit->is_masked()) {
          digitStatus = Muon::MdtStatusMasked;
        }else{
          digitStatus = getMdtDriftRadius(digit, radius, errRadius, descriptor, muDetMgr);
          if( radius < -999 ) {
            ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
                            << " Id is: "<<m_idHelperSvc->toString(channelId));
          }
        }
	  
        Amg::Vector2D driftRadius(radius,0);
        Amg::MatrixX* cov = new Amg::MatrixX(1,1);
        (*cov)(0,0) = errRadius*errRadius;

	  
        // Create new PrepData
        MdtPrepData *newPrepData = new MdtPrepData(channelId,
                                                   mdtHashId,
                                                   driftRadius,
                                                   cov,
                                                   descriptor,
                                                   digit->tdc(),
                                                   digit->adc(),
                                                   digitStatus);
	  
        newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
        driftCircleColl->push_back(newPrepData);
	  
        ATH_MSG_DEBUG(" MADE ORIGINAL PREPDATA " << m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(channelId)) 
                      << "  eta = " << m_idHelperSvc->mdtIdHelper().stationEta(channelId)
                      << "  phi = " << m_idHelperSvc->mdtIdHelper().stationPhi(channelId)
                      << "  ml = " << m_idHelperSvc->mdtIdHelper().multilayer(channelId)
                      << "  layer = " << m_idHelperSvc->mdtIdHelper().tubeLayer(channelId) 
                      << "  tube = " <<  m_idHelperSvc->mdtIdHelper().tube(channelId) 
                      << "  radius = " << radius << " +- " << errRadius);
	  
        delete digit;
        delete second_digit;
      } //end  --  if(!second_digit){
      else{
	  
        // Do something with second_digit
        Identifier     second_channelId   = second_digit->identify();
        IdentifierHash second_channelHash = second_digit->identifyHash();
	  
        ATH_MSG_VERBOSE("got digit with id ext / hash "
                        <<m_idHelperSvc->toString(channelId)<<" / "
                        <<(int)channelHash);
	  
        //second_digit
        ATH_MSG_VERBOSE("got second_digit with id ext / hash "
                        <<m_idHelperSvc->toString(second_channelId)<<" / "
                        <<(int)second_channelHash);
	  
	  
        // Calculate radius
        double radius(0.); double errRadius(0.);
        Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;
	
        //second_digit
        double second_radius(0.); double second_errRadius(0.);
        Muon::MdtDriftCircleStatus second_digitStatus = Muon::MdtStatusDriftTime;
	  
        // do lookup once
        const MdtReadoutElement * descriptor = muDetMgr->getMdtReadoutElement(channelId);
        if (!descriptor){
          ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
                          <<m_idHelperSvc->toString(channelId)<<">  =>>ignore this hit");
          delete digit;
          continue;
        }
	  
        if (digit->is_masked()) {
          digitStatus = Muon::MdtStatusMasked;
        }else{
          digitStatus = getMdtDriftRadius(digit, radius, errRadius, descriptor, muDetMgr);
          if( radius < -999 ) {
            ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
                            << " Id is: "<<m_idHelperSvc->toString(channelId));
          }
        }

        const MdtReadoutElement * second_descriptor = muDetMgr->getMdtReadoutElement(second_channelId);
        if (!second_descriptor){
          ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
                          <<m_idHelperSvc->toString(second_channelId)<<">  =>>ignore this hit");
          delete second_digit;
          continue;
        }
	  
        //second_digit
        if (second_digit->is_masked()) {
          second_digitStatus = Muon::MdtStatusMasked;
        }else{
          second_digitStatus = getMdtDriftRadius(second_digit, second_radius, second_errRadius, second_descriptor, muDetMgr);
          if( second_radius < -999 ) {
            ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
                            << " Id is: "<<m_idHelperSvc->toString(second_channelId));
          }
        }
	  
        Amg::Vector2D driftRadius(radius,0);
        Amg::MatrixX* cov = new Amg::MatrixX(1,1);
        (*cov)(0,0) = errRadius*errRadius;

	  
        //second_digit
        Amg::Vector2D second_driftRadius(second_radius,0);
        Amg::MatrixX* cov2 = new Amg::MatrixX(1,1);
        (*cov2)(0,0) = second_errRadius*second_errRadius;

        // Create new PrepData
        MdtPrepData *newPrepData = new MdtPrepData(channelId,
                                                   mdtHashId,
                                                   driftRadius,
                                                   cov,
                                                   descriptor,
                                                   digit->tdc(),
                                                   digit->adc(),
                                                   digitStatus);
	  
        newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
        driftCircleColl->push_back(newPrepData);
	  
        //second_digit
        // Create new PrepData
        MdtPrepData *second_newPrepData = new MdtPrepData(second_channelId,
                                                          mdtHashId,
                                                          second_driftRadius,
                                                          cov2,
                                                          second_descriptor,
                                                          second_digit->tdc(),
                                                          second_digit->adc(),
                                                          second_digitStatus);
	  
        second_newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
        driftCircleColl->push_back(second_newPrepData);
	  
        ATH_MSG_DEBUG(" MADE ORIGINAL PREPDATA " << m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(channelId)) 
                      << "  eta = " << m_idHelperSvc->mdtIdHelper().stationEta(channelId)
                      << "  phi = " << m_idHelperSvc->mdtIdHelper().stationPhi(channelId)
                      << "  ml = " << m_idHelperSvc->mdtIdHelper().multilayer(channelId)
                      << "  layer = " << m_idHelperSvc->mdtIdHelper().tubeLayer(channelId) 
                      << "  tube = " <<  m_idHelperSvc->mdtIdHelper().tube(channelId) 
                      << "  radius = " << radius << " +- " << errRadius);
	  
        //second_digit
        ATH_MSG_DEBUG(" MADE ORIGINAL PREPDATA FOR SECOND DIGIT " << m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(second_channelId))
                      << "  eta = " << m_idHelperSvc->mdtIdHelper().stationEta(second_channelId)
                      << "  phi = " << m_idHelperSvc->mdtIdHelper().stationPhi(second_channelId)
                      << "  ml = " << m_idHelperSvc->mdtIdHelper().multilayer(second_channelId)
                      << "  layer = " << m_idHelperSvc->mdtIdHelper().tubeLayer(second_channelId) 
                      << "  tube = " <<  m_idHelperSvc->mdtIdHelper().tube(second_channelId) 
                      << "  radius = " << second_radius << " +- " << second_errRadius);
	  
        delete digit;
        delete second_digit;
      } //end  --  else -- if(!second_digit){
    }
    else{
      ATH_MSG_DEBUG("Something strange in MdtRdoToPrepDataToolCore, MDT multilayer (must be 1 or 2)= " << multilayer);
    }
      
      
  } // end for( iter_map = mdtDigitColl.begin(); iter_map != mdtDigitColl.end(); iter_map++ )
    
  // Finished decoding RDO to PRD, so now we record the collection in the container
  StatusCode status_lock = lock.addOrDelete(std::move( driftCircleColl ));
  if (status_lock.isFailure()) {
    ATH_MSG_ERROR ( "Could not insert MdtCsmPrepdataCollection into MdtCsmPrepdataContainer..." );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


MdtDriftCircleStatus MdtRdoToPrepDataToolCore::getMdtDriftRadius(const MdtDigit* digit, double& radius, double& errRadius, const MuonGM::MdtReadoutElement* descriptor, const MuonGM::MuonDetectorManager* muDetMgr) {

  ATH_MSG_DEBUG("in getMdtDriftRadius()");

  if( m_calibratePrepData ){
    Identifier channelId = digit->identify();

    // here check validity
    // if invalid, reset flags
    if (!descriptor->containsId(channelId)) {
      radius = -1000.;
      ATH_MSG_WARNING("Identifier from the cabling service <"
                      <<m_idHelperSvc->toString(channelId)<<"> inconsistent with the geometry of detector element <"
                      <<m_idHelperSvc->toString(descriptor->identify())<<">  =>>ignore this hit");
      return MdtStatusUnDefined;
    }  

    // use center (cached) to get the tube position instead of tubepos
    const Amg::Vector3D& position = descriptor->center(channelId);
    MdtCalibHit calibHit(channelId, digit->tdc(), digit->adc(), position, descriptor);

    MdtCalibrationSvcInput inputData;
    double signedTrackLength = position.mag();
    inputData.tof = signedTrackLength*inverseSpeedOfLight;

    calibHit.setGlobalPointOfClosestApproach(position);

    bool drift_ok = m_calibrationTool->driftRadiusFromTime(calibHit,inputData,*m_mdtCalibSvcSettings,false);
    if (!drift_ok) {
      if( calibHit.driftTime() < 0. ) return MdtStatusBeforeSpectrum;
      else                             return MdtStatusAfterSpectrum;
    }
    radius = calibHit.driftRadius();
    errRadius = calibHit.sigmaDriftRadius();
    ATH_MSG_VERBOSE("Calibrated drift radius is "<<radius<<"+/-"<<errRadius);
  }
  else
    {
      Identifier channelId = digit->identify();
      radius = 0.;
      errRadius = muDetMgr->getMdtReadoutElement(channelId)->innerTubeRadius()/sqrt(12); // 14.6/sqrt(12)
    }
  return MdtStatusDriftTime;
}


MdtDriftCircleStatus MdtRdoToPrepDataToolCore::getMdtTwinPosition(const MdtDigit* digit, const MdtDigit* second_digit, double& radius, double& errRadius, double& zTwin, double& errZTwin, bool& secondHitIsPrompt, const MuonGM::MuonDetectorManager* muDetMgr) {

  ATH_MSG_DEBUG("in getMdtTwinPosition()");

  if( m_calibratePrepData ){
    
    // start digit
    Identifier channelId = digit->identify();
    const MdtReadoutElement * descriptor = muDetMgr->getMdtReadoutElement(channelId);

    // here check validity
    // if invalid, reset flags
    if (!descriptor){
      ATH_MSG_WARNING("getMdtTwinPosition(): Detector Element not found for Identifier from the cabling service <"
                      <<m_idHelperSvc->toString(channelId)<<">  =>>ignore this hit");
      zTwin = -100000.;
      return MdtStatusUnDefined;
    }
    else if (!descriptor->containsId(channelId)){
      zTwin = -100000.;
      ATH_MSG_WARNING("getMdtTwinPosition(): Identifier from the cabling service <"
                      <<m_idHelperSvc->toString(channelId)<<"> inconsistent with the geometry of detector element <"
                      <<m_idHelperSvc->toString(descriptor->identify())<<">  =>>ignore this hit");
      return MdtStatusUnDefined;
    }
    
    Amg::Vector3D position = descriptor->tubePos(channelId);
    double measured_perp = position.perp();
    if (descriptor->getStationS() != 0.) {
      measured_perp = sqrt(measured_perp*measured_perp-descriptor->getStationS()*descriptor->getStationS());
    }
    double measured_x = measured_perp * cos( position.phi() );
    double measured_y = measured_perp * sin( position.phi() );
    const Amg::Vector3D measured_position(measured_x, measured_y, position.z());
    MdtCalibHit calib_hit = MdtCalibHit( channelId, digit->tdc(), digit->adc(), measured_position, descriptor);
    calib_hit.setGlobalPointOfClosestApproach(measured_position);
    double signedTrackLength =  measured_position.mag();
     
    // start second digit
    Identifier second_channelId = second_digit->identify();
    const MdtReadoutElement * second_descriptor = muDetMgr->getMdtReadoutElement(second_channelId);
    
    // here check validity
    // if invalid, reset flags
    if (!second_descriptor){
      ATH_MSG_WARNING("getMdtTwinPosition(): Detector Element not found for Identifier from the cabling service <"
                      <<m_idHelperSvc->toString(second_channelId)<<">  =>>ignore this hit");
      zTwin = -100000.;
      return MdtStatusUnDefined;
    }
    else if (!second_descriptor->containsId(second_channelId)){
      zTwin = -100000.;
      ATH_MSG_WARNING("getMdtTwinPosition(): Identifier from the cabling service <"
                      <<m_idHelperSvc->toString(second_channelId)<<"> inconsistent with the geometry of detector element <"
                      <<m_idHelperSvc->toString(second_descriptor->identify())<<">  =>>ignore this hit");
      return MdtStatusUnDefined;
    }
    
    Amg::Vector3D second_position = second_descriptor->tubePos(second_channelId);
    double second_measured_perp = second_position.perp();
    if (second_descriptor->getStationS() != 0.) {
      second_measured_perp = sqrt(second_measured_perp*second_measured_perp-second_descriptor->getStationS()*second_descriptor->getStationS());
    }
    double second_measured_x = second_measured_perp * cos( second_position.phi() );
    double second_measured_y = second_measured_perp * sin( second_position.phi() );
    const Amg::Vector3D second_measured_position(second_measured_x, second_measured_y, second_position.z());
    MdtCalibHit second_calib_hit = MdtCalibHit( second_channelId, second_digit->tdc(), second_digit->adc(),
                                                second_measured_position,  second_descriptor );
    double second_signedTrackLength =  second_measured_position.mag();

    // calculate and calibrate radius for both hits and calculate twin position
    second_calib_hit.setGlobalPointOfClosestApproach(second_measured_position);

    bool second_ok = m_calibrationTool->twinPositionFromTwinHits(calib_hit, second_calib_hit, signedTrackLength, second_signedTrackLength, secondHitIsPrompt);
    if (!second_ok){
      if( calib_hit.driftTime() < 0. || second_calib_hit.driftTime() < 0. ) return MdtStatusBeforeSpectrum;
      else                             return MdtStatusAfterSpectrum;
    } 
    
    // set radius and error to the prompt tube (tube that was actually hit by the muon)
    radius = calib_hit.driftRadius();
    errRadius = calib_hit.sigmaDriftRadius();
    if(secondHitIsPrompt){
      radius = second_calib_hit.driftRadius();
      errRadius = second_calib_hit.sigmaDriftRadius();
    }

    zTwin = second_calib_hit.xtwin();
    errZTwin = second_calib_hit.sigmaXtwin();

    ATH_MSG_VERBOSE(" Calibrated drift radius of prompt hit (of twin pair) is " << radius << "+/-" << errRadius);
    ATH_MSG_VERBOSE(" Calibrated twin coordinate = " << zTwin << "+/-" << errZTwin);

  } // end if(m_calibratePrepData)    
  else{
    Identifier channelId = digit->identify();
    radius = 0.;
    errRadius = muDetMgr->getMdtReadoutElement(channelId)->innerTubeRadius()/sqrt(12); // 14.6/sqrt(12)
    zTwin = 0.;
    double tubelength = muDetMgr->getMdtReadoutElement(channelId)->getTubeLength(m_idHelperSvc->mdtIdHelper().tubeLayer(channelId),m_idHelperSvc->mdtIdHelper().tube(channelId));
    errZTwin = tubelength/2.;
  }
  
  return MdtStatusDriftTime;
}

void MdtRdoToPrepDataToolCore::initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl) {
  PVConstLink cv = mydetEl->getMaterialGeom(); // it is "Multilayer"
  int nGrandchildren = cv->getNChildVols();
  if(nGrandchildren <= 0) return;

  std::vector<int> tubes;
  geoGetIds ([&] (int id) { tubes.push_back (id); }, &*cv);
  std::sort (tubes.begin(), tubes.end());

  Identifier detElId = mydetEl->identify();

  int name = m_idHelperSvc->mdtIdHelper().stationName(detElId);
  int eta = m_idHelperSvc->mdtIdHelper().stationEta(detElId);
  int phi = m_idHelperSvc->mdtIdHelper().stationPhi(detElId);
  int ml = m_idHelperSvc->mdtIdHelper().multilayer(detElId);
  std::vector<Identifier> deadTubes;

  std::vector<int>::iterator it = tubes.begin();
  for(int layer = 1; layer <= mydetEl->getNLayers(); layer++){
    for(int tube = 1; tube <= mydetEl->getNtubesperlayer(); tube++){
      int want_id = layer*100 + tube;
      if (it != tubes.end() && *it == want_id) {
        ++it;
      }
      else {
        it = std::lower_bound (tubes.begin(), tubes.end(), want_id);
        if (it != tubes.end() && *it == want_id) {
          ++it;
        }
        else {
          Identifier deadTubeId = m_idHelperSvc->mdtIdHelper().channelID( name, eta, phi, ml, layer, tube );
          deadTubes.push_back( deadTubeId );
          ATH_MSG_VERBOSE("adding dead tube (" << tube  << "), layer(" <<  layer
                          << "), phi(" << phi << "), eta(" << eta << "), name(" << name
                          << "), multilayerId(" << ml << ") and identifier " << deadTubeId <<" .");
        }
      }
    }
  }
  std::sort(deadTubes.begin(), deadTubes.end());
  m_DeadChannels[detElId] = deadTubes;
  return;
}
