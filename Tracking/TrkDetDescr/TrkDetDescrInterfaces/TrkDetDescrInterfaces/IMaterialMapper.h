/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMaterialMapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_IMATERIALMAPPER_H
#define TRKDETDESCRINTERFACES_IMATERIALMAPPER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
    class TrackingVolume;
    class Layer;
    class AssociatedMaterial;
}

namespace Trk {


  /** Interface ID for IMaterialMapper*/  
  static const InterfaceID IID_IMaterialMapper("IMaterialMapper", 1, 0);
  
  /** @class IMaterialMapper
     
     Interface class IMaterialMapper
     it allows to log the material for single track events,
     it is usually steered by a MaterialEffectsUpdator instance/
     
     @author Andreas.Salzburger@cern.ch
    */
  class IMaterialMapper : virtual public IAlgTool {
      
     public:
         
       /**Virtual destructor*/
       virtual ~IMaterialMapper(){}
       
       /** AlgTool and IAlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IMaterialMapper; }
    
       /** Record navigation hit : 
            - navigation hit position       
         */
       virtual void record(const Amg::Vector3D& position, int type = 0) const = 0;
        
       /** Record material hit :
           -> For pure Material validation only pathInX0, x0, A, Z, rho, can be filled,
           -> For Material effects validation sigmaPhi, sigmaTheta, delta p and sigmaP
              can be filled as well
       */
       virtual void recordMaterialHit(const AssociatedMaterial& amhit, const Amg::Vector3D& projectedPosition) const = 0;

       /** Record material hit - if various hits per uniform layer are recorded */
       virtual void recordLayerHit(const AssociatedMaterial& amhit) const = 0;

       /** Record full material hit - to overwrite collected single layer hit informations */
       virtual void recordFullLayerHit(const AssociatedMaterial& amhit) const = 0;

       /** Finalize the SingleLayer Steps */    
       virtual void finalizeLayerHits() const = 0;
                                            
       /** Record material hit */
       virtual void recordSurfaceHit(const Amg::Vector2D& locpos,
                                         const AssociatedMaterial& amhit) const = 0;

       /** Finalize the SingleSurface Steps */    
       virtual void finalizeSurfaceHits() const = 0;

       /** FinalizeEvent - with possibility to fill dedicated layer information */
       virtual void finalizeEvent(double eta, double phi) const = 0;     


   };

} // end of namespace

#endif // TRKDETDESCRINTERFACES_IMATERIALMAPPER_H
