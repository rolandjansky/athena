/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoVAlignmentStore.h"
#include <string>

GeoVFullPhysVol::GeoVFullPhysVol(const GeoLogVol* logVol)
  : GeoVPhysVol(logVol)
  , m_id(nullptr)
  , m_absPosInfo(nullptr)
{
}

GeoVFullPhysVol::~GeoVFullPhysVol()
{
  delete m_absPosInfo;
  delete m_id;
}

const HepGeom::Transform3D & GeoVFullPhysVol::getAbsoluteTransform(GeoVAlignmentStore* store) const
{
  //------------------------------------------------------------------------------------------------//     
  //                                                                                                //     
  // Get ready for something to go wrong:                                                           //     
  //                                                                                                //     
  static std::string errorMessage("Full Physical Volume errroneously placed in a shared portion of a detector description graph.\nName of shared volume:  ");
  //                                                                                                //     
  //------------------------------------------------------------------------------------------------//     
  if(isShared()) throw std::runtime_error(errorMessage);

  if(store==nullptr && !m_absPosInfo) m_absPosInfo = new GeoAbsPositionInfo();

  //
  // Check the cache first. If not empty, then return the cached value
  //
  if(store==nullptr){
    if(m_absPosInfo->getAbsTransform()) return *m_absPosInfo->getAbsTransform();
  }
  else {
    const HepGeom::Transform3D* storedPos = store->getAbsPosition(this);
    if(storedPos!=nullptr) return *storedPos;
  }

  // The cache is empty. Compute the absolute position from the top of the tree down to here, and cache it 
  PVConstLink child(this), parent(getParent());
  HepGeom::Transform3D tProd;

  while (parent) {
#ifdef HOSED
    int index = parent->indexOf (child);
    HepGeom::Transform3D transform = parent->getXToChildVol(index,store);
#else
    HepGeom::Transform3D transform = child->getX(store);
#endif
    tProd = transform * tProd;
    child = parent;
    if(child->isShared()) {
      throw std::runtime_error(errorMessage);
    }
    else {
      parent = child->getParent();
    }
  }

  if(store==nullptr) {
    m_absPosInfo->setAbsTransform(tProd);
    return *m_absPosInfo->getAbsTransform();
  }
  else {
    store->setAbsPosition(this,tProd);
    const HepGeom::Transform3D* storedPosition = store->getAbsPosition(this);
    return *storedPosition;
  }
}

void GeoVFullPhysVol::clearPositionInfo() const
{
  delete m_absPosInfo;
  m_absPosInfo = nullptr;
}

const HepGeom::Transform3D& GeoVFullPhysVol::getDefAbsoluteTransform(GeoVAlignmentStore* store) const
{
  //------------------------------------------------------------------------------------------------//     
  //                                                                                                //     
  // Get ready for something to go wrong:                                                           //     
  //                                                                                                //     
  static std::string errorMessage("Full Physical Volume errroneously placed in a shared portion of a detector description graph.\nName of shared volume:  ");
  //                                                                                                //     
  //------------------------------------------------------------------------------------------------//     
  if(isShared()) throw std::runtime_error(errorMessage);

  if(store==nullptr && !m_absPosInfo) m_absPosInfo = new GeoAbsPositionInfo();

  //
  // Check the cache first. If not empty, then return the cached value
  //
  if(store==nullptr){
    if(m_absPosInfo->getDefAbsTransform()) return *m_absPosInfo->getDefAbsTransform();
  }
  else {
    const HepGeom::Transform3D* storedPos = store->getDefAbsPosition(this);
    if(storedPos!=nullptr) return *storedPos;
  }

  // The cache is empty. Compute the absolute position from the top of the tree down to here, and cache it 
  PVConstLink child(this), parent(getParent());
  HepGeom::Transform3D tProd;

  while (parent) {
#ifdef HOSED
    int index = parent->indexOf (child);
    HepGeom::Transform3D transform = parent->getDefXToChildVol(index,store);
#else
    HepGeom::Transform3D transform = child->getDefX(store);
#endif
    tProd = transform * tProd;
    child = parent;
    if(child->isShared()) {
      throw std::runtime_error(errorMessage);
    }
    else {
      parent = child->getParent();
    }
  }

  if(store==nullptr) {
    m_absPosInfo->setDefAbsTransform(tProd);
    return *m_absPosInfo->getDefAbsTransform();
  }
  else {
    store->setDefAbsPosition(this,tProd);
    const HepGeom::Transform3D* storedPosition = store->getAbsPosition(this);
    return *storedPosition;
  }
}

const std::string &  GeoVFullPhysVol::getAbsoluteName ()
{
  //------------------------------------------------------------------------------------------------//     
  //                                                                                                //     
  // Get ready for something to go wrong:                                                           //     
  //                                                                                                //     
  static std::string errorMessage("Full Physical Volume errroneously placed in a shared portion of a detector description graph.\nName of shared volume:  ");
  //                                                                                                //     
  //------------------------------------------------------------------------------------------------//     

  if(m_absName == "") {
    if(isShared()) throw std::runtime_error(errorMessage);

    //
    // Check the cache.  If it is empty, compute the absolute position from the
    // top of the tree down to here, and update the cache
    //
    PVConstLink child(this), parent(getParent());
    std::string tProd;

    while (parent) {
      int index = parent->indexOf (child);
      std::string name = parent->getNameOfChildVol (index);

      tProd = tProd + "/" + name;
      child = parent;
      if (child->isShared ()) {
        throw std::runtime_error(errorMessage);
      }
      else {
        parent = child->getParent ();
      }
    }
    m_absName = tProd;
  }

  return m_absName;
}

unsigned int GeoVFullPhysVol::getId () const
{
  //------------------------------------------------------------------------------------------------//     
  //                                                                                                //     
  // Get ready for something to go wrong:                                                           //     
  //                                                                                                //     
  static std::string errorMessage("Full Physical Volume errroneously placed in a shared portion of a detector description graph.\nName of shared volume:  ");
  //                                                                                                //     
  //------------------------------------------------------------------------------------------------//     

  if(m_id==nullptr) {
    if(isShared()) throw std::runtime_error(errorMessage);

    //     
    // Check the cache.  If it is empty, compute the absolute position from the     
    // top of the tree down to here, and update the cache     
    //     
    PVConstLink child = this, parent = nullptr;
    if(child->isShared()) {
      throw std::runtime_error(errorMessage);
    }	
    else {
      parent = child->getParent();
    }

    int index = parent->indexOf(child);
    m_id = new Query<int>(parent->getIdOfChildVol(index));
  }
  return *m_id;
}

