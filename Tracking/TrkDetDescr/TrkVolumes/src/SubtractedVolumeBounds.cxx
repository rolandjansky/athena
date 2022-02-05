/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedVolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/SubtractedVolumeBounds.h"
#include "TrkVolumes/CombinedVolumeBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
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
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/Volume.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <cmath>
#include <iostream>
#include <utility>

Trk::SubtractedVolumeBounds::SubtractedVolumeBounds()
  : VolumeBounds()
  , m_outer(nullptr)
  , m_inner(nullptr)
  , m_objectAccessor()
  , m_boundsOrientation()
{}

Trk::SubtractedVolumeBounds::SubtractedVolumeBounds(Volume* vol1, Volume* vol2)
  : VolumeBounds()
  , m_outer(vol1)
  , m_inner(vol2)
  , m_objectAccessor()
  , m_boundsOrientation()
{}

Trk::SubtractedVolumeBounds::SubtractedVolumeBounds(
  const Trk::SubtractedVolumeBounds& bobo)
  : VolumeBounds()
  , m_outer(bobo.m_outer)
  , m_inner(bobo.m_inner)
  , m_objectAccessor(bobo.m_objectAccessor)
  , m_boundsOrientation()
{
  m_boundsOrientation.resize(bobo.m_boundsOrientation.size());
  for (unsigned int i = 0; i < bobo.m_boundsOrientation.size(); i++)
    m_boundsOrientation[i] = bobo.m_boundsOrientation[i];
}

Trk::SubtractedVolumeBounds::~SubtractedVolumeBounds()
{
  m_boundsOrientation.clear();
  delete m_outer;
  delete m_inner;
}

Trk::SubtractedVolumeBounds&
Trk::SubtractedVolumeBounds::operator=(const Trk::SubtractedVolumeBounds& bobo)
{
  if (this != &bobo) {
    m_outer = bobo.m_outer;
    m_inner = bobo.m_inner;
    m_objectAccessor = bobo.m_objectAccessor;
    m_boundsOrientation = bobo.m_boundsOrientation;
    m_boundsOrientation.resize(bobo.m_boundsOrientation.size());
    for (unsigned int i = 0; i < bobo.m_boundsOrientation.size(); i++)
      m_boundsOrientation[i] = bobo.m_boundsOrientation[i];
  }
  return *this;
}

const std::vector<const Trk::Surface*>*
  Trk::SubtractedVolumeBounds::decomposeToSurfaces
  (const Amg::Transform3D& transf)
{
  // double tol=0.001;
  // get surfaces for outer boundaries
  const std::vector<const Trk::Surface*>* outerSurfaces =
    m_outer->volumeBounds().decomposeToSurfaces(transf * m_outer->transform());
  // get surfaces for inner boundaries
  const std::vector<const Trk::Surface*>* innerSurfaces =
    m_inner->volumeBounds().decomposeToSurfaces(transf * m_inner->transform());
  std::vector<unsigned int> subtrInner;
  std::vector<const Trk::Surface*>* retsf =
    new std::vector<const Trk::Surface*>;

  unsigned int nSurf = outerSurfaces->size() + innerSurfaces->size();
  m_boundsOrientation.resize(nSurf);

  const Trk::CylinderVolumeBounds* cylVol =
    dynamic_cast<const Trk::CylinderVolumeBounds*>(&(m_outer->volumeBounds()));
  const Trk::SimplePolygonBrepVolumeBounds* spbVol =
    dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*>(
      &(m_outer->volumeBounds()));
  const Trk::CombinedVolumeBounds* comVol =
    dynamic_cast<const Trk::CombinedVolumeBounds*>(&(m_outer->volumeBounds()));
  const Trk::SubtractedVolumeBounds* subVol =
    dynamic_cast<const Trk::SubtractedVolumeBounds*>(
      &(m_outer->volumeBounds()));

  // loop over 'outer' boundary surfaces; modified by subtracted volume
  for (unsigned int out = 0; out < outerSurfaces->size(); out++) {
    const SubtractedPlaneSurface* splo =
      dynamic_cast<const SubtractedPlaneSurface*>((*outerSurfaces)[out]);
    const PlaneSurface* plo =
      dynamic_cast<const PlaneSurface*>((*outerSurfaces)[out]);
    const SubtractedCylinderSurface* sclo =
      dynamic_cast<const SubtractedCylinderSurface*>((*outerSurfaces)[out]);
    const CylinderSurface* clo =
      dynamic_cast<const CylinderSurface*>((*outerSurfaces)[out]);
    const DiscSurface* dlo =
      dynamic_cast<const DiscSurface*>((*outerSurfaces)[out]);
    if (!(splo || plo || sclo || clo || dlo)) {
      throw std::runtime_error("Unhandled surface.");
    }
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
    //
    Trk::Volume* innerSub = createSubtractedVolume(
      (*outerSurfaces)[out]->transform().inverse() * transf, m_inner);

    if (splo || sclo) { // multiple subtraction
      SharedObject<Trk::AreaExcluder> vEx;
      bool shared = false;
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
      Trk::Volume* outerSub = new Trk::Volume(*volExcl->volume());

      Trk::Volume* comb_sub = nullptr;
      if (!shared)
        comb_sub = new Trk::Volume(
          nullptr, new Trk::CombinedVolumeBounds(innerSub, outerSub, false));
      else
        comb_sub = new Trk::Volume(
          nullptr, new Trk::SubtractedVolumeBounds(outerSub, innerSub));
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(comb_sub);
      if (splo)
        retsf->push_back(new Trk::SubtractedPlaneSurface(*splo, volEx, shared));
      if (sclo)
        retsf->push_back(
          new Trk::SubtractedCylinderSurface(*sclo, volEx, shared));
    } else {
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(innerSub);
      if (plo)
        retsf->push_back(new Trk::SubtractedPlaneSurface(*plo, volEx, false));
      if (clo)
        retsf->push_back(
          new Trk::SubtractedCylinderSurface(*clo, volEx, false));
      if (dlo) {
        // turn disc into ellipse for simplification
        const DiscBounds* db =
          dynamic_cast<const DiscBounds*>(&(dlo->bounds()));
        if (!db)
          throw std::logic_error("Not DiscBounds");
        EllipseBounds* eb = new EllipseBounds(
          db->rMin(), db->rMin(), db->rMax(), db->rMax(), db->halfPhiSector());
        plo = new PlaneSurface(Amg::Transform3D(dlo->transform()), eb);
        retsf->push_back(new Trk::SubtractedPlaneSurface(*plo, volEx, false));
        delete plo;
      }
    }
  }

  // loop over 'inner' boundary surfaces; include only if represent a new
  // surface change: include allways otherwise orientation messed up bonus :
  // solves 'double boundary' problem

  cylVol =
    dynamic_cast<const Trk::CylinderVolumeBounds*>(&(m_inner->volumeBounds()));
  spbVol = dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*>(
    &(m_inner->volumeBounds()));
  comVol =
    dynamic_cast<const Trk::CombinedVolumeBounds*>(&(m_inner->volumeBounds()));
  subVol = dynamic_cast<const Trk::SubtractedVolumeBounds*>(
    &(m_inner->volumeBounds()));
  unsigned int nOut = outerSurfaces->size();

  for (unsigned int in = 0; in < innerSurfaces->size(); in++) {
    const SubtractedPlaneSurface* spli =
      dynamic_cast<const SubtractedPlaneSurface*>((*innerSurfaces)[in]);
    const PlaneSurface* pli =
      dynamic_cast<const PlaneSurface*>((*innerSurfaces)[in]);
    const SubtractedCylinderSurface* scli =
      dynamic_cast<const SubtractedCylinderSurface*>((*innerSurfaces)[in]);
    const CylinderSurface* cli =
      dynamic_cast<const CylinderSurface*>((*innerSurfaces)[in]);
    const DiscSurface* dli =
      dynamic_cast<const DiscSurface*>((*innerSurfaces)[in]);
    // resolve bounds orientation : copy from combined/subtracted, swap inner
    // cyl, swap bottom spb, swap all
    if (comVol)
      m_boundsOrientation[nOut + in] = !comVol->boundsOrientation()[in];
    else if (subVol)
      m_boundsOrientation[nOut + in] = !subVol->boundsOrientation()[in];
    else if (cylVol && cli && in == 3)
      m_boundsOrientation[nOut + in] = true;
    else if (spbVol && in == 0)
      m_boundsOrientation[nOut + in] = true;
    else
      m_boundsOrientation[nOut + in] = false;
    //
    Trk::Volume* outerSub = createSubtractedVolume(
      (*innerSurfaces)[in]->transform().inverse() * transf, m_outer);

    if (spli || scli) {
      bool shared = false;
      SharedObject<Trk::AreaExcluder> vEx;
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
      Trk::Volume* innerSub = new Trk::Volume(*volExcl->volume());

      // combined volume
      Trk::Volume* comb_sub = nullptr;
      if (!shared)
        comb_sub = new Trk::Volume(
          nullptr, new Trk::SubtractedVolumeBounds(outerSub, innerSub));
      else
        comb_sub = new Trk::Volume(
          nullptr, new Trk::CombinedVolumeBounds(innerSub, outerSub, true));
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(comb_sub);
      if (spli)
        retsf->push_back(new Trk::SubtractedPlaneSurface(*spli, volEx, true));
      if (scli)
        retsf->push_back(
          new Trk::SubtractedCylinderSurface(*scli, volEx, true));

    } else if (pli || cli) {
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(outerSub);
      if (pli)
        retsf->push_back(new Trk::SubtractedPlaneSurface(*pli, volEx, true));
      if (cli)
        retsf->push_back(new Trk::SubtractedCylinderSurface(*cli, volEx, true));
    } else if (dli) {
      // turn disc into ellipse for simplification
      const DiscBounds* db = dynamic_cast<const DiscBounds*>(&(dli->bounds()));
      if (!db)
        throw std::logic_error("Not DiscBounds");
      EllipseBounds* eb = new EllipseBounds(
        db->rMin(), db->rMin(), db->rMax(), db->rMax(), db->halfPhiSector());
      PlaneSurface pla(Amg::Transform3D(dli->transform()), eb);
      Trk::VolumeExcluder* volEx = new Trk::VolumeExcluder(outerSub);
      retsf->push_back(new Trk::SubtractedPlaneSurface(pla, volEx, true));
    } else {
      throw std::runtime_error(
        "Unhandled surface in "
        "Trk::SubtractedVolumeBounds::decomposeToSurfaces.");
    }
  }

  for (size_t i = 0; i < outerSurfaces->size(); i++)
    delete (*outerSurfaces)[i];
  for (size_t i = 0; i < innerSurfaces->size(); i++)
    delete (*innerSurfaces)[i];
  delete outerSurfaces;
  delete innerSurfaces;

  return retsf;
}

// ostream operator overload

MsgStream&
Trk::SubtractedVolumeBounds::dump(MsgStream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::SubtractedVolumeBounds: outer,inner ";
  sl << temp_sl.str();
  std::as_const(*m_outer).volumeBounds().dump(sl);
  std::as_const(*m_inner).volumeBounds().dump(sl);
  return sl;
}

std::ostream&
Trk::SubtractedVolumeBounds::dump(std::ostream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::SubtractedVolumeBounds: outer,inner ";
  sl << temp_sl.str();
  std::as_const(*m_outer).volumeBounds().dump(sl);
  std::as_const(*m_inner).volumeBounds().dump(sl);
  return sl;
}

Trk::Volume*
Trk::SubtractedVolumeBounds::createSubtractedVolume(
  const Amg::Transform3D& transf,
  Trk::Volume* subtrVol) 
{
  Trk::Volume* subVol = nullptr;
  if (!subtrVol)
    return subVol;

  subVol = new Trk::Volume(*subtrVol, transf);

  return subVol;
}

