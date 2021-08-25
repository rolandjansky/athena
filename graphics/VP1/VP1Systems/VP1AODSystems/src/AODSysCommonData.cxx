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
#include "AODHandleBase.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>

#include <QTreeWidgetItem>

#include<map>
#include "xAODBase/IParticle.h"
#include <Inventor/SoPath.h>

//____________________________________________________________________
class AODSysCommonData::Imp {
public:
  std::map<SoNode*,AODHandleBase*> nodeToHandle;
  AODHandleBase* last_selectedHandle = nullptr;
};

//____________________________________________________________________
AODSysCommonData::AODSysCommonData(VP1AODSystem * sys,AODSystemController * controller)
  : VP1HelperClassBase(sys,"AODSysCommonData"), m_d(new Imp), m_3dsystem(sys),
    m_controller(controller), m_extrapolator(0)
{
  m_singlePoint = new SoPointSet;
  m_singlePoint->ref();
  SoVertexProperty * vertices = new SoVertexProperty;
  vertices->vertex.set1Value(0,0.0f,0.0f,0.0f);
  m_singlePoint->numPoints=1;
  m_singlePoint->vertexProperty.setValue(vertices);
  
  m_d->last_selectedHandle=0;
}

//____________________________________________________________________
AODSysCommonData::~AODSysCommonData()
{
  m_singlePoint->unref();
  delete m_d;
}

void AODSysCommonData::registerHandle( AODHandleBase* h ){
  // if (m_d->nodeToHandle.find(node)!=m_d->nodeToHandle.end()){
  //   messageVerbose("AODSysCommonData::registerHandle - handle already registered.");
  // }
  m_d->nodeToHandle[h->nodes()]=h;
}

void AODSysCommonData::deregisterHandle( AODHandleBase* h ){
  // if (m_d->nodeToTrackHandle.find(node)==m_d->nodeToHandle.end()){
  //   message("AODSysCommonData::deregisterHandle - handle not registered!");
  // }
  
  for (auto it : m_d->nodeToHandle) {
    if (it.second == h) {
      m_d->nodeToHandle.erase(it.first);
      break;
    }
  }
}
AODHandleBase* AODSysCommonData::getHandleFromNode( SoNode* node ) {
  auto it = m_d->nodeToHandle.find(node);
  if (it==m_d->nodeToHandle.end()){
    message("AODSysCommonData::getHandleFromNode - handle not registered!");
    message("AODSysCommonData::getHandleFromNode - have this many handles:"+str(m_d->nodeToHandle.size()));
    
    return 0;
  }
  return it->second;
}

//____________________________________________________________________
AODHandleBase* AODSysCommonData::getHandleFromNode(const SoPath*path) {
  const int n(path?path->getLength():0);
  AODHandleBase * handle(0);
  for (int i = 0; i < n; ++i) {
    if (path->getNodeFromTail(i)->getTypeId()==SoSeparator::getClassTypeId()) {
      handle = getHandleFromNode(static_cast<SoSeparator*>(path->getNodeFromTail(i)));
      if (handle)
        return handle;
    }
  }
  return 0;
}

SoNode* AODSysCommonData::getNodeFromBrowser( QTreeWidgetItem* item ){
  message("AODSysCommonData::getNodeFromBrowser - about to loop over this many handles:"+str(m_d->nodeToHandle.size()));
  if (!item)
    return 0;
  std::map<SoNode*,AODHandleBase*>::iterator it = m_d->nodeToHandle.begin(), itEnd=m_d->nodeToHandle.end();
  for (; it!=itEnd;++it)
    if (it->second->browserTreeItem()==item) return it->first;
  return 0;
}



void AODSysCommonData::setLastSelectedHandle(AODHandleBase* h) {
  m_d->last_selectedHandle=h;
}
