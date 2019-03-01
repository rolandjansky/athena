/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTest/LArHVTest.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/IChronoStatSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "LArIdentifier/LArElectrodeID.h"

#include <cmath>
#include <bitset>


LArHVTest::LArHVTest( const std::string & name, ISvcLocator * pSvcLocator) 
  : 
    AthAlgorithm(name, pSvcLocator),
    m_lar_on_id(NULL), 	    
    m_calocell_id(NULL),	
    m_larem_id(NULL),
    m_larhec_id(NULL),
    m_larfcal_id(NULL),	
    m_electrodeID(NULL),
    m_hvtool("LArHVToolDB")
{

  declareProperty("HVTool",m_hvtool,"HV tool used");

}


StatusCode LArHVTest::initialize() {
  ATH_MSG_DEBUG("LArHVTest initialize() begin");
  
  StatusCode sc = detStore()->retrieve(m_lar_on_id,"LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve  LArOnlineID from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }

  sc=detStore()->retrieve(m_calocell_id,"CaloCell_ID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve  CaloCellID from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }

  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );

  m_larem_id   = idHelper->em_idHelper();
  m_larhec_id   = idHelper->hec_idHelper();
  m_larfcal_id   = idHelper->fcal_idHelper();

  ATH_CHECK( detStore()->retrieve(m_electrodeID) );
  ATH_CHECK( m_hvtool.retrieve() );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_hvmapKey.initialize()) ;
  ATH_CHECK( m_hvdataKey.initialize() );

  ATH_MSG_DEBUG("LArHVTest initialize() end");
  return StatusCode::SUCCESS;
}


StatusCode LArHVTest::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode LArHVTest::execute()
{
  const LArOnOffIdMapping *cabling = nullptr;
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl(m_cablingKey);
  cabling = *cablingHdl;
  EventIDRange rangeMapping;
  if (cabling == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArOnOffIdMapping object");
  } else if (!cablingHdl.range(rangeMapping)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for LArOnOffIdMapping object with " <<  cablingHdl.key());
        return StatusCode::FAILURE;
  }

  const LArHVIdMapping *hvmap = nullptr;
  SG::ReadCondHandle<LArHVIdMapping> hvmapHdl(m_hvmapKey);
  hvmap = *hvmapHdl;
  if (hvmap == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArHVIdMapping object");
  } else if (!hvmapHdl.range(rangeMapping)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for LArHVIdMapping object with " <<  hvmapHdl.key());
        return StatusCode::FAILURE;
  }

  const LArHVData *hvdata = nullptr;
  SG::ReadCondHandle<LArHVData> hvdataHdl(m_hvdataKey);
  hvdata = *hvdataHdl;
  if (hvdata == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArHVData object");
  } else if (!hvdataHdl.range(rangeMapping)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for LArHVData object with " <<  hvdataHdl.key());
        return StatusCode::FAILURE;
  }
 
  std::vector<LArHVData::HV_t> vdata;
  std::vector<LArHVData::CURRENT_t> ihvdata;

  std::vector<ILArHVTool::HV_t> v;
  std::vector<ILArHVTool::CURRENT_t> ihv;
  // loop over all EM Identifiers
  for (auto id: m_larem_id->channel_ids()) {
     ATH_CHECK(m_hvtool->getHV(id,v));
     ATH_CHECK(hvdata->getHV(id,vdata));
     ATH_MSG_DEBUG("Id: "<<id<<"v size: "<<v.size()<<" vdata size "<<vdata.size());
     if(v.size() == vdata.size()) {
       for(unsigned int i=0; i<v.size(); ++i) {
         ATH_MSG_DEBUG("i: "<<i<<" v voltage: "<<v[i].hv<<":"<<v[i].weight<<" vdata voltage: "<<vdata[i].hv<<":"<<vdata[i].weight);
         if(fabs(v[i].hv-vdata[i].hv)>0.01 || fabs(v[i].weight-vdata[i].weight)>0.0001) {
            ATH_MSG_ERROR("Why different voltage/weights ???");
         }
       }
     }
  }      
  // loop over all HEC Identifiers
  for (auto id: m_larhec_id->channel_ids()) {
     ATH_CHECK(m_hvtool->getHV(id,v));
     ATH_CHECK(hvdata->getHV(id,vdata));
     ATH_MSG_DEBUG("Id: "<<id<<"v size: "<<v.size()<<" vdata size "<<vdata.size());
     if(v.size() == vdata.size()) {
       for(unsigned int i=0; i<v.size(); ++i) {
         ATH_MSG_DEBUG("i: "<<i<<" v voltage: "<<v[i].hv<<":"<<v[i].weight<<" vdata voltage: "<<vdata[i].hv<<":"<<vdata[i].weight);
         if(fabs(v[i].hv-vdata[i].hv)>0.01 || fabs(v[i].weight-vdata[i].weight)>0.0001) {
            ATH_MSG_ERROR("Why different voltage/weights ???");
         }
       }
     }
  }      
  // loop over all FCAL Identifiers
  for (auto id: m_larfcal_id->channel_ids()) {
     ATH_CHECK(m_hvtool->getHV(id,v));
     ATH_CHECK(hvdata->getHV(id,vdata));
     ATH_MSG_DEBUG("Id: "<<id<<"v size: "<<v.size()<<" vdata size "<<vdata.size());
     if(v.size() == vdata.size()) {
       for(unsigned int i=0; i<v.size(); ++i) {
         ATH_MSG_DEBUG("i: "<<i<<" v voltage: "<<v[i].hv<<":"<<v[i].weight<<" vdata voltage: "<<vdata[i].hv<<":"<<vdata[i].weight);
         if(fabs(v[i].hv-vdata[i].hv)>0.01 || fabs(v[i].weight-vdata[i].weight)>0.0001) {
            ATH_MSG_ERROR("Why different voltage/weights ???");
         }
       }
     }
  }      


  return StatusCode::SUCCESS;
}


