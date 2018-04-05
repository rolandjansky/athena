// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "GeomACTS/obj/ObjTrackingGeometryWriter.hpp"
#include <iostream>
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Surfaces/Surface.hpp"

Acts::ObjTrackingGeometryWriter::ObjTrackingGeometryWriter(
    const ObjTrackingGeometryWriter::Config& cfg)
  : m_cfg(cfg)
{
}

std::string
Acts::ObjTrackingGeometryWriter::name() const
{
  return m_cfg.name;
}

void
Acts::ObjTrackingGeometryWriter::write(const Acts::TrackingGeometry& tGeometry)
{
  ACTS_DEBUG(">>Obj: Writer for TrackingGeometry object called.");
  // get the world volume
  auto world = tGeometry.highestTrackingVolume();
  if (world) write(*world);
  // return the success code
}

/// process this volume
void
Acts::ObjTrackingGeometryWriter::write(const Acts::TrackingVolume& tVolume)
{
  ACTS_DEBUG(">>Obj: Writer for TrackingVolume object called.");
  // get the confined layers and process them
  if (tVolume.confinedLayers()) {
    ACTS_VERBOSE(">>Obj: Layers are present, process them.");
    // loop over the layers
    for (auto layer : tVolume.confinedLayers()->arrayObjects()) {
      // we jump navigation layers
      if (layer->layerType() == Acts::navigation) continue;
      // get the volume name
      const std::string& volumeName = tVolume.volumeName();
      // find the right surfacewriter
      std::shared_ptr<ObjSurfaceWriter> surfaceWriter = nullptr;
      for (auto writer : m_cfg.surfaceWriters) {
        // get name and writer
        auto writerName = writer->name();
        if (volumeName.find(writerName) != std::string::npos) {
          // asign the writer
          surfaceWriter = writer;
          // and break
          break;
        }
      }
      // bail out if you have no surface writer
      if (!surfaceWriter) return;
      // layer prefix
      surfaceWriter->write(m_cfg.layerPrefix);
      // try to write the material surface as well
      if (layer->surfaceRepresentation().associatedMaterial())
        surfaceWriter->write(layer->surfaceRepresentation());
      // the the approaching surfaces and check if they have material
      if (layer->approachDescriptor()) {
        // loop over the contained Surfaces
        for (auto& cSurface : layer->approachDescriptor()->containedSurfaces())
          if (cSurface->associatedMaterial()) surfaceWriter->write(*cSurface);
      }
      // check for sensitive surfaces
      if (layer->surfaceArray() && surfaceWriter) {
        // surfaces
        surfaceWriter->write(m_cfg.sensitiveGroupPrefix);
        // loop over the surface
        for (auto surface : layer->surfaceArray()->surfaces()) {
          if (surface) surfaceWriter->write(*surface);
        }
      }
    }
  }
  // get the confined volumes and step down the hierarchy
  if (tVolume.confinedVolumes()) {
    // loop over the volumes and write what they have
    for (auto volume : tVolume.confinedVolumes()->arrayObjects()) {
      write(*volume.get());
    }
  }
}
