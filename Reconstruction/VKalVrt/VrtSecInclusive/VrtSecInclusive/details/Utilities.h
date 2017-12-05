/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _VrtSecInclusive_VrtSecInclusive_Utilities_H
#define _VrtSecInclusive_VrtSecInclusive_Utilities_H

#include "VrtSecInclusive/IntersectionPos.h"
#include <numeric>

namespace VKalVrtAthena {
  
  using namespace std;
  
  //____________________________________________________________________________________________________
  template<class TrackT> void VrtSecInclusive::getIntersection( TrackT *trk, vector<IntersectionPos*>& layers, const Trk::Perigee* per) {
    //
    // get intersection point of track with various surfaces
    //
    
    //--------------------
    // main loop
    for( auto *layer : layers ) {
      
      ATH_MSG_VERBOSE( " >> getIntersection(): attempt to loop " << layer->name() );
      setIntersection( trk, layer, per );
      
    }
    
    
    ATH_MSG_VERBOSE( " >> getIntersection(): End of getIntersection" );
  }
  
  
  //____________________________________________________________________________________________________
  template<class TrackT> void VrtSecInclusive::setIntersection( TrackT* trk, IntersectionPos *layer, const Trk::Perigee* per) {
    const Trk::TrackParameters* Output (nullptr);
    
    
    if( layer->bec() == IntersectionPos::barrel ) {
      
      IntersectionPos_barrel *barrel = dynamic_cast<IntersectionPos_barrel*>( layer );
      
      ATH_MSG_VERBOSE( " >>> setIntersection: name = " << barrel->name() << ", Radius = " << barrel->radius() );
      
      Amg::Transform3D *trnsf = new Amg::Transform3D(); // remember to delete at the end
      (*trnsf).setIdentity();
      Trk::CylinderSurface surfBorder(trnsf, barrel->radius(), 300000.); // 
      Output = m_extrapolator->extrapolateDirectly(*per,surfBorder,Trk::anyDirection,true,Trk::pion);
      
      barrel->x()->push_back( Output? Output->position().x() : -10000 );
      barrel->y()->push_back( Output? Output->position().y() : -10000 );
      barrel->z()->push_back( Output? Output->position().z() : -10000 );
      
      
    } else if( layer->bec() == IntersectionPos::endcap ) {
      
      IntersectionPos_endcap *endcap = dynamic_cast<IntersectionPos_endcap*>( layer );
      
      ATH_MSG_VERBOSE( " >>> setIntersection: name = " << endcap->name() <<
		       ", zpos = " << endcap->zpos() << 
		       ", Rmin = " << endcap->rmin() <<
		       ", Rmax = " << endcap->rmax()    );
      
      Amg::Transform3D *trnsf = new Amg::Transform3D();
      trnsf->translate( Amg::Vector3D(0.,0.,endcap->zpos()) );
      Trk::DiscSurface surfBorder(trnsf, endcap->rmin(), endcap->rmax() );
      Output = m_extrapolator->extrapolateDirectly(*per, surfBorder, Trk::anyDirection, true, Trk::pion);
      
      endcap->x()->push_back( Output? Output->position().x() : -10000 );
      endcap->y()->push_back( Output? Output->position().y() : -10000 );
      endcap->z()->push_back( Output? Output->position().z() : -10000 );
      
    }
    
    
    if( Output ) {
      trk->template auxdecor<float>( Form("intersection_%s_x", layer->name().c_str()) ) = Output->position().x();
      trk->template auxdecor<float>( Form("intersection_%s_y", layer->name().c_str()) ) = Output->position().y();
      trk->template auxdecor<float>( Form("intersection_%s_z", layer->name().c_str()) ) = Output->position().z();
    }
      
    delete Output;
  }
  

  //____________________________________________________________________________________________________
  template<class LeptonFlavor>
  void genSequence( const LeptonFlavor*, std::vector<unsigned>& ) {}

  template<> void genSequence( const xAOD::Muon*, std::vector<unsigned>& trackTypes );
  template<> void genSequence( const xAOD::Electron* electron, std::vector<unsigned>& trackTypes );
  
  //____________________________________________________________________________________________________
  template<class LeptonFlavor>
  const xAOD::TrackParticle* getLeptonTrackParticle( const LeptonFlavor*, const unsigned& ) { return nullptr; }
  
  template<> const xAOD::TrackParticle* getLeptonTrackParticle( const xAOD::Muon* muon, const unsigned& trackType );
  template<> const xAOD::TrackParticle* getLeptonTrackParticle( const xAOD::Electron* electron, const unsigned& trackType );
  
  //____________________________________________________________________________________________________
  template<class LeptonFlavor>
  StatusCode VrtSecInclusive::augmentDVimpactParametersToLeptons( const std::string& containerName )
  {
    
    const xAOD::VertexContainer *secondaryVertexContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( secondaryVertexContainer, "VrtSecInclusive_" + m_jp.secondaryVerticesContainerName ) );
    
    using LeptonContainer = DataVector<LeptonFlavor>;
    
    const LeptonContainer *leptonContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( leptonContainer, containerName ) );
    
    static SG::AuxElement::Decorator< std::vector< std::vector<float> > > decor_d0wrtSV( "d0_wrtSVs" );
    static SG::AuxElement::Decorator< std::vector< std::vector<float> > > decor_z0wrtSV( "z0_wrtSVs" );
    static SG::AuxElement::Decorator< std::vector<ElementLink< xAOD::VertexContainer > > > decor_svLink("svLinks");
    
    std::vector< std::vector<float> > d0wrtSVs;
    std::vector< std::vector<float> > z0wrtSVs;
      
    // Loop over muons
    for( const auto& lepton : *leptonContainer ) {
      
      d0wrtSVs.clear();
      z0wrtSVs.clear();
      
      bool linkFlag { false };
      
      std::vector<unsigned> trackTypes;
      genSequence<LeptonFlavor>( lepton, trackTypes );
    
      // Loop over lepton types
      for( auto& trackType : trackTypes ) {
        
        std::vector<float> d0wrtSV;
        std::vector<float> z0wrtSV;
        
        const auto* trk = getLeptonTrackParticle<LeptonFlavor>( lepton, trackType );
        
        if( !trk ) continue;
      
        std::map< const xAOD::Vertex*, std::vector<double> > distanceMap;
      
        std::vector<ElementLink< xAOD::VertexContainer > > links;
      
        for( const auto& vtx : *secondaryVertexContainer ) {
      
          std::vector<double> impactParameters;
          std::vector<double> impactParErrors;
          
          m_fitSvc->VKalGetImpact( trk, vtx->position(), static_cast<int>( lepton->charge() ), impactParameters, impactParErrors);
          
          enum { k_d0, k_z0, k_theta, k_phi, k_qOverP };
          
          d0wrtSV.emplace_back( impactParameters.at(k_d0) );
          z0wrtSV.emplace_back( impactParameters.at(k_z0) );
          
          if( !linkFlag ) {
            
            ElementLink<xAOD::VertexContainer> link_SV( *( dynamic_cast<const xAOD::VertexContainer*>( vtx->container() ) ), static_cast<size_t>( vtx->index() ) );
            links.emplace_back( link_SV );
            
          }
          
        }
      
        if( !linkFlag ) {
          decor_svLink ( *lepton ) = links;
          linkFlag = true;
        }
        
        d0wrtSVs.emplace_back( d0wrtSV );
        z0wrtSVs.emplace_back( z0wrtSV );
        
      }
      
      decor_d0wrtSV( *lepton ) = d0wrtSVs;
      decor_z0wrtSV( *lepton ) = z0wrtSVs;
      
    }
    
    return StatusCode::SUCCESS;
  }
  
  
}


#endif
