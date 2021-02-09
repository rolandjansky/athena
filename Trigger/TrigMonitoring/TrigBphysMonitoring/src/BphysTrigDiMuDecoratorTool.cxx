///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BphysTrigDiMuDecoratorTool.cxx 
// Implementation file for class BphysTrigDiMuDecoratorTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// TrigBphysMonitoring includes
#include "BphysTrigDiMuDecoratorTool.h"

// STL includes
#include <memory>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "CxxUtils/make_unique.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "xAODTracking/TrackingPrimitives.h"

#include "xAODEventInfo/EventInfo.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
BphysTrigDiMuDecoratorTool::BphysTrigDiMuDecoratorTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   )
,m_V0Tools("Trk::V0Tools")
,m_trackMass(105.6583715)
{
    //declareInterface< BphysTrigDiMuDecoratorTool >(this);
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
    declareProperty("V0Tools",m_V0Tools);

    declareProperty( "TrackMass", m_trackMass );
    declareProperty( "PVContainerName",   m_pvCandidatesKey   = "PrimaryVertices" );

    
    declareInterface<IBphysTrigDiMuDecoratorTool>(this);
}

// Destructor
///////////////
BphysTrigDiMuDecoratorTool::~BphysTrigDiMuDecoratorTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode BphysTrigDiMuDecoratorTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

    if ( m_V0Tools.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_V0Tools);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Retrieved tool " << m_V0Tools);
    }
    
  return StatusCode::SUCCESS;
}

StatusCode BphysTrigDiMuDecoratorTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode BphysTrigDiMuDecoratorTool::decorateVertex(const xAOD::Vertex* vtx,
                                                      const xAOD::Vertex * pv) const {
    if (!vtx) {
        ATH_MSG_WARNING ("Input Vertex was null");
        return StatusCode::FAILURE;
    }
    
    
    // retrieve the jpsi container and add some aux info
    //const xAOD::VertexContainer* *pvContainer(nullptr);
    //ATH_CHECK( evtStore()->retrieve(pvContainer   ,m_pvCandidatesKey  ));
    //ATH_CHECK( evtStore()->retrieve(jpsiContainer ,m_JpsiCandidatesKey));


    const xAOD::VertexContainer* pvContainer(0);
    // check if PV container - ie. if cosmics // https://its.cern.ch/jira/browse/ATR-10633
    if (evtStore()->retrieve(pvContainer   ,m_pvCandidatesKey  ).isFailure() || !pvContainer) {
        ATH_MSG_DEBUG("No PrimaryVertex container of name: " << m_pvCandidatesKey);
    } // no PV
    
    //vtx->auxdecor<unsigned int>("TestVar") = 10;
    
    const xAOD::Vertex * vtxbs(nullptr), *vtxpv(nullptr);
    if (pvContainer) {
        for (auto primvtx: *pvContainer) {
            if ( primvtx->vertexType() == xAOD::VxType::PriVtx)
                vtxpv = primvtx;
        } // vtx
    } // if pv container is found
    
    const xAOD::EventInfo *evtInfo(nullptr);
    if ( evtStore()->retrieve(evtInfo).isFailure() || !evtInfo) {
        ATH_MSG_DEBUG("No EventInfo found; dummy BS position used");
    } else {
        ATH_MSG_VERBOSE("Beamspot: " << evtInfo->beamPosX() << " "
                      << evtInfo->beamPosY() << " " << evtInfo->beamPosZ() << " / "
                      << evtInfo->beamPosSigmaX() << " " << evtInfo->beamPosSigmaY() << " "
                      << evtInfo->beamPosSigmaZ() );
    }
    std::unique_ptr<xAOD::Vertex> bsVertex = CxxUtils::make_unique<xAOD::Vertex>();
    bsVertex->makePrivateStore();
    AmgSymMatrix(3) cov;
    cov.setZero();
    if (evtInfo) {
        bsVertex->setX(evtInfo->beamPosX());
        bsVertex->setY(evtInfo->beamPosY());
        bsVertex->setZ(evtInfo->beamPosZ());
        cov(0,0) = evtInfo->beamPosSigmaX() * evtInfo->beamPosSigmaX();
        cov(1,1) = evtInfo->beamPosSigmaY() * evtInfo->beamPosSigmaY();
        cov(2,2) = evtInfo->beamPosSigmaZ() * evtInfo->beamPosSigmaZ();
    } else {
        bsVertex->setX(0.);
        bsVertex->setY(0.);
        bsVertex->setZ(0.);
        cov(0,0) = 50.;
        cov(1,1) = 50.;
        cov(2,2) = 300.;
    }
    bsVertex->setCovariancePosition(cov);
    vtxbs = bsVertex.get(); // get the pointer from the unique object
    
    if (!vtxbs) {
        ATH_MSG_DEBUG("Expected dummy vertex for BS");
    }
    
    if (pv) {
        ATH_MSG_VERBOSE("Using argument provided PV");
        vtxpv = pv;
    }
    
    if (!vtxpv) {
        ATH_MSG_DEBUG("Expected PV");
    }
    
    std::vector<double> masses = {m_trackMass,m_trackMass};
    
    vtx->auxdecor<float>("Lxy_bs")      = (vtxbs ? m_V0Tools->lxy     (vtx,vtxbs) : -999.);
    vtx->auxdecor<float>("LxyError_bs") = (vtxbs ? m_V0Tools->lxyError(vtx,vtxbs) : -999.);
    vtx->auxdecor<float>("Tau_bs")      = (vtxbs ? m_V0Tools->tau     (vtx,vtxbs,masses) : -999.);
    vtx->auxdecor<float>("TauError_bs") = (vtxbs ? m_V0Tools->tauError(vtx,vtxbs,masses) : -999.);
    
    vtx->auxdecor<float>("Lxy_pv")      = (vtxpv ? m_V0Tools->lxy     (vtx,vtxpv) : -999.);
    vtx->auxdecor<float>("LxyError_pv") = (vtxpv ? m_V0Tools->lxyError(vtx,vtxpv) : -999.);
    vtx->auxdecor<float>("Tau_pv")      = (vtxpv ? m_V0Tools->tau     (vtx,vtxpv,masses) : -999.);
    vtx->auxdecor<float>("TauError_pv") = (vtxpv ? m_V0Tools->tauError(vtx,vtxpv,masses) : -999.);

    vtx->auxdecor<float>("pT")          = m_V0Tools->pT     (vtx);
    vtx->auxdecor<float>("pTError")     = m_V0Tools->pTError(vtx);
    
    vtx->auxdecor<float>("phiStar")     = m_V0Tools->phiStar     (vtx,m_trackMass,m_trackMass);
    vtx->auxdecor<float>("cosThetaStar")= m_V0Tools->cosThetaStar(vtx,m_trackMass,m_trackMass);
    
    vtx->auxdecor<float>("mass")        = m_V0Tools->invariantMass     (vtx,masses);
    vtx->auxdecor<float>("massError")   = m_V0Tools->invariantMassError(vtx,masses);

    
    
    
    return StatusCode::SUCCESS;
} //BphysTrigDiMuDecoratorTool




/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


