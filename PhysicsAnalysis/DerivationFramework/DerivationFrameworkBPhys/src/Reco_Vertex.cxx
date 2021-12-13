/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Reco_Vertex.cxx
///////////////////////////////////////////////////////////////////
// Author: Daniel Scheirich <daniel.scheirich@cern.ch>
// Based on the Integrated Simulation Framework
//
// Basic Jpsi->mu mu derivation example

#include "DerivationFrameworkBPhys/Reco_Vertex.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

namespace DerivationFramework {

  Reco_Vertex::Reco_Vertex(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools"),
    m_SearchTool(),
    m_pvRefitter("Analysis::PrimaryVertexRefitter")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare tools    
    declareProperty("V0Tools"   , m_v0Tools);
    declareProperty("VertexSearchTool", m_SearchTool);
    declareProperty("PVRefitter", m_pvRefitter);
    
    // Declare user-defined properties
    declareProperty("OutputVtxContainerName", m_outputVtxContainerName = "OniaCandidates");
    declareProperty("PVContainerName"       , m_pvContainerName        = "PrimaryVertices");
    declareProperty("RefPVContainerName"    , m_refPVContainerName     = "RefittedPrimaryVertices");
    declareProperty("RefitPV"               , m_refitPV                = false);
    declareProperty("MaxPVrefit"            , m_PV_max                 = 1000);
    declareProperty("DoVertexType"          , m_DoVertexType           = 7);
    // minimum number of tracks for PV to be considered for PV association
    declareProperty("MinNTracksInPV"        , m_PV_minNTracks          = 0);
    declareProperty("Do3d"                  , m_do3d                   = false);
    declareProperty("CheckCollections"      , m_checkCollections       = false);
    declareProperty("CheckVertexContainers" , m_CollectionsToCheck);
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Reco_Vertex::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
 
    // retrieve V0 tools
    CHECK( m_v0Tools.retrieve() );
    
    // get the Search tool
    CHECK( m_SearchTool.retrieve() );
     
    // get the PrimaryVertexRefitter tool
    CHECK( m_pvRefitter.retrieve() );

    // Get the beam spot service
    ATH_CHECK(m_beamSpotKey.initialize());


    ATH_CHECK(m_outputVtxContainerName.initialize());
    ATH_CHECK(m_pvContainerName.initialize());
    ATH_CHECK(m_refPVContainerName.initialize());
    if(m_checkCollections) ATH_CHECK(m_CollectionsToCheck.initialize());
    return StatusCode::SUCCESS;
    
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Reco_Vertex::addBranches() const
  {
    bool callTool = true;
    const EventContext& ctx = Gaudi::Hive::currentContext();
    if(m_checkCollections) {
      for(const auto &str : m_CollectionsToCheck){
         SG::ReadHandle<xAOD::VertexContainer> handle(str,ctx);
         ATH_CHECK(handle.isValid());
         if(handle->size() == 0) {
            callTool = false;
            ATH_MSG_DEBUG("Container VertexContainer (" << str << ") is empty");
            break;//No point checking other containers
         }
      }
    }

    // Vertex container and its auxilliary store
    std::unique_ptr<xAOD::VertexContainer>    vtxContainer = std::make_unique<xAOD::VertexContainer>();
    std::unique_ptr<xAOD::VertexAuxContainer> vtxAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
    vtxContainer->setStore(vtxAuxContainer.get());
   
    if(callTool) {
    //----------------------------------------------------
    // call Tool
    //----------------------------------------------------
    if( !m_SearchTool->performSearch(ctx,*vtxContainer).isSuccess() ) {
      ATH_MSG_FATAL("Tool (" << m_SearchTool << ") failed.");
      return StatusCode::FAILURE;
    }
 
    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------
    SG::ReadHandle<xAOD::VertexContainer> pvContainer(m_pvContainerName,ctx);

    //----------------------------------------------------
    // Try to retrieve refitted primary vertices
    //----------------------------------------------------
    std::unique_ptr<xAOD::VertexContainer>    refPvContainer;
    std::unique_ptr<xAOD::VertexAuxContainer> refPvAuxContainer;
    if(m_refitPV) {
        // refitted PV container does not exist. Create a new one.
        refPvContainer = std::make_unique<xAOD::VertexContainer>();
        refPvAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
        refPvContainer->setStore(refPvAuxContainer.get());
    }
    
    // Give the helper class the ptr to v0tools and beamSpotsSvc to use
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    if(not beamSpotHandle.isValid()) ATH_MSG_ERROR("Cannot Retrieve " << m_beamSpotKey.key() );
    BPhysPVTools helper(&(*m_v0Tools), beamSpotHandle.cptr());
    helper.SetMinNTracksInPV(m_PV_minNTracks);
    helper.SetSave3d(m_do3d);

    if(m_refitPV){ 
       if(vtxContainer->size() >0){
         StatusCode SC = helper.FillCandwithRefittedVertices(vtxContainer.get(),  pvContainer.cptr(), refPvContainer.get(), &(*m_pvRefitter) , m_PV_max, m_DoVertexType);
        if(SC.isFailure()){
            ATH_MSG_FATAL("refitting failed - check the vertices you passed");
            return SC;
        }
        }
    }else{
        if(vtxContainer->size() >0)CHECK(helper.FillCandExistingVertices(vtxContainer.get(), pvContainer.cptr(), m_DoVertexType));
    }
    
    //----------------------------------------------------
    // save in the StoreGate
    //----------------------------------------------------
    SG::WriteHandle<xAOD::VertexContainer> handle(m_outputVtxContainerName);
    ATH_CHECK(handle.record(std::move(vtxContainer), std::move(vtxAuxContainer)));
    
    if(m_refitPV) {
       SG::WriteHandle<xAOD::VertexContainer> handle(m_refPVContainerName);
       ATH_CHECK(handle.record(std::move(refPvContainer), std::move(refPvAuxContainer)));
    }
    }

    if (!callTool) { //Fill with empty containers
      SG::WriteHandle<xAOD::VertexContainer> handle(m_outputVtxContainerName);
      ATH_CHECK(handle.record(std::unique_ptr<xAOD::VertexContainer>(new xAOD::VertexContainer ),
          std::unique_ptr<xAOD::VertexAuxContainer>(new xAOD::VertexAuxContainer )));
    }
    
    return StatusCode::SUCCESS;
  }  
}
