/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This file was largely imported from the Acts testing framework

#include "ObjSurfaceWriter.h"

#include <ios>
#include <iostream>
#include <stdexcept>

#include "Acts/Geometry/Layer.hpp"
#include "Acts/Surfaces/CylinderBounds.hpp"
#include "Acts/Surfaces/PlanarBounds.hpp"
#include "Acts/Surfaces/RadialBounds.hpp"
#include "Acts/Surfaces/SurfaceBounds.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"

#include "Acts/Geometry/Polyhedron.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Surfaces/StrawSurface.hpp"

namespace {
	// this method went away in acts 0.19.0. Porting over this code to use IVisualization seems
	// to be not worth, so porting this (relatively simple) method here.
	std::string objString(const Acts::Polyhedron& polyhedron, size_t vtxOffset) {
		std::stringstream sstr;

		for (const auto& vtx : polyhedron.vertices) {
			sstr << "v " << vtx.x() << " " << vtx.y() << " " << vtx.z() << std::endl;

		}
		for (const auto& face : polyhedron.faces) {
			sstr << "f";
			for (const auto& idx : face) {
				sstr << " " << (vtxOffset + idx + 1);

			}
			sstr << std::endl;

		}
		return sstr.str();

	}
}




Acts::ObjSurfaceWriter::ObjSurfaceWriter(
    const ObjSurfaceWriter::Config& cfg)
  : m_cfg(cfg)
{
  // Validate the configuration
  if (!m_cfg.logger) {
    throw std::invalid_argument("Missing logger");
  } else if (m_cfg.name.empty()) {
    throw std::invalid_argument("Missing algorithm name");
  } else if (!m_cfg.outputStream) {
    throw std::invalid_argument("Missing output stream");
  }

  // Write down the file prefix
  (*(m_cfg.outputStream)) << m_cfg.filePrefix << '\n';
}

std::string
Acts::ObjSurfaceWriter::name() const
{
  return m_cfg.name;
}
void
Acts::ObjSurfaceWriter::write(const std::string& sinfo)
{
  // lock the mutex for writing
  std::lock_guard<std::mutex> lock(m_write_mutex);
  // and write
  (*m_cfg.outputStream) << sinfo;
}

void
Acts::ObjSurfaceWriter::write(const Acts::GeometryContext &gctx,
                              const Acts::Surface &surface)
{
  std::lock_guard<std::mutex> lock(m_write_mutex);

  // check
  ACTS_DEBUG(">>Obj: Writer for Surface object called.");

  auto scalor = m_cfg.outputScalor;
  // let's get the bounds & the transform
  const Acts::SurfaceBounds& surfaceBounds = surface.bounds();
  auto sTransform = surface.transform(gctx);

  // dynamic_cast to PlanarBounds
  const Acts::PlanarBounds* planarBounds
      = dynamic_cast<const Acts::PlanarBounds*>(&surfaceBounds);
  
  const Acts::CylinderBounds* cylinderBounds
      = dynamic_cast<const Acts::CylinderBounds*>(&surfaceBounds);
  
  const Acts::StrawSurface* strawSurface
      = dynamic_cast<const Acts::StrawSurface*>(&surface);

  // only continue if the cast worked
  if (m_cfg.outputSensitive) {
    if (planarBounds) {
      ACTS_VERBOSE(">>Obj: Writing out a PlaneSurface");
      // set the precision - just to be sure
      (*(m_cfg.outputStream)) << '\n';
      (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
      // get the vertices
      auto planarVertices = planarBounds->vertices();
      // loop over the vertices
      std::vector<Acts::Vector3D> vertices;
      vertices.reserve(planarVertices.size());
      for (auto pv : planarVertices) {
        // get the point in 3D
        Acts::Vector3D v3D(sTransform * Acts::Vector3D(pv.x(), pv.y(), 0.));
        vertices.push_back(v3D);
      }
      // get the thickness and vertical faces
      double                    thickness = 0.;
      std::vector<unsigned int> vfaces;
      if (surface.associatedDetectorElement()) {
        // get the thickness form the detector element
        thickness = surface.associatedDetectorElement()->thickness();
        vfaces    = {1, 1, 1, 1};
      }
      // output to file
      ObjHelper::writePlanarFace(*(m_cfg.outputStream),
                                   m_vtnCounter,
                                   scalor,
                                   vertices,
                                   thickness,
                                   vfaces);
      (*(m_cfg.outputStream)) << '\n';
    }
    else if(cylinderBounds) {
      
      auto cylinderSurface = dynamic_cast<const Acts::CylinderSurface*>(&surface);

      Acts::Polyhedron ph =
          cylinderSurface->polyhedronRepresentation(gctx, 10);
      (*(m_cfg.outputStream)) << objString(ph, m_vtnCounter.vcounter);
      m_vtnCounter.vcounter += ph.vertices.size();

    }
    else if(strawSurface) {

      Acts::Polyhedron ph =
          strawSurface->polyhedronRepresentation(gctx, 10);
      (*(m_cfg.outputStream)) << objString(ph, m_vtnCounter.vcounter);
      m_vtnCounter.vcounter += ph.vertices.size();

    }
    else {
      ACTS_ERROR("Unable to print this bounds type: " << surfaceBounds.type());
    }
  }

  // check if you have layer and check what your have
  // dynamic cast to CylinderBounds work the same
  if (cylinderBounds && m_cfg.outputLayerSurface) {
    ACTS_VERBOSE(">>Obj: Writing out a CylinderSurface with r = "
                 << cylinderBounds->get(Acts::CylinderBounds::eR));
    // name the object
    auto layerID = surface.geometryId().layer();
    (*(m_cfg.outputStream))
        << " o Cylinder_" << std::to_string(layerID) << '\n';
    // output to the file
    ObjHelper::writeTube(*(m_cfg.outputStream),
                           m_vtnCounter,
                           scalor,
                           m_cfg.outputPhiSegments,
                           sTransform,
                           cylinderBounds->get(Acts::CylinderBounds::eR),
                           cylinderBounds->get(Acts::CylinderBounds::eHalfLengthZ),
                           m_cfg.outputThickness);
    (*(m_cfg.outputStream)) << '\n';
  }

  ////dynamic cast to RadialBounds or disc bounds work the same
  const Acts::RadialBounds* radialBounds
      = dynamic_cast<const Acts::RadialBounds*>(&surfaceBounds);
  if (radialBounds && m_cfg.outputLayerSurface) {
    ACTS_VERBOSE(">>Obj: Writing out a DiskSurface at z = "
                 << sTransform.translation().z());
    // name the object
    auto layerID = surface.geometryId().layer();
    (*(m_cfg.outputStream)) << "o Disk_" << std::to_string(layerID) << '\n';
    // we use the tube writer in the other direction
    double rMin      = radialBounds->rMin();
    double rMax      = radialBounds->rMax();
    double thickness = rMax - rMin;
    // output to the file
    ObjHelper::writeTube(*(m_cfg.outputStream),
                           m_vtnCounter,
                           scalor,
                           m_cfg.outputPhiSegments,
                           sTransform,
                           0.5 * (rMin + rMax),
                           m_cfg.outputThickness,
                           thickness);
    (*(m_cfg.outputStream)) << '\n';
  }

}
