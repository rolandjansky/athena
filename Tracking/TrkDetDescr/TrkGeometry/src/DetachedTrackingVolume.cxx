/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DetachedTrackingVolume.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/DetachedTrackingVolume.h"

#include <utility>

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/TrackingVolume.h"

Trk::DetachedTrackingVolume::DetachedTrackingVolume()
    : m_trkVolume(),
      m_name("undefined"),
      m_layerRepresentation(nullptr),
      m_multilayerRepresentation(nullptr),
      m_baseTransform(nullptr),
      m_constituents(nullptr) {}

Trk::DetachedTrackingVolume::DetachedTrackingVolume(
    std::string name, const Trk::TrackingVolume* volume)
    : m_trkVolume(volume),
      m_name(std::move(name)),
      m_layerRepresentation(nullptr),
      m_multilayerRepresentation(nullptr),
      m_baseTransform(nullptr),
      m_constituents(nullptr) {}

Trk::DetachedTrackingVolume::DetachedTrackingVolume(
    std::string name, const Trk::TrackingVolume* volume, const Trk::Layer* lay,
    const std::vector<const Trk::Layer*>* multilay)
    : m_trkVolume(volume),
      m_name(std::move(name)),
      m_layerRepresentation(lay),
      m_multilayerRepresentation(multilay),
      m_baseTransform(nullptr),
      m_constituents(nullptr) {}

Trk::DetachedTrackingVolume::~DetachedTrackingVolume() {
  delete m_trkVolume;
  if (m_layerRepresentation) delete m_layerRepresentation;
  if (m_multilayerRepresentation) {
    for (unsigned int i = 0; i < m_multilayerRepresentation->size(); i++)
      delete (*m_multilayerRepresentation)[i];
    delete m_multilayerRepresentation;
  }
  delete m_baseTransform;
}

void Trk::DetachedTrackingVolume::move ATLAS_NOT_THREAD_SAFE(
    Amg::Transform3D& shift) const {
  m_trkVolume->moveTV(shift);
  if (m_layerRepresentation){
    (const_cast<Trk::Layer*>(m_layerRepresentation))->moveLayer(shift);
  }
  if (m_multilayerRepresentation) {
    for (unsigned int i = 0; i < m_multilayerRepresentation->size(); i++) {
      (const_cast<Trk::Layer*>((*m_multilayerRepresentation)[i]))->moveLayer(
        shift);
    }
  }
}

const Trk::DetachedTrackingVolume* Trk::DetachedTrackingVolume::clone
ATLAS_NOT_THREAD_SAFE(const std::string& name, Amg::Transform3D& shift) const {
  Trk::TrackingVolume* newTV =
      new TrackingVolume(*(this->trackingVolume()), shift);
  Trk::DetachedTrackingVolume* newStat = nullptr;
  // layer representation ?
  Trk::PlaneLayer* newLay = nullptr;
  if (this->layerRepresentation()) {
    std::vector<const Trk::Layer*>* newMulti = nullptr;
    const Trk::PlaneLayer* pl =
        dynamic_cast<const Trk::PlaneLayer*>(this->layerRepresentation());
    if (pl) {
      newLay = new Trk::PlaneLayer(*pl);
      newLay->moveLayer(shift);
      if (this->multilayerRepresentation()) {
        newMulti = new std::vector<const Trk::Layer*>;
        for (unsigned int i = 0; i < this->multilayerRepresentation()->size();
             i++) {
          const Trk::PlaneLayer* mpl = dynamic_cast<const Trk::PlaneLayer*>(
              (*(this->multilayerRepresentation()))[i]);
          if (mpl) {
            Trk::PlaneLayer* newPl = new Trk::PlaneLayer(*mpl);
            newPl->moveLayer(shift);
            newMulti->push_back(newPl);
          } else
            std::cout << "WARNING   Trk::DetachedTrackingVolume::clone()   "
                         "dynamic cast to 'const Trk::PlaneLayer* mpl' failed!"
                      << std::endl;
        }
      }
      newStat = new Trk::DetachedTrackingVolume(name, newTV, newLay, newMulti);
    } else {
      std::cout << "WARNING   Trk::DetachedTrackingVolume::clone()   dynamic "
                   "cast to 'const Trk::PlaneLayer* pl' failed!"
                << std::endl;
      newStat = new Trk::DetachedTrackingVolume(name, newTV);
    }
  } else {
    newStat = new Trk::DetachedTrackingVolume(name, newTV);
  }
  //
  // enclose layers
  if (newTV->confinedVolumes()) {
    BinnedArraySpan<Trk::TrackingVolume const * const> vols =
        newTV->confinedVolumes()->arrayObjects();
    for (unsigned int ivol = 0; ivol < vols.size(); ivol++) {
      const Trk::LayerArray* layAr = vols[ivol]->confinedLayers();
      const std::vector<const Trk::Layer*>* alays =
          vols[ivol]->confinedArbitraryLayers();
      if (layAr) {
        Trk::BinnedArraySpan<const Trk::Layer* const> lays = layAr->arrayObjects();
        for (unsigned int il = 0; il < lays.size(); il++) {
          (const_cast<Trk::Layer*>(lays[il]))
            ->encloseDetachedTrackingVolume(*newStat);
        }
      }
      if (alays) {
        for (unsigned int il = 0; il < alays->size(); il++) {
          (const_cast<Trk::Layer*>((*alays)[il]))
            ->encloseDetachedTrackingVolume(*newStat);
        }
      }
    }
  }
  if (newTV->confinedLayers()) {
    BinnedArraySpan<Trk::Layer* const> lays =
        newTV->confinedLayers()->arrayObjects();
    for (unsigned int il = 0; il < lays.size(); il++){
      (const_cast<Trk::Layer*>(lays[il]))
        ->encloseDetachedTrackingVolume(*newStat);
    }
  }
  if (newTV->confinedArbitraryLayers()) {
    const std::vector<const Trk::Layer*>* alays =
        newTV->confinedArbitraryLayers();
    for (unsigned int il = 0; il < alays->size(); il++){
      (const_cast<Trk::Layer*>((*alays)[il]))
        ->encloseDetachedTrackingVolume(*newStat);
    }
  }
  //
  newStat->saveConstituents(this->constituents());
  return newStat;
}

void Trk::DetachedTrackingVolume::compactify ATLAS_NOT_THREAD_SAFE(
    size_t& cSurfaces, size_t& tSurfaces) const {
  // deal with the Tracking Volume representation
  if (m_trkVolume) m_trkVolume->compactify(cSurfaces, tSurfaces);

  // deal with the layer representation
  if (layerRepresentation()) {
    ++tSurfaces;
    if (layerRepresentation()->surfaceRepresentation().owner() == Trk::noOwn) {
      const_cast<Trk::Surface&>(layerRepresentation()->surfaceRepresentation())
        .setOwner(Trk::TGOwn);
      ++cSurfaces;
    }
  }
  // deal with the multi-layer representation
  if (multilayerRepresentation()) {
    tSurfaces += m_multilayerRepresentation->size();
    for (const auto& mLayerIter : (*m_multilayerRepresentation)) {
      if ((*mLayerIter).surfaceRepresentation().owner() == Trk::noOwn) {
        const_cast<Trk::Surface&>((*mLayerIter).surfaceRepresentation())
          .setOwner(Trk::TGOwn);
        ++cSurfaces;
      }
    }
  }
  //<< !@ TODO include volumes
}

void Trk::DetachedTrackingVolume::sign ATLAS_NOT_THREAD_SAFE(
    GeometrySignature signat, GeometryType geotype) const {
  m_trkVolume->sign(signat, geotype);
}

Trk::GeometrySignature Trk::DetachedTrackingVolume::geometrySignature() const {
  return m_trkVolume->geometrySignature();
}

Trk::GeometryType Trk::DetachedTrackingVolume::geometryType() const {
  return m_trkVolume->geometryType();
}

void Trk::DetachedTrackingVolume::setBaseTransform(Amg::Transform3D* transf) {
  if (transf)
    m_baseTransform = transf;
  else {
    delete m_baseTransform;
    m_baseTransform = new Amg::Transform3D(this->trackingVolume()->transform());
  }
}

