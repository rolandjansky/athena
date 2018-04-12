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
    
    using IPDecoratorType = SG::AuxElement::Decorator< std::vector< std::vector<float> > >;
    static IPDecoratorType decor_d0wrtSV    ( "d0_wrtSVs" );
    static IPDecoratorType decor_z0wrtSV    ( "z0_wrtSVs" );
    static IPDecoratorType decor_ptwrtSV    ( "pt_wrtSVs" );
    static IPDecoratorType decor_etawrtSV   ( "eta_wrtSVs" );
    static IPDecoratorType decor_phiwrtSV   ( "phi_wrtSVs" );
    static IPDecoratorType decor_d0errWrtSV ( "d0err_wrtSVs" );
    static IPDecoratorType decor_z0errWrtSV ( "z0err_wrtSVs" );
    
    // Grouping decorators
    std::vector< IPDecoratorType > decor_ipWrtSVs { decor_d0wrtSV, decor_z0wrtSV, decor_ptwrtSV, decor_etawrtSV, decor_phiwrtSV, decor_d0errWrtSV, decor_z0errWrtSV };
    enum { k_ip_d0, k_ip_z0, k_ip_pt, k_ip_eta, k_ip_phi, k_ip_d0err, k_ip_z0err };
    
    static SG::AuxElement::Decorator< std::vector<ElementLink< xAOD::VertexContainer > > > decor_svLink("svLinks");
    
    // Loop over leptons
    for( const auto& lepton : *leptonContainer ) {
      
      std::vector< std::vector< std::vector<float> > > ip_wrtSVs( decor_ipWrtSVs.size() ); // triple nest of { ip parameters, tracks, DVs }
      
      bool linkFlag { false };
      
      std::vector<unsigned> trackTypes;
      genSequence<LeptonFlavor>( lepton, trackTypes );
    
      // Loop over lepton types
      for( auto& trackType : trackTypes ) {
        
        std::vector< std::vector<float> > ip_wrtSV( decor_ipWrtSVs.size() ); // nest of { tracks, DVs }
        
        const auto* trk = getLeptonTrackParticle<LeptonFlavor>( lepton, trackType );
        
        if( !trk ) continue;
      
        std::map< const xAOD::Vertex*, std::vector<double> > distanceMap;
      
        std::vector<ElementLink< xAOD::VertexContainer > > links;
        
        // Loop over vertices
        for( const auto& vtx : *secondaryVertexContainer ) {
      
          std::vector<double> impactParameters;
          std::vector<double> impactParErrors;
          
          m_fitSvc->VKalGetImpact( trk, vtx->position(), static_cast<int>( lepton->charge() ), impactParameters, impactParErrors );
          
          enum { k_d0, k_z0, k_theta, k_phi, k_qOverP }; // for the impact parameter
          enum { k_d0d0, k_d0z0, k_z0z0 };               // for the par errors
          
          const auto& theta = impactParameters.at( k_theta );
          const auto& phi   = impactParameters.at( k_phi );
          const auto  p     = fabs( 1.0 / impactParameters.at(k_qOverP) );
          const auto  pt    = fabs( p * sin( theta ) );
          const auto  eta   = -log( tan(theta/2.) );
          
          // filling the parameters to the corresponding container
          ip_wrtSV.at( k_ip_d0 )    .emplace_back( impactParameters.at(k_d0) );
          ip_wrtSV.at( k_ip_z0 )    .emplace_back( impactParameters.at(k_d0) );
          ip_wrtSV.at( k_ip_pt )    .emplace_back( pt );
          ip_wrtSV.at( k_ip_eta )   .emplace_back( eta );
          ip_wrtSV.at( k_ip_phi )   .emplace_back( phi );
          ip_wrtSV.at( k_ip_d0err ) .emplace_back( impactParErrors.at(k_d0d0) );
          ip_wrtSV.at( k_ip_z0err ) .emplace_back( impactParErrors.at(k_z0z0) );
          
          if( !linkFlag ) {
            
            ElementLink<xAOD::VertexContainer> link_SV( *( dynamic_cast<const xAOD::VertexContainer*>( vtx->container() ) ), static_cast<size_t>( vtx->index() ) );
            links.emplace_back( link_SV );
            
          }
          
        } // end of vertex loop
        
        // The linking to the vertices need to be done only once
        if( !linkFlag ) {
          decor_svLink ( *lepton ) = links;
          linkFlag = true;
        }
        
        for( size_t ipar = 0; ipar < ip_wrtSVs.size(); ipar++ ) ip_wrtSVs.at( ipar ).emplace_back( ip_wrtSV.at( ipar ) );
        
      } // end of track type loop
      
      // decoration
      for( size_t ipar = 0; ipar < decor_ipWrtSVs.size(); ipar++ ) decor_ipWrtSVs.at( ipar )( *lepton ) = ip_wrtSVs.at( ipar );
      
    } // end of lepton container loop
    
    return StatusCode::SUCCESS;
  }
  
  
}


#endif
