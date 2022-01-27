/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_TrackingGeometryBuilderCond.h (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HGTD_TRACKINGGEOMETRY_HGTD_TRACKINGGEOMETRYBUILDERCOND_H
#define HGTD_TRACKINGGEOMETRY_HGTD_TRACKINGGEOMETRYBUILDERCOND_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Trk
#include "TrkDetDescrInterfaces/IGeometryBuilderCond.h"
#include "TrkDetDescrUtils/GeometrySignature.h"

// STL
#include <vector>
#include <utility>
#include <algorithm>

namespace Trk {
  class Material;
  class Layer;
  class TrackingGeometry;
  class ITrackingVolumeCreator;
  class ILayerArrayCreator;
  class ILayerBuilderCond;
}

class IEnvelopeDefSvc;

class HGTD_TrackingGeometryBuilderCond : public AthAlgTool,
                                     virtual public Trk::IGeometryBuilderCond {

public:
  /** Constructor */
  HGTD_TrackingGeometryBuilderCond(const std::string&,const std::string&,const IInterface*);

  /** Destructor */
  virtual ~HGTD_TrackingGeometryBuilderCond();

  /** AlgTool initailize method.*/
  StatusCode initialize();

  /** AlgTool finalize method */
  StatusCode finalize();

  /** TrackingGeometry Interface methode */
  std::pair<EventIDRange, Trk::TrackingGeometry*> trackingGeometry
  ATLAS_NOT_THREAD_SAFE(
    const EventContext& ctx,
    std::pair<EventIDRange, const Trk::TrackingVolume*> tVolPair) const;

  /** The unique signature */
  Trk::GeometrySignature geometrySignature() const { return Trk::HGTD; }

private:
  /** Service to handle the envelope definition */
  ServiceHandle<IEnvelopeDefSvc>                m_enclosingEnvelopeSvc;
  /** Helper tools for the geometry building */
  ToolHandle<Trk::ILayerBuilderCond>            m_layerBuilder;
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

#endif // HGTD_TRACKINGGEOMETRY_HGTD_TRACKINGGEOMETRYBUILDERCOND_H
