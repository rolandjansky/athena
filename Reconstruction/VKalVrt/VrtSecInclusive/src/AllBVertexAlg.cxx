/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AllBVertexAlg.cxx, (c) ATLAS Detector software
// Author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)
///////////////////////////////////////////////////////////////////

#include "VrtSecInclusive/AllBVertexAlg.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "VrtSecInclusive/MultiSecVertexTool.h"

#include "TLorentzVector.h"
#include "CxxUtils/sincos.h"

namespace Rec {

  static const SG::AuxElement::Decorator<float> bvrtM("bvrtM");  
  static const SG::AuxElement::Decorator<float> bvrtPt("bvrtPt");  
  static const SG::AuxElement::Decorator<float> bvrtPhi("bvrtPhi");  
  static const SG::AuxElement::Decorator<float> bvrtEta("bvrtEta");  
  static const SG::AuxElement::Decorator<float> bvrtMinTPt("bvrtMinTPt");  
  static const SG::AuxElement::Decorator<float> bvrtSig3D("bvrtSig3D");  

   AllBVertexAlg::AllBVertexAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm( name, pSvcLocator ),
    m_bvertexContainerName("AllBVertices"),
    m_trContainerName("InDetTrackParticles"),
    m_pvContainerName("PrimaryVertices"),
    m_bvertextool("Rec::MultiSecVertexTool/InclusivetBVertexTool",this)
  {
    declareProperty("BVertexContainerName", m_bvertexContainerName);    
    declareProperty("TrackContainerName", m_trContainerName);
    declareProperty("PVContainerName",m_pvContainerName);
    declareProperty("BVertexTool",m_bvertextool);
  }

  StatusCode AllBVertexAlg::initialize()
  {
    if (m_bvertexContainerName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of AllBVertexAlg!");
      return StatusCode::FAILURE;
    }

    CHECK( m_bvertextool.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode AllBVertexAlg::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode AllBVertexAlg::execute()
  {
    // check that container we want to write in SG does not yet exist
    if (evtStore()->contains<xAOD::VertexContainer>(m_bvertexContainerName)) {
      ATH_MSG_ERROR("Algorithm is attempting to write a StoreGate key " << m_bvertexContainerName 
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
      if (v->vertexType()==xAOD::VxType::PriVtx) { pv = v;  break; }
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
          double minTPt=1.e10;
          for(auto & it : vtrk){
             const Trk::Perigee* mPer = dynamic_cast<const Trk::Perigee*>(it.perigeeAtVertex());
             CxxUtils::sincos   phi(mPer->parameters()[Trk::phi]);
             CxxUtils::sincos theta(mPer->parameters()[Trk::theta]);
             double absP  =  1./std::abs(mPer->parameters()[Trk::qOverP]);
             tmp.SetXYZM( phi.cs*theta.sn*absP, phi.sn*theta.sn*absP, theta.cs*absP, Trk::ParticleMasses().mass[Trk::pion]);
             minTPt=std::min(minTPt,tmp.Pt());
             VSUM+=tmp;
          }
          bvrtM(*iv)  =VSUM.M();
          bvrtPt(*iv) =VSUM.Pt();
          bvrtEta(*iv)=VSUM.Eta();
          bvrtPhi(*iv)=VSUM.Phi();
          bvrtMinTPt(*iv)=minTPt;
          bvrtSig3D(*iv)=vrtVrtDist(*pv,*iv);
       }
    }

    ATH_MSG_DEBUG("BVertex container size: " << bVertexContainer->size());
    CHECK(evtStore()->record( bVertexContainer.release(), m_bvertexContainerName));
    CHECK(evtStore()->record( bVertexAuxContainer.release(), m_bvertexContainerName+"Aux."));

    return StatusCode::SUCCESS;
  }

  double AllBVertexAlg::vrtVrtDist(const xAOD::Vertex & primVrt, const xAOD::Vertex & secVrt)
  const
  {
     double distx =  secVrt.x()- primVrt.x();
     double disty =  secVrt.y()- primVrt.y();
     double distz =  secVrt.z()- primVrt.z();

     AmgSymMatrix(3)  covMtx=primVrt.covariancePosition()+secVrt.covariancePosition();
 
     AmgSymMatrix(3)  wgtMtx = covMtx.inverse();
     if(wgtMtx(0,0)<=0. || wgtMtx(1,1)<=0. || wgtMtx(2,2)<=0.) return 1.e10; //rudimentary corrrectness check

     double signif = distx*wgtMtx(0,0)*distx
                    +disty*wgtMtx(1,1)*disty
                    +distz*wgtMtx(2,2)*distz
                 +2.*distx*wgtMtx(0,1)*disty
                 +2.*distx*wgtMtx(0,2)*distz
                 +2.*disty*wgtMtx(1,2)*distz;
     signif=std::sqrt(std::abs(signif));
     if( signif!=signif ) signif = 0.;
     return signif;
  }

}
