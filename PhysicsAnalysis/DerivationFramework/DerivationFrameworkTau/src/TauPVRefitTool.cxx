/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauPVRefitTool.cxx
// Author: Evelina Bouhova-Thacker (e.bouhova@cern.ch)
//
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkTau/TauPVRefitTool.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "xAODTracking/Vertex.h"

namespace DerivationFramework {

  TauPVRefitTool::TauPVRefitTool(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_linkName("PVLink"),
    m_reflinkName("TauRefittedPVLink"),
    m_pvrefContainerName("TauRefittedPrimaryVertices"),
    m_pvContainerName("PrimaryVertices"),
    m_tauTrkContainerName("TauPVTracks"),
    m_pvrefitter("Analysis::PrimaryVertexRefitter")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("PVLinkName", m_linkName);
    declareProperty("RefittedLinkName", m_reflinkName);
    declareProperty("PVRefContainerName", m_pvrefContainerName);
    declareProperty("PVContainerName", m_pvContainerName);
    declareProperty("TauTrkContainerName", m_tauTrkContainerName);
    declareProperty("PrimaryVertexRefitterTool", m_pvrefitter);
  }

  StatusCode TauPVRefitTool::initialize()
  {
    if (m_pvrefContainerName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of the tau PV refitter tool!");
      return StatusCode::FAILURE;
    }
    CHECK( m_pvrefitter.retrieve() );
    return StatusCode::SUCCESS;
  }

  StatusCode TauPVRefitTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TauPVRefitTool::addBranches() const
  {
    // check that container we want to write in SG does not yet exist
    if (evtStore()->contains<std::vector<float> >(m_pvrefContainerName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_pvrefContainerName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }

    xAOD::VertexContainer*    TauRefittedPrimaryVertices(0);
    xAOD::VertexAuxContainer* TauRefittedPrimaryVerticesAux(0);

    CHECK(refit(TauRefittedPrimaryVertices,TauRefittedPrimaryVerticesAux));
    ATH_MSG_DEBUG ("number of refitted PVs " << TauRefittedPrimaryVertices->size());

    if (!evtStore()->contains<xAOD::VertexContainer>(m_pvrefContainerName))
      CHECK(evtStore()->record(TauRefittedPrimaryVertices, m_pvrefContainerName));
    if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_pvrefContainerName+"Aux."))
      CHECK(evtStore()->record(TauRefittedPrimaryVerticesAux, m_pvrefContainerName+"Aux."));

    return StatusCode::SUCCESS;
  }  

  StatusCode TauPVRefitTool::refit(xAOD::VertexContainer*& TauRefittedPrimaryVertices, xAOD::VertexAuxContainer*& TauRefittedPrimaryVerticesAux) const
  {
    TauRefittedPrimaryVertices = new xAOD::VertexContainer;
    TauRefittedPrimaryVerticesAux = new xAOD::VertexAuxContainer;
    TauRefittedPrimaryVertices->setStore(TauRefittedPrimaryVerticesAux);

    // Get PV collection from StoreGate
    const xAOD::VertexContainer* pv_cont = evtStore()->retrieve< const xAOD::VertexContainer >( m_pvContainerName );
    if (pv_cont == 0) return StatusCode::SUCCESS;
    ATH_MSG_DEBUG ("number of PVs " << pv_cont->size());

    // Get selected tau tracks
    std::vector<const xAOD::TrackParticle*> tps; tps.clear();
    xAOD::TrackParticleContainer* trk_cont = evtStore()->retrieve< xAOD::TrackParticleContainer >( m_tauTrkContainerName );
    for (auto trk: *trk_cont) tps.push_back(trk);
    ATH_MSG_DEBUG ("tps size " << tps.size());

    std::vector<unsigned int> pv_index; pv_index.clear();
    unsigned int ipv = 0, ipv2 = 0;
    // loop over PV container and refit
    for (auto pv: *pv_cont ) {
      const xAOD::Vertex* pv_ref = m_pvrefitter->refitVertex(pv,tps);
      if (pv_ref) {
        if (pv_ref->nTrackParticles() < pv->nTrackParticles()) {
          TauRefittedPrimaryVertices->push_back(const_cast<xAOD::Vertex*>(pv_ref));
          pv_index.push_back(ipv);
        } else {
          delete pv_ref;
        }
      } else {
        delete pv_ref;
      }
      ipv++;
    }

    // loop over PV container and set links
    for (auto pv: *pv_cont ) {
      ElementLink<xAOD::VertexContainer> pvLink;
      static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > mDecor_pvLink(m_reflinkName);
      ElementLink<xAOD::VertexContainer> ref_pvLink;
      static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > mDecor_ref_pvLink(m_linkName);
      if (TauRefittedPrimaryVertices->size() == 0) mDecor_pvLink(*pv) = pvLink;
      unsigned int irpv = 0;
      for (auto rpv: *TauRefittedPrimaryVertices ) {
        if (ipv2 == pv_index[irpv]) {
          ATH_MSG_DEBUG ("    pv x " << pv->x() << " y " << pv->y() << " z " << pv->z() << " ntrks " << pv->nTrackParticles() << " ndf " << pv->numberDoF() << " chi2 " << pv->chiSquared());
          ATH_MSG_DEBUG ("ref pv x " << rpv->x() << " y " << rpv->y() << " z " << rpv->z() << " ntrks " << rpv->nTrackParticles() << " ndf " << rpv->numberDoF() << " chi2 " << rpv->chiSquared());
          pvLink.setElement(rpv);
          pvLink.setStorableObject(*TauRefittedPrimaryVertices);
          mDecor_pvLink(*pv) = pvLink;

          ref_pvLink.setElement(pv);
          ref_pvLink.setStorableObject(*pv_cont);
          mDecor_ref_pvLink(*rpv) = ref_pvLink;
        } else {
          mDecor_pvLink(*pv) = pvLink;
        }
        irpv++;
      }
      ipv2++;
    }

    return StatusCode::SUCCESS; 
  }
}
