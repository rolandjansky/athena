/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_TRT.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1PRDSystems/PRDHandle_TRT.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include <QColor>

#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_TRT::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasTRTGeometry() ? VP1SGContentsHelper(sys).getKeys<InDet::TRT_DriftCircleContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_TRT::Imp {
public:
  //Keep specific lists of handles here... i.e. those with a HT for instance
  //   std::vector<PRDHandle_TRT*> prdhandles;
  bool attempted_detmgrload;

  //We only load these on demand!
  const InDetDD::TRT_DetectorManager* trtdetmgr;
  const TRT_ID * idhelper;

  PRDCommonFlags::InDetPartsFlags indetpartsflags;
  double minToT;
  double maxToT;
  int minLE;
  int maxLE;
  bool needToTCheck;
  bool needLECheck;
  bool requireHT;
};

//____________________________________________________________________
PRDCollHandle_TRT::PRDCollHandle_TRT(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::TRT,cd,key), d(new Imp), m_highlightHT(false), m_project(false),m_appropriateProjections(InDetProjFlags::NoProjections)
{
  d->trtdetmgr = 0;
  d->idhelper = 0;
  d->attempted_detmgrload = false;
  d->indetpartsflags = (PRDCommonFlags::BarrelPositive | PRDCommonFlags::BarrelNegative | PRDCommonFlags::EndCapPositive | PRDCommonFlags::EndCapNegative);
  d->minToT = -0.5*3.125;
  d->maxToT = 24.5*3.125;
  d->minLE  = 0;
  d->maxLE  = 23;
  d->needToTCheck = false;
  d->needLECheck  = false;
  d->requireHT = false;
}

//____________________________________________________________________
PRDCollHandle_TRT::~PRDCollHandle_TRT()
{
  delete d;
}

//____________________________________________________________________
QColor PRDCollHandle_TRT::defaultColor() const
{
  return QColor::fromRgbF(1.0, 1.0, 1.0);//white

}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_TRT::addPRD( Trk::PrepRawData * prd )
{
  assert(dynamic_cast<InDet::TRT_DriftCircle*>(prd));
  return new PRDHandle_TRT(this,static_cast<InDet::TRT_DriftCircle*>(prd));
}

//____________________________________________________________________
bool PRDCollHandle_TRT::cut(PRDHandleBase*handlebase)
{
  PRDHandle_TRT * handle = static_cast<PRDHandle_TRT*>(handlebase);
  assert(handle);

  if (d->needToTCheck) {
    const double ToT = handle->driftCircle()->timeOverThreshold();
    if (ToT<d->minToT)
      return false;
    if (ToT>d->maxToT)
      return false;
  }

  if (d->needLECheck) {
    const int LE = handle->driftCircle()->driftTimeBin();//returns 0..24. 24 means no LT bits.
    if (LE>=0&&LE<24) {
      if (LE<d->minLE)
	return false;
      if (LE>d->maxLE)
	return false;
    }
  }


  if (d->indetpartsflags!=PRDCommonFlags::All) {
    if (handle->isBarrel()) {
      if (!(handle->isPositiveZ()?(d->indetpartsflags&PRDCommonFlags::BarrelPositive):(d->indetpartsflags&PRDCommonFlags::BarrelNegative)))
	return false;
    } else {
      if (!(handle->isPositiveZ()?(d->indetpartsflags&PRDCommonFlags::EndCapPositive):(d->indetpartsflags&PRDCommonFlags::EndCapNegative)))
	return false;
    }
  }

  if (d->requireHT&&!handle->highLevel())
    return false;

  return true;
}

//____________________________________________________________________
void PRDCollHandle_TRT::setPartsFlags(PRDCommonFlags::InDetPartsFlags flags ) {
  //NB: The code is this method is very similar in PRDCollHandle_Pixel::setPartsFlags, PRDCollHandle_SCT::setPartsFlags,
  //PRDCollHandle_TRT::setPartsFlags and and PRDCollHandle_SpacePoints::setPartsFlags
  //Fixme: base decision to recheck on visibility also!

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
    PRDHandle_TRT* handle = static_cast<PRDHandle_TRT*>(*it);
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
void PRDCollHandle_TRT::setMinToT(unsigned nbins)
{
  double newminToT = (nbins-0.5)*3.125;
  if (d->minToT==newminToT)
    return;
  bool cut_relaxed = (newminToT<d->minToT);
  d->minToT=newminToT;
  d->needToTCheck = (d->minToT>0.0||d->maxToT<24*3.125);
  if (cut_relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void PRDCollHandle_TRT::setMaxToT(unsigned nbins)
{
  double newmaxToT = (nbins+0.5)*3.125;
  if (d->maxToT==newmaxToT)
    return;
  bool cut_relaxed = (newmaxToT>d->maxToT);
  d->maxToT=newmaxToT;
  d->needToTCheck = (d->minToT>0.0||d->maxToT<24*3.125);
  if (cut_relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void PRDCollHandle_TRT::setMinLE(unsigned i)
{
  int newminLE = i-1;
  if (d->minLE==newminLE)
    return;
  bool cut_relaxed = (newminLE<d->minLE);
  d->minLE=newminLE;
  d->needLECheck = (d->minLE>0||d->maxLE<23);
  if (cut_relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void PRDCollHandle_TRT::setMaxLE(unsigned i)
{
  int newmaxLE = i-1;
  if (d->maxLE==newmaxLE)
    return;
  bool cut_relaxed = (newmaxLE>d->maxLE);
  d->maxLE=newmaxLE;
  d->needLECheck = (d->minLE>0||d->maxLE<23);
  if (cut_relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void PRDCollHandle_TRT::setRequireHT(bool reqHT)
{
  if (d->requireHT==reqHT)
    return;
  d->requireHT=reqHT;
  if (d->requireHT)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
  //NB: Since we assume the large majority are without HT, we do not
  //    base decision to recheck on presence of HT.
}

//____________________________________________________________________
void PRDCollHandle_TRT::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(projectTRTHitsChanged(bool)),this,SLOT(setProject(bool)));
  setProject(controller->projectTRTHits());//Fixme: Do in pixel/sct as well!

  connect(controller,SIGNAL(highLightTRTHitsByHighThresholdChanged(bool)),this,SLOT(setHighLightHighThresholds(bool)));
  setHighLightHighThresholds(controller->highLightTRTHitsByHighThreshold());

  connect(controller,SIGNAL(inDetPartsFlagsChanged(PRDCommonFlags::InDetPartsFlags)),this,SLOT(setPartsFlags(PRDCommonFlags::InDetPartsFlags)));
  setPartsFlags(controller->inDetPartsFlags());

  connect(controller,SIGNAL(trtMinToTChanged(unsigned)),this,SLOT(setMinToT(unsigned)));
  setMinToT(controller->trtMinToT());

  connect(controller,SIGNAL(trtMaxToTChanged(unsigned)),this,SLOT(setMaxToT(unsigned)));
  setMaxToT(controller->trtMaxToT());

  connect(controller,SIGNAL(trtMinLEChanged(unsigned)),this,SLOT(setMinLE(unsigned)));
  setMinLE(controller->trtMinLE());

  connect(controller,SIGNAL(trtMaxLEChanged(unsigned)),this,SLOT(setMaxLE(unsigned)));
  setMaxLE(controller->trtMaxLE());

  connect(controller,SIGNAL(trtRequireHTChanged(bool)),this,SLOT(setRequireHT(bool)));
  setRequireHT(controller->trtRequireHT());
}

//____________________________________________________________________
void PRDCollHandle_TRT::setHighLightHighThresholds(bool hl)
{
  if (m_highlightHT==hl)
    return;
  m_highlightHT=hl;

  //Fixme: check PRDCollHandle::hasCustomHighlighting() before proceeding to loop here?.

  //call updateMaterial on all handles which have a high threshold.
  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it) {
    if (static_cast<PRDHandle_TRT*>(*it)->highLevel())
      (*it)->updateMaterial();
  }
  largeChangesEnd();

}

//____________________________________________________________________
void PRDCollHandle_TRT::setAppropriateProjection( InDetProjFlags::InDetProjPartsFlags f )
{
  //Fixme: use message:
  messageVerbose("setAppropriateProjection called.");
  if (m_appropriateProjections==f)
    return;
  messageVerbose("setAppropriateProjection => update needed.");

  InDetProjFlags::InDetProjPartsFlags changedparts = ( m_appropriateProjections ^ f );
  m_appropriateProjections = f;

  bool updateposbarrel = ( changedparts & InDetProjFlags::Barrel_AllPos );
  bool updatenegbarrel = ( changedparts & InDetProjFlags::Barrel_AllNeg );
  bool updateposendcap = ( changedparts & InDetProjFlags::EndCap_AllPos );
  bool updatenegendcap = ( changedparts & InDetProjFlags::EndCap_AllNeg );
  bool updatebarrel = updateposbarrel || updatenegbarrel;
  bool updateendcap = updateposendcap || updatenegendcap;

  messageVerbose("setAppropriateProjection updating (brlA,brlC,ecA,ecC) = ("+str(updateposbarrel)+","+str(updatenegbarrel)
		 +","+str(updateposendcap)+","+str(updatenegendcap)+")");

  if (!updatebarrel&&!updateendcap) {
    message("setAppropriateProjection WARNING: Unexpected flag value.");
    return;
  }

  common()->system()->deselectAll();
  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it) {
    PRDHandle_TRT* handle = static_cast<PRDHandle_TRT*>(*it);
    if (handle->isBarrel()) {
      if (updatebarrel) {
	if (handle->isPositiveZ()) {
	  if (updateposbarrel)
	    handle->update3DObjects();
	} else {
	  if (updatenegbarrel)
	    handle->update3DObjects();
	}
      }
    } else {
      if (updateendcap) {
	if (handle->isPositiveZ()) {
	  if (updateposendcap)
	    handle->update3DObjects();
	} else {
	  if (updatenegendcap)
	    handle->update3DObjects();
	}
      }
    }
  }
  largeChangesEnd();

}

//____________________________________________________________________
void PRDCollHandle_TRT::setProject( bool b)
{
  //Fixme: use message:
  messageVerbose("setProject called with project = "+str(b));

  if (m_project==b) {
    messageVerbose("setProject ==> No change.");
    return;
  }
  m_project=b;
  messageVerbose("setProject ==> Setting changed.");

  common()->system()->deselectAll();
  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it)
    static_cast<PRDHandle_TRT*>(*it)->update3DObjects();
  largeChangesEnd();

}
