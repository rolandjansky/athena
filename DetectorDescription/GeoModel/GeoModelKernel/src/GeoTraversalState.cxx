/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoTraversalState.h"

GeoTraversalState::GeoTraversalState ()
{
}

GeoTraversalState::~GeoTraversalState()
{
}

const HepGeom::Transform3D & GeoTraversalState::getTransform () const
{
  return m_transform;
}

const std::string & GeoTraversalState::getName () const
{
  return m_name;
}

const HepGeom::Transform3D & GeoTraversalState::getDefTransform () const
{
  return m_defTransform;
}

const std::string & GeoTraversalState::getAbsoluteName () const
{
  return m_absName;
}

const HepGeom::Transform3D & GeoTraversalState::getDefAbsoluteTransform () const
{
  return m_defAbsTransform;
}

const HepGeom::Transform3D & GeoTraversalState::getAbsoluteTransform () const
{
  return m_absTransform;
}

void GeoTraversalState::setTransform (const HepGeom::Transform3D &transform)
{
  m_transform = transform;
  m_absTransform = m_absTransformList.top () * transform;
}

void GeoTraversalState::setName (const std::string &name)
{
  m_name = name;
  m_absName = m_absNameList.top () + "/" + name;
}

void GeoTraversalState::setDefTransform (const HepGeom::Transform3D &transform)
{
  m_defTransform = transform;
  m_defAbsTransform = m_defAbsTransformList.top () * transform;
}

void GeoTraversalState::nextLevel (const GeoVPhysVol* pv)
{
  m_absNameList.push (m_absName);
  m_absTransformList.push (m_absTransform);
  m_defAbsTransformList.push (m_defAbsTransform);

  m_path.push(pv);
  //      
  // Reinitialize to identity.     
  //     

  m_absTransform = m_defAbsTransform = m_transform = m_defTransform =
    HepGeom::Transform3D ();
}

void GeoTraversalState::previousLevel ()
{
  m_absTransform = m_absTransformList.top ();
  m_defAbsTransform = m_defAbsTransformList.top ();
  m_absTransformList.pop ();
  m_defAbsTransformList.pop ();
  m_absNameList.pop ();
  m_path.pop();
  m_transform = m_defTransform = HepGeom::Transform3D ();
}

const GeoNodePath * GeoTraversalState::getPath () const
{
  return &m_path;
}

void GeoTraversalState::setId (const Query<int> &id)
{
  m_id=id;
}

const Query<int> GeoTraversalState::getId () const
{
  return m_id;
}
