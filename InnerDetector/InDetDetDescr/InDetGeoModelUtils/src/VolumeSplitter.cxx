/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/VolumeSplitter.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/ServiceVolume.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <algorithm>

namespace InDetDD {
  VolumeSplitter::VolumeSplitter()
    : m_ownVolumes(true),
    m_epsilon(0.0001 * Gaudi::Units::mm)
  {}

  VolumeSplitter::~VolumeSplitter() {
    if (m_ownVolumes) {
      for (std::vector<const ServiceVolume*>::iterator iter = m_volumes.begin(); iter != m_volumes.end(); ++iter) {
        delete *iter;
      }
    }
  }

  const std::vector<const ServiceVolume*>&
  VolumeSplitter::splitAll(const Zone& zone, const std::vector<const ServiceVolume*> origVolumeList) {
    for (unsigned int i = 0; i < origVolumeList.size(); ++i) {
      split(zone, *(origVolumeList[i]));
    }
    return m_volumes;
  }

  void
  VolumeSplitter::split(const Zone& zone, const ServiceVolume& origVolume) {
    makeVolumes(&zone, origVolume);
  }

  Ray
  VolumeSplitter::makeRay(const ServiceVolume& origVolume) {
    double zmin = origVolume.zmin();
    double zmax = origVolume.zmax();
    double rmin = origVolume.rmin();
    double rmax = origVolume.rmax();

    if (zmax < zmin) std::swap(zmin, zmax);
    if (rmax < rmin) std::swap(rmin, rmax);

    bool horizontal = (std::abs(zmax - zmin) > std::abs(rmax - rmin));
    if (horizontal) {
      double rmid = 0.5 * (rmin + rmax);
      return Ray(Point(zmin, rmid), Point(zmax, rmid));
    } else {
      double zmid = 0.5 * (zmin + zmax);
      return Ray(Point(zmid, rmin), Point(zmid, rmax));
    }
  }

  void
  VolumeSplitter::makeVolumes(const Zone* zone, const ServiceVolume& origVolume) {
    // See if its a symmetric volume
    if (origVolume.zsymm()) {
      ServiceVolume param1(origVolume);
      ServiceVolume param2(origVolume);
      param1.addLabel("A");
      param1.setNeedsRotation(false);
      param2.addLabel("C");
      param2.setZmin(-origVolume.zmax());
      param2.setZmax(-origVolume.zmin());
      param2.setNeedsRotation(origVolume.needsRotation());
      splitVolume(zone, param1);
      splitVolume(zone, param2);
    } else {
      ServiceVolume param1(origVolume);
      param1.setNeedsRotation(false);
      splitVolume(zone, param1);
    }
  }

  void
  VolumeSplitter::splitVolume(const Zone* zone, const ServiceVolume& param) {
    Ray ray = makeRay(param);
    SegmentSplitter splitter;
    const SegmentList& segments = splitter.split(zone, ray);

    //std::cout << "Segments: " << std::endl;
    //segments.print();

    double volumeTotal = 0;

    std::vector<ServiceVolume* > tmpServiceVec;
    tmpServiceVec.reserve(segments.size());
    for (unsigned int i = 0; i < segments.size(); ++i) {
      const Segment& seg = segments.getSegment(i);
      ServiceVolume* paramNew = new ServiceVolume(param);

      if (segments.horizontal()) {
        if (param.splittableInZ()) {
          paramNew->setZmin(seg.zmin());
          paramNew->setZmax(seg.zmax());
          // If z changed adjust r in cone like volumes
          adjustR(param, paramNew);
        } else if (i == 1) {
          std::cout << "Volume " << param.fullLabel() << " cannot be split in Z" << std::endl;
        }
      } else {
        // vertical
        if (param.splittableInR()) {
          paramNew->setRmin(seg.rmin());
          paramNew->setRmax(seg.rmax());
        } else if (i == 1) {
          std::cout << "Volume " << param.fullLabel() << " cannot be split in R" << std::endl;
        }
      }

      if (seg.rotated()) {
        // Often the -ve endcap region is created as the +ve endcap and then rotated. Therefore we have to reflect the z
        // coords.
        // Also have to turn off the needsRotation flag in case it was set.
        //std::cout << "Segment " << i << " rotated " << paramNew->zmin() << " " << paramNew->zmax() <<std::endl;
        double zminTmp = paramNew->zmin();
        paramNew->setZmin(-paramNew->zmax());
        paramNew->setZmax(-zminTmp);
        paramNew->setNeedsRotation(false);
        //std::cout << "After adjusted " << paramNew->zmin() << " " << paramNew->zmax() <<std::endl;
      }

      if (param.splittableInZ()) {
        paramNew->reduceSize(m_epsilon); // safety gap between volumes
      }

      paramNew->setRegion(seg.label());
      tmpServiceVec.push_back(paramNew);

      paramNew->getShape();
      volumeTotal += paramNew->volume();
      //std::cout << i << ": volume: " <<  paramNew->volume() << std::endl;
    }
    //std::cout << "Total volume: " << volumeTotal << std::endl;
    for (unsigned int i = 0; i < segments.size(); ++i) {
      ServiceVolume* paramNew = tmpServiceVec[i];
      if (segments.size() > 1) {
        std::ostringstream ostr;
        ostr << "_" << i + 1;
        paramNew->addLabel(ostr.str());
      }
      paramNew->setOrigVolume(volumeTotal);
      m_volumes.push_back(paramNew);
    }
  }

// This takes care of cone like volumes and adjust the radius according to the adjusted z.
  void
  VolumeSplitter::adjustR(const ServiceVolume& param, ServiceVolume* paramNew) {
    double z1 = param.zmin();
    double z2 = param.zmax();
    double z1New = paramNew->zmin();
    double z2New = paramNew->zmax();
    double rmin1 = param.rmin();
    double rmin2 = param.rmin2();
    double rmax1 = param.rmax();
    double rmax2 = param.rmax2();

    // Make sure z1 < z2
    // as r1, r2 order assumes this
    if (z1 > z2) std::swap(z1, z2);
    if (z1New > z2New) std::swap(z1New, z2New);

    // Avoid divide by zero.
    if (z1 == z2) return;

    if (z1 != z1New || z2 != z2New) {
      if (rmin2 > 0 && rmax2 > 0 && (rmin1 != rmin2 || rmax1 != rmax2)) {
        double slopeMin = (rmin2 - rmin1) / (z2 - z1);
        double slopeMax = (rmax2 - rmax1) / (z2 - z1);

        double rmin1New = (z1New - z1) * slopeMin + rmin1;
        double rmin2New = (z2New - z1) * slopeMin + rmin1;

        double rmax1New = (z1New - z1) * slopeMax + rmax1;
        double rmax2New = (z2New - z1) * slopeMax + rmax1;

        paramNew->setRmin(rmin1New);
        paramNew->setRmax(rmax1New);
        paramNew->setRmin2(rmin2New);
        paramNew->setRmax2(rmax2New);
      }
    }
  }
} // end namespace
