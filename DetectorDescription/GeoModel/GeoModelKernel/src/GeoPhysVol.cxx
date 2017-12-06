/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNodeAction.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoCountVolAction.h"
#include "GeoModelKernel/GeoCountVolAndSTAction.h"
#include "GeoAccessVolumeAction.h"
#include <algorithm>

GeoPhysVol::GeoPhysVol(const GeoLogVol* LogVol)
  : GeoVPhysVol(LogVol)
{
}


GeoPhysVol::~GeoPhysVol()
{
  for(const GeoGraphNode* daughter : m_daughters) daughter->unref();
}

void GeoPhysVol::add(GeoGraphNode* graphNode)
{
  m_daughters.push_back(graphNode);
  graphNode->ref();
  graphNode->dockTo(this);
}

unsigned int GeoPhysVol::getNChildVols() const
{
  GeoCountVolAction cv;
  exec(&cv);
  return cv.getCount ();
}

PVConstLink GeoPhysVol::getChildVol(unsigned int index) const
{
  GeoAccessVolumeAction av(index,nullptr);
  exec(&av);
  return av.getVolume();
}

HepGeom::Transform3D GeoPhysVol::getXToChildVol(unsigned int index
						,const GeoVAlignmentStore* store) const
{
  GeoAccessVolumeAction av(index,store);
  exec(&av);
  return av.getTransform();
}

HepGeom::Transform3D GeoPhysVol::getDefXToChildVol(unsigned int index
						   ,const GeoVAlignmentStore* store) const
{
  GeoAccessVolumeAction av(index,store);
  exec(&av);
  return av.getDefTransform();
}

void GeoPhysVol::exec(GeoNodeAction *action) const
{
  //    
  // Put this node on the head of the path:    
  //    
  action->getPath()->push(this);
  if(action->getDepthLimit().isValid()
     && action->getPath()->getLength() - 1 > action->getDepthLimit()) {
    action->getPath ()->pop ();
    return;
  }
  //    
  // Perform the action on this node:    
  //    
  action->handlePhysVol(this);
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
    for(size_t c = 0; c < m_daughters.size (); c++) {
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

std::string GeoPhysVol::getNameOfChildVol(unsigned int i) const
{
  GeoAccessVolumeAction av(i,nullptr);
  exec(&av);
  return av.getName();
}

Query<int> GeoPhysVol::getIdOfChildVol(unsigned int i) const
{
  GeoAccessVolumeAction	av(i,nullptr);
  exec(&av);
  return av.getId();
}

unsigned int GeoPhysVol::getNChildVolAndST() const
{
  GeoCountVolAndSTAction cv;
  exec(&cv);
  return cv.getCount();
}

HepGeom::Transform3D GeoPhysVol::getX(const GeoVAlignmentStore* store) const {
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
    if(dynamic_cast<const GeoVPhysVol *>(*current)) break;               //
    //-------------------------------------------------------------------//

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if(dynamic_cast<const GeoSerialTransformer *>(*current)) break;      //
    //-------------------------------------------------------------------//

    const GeoTransform *xf = dynamic_cast<const GeoTransform *> (*current);
    
    //-------------------------------------------------------------------//
    // If this happens, accumulate into transform                        //
    if (xf) xform  = xf->getTransform(store)*xform;                      //
    //-------------------------------------------------------------------//
  }
  return xform;  
}

HepGeom::Transform3D GeoPhysVol::getDefX(const GeoVAlignmentStore* store) const {
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

unsigned int GeoPhysVol::getNChildNodes() const 
{
  return m_daughters.size();
}

const GeoGraphNode * const * GeoPhysVol::getChildNode(unsigned int i) const 
{
  return &(m_daughters[i]);
}

const GeoGraphNode * const * GeoPhysVol::findChildNode(const GeoGraphNode * n) const 
{
  std::vector<const GeoGraphNode *>::const_iterator i = std::find(m_daughters.begin(),m_daughters.end(),n);
  if (i==m_daughters.end()) {
    return nullptr;
  }
  else {
    return &*i;
  }
}
