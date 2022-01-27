/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigit.h"

#include "MuonRDO/MdtCsmIdHash.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/MdtCsm.h"

#include "MuonByteStreamCnvTest/MdtDigitToMdtRDO.h"

#include "EventInfoMgt/ITagInfoMgr.h"

#include <algorithm>
#include <cmath>
#include <atomic>

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
namespace {
   // elevator chambers are read out by 2 CSMs
   // they are split in the middle (for both multilayers)
   // the first tube read out by the 2nd CSM is (offline!) tube 43
   constexpr int BME_1st_tube_2nd_CSM = 43;
   
   
   /// Print one-time warnings about the missing BIS78 cabling
   std::atomic<bool> bisWarningPrinted = false;
   /// Print one-time warings about cases where the BMGs are part of the
   /// geometry but not implemented in the cabling. That should only happen in
   /// mc16a like setups.
   std::atomic<bool> bmgWarningPrinted = false;
   
}


MdtDigitToMdtRDO::MdtDigitToMdtRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MdtDigitToMdtRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_csmContainerKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_csmContainerKey );
  ATH_CHECK( m_digitContainerKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_digitContainerKey );
  ATH_CHECK( m_idHelperSvc.retrieve() );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_condKey.initialize() );

  if ( fillTagInfo().isFailure() ) {
    ATH_MSG_WARNING( "Could not fill the tagInfo for MDT cabling"  );
  }

  // check if the layout includes elevator chambers
  m_BME_station_name = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME");
  m_BMEpresent = m_BME_station_name != -1;
  if ( m_BMEpresent ){
    ATH_MSG_INFO( "Processing configuration for layouts with BME chambers (stationID: "<<m_BME_station_name<<")."  );
  }
  m_BMG_station_name =  m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG");
  m_BMGpresent =  m_BMG_station_name != -1;
  if ( m_BMGpresent ){
    ATH_MSG_INFO( "Processing configuration for layouts with BME chambers (stationID: "<<m_BMG_station_name<<")."  );
  }
  m_BIS_station_name = m_idHelperSvc->mdtIdHelper().stationNameIndex("BIS");
  return StatusCode::SUCCESS;
}
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode MdtDigitToMdtRDO::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG( "in execute()"  );

  ATH_CHECK(fill_MDTdata(ctx));

  return StatusCode::SUCCESS;
}
 std::unique_ptr<MdtCsm> MdtDigitToMdtRDO::make_csm(const MuonMDT_CablingMap* cabling_ptr, const Identifier module_id, IdentifierHash module_hash, bool need_second) const{
    MuonMDT_CablingMap::CablingData cabling_data{};
    if (!cabling_ptr->convert(module_id,cabling_data)){
        ATH_MSG_ERROR("Failed to convert the module "<<m_idHelperSvc->toString(module_id));
        return nullptr;
    }
    cabling_data.tube= 1;
    if (m_BMEpresent && cabling_data.stationIndex == m_BME_station_name && need_second){
          cabling_data.tube = BME_1st_tube_2nd_CSM;
    }      
    if(!cabling_ptr->getOnlineId(cabling_data, msgStream())){
        if (cabling_data.stationIndex == m_BMG_station_name){
            if (!bmgWarningPrinted) {
                ATH_MSG_WARNING("Apparently BMG chambers are disconnected to the cabling. "<<
                               "This has been checked to only appear in mc16a-like setups as the chambers were installed in the end-of-the-year shutdown 2016. "<<
                               "In any other case, be despaired in facing the villian and check what has gone wrong");
                bmgWarningPrinted = true;                    
            }
            return nullptr;
        }
        ATH_MSG_ERROR( "MDTcabling can't return an online ID for the channel : "<<cabling_data);
        return nullptr;
    }
    return std::make_unique<MdtCsm>(module_id, module_hash, cabling_data.subdetectorId, cabling_data.mrod, cabling_data.csm);
}

StatusCode MdtDigitToMdtRDO::fill_MDTdata(const EventContext& ctx) const {

  ATH_MSG_DEBUG( "in execute() : fill_MDTdata"  );
  // create an empty pad container and record it
  SG::WriteHandle<MdtCsmContainer> csmContainer(m_csmContainerKey, ctx);
  ATH_CHECK(csmContainer.record(std::make_unique<MdtCsmContainer>()));
  ATH_MSG_DEBUG("Recorded MdtCsmContainer called " << csmContainer.name() << " in store " << csmContainer.store());

  SG::ReadHandle<MdtDigitContainer> container (m_digitContainerKey, ctx);
  if (!container.isValid()) {
    ATH_MSG_ERROR("Could not find MdtDigitContainer called " << container.name() << " in store " << container.store());
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Found MdtDigitContainer called " << container.name() << " in store " << container.store());

  
  MdtCsmIdHash hashF;
  
  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle_Cabling{m_cablingKey, ctx};
  const MuonMDT_CablingMap* cabling_ptr{*readHandle_Cabling};
  if(!cabling_ptr){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<MdtCondDbData> readHandle_Conditions{m_condKey, ctx};
  const MdtCondDbData* condtionsPtr{*readHandle_Conditions};
  if(!condtionsPtr){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  auto &msg = msgStream();

  // Iterate on the collections
  for ( const MdtDigitCollection* mdtCollection : *container) {
      IdentifierHash moduleHash = mdtCollection->identifierHash();
      Identifier moduleId = mdtCollection->identify();
      
      // Get the online ID of the MDT module
      Identifier chid1, chid2;
      if ( m_BMEpresent ){
          const int name    = m_idHelperSvc->mdtIdHelper().stationName(moduleId);
          const int eta     = m_idHelperSvc->mdtIdHelper().stationEta(moduleId);
          const int phi     = m_idHelperSvc->mdtIdHelper().stationPhi(moduleId);
 
            /// 1st ML channel get_id
            chid1 = m_idHelperSvc->mdtIdHelper().channelID(name,
                                                           eta,
                                                           phi,
                                                           1, 1, 1 );
            /// 2nd ML channel id
            if ( name == m_BME_station_name ) {
                chid2 = m_idHelperSvc->mdtIdHelper().channelID(name,
                                                               eta,
                                                               phi,
                                                               2, 1, 1 );        
            }
      } else{
          chid1 = moduleId;
      }
      /// Remove dead tubes from the container
      if (!condtionsPtr->isGood(chid1)) continue;

      MuonMDT_CablingMap::CablingData cabling_data;
      if (!cabling_ptr->convert(chid1,cabling_data)){
        ATH_MSG_FATAL("Found a non mdt identifier "<<m_idHelperSvc->toString(chid1));
        return StatusCode::FAILURE;
      }
      std::unique_ptr<MdtCsm> mdtCsm {nullptr}, mdtCsm_2nd{nullptr};
      
      mdtCsm = make_csm(cabling_ptr, chid1, moduleHash, false);
      if (cabling_data.stationIndex == m_BME_station_name) mdtCsm_2nd =  make_csm(cabling_ptr, chid2, hashF(chid2), true);
      /// Iterate on the digits of the collection
      for (const MdtDigit* mdtDigit: *mdtCollection) {
        Identifier channelId = mdtDigit->identify();
	    
        if (!m_idHelperSvc->mdtIdHelper().valid(channelId) || !cabling_ptr->convert(channelId,cabling_data)) {
            ATH_MSG_DEBUG("Found invalid mdt identifier "<<channelId);
            continue;
        }
       
               
        /// Get the online Id of the channel
        bool cabling = cabling_ptr->getOnlineId(cabling_data, msg);
	            
       if (!cabling) {
          if (cabling_data.stationIndex == m_BMG_station_name) {
                if (!bmgWarningPrinted){
                    ATH_MSG_WARNING("Apparently BMG chambers are disconnected to the cabling. "<<
                                    "This has been checked to only appear in mc16a-like setups as the chambers were installed in the end-of-the-year shutdown 2016. "<<
                                    "In any other case, be despaired in facing the villian and check what has gone wrong");
                    bmgWarningPrinted = true;                    
                }
                continue;
          }
          // as long as there is no BIS sMDT cabling, to avoid a hard crash, replace the tubeNumber
          // of tubes not covered in the cabling by 1
          if (m_idHelperSvc->mdtIdHelper().stationName(channelId)== m_BIS_station_name && m_idHelperSvc->issMdt(channelId)) {
                 if (!bisWarningPrinted) {
                    ATH_MSG_WARNING("Found BIS sMDT with tubeLayer="<<cabling_data.layer<<" and tubeNumber="<<cabling_data.tube<<". Setting to "<<cabling_data.layer<<",1 until a proper cabling is implemented, cf. ATLASRECTS-5804");
                    bisWarningPrinted = true;
                 }
               cabling_data.layer =  std::min(cabling_data.layer, 3);
               cabling_data.tube = 1;
               cabling = cabling_ptr->getOnlineId(cabling_data, msg);
          }
          if (!cabling) {
                ATH_MSG_ERROR( "MDTcabling can't return an online ID for the channel : "<<cabling_data );
                return StatusCode::FAILURE;
          }
       } 

        bool masked = mdtDigit->is_masked();
        // Create the new AMT hit
        std::unique_ptr<MdtAmtHit> amtHit = std::make_unique<MdtAmtHit>(cabling_data.tdcId,cabling_data.channelId, masked);
	            
	    // Get coarse time and fine time
	    int tdc_counts = mdtDigit->tdc();

	    uint16_t coarse = (tdc_counts>>5) & 0xfff;
	    uint16_t fine = tdc_counts & 0x1f;
	    uint16_t width = mdtDigit->adc();
	            
	    amtHit->setValues(coarse, fine, width);
	            
	    ATH_MSG_DEBUG( "Adding a new AmtHit -- "<<cabling_data  );
	    ATH_MSG_DEBUG( " Coarse time : " << coarse 
                             << " Fine time : " << fine 
                             << " Width : " << width  );

        if (!mdtCsm) mdtCsm = make_csm(cabling_ptr, chid1, moduleHash, false);
          
        // Add the digit to the CSM
	    if( cabling_data.stationIndex != m_BME_station_name || cabling_data.csm == mdtCsm->CsmId()) mdtCsm->push_back(amtHit.release());
	    else if (cabling_data.stationIndex == m_BME_station_name){
            if (!mdtCsm_2nd) mdtCsm_2nd = make_csm(cabling_ptr, chid2, hashF(chid2), true);
            if (cabling_data.csm == mdtCsm_2nd->CsmId()) mdtCsm_2nd->push_back(amtHit.release());
            else {
               ATH_MSG_ERROR( "There's a BME digit that doesn't match a CSM"  );               
            }
        }
	}

    /// Add the CSM to the CsmContainer
    if (mdtCsm && csmContainer->addCollection(mdtCsm.release(), hashF(chid1)).isFailure())
            ATH_MSG_WARNING( "Unable to record MDT CSM in IDC"  );
    if (mdtCsm_2nd && csmContainer->addCollection(mdtCsm_2nd.release(), hashF(chid2)).isFailure())
	  ATH_MSG_WARNING( "Unable to record MDT CSM in IDC 2nd"  );
      
  }
  return StatusCode::SUCCESS;

}

// NOTE: although this function has no clients in release 22, currently the Run2 trigger simulation is still run in
//       release 21 on RDOs produced in release 22. Since release 21 accesses the TagInfo, it needs to be written to the
//       RDOs produced in release 22. The fillTagInfo() function thus needs to stay in release 22 until the workflow changes
StatusCode MdtDigitToMdtRDO::fillTagInfo() const {

  ServiceHandle<ITagInfoMgr> tagInfoMgr ("TagInfoMgr", name());
  if (tagInfoMgr.retrieve().isFailure()){
    return StatusCode::FAILURE;
  }

  std::string cablingType="NewMDT_Cabling"; // everything starting from Run2 should be 'New'
  StatusCode sc = tagInfoMgr->addTag("MDT_CablingType",cablingType); 
  
  if(sc.isFailure()) {
    ATH_MSG_WARNING( "MDT_CablingType " << cablingType
                     << " not added to TagInfo "  );
    return sc;
  } else {
    ATH_MSG_DEBUG( "MDT_CablingType " << cablingType
                   << " is Added TagInfo "  );
  }
  
  return StatusCode::SUCCESS;

}
