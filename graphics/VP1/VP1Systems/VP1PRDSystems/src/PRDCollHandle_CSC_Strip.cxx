/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_CSC_Strip.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1PRDSystems/PRDHandle_CSC_Strip.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "VP1Utils/VP1DetInfo.h"
#include <QColor>

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_CSC_Strip::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasMuonGeometry() ? VP1SGContentsHelper(sys).getKeys<Muon::CscStripPrepDataContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_CSC_Strip::Imp {
public:
  bool attempted_detmgrload;
  //We only load these on demand!
  const MuonGM::MuonDetectorManager* muondetmgr;
  const CscIdHelper * idhelper;
};

//____________________________________________________________________
PRDCollHandle_CSC_Strip::PRDCollHandle_CSC_Strip(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::CSCstrip,cd,key), m_d(new Imp), m_project(false)
{
  m_d->muondetmgr = 0;
  m_d->idhelper = 0;
  m_d->attempted_detmgrload = false;
}

//____________________________________________________________________
PRDCollHandle_CSC_Strip::~PRDCollHandle_CSC_Strip()
{
  delete m_d;
}

//____________________________________________________________________
QColor PRDCollHandle_CSC_Strip::defaultColor() const
{
  return QColor::fromRgbF( 0.1, 0.5, 1.0);//blue

}

//____________________________________________________________________
void PRDCollHandle_CSC_Strip::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(projectCSCHitsChanged(bool)),this,SLOT(setProjectionMode(bool)));
  setProjectionMode(controller->projectCSCHits());
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_CSC_Strip::addPRD( Trk::PrepRawData * prd )
{
  assert(dynamic_cast<Muon::CscStripPrepData*>(prd));
  return new PRDHandle_CSC_Strip(this,static_cast<Muon::CscStripPrepData*>(prd));
}

//____________________________________________________________________
void PRDCollHandle_CSC_Strip::setProjectionMode( bool b )
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
