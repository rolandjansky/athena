/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CombinedVolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/CombinedVolumeBounds.h"

#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
#include "TrkVolumes/SubtractedVolumeBounds.h"
#include "TrkVolumes/VolumeExcluder.h"
// TrkSurfaces
#include "TrkGeometrySurfaces/SubtractedCylinderSurface.h"
#include "TrkGeometrySurfaces/SubtractedPlaneSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/EllipseBounds.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/Surface.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <utility>

Trk::CombinedVolumeBounds::CombinedVolumeBounds()
  : VolumeBounds()
  , m_first(nullptr)
  , m_second(nullptr)
  , m_intersection(false)
  , m_objectAccessor()
  , m_boundsOrientation()
{}

Trk::CombinedVolumeBounds::CombinedVolumeBounds(
  Volume* vol1,
  Volume* vol2,
  bool intersection)
  : VolumeBounds()
  , m_first(vol1)
  , m_second(vol2)
  , m_intersection(intersection)
  , m_objectAccessor()
  , m_boundsOrientation()
{}

Trk::CombinedVolumeBounds::CombinedVolumeBounds(
  const Trk::CombinedVolumeBounds& bobo)
  : VolumeBounds()
  , m_first(bobo.m_first)
  , m_second(bobo.m_second)
  , m_intersection(bobo.m_intersection)
  , m_objectAccessor(bobo.m_objectAccessor)
  , m_boundsOrientation()
{
  m_boundsOrientation.resize(bobo.m_boundsOrientation.size());
  for (unsigned int i = 0; i < bobo.m_boundsOrientation.size(); i++)
    m_boundsOrientation[i] = bobo.m_boundsOrientation[i];
}

Trk::CombinedVolumeBounds::~CombinedVolumeBounds()
{
  m_boundsOrientation.clear();
  delete m_first;
  delete m_second;
}

Trk::CombinedVolumeBounds&
Trk::CombinedVolumeBounds::operator=(const Trk::CombinedVolumeBounds& bobo)
{
  if (this != &bobo) {
    m_first = bobo.m_first;
    m_second = bobo.m_second;
    m_intersection = bobo.m_intersection;
    m_objectAccessor = bobo.m_objectAccessor;
    m_boundsOrientation = bobo.m_boundsOrientation;
    m_boundsOrientation.resize(bobo.m_boundsOrientation.size());
    for (unsigned int i = 0; i < bobo.m_boundsOrientation.size(); i++)
      m_boundsOrientation[i] = bobo.m_boundsOrientation[i];
  }
  return *this;
}

const std::vector<const Trk::Surface*>*
  Trk::CombinedVolumeBounds::decomposeToSurfaces
  (const Amg::Transform3D& transf) 
{
  std::vector<const Trk::Surface*>* retsf =
    new std::vector<const Trk::Surface*>;

  const Trk::CylinderVolumeBounds* cylVol =
    dynamic_cast<const Trk::CylinderVolumeBounds*>(&(m_first->volumeBounds()));
  const Trk::SimplePolygonBrepVolumeBounds* spbVol =
    dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*>(
      &(m_first->volumeBounds()));
  const Trk::CombinedVolumeBounds* comVol =
    dynamic_cast<const Trk::CombinedVolumeBounds*>(&(m_first->volumeBounds()));
  const Trk::SubtractedVolumeBounds* subVol =
    dynamic_cast<const Trk::SubtractedVolumeBounds*>(
      &(m_first->volumeBounds()));

  // get surfaces for first boundaries
  const std::vector<const Trk::Surface*>* firstSurfaces =
    m_first->volumeBounds().decomposeToSurfaces(transf * m_first->transform());
  // get surfaces for second boundaries
  const std::vector<const Trk::Surface*>* secondSurfaces =
    m_second->volumeBounds().decomposeToSurfaces(
      transf * m_second->transform());
  unsigned int nSurf = firstSurfaces->size() + secondSurfaces->size();
  m_boundsOrientation.resize(nSurf);

  std::vector<unsigned int> subtrSecond;

  // loop over surfaces; convert disc surface to a plane surface using elliptic
  // bounds
  for (unsigned int out = 0; out < firstSurfaces->size(); out++) {
    //
    const SubtractedPlaneSurface* splo =
      dynamic_cast<const SubtractedPlaneSurface*>((*firstSurfaces)[out]);
    const PlaneSurface* plo =
      dynamic_cast<const PlaneSurface*>((*firstSurfaces)[out]);
    const SubtractedCylinderSurface* sclo =
      dynamic_cast<const SubtractedCylinderSurface*>((*firstSurfaces)[out]);
    const CylinderSurface* clo =
      dynamic_cast<const CylinderSurface*>((*firstSurfaces)[out]);
    const DiscSurface* dlo =
      dynamic_cast<const DiscSurface*>((*firstSurfaces)[out]);

    // resolve bounds orientation : copy from combined/subtracted, swap inner
    // cyl, swap bottom spb
    if (comVol)
      m_boundsOrientation[out] = comVol->boundsOrientation()[out];
    else if (subVol)
      m_boundsOrientation[out] = subVol->boundsOrientation()[out];
    else if (cylVol && clo && out == 3)
      m_boundsOrientation[out] = false;
    else if (spbVol && out == 0)
      m_boundsOrientation[out] = false;
    else
      m_boundsOrientation[out] = true;

    Trk::Volume* secondSub = createSubtractedVolume(
      (*firstSurfaces)[out]->transform().inverse() * transf, m_second);

    if (sclo || splo) {
      bool shared = false;
      SharedObject<Trk::AreaExcluder> vEx;
      if (splo) {
        vEx = splo->subtractedVolume();
        shared = splo->shared();
      }
      if (sclo) {
        vEx = sclo->subtractedVolume();
        shared = sclo->shared();
      }
      const Trk::VolumeExcluder* volExcl =
        dynamic_cast<const Trk::VolumeExcluder*>(vEx.get());
      if (!volExcl)
        throw std::logic_error("Not a VolumeExcluder");

      Trk::Volume* firstSub = new Trk::Volume(*volExcl->volume());

      Trk::Volume* comb_sub = nullptr;
      if (!shared && !m_intersection)
        comb_sub = new Trk::Volume(
          nullptr,
          new Trk::CombinedVolumeBounds(secondSub, firstSub, m_intersection));
      if (!shared && m_intersection)
        comb_sub = new Trk::Volume(
          nullptr, new Trk::SubtractedVolumeBounds(secondSub, firstSub));
      if (shared && m_intersection)
        comb_sub = new Trk::Volume(
          nullptr,
          new Trk::CombinedVolumeBounds(secondSub, firstSub, m_intersection));
      if (shared && !m_intersection)
        comb_sub = new Trk::Volume(
          nullptr, new Trk::SubtractedVolumeBounds(firstSub, secondSub));
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(comb_sub);
      bool new_shared = shared;
      if (m_intersection)
        new_shared = true;
      if (splo)
        retsf->push_back(
          new Trk::SubtractedPlaneSurface(*splo, volEx, new_shared));
      if (sclo)
        retsf->push_back(
          new Trk::SubtractedCylinderSurface(*sclo, volEx, new_shared));

    } else if (plo || clo || dlo) {
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(secondSub);
      if (plo)
        retsf->push_back(
          new Trk::SubtractedPlaneSurface(*plo, volEx, m_intersection));
      if (clo)
        retsf->push_back(
          new Trk::SubtractedCylinderSurface(*clo, volEx, m_intersection));
      if (dlo) {
        const DiscBounds* db =
          dynamic_cast<const DiscBounds*>(&(dlo->bounds()));
        if (!db)
          throw std::logic_error("Not DiscBounds");

        EllipseBounds* eb = new EllipseBounds(
          db->rMin(), db->rMin(), db->rMax(), db->rMax(), db->halfPhiSector());
        plo = new PlaneSurface(Amg::Transform3D(dlo->transform()), eb);
        retsf->push_back(
          new Trk::SubtractedPlaneSurface(*plo, volEx, m_intersection));
        delete plo;
      }
    } else {
      throw std::runtime_error(
        "Unhandled surface in CombinedVolumeBounds::decomposeToSurfaces.");
    }
  }

  cylVol =
    dynamic_cast<const Trk::CylinderVolumeBounds*>(&(m_second->volumeBounds()));
  spbVol = dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*>(
    &(m_second->volumeBounds()));
  comVol =
    dynamic_cast<const Trk::CombinedVolumeBounds*>(&(m_second->volumeBounds()));
  subVol = dynamic_cast<const Trk::SubtractedVolumeBounds*>(
    &(m_second->volumeBounds()));
  unsigned int nOut = firstSurfaces->size();

  for (unsigned int in = 0; in < secondSurfaces->size(); in++) {
    //
    const SubtractedPlaneSurface* spli =
      dynamic_cast<const SubtractedPlaneSurface*>((*secondSurfaces)[in]);
    const PlaneSurface* pli =
      dynamic_cast<const PlaneSurface*>((*secondSurfaces)[in]);
    const SubtractedCylinderSurface* scli =
      dynamic_cast<const SubtractedCylinderSurface*>((*secondSurfaces)[in]);
    const CylinderSurface* cli =
      dynamic_cast<const CylinderSurface*>((*secondSurfaces)[in]);
    const DiscSurface* dli =
      dynamic_cast<const DiscSurface*>((*secondSurfaces)[in]);

    // resolve bounds orientation : copy from combined/subtracted, swap inner
    // cyl, swap bottom spb
    if (comVol)
      m_boundsOrientation[nOut + in] = comVol->boundsOrientation()[in];
    else if (subVol)
      m_boundsOrientation[nOut + in] = subVol->boundsOrientation()[in];
    else if (cylVol && cli && in == 3)
      m_boundsOrientation[nOut + in] = false;
    else if (spbVol && in == 0)
      m_boundsOrientation[nOut + in] = false;
    else
      m_boundsOrientation[nOut + in] = true;

    Trk::Volume* firstSub = createSubtractedVolume(
      (*secondSurfaces)[in]->transform().inverse() * transf, m_first);
    if (scli || spli) {
      bool shared = false;
      Trk::SharedObject<Trk::AreaExcluder> vEx;
      if (spli) {
        vEx = spli->subtractedVolume();
        shared = spli->shared();
      }
      if (scli) {
        vEx = scli->subtractedVolume();
        shared = scli->shared();
      }
      const Trk::VolumeExcluder* volExcl =
        dynamic_cast<const Trk::VolumeExcluder*>(vEx.get());
      if (!volExcl)
        throw std::logic_error("Not a VolumeExcluder");
      Trk::Volume* secondSub = new Trk::Volume(*volExcl->volume());

      Trk::Volume* comb_sub = nullptr;
      if (!shared && !m_intersection)
        comb_sub = new Trk::Volume(
          nullptr,
          new Trk::CombinedVolumeBounds(firstSub, secondSub, m_intersection));
      if (!shared && m_intersection)
        comb_sub = new Trk::Volume(
          nullptr, new Trk::SubtractedVolumeBounds(firstSub, secondSub));
      if (shared && m_intersection)
        comb_sub = new Trk::Volume(
          nullptr,
          new Trk::CombinedVolumeBounds(firstSub, secondSub, m_intersection));
      if (shared && !m_intersection)
        comb_sub = new Trk::Volume(
          nullptr, new Trk::SubtractedVolumeBounds(secondSub, firstSub));
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(comb_sub);
      bool new_shared = shared;
      if (m_intersection)
        new_shared = true;
      if (spli)
        retsf->push_back(
          new Trk::SubtractedPlaneSurface(*spli, volEx, new_shared));
      if (scli)
        retsf->push_back(
          new Trk::SubtractedCylinderSurface(*scli, volEx, new_shared));

    } else if (pli || cli || dli) {
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(firstSub);
      if (pli)
        retsf->push_back(
          new Trk::SubtractedPlaneSurface(*pli, volEx, m_intersection));
      if (cli)
        retsf->push_back(
          new Trk::SubtractedCylinderSurface(*cli, volEx, m_intersection));
      if (dli) {
        const DiscBounds* db =
          dynamic_cast<const DiscBounds*>(&(dli->bounds()));
        if (!db)
          throw std::logic_error("Not DiscBounds");

        EllipseBounds* eb = new EllipseBounds(
          db->rMin(), db->rMin(), db->rMax(), db->rMax(), db->halfPhiSector());
        pli = new PlaneSurface(Amg::Transform3D(dli->transform()), eb);
        retsf->push_back(
          new Trk::SubtractedPlaneSurface(*pli, volEx, m_intersection));
        delete pli;
      }
    } else {
      throw std::runtime_error(
        "Unhandled surface in CombinedVolumeBounds::decomposeToSurfaces.");
    }
  }

  for (size_t i = 0; i < firstSurfaces->size(); i++) {
    delete (*firstSurfaces)[i];
  }
  for (size_t i = 0; i < secondSurfaces->size(); i++) {
    delete (*secondSurfaces)[i];
  }
  delete firstSurfaces;
  delete secondSurfaces;

  return retsf;
}

// ostream operator overload

MsgStream&
Trk::CombinedVolumeBounds::dump(MsgStream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::CombinedVolumeBounds: first,second ";
  sl << temp_sl.str();
  std::as_const(*m_first).volumeBounds().dump(sl);
  std::as_const(*m_second).volumeBounds().dump(sl);
  return sl;
}

std::ostream&
Trk::CombinedVolumeBounds::dump(std::ostream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::CombinedVolumeBounds: first,second ";
  sl << temp_sl.str();
  std::as_const(*m_first).volumeBounds().dump(sl);
  std::as_const(*m_second).volumeBounds().dump(sl);
  return sl;
}

Trk::Volume*
Trk::CombinedVolumeBounds::createSubtractedVolume(
  const Amg::Transform3D& transf,
  Trk::Volume* subtrVol) 
{
  Trk::Volume* subVol = nullptr;
  if (!subtrVol)
    return subVol;

  subVol = new Trk::Volume(*subtrVol, transf);

  return subVol;
}

