/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsCaloTrackingVolumeBuilder.h"
#include "ActsInterop/Logger.h"

#include "StoreGate/ReadHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "Acts/Geometry/TrackingVolume.hpp"
#include "Acts/Geometry/VolumeBounds.hpp"
#include "Acts/Geometry/GlueVolumesDescriptor.hpp"

// ACTS
#include "Acts/Geometry/GenericCuboidVolumeBounds.hpp"
#include "Acts/Geometry/CutoutCylinderVolumeBounds.hpp"
#include "Acts/Geometry/CylinderVolumeBounds.hpp"
//#include "Acts/Utilities/IVisualization.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Geometry/TrackingVolumeArrayCreator.hpp"
#include "Acts/Utilities/BinnedArrayXD.hpp"
#include "Acts/Geometry/CylinderVolumeHelper.hpp"

#include <fstream>

using Box = Acts::Volume::BoundingBox; // shortcut

using CVBBV = Acts::CylinderVolumeBounds::BoundValues;
using CCVBBV = Acts::CutoutCylinderVolumeBounds::BoundValues;

ActsCaloTrackingVolumeBuilder::ActsCaloTrackingVolumeBuilder(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
: base_class(type, name, parent)
{

}

StatusCode
ActsCaloTrackingVolumeBuilder::initialize()
{
  ATH_CHECK( m_caloDDMgrKey.initialize() );
  return StatusCode::SUCCESS;
}

std::shared_ptr<Acts::TrackingVolume>
ActsCaloTrackingVolumeBuilder::trackingVolume(
    const Acts::GeometryContext& gctx,
    std::shared_ptr<const Acts::TrackingVolume> insideVolume,
    std::shared_ptr<const Acts::VolumeBounds> /*outsideBounds*/) const
{


  // generate the calo cell volume description
  std::vector<std::unique_ptr<Acts::AbstractVolume>> cells;
  cells = cellFactory();

  ATH_MSG_VERBOSE("Collected " << cells.size() << " calo cells");


  // we need to turn the cells into boundary boxes
  std::vector<std::unique_ptr<Box>> boxStore;
  std::vector<Box*>                 prims;
  for (const auto& cell : cells) {
    boxStore.push_back(
        std::make_unique<Box>(cell->boundingBox({0.1, 0.1, 0.1})));
    prims.push_back(boxStore.back().get());
  }

  ATH_MSG_VERBOSE("Generated Bounding Boxes");


  ATH_MSG_VERBOSE("Figure out dimensions of wrapping volume");

  std::shared_ptr<Acts::CutoutCylinderVolumeBounds> caloVolBounds
   = makeCaloVolumeBounds(boxStore, insideVolume);

  // build a BVH octree for the bounding boxes
  // but only AFTER we've built the calo volume bounds
  Box* top;
  top = Acts::make_octree(boxStore, prims, 1, 0.1);

  // Create Tracking Volume that coutains the Calo
  // This needs to own the Abstract Volumes (cells), but we
  // need to up-cast them to Volume, since that's what TrackingVolume can own
  std::vector<std::unique_ptr<const Acts::Volume>> cellVols;
  cellVols.reserve(cells.size());
  for(auto& cell : cells) {
    std::unique_ptr<const Acts::Volume> up;
    // release, up-cast, then immediately pack again
    up = std::unique_ptr<const Acts::Volume>(dynamic_cast<const Acts::Volume*>(cell.release()));
    cellVols.push_back(std::move(up));
  }

  std::shared_ptr<Acts::TrackingVolume> calo
      = Acts::TrackingVolume::create(Acts::Transform3D::Identity(),
                                     caloVolBounds,
                                     std::move(boxStore),
                                     std::move(cellVols),
                                     top,
                                     nullptr,  // no material for now
                                     "Calo");

  // We need to interglue all the volumes together
  std::shared_ptr<Acts::TrackingVolume> mutInsideVolume
    = std::const_pointer_cast<Acts::TrackingVolume>(insideVolume);
  auto idBounds = dynamic_cast<const Acts::CylinderVolumeBounds*>(&insideVolume->volumeBounds());

  // we want gap volumes at pos and neg xy face, and at outer cyl cover
  // which will include the solenoid area

  auto trackingVolumeArrayCreator
      = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
          Acts::TrackingVolumeArrayCreator::Config{},
          makeActsAthenaLogger(this, "TrkVolArrCrtr", "ActsTGSvc"));
  Acts::CylinderVolumeHelper::Config cvhCfg;
  cvhCfg.trackingVolumeArrayCreator = trackingVolumeArrayCreator;
  Acts::CylinderVolumeHelper cvh(cvhCfg, makeActsAthenaLogger(this, "ACaloTrkVB", "CylVolHlp"));

  std::vector<double> lPos = {};
  std::vector<std::shared_ptr<Acts::TrackingVolume>> noVolumes;

  ATH_MSG_VERBOSE("Creating gap volume to extend ID");
  // positive xy gap
  auto idGapPosXY = cvh.createGapTrackingVolume(gctx,
                                                noVolumes,
                                                nullptr,
                                                idBounds->get(CVBBV::eMinR),
                                                idBounds->get(CVBBV::eMaxR),
                                                idBounds->get(CVBBV::eHalfLengthZ),
                                                caloVolBounds->get(CCVBBV::eHalfLengthZcutout),
                                                lPos,
                                                false,
                                                "ID::PositiveGap"
                                                );
  // negative xy gap
  auto idGapNegXY = cvh.createGapTrackingVolume(gctx,
                                                noVolumes,
                                                nullptr,
                                                idBounds->get(CVBBV::eMinR),
                                                idBounds->get(CVBBV::eMaxR),
                                                -caloVolBounds->get(CCVBBV::eHalfLengthZcutout),
                                                -idBounds->get(CVBBV::eHalfLengthZ),
                                                lPos,
                                                false,
                                                "ID::NegativeGap"
                                                );
  // outer cover gap
  auto idGapCylOuter = cvh.createGapTrackingVolume(gctx,
                                                   noVolumes,
                                                   nullptr,
                                                   idBounds->get(CVBBV::eMaxR),
                                                   caloVolBounds->get(CCVBBV::eMedR),
                                                   -caloVolBounds->get(CCVBBV::eHalfLengthZcutout),
                                                   +caloVolBounds->get(CCVBBV::eHalfLengthZcutout),
                                                   lPos,
                                                   false,
                                                   "ID::CylOutGap"
                                                   );

  ATH_MSG_VERBOSE("Create container volume to contain ID and gap volumes");
  auto idContainerZ = cvh.createContainerTrackingVolume(gctx, {idGapNegXY, mutInsideVolume, idGapPosXY});
  auto idContainer = cvh.createContainerTrackingVolume(gctx, {idContainerZ, idGapCylOuter});


  ATH_MSG_VERBOSE("Begin volume glueing");

  const Acts::GlueVolumesDescriptor& gvd
    = idContainer->glueVolumesDescriptor();
  // let's see what the GVD says is on the inner cover of the ID
  const auto& tVolArrPos = gvd.glueVolumes(Acts::positiveFaceXY);
  const auto& tVolArrNeg = gvd.glueVolumes(Acts::negativeFaceXY);

  std::cout << "POSITIVE: " << std::endl;
  for(const auto& subvol : tVolArrPos->arrayObjects()) {
    std::cout << subvol->volumeName() << std::endl;
    std::cout << *subvol << std::endl;
  }

  std::cout << "NEGATIVE: " << std::endl;
  for(const auto& subvol : tVolArrNeg->arrayObjects()) {
    std::cout << subvol->volumeName() << std::endl;
    std::cout << *subvol << std::endl;
  }

  using BoundarySurface = Acts::BoundarySurfaceT<Acts::TrackingVolume>;


  // Glue outer radial cover of ID to inner cover of Calo cutout
  auto idOutVolArray = gvd.glueVolumes(Acts::tubeOuterCover);
  // Attach that volume array to the calo inner cover
  ATH_MSG_VERBOSE("Glueing " << calo->volumeName() << " inner cover to " << idOutVolArray->arrayObjects().size() << " volumes");
  std::const_pointer_cast<BoundarySurface>(calo->boundarySurfaces().at(Acts::tubeInnerCover))
    ->attachVolumeArray(idOutVolArray, Acts::backward);
  // Loop through the array and attach their boundary surfaces to the calo
  for(const auto& idVol : idOutVolArray->arrayObjects()){
    ATH_MSG_VERBOSE("Glueing outer cover of " << idVol->volumeName()
    << " to inner cover of " << calo->volumeName());
    std::const_pointer_cast<BoundarySurface>(idVol->boundarySurfaces().at(Acts::tubeOuterCover))
      ->attachVolume(calo.get(), Acts::forward);
  }

  // Glue positive XY face of ID to inner positive XY face of Calo.
  // ID has multiple, Calo has only one
  auto idPosXYVolArray = gvd.glueVolumes(Acts::positiveFaceXY);
  ATH_MSG_VERBOSE("Glueing " << calo->volumeName() << " positive inner cutout disc to "
      << idPosXYVolArray->arrayObjects().size() << " volumes");
  std::const_pointer_cast<BoundarySurface>(calo->boundarySurfaces().at(Acts::index5))
    ->attachVolumeArray(idPosXYVolArray, Acts::backward);
  // Other way round, attach ID volumes to calo
  for(const auto& idVol : idPosXYVolArray->arrayObjects()){
    ATH_MSG_VERBOSE("Glueing positive XY face of " << idVol->volumeName()
    << " to positive inner coutout disc of " << calo->volumeName());
    std::const_pointer_cast<BoundarySurface>(idVol->boundarySurfaces().at(Acts::positiveFaceXY))
      ->attachVolume(calo.get(), Acts::forward);
  }

  // Glue negative XY face of ID to inner negative XY face of Calo.
  // ID has multiple, Calo has only one
  auto idNegXYVolArray = gvd.glueVolumes(Acts::negativeFaceXY);
  ATH_MSG_VERBOSE("Glueing " << calo->volumeName() << " negative inner cutout disc to "
      << idNegXYVolArray->arrayObjects().size() << " volumes");
  std::const_pointer_cast<BoundarySurface>(calo->boundarySurfaces().at(Acts::index4))
    ->attachVolumeArray(idNegXYVolArray, Acts::forward);
  // Other way round, attach ID volumes to calo
  for(const auto& idVol : idNegXYVolArray->arrayObjects()){
    ATH_MSG_VERBOSE("Glueing negative XY face of " << idVol->volumeName()
    << " to negative inner coutout disc of " << calo->volumeName());
    std::const_pointer_cast<BoundarySurface>(idVol->boundarySurfaces().at(Acts::negativeFaceXY))
      ->attachVolume(calo.get(), Acts::backward);
  }

  // For navigational purposes we need to create three pseudo container cylinders.
  // One for the middle, which includes the central part of the Calo and the ID, and
  // two that fit cleanly at the +- XY face that then only include the Calo

  // Construct track vol array for use in positive and negative pseudocontainer.
  // This will only contain the calo

  double caloRMin = caloVolBounds->get(CCVBBV::eMinR);
  double caloRMed = caloVolBounds->get(CCVBBV::eMedR);
  double caloRMax = caloVolBounds->get(CCVBBV::eMaxR);
  double caloDZ1 = caloVolBounds->get(CCVBBV::eHalfLengthZ);
  double caloDZ2 = caloVolBounds->get(CCVBBV::eHalfLengthZcutout);

  Acts::Vector3D caloChokeRPos
    = {caloRMin + (caloRMax - caloRMin)/2., 0, 0};

  std::vector<Acts::TrackingVolumeOrderPosition> tVolOrdPosNeg;
  tVolOrdPosNeg.push_back(std::make_pair(calo, caloChokeRPos));
  std::vector<float> posNegBoundaries
   = {float(caloRMin), float(caloRMax)};
  auto binUtilityPosNeg = std::make_unique<const Acts::BinUtility>(posNegBoundaries,
      Acts::open, Acts::binR);

  auto tVolArrPosNeg
      = std::make_shared<const Acts::BinnedArrayXD<Acts::TrackingVolumePtr>>(
          tVolOrdPosNeg, std::move(binUtilityPosNeg));

  double chokeZOffset = caloDZ2 + (caloDZ1 - caloDZ2)/2.;
  Acts::Transform3D posTrf(Acts::Translation3D(Acts::Vector3D::UnitZ() * chokeZOffset));
  Acts::Transform3D negTrf(Acts::Translation3D(Acts::Vector3D::UnitZ()* -1 *chokeZOffset));

  auto posNegCylBounds = std::make_shared<Acts::CylinderVolumeBounds>(
       caloRMin, caloRMax, (caloDZ1 - caloDZ2) / 2.);

  // they share the same bounds and tvol array
  auto posContainer = Acts::TrackingVolume::create(posTrf, posNegCylBounds, tVolArrPosNeg);
  ATH_MSG_VERBOSE("Built positive container " << *posContainer);
  ATH_MSG_VERBOSE(" - containing: " << calo->volumeName());
  auto negContainer = Acts::TrackingVolume::create(negTrf, posNegCylBounds, tVolArrPosNeg);
  ATH_MSG_VERBOSE("Built negative container " << *negContainer);
  ATH_MSG_VERBOSE(" - containing: " << calo->volumeName());

  // now build the central pseudocontainer
  std::vector<Acts::TrackingVolumeOrderPosition> tVolOrderedCtr;
  tVolOrderedCtr.push_back(std::make_pair(idContainer, Acts::Vector3D(caloRMed / 2., 0, 0)));
  tVolOrderedCtr.push_back(std::make_pair(calo,
        Acts::Vector3D(caloRMed +
          (caloRMax- caloRMed) / 2., 0, 0)));

  std::vector<float> ctrBoundaries = {0, float(caloRMed), float(caloRMax)};
  auto binUtilityCtr
   = std::make_unique<const Acts::BinUtility>(
      ctrBoundaries,
      Acts::open, Acts::binR);

  auto tVolArrCtr
      = std::make_shared<const Acts::BinnedArrayXD<Acts::TrackingVolumePtr>>(
          tVolOrderedCtr, std::move(binUtilityCtr));

auto ctrContainer = Acts::TrackingVolume::create(Acts::Transform3D::Identity(),
                        std::make_shared<Acts::CylinderVolumeBounds>(
                        caloRMin, caloRMax, caloDZ2),
                        tVolArrCtr);
  ATH_MSG_VERBOSE("Built central container " << *ctrContainer);
  ATH_MSG_VERBOSE("- containing: " << idContainer->volumeName() << ", " << calo->volumeName());

  // and now combine those together into another one
  Acts::TrackingVolumeArrayCreator tvac{Acts::TrackingVolumeArrayCreator::Config{}};

auto mainContainer = Acts::TrackingVolume::create(Acts::Transform3D::Identity(),
    std::make_shared<Acts::CylinderVolumeBounds>(
    caloRMin, caloRMax, caloDZ1),
    tvac.trackingVolumeArray(gctx, {negContainer, ctrContainer, posContainer},
    Acts::binZ));

  ATH_MSG_VERBOSE("Built main container: " << *mainContainer);

  return mainContainer;
}

std::shared_ptr<Acts::CutoutCylinderVolumeBounds>
ActsCaloTrackingVolumeBuilder::makeCaloVolumeBounds(const std::vector<std::unique_ptr<Box>>& boxStore,
                     std::shared_ptr<const Acts::TrackingVolume> insideVolume) const
{
  using namespace Acts::VectorHelpers; 
  // determine the dimensions of the
  double rmin_at_center = std::numeric_limits<double>::max();
  double rmin_at_choke  = std::numeric_limits<double>::max();
  double rmax           = std::numeric_limits<double>::lowest();
  double zmin           = std::numeric_limits<double>::max();
  double zmax           = std::numeric_limits<double>::lowest();
  double cutout_zmin_abs = std::numeric_limits<double>::max();

  // We need to figure out what the size of the inner cutout cylinder is
  // so we can make sure everything worked fine!
  // We check what the min radius at small z, and then we turn it around and
  // check z bounds at lower radii.
  for (const auto& box : boxStore) {
    Acts::Vector3D vmin = box->min().cast<double>();
    Acts::Vector3D vmax = box->max().cast<double>();

    double vrmin = perp(vmin);
    double vrmax = perp(vmax);

    rmin_at_choke = std::min(rmin_at_choke, std::min(vrmin, vrmax));

    rmax = std::max(rmax, std::max(vrmin, vrmax));
    zmin = std::min(zmin, std::min(vmin.z(), vmax.z()));
    zmax = std::max(zmax, std::max(vmin.z(), vmax.z()));

    if (std::abs(vmin.z()) < 100) {
      rmin_at_center = std::min(vrmin, rmin_at_center);
    }
    if (std::abs(vmax.z()) < 100) {
      rmin_at_center = std::min(vrmax, rmin_at_center);
    }
  }

  for (const auto& box : boxStore) {
    Acts::Vector3D vmin  = box->min().cast<double>();
    Acts::Vector3D vmax  = box->max().cast<double>();
    double         vrmin = perp(vmin);
    double         vrmax = perp(vmax);

    if (vrmin < rmin_at_center * 0.9) {
      cutout_zmin_abs = std::min(cutout_zmin_abs, std::abs(vmin.z()));
    }
    if (vrmax < rmin_at_center * 0.9) {
      cutout_zmin_abs = std::min(cutout_zmin_abs, std::abs(vmax.z()));
    }
  }

  double dz1 = (zmax - zmin) / 2.;
  double dz2 = cutout_zmin_abs;

  // envelopes
  double envZ = 5;
  double envR = 5;
  dz1 += envZ;
  dz2 -= envZ;
  rmax += envR;
  if(rmin_at_choke > envR) rmin_at_choke -= envR;
  rmin_at_center -= envR;


  ATH_MSG_VERBOSE("rmin_at_center: " << rmin_at_center
                  << " rmin at choke: " << rmin_at_choke
                  << " rmax: " << rmax << " zmin: " << zmin << " zmax: " << zmax
                  << " coutout_zmin_abs: " << cutout_zmin_abs);

  // Ok now let's analyse what we're wrapping the calo around: the ID
  // The ID will have to be built already.

  // We need to figure out the dimensions of the ID.
  // Assuming the wrapping volume is a cylinder.
  auto idCylBds
    = dynamic_cast<const Acts::CylinderVolumeBounds*>(&insideVolume->volumeBounds());

  double idRMax = idCylBds->get(CVBBV::eMaxR);
  double idRMin = idCylBds->get(CVBBV::eMinR);
  double idHlZ = idCylBds->get(CVBBV::eHalfLengthZ);

  ATH_MSG_VERBOSE("ID volume bounds:\n" << *idCylBds);

  ATH_MSG_VERBOSE("Inside volume transform: \n" << insideVolume->transform().matrix());

  if (!insideVolume->transform().isApprox(Acts::Transform3D::Identity())) {
    ATH_MSG_VERBOSE("Inside volume transform is not unity.");
    
    // transformation matrix is NOT unity. Let's check:
    // - Rotation is approximate unity
    // - Translation is only along z axis
    const auto& trf = insideVolume->transform();
  
    Acts::RotationMatrix3D rot = trf.rotation();
    bool unityRot = rot.isApprox(Acts::RotationMatrix3D::Identity());

    ATH_MSG_VERBOSE("\n" << rot);

    // dot product with Z axis is about 1 => ok
    const Acts::Vector3D trl = trf.translation();
    bool transZOnly = std::abs(1 - std::abs(Acts::Vector3D::UnitZ().dot(trl.normalized()))) < 1e-6;

    ATH_MSG_VERBOSE("TRL "<< trl.transpose());
    ATH_MSG_VERBOSE("TRL "<< trl.normalized().dot(Acts::Vector3D::UnitZ()));

    if(!unityRot || !transZOnly) {
      ATH_MSG_ERROR("The ID appears to be shifted from the origin. I cannot handle this.");
      ATH_MSG_ERROR("(I'm not building the Calo!)");
      throw std::runtime_error("Error building calo");
    }
    else {
      ATH_MSG_VERBOSE("Checked: non unitarity is ONLY due to shift along z axis: that's ok");
      double prevIdHlZ = idHlZ;
      idHlZ += std::abs(trl.z());
      ATH_MSG_VERBOSE("Modifying effective half length of ID cylinder: " << prevIdHlZ << " => " << idHlZ);
    }
  }

  // make sure we can fit the ID inside the calo cutout
  if (idRMax > rmin_at_center || idHlZ > dz2 || (idRMin > rmin_at_choke && idRMin != 0.)) {
    ATH_MSG_ERROR("Cannot fit ID inside the Calo");
    ATH_MSG_ERROR("This can be because the ID overlaps into the calo volume");
    ATH_MSG_ERROR("Or because the Calo choke radius is SMALLER than the ID inner radius");
    ATH_MSG_ERROR("Currently, I can only make the choke radius smaller, I can not make it larger");
    ATH_MSG_ERROR("nor can I manipulate the ID volume bounds at this point.");
    ATH_MSG_ERROR("ID rMax: " << idRMax << " Calo rMin@center: " << rmin_at_center);
    ATH_MSG_ERROR("ID hlZ: " << idHlZ << " Calo inner Z hl: " << dz2);
    ATH_MSG_ERROR("ID rMin: " << idRMin << " Calo rMin@choke: " << rmin_at_choke);
    ATH_MSG_ERROR("(I'm not building the Calo!)");
    throw std::runtime_error("Error building calo");
  }

  // Let's harmonize the sizes, so we have a exact wrap of the ID
  // Choke is now exactly as wide as space inside the ID.
  // We can fit the beam pipe in there later.
  rmin_at_choke = idRMin;

  std::shared_ptr<Acts::CutoutCylinderVolumeBounds> volBds = nullptr;
  volBds = std::make_shared<Acts::CutoutCylinderVolumeBounds>(
      rmin_at_choke, rmin_at_center, rmax, dz1, dz2);

  ATH_MSG_VERBOSE(*volBds);

  return volBds;
}


namespace {
  double
  eta_to_theta(double eta)
  {
    return 2 * std::atan(std::exp(-eta));
  }
}

Acts::AbstractVolume
ActsCaloTrackingVolumeBuilder::build_endcap(double z,
             double dz,
             double eta,
             double deta,
             double phi,
             double dphi) const
{
  double eta_max   = eta + deta * 0.5;
  double eta_min   = eta - deta * 0.5;
  double theta_max = eta_to_theta(eta_max);
  double theta     = eta_to_theta(eta);
  double theta_min = eta_to_theta(eta_min);
  double phi_max   = phi + dphi * 0.5;
  double phi_min   = phi - dphi * 0.5;
  double z_min     = z - dz;
  double z_max     = z + dz;

  double         r_min, r_max;
  Acts::Vector3D p1, p2, p3, p4, p5, p6, p7, p8;

  // inner face
  r_min = std::tan(theta_min) * z_min;
  r_max = std::tan(theta_max) * z_min;

  p1 << r_min * std::cos(phi_min), r_min * std::sin(phi_min), z_min;
  p2 << r_min * std::cos(phi_max), r_min * std::sin(phi_max), z_min;
  p3 << r_max * std::cos(phi_max), r_max * std::sin(phi_max), z_min;
  p4 << r_max * std::cos(phi_min), r_max * std::sin(phi_min), z_min;

  // outer face
  r_min = std::tan(theta_min) * z_max;
  r_max = std::tan(theta_max) * z_max;

  p5 << r_min * std::cos(phi_min), r_min * std::sin(phi_min), z_max;
  p6 << r_min * std::cos(phi_max), r_min * std::sin(phi_max), z_max;
  p7 << r_max * std::cos(phi_max), r_max * std::sin(phi_max), z_max;
  p8 << r_max * std::cos(phi_min), r_max * std::sin(phi_min), z_max;

  double         r_mid = std::tan(theta) * z_min;
  Acts::Vector3D center;
  center.x() = r_mid * std::cos(phi);
  center.y() = r_mid * std::sin(phi);
  center.z() = z;

  Acts::Transform3D glob2vol = Acts::Transform3D::Identity();
  glob2vol *= Acts::AngleAxis3D(-phi, Acts::Vector3D::UnitZ());
  glob2vol *= Acts::AngleAxis3D(
      -theta, Acts::Vector3D::UnitZ().cross(center).normalized());
  glob2vol
      *= Acts::Translation3D(-(p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8) / 8.);

  p1 = glob2vol * p1;
  p2 = glob2vol * p2;
  p3 = glob2vol * p3;
  p4 = glob2vol * p4;
  p5 = glob2vol * p5;
  p6 = glob2vol * p6;
  p7 = glob2vol * p7;
  p8 = glob2vol * p8;

  auto globalToLocal = glob2vol.inverse();

  auto cubo = std::make_shared<Acts::GenericCuboidVolumeBounds>(
      std::array<Acts::Vector3D, 8>({{p1, p2, p3, p4, p5, p6, p7, p8}}));
  Acts::AbstractVolume vol(globalToLocal, std::move(cubo));

  return vol;
}


Acts::AbstractVolume
ActsCaloTrackingVolumeBuilder::build_barrel(double r,
             double dr,
             double eta,
             double deta,
             double phi,
             double dphi) const
{
  // std::cout << "build barrel" << std::endl;
  double eta_max   = eta + deta * 0.5;
  double eta_min   = eta - deta * 0.5;
  double theta     = eta_to_theta(eta);
  double theta_max = eta_to_theta(eta_max);
  double theta_min = eta_to_theta(eta_min);
  double phi_max   = phi + dphi * 0.5;
  double phi_min   = phi - dphi * 0.5;

  double r_min = r - dr;
  double r_max = r + dr;

  double         z_min, z_max;
  Acts::Vector3D p1, p2, p3, p4, p5, p6, p7, p8;

  // inner face
  z_min = r_min / std::tan(theta_min);
  z_max = r_min / std::tan(theta_max);

  p1 << r_min * std::cos(phi_min), r_min * std::sin(phi_min), z_min;
  p2 << r_min * std::cos(phi_min), r_min * std::sin(phi_min), z_max;
  p3 << r_min * std::cos(phi_max), r_min * std::sin(phi_max), z_max;
  p4 << r_min * std::cos(phi_max), r_min * std::sin(phi_max), z_min;

  // outer face
  z_min = r_max / std::tan(theta_min);
  z_max = r_max / std::tan(theta_max);

  p5 << r_max * std::cos(phi_min), r_max * std::sin(phi_min), z_min;
  p6 << r_max * std::cos(phi_min), r_max * std::sin(phi_min), z_max;
  p7 << r_max * std::cos(phi_max), r_max * std::sin(phi_max), z_max;
  p8 << r_max * std::cos(phi_max), r_max * std::sin(phi_max), z_min;

  Acts::Vector3D center;
  center.x() = r * std::cos(phi);
  center.y() = r * std::sin(phi);
  center.z() = r / std::tan(theta);

  Acts::Transform3D glob2vol = Acts::Transform3D::Identity();
  glob2vol *= Acts::AngleAxis3D(-phi, Acts::Vector3D::UnitZ());
  glob2vol *= Acts::AngleAxis3D(
      -theta, Acts::Vector3D::UnitZ().cross(center).normalized());
  glob2vol
      *= Acts::Translation3D(-(p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8) / 8.);

  p1 = glob2vol * p1;
  p2 = glob2vol * p2;
  p3 = glob2vol * p3;
  p4 = glob2vol * p4;
  p5 = glob2vol * p5;
  p6 = glob2vol * p6;
  p7 = glob2vol * p7;
  p8 = glob2vol * p8;

  auto globalToLocal = glob2vol.inverse();

  auto cubo = std::make_shared<Acts::GenericCuboidVolumeBounds>(
      std::array<Acts::Vector3D, 8>({{p1, p2, p3, p4, p5, p6, p7, p8}}));

  Acts::AbstractVolume vol(globalToLocal, std::move(cubo));

  return vol;
}

Acts::AbstractVolume
ActsCaloTrackingVolumeBuilder::build_box(double x, double dx, double y, double dy, double z, double dz) const
{
  // std::cout << "build box" << std::endl;

  double x_min, x_max, y_min, y_max, z_min, z_max;
  x_min = x - dx;
  x_max = x + dx;
  y_min = y - dy;
  y_max = y + dy;
  z_min = z - dz;
  z_max = z + dz;

  Acts::Vector3D p1, p2, p3, p4, p5, p6, p7, p8;

  // inner face
  p1 << x_min, y_min, z_min;
  p2 << x_min, y_max, z_min;
  p3 << x_max, y_max, z_min;
  p4 << x_max, y_min, z_min;

  // outer face
  p5 << x_min, y_min, z_max;
  p6 << x_min, y_max, z_max;
  p7 << x_max, y_max, z_max;
  p8 << x_max, y_min, z_max;

  Acts::Transform3D glob2vol = Acts::Transform3D::Identity();
  glob2vol
      *= Acts::Translation3D(-(p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8) / 8.);

  p1 = glob2vol * p1;
  p2 = glob2vol * p2;
  p3 = glob2vol * p3;
  p4 = glob2vol * p4;
  p5 = glob2vol * p5;
  p6 = glob2vol * p6;
  p7 = glob2vol * p7;
  p8 = glob2vol * p8;

  auto globalToLocal = glob2vol.inverse();

  auto cubo = std::make_shared<Acts::GenericCuboidVolumeBounds>(
      std::array<Acts::Vector3D, 8>({{p1, p2, p3, p4, p5, p6, p7, p8}}));
  Acts::AbstractVolume vol(globalToLocal, std::move(cubo));

  return vol;
}

std::vector<std::unique_ptr<Acts::AbstractVolume>>
ActsCaloTrackingVolumeBuilder::cellFactory() const
{
  //Acts::ply_helper<double> ply_lar;
  //Acts::ply_helper<double> ply_tile;
  //Acts::ply_helper<double> ply_fcal;

  //float  x, y, z, r, phi_raw, eta_raw, dphi, deta, dr, dx, dy, dz;
  float z, dz, eta_raw, deta, phi_raw, dphi, r, dr, x, y, dx, dy;
  size_t calosample;
  float  scale;

  SG::ReadHandle<CaloDetDescrManager> caloMgr(m_caloDDMgrKey);

  // storage of cells we will produce
  std::vector<std::unique_ptr<Acts::AbstractVolume>> cells;
  cells.reserve(caloMgr->element_size());  // about 180k

  for(auto it = caloMgr->element_begin();it < caloMgr->element_end();++it) {
    const CaloDetDescrElement* cde = *it;

    z = cde->z();
    dz = cde->dz();
    eta_raw = cde->eta_raw();
    deta = cde->deta();
    phi_raw = cde->phi_raw();
    dphi = cde->dphi();
    r = cde->r();
    dr = cde->dr();
    x = cde->x();
    y = cde->y();
    dx = cde->dx();
    dy = cde->dy();

    calosample = cde->getSampling();

    scale = 1.;
    if (calosample >= 12 && calosample <= 20) {
      scale = 0.5;
    }

    //Acts::ply_helper<double>* ply;
    //if (calosample <= 11) {
      //ply = &ply_lar;
    //} else if (calosample <= 20) {
      //ply = &ply_tile;
    //} else {
      //ply = &ply_fcal;
    //}


    switch (calosample) {
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 17:
      dz *= scale;
      cells.push_back(std::make_unique<Acts::AbstractVolume>(
          build_endcap(z, dz, eta_raw, deta, phi_raw, dphi)));
      break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 18:
    case 19:
    case 20:
      dr *= scale;
      cells.push_back(std::make_unique<Acts::AbstractVolume>(
          build_barrel(r, dr, eta_raw, deta, phi_raw, dphi)));
      break;
    case 21:
    case 22:
    case 23:
      scale = 1.;
      dx *= scale;
      dy *= scale;
      // dz *= scale;
      cells.push_back(std::make_unique<Acts::AbstractVolume>(
          build_box(x, dx, y, dy, z, dz)));
      break;
    default:
      std::stringstream ss;
      ss << "Unkown calo sample " << calosample;
      std::runtime_error(ss.str());
    }

     //cells.back()->boundingBox({0.1, 0.1, 0.1}).draw(*ply);
     //auto cvb = dynamic_cast<const
     //Acts::GenericCuboidVolumeBounds*>(&cells.back()->volumeBounds());
     //cvb->draw(*ply, cells.back()->transform());
  }

   //std::ofstream os("lar.ply");
   //os << ply_lar << std::flush;
   //os.close();

   //os = std::ofstream("tile.ply");
   //os << ply_tile << std::flush;
   //os.close();

   //os = std::ofstream("fcal.ply");
   //os << ply_fcal << std::flush;
   //os.close();

  return cells;
}
