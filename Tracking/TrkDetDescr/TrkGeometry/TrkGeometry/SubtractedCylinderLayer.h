/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedCylinderLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H
#define TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H

class MsgStream;

#include "TrkGeometry/Layer.h"
#include "TrkGeometrySurfaces/SubtractedCylinderSurface.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"


namespace Trk {

class LayerMaterialProperties;
class OverlapDescriptor;

  /**
   @class SubtractedCylinderLayer
   
   Class to describe a cylindrical detector layer for tracking, with subtraction; it inhertis from both, 
   Layer base class and SubtractedCylinderSurface class
       
   @author Sarka.Todorova@cern.ch
  */

  class SubtractedCylinderLayer : virtual public SubtractedCylinderSurface, public Layer {
                   
      public:
        /**Default Constructor*/
        SubtractedCylinderLayer(){}
        
        /**Constructor with SubtractedCylinderSurface components and  MaterialProperties */
        SubtractedCylinderLayer(const SubtractedCylinderSurface* subCyl,
                      const LayerMaterialProperties& laymatprop,
                      double thickness = 0.,
                      OverlapDescriptor* od = 0,
                      int laytyp=int(Trk::active));
                              
        /**Copy constructor*/
        SubtractedCylinderLayer(const SubtractedCylinderLayer& cla);

        /**Copy constructor with shift*/
        SubtractedCylinderLayer(const SubtractedCylinderLayer& cla, const Amg::Transform3D& tr);
        
        /**Assignment operator */
        SubtractedCylinderLayer& operator=(const SubtractedCylinderLayer&);
                      
        /**Destructor*/
        virtual ~SubtractedCylinderLayer(){}  
        
        /** Transforms the layer into a Surface representation for extrapolation */
        const SubtractedCylinderSurface& surfaceRepresentation() const;
        
        /** getting the MaterialProperties back - for pre-update*/ 
        double preUpdateMaterialFactor(const Trk::TrackParameters& par,
                                       Trk::PropDirection dir) const;

        /** getting the MaterialProperties back - for post-update*/ 
        double  postUpdateMaterialFactor(const Trk::TrackParameters& par,
                                         Trk::PropDirection dir) const;

       /** Return the path correction */
       double pathCorrection(const TrackParameters& par) const;

       /** move the Layer */
       void moveLayer( Amg::Transform3D& shift ) const;

   private:
       /** Resize the layer to the tracking volume - not implemented */ 
       void resizeLayer(const VolumeBounds&, double) const {}      
       
       /** Resize the layer to the tracking volume - not implemented */ 
       virtual void resizeAndRepositionLayer(const VolumeBounds&, const Amg::Vector3D&, double) const {}
               
 
  };
 
} // end of namespace

#endif // TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H

