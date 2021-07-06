/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AssociatedMaterial.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/AssociatedMaterial.h"

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/TrackingVolume.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iostream>

Trk::AssociatedMaterial::AssociatedMaterial()
    : m_materialPosition(Amg::Vector3D(0., 0., 0.)),
      m_correctionFactor(0.),
      m_materialProperties(nullptr),
      m_materialStep(nullptr),
      m_trackingVolume(nullptr),
      m_layer(nullptr),
      m_cleanup(false) {}

Trk::AssociatedMaterial::AssociatedMaterial(const Amg::Vector3D& hitpos,
                                            float steplength, float X0,
                                            float L0, float A, float Z,
                                            float rho, float cFactor,
                                            const TrackingVolume* tvol,
                                            const Layer* lay)
    : m_materialPosition(hitpos),
      m_correctionFactor(cFactor),
      m_materialProperties(nullptr),
      m_materialStep(new Trk::MaterialStep(hitpos.x(), hitpos.y(), hitpos.z(),
                                           steplength, X0, L0, A, Z, rho)),
      m_trackingVolume(tvol),
      m_layer(lay),
      m_cleanup(true) {}

Trk::AssociatedMaterial::AssociatedMaterial(
    const Amg::Vector3D& hitpos, float steplength, const Trk::Material& mat,
    float cFactor, const TrackingVolume* tvol, const Layer* lay)
    : m_materialPosition(hitpos),
      m_correctionFactor(cFactor),
      m_materialProperties(new Trk::MaterialProperties(mat, steplength)),
      m_materialStep(nullptr),
      m_trackingVolume(tvol),
      m_layer(lay),
      m_cleanup(true) {}

Trk::AssociatedMaterial::AssociatedMaterial(
    const Amg::Vector3D& hitpos, const Trk::MaterialProperties* mprop,
    float cFactor, const Trk::TrackingVolume* tvol, const Trk::Layer* lay)
    : m_materialPosition(hitpos),
      m_correctionFactor(cFactor),
      m_materialProperties(mprop),
      m_materialStep(nullptr),
      m_trackingVolume(tvol),
      m_layer(lay),
      m_cleanup(false)

{}

Trk::AssociatedMaterial::AssociatedMaterial(const Trk::MaterialStep* ms,
                                            float cFactor,
                                            const Trk::TrackingVolume* tvol,
                                            const Trk::Layer* lay)
    : m_materialPosition(Amg::Vector3D(ms->hitX(), ms->hitY(), ms->hitZ())),
      m_correctionFactor(cFactor),
      m_materialProperties(nullptr),
      m_materialStep(ms),
      m_trackingVolume(tvol),
      m_layer(lay),
      m_cleanup(false) {}

Trk::AssociatedMaterial::AssociatedMaterial(const Amg::Vector3D& hitpos,
                                            const Trk::TrackingVolume* tvol,
                                            const Trk::Layer* lay)
    : m_materialPosition(hitpos),
      m_correctionFactor(0.),
      m_materialProperties(nullptr),
      m_materialStep(nullptr),
      m_trackingVolume(tvol),
      m_layer(lay),
      m_cleanup(false) {}

Trk::AssociatedMaterial::AssociatedMaterial(const Trk::AssociatedMaterial& am)
    : m_materialPosition(am.m_materialPosition),
      m_correctionFactor(am.m_correctionFactor),
      m_materialProperties(am.m_cleanup && am.m_materialProperties
                               ? am.m_materialProperties->clone()
                               : am.m_materialProperties),
      m_materialStep((am.m_cleanup && am.m_materialStep)
                         ? new Trk::MaterialStep(*am.m_materialStep)
                         : am.m_materialStep),
      m_trackingVolume(am.m_trackingVolume),
      m_layer(am.m_layer),
      m_cleanup(am.m_cleanup) {}

Trk::AssociatedMaterial::AssociatedMaterial(
    Trk::AssociatedMaterial&& am) noexcept
    : m_materialPosition(std::move(am.m_materialPosition)),
      m_correctionFactor(am.m_correctionFactor),
      m_materialProperties(am.m_materialProperties),
      m_materialStep(am.m_materialStep),
      m_trackingVolume(am.m_trackingVolume),
      m_layer(am.m_layer),
      m_cleanup(am.m_cleanup) {
  am.m_materialProperties = nullptr;
  am.m_materialStep = nullptr;
  am.m_cleanup = false;
}

Trk::AssociatedMaterial& Trk::AssociatedMaterial::operator=(
    const Trk::AssociatedMaterial& am) {
  if (&am != this) {
    if (m_cleanup) {
      delete m_materialStep;
      delete m_materialProperties;
    }
    m_materialPosition = am.m_materialPosition;
    m_correctionFactor = am.m_correctionFactor;
    m_materialProperties = (am.m_cleanup && am.m_materialProperties)
                               ? am.m_materialProperties->clone()
                               : am.m_materialProperties;
    m_materialStep = (am.m_cleanup && am.m_materialStep)
                         ? new Trk::MaterialStep(*am.m_materialStep)
                         : am.m_materialStep;
    m_trackingVolume = am.m_trackingVolume;
    m_layer = am.m_layer;
    m_cleanup = am.m_cleanup;
  }
  return (*this);
}

Trk::AssociatedMaterial& Trk::AssociatedMaterial::operator=(
    Trk::AssociatedMaterial&& am) noexcept {
  m_materialPosition = std::move(am.m_materialPosition);
  m_correctionFactor = am.m_correctionFactor;
  m_trackingVolume = am.m_trackingVolume;
  m_layer = am.m_layer;
  m_cleanup = am.m_cleanup;

  if (m_materialProperties && m_materialProperties != am.m_materialProperties) {
    delete m_materialProperties;
  }
  m_materialProperties = am.m_materialProperties;
  am.m_materialProperties = nullptr;

  if (m_materialStep && m_materialStep != am.m_materialStep) {
    delete m_materialStep;
  }
  m_materialStep = am.m_materialStep;
  am.m_materialStep = nullptr;

  am.m_cleanup = false;
  return *this;
}

Trk::AssociatedMaterial::~AssociatedMaterial() {
  if (m_cleanup) {
    delete m_materialStep;
    delete m_materialProperties;
  }
}

MsgStream& Trk::AssociatedMaterial::dump(MsgStream& sl) const {
  sl << "----> AssociatedMaterial - recorded at (x/y/z) | r : ("
     << m_materialPosition.x() << "/ ";
  sl << m_materialPosition.y() << "/ ";
  sl << m_materialPosition.z() << ")" << std::endl;
  sl << "      Material (t/x0/l0/A/Z/rho) : (" << steplength() << "/ ";
  sl << x0() << "/ ";
  sl << l0() << "/ ";
  sl << A() << "/ ";
  sl << Z() << "/  ";
  sl << rho() << ")" << std::endl;
  if (m_trackingVolume)
    sl << "      Associated to TrackingVolume : "
       << m_trackingVolume->volumeName() << std::endl;
  if (m_layer)
    sl << "              and Layer with Index : "
       << m_layer->layerIndex().value() << std::endl;
  return sl;
}

std::ostream& Trk::AssociatedMaterial::dump(std::ostream& sl) const {
  sl << "----> AssociatedMaterial - recorded at (x/y/z) | r : ("
     << m_materialPosition.x() << "/ ";
  sl << m_materialPosition.y() << "/ ";
  sl << m_materialPosition.z() << ")" << std::endl;
  sl << "      Material (t/x0/l0/A/Z/rho) : (" << steplength() << "/ ";
  sl << x0() << "/ ";
  sl << l0() << "/ ";
  sl << A() << "/ ";
  sl << Z() << "/  ";
  sl << rho() << ")" << std::endl;
  if (m_trackingVolume)
    sl << "      Associated to TrackingVolume : "
       << m_trackingVolume->volumeName() << std::endl;
  if (m_layer)
    sl << "              and Layer with Index : "
       << m_layer->layerIndex().value() << std::endl;
  return sl;
}

MsgStream& Trk::operator<<(MsgStream& sl,
                           const Trk::AssociatedMaterial& mstep) {
  return mstep.dump(sl);
}

std::ostream& Trk::operator<<(std::ostream& sl,
                              const Trk::AssociatedMaterial& mstep) {
  return mstep.dump(sl);
}

