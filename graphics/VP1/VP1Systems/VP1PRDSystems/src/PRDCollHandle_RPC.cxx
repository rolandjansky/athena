/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_RPC.h"
#include "VP1Base/IVP13DSystem.h"//fixme: add deselectAll to common()...
#include "VP1PRDSystems/PRDHandle_RPC.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include <QColor>

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_RPC::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasMuonGeometry() ? VP1SGContentsHelper(sys).getKeys<Muon::RpcPrepDataContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_RPC::Imp {
public:
  bool attempted_detmgrload;
  //We only load these on demand!
  const MuonGM::MuonDetectorManager* muondetmgr;
  const RpcIdHelper * idhelper;
};

//____________________________________________________________________
PRDCollHandle_RPC::PRDCollHandle_RPC(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::RPC,cd,key), m_d(new Imp), m_project(false)
{
  m_d->muondetmgr = 0;
  m_d->idhelper = 0;
  m_d->attempted_detmgrload = false;
}

//____________________________________________________________________
PRDCollHandle_RPC::~PRDCollHandle_RPC()
{
  delete m_d;
}

//____________________________________________________________________
QColor PRDCollHandle_RPC::defaultColor() const
{
  return QColor::fromRgbF( 0.75, 0.39, 1.0);//light purple
}

//____________________________________________________________________
void PRDCollHandle_RPC::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(projectRPCHitsChanged(bool)),this,SLOT(setProjectionMode(bool)));
  setProjectionMode(controller->projectRPCHits());
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_RPC::addPRD( Trk::PrepRawData * prd )
{
  assert(dynamic_cast<Muon::RpcPrepData*>(prd));
  return new PRDHandle_RPC(this,static_cast<Muon::RpcPrepData*>(prd));
}

//____________________________________________________________________
void PRDCollHandle_RPC::setProjectionMode( bool b )
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
