/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SoftBVertexTool.cxx, (c) ATLAS Detector software
// Author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkHiggs/SoftBVertexTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "VrtSecInclusive/MultiSecVertexTool.h"

#include "TLorentzVector.h"
#include "CxxUtils/sincos.h"

namespace DerivationFramework {

  static const SG::AuxElement::Decorator<float> bvrtM("bvrtM");  
  static const SG::AuxElement::Decorator<float> bvrtPt("bvrtPt");  
  static const SG::AuxElement::Decorator<float> bvrtPhi("bvrtPhi");  
  static const SG::AuxElement::Decorator<float> bvrtEta("bvrtEta");  

  SoftBVertexTool::SoftBVertexTool(const std::string& t,
				   const std::string& n,
				   const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_bvertexContainerName(""),
    m_trContainerName(""),
    m_pvContainerName(""),
    m_bvertextool("Rec::MultiSecVertexTool/SoftBVertexTool",this)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("BVertexContainerName", m_bvertexContainerName);    
    declareProperty("TrackContainerName", m_trContainerName,"InDetTrackParticles");
    declareProperty("PVContainerName",m_pvContainerName,"PrimaryVertices");
    declareProperty("BVertexTool",m_bvertextool);
  }

  StatusCode SoftBVertexTool::initialize()
  {
    if (m_bvertexContainerName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of SoftBVertexTool!");
      return StatusCode::FAILURE;
    }

    CHECK( m_bvertextool.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode SoftBVertexTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode SoftBVertexTool::addBranches() const
  {
    // check that container we want to write in SG does not yet exist
    if (evtStore()->contains<xAOD::VertexContainer>(m_bvertexContainerName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_bvertexContainerName 
                     << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }

    const xAOD::VertexContainer* pv_cont = nullptr;
    ATH_CHECK( evtStore()->retrieve( pv_cont, m_pvContainerName ) );

    std::unique_ptr<xAOD::VertexContainer> bVertexContainer = std::make_unique<xAOD::VertexContainer>();
    std::unique_ptr<xAOD::VertexAuxContainer> bVertexAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
    bVertexContainer->setStore(bVertexAuxContainer.get());

    const xAOD::Vertex* pv = nullptr;
    for ( auto v : *pv_cont ) {
      if (v->vertexType()==xAOD::VxType::PriVtx) {
	pv = v;
	break;
      }
    }
    if (!pv) {
      ATH_MSG_WARNING("Primary vertex not found");
      CHECK(evtStore()->record( bVertexContainer.release(), m_bvertexContainerName));
      CHECK(evtStore()->record( bVertexAuxContainer.release(), m_bvertexContainerName+"Aux."));
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Found PV");

    // retrieve particle collections
    const xAOD::TrackParticleContainer* tp_cont = nullptr;
    ATH_CHECK( evtStore()->retrieve( tp_cont, m_trContainerName ) );
    std::vector<const xAOD::TrackParticle*> trkparticles(0);
    for(auto tp : (*tp_cont)) trkparticles.push_back(tp);


    std::unique_ptr<Trk::VxSecVertexInfo> foundVrts = m_bvertextool->findAllVertices(trkparticles,*pv);      
    if(foundVrts && foundVrts->vertices().size()){
       const std::vector<xAOD::Vertex*> vtmp=foundVrts->vertices();
       for(auto & iv :  vtmp) {
          bVertexContainer->push_back(iv);
          std::vector< Trk::VxTrackAtVertex > & vtrk = iv->vxTrackAtVertex();
          TLorentzVector VSUM(0.,0.,0.,0.);
          TLorentzVector tmp;
          for(auto & it : vtrk){
             const Trk::Perigee* mPer = dynamic_cast<const Trk::Perigee*>(it.perigeeAtVertex());
             CxxUtils::sincos   phi(mPer->parameters()[Trk::phi]);
             CxxUtils::sincos theta(mPer->parameters()[Trk::theta]);
             double absP  =  1./std::abs(mPer->parameters()[Trk::qOverP]);
             tmp.SetXYZM( phi.cs*theta.sn*absP, phi.sn*theta.sn*absP, theta.cs*absP, Trk::ParticleMasses().mass[Trk::pion]);
             VSUM+=tmp;
          }
          bvrtM(*iv)  =VSUM.M();
          bvrtPt(*iv) =VSUM.Pt();
          bvrtEta(*iv)=VSUM.Eta();
          bvrtPhi(*iv)=VSUM.Phi();
       }
    }

    ATH_MSG_DEBUG("Vertex container size: " << bVertexContainer->size());
    CHECK(evtStore()->record( bVertexContainer.release(), m_bvertexContainerName));
    CHECK(evtStore()->record( bVertexAuxContainer.release(), m_bvertexContainerName+"Aux."));

    return StatusCode::SUCCESS;
  }


}
