/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackLODManager                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackLODManager.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoSeparator.h>
#include <map>

//____________________________________________________________________
class TrackLODManager::Imp {
public:
  Imp() : attachnode(0),
	  attachnode_lod(0),
	  attachsep_simple(0),
	  attachsep_detailed(0),
	  attachHelper_simple(0),
	  attachHelper_detailed(0),
	  detailLevel(TrackCommonFlags::AUTO) {}
  SoGroup * attachnode;
  SoGroup * attachnode_lod;
  SoSeparator * attachsep_simple;
  SoSeparator * attachsep_detailed;
  VP1ExtraSepLayerHelper * attachHelper_simple;
  VP1ExtraSepLayerHelper * attachHelper_detailed;
  void ensureLODDetached();
  void ensureSimpleDetached();
  void ensureDetailedDetached();
  void updateAttachmentForDetailLevel(TrackCommonFlags::DETAILLEVEL);

  TrackCommonFlags::DETAILLEVEL detailLevel;
  std::map<std::pair<int,double>,TrackLODHandle* > id_2_lodhandle;
};


//____________________________________________________________________
TrackLODManager::TrackLODManager(QObject * parent, IVP1System * sys)
  : QObject(parent), VP1HelperClassBase(sys,"TrackLODManager"), m_d(new Imp)
{
}

//____________________________________________________________________
TrackLODManager::~TrackLODManager()
{
  if (m_d->attachnode) {
    messageDebug("WARNING: Destructed before event data cleared!");
    eraseEventData();
  }
  delete m_d;
}

//____________________________________________________________________
void TrackLODManager::Imp::ensureDetailedDetached()
{
  if ( attachnode && attachsep_detailed && attachnode->findChild(attachsep_detailed)>-1)
    attachnode->removeChild(attachsep_detailed);
}

//____________________________________________________________________
void TrackLODManager::Imp::ensureSimpleDetached()
{
  if ( attachnode && attachsep_simple && attachnode->findChild(attachsep_simple)>-1)
    attachnode->removeChild(attachsep_simple);
}

//____________________________________________________________________
void TrackLODManager::Imp::ensureLODDetached()
{
  if ( attachnode && attachnode_lod && attachnode->findChild(attachnode_lod)>-1)
    attachnode->removeChild(attachnode_lod);
}

//____________________________________________________________________
void TrackLODManager::Imp::updateAttachmentForDetailLevel(TrackCommonFlags::DETAILLEVEL dl)
{
  if (dl == TrackCommonFlags::SIMPLE) {
    if ( attachnode && attachsep_simple && attachnode->findChild(attachsep_simple)<0)
      attachnode->addChild(attachsep_simple);
  } else {
    ensureSimpleDetached();
  }

  if (dl == TrackCommonFlags::DETAILED) {
    if ( attachnode && attachsep_detailed && attachnode->findChild(attachsep_detailed)<0)
      attachnode->addChild(attachsep_detailed);
  } else {
    ensureDetailedDetached();
  }

  if (dl == TrackCommonFlags::AUTO) {
    if ( attachnode && attachnode_lod && attachnode->findChild(attachnode_lod)<0)
      attachnode->addChild(attachnode_lod);
  } else {
    ensureLODDetached();
  }
}

//____________________________________________________________________
void TrackLODManager::setDetailLevel(TrackCommonFlags::DETAILLEVEL dl)
{
  if (verbose())
    messageVerbose("signal received in setDetailLevel (old = "+TrackCommonFlags::toString(m_d->detailLevel)+", new = "+TrackCommonFlags::toString(dl));
  if ( m_d->detailLevel == dl )
    return;
  m_d->detailLevel = dl;
  m_d->updateAttachmentForDetailLevel(dl);
}

//____________________________________________________________________
void TrackLODManager::setAttachNode(SoGroup* an)
{
  if (!an) {
    message("setAttachNode ERROR: Received null pointer!");
    return;
  }
  if (m_d->attachnode) {
    message("setAttachNode ERROR: Received new attachment group pointer while still having previous one!");
    eraseEventData();
    return;
  }

  m_d->attachnode = an;
  m_d->attachnode->ref();

  m_d->attachsep_simple = new SoSeparator;
  m_d->attachsep_simple->ref();
  m_d->attachsep_detailed = new SoSeparator;
  m_d->attachsep_detailed->ref();
  m_d->attachnode_lod = new SoGroup;
  m_d->attachnode_lod->ref();

  m_d->attachHelper_simple = new VP1ExtraSepLayerHelper(m_d->attachsep_simple);
  m_d->attachHelper_detailed = new VP1ExtraSepLayerHelper(m_d->attachsep_detailed);

  m_d->updateAttachmentForDetailLevel(m_d->detailLevel);
}

//____________________________________________________________________
void TrackLODManager::eraseEventData()
{
  messageVerbose("eraseEventData start");
  if (!m_d->attachnode) {
    messageDebug("eraseEventData WARNING: - called before attachment node was set!");
    return;
  }
  m_d->ensureSimpleDetached();
  m_d->ensureDetailedDetached();
  m_d->ensureLODDetached();

  std::map<std::pair<int,double>,TrackLODHandle* >::iterator it, itE(m_d->id_2_lodhandle.end());
  for (it = m_d->id_2_lodhandle.begin(); it!=itE;++it) {
    delete it->second;
  }
  m_d->id_2_lodhandle.clear();

  if (m_d->attachnode_lod) {
    m_d->attachnode_lod->unref();
    m_d->attachnode_lod = 0;
  }

  if (m_d->attachsep_simple) {
    delete m_d->attachHelper_simple;
    m_d->attachHelper_simple = 0;
    m_d->attachsep_simple->unref();
    m_d->attachsep_simple = 0;
  }

  if (m_d->attachsep_detailed) {
    delete m_d->attachHelper_detailed;
    m_d->attachHelper_detailed = 0;
    m_d->attachsep_detailed->unref();
    m_d->attachsep_detailed = 0;
  }

  m_d->attachnode->unref();
  m_d->attachnode = 0;
  messageVerbose("eraseEventData end");

}

//____________________________________________________________________
TrackLODHandle * TrackLODManager::getLODHandle(int regionindex, const double& crossover_value)
{
  std::map<std::pair<int,double>,TrackLODHandle* >::iterator it = m_d->id_2_lodhandle.find(std::make_pair(regionindex,crossover_value));
  if (it!=m_d->id_2_lodhandle.end())
    return it->second;
  if (!m_d->attachnode) {
    message("getTrackLODHandle ERROR: Called before attachment node was set!");
    return 0;
  }
  TrackLODHandle * lh = new TrackLODHandle(m_d->attachnode_lod,m_d->attachHelper_simple,m_d->attachHelper_detailed,regionindex,crossover_value);
  m_d->id_2_lodhandle[std::make_pair(regionindex,crossover_value)] = lh;
  return lh;
}


/////////////////////////////////////
//  TrackLODHandle implementation  //
/////////////////////////////////////


//____________________________________________________________________
class TrackLODHandle::Imp {
public:
  Imp( SoGroup* ag,
       VP1ExtraSepLayerHelper * shs,
       VP1ExtraSepLayerHelper * shd,
       int i, const double& c)
    : attachGroup_LOD(ag),
      attachHelper_simple(shs),
      attachHelper_detailed(shd),
      regIdx(i),
      crossVal(c),
      lod(0),
      sep_detailed(0),
      sep_simple(0) {}
  SoGroup* attachGroup_LOD;
  VP1ExtraSepLayerHelper * attachHelper_simple;
  VP1ExtraSepLayerHelper * attachHelper_detailed;
  int regIdx;
  double crossVal;
  SoLevelOfDetail * lod;
  SoSeparator * sep_detailed;
  SoSeparator * sep_simple;
  void updateCrossOverField()
  {
    //Fixme: We should scale this with complexity! I.e. multiply the value with 1.0/(complexity+0.5)
    //    lod->screenArea.setValue(crossVal/sqrt(sep_detailed->getNumChildren()+1.0));
    lod->screenArea.setValue(crossVal*sqrt(sep_detailed->getNumChildren())+0.1);
  }
};


//____________________________________________________________________
TrackLODHandle::TrackLODHandle(SoGroup* attachgroup_LOD,
			       VP1ExtraSepLayerHelper * sephelper_simple,
			       VP1ExtraSepLayerHelper * sephelper_detailed,
			       int regionindex, const double& crossover_value)
  : m_d(new Imp(attachgroup_LOD,sephelper_simple,sephelper_detailed,regionindex,crossover_value))
{
//   if (VP1Msg::verbose())
//     VP1Msg::messageVerbose("TrackLODHandle Constructed with regionIndex = "+QString::number(regionindex)
// 			    +" and nominal crossover value = "+QString::number(crossover_value));
  if (!attachgroup_LOD||!sephelper_simple||!sephelper_detailed)
    VP1Msg::messageDebug("TrackLODHandle ERROR: Received null pointer!");
}

//____________________________________________________________________
TrackLODHandle::~TrackLODHandle()
{
  if (m_d->lod) {
    if (m_d->attachGroup_LOD->findChild(m_d->lod)>-1) {
      m_d->attachGroup_LOD->removeChild(m_d->lod);
      m_d->attachHelper_simple->removeNode(m_d->sep_simple);
      m_d->attachHelper_detailed->removeNode(m_d->sep_detailed);
    }
    m_d->lod->unref();
    m_d->sep_detailed->unref();
    m_d->sep_simple->unref();
  }
  delete m_d;
}

//____________________________________________________________________
void TrackLODHandle::addNodes(SoGroup* simple,SoGroup*detailed )
{
  if (!m_d->lod) {
//     if (VP1Msg::verbose())
//       VP1Msg::messageVerbose( "TrackLODHandle addNodes: Initialising nodes: LOD, sep_detailed and sep_simple.");
    m_d->lod = new SoLevelOfDetail;
    m_d->sep_detailed = new SoSeparator;
    m_d->sep_simple = new SoSeparator;
    m_d->lod->ref();
    m_d->sep_detailed->ref();
    m_d->sep_simple->ref();
    m_d->lod->addChild(m_d->sep_detailed);
    m_d->lod->addChild(m_d->sep_simple);
  }

  if (VP1Msg::verbose()) {
    //Extra sanity checks:
    if (!detailed) {
      VP1Msg::messageVerbose("TrackLODHandle ERROR: addNodes received null pointer for detailed node");
      return;
    }
    if (!simple) {
      VP1Msg::messageVerbose("TrackLODHandle ERROR: addNodes received null pointer for simple node");
      return;
    }
    if (m_d->sep_detailed->findChild(detailed)>=0) {
      VP1Msg::messageVerbose("TrackLODHandle ERROR: addNodes called for detailed node which is already added");
      return;
    }
    if (m_d->sep_simple->findChild(simple)>=0) {
      VP1Msg::messageVerbose("TrackLODHandle ERROR: addNodes called for simple node which is already added");
      return;
    }
  }

  m_d->sep_detailed->addChild(detailed);
  m_d->sep_simple->addChild(simple);

  if (m_d->sep_detailed->getNumChildren()==1) {
    //We went from 0 to 1 children!
    if (VP1Msg::verbose()&&m_d->attachGroup_LOD->findChild(m_d->lod)>=0)
      VP1Msg::messageVerbose("TrackLODHandle ERROR: adding lod, but it is already a child!!");
    m_d->attachGroup_LOD->addChild(m_d->lod);
    m_d->attachHelper_simple->addNode(m_d->sep_simple);
    m_d->attachHelper_detailed->addNode(m_d->sep_detailed);
  }
  m_d->updateCrossOverField();
}

//____________________________________________________________________
void TrackLODHandle::removeNodes(SoGroup* simple,SoGroup*detailed )
{
  if (!m_d->lod)
    return;

  if (VP1Msg::verbose()) {
    //Extra sanity checks:
    if (!detailed) {
      VP1Msg::messageVerbose("TrackLODHandle ERROR: removeNodes received null pointer for detailed node");
      return;
    }
    if (!simple) {
      VP1Msg::messageVerbose("TrackLODHandle ERROR: removeNodes received null pointer for simple node");
      return;
    }
    if (m_d->sep_detailed->findChild(detailed)<0) {
      VP1Msg::messageVerbose("TrackLODHandle ERROR: removeNodes called for detailed node which is not already added");
      return;
    }
    if (m_d->sep_simple->findChild(simple)<0) {
      VP1Msg::messageVerbose("TrackLODHandle ERROR: removeNodes called for simple node which is not already added");
      return;
    }
  }
  m_d->sep_detailed->removeChild(detailed);
  m_d->sep_simple->removeChild(simple);

  if (m_d->sep_detailed->getNumChildren()<1) {
    //We went from 1 to 0 children
    if (VP1Msg::verbose()&&m_d->attachGroup_LOD->findChild(m_d->lod)<0)
      VP1Msg::messageVerbose("TrackLODHandle ERROR: removing child, but node is not currently a child!!");
    m_d->attachGroup_LOD->removeChild(m_d->lod);
    m_d->attachHelper_simple->removeNode(m_d->sep_simple);
    m_d->attachHelper_detailed->removeNode(m_d->sep_detailed);
  } else {
    m_d->updateCrossOverField();
  }
}
