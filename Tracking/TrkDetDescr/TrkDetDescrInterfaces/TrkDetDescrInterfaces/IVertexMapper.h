/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexMapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_IVERTEXMAPPER_H
#define TRKDETDESCRINTERFACES_IVERTEXMAPPER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/Layer.h"

namespace Trk {
    
  class TrackingVolume;

  struct MappedVertex {
      
      Amg::Vector3D         localPosition;
      const Surface*        surface;
      int                   identifierHash;

      const Layer*          layer;
      int                   layerIndex;
      
      const TrackingVolume* trackingVolume;

      bool                  valid;
      
      /** valid constructor*/
      MappedVertex(const Amg::Vector3D& lPosition, 
                   const Surface& mSurface,
                   const Layer& mLayer,
                   const TrackingVolume& mVolume) :
        localPosition(lPosition),
        surface(&mSurface),
        identifierHash(mSurface.associatedDetectorElement() ? int(mSurface.associatedDetectorElement()->identifyHash()) : -1),
        layer(&mLayer),
        layerIndex(mLayer.layerIndex().value()),
        trackingVolume(&mVolume),
        valid(true)
      {}       
      
       /** invalid constructor*/
       MappedVertex() :
         localPosition(0.,0.,0.),
         surface(nullptr),
         identifierHash(-1),
         layer(nullptr),
         layerIndex(-1),
         trackingVolume(nullptr),
         valid(false)
       {}       
      
      
  };


  /** Interface ID for IVertexMapper*/  
  static const InterfaceID IID_IVertexMapper("IVertexMapper", 1, 0);
  
  /** @class IVertexMapper
     
     Interface class IVertexMapper
  
     interface for a tool that transfers global coordinates into the local coordinate frame of the 
     closest sensitive surface.
  
     @author Andreas.Salzburger@cern.ch
    */
  
  class IVertexMapper : virtual public IAlgTool {
      
     public:
         
       /** Virtual destructor */
       virtual ~IVertexMapper(){}
       
       /** AlgTool and IAlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IVertexMapper; }
                                                    
       /** Record the vertex into the local frame of the closest module  */
       virtual MappedVertex mapToLocal(const Amg::Vector3D& vertex) const = 0;

   };

} // end of namespace

#endif // TRKDETDESCRINTERFACES_IVERTEXMAPPER_H
