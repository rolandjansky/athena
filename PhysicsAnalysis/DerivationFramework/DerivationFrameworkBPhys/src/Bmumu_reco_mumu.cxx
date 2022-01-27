/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Bmumu_reco_mumu.cxx
///////////////////////////////////////////////////////////////////
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Original author (Reco_mumu):
//          Daniel Scheirich <daniel.scheirich@cern.ch>
// 
// Changes:
// Basic dimuon reconstruction for the derivation framework.
// This class inherits from CfAthAlgTool instead of AthAlgTool in order
// to have access to the CutFlowSvc instance.
//
//============================================================================
//

#include "DerivationFrameworkBPhys/Bmumu_reco_mumu.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"


namespace DerivationFramework {

  Bmumu_reco_mumu::Bmumu_reco_mumu(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    CfAthAlgTool(t,n,p){
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Bmumu_reco_mumu::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
 
    // retrieve V0 tools
    CHECK( m_v0Tools.retrieve() );
    
    // get the JpsiFinder tool
    CHECK( m_jpsiFinder.retrieve() );
     
    // get the PrimaryVertexRefitter tool
    CHECK( m_pvRefitter.retrieve() );

    // Get the beam spot service
    CHECK( m_beamSpotKey.initialize() );
    ATH_CHECK(m_pvContainerKey.initialize());
    ATH_CHECK(m_refContainerKey.initialize(m_refitPV));
    ATH_CHECK(m_outVtxContainerKey.initialize());
    
    return StatusCode::SUCCESS;
    
  }
 
  StatusCode Bmumu_reco_mumu::addBranches() const {
    
    const EventContext& ctx = Gaudi::Hive::currentContext();
    // Jpsi container and its auxilliary store
    std::unique_ptr<xAOD::VertexContainer>  vtxContainer = std::make_unique<xAOD::VertexContainer>();
    std::unique_ptr<xAOD::VertexAuxContainer> vtxAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
    vtxContainer->setStore(vtxAuxContainer.get());
    
    //----------------------------------------------------
    // call Jpsi finder
    //----------------------------------------------------
    if( !m_jpsiFinder->performSearch(ctx, *vtxContainer).isSuccess() ) {
      ATH_MSG_FATAL("Jpsi finder (" << m_jpsiFinder << ") failed.");
      return StatusCode::FAILURE;
    }

    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------
    SG::ReadHandle<xAOD::VertexContainer> pvContainer{m_pvContainerKey,ctx};
    if (!pvContainer.isValid()){
        ATH_MSG_FATAL("Failed to retrieve "<<m_pvContainerKey);
        return StatusCode::FAILURE;
    }
    
    
    //----------------------------------------------------
    // Try to retrieve refitted primary vertices
    //----------------------------------------------------
    std::unique_ptr<xAOD::VertexContainer>    refPvContainer = std::make_unique<xAOD::VertexContainer>();
    std::unique_ptr<xAOD::VertexAuxContainer> refPvAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
    refPvContainer->setStore(refPvAuxContainer.get());
    
    //m_refContainerKey
    
    // Give the helper class the ptr to v0tools and beamSpotsSvc to use
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    if(!beamSpotHandle.isValid()) {
        ATH_MSG_ERROR("Cannot Retrieve " << m_beamSpotKey.key() );
        return StatusCode::FAILURE;
    }
    BPhysPVTools helper(&(*m_v0Tools), beamSpotHandle.cptr());
    helper.SetMinNTracksInPV(m_PV_minNTracks);
    helper.SetSave3d(m_do3d);

    if(m_refitPV && vtxContainer->size()){
        ATH_CHECK(helper.FillCandwithRefittedVertices(vtxContainer.get(),  pvContainer.cptr(), refPvContainer.get(), &(*m_pvRefitter) , m_PV_max, m_DoVertexType));
    }else if (!m_refitPV && vtxContainer->size() >0){
        ATH_CHECK(helper.FillCandExistingVertices(vtxContainer.get(), pvContainer.cptr(), m_DoVertexType));
    }
    
    
    //----------------------------------------------------
    // save in the StoreGate
    //----------------------------------------------------
    SG::WriteHandle<xAOD::VertexContainer> outputVertices{m_outVtxContainerKey, ctx};
    ATH_CHECK(outputVertices.record(std::move(vtxContainer), std::move(vtxAuxContainer)));
    
    if(m_refitPV) {
      SG::WriteHandle<xAOD::VertexContainer> refitVertices{m_refContainerKey,ctx};
      ATH_CHECK(refitVertices.record(std::move(refPvContainer), std::move(refPvAuxContainer)));
    }

    // add counter for number of events seen
    addEvent("dimuEvents");
    // add counter for the number of events with >= 1 reco'd vertices
    if ( vtxContainer->size() > 0 ) {
      addEvent("dimuWithVertexCand");
    }
    // add counter for the number of vertices
    addToCounter("dimuNumVertices", vtxContainer->size());
    
    return StatusCode::SUCCESS;
  }  
}
