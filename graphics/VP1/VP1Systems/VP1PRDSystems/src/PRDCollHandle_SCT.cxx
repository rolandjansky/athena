/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_SCT.h"
#include "VP1PRDSystems/PRDHandle_SCT.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include <QColor>

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_SCT::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasSCTGeometry() ? VP1SGContentsHelper(sys).getKeys<InDet::SCT_ClusterContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_SCT::Imp {
public:
  std::set<const InDetDD::SiDetectorElement*> touchedelements;

  PRDCommonFlags::InDetPartsFlags indetpartsflags;
  bool excludeisolatedclusters;
};

//____________________________________________________________________
PRDCollHandle_SCT::PRDCollHandle_SCT(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::SCT,cd,key), m_d(new Imp)
{
  m_d->indetpartsflags = (PRDCommonFlags::BarrelPositive | PRDCommonFlags::BarrelNegative | PRDCommonFlags::EndCapPositive | PRDCommonFlags::EndCapNegative);
  m_d->excludeisolatedclusters = true;
}

//____________________________________________________________________
PRDCollHandle_SCT::~PRDCollHandle_SCT()
{
  delete m_d;
}

//____________________________________________________________________
QColor PRDCollHandle_SCT::defaultColor() const
{
  return QColor::fromRgbF(1.0, 1.0, 0.5 );//light yellow
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_SCT::addPRD( const Trk::PrepRawData * prd )
{
  assert(dynamic_cast<const InDet::SCT_Cluster*>(prd));
  return new PRDHandle_SCT(this,static_cast<const InDet::SCT_Cluster*>(prd));
}

//____________________________________________________________________
void PRDCollHandle_SCT::postLoadInitialisation()
{
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it)
    m_d->touchedelements.insert(static_cast<PRDHandle_SCT*>(*it)->cluster()->detectorElement());
  m_d->touchedelements.insert(0);//To always show clusters whose elements have no otherSide() pointer.
}

//____________________________________________________________________
void PRDCollHandle_SCT::eraseEventDataSpecific()
{
  m_d->touchedelements.clear();
}

//____________________________________________________________________
bool PRDCollHandle_SCT::cut(PRDHandleBase*handlebase)
{
  PRDHandle_SCT * handle = static_cast<PRDHandle_SCT*>(handlebase);
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

  if (m_d->excludeisolatedclusters) {
    if (!m_d->touchedelements.count(handle->cluster()->detectorElement()->otherSide()))
      return false;
  }

  return true;
}

//____________________________________________________________________
void PRDCollHandle_SCT::setPartsFlags(PRDCommonFlags::InDetPartsFlags flags ) {
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
    PRDHandle_SCT* handle = static_cast<PRDHandle_SCT*>(*it);
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

//____________________________________________________________________
void PRDCollHandle_SCT::setExcludeIsolatedClusters(bool excludeisolated)
{
  if (m_d->excludeisolatedclusters==excludeisolated)
    return;
  m_d->excludeisolatedclusters=excludeisolated;
  if (excludeisolated)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
}

//____________________________________________________________________
void PRDCollHandle_SCT::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(inDetPartsFlagsChanged(PRDCommonFlags::InDetPartsFlags)),this,SLOT(setPartsFlags(PRDCommonFlags::InDetPartsFlags)));
  setPartsFlags(controller->inDetPartsFlags());

  connect(controller,SIGNAL(sctExcludeIsolatedClustersChanged(bool)),this,SLOT(setExcludeIsolatedClusters(bool)));
  setExcludeIsolatedClusters(controller->sctExcludeIsolatedClusters());
}
