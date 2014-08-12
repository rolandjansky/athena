/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AODSysCommonData                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "AODSysCommonData.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1AODSystems/AODSystemController.h"
#include "IParticleHandleBase.h"

#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>

#include <QTreeWidgetItem>

#include<map>
#include "xAODBase/IParticle.h"

//____________________________________________________________________
class AODSysCommonData::Imp {
public:
  std::map<SoNode*,IParticleHandleBase*> nodeToHandle;
};

//____________________________________________________________________
AODSysCommonData::AODSysCommonData(VP1AODSystem * sys,AODSystemController * controller)
  : VP1HelperClassBase(sys,"AODSysCommonData"), m_textSep(0), d(new Imp),
    m_controller(controller), m_lastSelectedTrack(0)
{
  m_3dsystem = sys;

  m_singlePoint = new SoPointSet;
  m_singlePoint->ref();
  SoVertexProperty * vertices = new SoVertexProperty;
  vertices->vertex.set1Value(0,0.0f,0.0f,0.0f);
  m_singlePoint->numPoints=1;
  m_singlePoint->vertexProperty.setValue(vertices);
}

//____________________________________________________________________
AODSysCommonData::~AODSysCommonData()
{
  m_singlePoint->unref();
  if (m_textSep) {
    m_textSep->unref();
    m_textSep = 0;
  }
  delete d;
}

//____________________________________________________________________
void AODSysCommonData::registerHandle(SoNode*node,IParticleHandleBase*handle)
{
  if (verbose()) {
    if (!node||!handle) {
      message("registerTrack ERROR: Received null pointer!");
      return;
    }
    std::map<SoNode*,IParticleHandleBase*>::iterator it = d->nodeToHandle.find(node);
    if (it!=d->nodeToHandle.end())
      message("registerTrack ERROR: Node already registered!");
  }
  d->nodeToHandle[node]=handle;
}

//____________________________________________________________________
void AODSysCommonData::unregisterHandle(SoNode*node)
{
  if (verbose()) {
    if (!node) {
      message("unregisterTrack ERROR: Received null pointer!");
      return;
    }
  }
  std::map<SoNode*,IParticleHandleBase*>::iterator it = d->nodeToHandle.find(node);
  if (it==d->nodeToHandle.end()) {
    message("unregisterTrack ERROR: Not previously registered!");
    return;
  }
  d->nodeToHandle.erase(it);
}

//____________________________________________________________________
IParticleHandleBase* AODSysCommonData::handle(SoNode*n)
{
  if (!n)
    return 0;
  std::map<SoNode*,IParticleHandleBase*>::iterator it = d->nodeToHandle.find(n);
  if (it!=d->nodeToHandle.end())
    return it->second;
  return 0;
}

//____________________________________________________________________
SoNode* AODSysCommonData::node(IParticleHandleBase* h)
{
  if (!h)
    return 0;
  std::map<SoNode*,IParticleHandleBase*>::iterator it = d->nodeToHandle.begin(), itEnd=d->nodeToHandle.end();
  for (; it!=itEnd;++it)
    if (it->second==h) return it->first;
  return 0;
}

//____________________________________________________________________
SoNode* AODSysCommonData::node(QTreeWidgetItem* item)
{
  if (!item)
    return 0;
  std::map<SoNode*,IParticleHandleBase*>::iterator it = d->nodeToHandle.begin(), itEnd=d->nodeToHandle.end();
  for (; it!=itEnd;++it)
    if (it->second->browserTreeItem()==item) return it->first;
  return 0;
}

const IParticleHandleBase* AODSysCommonData::getHandle(const xAOD::IParticle* obj)
{
  if (!obj) {
    messageVerbose("AODSysCommonData::getHandle(): Received null pointer!");
    return 0;
  }
//  messageVerbose("AODSysCommonData::getHandle(): about to loop over this many elements:"+QString::number(d->nodeToHandle.size()));

  std::map<SoNode*,IParticleHandleBase*>::iterator it = d->nodeToHandle.begin(), itEnd=d->nodeToHandle.end();
  for (; it!=itEnd;++it){
    if (it->second && &(it->second->iParticle())==obj) return it->second;
  }
  return 0;  
}
