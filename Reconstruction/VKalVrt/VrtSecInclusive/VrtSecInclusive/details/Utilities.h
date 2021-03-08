/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _VrtSecInclusive_VrtSecInclusive_Utilities_H
#define _VrtSecInclusive_VrtSecInclusive_Utilities_H

#include "VrtSecInclusive/IntersectionPos.h"

#include <TString.h>

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
  
  
  
}


#endif
