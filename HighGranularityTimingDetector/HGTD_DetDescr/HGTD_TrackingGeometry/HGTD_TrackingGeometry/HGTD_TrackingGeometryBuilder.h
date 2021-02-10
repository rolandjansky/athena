/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_TrackingGeometryBuilder.h (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HGTD_TRACKINGGEOMETRY_HGTD_TRACKINGGEOMETRYBUILDER_H
#define HGTD_TRACKINGGEOMETRY_HGTD_TRACKINGGEOMETRYBUILDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Trk
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkDetDescrUtils/ComparisonUtils.h"

#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

// STL
#include <vector>

class StoreGateSvc;

namespace Trk {
  class Material;
  class Layer;
  class TrackingGeometry;
  class ITrackingVolumeCreator;
  class ILayerArrayCreator;
  class ILayerBuilder;
}

class IEnvelopeDefSvc;

struct DiscLayerInfo {
  float maxR = 0.;
  float maxZ = 0.;
  float thickness = 0.;
  float minZ = std::numeric_limits<float>::max(); 
  float minR = std::numeric_limits<float>::max(); 
};

namespace HGTDet {
  
  class HGTD_TrackingGeometryBuilder : public AthAlgTool,
                                       virtual public Trk::IGeometryBuilder {
  
  public:
    /** Constructor */
    HGTD_TrackingGeometryBuilder(const std::string&,const std::string&,const IInterface*);
    
    /** Destructor */
    virtual ~HGTD_TrackingGeometryBuilder();
      
    /** AlgTool initailize method.*/
    StatusCode initialize();
    
    /** AlgTool finalize method */
    StatusCode finalize();
    
    /** TrackingGeometry Interface methode */
    const Trk::TrackingGeometry* trackingGeometry(const Trk::TrackingVolume* tvol = 0) const; 
    
    /** The unique signature */
    //TODO: Add HGTD Trk signature
    Trk::GeometrySignature geometrySignature() const { return Trk::HGTD; }
      
  private:
    /** Service to handle the envelope definition */
    ServiceHandle<IEnvelopeDefSvc>                m_enclosingEnvelopeSvc;
    /** Helper tools for the geometry building */
    ToolHandle<Trk::ILayerBuilder>                m_layerBuilder;
    /** Helper Tool to create TrackingVolumes */
    ToolHandle<Trk::ITrackingVolumeCreator>       m_trackingVolumeCreator;   
    
    /** configurations for the layer builder */
    /** forces robust indexing for layers */
    bool                                           m_indexStaticLayers;         
    /** create boundary layers */
    bool                                           m_buildBoundaryLayers;       
    /** run with replacement of all joint boundaries  */
    bool                                           m_replaceJointBoundaries;    
    /** binning type for layers */
    int                                            m_layerBinningType;          
    /** Color code for layers */
    unsigned int                                   m_colorCodeConfig;          
    
  };                                       
  
  
  
} // end of namespace



#endif // HGTD_TRACKINGGEOMETRY_HGTD_TRACKINGGEOMETRYBUILDER_H
