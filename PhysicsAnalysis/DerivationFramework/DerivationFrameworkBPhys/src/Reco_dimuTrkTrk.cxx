/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////
// Reco_dimuTrkTrk.cxx
///////////////////////////////////////////////////////////////////
// Author: James Catmore <james,catmore@cern.ch>
// Author: Adam Barton <abarton@SPAMMENOTTtttcern.ch>
#include "DerivationFrameworkBPhys/Reco_dimuTrkTrk.h"


#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"


namespace DerivationFramework {

  Reco_dimuTrkTrk::Reco_dimuTrkTrk(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools"),
    m_jpsi2Track("Analysis::JpsiPlus2Tracks"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_beamSpotSvc("BeamCondSvc",n),
    m_outputVtxContainerName("Jpsi2TrackCandidates")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare user-defined properties
    declareProperty("V0Tools"               , m_v0Tools);
    declareProperty("Jpsi2PlusTrackName"    , m_jpsi2Track);
    declareProperty("PVRefitter"            , m_pvRefitter);
    declareProperty("OutputVtxContainerName", m_outputVtxContainerName);
    declareProperty("PVContainerName"       , m_pvContainerName        = "PrimaryVertices");
    declareProperty("RefPVContainerName"    , m_refPVContainerName     = "RefittedPrimaryVertices");
    declareProperty("RefitPV"               , m_refitPV                = false);
    //This parameter will allow us to optimize the number of PVs under consideration as the probability
    //of a useful primary vertex drops significantly the higher you go
    declareProperty("MaxPVrefit"            , m_PV_max                = 1000);
    declareProperty("DoVertexType"          , m_DoVertexType           = 7);
    // minimum number of tracks for PV to be considered for PV association
    declareProperty("MinNTracksInPV"        , m_PV_minNTracks          = 0);
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Reco_dimuTrkTrk::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
  
    // get the JpsiPlus2Track tool
    if ( m_jpsi2Track.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_jpsi2Track);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Retrieved tool " << m_jpsi2Track);
    }
    // Get the beam spot service
    CHECK( m_beamSpotSvc.retrieve() );
    
    return StatusCode::SUCCESS;
    
  }
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  StatusCode Reco_dimuTrkTrk::finalize()
  {
    // everything all right
    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Reco_dimuTrkTrk::addBranches() const
  {

//    ATH_MSG_INFO("in Reco_dimuTrkTrk::addBranches");
    // Jpsi container and its auxilliary store
    xAOD::VertexContainer*    jpsi2TrackContainer = NULL;
    xAOD::VertexAuxContainer* jpsi2TrackAuxContainer = NULL;
    
    // call Jpsi+2track tool
    if( !m_jpsi2Track->performSearch(jpsi2TrackContainer, jpsi2TrackAuxContainer).isSuccess() ) {
      ATH_MSG_FATAL("Jpsi+2 track tool (" << m_jpsi2Track << ") failed.");
      return StatusCode::FAILURE;
    }
    if(jpsi2TrackContainer->size() >0){
      ATH_MSG_DEBUG("jpsi2TrackContainer size " << jpsi2TrackContainer->size()); 
    }
    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------
    const xAOD::VertexContainer* pvContainer = NULL;
    CHECK(evtStore()->retrieve(pvContainer, m_pvContainerName));
    
    //----------------------------------------------------
    // Try to retrieve refitted primary vertices
    //----------------------------------------------------
    bool refPvExists = false;
    xAOD::VertexContainer*    refPvContainer    = NULL;
    xAOD::VertexAuxContainer* refPvAuxContainer = NULL;
    if(m_refitPV) {
      if(evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
        // refitted PV container exists. Get it from the store gate
        CHECK(evtStore()->retrieve(refPvContainer   , m_refPVContainerName       ));
        CHECK(evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName + "Aux."));
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
       if(jpsi2TrackContainer->size() >0){
        StatusCode SC = helper.FillCandwithRefittedVertices(jpsi2TrackContainer, pvContainer, refPvContainer, &(*m_pvRefitter) , m_PV_max, m_DoVertexType);
        if(SC.isFailure()){
            ATH_MSG_FATAL("refitting failed - check the vertices you passed");
            return SC;
        }
        }
    }else{
        refPvContainer = const_cast<xAOD::VertexContainer*>(pvContainer);
        if(jpsi2TrackContainer->size() >0) CHECK(helper.FillCandExistingVertices(jpsi2TrackContainer, refPvContainer, m_DoVertexType));
    }
    
    // save in the StoreGate
    if (!evtStore()->contains<xAOD::VertexContainer>(m_outputVtxContainerName))       
      CHECK(evtStore()->record(jpsi2TrackContainer, m_outputVtxContainerName));
    
    if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_outputVtxContainerName+"Aux.")) 
      CHECK(evtStore()->record(jpsi2TrackAuxContainer, m_outputVtxContainerName+"Aux."));

    if(!refPvExists && m_refitPV) {
      CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
      CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
    }
    
 
    return StatusCode::SUCCESS;    
  }
}




