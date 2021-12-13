/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeArrayCreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include <cmath>

#include "TrkDetDescrTools/TrackingVolumeArrayCreator.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/BinnedArray2D.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/NavBinnedArray1D.h"
#include "TrkVolumes/BevelledCylinderVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/DoubleTrapezoidVolumeBounds.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

// constructor
Trk::TrackingVolumeArrayCreator::TrackingVolumeArrayCreator(
  const std::string& t,
  const std::string& n,
  const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<ITrackingVolumeArrayCreator>(this);
}

// destructor
Trk::TrackingVolumeArrayCreator::~TrackingVolumeArrayCreator() {}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::cylinderVolumesArrayInR(
  const std::vector<const Trk::TrackingVolume*>& vols,
  bool navtype) const
{
  ATH_MSG_VERBOSE("Create VolumeArray of "
                  << vols.size()
                  << " Volumes (with CylinderVolumeBounds) with R-binning. ");

  // check for compatibility - needs r-sorting first
  double lastZmin = 0.;
  double lastZmax = 0.;
  double lastOuterRadius = 0.;

  // the vector of doubles for identification
  std::vector<float> boundaries;
  boundaries.reserve(vols.size() + 1);

  // the vector needed for the BinnedArray
  std::vector<Trk::TrackingVolumeOrderPosition> volOrder;
  // loop over volumes and fill primaries
  std::vector<const Trk::TrackingVolume*>::const_iterator volIter =
    vols.begin();
  for (unsigned int ivol = 0; volIter != vols.end(); ++volIter, ++ivol) {
    const Trk::CylinderVolumeBounds* currentCylBounds = nullptr;
    if (*volIter)
      currentCylBounds = dynamic_cast<const Trk::CylinderVolumeBounds*>(
        &((*volIter)->volumeBounds()));
    if (!currentCylBounds) {
      ATH_MSG_ERROR("Given TrackingVolume doesn't exist or is not of shape "
                    "'CylinderVolumeBounds': return 0");
      return nullptr;
    }
    // current rmin/rmax
    double currentRmin = currentCylBounds->innerRadius();
    double currentRmax = currentCylBounds->outerRadius();
    if (!ivol)
      boundaries.push_back(currentRmin);
    boundaries.push_back(currentRmax);

    // compatibility checks
    double currentZmin =
      (*volIter)->center().z() - currentCylBounds->halflengthZ();
    double currentZmax =
      (*volIter)->center().z() + currentCylBounds->halflengthZ();

    // check for compatibility of the new volume - not for navigation type
    if (ivol && !navtype) {
      // longitudinal clinch
      if (fabs(currentZmin - lastZmin) > 0.1 ||
          fabs(currentZmax - lastZmax) > 0.1) {
        ATH_MSG_ERROR("Given TrackingVolume(s) do not extend in z to the same "
                      "point (required) : return 0");
        ATH_MSG_VERBOSE("Information : lastZmin / lastZmin    = "
                        << lastZmin << " / " << currentZmin);
        ATH_MSG_VERBOSE("              lastZmax / currentZmax = "
                        << lastZmax << " / " << currentZmax);
        return nullptr;
      }
      // radial clinch
      if (fabs(currentRmin - lastOuterRadius) > 0.1) {
        ATH_MSG_ERROR("Given TrackingVolume(s) are not wrapping, neither "
                      "inside-out, nor v.v. : return 0");
        ATH_MSG_VERBOSE("Information : currentRmin / lastOuterRadius = "
                        << currentRmin << " / " << lastOuterRadius);
        return nullptr;
      }
    }
    // register for next round
    lastZmin = currentZmin;
    lastZmax = currentZmax;
    lastOuterRadius = currentRmax;
    // output
    ATH_MSG_VERBOSE("Adding Volume '" << (*volIter)->volumeName()
                                      << "' to Array");
    // push back the volume order position
    if (!navtype) {
      volOrder.emplace_back(
        Trk::SharedObject<const TrackingVolume>((*volIter)),
        Amg::Vector3D(currentCylBounds->mediumRadius(), 0.0, 0.0));
    } else {
      volOrder.emplace_back(
        Trk::SharedObject<const TrackingVolume>(
          (*volIter), do_not_delete<const TrackingVolume>),
        Amg::Vector3D(currentCylBounds->mediumRadius(), 0.0, 0.0));
    }
  }
  if (!volOrder.empty()) {
    Trk::BinUtility* volBinUtilR =
      new Trk::BinUtility(boundaries, Trk::open, Trk::binR);
    ATH_MSG_VERBOSE("Return created Array. ");
    return new Trk::BinnedArray1D<Trk::TrackingVolume>(volOrder, volBinUtilR);
  }
  ATH_MSG_ERROR(
    "No TrackingVolumes provided to the TrackingVolumeArrayCreator: return 0");
  return nullptr;
}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::cylinderVolumesArrayInZ(
  const std::vector<const Trk::TrackingVolume*>& vols,
  bool navtype) const
{
  ATH_MSG_VERBOSE("Create VolumeArray of "
                  << vols.size()
                  << " Volumes (with CylinderVolumeBounds) with Z-binning. ");

  // for compatibility checks
  double lastRmin = 0.;
  double lastRmax = 0.;
  double lastZmax = 0.;

  // the vector of doubles for identification
  std::vector<float> boundaries;
  boundaries.reserve(vols.size() + 1);

  // the vector needed for the BinnedArray
  std::vector<Trk::TrackingVolumeOrderPosition> volOrder;
  // loop over volumes and fill primaries
  std::vector<const Trk::TrackingVolume*>::const_iterator volIter =
    vols.begin();
  for (unsigned int ivol = 0; volIter != vols.end(); ++volIter, ++ivol) {
    const Trk::CylinderVolumeBounds* currentCylBounds = nullptr;
    if (*volIter)
      currentCylBounds = dynamic_cast<const Trk::CylinderVolumeBounds*>(
        &((*volIter)->volumeBounds()));
    if (!currentCylBounds) {
      ATH_MSG_ERROR("Given TrackingVolume doesn't exist or is not of shape "
                    "'CylinderVolumeBounds': return 0");
      return nullptr;
    }
    //
    const Amg::Vector3D& volCenter = (*volIter)->center();
    double halflengthZ = currentCylBounds->halflengthZ();
    // get the numbers
    double currentZmin = volCenter.z() - halflengthZ;
    double currentZmax = volCenter.z() + halflengthZ;
    if (!ivol)
      boundaries.push_back(currentZmin);
    boundaries.push_back(currentZmax);

    // consistency checks
    double currentRmin = currentCylBounds->innerRadius();
    double currentRmax = currentCylBounds->outerRadius();

    // compatibility check - not for navtype
    if (ivol && !navtype) {
      // first the radial check
      if (fabs(lastRmin - currentRmin) > 0.1 ||
          fabs(lastRmax - currentRmax) > 0.1) {
        ATH_MSG_ERROR("Given TrackingVolume(s) do not have same radial extends "
                      "(required): return 0");
        ATH_MSG_VERBOSE("Information : lastRmin / currentRmin = "
                        << lastRmin << " / " << currentRmin);
        ATH_MSG_VERBOSE("              lastRmax / currentRmax = "
                        << lastRmax << " / " << currentRmax);
        return nullptr;
      }
      // then let's see whether they leave gaps in z
      if (fabs(lastZmax - currentZmin) > 0.1) {
        ATH_MSG_ERROR("Given TrackingVolume(s) are not attaching in z "
                      "(required) : return 0");
        return nullptr;
      }
    }
    // for the next round
    lastRmin = currentRmin;
    lastRmax = currentRmax;
    lastZmax = currentZmax;
    // output
    ATH_MSG_VERBOSE("Adding Volume '" << (*volIter)->volumeName()
                                      << "' to Array");
    // push back the volume order position
    if (!navtype) {
      volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>((*volIter)),
                            Amg::Vector3D((*volIter)->center()));
    } else {
      volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>(
                              (*volIter), do_not_delete<const TrackingVolume>),
                            Amg::Vector3D((*volIter)->center()));
    }
  }
  if (!volOrder.empty()) {
    Trk::BinUtility* volBinUtil =
      new Trk::BinUtility(boundaries, Trk::open, Trk::binZ);
    ATH_MSG_VERBOSE("Return created Array. ");
    return new Trk::BinnedArray1D<Trk::TrackingVolume>(volOrder, volBinUtil);
  }
  ATH_MSG_ERROR(
    "No TrackingVolumes provided to the TrackingVolumeArrayCreator: return 0");
  return nullptr;
}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::cylinderVolumesArrayInPhi(
  const std::vector<const Trk::TrackingVolume*>& vols,
  bool navtype) const
{

  ATH_MSG_VERBOSE("Create VolumeArray of "
                  << vols.size()
                  << " Volumes (with CylinderVolumeBounds) with Phi-binning. ");

  // phi binning; assume equidistant
  int nPhiBins = !vols.empty() ? vols.size() : 1;
  double phi = M_PI;
  // the vector needed for the BinnedArray
  std::vector<Trk::TrackingVolumeOrderPosition> volOrder;
  // loop over volumes and fill primaries
  std::vector<const Trk::TrackingVolume*>::const_iterator volIter =
    vols.begin();
  for (; volIter != vols.end(); ++volIter) {
    const Trk::CylinderVolumeBounds* cyl = nullptr;
    if (*volIter)
      cyl = dynamic_cast<const Trk::CylinderVolumeBounds*>(
        &((*volIter)->volumeBounds()));
    if (!cyl) {
      ATH_MSG_ERROR("Given TrackingVolume doesn't exist or is not of shape "
                    "'CylinderVolumeBounds': return 0");
      return nullptr;
    }
    // output
    ATH_MSG_VERBOSE("Adding Volume '" << (*volIter)->volumeName()
                                      << "' to Array");
    // push back the volume order position
    Amg::Vector3D gp(cyl->mediumRadius(), 0., 0.);
    if (!navtype) {
      volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>((*volIter)),
                            Amg::Vector3D(((*volIter)->transform()) * gp));
    } else {
      volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>(
                              (*volIter), do_not_delete<const TrackingVolume>),
                            Amg::Vector3D(((*volIter)->transform()) * gp));
    }
  }
  if (!volOrder.empty()) {
    Trk::BinUtility* volBinUtil =
      new Trk::BinUtility(nPhiBins, -phi, +phi, Trk::closed, Trk::binPhi);
    return new Trk::BinnedArray1D<Trk::TrackingVolume>(volOrder, volBinUtil);
  }
  ATH_MSG_ERROR(
    "No TrackingVolumes provided to the TrackingVolumeArrayCreator: return 0");
  return nullptr;
}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::cylinderVolumesArrayInPhiR(
  const std::vector<const Trk::TrackingVolume*>& vols,
  bool navtype) const
{
  if (vols.empty())
    return nullptr;

  bool bevelled = false;
  for (unsigned int i = 0; i < vols.size(); ++i) {
    const Trk::BevelledCylinderVolumeBounds* bcyl =
      dynamic_cast<const Trk::BevelledCylinderVolumeBounds*>(
        &(vols[i]->volumeBounds()));
    if (bcyl) {
      bevelled = true;
      break;
    }
  }

  double tol = 0.001;

  // the vector needed for the BinnedArray
  std::vector<Trk::TrackingVolumeOrderPosition> volOrder;

  if (bevelled) {
    ATH_MSG_VERBOSE(
      "Create 2dim VolumeArray of "
      << vols.size()
      << " Volumes (with CylinderVolumeBounds) with PhiH-binning. ");
    std::vector<float> phiSteps;
    std::vector<std::pair<std::pair<double, int>, std::pair<double, double>>>
      volPos;
    std::vector<const Trk::TrackingVolume*> fullPhiVols;

    for (unsigned int i = 0; i < vols.size(); ++i) {
      const Trk::CylinderVolumeBounds* cyl =
        dynamic_cast<const Trk::CylinderVolumeBounds*>(
          &(vols[i]->volumeBounds()));
      const Trk::BevelledCylinderVolumeBounds* bcyl =
        dynamic_cast<const Trk::BevelledCylinderVolumeBounds*>(
          &(vols[i]->volumeBounds()));
      double rmin = 0.;
      double rmax = 0.;
      double dphi = 0.;
      double mRad = 0.;
      int type = 0;

      if (cyl) {
        rmin = cyl->innerRadius();
        rmax = cyl->outerRadius();
        dphi = cyl->halfPhiSector();
        mRad = cyl->mediumRadius();
      } else if (bcyl) {
        rmin = bcyl->innerRadius();
        rmax = bcyl->outerRadius();
        dphi = bcyl->halfPhiSector();
        mRad = bcyl->mediumRadius();
        type = bcyl->type();
      } else {
        ATH_MSG_ERROR("volume not cylinder nor bevelled cylinder ");
        return nullptr;
      }

      if (dphi < M_PI) {
        // push back the volume order position
        Amg::Vector3D gp(mRad, 0., 0.);
        Amg::Vector3D ngp((vols[i]->transform()) * gp);
        if (!navtype) {
          volOrder.emplace_back(
            Trk::SharedObject<const TrackingVolume>(vols[i]), ngp);
        } else {
          volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>(
                                  vols[i], do_not_delete<const TrackingVolume>),
                                ngp);
        }
        // push back volume position to avoid another loop
        volPos.emplace_back(std::pair<double, int>(ngp.phi(), type),
                            std::pair<double, double>(rmin, rmax));
        // phi binning
        double phi1 = ngp.phi() - dphi;
        double phi2 = ngp.phi() + dphi;
        if (phi1 < -2 * M_PI)
          phi1 += 2 * M_PI;
        if (phi2 < -2 * M_PI)
          phi2 += 2 * M_PI;
        if (phi1 > 2 * M_PI)
          phi1 -= 2 * M_PI;
        if (phi2 > 2 * M_PI)
          phi2 -= 2 * M_PI;

        if (!phiSteps.empty()) {
          std::vector<float>::iterator iter = phiSteps.begin();
          bool known = false;
          while (iter != phiSteps.end()) {
            if (fabs(phi1 - (*iter)) < tol) {
              known = true;
              break;
            }
            if (phi1 < (*iter)) {
              phiSteps.insert(iter, phi1);
              known = true;
              break;
            }
            ++iter;
          }
          if (!known)
            phiSteps.push_back(phi1);
          iter = phiSteps.begin();
          known = false;
          while (iter != phiSteps.end()) {
            if (fabs(phi2 - (*iter)) < tol) {
              known = true;
              break;
            }
            if (phi2 < (*iter)) {
              phiSteps.insert(iter, phi2);
              known = true;
              break;
            }
            ++iter;
          }
          if (!known)
            phiSteps.push_back(phi2);
        } else {
          phiSteps.push_back(fmin(phi1, phi2));
          phiSteps.push_back(fmax(phi1, phi2));
        }
      } else {
        fullPhiVols.push_back(vols[i]);
      }
    } // end of first loop over volumes
    // collect volumes with full phi range
    if (phiSteps.empty()) {
      phiSteps.push_back(-M_PI);
      phiSteps.push_back(+M_PI);
    }
    for (unsigned int iv = 0; iv < fullPhiVols.size(); ++iv) {
      const Trk::CylinderVolumeBounds* cyl =
        dynamic_cast<const Trk::CylinderVolumeBounds*>(
          &(fullPhiVols[iv]->volumeBounds()));
      if (!cyl) {
        ATH_MSG_WARNING("dynamic_cast<const Trk::CylinderVolumeBounds*>   "
                        "failed ... trying to continue loop");
        continue;
      }
      double rmin = cyl->innerRadius();
      double rmax = cyl->outerRadius();

      for (unsigned int iphi = 0; iphi < phiSteps.size(); ++iphi) {
        // reference position
        double phiRef = 0.5 * phiSteps[iphi];
        if (iphi < phiSteps.size() - 1)
          phiRef += 0.5 * phiSteps[iphi + 1];
        else
          phiRef += 0.5 * phiSteps[0] + M_PI;
        // setting the position in the phi sector
        Amg::Vector3D ngp(cyl->mediumRadius() * cos(phiRef),
                          cyl->mediumRadius() * sin(phiRef),
                          0.);

        if (!navtype) {
          volOrder.emplace_back(
            Trk::SharedObject<const TrackingVolume>(fullPhiVols[iv]), ngp);
        } else {
          volOrder.emplace_back(
            Trk::SharedObject<const TrackingVolume>(
              fullPhiVols[iv], do_not_delete<const TrackingVolume>),
            ngp);
        }
        // push back volume position to avoid another loop
        volPos.emplace_back(std::pair<double, int>(ngp.phi(), 0),
                            std::pair<double, double>(rmin, rmax));
      }
    }
    // all volumes in arrays : build bin utilities

    // adjust phiSteps : upper bound equal the lower
    if (phiSteps.size() > 1) {
      if (phiSteps.back() > M_PI)
        phiSteps.erase(phiSteps.end() - 1);
      else
        phiSteps.erase(phiSteps.begin());
    }

    // phi binning
    std::vector<std::vector<std::pair<int, float>>> hSteps(phiSteps.size());
    std::vector<float> phiRef(phiSteps.size());
    for (unsigned int ip = 0; ip < phiSteps.size() - 1; ++ip)
      phiRef[ip] = 0.5 * (phiSteps[ip] + phiSteps[ip + 1]);
    phiRef.back() = 0.5 * (phiSteps.back() + phiSteps.front());
    phiRef.back() += (phiRef.back() > 0) ? -M_PI : M_PI;

    Trk::BinUtility* phiBinUtil =
      new Trk::BinUtility(phiSteps, Trk::closed, Trk::binPhi);

    // H binning

    for (unsigned int i = 0; i < volPos.size(); ++i) {

      // double phi = volPos[i].first.first;
      int type = volPos[i].first.second;
      double rmin = volPos[i].second.first;
      double rmax = volPos[i].second.second;
      int tmin = (type != 1 && type != 3) ? 0 : 1;
      int tmax = (type < 2) ? 0 : 1;

      int phibin = phiBinUtil->bin(volOrder[i].second);

      if (!hSteps[phibin].empty()) {
        std::vector<std::pair<int, float>>::iterator iter =
          hSteps[phibin].begin();
        bool known = false;
        while (iter != hSteps[phibin].end()) {
          if (fabs(rmin - (*iter).second) < tol) {
            known = true;
            break;
          }
          if (rmin < (*iter).second) {
            hSteps[phibin].insert(iter, std::pair<int, float>(tmin, rmin));
            known = true;
            break;
          }
          ++iter;
        }
        if (!known)
          hSteps[phibin].push_back(std::pair<int, float>(tmin, rmin));
        iter = hSteps[phibin].begin();
        known = false;
        while (iter != hSteps[phibin].end()) {
          if (fabs(rmax - (*iter).second) < tol) {
            known = true;
            break;
          }
          if (rmax < (*iter).second) {
            hSteps[phibin].insert(iter, std::pair<int, float>(tmax, rmax));
            known = true;
            break;
          }
          ++iter;
        }
        if (!known)
          hSteps[phibin].push_back(std::pair<int, float>(tmax, rmax));
      } else {
        hSteps[phibin].push_back(std::pair<int, float>(tmin, rmin));
        hSteps[phibin].push_back(std::pair<int, float>(tmax, rmax));
      }
    }
    // verify size of the array
    // 2dim array
    // steering bin utility in phi
    std::vector<Trk::BinUtility*>* hUtil =
      new std::vector<Trk::BinUtility*>(phiSteps.size());

    for (unsigned int ih = 0; ih < phiSteps.size(); ++ih) {
      (*hUtil)[ih] = new Trk::BinUtility(phiRef[ih], hSteps[ih]);
    }

    return new Trk::BinnedArray1D1D<Trk::TrackingVolume>(
      volOrder, phiBinUtil, hUtil);
  }

  ATH_MSG_VERBOSE(
    "Create 2dim VolumeArray of of "
    << vols.size()
    << " Volumes (with CylinderVolumeBounds) with PhiR-binning. ");

  std::vector<float> rSteps;
  double phiSector = M_PI;
  std::vector<std::pair<double, std::pair<double, double>>> volPos;

  for (unsigned int i = 0; i < vols.size(); ++i) {
    const Trk::CylinderVolumeBounds* cyl =
      dynamic_cast<const Trk::CylinderVolumeBounds*>(
        &(vols[i]->volumeBounds()));
    if (!cyl) {
      ATH_MSG_WARNING("dynamic_cast<const Trk::CylinderVolumeBounds*>   "
                      "failed ... trying to continue loop");
      continue;
    }
    double rmin = cyl->innerRadius();
    double rmax = cyl->outerRadius();
    double dphi = cyl->halfPhiSector();
    if (phiSector > 0. && fabs(dphi - phiSector) > 0.001)
      phiSector = phiSector < M_PI ? -1. : dphi;

    // push back the volume order position
    Amg::Vector3D gp(cyl->mediumRadius(), 0., 0.);
    Amg::Vector3D ngp((vols[i]->transform()) * gp);
    if (!navtype) {
      volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>(vols[i]),
                            ngp);
    } else {
      volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>(
                              vols[i], do_not_delete<const TrackingVolume>),
                            ngp);
    }
    // push back volume position to avoid another loop
    volPos.emplace_back(cyl->mediumRadius(),
                        std::pair<double, double>(ngp.phi(), dphi));
    // r binning
    if (!rSteps.empty()) {
      std::vector<float>::iterator iter = rSteps.begin();
      bool known = false;
      while (iter != rSteps.end()) {
        if (fabs(rmin - (*iter)) < tol) {
          known = true;
          break;
        }
        if (rmin < (*iter)) {
          rSteps.insert(iter, rmin);
          known = true;
          break;
        }
        ++iter;
      }
      if (!known)
        rSteps.push_back(rmin);
      iter = rSteps.begin();
      known = false;
      while (iter != rSteps.end()) {
        if (fabs(rmax - (*iter)) < tol) {
          known = true;
          break;
        }
        if (rmax < (*iter)) {
          rSteps.insert(iter, rmax);
          known = true;
          break;
        }
        ++iter;
      }
      if (!known)
        rSteps.push_back(rmax);
    } else {
      rSteps.push_back(rmin);
      rSteps.push_back(rmax);
    }
  }

  if (phiSector > 0.) { // overall equidistant binning

    std::vector<double> phi;
    std::vector<int> phiSect;
    for (unsigned int i = 0; i < rSteps.size() - 1; ++i)
      phi.push_back(M_PI);
    for (unsigned int i = 0; i < rSteps.size() - 1; ++i)
      phiSect.push_back(int(M_PI / phiSector));

    // simplify if possible
    if (rSteps.size() == 1) {
      return cylinderVolumesArrayInPhi(vols, navtype);
    }
    if (phiSector == M_PI) {
      return cylinderVolumesArrayInR(vols, navtype);
    }
    // 2dim array
    Trk::BinUtility* rBinUtil =
      new Trk::BinUtility(rSteps, Trk::open, Trk::binR);
    std::vector<Trk::BinUtility*>* phiUtil =
      new std::vector<Trk::BinUtility*>(rSteps.size() - 1);
    for (unsigned int ip = 0; ip < phiUtil->size(); ++ip) {
      (*phiUtil)[ip] =
        new Trk::BinUtility(phiSect[ip], Trk::closed, Trk::binPhi);
    }
    return new Trk::BinnedArray1D1D<Trk::TrackingVolume>(
      volOrder, rBinUtil, phiUtil);
  }

  // R binning : steering binUtility
  Trk::BinUtility* binGenR = new Trk::BinUtility(rSteps, Trk::open, Trk::binR);

  // phi binning
  std::vector<std::vector<float>> phiSteps(rSteps.size() - 1);

  for (unsigned int i = 0; i < volPos.size(); ++i) {

    double phi = volPos[i].second.first;
    double dphi = volPos[i].second.second;

    int binr = binGenR->bin(volOrder[i].second);

    float phi1 = phi - dphi;
    float phi2 = phi + dphi;
    if (phi1 < 0)
      phi1 += 2 * M_PI;
    if (phi2 < 0)
      phi2 += 2 * M_PI;

    if (!phiSteps[binr].empty()) {
      std::vector<float>::iterator iter = phiSteps[binr].begin();
      bool known = false;
      while (iter != phiSteps[binr].end()) {
        if (std::fabs(phi1 - (*iter)) < tol) {
          known = true;
          break;
        }
        if (phi1 < (*iter)) {
          phiSteps[binr].insert(iter, phi1);
          known = true;
          break;
        }
        ++iter;
      }
      if (!known)
        phiSteps[binr].push_back(phi1);
      iter = phiSteps[binr].begin();
      known = false;
      while (iter != phiSteps[binr].end()) {
        if (std::fabs(phi2 - (*iter)) < tol) {
          known = true;
          break;
        }
        if (phi2 < (*iter)) {
          phiSteps[binr].insert(iter, phi2);
          known = true;
          break;
        }
        ++iter;
      }
      if (!known)
        phiSteps[binr].push_back(phi2);
    } else {
      phiSteps[binr].push_back(std::fmin(phi1, phi2));
      phiSteps[binr].push_back(std::fmax(phi1, phi2));
    }
  }

  // 2dim array
  std::vector<Trk::BinUtility*>* phiUtil =
    new std::vector<Trk::BinUtility*>(phiSteps.size());

  for (unsigned int ip = 0; ip < phiSteps.size(); ++ip) {
    (*phiUtil)[ip] =
      new Trk::BinUtility(phiSteps[ip], Trk::closed, Trk::binPhi);
  }

  return new Trk::BinnedArray1D1D<Trk::TrackingVolume>(
    volOrder, binGenR, phiUtil);
}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::cylinderVolumesArrayInPhiZ(
  const std::vector<const Trk::TrackingVolume*>& vols,
  bool navtype) const
{
  ATH_MSG_VERBOSE(
    "Create 2dim VolumeArray of of "
    << vols.size()
    << " Volumes (with CylinderVolumeBounds) with PhiZ-binning. ");

  double tol = 0.001;
  // the vector needed for the BinnedArray
  std::vector<Trk::TrackingVolumeOrderPosition> volOrder;

  std::vector<float> zSteps;

  double phiSector = M_PI;
  std::vector<std::pair<float, std::pair<float, float>>> volPos;

  for (unsigned int i = 0; i < vols.size(); ++i) {
    const Trk::CylinderVolumeBounds* cyl =
      dynamic_cast<const Trk::CylinderVolumeBounds*>(
        &(vols[i]->volumeBounds()));
    const Trk::BevelledCylinderVolumeBounds* bcyl =
      dynamic_cast<const Trk::BevelledCylinderVolumeBounds*>(
        &(vols[i]->volumeBounds()));
    double zmin = 0.;
    double zmax = 0.;
    double dphi = 0.;
    double mRad = 0.;
    if (cyl) {
      zmin = vols[i]->center()[2] - cyl->halflengthZ();
      zmax = vols[i]->center()[2] + cyl->halflengthZ();
      dphi = cyl->halfPhiSector();
      mRad = cyl->mediumRadius();
    } else if (bcyl) {
      zmin = vols[i]->center()[2] - bcyl->halflengthZ();
      zmax = vols[i]->center()[2] + bcyl->halflengthZ();
      dphi = bcyl->halfPhiSector();
      mRad = bcyl->mediumRadius();
    } else {
      ATH_MSG_ERROR("volume not cylinder nor bevelled cylinder ");
      return nullptr;
    }

    if (phiSector > 0. && fabs(dphi - phiSector) > 0.001)
      phiSector = phiSector < M_PI ? -1. : dphi;

    // push back the volume order position
    Amg::Vector3D gp(mRad, 0., 0.);
    Amg::Vector3D ngp((vols[i]->transform()) * gp);
    if (!navtype) {
      volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>(vols[i]),
                            ngp);
    } else {
      volOrder.emplace_back(Trk::SharedObject<const TrackingVolume>(
                              vols[i], do_not_delete<const TrackingVolume>),
                            ngp);
    }
    // push back volume position to avoid another loop
    volPos.emplace_back(vols[i]->center()[2],
                        std::pair<float, float>(ngp.phi(), dphi));
    // z binning
    if (!zSteps.empty()) {
      std::vector<float>::iterator iter = zSteps.begin();
      bool known = false;
      while (iter != zSteps.end()) {
        if (fabs(zmin - (*iter)) < tol) {
          known = true;
          break;
        }
        if (zmin < (*iter)) {
          zSteps.insert(iter, zmin);
          known = true;
          break;
        }
        ++iter;
      }
      if (!known)
        zSteps.push_back(zmin);
      iter = zSteps.begin();
      known = false;
      while (iter != zSteps.end()) {
        if (fabs(zmax - (*iter)) < tol) {
          known = true;
          break;
        }
        if (zmax < (*iter)) {
          zSteps.insert(iter, zmax);
          known = true;
          break;
        }
        ++iter;
      }
      if (!known)
        zSteps.push_back(zmax);
    } else {
      zSteps.push_back(zmin);
      zSteps.push_back(zmax);
    }
  }

  if (phiSector > 0.) { // overall equidistant binning

    std::vector<float> phi;
    std::vector<int> phiSect;
    for (unsigned int i = 0; i < zSteps.size() - 1; ++i)
      phi.push_back(M_PI);
    for (unsigned int i = 0; i < zSteps.size() - 1; ++i)
      phiSect.push_back(int(M_PI / phiSector));

    // simplify if possible
    if (phiSector == M_PI) {
      return cylinderVolumesArrayInZ(vols, navtype);
    }
    if (zSteps.size() == 2) {
      return cylinderVolumesArrayInPhi(vols, navtype);
    }
    // 2dim array
    Trk::BinUtility* binGenZPhi =
      new Trk::BinUtility(zSteps, Trk::open, Trk::binZ);
    (*binGenZPhi) +=
      Trk::BinUtility(phiSector, -M_PI, M_PI, Trk::closed, Trk::binPhi);
    return new Trk::BinnedArray2D<Trk::TrackingVolume>(volOrder, binGenZPhi);
  }

  // steering binUtility in binZ
  Trk::BinUtility* binGenZ = new Trk::BinUtility(zSteps, Trk::open, Trk::binZ);

  // phi binning  - steering binUtility in binZ
  std::vector<std::vector<float>> phiSteps(zSteps.size() - 1);

  for (unsigned int i = 0; i < volPos.size(); ++i) {

    float phi = volPos[i].second.first;
    float dphi = volPos[i].second.second;

    int binZ = binGenZ->bin(volOrder[i].second);

    float phi1 = phi - dphi;
    float phi2 = phi + dphi;
    if (phi1 < 0)
      phi1 += 2 * M_PI;
    if (phi2 < 0)
      phi2 += 2 * M_PI;

    if (!phiSteps[binZ].empty()) {
      std::vector<float>::iterator iter = phiSteps[binZ].begin();
      bool known = false;
      while (iter != phiSteps[binZ].end()) {
        if (std::fabs(phi1 - (*iter)) < tol) {
          known = true;
          break;
        }
        if (phi1 < (*iter)) {
          phiSteps[binZ].insert(iter, phi1);
          known = true;
          break;
        }
        ++iter;
      }
      if (!known)
        phiSteps[binZ].push_back(phi1);
      iter = phiSteps[binZ].begin();
      known = false;
      while (iter != phiSteps[binZ].end()) {
        if (std::fabs(phi2 - (*iter)) < tol) {
          known = true;
          break;
        }
        if (phi2 < (*iter)) {
          phiSteps[binZ].insert(iter, phi2);
          known = true;
          break;
        }
        ++iter;
      }
      if (!known)
        phiSteps[binZ].push_back(phi2);
    } else {
      phiSteps[binZ].push_back(std::fmin(phi1, phi2));
      phiSteps[binZ].push_back(std::fmax(phi1, phi2));
      //	phiSectors[binZ] = dphi ;
    }
  }

  // 2dim array: construct from two 1D boundaries
  std::vector<Trk::BinUtility*>* phiUtil =
    new std::vector<Trk::BinUtility*>(phiSteps.size());

  for (unsigned int ip = 0; ip < phiSteps.size(); ++ip) {
    (*phiUtil)[ip] =
      new Trk::BinUtility(phiSteps[ip], Trk::closed, Trk::binPhi);
  }

  return new Trk::BinnedArray1D1D<Trk::TrackingVolume>(
    volOrder, binGenZ, phiUtil);
}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::cuboidVolumesArrayInZ(
  const std::vector<const TrackingVolume*>&,
  bool) const
{
  ATH_MSG_ERROR(" cuboidVolumesArrayInZ not implemented ");
  return nullptr;
}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::cuboidVolumesArrayNav(
  const std::vector<const TrackingVolume*>& vols,
  Trk::BinUtility* binUtil,
  bool navtype) const
{
  // the vector needed for the BinnedArray
  std::vector<Trk::SharedObject<const Trk::TrackingVolume>> volOrder;
  // loop over volumes and fill primaries
  std::vector<const Trk::TrackingVolume*>::const_iterator volIter =
    vols.begin();
  for (; volIter != vols.end(); ++volIter) {
    const Trk::CuboidVolumeBounds* currentCubBounds =
      dynamic_cast<const Trk::CuboidVolumeBounds*>(
        &((*volIter)->volumeBounds()));
    if (!currentCubBounds) {
      ATH_MSG_ERROR("Given TrackingVolume to TrackingVolumeArrayCreator didn't "
                    "match specified shape: return 0");
      return nullptr;
    }
    // push back the volume order position
    if (!navtype) {
      volOrder.push_back(Trk::SharedObject<const TrackingVolume>((*volIter)));
    } else {
      volOrder.push_back(Trk::SharedObject<const TrackingVolume>(
        (*volIter), do_not_delete<const TrackingVolume>));
    }
  }
  if (!volOrder.empty()) {
    Amg::Transform3D* navTransform = new Amg::Transform3D;
    navTransform->setIdentity();
    return new Trk::NavBinnedArray1D<Trk::TrackingVolume>(
      volOrder, binUtil, navTransform);
  }
  ATH_MSG_ERROR(
    "No TrackingVolumes provided to the TrackingVolumeArrayCreator: return 0");
  return nullptr;
}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::trapezoidVolumesArrayNav(
  const std::vector<const TrackingVolume*>& vols,
  Trk::BinUtility* binUtil,
  bool navtype) const
{
  // the vector needed for the BinnedArray
  std::vector<Trk::SharedObject<const Trk::TrackingVolume>> volOrder;
  // loop over volumes and fill primaries
  std::vector<const Trk::TrackingVolume*>::const_iterator volIter =
    vols.begin();
  for (; volIter != vols.end(); ++volIter) {
    const Trk::TrapezoidVolumeBounds* currentTrdBounds =
      dynamic_cast<const Trk::TrapezoidVolumeBounds*>(
        &((*volIter)->volumeBounds()));
    if (!currentTrdBounds) {
      ATH_MSG_ERROR("Given TrackingVolume to TrackingVolumeArrayCreator didn't "
                    "match specified shape: return 0");
      return nullptr;
    }
    // push back the volume order position
    if (!navtype) {
      volOrder.push_back(Trk::SharedObject<const TrackingVolume>(*volIter));
    } else {
      volOrder.push_back(Trk::SharedObject<const TrackingVolume>(
        *volIter, do_not_delete<const TrackingVolume>));
    }
  }
  if (!volOrder.empty()) {
    Amg::Transform3D* navTransform = new Amg::Transform3D;
    navTransform->setIdentity();
    return new Trk::NavBinnedArray1D<Trk::TrackingVolume>(
      volOrder, binUtil, navTransform);
  }
  ATH_MSG_ERROR(
    "No TrackingVolumes provided to the TrackingVolumeArrayCreator: return 0");
  return nullptr;
}

Trk::TrackingVolumeArray*
Trk::TrackingVolumeArrayCreator::doubleTrapezoidVolumesArrayNav(
  const std::vector<const TrackingVolume*>& vols,
  Trk::BinUtility* binUtil,
  bool navtype) const
{
  // the vector needed for the BinnedArray
  std::vector<Trk::SharedObject<const Trk::TrackingVolume>> volOrder;
  // loop over volumes and fill primaries
  std::vector<const Trk::TrackingVolume*>::const_iterator volIter =
    vols.begin();
  for (; volIter != vols.end(); ++volIter) {
    const Trk::DoubleTrapezoidVolumeBounds* currentDTrdBounds =
      dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*>(
        &((*volIter)->volumeBounds()));
    if (!currentDTrdBounds) {
      ATH_MSG_ERROR("Given TrackingVolume to TrackingVolumeArrayCreator didn't "
                    "match specified shape: return 0");
      return nullptr;
    }
    // push back the volume order position
    if (!navtype) {
      volOrder.push_back(Trk::SharedObject<const TrackingVolume>((*volIter)));
    } else {
      volOrder.push_back(Trk::SharedObject<const TrackingVolume>(
        (*volIter), do_not_delete<const TrackingVolume>));
    }
  }
  if (!volOrder.empty()) {
    Amg::Transform3D* navTransform = new Amg::Transform3D;
    navTransform->setIdentity();
    return new Trk::NavBinnedArray1D<Trk::TrackingVolume>(
      volOrder, binUtil, navTransform);
  }
  ATH_MSG_ERROR(
    "No TrackingVolumes provided to the TrackingVolumeArrayCreator: return 0");
  return nullptr;
}

