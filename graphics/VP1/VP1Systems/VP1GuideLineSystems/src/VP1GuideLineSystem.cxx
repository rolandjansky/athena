/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class VP1GuideLineSystem                         //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: July 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
#include "VP1GuideLineSystems/GuideSysController.h"
#include "VP1GuideLineSystems/ProjectionSurfacesHelper.h"
#include "VP1GuideLineSystems/VP1CoordinateAxes.h"
#include "VP1GuideLineSystems/VP1Floor.h"
#include "VP1GuideLineSystems/VP1Letters.h"
#include "VP1GuideLineSystems/VP1EtaCone.h"
#include "VP1GuideLineSystems/VP1CartesianGrid.h"
#include "VP1GuideLineSystems/VP1CylindricalGrid.h"
#include "VP1GuideLineSystems/VP1TrackingVolumes.h"

#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/SbVec3f.h>

#include <map>

class VP1GuideLineSystem::Imp {
public:
  Imp(VP1GuideLineSystem*tc) : theclass(tc),
			       controller(0),
			       coordinateaxes(0),
			       floor(0),
			       cartesiangrid(0),
			       cylindricalgrid(0),
			       letters(0),
			       etacone1(0),
			       etacone2(0),
			       etacone3(0),
			       trackingVolumes(0),
			       projsurfhelper_pixel(0),
			       projsurfhelper_sct(0),
			       projsurfhelper_trt(0),
			       lastemit_pixel(InDetProjFlags::NoProjections),
			       lastemit_sct(InDetProjFlags::NoProjections),
			       lastemit_trt(InDetProjFlags::NoProjections) {}
  VP1GuideLineSystem *theclass;
  GuideSysController * controller;
  VP1CoordinateAxes * coordinateaxes;
  VP1Floor * floor;
  VP1CartesianGrid * cartesiangrid;
  VP1CylindricalGrid * cylindricalgrid;
  VP1Letters * letters;
  VP1EtaCone * etacone1;
  VP1EtaCone * etacone2;
  VP1EtaCone * etacone3;
  VP1TrackingVolumes * trackingVolumes;

  ProjectionSurfacesHelper * projsurfhelper_pixel;
  ProjectionSurfacesHelper * projsurfhelper_sct;
  ProjectionSurfacesHelper * projsurfhelper_trt;
  InDetProjFlags::InDetProjPartsFlags lastemit_pixel;
  InDetProjFlags::InDetProjPartsFlags lastemit_sct;
  InDetProjFlags::InDetProjPartsFlags lastemit_trt;

  //For knowing which ID parts are used by data systems:
  std::map<QObject*,InDetProjFlags::DetTypeFlags> sender2iddettypeswithprojs;
};

//_____________________________________________________________________________________
VP1GuideLineSystem::VP1GuideLineSystem()
  : IVP13DSystemSimple("Guides",
		       "System providing visual guides",
		       "Thomas.Kittelmann@cern.ch"), d(new Imp(this))
{
}


//_____________________________________________________________________________________
VP1GuideLineSystem::~VP1GuideLineSystem()
{
  delete d;
}

//_____________________________________________________________________________________
QWidget * VP1GuideLineSystem::buildController()
{
  d->controller = new GuideSysController(this);
  return d->controller;
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::buildEventSceneGraph(StoreGateSvc*, SoSeparator *)
{
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::buildPermanentSceneGraph(StoreGateSvc* /*detstore*/, SoSeparator *root)
{
  messageVerbose("buildPermanentSceneGraph");
  //No guidelines are pickable:
  SoPickStyle *pickStyle = new SoPickStyle;
  pickStyle->style=SoPickStyle::UNPICKABLE;
  root->addChild(pickStyle);

  //High complexity val for eta cones:
  SoComplexity * complexity = new SoComplexity;
  complexity->value.setValue(2.0f);
  root->addChild(complexity);

  //Floor:
  d->floor = new VP1Floor(root,this);
  connect(d->controller,SIGNAL(floorExtentChanged(const double&)),d->floor,SLOT(setExtent(const double&)));
  d->floor->setExtent(d->controller->floorExtent());
  connect(d->controller,SIGNAL(floorSpacingChanged(const double&)),d->floor,SLOT(setSpacing(const double&)));
  d->floor->setSpacing(d->controller->floorSpacing());
  connect(d->controller,SIGNAL(floorHeightChanged(const double&)),d->floor,SLOT(setVerticalPosition(const double&)));
  d->floor->setVerticalPosition(d->controller->floorHeight());
  connect(d->controller,SIGNAL(floorColourAndTranspChanged( const SbColor4f&)),d->floor,SLOT(setColourAndTransp(const SbColor4f&)));
  d->floor->setColourAndTransp(d->controller->floorColourAndTransp());
  connect(d->controller,SIGNAL(showFloorChanged(bool)),d->floor,SLOT(setShown(bool)));
  d->floor->setShown(d->controller->showFloor());

  //CartesianGrid:
  d->cartesiangrid = new VP1CartesianGrid(root,this);
  connect(d->controller,SIGNAL(gridExtentChanged(const double&)),d->cartesiangrid,SLOT(setExtent(const double&)));
  d->cartesiangrid->setExtent(d->controller->gridExtent());
  connect(d->controller,SIGNAL(gridSpacingChanged(const double&)),d->cartesiangrid,SLOT(setSpacing(const double&)));
  d->cartesiangrid->setSpacing(d->controller->gridSpacing());
  connect(d->controller,SIGNAL(gridColourAndTranspChanged( const SbColor4f&)),d->cartesiangrid,SLOT(setColourAndTransp(const SbColor4f&)));
  d->cartesiangrid->setColourAndTransp(d->controller->gridColourAndTransp());
  connect(d->controller,SIGNAL(showCartesianGridChanged(bool)),d->cartesiangrid,SLOT(setShown(bool)));
  d->cartesiangrid->setShown(d->controller->showCartesianGrid());

  //CylindricalGrid:
  d->cylindricalgrid = new VP1CylindricalGrid(root,this);
  connect(d->controller,SIGNAL(gridExtentChanged(const double&)),d->cylindricalgrid,SLOT(setExtent(const double&)));
  d->cylindricalgrid->setExtent(d->controller->gridExtent());
  connect(d->controller,SIGNAL(gridSpacingChanged(const double&)),d->cylindricalgrid,SLOT(setSpacing(const double&)));
  d->cylindricalgrid->setSpacing(d->controller->gridSpacing());
  connect(d->controller,SIGNAL(gridColourAndTranspChanged( const SbColor4f&)),d->cylindricalgrid,SLOT(setColourAndTransp(const SbColor4f&)));
  d->cylindricalgrid->setColourAndTransp(d->controller->gridColourAndTransp());
  connect(d->controller,SIGNAL(showCylindricalGridChanged(bool)),d->cylindricalgrid,SLOT(setShown(bool)));
  d->cylindricalgrid->setShown(d->controller->showCylindricalGrid());

  //Letters:
  d->letters = new VP1Letters(d->controller->lettersMaterial(),root,this);
  connect(d->controller,SIGNAL(lettersZPosChanged(const double&)),d->letters,SLOT(setZPos(const double&)));
  d->letters->setZPos(d->controller->lettersZPos());
  connect(d->controller,SIGNAL(lettersVerticalPosChanged(const double&)),d->letters,SLOT(setVerticalPosition(const double&)));
  d->letters->setVerticalPosition(d->controller->lettersVerticalPos());
  connect(d->controller,SIGNAL(showLettersChanged(bool)),d->letters,SLOT(setShown(bool)));
  d->letters->setShown(d->controller->showLetters());

  //Coordinate axes:
  d->coordinateaxes = new VP1CoordinateAxes(d->controller->xAxisMaterial(),
					    d->controller->yAxisMaterial(),
					    d->controller->zAxisMaterial(),
					    root,this);
  connect(d->controller,SIGNAL(axesLengthChanged(const double&)),d->coordinateaxes,SLOT(setLength(const double&)));
  d->coordinateaxes->setLength(d->controller->axesLength());
  connect(d->controller,SIGNAL(axesPositionChanged(const SbVec3f&)),d->coordinateaxes,SLOT(setPosition(const SbVec3f&)));
  d->coordinateaxes->setPosition(d->controller->axesPosition());
  connect(d->controller,SIGNAL(relAxesThicknessChanged(const double&)),d->coordinateaxes,SLOT(setRelativeAxisThickness(const double&)));
  d->coordinateaxes->setRelativeAxisThickness(d->controller->relAxesThickness());
  connect(d->controller,SIGNAL(showAxesChanged(bool)),d->coordinateaxes,SLOT(setShown(bool)));
  d->coordinateaxes->setShown(d->controller->showAxes());

  //Eta cones:
  d->etacone1 = new VP1EtaCone(d->controller->etaCone1Material(),root,this);
  connect(d->controller,SIGNAL(etaExtentChanged(const double&)),d->etacone1,SLOT(setExtent(const double&)));
  d->etacone1->setExtent(d->controller->etaExtent());
  connect(d->controller,SIGNAL(etaConeValue1Changed(const double&)),d->etacone1,SLOT(setEtaValue(const double&)));
  d->etacone1->setEtaValue(d->controller->etaConeValue1());
  connect(d->controller,SIGNAL(showEtaCone1Changed(bool)),d->etacone1,SLOT(setShown(bool)));
  d->etacone1->setShown(d->controller->showEtaCone1());

  d->etacone2 = new VP1EtaCone(d->controller->etaCone2Material(),root,this);
  connect(d->controller,SIGNAL(etaExtentChanged(const double&)),d->etacone2,SLOT(setExtent(const double&)));
  d->etacone2->setExtent(d->controller->etaExtent());
  connect(d->controller,SIGNAL(etaConeValue2Changed(const double&)),d->etacone2,SLOT(setEtaValue(const double&)));
  d->etacone2->setEtaValue(d->controller->etaConeValue2());
  connect(d->controller,SIGNAL(showEtaCone2Changed(bool)),d->etacone2,SLOT(setShown(bool)));
  d->etacone2->setShown(d->controller->showEtaCone2());

  d->etacone3 = new VP1EtaCone(d->controller->etaCone3Material(),root,this);
  connect(d->controller,SIGNAL(etaExtentChanged(const double&)),d->etacone3,SLOT(setExtent(const double&)));
  d->etacone3->setExtent(d->controller->etaExtent());
  connect(d->controller,SIGNAL(etaConeValue3Changed(const double&)),d->etacone3,SLOT(setEtaValue(const double&)));
  d->etacone3->setEtaValue(d->controller->etaConeValue3());
  connect(d->controller,SIGNAL(showEtaCone3Changed(bool)),d->etacone3,SLOT(setShown(bool)));
  d->etacone3->setShown(d->controller->showEtaCone3());

	//Tracking Volumes:
	d->trackingVolumes = new VP1TrackingVolumes(d->controller->idTrkVolumesMaterial(),
                                              d->controller->caloTrkVolumesMaterial(),
                                              d->controller->msTrkVolumesMaterial(),
                                              d->controller->showInnerDetector(),
                                              d->controller->showCalorimeters(),
                                              d->controller->showMuonSpectrometer(),
                                              root,this);
	connect(d->controller,SIGNAL(showTrackingVolumesChanged(bool)),d->trackingVolumes,SLOT(setShown(bool)));
	connect(d->controller,SIGNAL(showInnerDetectorChanged(bool)),d->trackingVolumes,SLOT(setShownID(bool)));
	connect(d->controller,SIGNAL(showCalorimetersChanged(bool)),d->trackingVolumes,SLOT(setShownCalo(bool)));
	connect(d->controller,SIGNAL(showMuonSpectrometerChanged(bool)),d->trackingVolumes,SLOT(setShownMS(bool)));
	d->trackingVolumes->setShown(d->controller->showTrackingVolumes());	
	
  SoSeparator * projsep = new SoSeparator;
  root->addChild(projsep);

  d->projsurfhelper_pixel = ProjectionSurfacesHelper::createPixelHelper(d->controller->pixelProjSurfMaterial(),projsep,this,this);
  d->projsurfhelper_sct = ProjectionSurfacesHelper::createSCTHelper(d->controller->sctProjSurfMaterial(),projsep,this,this);
  d->projsurfhelper_trt = ProjectionSurfacesHelper::createTRTHelper(d->controller->trtProjSurfMaterial(),projsep,this,this);

  connect(d->controller,SIGNAL(shownPixelProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags)),
	  d->projsurfhelper_pixel,SLOT(setSurfaces(InDetProjFlags::InDetProjPartsFlags)));
  d->projsurfhelper_pixel->setSurfaces(d->controller->shownPixelProjSurfaces());

  connect(d->controller,SIGNAL(shownSCTProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags)),
	  d->projsurfhelper_sct,SLOT(setSurfaces(InDetProjFlags::InDetProjPartsFlags)));
  d->projsurfhelper_sct->setSurfaces(d->controller->shownSCTProjSurfaces());

  connect(d->controller,SIGNAL(shownTRTProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags)),
	  d->projsurfhelper_trt,SLOT(setSurfaces(InDetProjFlags::InDetProjPartsFlags)));
  d->projsurfhelper_trt->setSurfaces(d->controller->shownTRTProjSurfaces());

  connect(d->controller,SIGNAL(applicablePixelProjPartsChanged(InDetProjFlags::InDetProjPartsFlags)),this,SLOT(possiblyEmit_ApplicableProjectionsChanged()));
  connect(d->controller,SIGNAL(applicableSCTProjPartsChanged(InDetProjFlags::InDetProjPartsFlags)),this,SLOT(possiblyEmit_ApplicableProjectionsChanged()));
  connect(d->controller,SIGNAL(applicableTRTProjPartsChanged(InDetProjFlags::InDetProjPartsFlags)),this,SLOT(possiblyEmit_ApplicableProjectionsChanged()));

  possiblyEmit_ApplicableProjectionsChanged();//fixme; late enough??? Problem with new delayed init: until guides are enabled, proper projections won't be known!!
  //Thus, the channel should call this slot explicitly after making all connections!!!
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::systemuncreate()
{
  delete d->coordinateaxes; d->coordinateaxes = 0;
  delete d->floor; d->floor = 0;
  delete d->cartesiangrid; d->cartesiangrid = 0;
  delete d->cylindricalgrid; d->cylindricalgrid = 0;
  delete d->letters; d->letters = 0;
  delete d->etacone1; d->etacone1 = 0;
  delete d->etacone2; d->etacone2 = 0;
  delete d->etacone3; d->etacone3 = 0;
  delete d->projsurfhelper_pixel; d->projsurfhelper_pixel = 0;
  delete d->projsurfhelper_sct; d->projsurfhelper_sct = 0;
  delete d->projsurfhelper_trt; d->projsurfhelper_trt = 0;
  delete d->trackingVolumes; d->trackingVolumes = 0;

  d->controller=0;
}


//_____________________________________________________________________________________
QByteArray VP1GuideLineSystem::saveState() {

  ensureBuildController();

  VP1Serialise serialise(4/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());
  serialise.save(d->controller->saveSettings());
  serialise.disableUnsavedChecks();//We do the testing in the controller
  return serialise.result();
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::restoreFromState(QByteArray ba) {

  VP1Deserialise state(ba,this);
  if (state.version()<0||state.version()>4) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    state.disableUnrestoredChecks();//We do the testing in the controller
    return;
  }
  if (state.version()>=0&&state.version()<=3) {
    message("Warning: State data in .vp1 file is in obsolete format - ignoring!");
    state.disableUnrestoredChecks();//We do the testing in the controller
    return;
  }
  ensureBuildController();

  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());
  d->controller->restoreSettings(state.restoreByteArray());
  state.disableUnrestoredChecks();//We do the testing in the controller
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::setIDDetTypesUsingProjections( InDetProjFlags::DetTypeFlags f )
{
  ensureBuildController();
  if (!d->controller)
    return;//if receiving signals after uncreate
  messageVerbose("Signal received in setIDDetTypesUsingProjections("+str(f)+") slot (from "
		 +QString(sender()?sender()->objectName():"NULL sender")+")");
  d->sender2iddettypeswithprojs[sender()] = f;
  InDetProjFlags::DetTypeFlags used(InDetProjFlags::NoDet);
  std::map<QObject*,InDetProjFlags::DetTypeFlags>::iterator it, itE = d->sender2iddettypeswithprojs.end();
  for (it=d->sender2iddettypeswithprojs.begin();it!=itE;++it)
    used |= it->second;
  d->controller->setInDetDetTypesUsingProjections(used);
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::possiblyEmit_ApplicableProjectionsChanged()
{
  ensureBuildController();
  if (!d->controller)
    return;//if receiving signals after uncreate
  InDetProjFlags::InDetProjPartsFlags new_pixel = d->controller->applicablePixelProjParts();
  InDetProjFlags::InDetProjPartsFlags new_sct = d->controller->applicableSCTProjParts();
  InDetProjFlags::InDetProjPartsFlags new_trt = d->controller->applicableTRTProjParts();
  if (d->lastemit_pixel==new_pixel&&d->lastemit_sct==new_sct&&d->lastemit_trt==new_trt)
    return;
  d->lastemit_pixel=new_pixel;
  d->lastemit_sct=new_sct;
  d->lastemit_trt=new_trt;
  if (verbose())
    messageVerbose("Emitting applicableProjectionsChanged("+str(d->lastemit_pixel)+", "+str(d->lastemit_sct)+", "+str(d->lastemit_trt)+")");
  emit applicableProjectionsChanged( d->lastemit_pixel,d->lastemit_sct,d->lastemit_trt );
}
