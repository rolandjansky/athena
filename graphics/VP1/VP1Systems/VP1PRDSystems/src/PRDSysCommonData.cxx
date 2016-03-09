/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class PRDSysCommonData                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1PRDSystems/PRDSysCommonData.h"
#include "VP1Base/IVP13DSystem.h"

#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1PRDSystems/MuonChamberProjectionHelper.h"
#include "VP1PRDSystems/TouchedMuonChamberHelper.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "VP1PRDSystems/PRDTrackSegmentHelper.h"

#include "VP1GuideLineSystems/InDetProjHelper.h"
#include "VP1Base/VP1SoMaterialMixer.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/SoPath.h>


//____________________________________________________________________
class PRDSysCommonData::Imp {
public:
  //The following map is used to figure out which handle a clicked node corresponds to.
  //We use the SoTransform node to uniquely identify this association.
  std::map<SoNode*,PRDHandleBase*> sotransform2prdhandle;
  PRDHandleBase * pickedPathToHandle( SoPath*pickedPath );

  //The following map is used to find handles for a given prd pointer:
  std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> > prd2handles;
};


//____________________________________________________________________
PRDSysCommonData::PRDSysCommonData(IVP13DSystem *sys, PRDSystemController*controller)
  : VP1HelperClassBase(sys,"PRDSysCommonData"), d(new Imp),
    m_controller(controller)
{
  m_3dsystem = sys;
  m_nodeManager = new HitsSoNodeManager(sys);
  m_materialMixer = new VP1SoMaterialMixer;
  m_muonChamberProjectionHelper = new MuonChamberProjectionHelper(sys);
  m_indetProjHelper_Pixel = InDetProjHelper::createPixelHelper(sys);
  m_indetProjHelper_SCT = InDetProjHelper::createSCTHelper(sys);
  m_indetProjHelper_TRT = InDetProjHelper::createTRTHelper(sys);
  m_touchedMuonChamberHelper = new TouchedMuonChamberHelper;

  QObject::connect(m_touchedMuonChamberHelper,SIGNAL(touchedMuonChambersChanged(const std::set<GeoPVConstLink>&)),
		   sys,SLOT(emitTouchedMuonChambersChanged(const std::set<GeoPVConstLink>&)));//Fixme: need track sys!!

  m_trackAndSegmentHelper = new PRDTrackSegmentHelper(&(d->prd2handles),sys);
}

//____________________________________________________________________
PRDSysCommonData::~PRDSysCommonData()
{
  delete m_nodeManager;
  delete m_materialMixer;
  delete m_muonChamberProjectionHelper;
  delete m_indetProjHelper_Pixel;
  delete m_indetProjHelper_SCT;
  delete m_indetProjHelper_TRT;
  delete m_touchedMuonChamberHelper;
  delete m_trackAndSegmentHelper;
  delete d;
}

//____________________________________________________________________
void PRDSysCommonData::clearEventData()
{
  m_touchedMuonChamberHelper->eraseEventData();
  d->sotransform2prdhandle.clear();
  d->prd2handles.clear();
}


//_____________________________________________________________________________________
void PRDSysCommonData::registerTransform2Handle(SoTransform*transform,PRDHandleBase*handle)
{
  assert(d->sotransform2prdhandle.find(transform)==d->sotransform2prdhandle.end());
  d->sotransform2prdhandle[transform] = handle;
}

//_____________________________________________________________________________________
void PRDSysCommonData::registerPRD2Handle(const Trk::PrepRawData*prd,PRDHandleBase*handle)
{
  if (d->prd2handles.find(prd)==d->prd2handles.end()) {
    QList<PRDHandleBase*> l;
    l<<handle;
    d->prd2handles[prd] = l;
  } else {
    d->prd2handles[prd] << handle;
  }
}

//_____________________________________________________________________________________
PRDHandleBase * PRDSysCommonData::pickedPathToHandle( SoPath*pickedPath )
{
  while (pickedPath->getLength()>4) {//4 is a bit arbitrary...
    if (pickedPath->getNodeFromTail(0)->getTypeId()==SoSeparator::getClassTypeId()) {
      //If at least two children and the first is an SoTransform, then it might be what we are looking for.
      SoSeparator * sep = static_cast<SoSeparator*>(pickedPath->getNodeFromTail(0));
      if (sep->getNumChildren()>1&&sep->getChild(0)->getTypeId()==SoTransform::getClassTypeId()) {
	std::map<SoNode*,PRDHandleBase*>::iterator it = d->sotransform2prdhandle.find(sep->getChild(0));
	if ( it!=d->sotransform2prdhandle.end())
	  return it->second;
      }
    }
    pickedPath->pop();//FIXME:  NO NEED TO DO ANYTHING ELSE THAN FIND THE HANDLE!!
  }
  return 0;
}

