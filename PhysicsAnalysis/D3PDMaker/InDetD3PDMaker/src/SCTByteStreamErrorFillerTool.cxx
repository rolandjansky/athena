/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTByteStreamErrorFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include <string>

namespace D3PD {


/**
  * @brief Standard Gaudi tool constructor.
  * @param type The name of the tool type.
  * @param name The tool name.
  * @param parent The tool's Gaudi parent.
  */
SCTByteStreamErrorFillerTool::SCTByteStreamErrorFillerTool
   (const std::string& type,
    const std::string& name,
    const IInterface* parent)
     : BlockFillerTool<xAOD::EventInfo> (type, name, parent),
       m_sctid(0)
 {
   book().ignore(); // Avoid coverity warnings.
 }
 
  StatusCode SCTByteStreamErrorFillerTool::initialize(){

    if (detStore()->retrieve(m_sctid, "SCT_ID").isFailure()) {
      REPORT_MESSAGE (MSG::ERROR) <<"Could not get SCT ID helper";
      return StatusCode::FAILURE;
    }



    if (m_byteStreamErrTool.retrieve().isFailure()) {
      REPORT_MESSAGE (MSG::ERROR) << "Failed to retrieve tool " << m_byteStreamErrTool;
      return StatusCode::FAILURE;
    } else {
      REPORT_MESSAGE (MSG::INFO) << "Retrieved service " << m_byteStreamErrTool;
    }
    if (m_cabling.retrieve().isFailure()) {
      REPORT_MESSAGE (MSG::ERROR) << "Failed to retrieve service " << m_cabling;
      return StatusCode::FAILURE;
    } else {
      REPORT_MESSAGE (MSG::INFO) << "Retrieved service " << m_cabling;
    }



    return StatusCode::SUCCESS;

  }
 
 /**
  * @brief Book variables for this block.
  */
 StatusCode SCTByteStreamErrorFillerTool::book()
 {
   
     CHECK( addVariable ("sctBSerr_Ntot", m_totalNumErrors) );
     CHECK( addVariable ("sctBSerr_bec", m_scterr_bec) ) ;
     CHECK( addVariable ("sctBSerr_layer", m_scterr_layer) );
     CHECK( addVariable ("sctBSerr_eta", m_scterr_eta) );
     CHECK( addVariable ("sctBSerr_phi", m_scterr_phi) );
     CHECK( addVariable ("sctBSerr_side", m_scterr_side) );
     CHECK( addVariable ("sctBSerr_rodid", m_scterr_rodid) );
     CHECK( addVariable ("sctBSerr_channel", m_scterr_channel) );
     CHECK( addVariable ("sctBSerr_type", m_scterr_type) );

     return StatusCode::SUCCESS;
 }
 
 
StatusCode SCTByteStreamErrorFillerTool::fill (const xAOD::EventInfo& /*p*/)
 {
   *m_totalNumErrors = 0;
   for (int type=0; type < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++type) {
     const std::set<IdentifierHash> errorSet = m_byteStreamErrTool->getErrorSet(type);
       int eta=0,phi=0,bec=0,layer=0,side=0;
       *m_totalNumErrors += errorSet.size();
       for(const auto& idHash : errorSet) {
		 Identifier itId = m_sctid->wafer_id(idHash);
		 layer = m_sctid->layer_disk(itId);
		 side = m_sctid->side(itId);
		 eta = m_sctid->eta_module(itId);
		 phi = m_sctid->phi_module(itId);
		 bec = m_sctid->barrel_ec(itId);
		 m_scterr_type->push_back(type);
		 m_scterr_bec->push_back(bec);
		 m_scterr_layer->push_back(layer);
		 m_scterr_eta->push_back(eta);
		 m_scterr_phi->push_back(phi);
		 m_scterr_side->push_back(side);
	 
		 uint32_t onlineID = m_cabling->getOnlineIdFromHash(idHash);
		 SCT_OnlineId online(onlineID);
		 uint32_t rod = online.rod();
		 uint32_t fibre = online.fibre();
		 m_scterr_rodid->push_back((int)rod);
		 m_scterr_channel->push_back((int)fibre);
       }
     
   }

  
   return StatusCode::SUCCESS;
 }
 
 
 } // namespace D3PD
