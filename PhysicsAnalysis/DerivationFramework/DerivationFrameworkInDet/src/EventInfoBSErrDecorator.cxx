/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EventInfoBSErrDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Daiki Hayakawa ( daiki.hayakawa@cern.ch )

#include "DerivationFrameworkInDet/EventInfoBSErrDecorator.h"
#include "DerivationFrameworkInDet/DecoratorUtils.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "InDetIdentifier/SCT_ID.h"

#include <vector>
#include <utility>
#include <string>

namespace DerivationFramework {

  EventInfoBSErrDecorator::EventInfoBSErrDecorator(const std::string& type,
      const std::string& name,
      const IInterface* parent) :
    AthAlgTool(type,name,parent)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode EventInfoBSErrDecorator::initialize()
  {

    if (m_prefix.empty()) {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of EventInfoBSErrDecorator!");
    }

    ATH_CHECK(m_eventInfoKey.initialize() );

    // need Atlas id-helpers to identify sub-detectors, take them from detStore
    if( detStore()->retrieve(m_sctId,"SCT_ID").isFailure() ){
      ATH_MSG_ERROR("Could not retrieve SCT helper");
      return StatusCode::FAILURE; 
    } 

    CHECK ( m_byteStreamErrTool.retrieve() );
    CHECK ( m_cabling.retrieve() );

    {
       std::vector<std::string> names;
       names.resize(kNIntDecor);
       names[kSCT_BSErr_Ntot]    ="SCT_BSErr_Ntot";
       names[kSCT_BSErr_bec]     ="SCT_BSErr_bec";
       names[kSCT_BSErr_layer]   ="SCT_BSErr_layer";
       names[kSCT_BSErr_eta]     ="SCT_BSErr_eta";
       names[kSCT_BSErr_phi]     ="SCT_BSErr_phi";
       names[kSCT_BSErr_side]    ="SCT_BSErr_side";
       names[kSCT_BSErr_rodid]   ="SCT_BSErr_rodid";
       names[kSCT_BSErr_channel] ="SCT_BSErr_channel";
       names[kSCT_BSErr_type]    ="SCT_BSErr_type";
       createDecoratorKeys(*this,m_eventInfoKey,m_prefix.value()+"_",names, m_intDecorKeys);
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

    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey,ctx);
    CHECK( eventInfo.isValid() ? StatusCode::SUCCESS : StatusCode::FAILURE );

    std::array<std::vector<int>,kNIntDecor> scterr;
    int totalNumErrors=0;

    // fill BS error information
    for (int n_type=0; n_type < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++n_type) {
      const std::set<IdentifierHash> errorSet = m_byteStreamErrTool->getErrorSet(n_type);
      int eta=0,phi=0,bec=0,layer=0,side=0;
      // add totalNumErrors to vector
      totalNumErrors += errorSet.size();
      scterr[kSCT_BSErr_Ntot].push_back(totalNumErrors);

      // loop in errorSet
      for(const auto& error : errorSet) {
        ATH_MSG_DEBUG("debugging information");
        Identifier itId = m_sctId->wafer_id(error);
        ATH_MSG_DEBUG(error);
        ATH_MSG_DEBUG(n_type);
        layer = m_sctId->layer_disk(itId);
        side = m_sctId->side(itId);
        eta = m_sctId->eta_module(itId);
        phi = m_sctId->phi_module(itId);
        bec = m_sctId->barrel_ec(itId);

        // add variables to vector
        scterr[kSCT_BSErr_bec].push_back(bec);
        scterr[kSCT_BSErr_layer].push_back(layer);
        scterr[kSCT_BSErr_eta].push_back(eta);
        scterr[kSCT_BSErr_phi].push_back(phi);
        scterr[kSCT_BSErr_side].push_back(side);
        scterr[kSCT_BSErr_type].push_back(n_type);

        uint32_t onlineID = m_cabling->getOnlineIdFromHash(error);
        SCT_OnlineId online(onlineID);
        uint32_t rod = online.rod();
        uint32_t fibre = online.fibre();
        scterr[kSCT_BSErr_rodid].push_back((int)rod);
        scterr[kSCT_BSErr_channel].push_back((int)fibre);
      }
    }
    std::vector<SG::WriteDecorHandle<xAOD::EventInfo,std::vector<int> > >   int_decor_handles(createDecorators<xAOD::EventInfo,std::vector<int> >(m_intDecorKeys,ctx));

    assert(int_decor_handles.size() == kNIntDecor);
    // decorate SCT BS error
    for(unsigned int decorate_i=0; decorate_i<int_decor_handles.size(); ++decorate_i) {
       int_decor_handles[decorate_i](*eventInfo) = std::move(scterr[decorate_i]);
    }
    return StatusCode::SUCCESS;
  }  
  
}
