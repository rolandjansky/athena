/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
 
 ///////////////////////////////////////////////////////////////////
 // NewVrtSecInclusiveAlg.cxx, (c) ATLAS Detector software
 // Author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)
 ///////////////////////////////////////////////////////////////////
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "NewVrtSecInclusiveTool/NewVrtSecInclusiveAlg.h"

#include "TLorentzVector.h"
#include "CxxUtils/sincos.h"

namespace Rec {

   static const SG::AuxElement::Decorator<float> bvrtM("bvrtM");  
   static const SG::AuxElement::Decorator<float> bvrtPt("bvrtPt");  
   static const SG::AuxElement::Decorator<float> bvrtPhi("bvrtPhi");  
   static const SG::AuxElement::Decorator<float> bvrtEta("bvrtEta");  

   NewVrtSecInclusiveAlg::NewVrtSecInclusiveAlg(const std::string& name, ISvcLocator* pSvcLocator) :
     AthAlgorithm( name, pSvcLocator ),
     m_tpContainerName(""),
     m_pvContainerName(""),
     m_foundVertices("AllBVertices"),
     m_bvertextool("Rec::NewVrtSecInclusiveTool/SVTool",this)
   {
     declareProperty("TrackContainerName",   m_tpContainerName,"InDetTrackParticles");
     declareProperty("PVContainerName",      m_pvContainerName,"PrimaryVertices");
     declareProperty("BVertexContainerName", m_foundVertices);
     declareProperty("BVertexTool",m_bvertextool);
   }

   StatusCode NewVrtSecInclusiveAlg::initialize()
   {
     ATH_CHECK( m_foundVertices.initialize() );
     ATH_CHECK( m_bvertextool.retrieve() );

     return StatusCode::SUCCESS;
   }

   StatusCode NewVrtSecInclusiveAlg::finalize()
   {
     return StatusCode::SUCCESS;
   }

   StatusCode NewVrtSecInclusiveAlg::execute()
   {
     //-- Extract TrackParticles
     SG::ReadHandle<xAOD::TrackParticleContainer> tp_cont(m_tpContainerName);
     if ( !tp_cont.isValid() ) {
        ATH_MSG_WARNING( "No TrackParticle container found in TES" );
        return StatusCode::FAILURE; }

     //-- Extract Primary Vertices
     SG::ReadHandle<xAOD::VertexContainer> pv_cont(m_pvContainerName);
     if ( !pv_cont.isValid() ) {
       ATH_MSG_WARNING( "No Primary Vertices container found in TDS" );
       return StatusCode::FAILURE; }

     //-- Extract PV itself
     const xAOD::Vertex* pv = nullptr;
     for ( auto v : *pv_cont ) {
       if (v->vertexType()==xAOD::VxType::PriVtx) {    pv = v;   break; }
     }
     if (!pv) {
       ATH_MSG_WARNING("Primary vertex not found");
       return StatusCode::FAILURE;
     }

     //-- create container for new vertices
     xAOD::VertexContainer    * bVertexContainer    = new (std::nothrow) xAOD::VertexContainer();
     xAOD::VertexAuxContainer * bVertexAuxContainer = new (std::nothrow) xAOD::VertexAuxContainer();
     if(!bVertexContainer || !bVertexAuxContainer )return StatusCode::FAILURE;
     bVertexContainer->setStore(bVertexAuxContainer);

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
     ATH_MSG_DEBUG("Found Vertices in this event: " << bVertexContainer->size());

     SG::WriteHandle<xAOD::VertexContainer>  vrtInThisEvent(m_foundVertices);
     ATH_CHECK( vrtInThisEvent.record (std::make_unique<xAOD::VertexContainer>(*bVertexContainer),
                                       std::make_unique<xAOD::VertexAuxContainer>(*bVertexAuxContainer)) );
     return StatusCode::SUCCESS;
   }
}

