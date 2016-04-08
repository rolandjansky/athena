/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NavigationLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_NAVIGATIONLAYER_H
#define TRKGEOMETRY_NAVIGATIONLAYER_H

class MsgStream;

#include "TrkGeometry/Layer.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
    
  class Surface;
  class BinUtility;
    
  /**
   @class NavigationLayer
  
   Class to be used for gaps in Volumes as a navigational link.
   Navigation Layers have a surface representation, but should usually never be
   propagated to.
   
   The fullfill a faster acess to the nextLayer in the navigation process inside a volume.
   
   @author Andreas.Salzburger@cern.ch
   
   */

 class NavigationLayer : public Layer {
        
      public:                
        /**Constructor - the surface representation is given by pointer (ownership passed)*/
        NavigationLayer(Surface* surfaceRepresentation, 
                        Layer* previous=0,
                        Layer* next=0,
                        BinUtility* binUtil = 0);
        
        /**Constructor - the surface representation is given by pointer (ownership passed)
            - spacer layer if needed            
                        */
        NavigationLayer(Surface* surfaceRepresentation, 
                        double thickness);
        
        /**Copy Constructor - */
        NavigationLayer(const NavigationLayer& lay);
                                      
        /**Destructor*/
        virtual ~NavigationLayer();
        
        /** Assignment operator */
        NavigationLayer& operator=(const NavigationLayer& lay);
                    
        /** Transforms the layer into a Surface representation for extrapolation */
        virtual const Surface& surfaceRepresentation() const override;
        
        /** isOnLayer() method, using isOnSurface() with Layer specific tolerance */
        virtual bool isOnLayer(const Amg::Vector3D& gp, const BoundaryCheck& bcheck = BoundaryCheck(true)) const override;
        
        /** getting the MaterialProperties back - for full update*/ 
        const MaterialProperties* fullUpdateMaterialProperties() const;
        
        /** getting the MaterialProperties back - for pre-update*/ 
        double preUpdateMaterialFactor(const Trk::TrackParameters& par,
                                       Trk::PropDirection dir) const override;

        /** getting the MaterialProperties back - for post-update*/ 
        double  postUpdateMaterialFactor(const Trk::TrackParameters& par,
                                         Trk::PropDirection dir) const override;
                                                                      
        /** getting the next/overlapping Surface */
        const Surface* overlapSurface(const TrackParameters& tp, const Surface* sf = 0) const;
        
        /** move the Layer */
        void moveLayer( Amg::Transform3D&  ) const override;

    protected:
       /** Resize the layer to the tracking volume - not implemented */ 
       void resizeLayer(const VolumeBounds&, double) const  override {}      
       
       /** Resize the layer to the tracking volume - not implemented */ 
       virtual void resizeAndRepositionLayer(const VolumeBounds&, const Amg::Vector3D&, double) const  override {}

        Surface*  m_surfaceRepresentation;       //!< for the navigation Volume the surface is a private member */
      
    private:
        /**Default Constructor*/
        NavigationLayer(){}
            
  };

inline const Surface&            NavigationLayer::surfaceRepresentation() const { return (*m_surfaceRepresentation); }  
  
inline const MaterialProperties* NavigationLayer::fullUpdateMaterialProperties() const 
{ return 0; }

inline double NavigationLayer::preUpdateMaterialFactor(const TrackParameters&, PropDirection) const 
{ return 0; }

inline double NavigationLayer::postUpdateMaterialFactor(const TrackParameters&, PropDirection) const 
{ return 0; }

inline const Surface* NavigationLayer::overlapSurface(const TrackParameters&, const Surface*) const
{ return 0; }

inline void NavigationLayer::moveLayer( Amg::Transform3D& ) const
{ }

} // end of namespace

#endif // TRKGEOMETRY_NAVIGATIONLAYER_H



