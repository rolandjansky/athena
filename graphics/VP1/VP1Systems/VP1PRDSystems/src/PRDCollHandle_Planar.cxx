/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_Planar.h"
#include "VP1PRDSystems/PRDHandle_Planar.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "ISF_FatrasEvent/PlanarCluster.h"
#include <QtGui/QColor>

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "ISF_FatrasEvent/PlanarClusterContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_Planar::availableCollections(IVP1System*sys)
{
  return VP1SGContentsHelper(sys).getKeys<iFatras::PlanarClusterContainer>();
}

//____________________________________________________________________
class PRDCollHandle_Planar::Imp {
public:
  PRDCommonFlags::InDetPartsFlags indetpartsflags;
  unsigned minNRDOPerCluster;
};

//____________________________________________________________________
PRDCollHandle_Planar::PRDCollHandle_Planar(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::Pixel,cd,key), d(new Imp)
{
  d->indetpartsflags = (PRDCommonFlags::BarrelPositive | PRDCommonFlags::BarrelNegative | PRDCommonFlags::EndCapPositive | PRDCommonFlags::EndCapNegative);
  d->minNRDOPerCluster = 2;
}

//____________________________________________________________________
PRDCollHandle_Planar::~PRDCollHandle_Planar()
{
  delete d;
}

//____________________________________________________________________
bool PRDCollHandle_Planar::load() {
  return PRDCollHandleBase::loadPlanarClusters();
}

//____________________________________________________________________
QColor PRDCollHandle_Planar::defaultColor() const
{
  return QColor::fromRgbF(1.0, 0.666667, 0.5 );//beige
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_Planar::addPRD( Trk::PrepRawData * prd )
{
  assert(dynamic_cast<iFatras::PlanarCluster*>(prd));
  return new PRDHandle_Planar(this,static_cast<iFatras::PlanarCluster*>(prd));
}

//____________________________________________________________________
bool PRDCollHandle_Planar::cut(PRDHandleBase* /*handlebase*/ )
{
  // PRDHandle_Planar * handle = static_cast<PRDHandle_Planar*>(handlebase);
  // assert(handle);

  // if (d->indetpartsflags!=PRDCommonFlags::All) {
  //   if (handle->isBarrel()) {
  //     if (!(handle->isPositiveZ()?(d->indetpartsflags&PRDCommonFlags::BarrelPositive):(d->indetpartsflags&PRDCommonFlags::BarrelNegative)))
  // 	return false;
  //   } else {
  //     if (!(handle->isPositiveZ()?(d->indetpartsflags&PRDCommonFlags::EndCapPositive):(d->indetpartsflags&PRDCommonFlags::EndCapNegative)))
  // 	return false;
  //   }
  // }
  // if (d->minNRDOPerCluster&&handle->cluster()->rdoList().size()<d->minNRDOPerCluster)
  //   return false;
  return true;
}

//____________________________________________________________________
void PRDCollHandle_Planar::setPartsFlags(PRDCommonFlags::InDetPartsFlags flags ) {

  if (d->indetpartsflags==flags)
    return;

  bool barrelPosChanged = (d->indetpartsflags&PRDCommonFlags::BarrelPositive)!=(flags&PRDCommonFlags::BarrelPositive);
  bool barrelNegChanged = (d->indetpartsflags&PRDCommonFlags::BarrelNegative)!=(flags&PRDCommonFlags::BarrelNegative);
  bool endcapPosChanged = (d->indetpartsflags&PRDCommonFlags::EndCapPositive)!=(flags&PRDCommonFlags::EndCapPositive);
  bool endcapNegChanged = (d->indetpartsflags&PRDCommonFlags::EndCapNegative)!=(flags&PRDCommonFlags::EndCapNegative);
  bool barrelChanged = (barrelPosChanged || barrelNegChanged);
  bool endcapChanged = (endcapPosChanged || endcapNegChanged);
  d->indetpartsflags=flags;

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it) {
    PRDHandle_Planar* handle = static_cast<PRDHandle_Planar*>(*it);

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
void PRDCollHandle_Planar::setMinNRDOPerCluster(unsigned minnrdo)
{
  if (d->minNRDOPerCluster==minnrdo)
    return;
  bool cut_relaxed =  minnrdo<d->minNRDOPerCluster;
  d->minNRDOPerCluster = minnrdo;
  if (cut_relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void PRDCollHandle_Planar::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(inDetPartsFlagsChanged(PRDCommonFlags::InDetPartsFlags)),this,SLOT(setPartsFlags(PRDCommonFlags::InDetPartsFlags)));
  setPartsFlags(controller->inDetPartsFlags());

  connect(controller,SIGNAL(pixelMinNRDOPerClusterChanged(unsigned)),this,SLOT(setMinNRDOPerCluster(unsigned)));
  setMinNRDOPerCluster(controller->pixelMinNRDOPerCluster());
}
