/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "DerivationFrameworkBPhys/ReVertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"

#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"

using namespace DerivationFramework;

ReVertex::ReVertex(const std::string& t,
                   const std::string& n,
                   const IInterface* p) :
    AthAlgTool(t,n,p), m_vertexEstimator("InDet::VertexPointEstimator"), m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_massConst(0.),
    m_v0Tools("Trk::V0Tools"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_beamSpotSvc("BeamCondSvc",n)
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TrackIndices", m_TrackIndices);
    declareProperty("TrkVertexFitterTool", m_iVertexFitter);
    declareProperty("VertexPointEstimator",m_vertexEstimator);

    declareProperty("OutputVtxContainerName", m_OutputContainerName);
    declareProperty("InputVtxContainerName", m_inputContainerName);
    declareProperty("TrackContainerName", m_trackContainer = "InDetTrackParticles");

    declareProperty("V0Tools"               , m_v0Tools);
    declareProperty("PVRefitter"            , m_pvRefitter);
    declareProperty("PVContainerName"       , m_pvContainerName        = "PrimaryVertices");
    declareProperty("RefPVContainerName"    , m_refPVContainerName     = "RefittedPrimaryVertices");

    declareProperty("RefitPV"               , m_refitPV                = false);
    //This parameter will allow us to optimize the number of PVs under consideration as the probability
    //of a useful primary vertex drops significantly the higher you go
    declareProperty("MaxPVrefit"            , m_PV_max                = 1000);
    declareProperty("DoVertexType"          , m_DoVertexType           = 7);
    // minimum number of tracks for PV to be considered for PV association
    declareProperty("MinNTracksInPV"        , m_PV_minNTracks          = 0);
    declareProperty("Do3d"        , m_do3d          = false);
    declareProperty("AddPVData"        , m_AddPVData          = true);
    declareProperty("StartingPoint0"        , m_startingpoint0     = false);
    
    
}

StatusCode ReVertex::initialize() {
    ATH_MSG_DEBUG("in initialize()");
    if(m_TrackIndices.empty()) {
        ATH_MSG_FATAL("No Indices provided");
        return StatusCode::FAILURE;
    }
    m_VKVFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iVertexFitter));
    return StatusCode::SUCCESS;
}


StatusCode ReVertex::addBranches() const {

    xAOD::VertexContainer    *vtxContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer	 *vtxAuxContainer = new xAOD::VertexAuxContainer();
    vtxContainer->setStore(vtxAuxContainer);

    ATH_CHECK(evtStore()->record(vtxContainer, m_OutputContainerName));
    ATH_CHECK(evtStore()->record(vtxAuxContainer, m_OutputContainerName+"Aux."));

    const size_t Ntracks = m_TrackIndices.size();

    const xAOD::VertexContainer    *InVtxContainer = nullptr;
    ATH_CHECK(evtStore()->retrieve(InVtxContainer, m_inputContainerName ));

    const xAOD::TrackParticleContainer* importedTrackCollection = nullptr;
    ATH_CHECK(evtStore()->retrieve(importedTrackCollection, m_trackContainer ));

    std::vector<const xAOD::TrackParticle*> fitpair(Ntracks);
    for(const xAOD::Vertex* v : *InVtxContainer)
    {

        for(size_t i =0; i<Ntracks; i++)
        {
            size_t trackN = m_TrackIndices[i];
            if(trackN >= v->nTrackParticles())
            {
                ATH_MSG_FATAL("Indices exceeds limit in particle");
                return StatusCode::FAILURE;
            }
            fitpair[i] = v->trackParticle(trackN);
        }

        int sflag = 0;
        int errorcode = 0;
        Amg::Vector3D startingPoint;
        m_VKVFitter->setDefault();

        if (m_doMassConst)
        {
           m_VKVFitter->setMassInputParticles(m_MassConstraints);
           m_VKVFitter->setMassForConstraint(m_massConst, m_indices);
        }


        if (!m_startingpoint0) {
            const Trk::Perigee& aPerigee1 = fitpair[0]->perigeeParameters();
            const Trk::Perigee& aPerigee2 = fitpair[1]->perigeeParameters();
            startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1, &aPerigee2, sflag, errorcode);
        }
        if (m_startingpoint0 || errorcode!=0 )
        {
            startingPoint(0) = 0.0;
            startingPoint(1) = 0.0;
            startingPoint(2) = 0.0;
        }
        std::unique_ptr<xAOD::Vertex> ptr(m_VKVFitter->fit(fitpair, startingPoint));
        if(!ptr) continue;
        
        DerivationFramework::BPhysPVTools::PrepareVertexLinks( ptr.get(), importedTrackCollection );
        std::vector<const xAOD::Vertex*> thePreceding;
        thePreceding.push_back(v);
        xAOD::BPhysHelper bHelper(ptr.get());
        bHelper.setRefTrks();
        bHelper.setPrecedingVertices(thePreceding, InVtxContainer);
        vtxContainer->push_back(ptr.release());

    }

    if(m_AddPVData){
     // Give the helper class the ptr to v0tools and beamSpotsSvc to use
     BPhysPVTools helper(&(*m_v0Tools), &m_beamSpotSvc);
     helper.SetMinNTracksInPV(m_PV_minNTracks);
     helper.SetSave3d(m_do3d);

    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------
    const xAOD::VertexContainer* pvContainer = nullptr;
    CHECK(evtStore()->retrieve(pvContainer, m_pvContainerName));

    if(m_refitPV) {
        //----------------------------------------------------
        // Try to retrieve refitted primary vertices
        //----------------------------------------------------
        xAOD::VertexContainer*    refPvContainer    = nullptr;
        xAOD::VertexAuxContainer* refPvAuxContainer = nullptr;
        if(evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
          // refitted PV container exists. Get it from the store gate
          CHECK(evtStore()->retrieve(refPvContainer   , m_refPVContainerName       ));
          CHECK(evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName + "Aux."));
        } else {
          // refitted PV container does not exist. Create a new one.
          refPvContainer = new xAOD::VertexContainer;
          refPvAuxContainer = new xAOD::VertexAuxContainer;
          refPvContainer->setStore(refPvAuxContainer);
          CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
          CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
        }

        if(vtxContainer->size() >0){
          ATH_CHECK(helper.FillCandwithRefittedVertices(vtxContainer, pvContainer, refPvContainer, &(*m_pvRefitter) ,  m_PV_max, m_DoVertexType));
        }
     }else{
         if(vtxContainer->size() >0) ATH_CHECK(helper.FillCandExistingVertices(vtxContainer, pvContainer, m_DoVertexType));
     }
    }

    return StatusCode::SUCCESS;
}



