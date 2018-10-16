/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_sTGC.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1PRDSystems/PRDHandle_sTGC.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include <QColor>

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_sTGC::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasMuonGeometry() ? VP1SGContentsHelper(sys).getKeys<Muon::sTgcPrepDataContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_sTGC::Imp {
public:
  bool attempted_detmgrload;
  //We only load these on demand!
  const MuonGM::MuonDetectorManager* muondetmgr;
  const sTgcIdHelper * idhelper;
};

//____________________________________________________________________
PRDCollHandle_sTGC::PRDCollHandle_sTGC(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::sTGC,cd,key), m_d(new Imp), m_project(false)
{
  m_d->muondetmgr = 0;
  m_d->idhelper = 0;
  m_d->attempted_detmgrload = false;
}

//____________________________________________________________________
PRDCollHandle_sTGC::~PRDCollHandle_sTGC()
{
  delete m_d;
}

//____________________________________________________________________
QColor PRDCollHandle_sTGC::defaultColor() const
{
  return QColor::fromRgbF(0.42, 0.96, 0.16);//bright green
}

//____________________________________________________________________
void PRDCollHandle_sTGC::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(project_TGCHitsChanged(bool)),this,SLOT(setProjectionMode(bool)));
  setProjectionMode(controller->projectTGCHits());
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_sTGC::addPRD( const Trk::PrepRawData * prd )
{
  assert(dynamic_cast<const Muon::sTgcPrepData*>(prd));
  return new PRDHandle_sTGC(this,static_cast<const Muon::sTgcPrepData*>(prd));
}

//____________________________________________________________________
void PRDCollHandle_sTGC::setProjectionMode( bool b )
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
