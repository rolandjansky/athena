/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_TGC.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1PRDSystems/PRDHandle_TGC.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include <QColor>

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_TGC::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasMuonGeometry() ? VP1SGContentsHelper(sys).getKeys<Muon::TgcPrepDataContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_TGC::Imp {
public:
  bool attempted_detmgrload;
  //We only load these on demand!
  const MuonGM::MuonDetectorManager* muondetmgr;
  const TgcIdHelper * idhelper;
};

//____________________________________________________________________
PRDCollHandle_TGC::PRDCollHandle_TGC(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::TGC,cd,key), m_d(new Imp), m_project(false)
{
  m_d->muondetmgr = 0;
  m_d->idhelper = 0;
  m_d->attempted_detmgrload = false;
}

//____________________________________________________________________
PRDCollHandle_TGC::~PRDCollHandle_TGC()
{
  delete m_d;
}

//____________________________________________________________________
QColor PRDCollHandle_TGC::defaultColor() const
{
  return QColor::fromRgbF(0.42, 0.96, 0.16);//bright green
}

//____________________________________________________________________
void PRDCollHandle_TGC::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(projectTGCHitsChanged(bool)),this,SLOT(setProjectionMode(bool)));
  setProjectionMode(controller->projectTGCHits());
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_TGC::addPRD( Trk::PrepRawData * prd )
{
  assert(dynamic_cast<Muon::TgcPrepData*>(prd));
  return new PRDHandle_TGC(this,static_cast<Muon::TgcPrepData*>(prd));
}

//____________________________________________________________________
void PRDCollHandle_TGC::setProjectionMode( bool b )
{
  if (m_project==b)
    return;
  m_project=b;

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it)
    (*it)->update3DObjects();
  largeChangesEnd();
  common()->system()->deselectAll();
}
