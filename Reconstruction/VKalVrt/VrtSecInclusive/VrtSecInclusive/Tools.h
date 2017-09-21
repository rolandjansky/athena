#ifndef _VrtSecInclusive_Tools_H
#define _VrtSecInclusive_Tools_H

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

namespace VKalVrtAthena {
  
  bool isAssociatedToVertices( const xAOD::TrackParticle *trk, const xAOD::VertexContainer* vertices ) {
    
      bool is_pv_associated = false;
      
      for( auto* vtx : *vertices ) {
        for( size_t iv = 0; iv < vtx->nTrackParticles(); iv++ ) {
          auto* pvtrk = vtx->trackParticle( iv );
          if( trk == pvtrk ) {
            is_pv_associated = true;
            break;
          }
        }
      }
      return is_pv_associated;
  }

}

#endif /* _VrtSecInclusive_Tools_H */
