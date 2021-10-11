/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataCollBase                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataCollBase.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataHandleBase.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1Msg.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

//____________________________________________________________________
class VP1RawDataCollBase::Imp {
public:
  QString key;
  bool commonCuts(VP1RawDataHandleBase*handle);
  VP1ExtraSepLayerHelper * sepHelper = nullptr;
  VP1Interval allowedEta;
  QList<VP1Interval> allowedPhi;
  VP1RawDataFlags::InDetPartsFlags allowedInDetParts;
  std::vector<VP1RawDataHandleBase*> handles;
  int iupdategui = 0;
};


//____________________________________________________________________
VP1RawDataCollBase::VP1RawDataCollBase(VP1RawDataCommonData*common,const QString& key)
  : VP1StdCollection(common->system(),"RawDataColl_"+key), m_common(common), m_d(new Imp)
{
  m_d->key = key;
  m_d->sepHelper = 0;
  m_d->iupdategui = 0;
}

//____________________________________________________________________
void VP1RawDataCollBase::init(VP1MaterialButtonBase*)
{
  VP1StdCollection::init();//Must be here
  m_d->allowedInDetParts = (usesInDetPartsCuts() ? VP1RawDataFlags::None : VP1RawDataFlags::All);
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));

  connect(m_common->controller(),SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),
	  this,SLOT(setAllowedEta(const VP1Interval&)));
  setAllowedEta(m_common->controller()->cutAllowedEta());

  connect(m_common->controller(),SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),
	  this,SLOT(setAllowedPhi(const QList<VP1Interval>&)));
  setAllowedPhi(m_common->controller()->cutAllowedPhi());

  connect(m_common->controller(),SIGNAL(enabledInDetPartsChanged(VP1RawDataFlags::InDetPartsFlags)),
	  this,SLOT(setEnabledInDetParts(VP1RawDataFlags::InDetPartsFlags)));
  setEnabledInDetParts(m_common->controller()->enabledInDetParts());
}

//____________________________________________________________________
VP1ExtraSepLayerHelper * VP1RawDataCollBase::sepHelper() const
{
  if (!m_d->sepHelper)
    m_d->sepHelper = new VP1ExtraSepLayerHelper(collSep());
  return m_d->sepHelper;
}

//____________________________________________________________________
VP1RawDataCollBase::~VP1RawDataCollBase()
{
  std::vector<VP1RawDataHandleBase*>::iterator it(m_d->handles.begin()),itE(m_d->handles.end());
  for (;it!=itE;++it)
    delete *it;
  m_d->handles.clear();

  delete m_d->sepHelper;
  delete m_d;
}

//____________________________________________________________________
QString VP1RawDataCollBase::provideText() const
{
  return m_d->key;
}

//____________________________________________________________________
void VP1RawDataCollBase::collVisibilityChanged(bool vis)
{
  if (VP1Msg::verbose())
    messageVerbose("TrackCollHandleBase::collVisibilityChanged => "+str(vis));
  if (vis)
    recheckCutStatusOfAllNotVisibleHandles();//Fixme -> ofallhandles? All must be not visible anyway...
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void VP1RawDataCollBase::setAllowedEta(const VP1Interval&e)
{
  if (m_d->allowedEta==e)
    return;
  bool relaxed(e.contains(m_d->allowedEta));
  bool tightened(m_d->allowedEta.contains(e));
  m_d->allowedEta=e;
  if (relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else if (tightened)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllHandles();
}

//____________________________________________________________________
void VP1RawDataCollBase::setAllowedPhi(const QList<VP1Interval>&l)
{
  if (m_d->allowedPhi==l)
    return;
  m_d->allowedPhi=l;
  recheckCutStatusOfAllHandles();
}

//____________________________________________________________________
void VP1RawDataCollBase::setEnabledInDetParts(VP1RawDataFlags::InDetPartsFlags f)
{
  if (!usesInDetPartsCuts())
    return;
  if (m_d->allowedInDetParts==f)
    return;
  bool relaxed((m_d->allowedInDetParts&f) == m_d->allowedInDetParts);
  bool tightened((m_d->allowedInDetParts&f) == f);
  m_d->allowedInDetParts=f;
  if (relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else if (tightened)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllHandles();
}

//____________________________________________________________________
void VP1RawDataCollBase::addHandle(VP1RawDataHandleBase*handle)
{
  if (handle)
    m_d->handles.push_back(handle);
  if (!((m_d->iupdategui)++)%200)
    systemBase()->updateGUI();
}

//____________________________________________________________________
bool VP1RawDataCollBase::Imp::commonCuts(VP1RawDataHandleBase*handle)
{
  if (allowedInDetParts!=VP1RawDataFlags::All)
    if (!(handle->inInDetParts()&allowedInDetParts))
      return false;

  bool allPhiAllowed = allowedPhi.count()==1&&allowedPhi.at(0).isAllR();
  bool allEtaAllowed = allowedEta.isAllR();
  if (allEtaAllowed&&allPhiAllowed)
    return true;
  if (allowedPhi.isEmpty()||allowedEta.isEmpty())
    return false;
  Amg::Vector3D p = handle->center();
  if (!allEtaAllowed) {
    if (!allowedEta.contains(p.eta()))
      return false;
  }
  if (!allPhiAllowed) {
    double phi(p.phi());
    foreach(VP1Interval i,allowedPhi) {
      if (i.contains(phi)||i.contains(phi+2*M_PI)||i.contains(phi-2*M_PI))
	return true;
    }
    return false;
  }
  return true;
}

//____________________________________________________________________
void VP1RawDataCollBase::recheckCutStatus(VP1RawDataHandleBase*handle)
{
  handle->setVisible( visible() && cut(handle) && m_d->commonCuts(handle) );
}

//____________________________________________________________________
void VP1RawDataCollBase::recheckCutStatusOfAllHandles()
{
  if (!isLoaded())
    return;
  common()->system()->deselectAll();
  largeChangesBegin();
  std::vector<VP1RawDataHandleBase*>::iterator it(m_d->handles.begin()),itE(m_d->handles.end());
  int i(0);
  for (;it!=itE;++it) {
    recheckCutStatus(*it);
    if (!(i++%200))
      systemBase()->updateGUI();//since called from ::actualLoad<..>(..)
  }
  largeChangesEnd();
}

//____________________________________________________________________
void VP1RawDataCollBase::recheckCutStatusOfAllVisibleHandles()
{
  if (!isLoaded())
    return;
  //This method is called when a cut is tightened - thus we better start by deselectAll to avoid weird highlighting issues.
  common()->system()->deselectAll();

  largeChangesBegin();
  std::vector<VP1RawDataHandleBase*>::iterator it(m_d->handles.begin()),itE(m_d->handles.end());
  for (;it!=itE;++it) {
    if ((*it)->visible())
      recheckCutStatus(*it);
  }
  largeChangesEnd();
}

//____________________________________________________________________
void VP1RawDataCollBase::recheckCutStatusOfAllNotVisibleHandles()
{
  if (!isLoaded())
    return;
  largeChangesBegin();
  std::vector<VP1RawDataHandleBase*>::iterator it(m_d->handles.begin()),itE(m_d->handles.end());
  for (;it!=itE;++it) {
    if (!(*it)->visible())
      recheckCutStatus(*it);
  }
  largeChangesEnd();
}

//____________________________________________________________________
std::vector<VP1RawDataHandleBase*>& VP1RawDataCollBase::getHandles()
{
  return m_d->handles;
}
