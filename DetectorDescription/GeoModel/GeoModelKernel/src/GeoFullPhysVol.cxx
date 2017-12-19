/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNodeAction.h"
#include "GeoModelKernel/GeoCountVolAction.h"
#include "GeoModelKernel/GeoCountVolAndSTAction.h"
#include "GeoAccessVolumeAction.h"
#include <algorithm>

GeoFullPhysVol::GeoFullPhysVol (const GeoLogVol* LogVol)
  : GeoVFullPhysVol(LogVol)
  , m_cloneOrigin(nullptr)
{
}

GeoFullPhysVol::~GeoFullPhysVol()
{
  for(const GeoGraphNode* daughter : m_daughters) daughter->unref();
  if(m_cloneOrigin && m_cloneOrigin!=this) m_cloneOrigin->unref();
}

void GeoFullPhysVol::add(GeoGraphNode* graphNode)
{
  if(m_cloneOrigin) throw std::runtime_error("Attempt to modify contents of a cloned FPV");

  m_daughters.push_back(graphNode);
  graphNode->ref();
  graphNode->dockTo(this);
}

unsigned int GeoFullPhysVol::getNChildVols() const
{
  GeoCountVolAction cv;
  exec(&cv);
  return cv.getCount();
}

PVConstLink GeoFullPhysVol::getChildVol(unsigned int index) const
{
  GeoAccessVolumeAction av(index,nullptr);
  exec(&av);
  return av.getVolume();
}

HepGeom::Transform3D GeoFullPhysVol::getXToChildVol(unsigned int index, const GeoVAlignmentStore* store) const
{
  GeoAccessVolumeAction av(index,store);
  exec(&av);
  return av.getTransform();
}

HepGeom::Transform3D GeoFullPhysVol::getDefXToChildVol(unsigned int index, const GeoVAlignmentStore* store) const
{
  GeoAccessVolumeAction av(index,store);
  exec(&av);
  return av.getDefTransform();
}

void GeoFullPhysVol::exec(GeoNodeAction *action) const
{
  //        
  // Put this node on the head of the path:   bb 
  //    
  action->getPath()->push(this);
  if(action->getDepthLimit().isValid()
     && action->getPath()->getLength()-1 > action->getDepthLimit()) {
    action->getPath()->pop();
    return;
  }
  //    
  // Perform the action on this node:    
  //    
  action->handleFullPhysVol(this);
  if(action->shouldTerminate()) {
    action->getPath()->pop();
    return;
  }

  //    
  // Pass this on to the children.    
  //    
  if(action->getDepthLimit().isValid()
     && action->getPath()->getLength() > action->getDepthLimit()) {
  }
  else {
    for(size_t c = 0; c<m_daughters.size(); c++) {
      m_daughters[c]->exec(action);
      if(action->shouldTerminate()) {
	action->getPath()->pop();
	return;
      }
    }
  }

  //    
  // Take this node back off the head of the path:    
  //    
  action->getPath()->pop();
}

std::string GeoFullPhysVol::getNameOfChildVol(unsigned int i) const
{
  GeoAccessVolumeAction av(i,nullptr);
  exec(&av);
  return av.getName();
}

Query<int> GeoFullPhysVol::getIdOfChildVol(unsigned int i) const
{
  GeoAccessVolumeAction av(i,nullptr);
  exec(&av);
  return av.getId ();
}

unsigned int GeoFullPhysVol::getNChildVolAndST() const
{
  GeoCountVolAndSTAction cv;
  exec(&cv);
  return cv.getCount();
}

/// Meaning of the input parameter 'attached'
/// TRUE: all cloned volumes are meant to stay identical to their clone origin for the lifetime
///       further changes are permitted neither in the origin nor in the clone results
///
/// FALSE: use this value if you expect further changes in either clone origing or its clone results
///        which don't need to be syncronized. The clone origin and its clone are identical ONLY by
///        the time of cloning, further identity is not guaranteed
GeoFullPhysVol* GeoFullPhysVol::clone(bool attached)
{
  GeoFullPhysVol* clone = new GeoFullPhysVol(this->getLogVol());
  for(unsigned int ind = 0; ind < this->m_daughters.size(); ind++) {
    GeoGraphNode* daughter =(GeoGraphNode*) m_daughters[ind];
    clone->add(daughter);
  }

  if(attached) {
    if(this->m_cloneOrigin==0) {
      this->m_cloneOrigin = this;
    }
    clone->m_cloneOrigin = this->m_cloneOrigin;
    this->m_cloneOrigin->ref();
  }

  return clone;
}

const GeoFullPhysVol* GeoFullPhysVol::cloneOrigin() const
{
  return m_cloneOrigin;
}

/// Breaks the consistency of cloned volumes!
/// Use it only in Simulation jobs and
/// Don't call it until geometry has been completely translated to G4
void GeoFullPhysVol::clear()
{
  for(size_t i=0; i<m_daughters.size(); i++)
    m_daughters[i]->unref();
  m_daughters.clear();
}

HepGeom::Transform3D GeoFullPhysVol::getX(const GeoVAlignmentStore* store) const {
  //
  // Check we are not shared:
  //
  if (isShared()) throw std::runtime_error("Transform requested from shared volume");
  HepGeom::Transform3D xform;
  //
  // Get the address of the first graph node, from the parent:
  //
  const GeoGraphNode * const * fence =  getParent()->getChildNode(0);
  const GeoGraphNode * const * node1 =  getParent()->findChildNode(this);
  
  for(const GeoGraphNode * const * current = node1 - 1; current>=fence; current--) {

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if (dynamic_cast<const GeoVPhysVol *>(*current)) break;              //
    //-------------------------------------------------------------------//

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if (dynamic_cast<const GeoSerialTransformer *>(*current)) break;     //
    //-------------------------------------------------------------------//

    const GeoTransform *xf = dynamic_cast<const GeoTransform *> (*current);
    
    //-------------------------------------------------------------------//
    // If this happens, accumulate into transform                        //
    if (xf) xform  = xf->getTransform(store)*xform;                      //
    //-------------------------------------------------------------------//
  }
  return xform;  
}

HepGeom::Transform3D GeoFullPhysVol::getDefX(const GeoVAlignmentStore* store) const {
  //
  // Check we are not shared:
  //
  if (isShared()) throw std::runtime_error("Transform requested from shared volume");
  HepGeom::Transform3D xform;
  //
  // Get the address of the first graph node, from the parent:
  //
  const GeoGraphNode * const * fence =  getParent()->getChildNode(0);
  const GeoGraphNode * const * node1 =  getParent()->findChildNode(this);
  
  for(const GeoGraphNode * const * current = node1 - 1; current>=fence; current--) {

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if (dynamic_cast<const GeoVPhysVol *>(*current)) break;              //
    //-------------------------------------------------------------------//

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if (dynamic_cast<const GeoSerialTransformer *>(*current)) break;     //
    //-------------------------------------------------------------------//

    const GeoTransform *xf = dynamic_cast<const GeoTransform *> (*current);
    
    //-------------------------------------------------------------------//
    // If this happens, accumulate into transform                        //
    if (xf) xform  = xf->getDefTransform(store)*xform;                   //
    //-------------------------------------------------------------------//
  }
  return xform;
  
}

unsigned int GeoFullPhysVol::getNChildNodes() const 
{
  return m_daughters.size();
}

const GeoGraphNode * const * GeoFullPhysVol::getChildNode(unsigned int i) const 
{
  return &(m_daughters[i]);
}
const GeoGraphNode * const * GeoFullPhysVol::findChildNode(const GeoGraphNode * n) const 
{
  std::vector<const GeoGraphNode *>::const_iterator i = std::find(m_daughters.begin(),m_daughters.end(),n);
  if (i==m_daughters.end()) {
    return nullptr;
  }
  else {
    return &*i;
  }
}
