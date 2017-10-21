/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class PRDCollHandle_SpacePoints         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1PRDSystems/PRDCollHandle_SpacePoints.h"
#include "VP1PRDSystems/PRDHandle_SpacePoint.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "VP1Base/IVP1System.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include <QColor>

//____________________________________________________________________
QStringList PRDCollHandle_SpacePoints::availableCollections(IVP1System*sys)
{
  if (!VP1JobConfigInfo::hasSCTGeometry()&&!VP1JobConfigInfo::hasPixelGeometry())
    return QStringList();
  QStringList keys = VP1SGContentsHelper(sys).getKeys<SpacePointContainer>();
  QStringList unsafekeys;
  if (!VP1JobConfigInfo::hasSCTGeometry()) {
    foreach (QString key,keys) {
      if (key.contains("sct",Qt::CaseInsensitive))
	unsafekeys << key;
    }
  }
  if (!VP1JobConfigInfo::hasPixelGeometry()) {
    foreach (QString key,keys) {
      if (key.contains("pixel",Qt::CaseInsensitive))
	unsafekeys << key;
    }
  }
  foreach (QString unsafekey,unsafekeys)
    keys.removeAll(unsafekey);
  return keys;
}

//____________________________________________________________________
class PRDCollHandle_SpacePoints::Imp {
public:
  PRDCommonFlags::InDetPartsFlags indetpartsflags;
};


//____________________________________________________________________
PRDCollHandle_SpacePoints::PRDCollHandle_SpacePoints(PRDSysCommonData* common,const QString& key)
  : PRDCollHandleBase(PRDDetType::SpacePoints,common,key), m_d(new Imp)
{
  m_d->indetpartsflags = (PRDCommonFlags::BarrelPositive | PRDCommonFlags::BarrelNegative | PRDCommonFlags::EndCapPositive | PRDCommonFlags::EndCapNegative);
}

//____________________________________________________________________
PRDCollHandle_SpacePoints::~PRDCollHandle_SpacePoints()
{
  delete m_d;
}

//____________________________________________________________________
QColor PRDCollHandle_SpacePoints::defaultColor() const
{
  if (text().contains("pixel",Qt::CaseInsensitive))
    return QColor::fromRgbF(1.0, 0.666667, 0.5 );//beige
  else
    return QColor::fromRgbF(1.0, 1.0, 0.5 );//light yellow
}

//____________________________________________________________________
bool PRDCollHandle_SpacePoints::load()
{
  /////////////////////////////////////////////////////////////////////////
  // Retrieve element container from event store:
  const SpacePointContainer* container;
  if(!VP1SGAccessHelper(systemBase()).retrieve(container,text()))
    return false;

  int isp(0);
  SpacePointContainer::const_iterator itElement, itElementE(container->end());
  for (itElement=container->begin();itElement!=itElementE;++itElement) {
    SpacePointContainer::base_value_type::const_iterator it, itE((*itElement)->end());
    for (it=(*itElement)->begin();it!=itE;++it) {
      const Trk::SpacePoint * sp = *it;
      ++isp;
      if (!sp) {
	message("WARNING - ignoring null spacepoint pointer.");
	continue;
      }
      if (!sp->clusterList().first) {
	message("WARNING - ignoring spacepoint with null first cluster.");
	continue;
      }
      PRDHandleBase * handle = new PRDHandle_SpacePoint(this,sp);
      if (handle) {
	addHandle(handle);
	//We just register the first of the (possibly) two prds here (fixme: check that it works):
	common()->registerPRD2Handle(handle->getPRD(),handle);
      } else {
	message("WARNING - ignoring null handle pointer.");
      }
      if (!(isp%100))
	systemBase()->updateGUI();
    }
  }
  return true;
}

//____________________________________________________________________
bool PRDCollHandle_SpacePoints::cut(PRDHandleBase*handlebase)
{
  PRDHandle_SpacePoint * handle = static_cast<PRDHandle_SpacePoint*>(handlebase);
  assert(handle);

  if (m_d->indetpartsflags!=PRDCommonFlags::All) {
    if (handle->isBarrel()) {
      if (!(handle->isPositiveZ()?(m_d->indetpartsflags&PRDCommonFlags::BarrelPositive):(m_d->indetpartsflags&PRDCommonFlags::BarrelNegative)))
 	return false;
    } else {
      if (!(handle->isPositiveZ()?(m_d->indetpartsflags&PRDCommonFlags::EndCapPositive):(m_d->indetpartsflags&PRDCommonFlags::EndCapNegative)))
 	return false;
    }
  }
  return true;
}

//____________________________________________________________________
void PRDCollHandle_SpacePoints::eraseEventDataSpecific()
{
}

//____________________________________________________________________
void PRDCollHandle_SpacePoints::postLoadInitialisation()
{
}

//____________________________________________________________________
void PRDCollHandle_SpacePoints::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(inDetPartsFlagsChanged(PRDCommonFlags::InDetPartsFlags)),
	  this,SLOT(setPartsFlags(PRDCommonFlags::InDetPartsFlags)));
  setPartsFlags(controller->inDetPartsFlags());
}

//____________________________________________________________________
void PRDCollHandle_SpacePoints::setPartsFlags(PRDCommonFlags::InDetPartsFlags flags)
{
  //NB: The code is this method is very similar in PRDCollHandle_Pixel::setPartsFlags, PRDCollHandle_SCT::setPartsFlags,
  //PRDCollHandle_TRT::setPartsFlags and and PRDCollHandle_SpacePoints::setPartsFlags
  //Fixme: base decision to recheck on visibility also!

  if (m_d->indetpartsflags==flags)
    return;

  bool barrelPosChanged = (m_d->indetpartsflags&PRDCommonFlags::BarrelPositive)!=(flags&PRDCommonFlags::BarrelPositive);
  bool barrelNegChanged = (m_d->indetpartsflags&PRDCommonFlags::BarrelNegative)!=(flags&PRDCommonFlags::BarrelNegative);
  bool endcapPosChanged = (m_d->indetpartsflags&PRDCommonFlags::EndCapPositive)!=(flags&PRDCommonFlags::EndCapPositive);
  bool endcapNegChanged = (m_d->indetpartsflags&PRDCommonFlags::EndCapNegative)!=(flags&PRDCommonFlags::EndCapNegative);
  bool barrelChanged = (barrelPosChanged || barrelNegChanged);
  bool endcapChanged = (endcapPosChanged || endcapNegChanged);
  m_d->indetpartsflags=flags;

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it) {
    PRDHandle_SpacePoint* handle = static_cast<PRDHandle_SpacePoint*>(*it);
    if (handle->isBarrel()) {
      if (barrelChanged&&(handle->isPositiveZ()?barrelPosChanged:barrelNegChanged))
	recheckCutStatus(handle);
    } else {
      if (endcapChanged&&(handle->isPositiveZ()?endcapPosChanged:endcapNegChanged))
	recheckCutStatus(handle);
    }
  }
  largeChangesEnd();
}
