/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "MdtCablingTestAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"



MdtCablingTestAlg::MdtCablingTestAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator) {}


StatusCode MdtCablingTestAlg::initialize(){
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_CHECK(m_cablingKey.initialize());
  return StatusCode::SUCCESS;
} 

StatusCode MdtCablingTestAlg::execute(){
  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_MSG_INFO("Start validation of the Mdt cabling");

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> detectorMgr{m_DetectorManagerKey, ctx};
  if (!detectorMgr.isValid()){
      ATH_MSG_FATAL("Failed to retrieve the Detector manager "<<m_DetectorManagerKey.fullKey());
      return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<MuonMDT_CablingMap> cabling{m_cablingKey,ctx};
  if (!cabling.isValid()) {
     ATH_MSG_ERROR("Failed to retrieve the Mdt cabling "<<m_cablingKey.fullKey());
     return StatusCode::FAILURE;
  }
  const MdtIdHelper& idHelper = m_idHelperSvc->mdtIdHelper();
  unsigned int n_elements{0}, n_success{0};
  bool failure{false};
  for (unsigned int hash = 0; hash < MuonGM::MuonDetectorManager::MdtRElMaxHash; ++hash){
    const IdentifierHash id_hash{hash};

    const MuonGM::MdtReadoutElement* readEle = detectorMgr->getMdtReadoutElement(id_hash);
    if (!readEle) {
        ATH_MSG_DEBUG("Detector element does not exist. ");
        continue;
    }
    const Identifier station_id = idHelper.elementID(readEle->identify());
    ATH_MSG_DEBUG("Check station "<<m_idHelperSvc->toString(station_id));
    for (int layer = 1 ; layer <= readEle->getNLayers(); ++layer){
      for (int tube = 1 ; tube <= readEle->getNtubesperlayer(); ++tube){
          bool is_valid{false};
          const Identifier tube_id = idHelper.channelID(station_id,readEle->getMultilayer(), layer, tube, is_valid);
          if (!is_valid){
            ATH_MSG_VERBOSE("Invalid element");
            continue;
          }
          ++n_elements;
          /// Create the cabling object
          MdtCablingData cabling_data{};
          cabling->convert(tube_id,cabling_data);
          /// Test if the online channel can be found
          if (!cabling->getOnlineId(cabling_data,msgStream())){
             ATH_MSG_ERROR("Could no retrieve a valid online channel for "<<m_idHelperSvc->toString(tube_id));
             continue;
          }
          /// Test if the online channel can be transformed back
          
          /// Reset the offline cabling          
          const MdtCablingOffData off_data{};
          cabling_data.MdtCablingOffData::operator=(off_data);
          if (!cabling->getOfflineId(cabling_data, msgStream())){
            ATH_MSG_ERROR("Could not convert the online cabling "<<cabling_data<<" to an offline identifier. Initial identifier "<<m_idHelperSvc->toString(tube_id));
            continue;
          }
          
          Identifier test_id{};
          if (!cabling->convert(cabling_data, test_id, true)){
            ATH_MSG_ERROR("The extracted offline identifier "<<cabling_data<<" does not make sense");
            failure = true;
            continue;
          }
          if (test_id != tube_id){
            ATH_MSG_ERROR("The forward -> backward conversion failed. Started with "<<m_idHelperSvc->toString(tube_id)<<" ended with "<<m_idHelperSvc->toString(test_id));
            failure = true;
            continue;
          }
          
          /// Reset the offline cabling again
          cabling_data.MdtCablingOffData::operator=(off_data);
          
          /// Test whether the online module can be decoded successfully
          cabling_data.tdcId = 0xff;
          cabling_data.channelId =0xff;
          if (!cabling->getOfflineId(cabling_data, msgStream()) || !cabling->convert(cabling_data, test_id, true)) {
              ATH_MSG_ERROR("Failed to decode Mdt module");
              failure = true;
              continue;
          }
          MdtCablingData mrod_module{cabling_data};
          if (!cabling->getOnlineId(mrod_module,msgStream())){
             ATH_MSG_ERROR("Could no retrieve a valid online channel for "<<m_idHelperSvc->toString(tube_id));
             failure = true;
             continue;
          }
          /// There might be channels sitting on the same CSM but on differnt stations
          if (idHelper.elementID(test_id) != station_id && mrod_module.MdtCablingOnData::operator!=(cabling_data)) {
              ATH_MSG_ERROR("Failed to decode module "<<m_idHelperSvc->toString(tube_id)<<" got "<<m_idHelperSvc->toString(test_id) );
              failure = true;
          } else ++n_success;
       }
    }  
  }
  ATH_MSG_INFO( n_success<<" out of "<<n_elements<<" channels were successfully validated.");
  return failure ? StatusCode::FAILURE: StatusCode::SUCCESS;
} 
   
