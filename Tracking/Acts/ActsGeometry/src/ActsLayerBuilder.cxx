/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// ATHENA
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

// PACKAGE
#include "ActsGeometry/ActsLayerBuilder.h"
#include "ActsInterop/IdentityHelper.h"

// ACTS
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Geometry/ApproachDescriptor.hpp"
#include "Acts/Geometry/GenericApproachDescriptor.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/LayerCreator.hpp"
#include "Acts/Geometry/ProtoLayer.hpp"
#include "Acts/Material/ProtoSurfaceMaterial.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Surfaces/DiscSurface.hpp"
#include "Acts/Utilities/BinningType.hpp"
#include "Acts/Surfaces/AnnulusBounds.hpp"

#include "Acts/Visualization/GeometryView3D.hpp"
#include "Acts/Visualization/ObjVisualization3D.hpp"

#include <iterator>
#include <unordered_map>

using Acts::Surface;
using Acts::Transform3;
using Acts::Translation3;

using namespace Acts::UnitLiterals;

ActsLayerBuilder::ActsLayerBuilder(const Config&                cfg,
                      std::unique_ptr<const Acts::Logger> logger)
  : m_logger(std::move(logger))
{
  m_cfg = cfg;
  if(m_cfg.mode == Mode::Undefined) {
    throw std::invalid_argument("ALB mode is undefined");
  }
}

const Acts::LayerVector
ActsLayerBuilder::negativeLayers(const Acts::GeometryContext &gctx) const {
  ACTS_VERBOSE("Building negative layers");
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto mutableThis = const_cast<ActsLayerBuilder *>(this);
  Acts::LayerVector nVector;
  mutableThis->buildEndcap(gctx, nVector, -1);
  return nVector;
}

const Acts::LayerVector
ActsLayerBuilder::centralLayers(const Acts::GeometryContext &gctx) const {
  ACTS_VERBOSE("Building central layers");
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto mutableThis = const_cast<ActsLayerBuilder *>(this);
  Acts::LayerVector cVector;
  mutableThis->buildBarrel(gctx, cVector);
  return cVector;
}

const Acts::LayerVector
ActsLayerBuilder::positiveLayers(const Acts::GeometryContext &gctx) const {
  ACTS_VERBOSE("Building positive layers");
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto mutableThis = const_cast<ActsLayerBuilder *>(this);
  Acts::LayerVector pVector;
  mutableThis->buildEndcap(gctx, pVector, 1);
  return pVector;
}

std::vector<std::shared_ptr<const ActsDetectorElement>>
ActsLayerBuilder::getDetectorElements() const {
  ACTS_VERBOSE("Retrieving detector elements from detector manager");
  if (m_cfg.mng == nullptr) {
    ACTS_ERROR("Manager is null");
    throw std::runtime_error{"Detector manager is null"};
  }
  auto siDetMng = static_cast<const InDetDD::SiDetectorManager *>(m_cfg.mng);
  ACTS_VERBOSE("Detector manager has "
               << std::distance(siDetMng->getDetectorElementBegin(),
                                siDetMng->getDetectorElementEnd())
               << " elements");

  std::vector<std::shared_ptr<const ActsDetectorElement>> elements;

  InDetDD::SiDetectorElementCollection::const_iterator iter;
  for (iter = siDetMng->getDetectorElementBegin();
       iter != siDetMng->getDetectorElementEnd(); ++iter) {
    const InDetDD::SiDetectorElement *siDetElement =
        dynamic_cast<InDetDD::SiDetectorElement *>(*iter);
    if (siDetElement == nullptr) {
      ACTS_ERROR("Detector element was nullptr");
      throw std::runtime_error{"Corrupt detector element collection"};
    }
    elements.push_back(
        std::make_shared<const ActsDetectorElement>(*siDetElement));
  }
  ACTS_VERBOSE("Retrieved " << elements.size() << " elements");

  return elements;
}

void ActsLayerBuilder::buildBarrel(const Acts::GeometryContext &gctx,
                                   Acts::LayerVector &layersOutput) {

  ACTS_VERBOSE("Build layers: BARREL");
  std::vector<std::shared_ptr<const ActsDetectorElement>> elements =
      getDetectorElements();

  std::map<int, std::vector<std::shared_ptr<const Acts::Surface>>> layers{};

  for (const auto &element : elements) {

    IdentityHelper id = element->identityHelper();
    if (id.bec() != 0)
      continue;

    if(m_cfg.mode == Mode::ITkPixelInner) {
      if(id.layer_disk() >= 2) continue;
    }
    if(m_cfg.mode == Mode::ITkPixelOuter) {
      if(id.layer_disk() <= 1) continue;
    }

    m_cfg.elementStore->push_back(element);

    int elementLayer;
    elementLayer = id.layer_disk();

    layers[elementLayer].push_back(element->surface().getSharedPtr());
  }


  if (m_cfg.objDebugOutput) {
    std::string lab;
    for (auto &[key, layerSurfaces] : layers) {

      std::stringstream name;



      name << "obj/" << m_cfg.mode << "_brl_" << std::setfill('0') << std::setw(2)
          << std::to_string(key) + "_bec"
          << ".obj";

      std::ofstream ofs{name.str()};
      Acts::ObjVisualization3D vis{};
      Acts::ViewConfig vc = Acts::s_viewSensitive;
      vc.nSegments = 200;
      for (const auto &surface : layerSurfaces) {
        Acts::GeometryView3D::drawSurface(vis, *surface, gctx,
                                          Acts::Transform3::Identity(), vc);
      }

      vis.write(ofs);
    }
  }

  ACTS_VERBOSE("Found " << layers.size() << " barrel layers");
  // pre loop for more concise output. only do if we actually print it
  if (logger().doPrint(Acts::Logging::VERBOSE)) {
    size_t n = 1;
    for (const auto &[key, surfaces] : layers) {
      Acts::ProtoLayer pl(gctx, surfaces);
      ACTS_VERBOSE("Layer #" << n << " with layerKey: (" << key << ")");
      ACTS_VERBOSE(" -> at rMin / rMax: " << pl.min(Acts::binR) << " / "
                                          << pl.max(Acts::binR));
      ACTS_VERBOSE("    -> at zMin / zMax: " << pl.min(Acts::binZ) << " / "
                                             << pl.max(Acts::binZ));

      n++;
    }
  }

  for (const auto &[key, surfaces] : layers) {

    std::unique_ptr<Acts::ApproachDescriptor> approachDescriptor = nullptr;
    std::shared_ptr<const Acts::ProtoSurfaceMaterial> materialProxy = nullptr;

    // layers and extent are determined, build actual layer
    Acts::ProtoLayer pl(gctx, surfaces);
    pl.envelope[Acts::binR] = m_cfg.barrelEnvelopeR;
    pl.envelope[Acts::binZ] = m_cfg.barrelEnvelopeZ;

    double layerZ = pl.medium(Acts::binZ, true);
    double layerHalfZ = 0.5 * pl.range(Acts::binZ);

    Acts::Transform3 transform(Translation3(0., 0., -layerZ));
    // set up approach descriptor

    std::shared_ptr<Acts::CylinderSurface> innerBoundary =
        Acts::Surface::makeShared<Acts::CylinderSurface>(
            transform, pl.min(Acts::binR), layerHalfZ);

    std::shared_ptr<Acts::CylinderSurface> outerBoundary =
        Acts::Surface::makeShared<Acts::CylinderSurface>(
            transform, pl.max(Acts::binR), layerHalfZ);

    std::shared_ptr<Acts::CylinderSurface> centralSurface =
        Acts::Surface::makeShared<Acts::CylinderSurface>(
            transform, (pl.min(Acts::binR) + pl.max(Acts::binR)) / 2.,
            layerHalfZ);

    size_t binsPhi = m_cfg.barrelMaterialBins.first;
    size_t binsZ = m_cfg.barrelMaterialBins.second;

    Acts::BinUtility materialBinUtil(binsPhi, -M_PI, M_PI, Acts::closed,
                                     Acts::binPhi);
    materialBinUtil += Acts::BinUtility(binsZ, -layerHalfZ, layerHalfZ,
                                        Acts::open, Acts::binZ, transform);

    materialProxy =
        std::make_shared<const Acts::ProtoSurfaceMaterial>(materialBinUtil);

    ACTS_VERBOSE("[L] Layer is marked to carry support material on Surface ( "
                 "inner=0 / center=1 / outer=2 ) : "
                 << "inner");
    ACTS_VERBOSE("with binning: [" << binsPhi << ", " << binsZ << "]");

    ACTS_VERBOSE("Created ApproachSurfaces for cylinder layer at:");
    ACTS_VERBOSE(" - inner:   R=" << pl.min(Acts::binR));
    ACTS_VERBOSE(" - central: R=" << (pl.min(Acts::binR) + pl.max(Acts::binR)) /
                                         2.);
    ACTS_VERBOSE(" - outer:   R=" << pl.max(Acts::binR));

    // set material on inner
    innerBoundary->assignSurfaceMaterial(materialProxy);

    std::vector<std::shared_ptr<const Acts::Surface>> aSurfaces;
    aSurfaces.push_back(std::move(innerBoundary));
    aSurfaces.push_back(std::move(centralSurface));
    aSurfaces.push_back(std::move(outerBoundary));

    approachDescriptor =
        std::make_unique<Acts::GenericApproachDescriptor>(std::move(aSurfaces));

    // count the number of relevant modules in each direction
    auto phiEqual = [this](const Acts::Surface &a,
                           const Acts::Surface &b) {
      Acts::GeometryContext gctx; // unused in matcher
      Acts::BinningValue bv = Acts::binPhi;
      return m_cfg.surfaceMatcher(gctx, bv, &a, &b);
    };

    auto zEqual = [this](const Acts::Surface &a,
                         const Acts::Surface &b) {
      Acts::GeometryContext gctx; // unused in matcher
      Acts::BinningValue bv = Acts::binZ;
      return m_cfg.surfaceMatcher(gctx, bv, &a, &b);
    };

    // Work around issue with clang10 --- it doesn't allow
    // listing surfaces directly in the capture list.
    auto& xsurfaces = surfaces;
    auto countKey = [&xsurfaces](auto equal) -> size_t {
      std::vector<const Acts::Surface *> keySurfaces;
      for (const auto &surface : xsurfaces) {
        bool exists = false;
        for (const auto* existing : keySurfaces) {
          if (equal(*surface, *existing)) {
            exists = true;
            break;
          }
        }
        if (!exists) {
          keySurfaces.push_back(surface.get());
        }
      }
      return keySurfaces.size();
    };

    size_t nModPhi = countKey(phiEqual);
    size_t nModZ = countKey(zEqual);

    ACTS_VERBOSE("Found " << nModPhi << " modules in phi " << nModZ
                          << " modules in z");

    std::shared_ptr<Acts::Layer> layer;
    if (m_cfg.mode == Mode::ITkStrip) {
      double f = 1.0;
      if (key <= 1) {
        f = 4.0; // four rows per module
      } else {
        f = 2.0; // two rows per module
      }
      size_t nBinsPhi = nModPhi / f;
      size_t nBinsZ = nModZ / f;
      layer = m_cfg.layerCreator->cylinderLayer(gctx, surfaces, nBinsPhi,
                                                nBinsZ, pl, transform,
                                                std::move(approachDescriptor));
    } else if (m_cfg.mode == Mode::ITkPixelInner ||
               m_cfg.mode == Mode::ITkPixelOuter) {
      double f = 1.0;
      if (key == 0) {
        f = 2.0;
      }
      size_t nBinsPhi = nModPhi / f;
      size_t nBinsZ = nModZ / f;
      layer = m_cfg.layerCreator->cylinderLayer(gctx, surfaces, nBinsPhi,
                                                nBinsZ, pl, transform,
                                                std::move(approachDescriptor));
    } else {
      layer = m_cfg.layerCreator->cylinderLayer(
          gctx, surfaces, Acts::equidistant, Acts::equidistant, pl, transform,
          std::move(approachDescriptor));
    }

    layersOutput.push_back(layer);
  }
}

void ActsLayerBuilder::buildEndcap(const Acts::GeometryContext &gctx,
                                   Acts::LayerVector &layersOutput, int type) {

  ACTS_VERBOSE("Build layers: " << (type < 0 ? "NEGATIVE" : "POSITIVE")
                                << " ENDCAP");
  std::vector<std::shared_ptr<const ActsDetectorElement>> elements =
      getDetectorElements();
  std::map<std::tuple<int, int, int>, std::vector<const Acts::Surface *>>
      initialLayers{};

  for (const auto &element : elements) {

    IdentityHelper id = element->identityHelper();
    if (id.bec() * type <= 0) {
      continue;
    }

    if(m_cfg.mode == Mode::ITkPixelInner) {
      if(id.layer_disk() >= 3) continue;
    }
    if(m_cfg.mode == Mode::ITkPixelOuter) {
      if(id.layer_disk() <= 2) continue;
    }

    m_cfg.elementStore->push_back(element);

    std::tuple<int, int, int> key{id.bec(), id.layer_disk(), id.eta_module()};

    initialLayers[key].push_back(&element->surface());
  }

  ACTS_VERBOSE("Found " << initialLayers.size() << " "
                        << (type < 0 ? "NEGATIVE" : "POSITIVE")
                        << " ENDCAP inital layers");

  std::vector<Acts::ProtoLayer> protoLayers;
  protoLayers.reserve(initialLayers.size());

  for (const auto &[key, surfaces] : initialLayers) {
    auto &pl = protoLayers.emplace_back(gctx, surfaces);
    pl.envelope[Acts::binR] = m_cfg.endcapEnvelopeR;
    pl.envelope[Acts::binZ] = m_cfg.endcapEnvelopeZ;
  }

  // sort proto layers by their medium z position
  std::sort(protoLayers.begin(), protoLayers.end(),
            [type](const Acts::ProtoLayer &a, const Acts::ProtoLayer &b) {
              double midA = (a.min(Acts::binZ) + a.max(Acts::binZ)) / 2.0;
              double midB = (b.min(Acts::binZ) + b.max(Acts::binZ)) / 2.0;
              if (type < 0) {
                return midA < midB;
              } else {
                return midA > midB;
              }
            });

  auto plPrintZ = [](const auto &pl) -> std::string {
    std::stringstream ss;
    double zMid = (pl.min(Acts::binZ) + pl.max(Acts::binZ)) / 2.0;
    ss << " < " << pl.min(Acts::binZ) << " | " << zMid << " | "
       << pl.max(Acts::binZ) << " > ";
    return ss.str();
  };
  if (logger().doPrint(Acts::Logging::VERBOSE)) {
    for (const auto &pl : protoLayers) {

      ACTS_VERBOSE(" -> at < zMin | zMid | zMax >: " << plPrintZ(pl));

      ACTS_VERBOSE("    -> at rMin / rMax: " << pl.min(Acts::binR) << " / "
                                             << pl.max(Acts::binR));
    }
  }

  std::vector<Acts::ProtoLayer> mergedProtoLayers;

  if (m_cfg.doEndcapLayerMerging) {
    mergedProtoLayers.push_back(protoLayers.front());
    std::vector<const Acts::Surface *> surfaces;
    for (size_t i = 1; i < protoLayers.size(); i++) {
      auto &pl = protoLayers[i];
      auto &pl_prev = mergedProtoLayers.back();
      // do they intersect?
      ACTS_VERBOSE("Compare: " << plPrintZ(pl_prev) << " and " << plPrintZ(pl));
      bool overlap = (pl.min(Acts::binZ) <= pl_prev.max(Acts::binZ) &&
                      pl.max(Acts::binZ) >= pl_prev.min(Acts::binZ));
      ACTS_VERBOSE(" -> overlap? " << (overlap ? "yes" : "no"));
      if (overlap) {
        ACTS_VERBOSE(" ===> merging");
        surfaces.clear();
        surfaces.reserve(pl.surfaces().size() + pl_prev.surfaces().size());
        surfaces.insert(surfaces.end(), pl.surfaces().begin(),
                        pl.surfaces().end());
        surfaces.insert(surfaces.end(), pl_prev.surfaces().begin(),
                        pl_prev.surfaces().end());
        mergedProtoLayers.pop_back();
        auto &new_pl =
            mergedProtoLayers.emplace_back(gctx, std::move(surfaces));
        new_pl.envelope[Acts::binR] = pl.envelope[Acts::binR];
        new_pl.envelope[Acts::binZ] = pl.envelope[Acts::binZ];
      } else {
        mergedProtoLayers.push_back(std::move(pl));
      }
    }

    ACTS_VERBOSE("" << mergedProtoLayers.size() << " "
                    << (type < 0 ? "NEGATIVE" : "POSITIVE")
                    << " ENDCAP layers remain after merging");
  } else {
    mergedProtoLayers = protoLayers;
  }

  if (m_cfg.objDebugOutput) {
    for (size_t i = 0; i < mergedProtoLayers.size(); i++) {

      std::stringstream ss;
      ss << "obj/" << m_cfg.mode << "_" << (type < 0 ? "neg" : "pos")
         << "_disk_" << std::setfill('0') << std::setw(2) << i << ".obj";

      std::ofstream ofs{ss.str()};
      Acts::ObjVisualization3D vis{};
      Acts::ViewConfig vc = Acts::s_viewSensitive;
      vc.nSegments = 200;
      for (const auto &surface : mergedProtoLayers[i].surfaces()) {
        Acts::GeometryView3D::drawSurface(vis, *surface, gctx,
                                          Acts::Transform3::Identity(), vc);
      }

      vis.write(ofs);
    }
  }

  std::vector<std::shared_ptr<const Surface>> ownedSurfaces;
  for (const auto &pl : mergedProtoLayers) {

    std::unique_ptr<Acts::ApproachDescriptor> approachDescriptor = nullptr;
    std::shared_ptr<const Acts::ProtoSurfaceMaterial> materialProxy = nullptr;

    double layerZ = pl.medium(Acts::binZ);
    double layerHalfZ = 0.5 * pl.range(Acts::binZ);
    double layerThickness = pl.range(Acts::binZ);

    double layerZInner = layerZ - layerHalfZ;
    double layerZOuter = layerZ + layerHalfZ;

    if (std::abs(layerZInner) > std::abs(layerZOuter))
      std::swap(layerZInner, layerZOuter);

    std::vector<std::shared_ptr<const Acts::Surface>> aSurfaces;

    Acts::Transform3 transformNominal(Translation3(0., 0., layerZ));
    Acts::Transform3 transformInner(Translation3(0., 0., layerZInner));
    Acts::Transform3 transformOuter(Translation3(0., 0., layerZOuter));

    std::shared_ptr<Acts::DiscSurface> innerBoundary =
      Acts::Surface::makeShared<Acts::DiscSurface>(
        transformInner, pl.min(Acts::binR), pl.max(Acts::binR));
    aSurfaces.push_back(innerBoundary);

    std::shared_ptr<Acts::DiscSurface> nominalSurface =
      Acts::Surface::makeShared<Acts::DiscSurface>(
        transformNominal, pl.min(Acts::binR), pl.max(Acts::binR));
    aSurfaces.push_back(nominalSurface);

    std::shared_ptr<Acts::DiscSurface> outerBoundary =
      Acts::Surface::makeShared<Acts::DiscSurface>(
        transformOuter, pl.min(Acts::binR), pl.max(Acts::binR));
    aSurfaces.push_back(outerBoundary);

    if(layerThickness > 2_mm) {
      ACTS_VERBOSE("Wide disc layer ("<< layerThickness << ") => adding cylinder like approach surfaces");
      Acts::Transform3 trf{Translation3{0, 0, layerZ}};
      auto cylinderInner = 
        Acts::Surface::makeShared<Acts::CylinderSurface>(
          trf, pl.min(Acts::binR), layerHalfZ);
        aSurfaces.push_back(cylinderInner);

      auto cylinderOuter = 
        Acts::Surface::makeShared<Acts::CylinderSurface>(
          trf, pl.max(Acts::binR), layerHalfZ);
        aSurfaces.push_back(cylinderOuter);
    }


    size_t matBinsPhi = m_cfg.endcapMaterialBins.first;
    size_t matBinsR = m_cfg.endcapMaterialBins.second;

    Acts::BinUtility materialBinUtil(matBinsPhi, -M_PI, M_PI, Acts::closed,
                                     Acts::binPhi);
    materialBinUtil +=
        Acts::BinUtility(matBinsR, pl.min(Acts::binR), pl.max(Acts::binR),
                         Acts::open, Acts::binR, transformNominal);

    materialProxy =
        std::make_shared<const Acts::ProtoSurfaceMaterial>(materialBinUtil);

    ACTS_VERBOSE("[L] Layer is marked to carry support material on Surface ( "
                 "inner=0 / center=1 / outer=2 ) : "
                 << "inner");
    ACTS_VERBOSE("with binning: [" << matBinsPhi << ", " << matBinsR << "]");

    ACTS_VERBOSE("Created ApproachSurfaces for disc layer at:");
    ACTS_VERBOSE(" - inner:   Z=" << layerZInner);
    ACTS_VERBOSE(" - central: Z=" << layerZ);
    ACTS_VERBOSE(" - outer:   Z=" << layerZOuter);

    // set material on inner
    innerBoundary->assignSurfaceMaterial(materialProxy);


    // std::set<int> ringIds;
    bool isITk = m_cfg.mode == Mode::ITkPixelInner ||
                 m_cfg.mode == Mode::ITkPixelOuter ||
                 m_cfg.mode == Mode::ITkStrip;

    std::map<int, std::set<int>> phiModuleByRing;

    // want to figure out bins in phi
    for (const auto &srf : pl.surfaces()) {
      auto elm = dynamic_cast<const ActsDetectorElement *>(
          srf->associatedDetectorElement());
      if (elm) {
        auto id = elm->identityHelper();
        int ring_number;
        if(m_cfg.mode == Mode::ITkStrip || !isITk) {
          ring_number = id.eta_module();
        }
        else {
          ring_number = id.layer_disk();
        }
        phiModuleByRing[ring_number].insert(id.phi_module());
      }
    }
    size_t nModPhi = std::numeric_limits<size_t>::max();
    for(const auto& [ring, phiModules] : phiModuleByRing) {
      nModPhi = std::min(nModPhi, phiModules.size());
    }

    size_t nModR = phiModuleByRing.size();

    ACTS_VERBOSE("Identifier reports: " << nModPhi << " is lowest for " << nModR
                                        << " r-rings");

    size_t nBinsPhi = nModPhi;
    size_t nBinsR = nModR;
    if(!isITk) {
      // In the ID, the modules in the innermost r-rings are exactly shifted by
      // one half module width since it's the same number of modules, this gives
      // binning trouble. Reduce bins by half: about 2 module pairs should be in
      // each bin. This should be fine.
      // @TODO: evaluate
      nBinsPhi /= 2.0;
    }
    if(m_cfg.mode == Mode::ITkStrip) {
      // up to four rows per module
      nBinsR /= 4.0;
    }

    ACTS_VERBOSE("Creating r x phi binned layer with " << nBinsR << " x "
                                                       << nBinsPhi << " bins");


    approachDescriptor =
        std::make_unique<Acts::GenericApproachDescriptor>(aSurfaces);

    // get ownership of pl surfaces
    ownedSurfaces.clear();
    ownedSurfaces.reserve(pl.surfaces().size());
    std::transform(pl.surfaces().begin(), pl.surfaces().end(),
                   std::back_inserter(ownedSurfaces),
                   [](const auto &s) { return s->getSharedPtr(); });

    auto layer = m_cfg.layerCreator->discLayer(gctx, ownedSurfaces, nBinsR,
                                               nBinsPhi, pl, transformNominal,
                                               std::move(approachDescriptor));

    layersOutput.push_back(layer);
  }
}

std::ostream& operator<<(std::ostream& os, const ActsLayerBuilder::Mode& mode) {

  using Mode = ActsLayerBuilder::Mode;
  switch(mode) {
    case Mode::Undefined: 
      os << "Undefined";
      break;
    case Mode::Pixel: 
      os << "Pixel";
      break;
    case Mode::SCT: 
      os << "SCT";
      break;
    case Mode::TRT: 
      os << "TRT";
      break;
    case Mode::ITkPixelInner: 
      os << "ITkPixelInner";
      break;
    case Mode::ITkPixelOuter: 
      os << "ITkPixelOuter";
      break;
    case Mode::ITkStrip: 
      os << "ITkStrip";
      break;
  }

  return os;
}
