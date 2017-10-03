/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EventInfoBSErrDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Daiki Hayakawa ( daiki.hayakawa@cern.ch )

#include "DerivationFrameworkInDet/EventInfoBSErrDecorator.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "InDetIdentifier/SCT_ID.h"

#include <vector>
#include <utility>
#include <string>

namespace DerivationFramework {

  EventInfoBSErrDecorator::EventInfoBSErrDecorator(const std::string& type,
      const std::string& name,
      const IInterface* parent) : 
    AthAlgTool(type,name,parent),  
    m_sgName(""),
    m_containerName(""),
    m_sctId(0),  
    m_byteStreamErrSvc("SCT_ByteStreamErrorsSvc",name),
    m_cabling("SCT_CablingSvc",name)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("DecorationPrefix",       m_sgName);
    declareProperty("ContainerName",          m_containerName="EventInfo");
  }

  StatusCode EventInfoBSErrDecorator::initialize()
  {

    if (m_sgName=="") {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of EventInfoBSErrDecorator!");
    }
    
    if (m_containerName=="") {
      ATH_MSG_ERROR("No TrackParticle collection provided for EventInfoBSErrDecorator!");
      return StatusCode::FAILURE;
    }

    // need Atlas id-helpers to identify sub-detectors, take them from detStore
    if( detStore()->retrieve(m_sctId,"SCT_ID").isFailure() ){
      ATH_MSG_ERROR("Could not retrieve SCT helper");
      return StatusCode::FAILURE; 
    } 

    return StatusCode::SUCCESS;
  }

  StatusCode EventInfoBSErrDecorator::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode EventInfoBSErrDecorator::addBranches() const
  {
    ATH_MSG_DEBUG("Adding ByteStream errors to EventInfo");

    const xAOD::EventInfo* eventInfo;
    CHECK( evtStore()->retrieve( eventInfo, m_containerName ) );

    CHECK ( m_byteStreamErrSvc.retrieve() );

    std::vector<int> scterr_Ntot;
    std::vector<int> scterr_bec;
    std::vector<int> scterr_layer;
    std::vector<int> scterr_eta;
    std::vector<int> scterr_phi;
    std::vector<int> scterr_side;
    std::vector<int> scterr_rodid;
    std::vector<int> scterr_channel;
    std::vector<int> scterr_type;
    int totalNumErrors=0;

    // fill BS error information
    for (int n_type=0; n_type < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++n_type) {
      std::set<IdentifierHash>* errorSet = m_byteStreamErrSvc->getErrorSet(n_type);
      if (errorSet != 0) {
        int eta=0,phi=0,bec=0,layer=0,side=0;
        std::set<IdentifierHash>::iterator it = errorSet->begin();
        std::set<IdentifierHash>::iterator itEnd = errorSet->end();
        // add totalNumErrors to vector
        totalNumErrors += errorSet->size();
        scterr_Ntot.push_back(totalNumErrors);

        // loop in errorSet
        for (; it != itEnd; ++it) {
          ATH_MSG_DEBUG("debugging information");
          Identifier itId = m_sctId->wafer_id(*it);
          ATH_MSG_DEBUG(*it);
          ATH_MSG_DEBUG(n_type);
          layer = m_sctId->layer_disk(itId);
          side = m_sctId->side(itId);
          eta = m_sctId->eta_module(itId);
          phi = m_sctId->phi_module(itId);
          bec = m_sctId->barrel_ec(itId);

          // add variables to vector
          scterr_bec.push_back(bec);
          scterr_layer.push_back(layer);
          scterr_eta.push_back(eta);
          scterr_phi.push_back(phi);
          scterr_side.push_back(side);
          scterr_type.push_back(n_type);

          uint32_t onlineID = m_cabling->getOnlineIdFromHash(*it);
          SCT_OnlineId online(onlineID);
          uint32_t rod = online.rod();
          uint32_t fibre = online.fibre();
          scterr_rodid.push_back((int)rod);
          scterr_channel.push_back((int)fibre);
        }
      }
    }

    // decorate SCT BS error
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_Ntot")    = scterr_Ntot;
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_bec")     = scterr_bec;
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_layer")   = scterr_layer;
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_eta")     = scterr_eta;
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_phi")     = scterr_phi;
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_side")    = scterr_side;
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_rodid")   = scterr_rodid;
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_channel") = scterr_channel;
    eventInfo->auxdecor< std::vector<int> >(m_sgName+"_SCT_BSErr_type")    = scterr_type;
    
    return StatusCode::SUCCESS;
  }  
  
}
