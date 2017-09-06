/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_MM.h"
#include "VP1PRDSystems/PRDHandle_MM.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "VP1PRDSystems/TouchedMuonChamberHelper.h"

#include "VP1Base/IVP13DSystem.h"
#include <QColor>

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepData.h"

//____________________________________________________________________
QStringList PRDCollHandle_MM::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasMuonGeometry() ? VP1SGContentsHelper(sys).getKeys<Muon::MMPrepDataContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_MM::Imp {
public:
  PRDCollHandle_MM * theclass;
  std::map<const MuonGM::MuonStation*,unsigned> mm2stationcounter;
  unsigned minHitsPerStation;
  VP1Interval allowedADCValues;
  bool excludeMaskedHits;
  bool onlyShowActive;
  QString status;
  bool projectionsEnabled;
  int appropriateprojection;
  void updateProjectionFlag();
};

//____________________________________________________________________
PRDCollHandle_MM::PRDCollHandle_MM(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::MM,cd,key), d(new Imp) //,
  // m_highLightMasked(false), m_highLightADCBelow(-1), m_projection(PRDCollHandle_MM::NONE)
{
  d->theclass = this;
  d->minHitsPerStation = 0;//If the intention is no cut, then put to 0 instead of 1.
  d->allowedADCValues = VP1Interval();
  d->excludeMaskedHits = true;
  d->onlyShowActive = true;
  d->projectionsEnabled = false;
  d->appropriateprojection = 0;//none
}

//____________________________________________________________________
PRDCollHandle_MM::~PRDCollHandle_MM()
{
  delete d;
}

//____________________________________________________________________
QColor PRDCollHandle_MM::defaultColor() const
{
  return QColor::fromRgbF( 1.0, 0.666667, 0.0);//orange
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_MM::addPRD( Trk::PrepRawData * prd )
{
  assert(dynamic_cast<Muon::MMPrepData*>(prd));
  return new PRDHandle_MM(this,static_cast<Muon::MMPrepData*>(prd));
}

//____________________________________________________________________
void PRDCollHandle_MM::postLoadInitialisation()
{
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  std::map<const MuonGM::MuonStation*,unsigned>::iterator itStation;
  for (;it!=itE;++it) {
    itStation = d->mm2stationcounter.find(static_cast<PRDHandle_MM*>(*it)->mm()->detectorElement()->parentMuonStation());
    if (itStation!=d->mm2stationcounter.end())
      ++(itStation->second);
    else
      d->mm2stationcounter[static_cast<PRDHandle_MM*>(*it)->mm()->detectorElement()->parentMuonStation()]=1;
  }
}

//____________________________________________________________________
void PRDCollHandle_MM::eraseEventDataSpecific()
{
  d->mm2stationcounter.clear();
}

//____________________________________________________________________
bool PRDCollHandle_MM::cut(PRDHandleBase*handlebase)
{
  
  PRDHandle_MM * handle = static_cast<PRDHandle_MM*>(handlebase);
  assert(handle);

  // if (d->excludeMaskedHits) {
  //   // messageVerbose(QString("Handle status = ")+handle->driftCircleStatus()+QString(", GUI: ")+common()->controller()->mdt_cutMdtDriftCircleStatus() );
  //   if(!(handle->driftCircleStatus()==common()->controller()->mdt_cutMdtDriftCircleStatus()))
  //     return false;
  // }

  // if (!d->allowedADCValues.isAllR()) {
  //   if (!d->allowedADCValues.contains(handle->ADC()))
  //     return false;
  // }

  if (d->minHitsPerStation) {
    assert(d->mm2stationcounter.find(handle->mm()->detectorElement()->parentMuonStation())!=d->mm2stationcounter.end());
    if (d->mm2stationcounter[handle->mm()->detectorElement()->parentMuonStation()]<d->minHitsPerStation)
      return false;
  }

  // messageVerbose("PRDCollHandle_MM::cut: ");
  if (d->onlyShowActive) {
    if (!common()->touchedMuonChamberHelper()->isTouchedByTrack(handle->parentMuonChamberPV()))
      return false;
  }

  return true;
}

//____________________________________________________________________
void PRDCollHandle_MM::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(mdtMinNHitsPerStationChanged(unsigned)),this,SLOT(setMinNHitsPerStation(unsigned)));
  setMinNHitsPerStation(controller->mdtMinNHitsPerStation());

  // connect(controller,SIGNAL(highLightMDTHitsByMaskChanged(bool)),this,SLOT(setHighLightByMask(bool)));
  // setHighLightByMask(controller->highLightMDTHitsByMask());

  // connect(controller,SIGNAL(highLightMDTHitsByUpperADCBoundChanged(int)),this,SLOT(setHighLightByUpperADCBound(int)));
  // setHighLightByUpperADCBound(controller->highLightMDTHitsByUpperADCBound());
  // 
  // connect(controller,SIGNAL(mdtAllowedADCValuesChanged(VP1Interval)),this,SLOT(setAllowedADCValues(VP1Interval)));
  // setAllowedADCValues(controller->mdtAllowedADCValues());

  // connect(controller,SIGNAL(mdtExcludeMaskedHitsChanged(bool)),this,SLOT(setExcludeMaskedHits(bool)));
  // setExcludeMaskedHits(controller->mdtExcludeMaskedHits());

  // connect(controller,SIGNAL(mdt_cutMdtDriftCircleStatusChanged(QString)),this,SLOT(setStatus(QString)));
  // setStatus(controller->mdt_cutMdtDriftCircleStatus());

  // connect(controller,SIGNAL(projectMDTHitsChanged(bool)),this,SLOT(setEnableProjections(bool)));
  // setEnableProjections(controller->projectMDTHits());

  connect(controller,SIGNAL(limitToActiveChambersChanged(bool)),this,SLOT(setLimitToActiveChambers(bool)));
  setLimitToActiveChambers(controller->limitToActiveChambers());
  
  connect(common()->touchedMuonChamberHelper(),SIGNAL(muonChambersTouchedByTracksChanged(void)),this,SLOT(muonChambersTouchedByTracksChanged(void)));  
}

//____________________________________________________________________
void PRDCollHandle_MM::setMinNHitsPerStation(unsigned minnhits)
{
  if (minnhits==1)
    minnhits = 0;//Since 0 and 1 gives same result, we map all 1's to 0's to avoid unnecessary cut rechecks.
  if (d->minHitsPerStation==minnhits)
    return;
  bool cut_relaxed = minnhits < d->minHitsPerStation;
  d->minHitsPerStation=minnhits;
  if (cut_relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

// //____________________________________________________________________
// void PRDCollHandle_MM::setAllowedADCValues(VP1Interval newinterval)
// {
//   if (d->allowedADCValues==newinterval)
//     return;
//   VP1Interval old(d->allowedADCValues);
//   d->allowedADCValues = newinterval;
//   if (newinterval.contains(old)) {
//     recheckCutStatusOfAllNotVisibleHandles();//cut relaxed
//   } else {
//     if (old.contains(newinterval))
//       recheckCutStatusOfAllVisibleHandles();//cut tightened
//     else
//       recheckCutStatusOfAllHandles();
//   }
// }

// //____________________________________________________________________
// void PRDCollHandle_MM::setExcludeMaskedHits(bool b)
// {
//   if (d->excludeMaskedHits==b)
//     return;
//   bool cut_relaxed = !b;
//   d->excludeMaskedHits=b;
//   if (cut_relaxed)
//     recheckCutStatusOfAllNotVisibleHandles();
//   else
//     recheckCutStatusOfAllVisibleHandles();
// }

// //____________________________________________________________________
// void PRDCollHandle_MM::setStatus(QString b)
// {
//   // messageVerbose(QString("SetStatus: ")+b);
//   if (d->status==b)
//     return;
//   d->status=b;
//   recheckCutStatusOfAllHandles();
// }

// //____________________________________________________________________
// void PRDCollHandle_MM::setHighLightByMask(bool b)
// {
//   if (m_highLightMasked==b)
//     return;
//   m_highLightMasked=b;
// 
//   //Fixme: check PRDCollHandle::hasCustomHighlighting() before proceeding to loop here?.
// 
//   //call updateMaterial on all handles which are masked.
//   std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
//   for (;it!=itE;++it) {
//     if (static_cast<PRDHandle_MM*>(*it)-> masked())
//       (*it)->updateMaterial();
//   }
// }

// //____________________________________________________________________
// void PRDCollHandle_MM::setHighLightByUpperADCBound(int bound)
// {
//   if (bound<-1)
//     bound = -1;//Since all negative gives same result, we map all those to -1 to avoid unnecessary rechecks.
//   if (m_highLightADCBelow==bound)
//     return;
// 
//   int low=std::min(m_highLightADCBelow,bound);
//   int high=std::max(m_highLightADCBelow,bound);
//   m_highLightADCBelow=bound;
// 
//   //Fixme: check PRDCollHandle::hasCustomHighlighting() before proceeding to loop here?.
// 
//   //call updateMaterial on all handles which are between the former and present values of the bound.
//   std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
//   int adc;
//   for (;it!=itE;++it) {
//     adc = static_cast<PRDHandle_MM*>(*it)->ADC();
//     if (adc>=low&&adc<=high)
//       (*it)->updateMaterial();
//   }
// }

// //____________________________________________________________________
// void PRDCollHandle_MM::setEnableProjections( bool b )
// {
//   if (d->projectionsEnabled==b)
//     return;
//   d->projectionsEnabled=b;
//   d->updateProjectionFlag();
// }
// 
// //____________________________________________________________________
// void PRDCollHandle_MM::setAppropriateProjection( int i ) {
//   if (d->appropriateprojection==i)
//     return;
//   d->appropriateprojection=i;
//   d->updateProjectionFlag();
// }

//____________________________________________________________________
void PRDCollHandle_MM::setLimitToActiveChambers(bool l)
{
  messageVerbose("PRDCollHandle_MM::setLimitToActiveChambers => "+str(l)+" current="+str(d->onlyShowActive));

  if (d->onlyShowActive==l)
    return;
  bool cut_relaxed=(d->onlyShowActive);
  d->onlyShowActive=l;
  if (cut_relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

void PRDCollHandle_MM::muonChambersTouchedByTracksChanged(void)
{
  messageVerbose("PRDCollHandle_MM::muonChambersTouchedByTracksChanged");
  recheckCutStatusOfAllHandles();
}


// //____________________________________________________________________
// void PRDCollHandle_MM::Imp::updateProjectionFlag() {
//   PROJECTION projflag(PRDCollHandle_MM::NONE);
//   if (projectionsEnabled) {
//     if (appropriateprojection==1)
//       projflag = PRDCollHandle_MM::TOTUBES;
//     else if (appropriateprojection==2)
//       projflag = PRDCollHandle_MM::TOCHAMBERS;
//   }
// 
//   if (theclass->m_projection!=projflag) {
//     theclass->m_projection = projflag;
//     theclass->common()->system()->deselectAll();
//     theclass->largeChangesBegin();
//     std::vector<PRDHandleBase*>::iterator it(theclass->getPrdHandles().begin()),itE(theclass->getPrdHandles().end());
//     for (;it!=itE;++it)
//       (*it)->update3DObjects();
//     theclass->largeChangesEnd();
//   }
// }
