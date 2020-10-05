///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
{
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
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

    if ( m_V0Tools.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_V0Tools);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_DEBUG("Retrieved tool " << m_V0Tools);
    }

  ATH_CHECK( m_pvCandidatesKey.initialize() );
  ATH_CHECK( m_beamSpotKey.initialize() );
    
  return StatusCode::SUCCESS;
}

StatusCode BphysTrigDiMuDecoratorTool::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

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


    SG::ReadHandle<xAOD::VertexContainer> pvContainer{m_pvCandidatesKey};
    // check if PV container - ie. if cosmics // https://its.cern.ch/jira/browse/ATR-10633
    if (!pvContainer.isValid()) {
        ATH_MSG_DEBUG("No PrimaryVertex container of name: " << m_pvCandidatesKey);
    } // no PV
       
    const xAOD::Vertex * vtxbs(nullptr), *vtxpv(nullptr);
    if (pvContainer.isValid()) {
        for (const auto& primvtx: *pvContainer) {
            if ( primvtx->vertexType() == xAOD::VxType::PriVtx) {
                vtxpv = primvtx;
                break;
            }
        } // vtx
    } // if pv container is found
    
    SG::ReadCondHandle<InDet::BeamSpotData> beamspot{m_beamSpotKey};
    if ( ! beamspot.isValid() ) {
        ATH_MSG_DEBUG("No EventInfo found; dummy BS position used");
    } else {
        ATH_MSG_VERBOSE("Beamspot: " 
                      << beamspot->beamPos()[0] << " "
                      << beamspot->beamPos()[1] << " "
                      << beamspot->beamPos()[2] << " / "
                      << beamspot->beamSigma(0) << " "
                      << beamspot->beamSigma(1) << " " 
                      << beamspot->beamSigma(2) << " " );
    }
    std::unique_ptr<xAOD::Vertex> bsVertex = std::make_unique<xAOD::Vertex>();
    bsVertex->makePrivateStore();
    AmgSymMatrix(3) cov;
    cov.setZero();
    if (beamspot.isValid()) {
        bsVertex->setX(beamspot->beamPos()[0]);
        bsVertex->setY(beamspot->beamPos()[1]);
        bsVertex->setZ(beamspot->beamPos()[2]);
        for (short axis = 0; axis < 3; ++axis ) {
            cov(axis,axis) = beamspot->beamSigma( axis ) * beamspot->beamSigma( axis );
        }
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


