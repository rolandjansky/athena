/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonVertexSelectionAlg.cxx

#include "PhotonVertexSelectionAlg.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "AthContainers/ConstDataVector.h"
#include "PhotonVertexSelection/IPhotonVertexSelectionTool.h"

namespace CP {

  using std::string;
  using namespace xAOD;

  //**********************************************************************

  PhotonVertexSelectionAlg::PhotonVertexSelectionAlg(const string& name,
			 ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ),
      m_vxSelTool("CP::PhotonVertexSelectionTool/PhotonVtxSel")
  {
    declareProperty( "InputVertices",  m_inVxColl ="PrimaryVertices"    );
    declareProperty( "InputPhotons",   m_inGamColl="Photons"            );
    declareProperty( "OutputVertices", m_outVxColl="HggPrimaryVertices" );
    declareProperty( "VxSelTool",      m_vxSelTool                      );

  }

  //**********************************************************************

  PhotonVertexSelectionAlg::~PhotonVertexSelectionAlg() { }

  //**********************************************************************

  StatusCode PhotonVertexSelectionAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode PhotonVertexSelectionAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode PhotonVertexSelectionAlg::execute() { 
    ATH_MSG_VERBOSE("Executing " << name() << "...");

    VertexContainer* vxOut(new VertexContainer);
    VertexAuxContainer* vxOutAux(new VertexAuxContainer);
    vxOut->setStore(vxOutAux);
    vxOut->push_back(new Vertex);
    // Immediately add one vertex, because we will always want one in the container.
    // Strictly speaking, if the event has no vertex, this could wind up being a dummy.
    ATH_CHECK( evtStore()->record(vxOut, m_outVxColl) );
    ATH_CHECK( evtStore()->record(vxOutAux, m_outVxColl+"Aux.") );

    const PhotonContainer* photons(0);
    ATH_CHECK( evtStore()->retrieve(photons, m_inGamColl) );

    ConstDataVector<PhotonContainer> photons_presel(SG::VIEW_ELEMENTS);
    for(const auto& ph : *photons) {
      if(acceptPhoton(*ph)) photons_presel.push_back( ph );
    }

    const Vertex* photonVx(0);
    if( m_vxSelTool->getVertex(*photons_presel.asDataVector(), photonVx).isSuccess() && photonVx) {
      *vxOut->front() = *photonVx; // deep copy of photon vertex, to record
      vxOut->front()->setVertexType(VxType::PriVtx);
    } else {
      const VertexContainer* vxIn(0);
      ATH_CHECK( evtStore()->retrieve(vxIn, m_inVxColl) );
      for(const auto& vx : *vxIn) {
	if(vx->vertexType()==VxType::PriVtx) *vxOut->front() = *vx;
      }
    }

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  // add some reasonable preselection here a la
  // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/HiggsPhys/Run2/HGamma/xAOD/HGamAnalysisFramework/trunk/Root/PhotonHandler.cxx#L511
  bool PhotonVertexSelectionAlg::acceptPhoton(const xAOD::Photon& /*gam*/) {return true;}

}

