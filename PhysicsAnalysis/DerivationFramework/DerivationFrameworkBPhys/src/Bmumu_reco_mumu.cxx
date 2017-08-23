/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"


namespace DerivationFramework {

  Bmumu_reco_mumu::Bmumu_reco_mumu(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    CfAthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools"),
    m_jpsiFinder("Analysis::JpsiFinder"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_beamSpotSvc("BeamCondSvc",n)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare tools    
    declareProperty("V0Tools"   , m_v0Tools);
    declareProperty("JpsiFinder", m_jpsiFinder);
    declareProperty("PVRefitter", m_pvRefitter);
    
    // Declare user-defined properties
    declareProperty("OutputVtxContainerName", m_outputVtxContainerName = "OniaCandidates");
    declareProperty("PVContainerName"       , m_pvContainerName        = "PrimaryVertices");
    declareProperty("RefPVContainerName"    , m_refPVContainerName     = "RefittedPrimaryVertices");
    declareProperty("RefitPV"               , m_refitPV                = false);
    declareProperty("MaxPVrefit"            , m_PV_max                 = 1);
    declareProperty("DoVertexType"          , m_DoVertexType           = 1);
    // minimum number of tracks for PV to be considered for PV association
    declareProperty("MinNTracksInPV"        , m_PV_minNTracks          = 0);
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
    CHECK( m_beamSpotSvc.retrieve() );
    
    return StatusCode::SUCCESS;
    
  }
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  StatusCode Bmumu_reco_mumu::finalize()
  {
    // everything all right
    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Bmumu_reco_mumu::addBranches() const
  {
    // Jpsi container and its auxilliary store
    xAOD::VertexContainer*    vtxContainer = NULL;
    xAOD::VertexAuxContainer* vtxAuxContainer = NULL;
    
    //----------------------------------------------------
    // call Jpsi finder
    //----------------------------------------------------
    if( !m_jpsiFinder->performSearch(vtxContainer, vtxAuxContainer).isSuccess() ) {
      ATH_MSG_FATAL("Jpsi finder (" << m_jpsiFinder << ") failed.");
      return StatusCode::FAILURE;
    }

    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------
    const xAOD::VertexContainer*    pvContainer = NULL;
    CHECK( evtStore()->retrieve(pvContainer, m_pvContainerName) );

    //----------------------------------------------------
    // Try to retrieve refitted primary vertices
    //----------------------------------------------------
    bool refPvExists = false;
    xAOD::VertexContainer*    refPvContainer = NULL;
    xAOD::VertexAuxContainer* refPvAuxContainer = NULL;
    if(m_refitPV) {
      if(evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
        // refitted PV container exists. Get it from the store gate
        CHECK( evtStore()->retrieve(refPvContainer, m_refPVContainerName) );
        CHECK( evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName+"Aux.") );
        refPvExists = true;
      } else {
        // refitted PV container does not exist. Create a new one.
        refPvContainer = new xAOD::VertexContainer;
        refPvAuxContainer = new xAOD::VertexAuxContainer;
        refPvContainer->setStore(refPvAuxContainer);
      }
    }
    
    // Give the helper class the ptr to v0tools and beamSpotsSvc to use
    BPhysPVTools helper(&(*m_v0Tools), &m_beamSpotSvc);
    helper.SetMinNTracksInPV(m_PV_minNTracks);

    if(m_refitPV){ 
       if(vtxContainer->size() >0){
        StatusCode SC = helper.FillCandwithRefittedVertices(vtxContainer,  pvContainer, refPvContainer, &(*m_pvRefitter) , m_PV_max, m_DoVertexType);
        if(SC.isFailure()){
            ATH_MSG_FATAL("refitting failed - check the vertices you passed");
            return SC;
        }
        }
    }else{
        refPvContainer = const_cast<xAOD::VertexContainer*>(pvContainer);
        if(vtxContainer->size() >0)CHECK(helper.FillCandExistingVertices(vtxContainer, refPvContainer, m_DoVertexType));
    }
    
    
    //----------------------------------------------------
    // save in the StoreGate
    //----------------------------------------------------
    if (!evtStore()->contains<xAOD::VertexContainer>(m_outputVtxContainerName))       
      CHECK(evtStore()->record(vtxContainer, m_outputVtxContainerName));
    
    if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_outputVtxContainerName+"Aux.")) 
      CHECK(evtStore()->record(vtxAuxContainer, m_outputVtxContainerName+"Aux."));
    
    if(!refPvExists && m_refitPV) {
      CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
      CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
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
