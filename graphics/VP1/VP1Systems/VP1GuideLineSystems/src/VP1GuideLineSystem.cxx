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
#include "VP1GuideLineSystems/VP1Lines.h"

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
  VP1Lines * lines;

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
		       "Thomas.Kittelmann@cern.ch"), m_d(new Imp(this))
{
}


//_____________________________________________________________________________________
VP1GuideLineSystem::~VP1GuideLineSystem()
{
  delete m_d;
}

//_____________________________________________________________________________________
QWidget * VP1GuideLineSystem::buildController()
{
  m_d->controller = new GuideSysController(this);
  return m_d->controller;
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
  m_d->floor = new VP1Floor(root,this);
  connect(m_d->controller,SIGNAL(floorExtentChanged(const double&)),m_d->floor,SLOT(setExtent(const double&)));
  m_d->floor->setExtent(m_d->controller->floorExtent());
  connect(m_d->controller,SIGNAL(floorSpacingChanged(const double&)),m_d->floor,SLOT(setSpacing(const double&)));
  m_d->floor->setSpacing(m_d->controller->floorSpacing());
  connect(m_d->controller,SIGNAL(floorHeightChanged(const double&)),m_d->floor,SLOT(setVerticalPosition(const double&)));
  m_d->floor->setVerticalPosition(m_d->controller->floorHeight());
  connect(m_d->controller,SIGNAL(floorColourAndTranspChanged( const SbColor4f&)),m_d->floor,SLOT(setColourAndTransp(const SbColor4f&)));
  m_d->floor->setColourAndTransp(m_d->controller->floorColourAndTransp());
  connect(m_d->controller,SIGNAL(showFloorChanged(bool)),m_d->floor,SLOT(setShown(bool)));
  m_d->floor->setShown(m_d->controller->showFloor());

  //CartesianGrid:
  m_d->cartesiangrid = new VP1CartesianGrid(root,this);
  connect(m_d->controller,SIGNAL(gridExtentChanged(const double&)),m_d->cartesiangrid,SLOT(setExtent(const double&)));
  m_d->cartesiangrid->setExtent(m_d->controller->gridExtent());
  connect(m_d->controller,SIGNAL(gridSpacingChanged(const double&)),m_d->cartesiangrid,SLOT(setSpacing(const double&)));
  m_d->cartesiangrid->setSpacing(m_d->controller->gridSpacing());
  connect(m_d->controller,SIGNAL(gridColourAndTranspChanged( const SbColor4f&)),m_d->cartesiangrid,SLOT(setColourAndTransp(const SbColor4f&)));
  m_d->cartesiangrid->setColourAndTransp(m_d->controller->gridColourAndTransp());
  connect(m_d->controller,SIGNAL(showCartesianGridChanged(bool)),m_d->cartesiangrid,SLOT(setShown(bool)));
  m_d->cartesiangrid->setShown(m_d->controller->showCartesianGrid());

  //CylindricalGrid:
  m_d->cylindricalgrid = new VP1CylindricalGrid(root,this);
  connect(m_d->controller,SIGNAL(gridExtentChanged(const double&)),m_d->cylindricalgrid,SLOT(setExtent(const double&)));
  m_d->cylindricalgrid->setExtent(m_d->controller->gridExtent());
  connect(m_d->controller,SIGNAL(gridSpacingChanged(const double&)),m_d->cylindricalgrid,SLOT(setSpacing(const double&)));
  m_d->cylindricalgrid->setSpacing(m_d->controller->gridSpacing());
  connect(m_d->controller,SIGNAL(gridColourAndTranspChanged( const SbColor4f&)),m_d->cylindricalgrid,SLOT(setColourAndTransp(const SbColor4f&)));
  m_d->cylindricalgrid->setColourAndTransp(m_d->controller->gridColourAndTransp());
  connect(m_d->controller,SIGNAL(showCylindricalGridChanged(bool)),m_d->cylindricalgrid,SLOT(setShown(bool)));
  m_d->cylindricalgrid->setShown(m_d->controller->showCylindricalGrid());

  //Letters:
  m_d->letters = new VP1Letters(m_d->controller->lettersMaterial(),root,this);
  connect(m_d->controller,SIGNAL(lettersZPosChanged(const double&)),m_d->letters,SLOT(setZPos(const double&)));
  m_d->letters->setZPos(m_d->controller->lettersZPos());
  connect(m_d->controller,SIGNAL(lettersVerticalPosChanged(const double&)),m_d->letters,SLOT(setVerticalPosition(const double&)));
  m_d->letters->setVerticalPosition(m_d->controller->lettersVerticalPos());
  connect(m_d->controller,SIGNAL(showLettersChanged(bool)),m_d->letters,SLOT(setShown(bool)));
  m_d->letters->setShown(m_d->controller->showLetters());

  //Coordinate axes:
  m_d->coordinateaxes = new VP1CoordinateAxes(m_d->controller->xAxisMaterial(),
					    m_d->controller->yAxisMaterial(),
					    m_d->controller->zAxisMaterial(),
					    root,this);
  connect(m_d->controller,SIGNAL(axesLengthChanged(const double&)),m_d->coordinateaxes,SLOT(setLength(const double&)));
  m_d->coordinateaxes->setLength(m_d->controller->axesLength());
  connect(m_d->controller,SIGNAL(axesPositionChanged(const SbVec3f&)),m_d->coordinateaxes,SLOT(setPosition(const SbVec3f&)));
  m_d->coordinateaxes->setPosition(m_d->controller->axesPosition());
  connect(m_d->controller,SIGNAL(relAxesThicknessChanged(const double&)),m_d->coordinateaxes,SLOT(setRelativeAxisThickness(const double&)));
  m_d->coordinateaxes->setRelativeAxisThickness(m_d->controller->relAxesThickness());
  connect(m_d->controller,SIGNAL(showAxesChanged(bool)),m_d->coordinateaxes,SLOT(setShown(bool)));
  m_d->coordinateaxes->setShown(m_d->controller->showAxes());

  //Eta cones:
  m_d->etacone1 = new VP1EtaCone(m_d->controller->etaCone1Material(),root,this);
  connect(m_d->controller,SIGNAL(etaExtentChanged(const double&)),m_d->etacone1,SLOT(setExtent(const double&)));
  m_d->etacone1->setExtent(m_d->controller->etaExtent());
  connect(m_d->controller,SIGNAL(etaConeValue1Changed(const double&)),m_d->etacone1,SLOT(setEtaValue(const double&)));
  m_d->etacone1->setEtaValue(m_d->controller->etaConeValue1());
  connect(m_d->controller,SIGNAL(showEtaCone1Changed(bool)),m_d->etacone1,SLOT(setShown(bool)));
  m_d->etacone1->setShown(m_d->controller->showEtaCone1());

  m_d->etacone2 = new VP1EtaCone(m_d->controller->etaCone2Material(),root,this);
  connect(m_d->controller,SIGNAL(etaExtentChanged(const double&)),m_d->etacone2,SLOT(setExtent(const double&)));
  m_d->etacone2->setExtent(m_d->controller->etaExtent());
  connect(m_d->controller,SIGNAL(etaConeValue2Changed(const double&)),m_d->etacone2,SLOT(setEtaValue(const double&)));
  m_d->etacone2->setEtaValue(m_d->controller->etaConeValue2());
  connect(m_d->controller,SIGNAL(showEtaCone2Changed(bool)),m_d->etacone2,SLOT(setShown(bool)));
  m_d->etacone2->setShown(m_d->controller->showEtaCone2());

  m_d->etacone3 = new VP1EtaCone(m_d->controller->etaCone3Material(),root,this);
  connect(m_d->controller,SIGNAL(etaExtentChanged(const double&)),m_d->etacone3,SLOT(setExtent(const double&)));
  m_d->etacone3->setExtent(m_d->controller->etaExtent());
  connect(m_d->controller,SIGNAL(etaConeValue3Changed(const double&)),m_d->etacone3,SLOT(setEtaValue(const double&)));
  m_d->etacone3->setEtaValue(m_d->controller->etaConeValue3());
  connect(m_d->controller,SIGNAL(showEtaCone3Changed(bool)),m_d->etacone3,SLOT(setShown(bool)));
  m_d->etacone3->setShown(m_d->controller->showEtaCone3());

	//Tracking Volumes:
	m_d->trackingVolumes = new VP1TrackingVolumes(m_d->controller->idTrkVolumesMaterial(),
                                              m_d->controller->caloTrkVolumesMaterial(),
                                              m_d->controller->msTrkVolumesMaterial(),
                                              m_d->controller->showInnerDetector(),
                                              m_d->controller->showCalorimeters(),
                                              m_d->controller->showMuonSpectrometer(),
                                              root,this);
	connect(m_d->controller,SIGNAL(showTrackingVolumesChanged(bool)),m_d->trackingVolumes,SLOT(setShown(bool)));
	connect(m_d->controller,SIGNAL(showInnerDetectorChanged(bool)),m_d->trackingVolumes,SLOT(setShownID(bool)));
	connect(m_d->controller,SIGNAL(showCalorimetersChanged(bool)),m_d->trackingVolumes,SLOT(setShownCalo(bool)));
	connect(m_d->controller,SIGNAL(showMuonSpectrometerChanged(bool)),m_d->trackingVolumes,SLOT(setShownMS(bool)));
	m_d->trackingVolumes->setShown(m_d->controller->showTrackingVolumes());	
  
  //Lines
  m_d->lines = new VP1Lines(root, this);
  connect(m_d->controller,SIGNAL(showLinesChanged(bool)),m_d->lines,SLOT(setShown(bool)));
	m_d->lines->setShown(m_d->controller->showLines());	
  connect(m_d->controller,SIGNAL(lineDirectionChanged(const SbVec3f&)),m_d->lines,SLOT(setDirection(const SbVec3f&)));
  m_d->lines->setDirection(m_d->controller->lineDirection());  
  
  SoSeparator * projsep = new SoSeparator;
  root->addChild(projsep);

  m_d->projsurfhelper_pixel = ProjectionSurfacesHelper::createPixelHelper(m_d->controller->pixelProjSurfMaterial(),projsep,this,this);
  m_d->projsurfhelper_sct = ProjectionSurfacesHelper::createSCTHelper(m_d->controller->sctProjSurfMaterial(),projsep,this,this);
  m_d->projsurfhelper_trt = ProjectionSurfacesHelper::createTRTHelper(m_d->controller->trtProjSurfMaterial(),projsep,this,this);

  connect(m_d->controller,SIGNAL(shownPixelProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags)),
	  m_d->projsurfhelper_pixel,SLOT(setSurfaces(InDetProjFlags::InDetProjPartsFlags)));
  m_d->projsurfhelper_pixel->setSurfaces(m_d->controller->shownPixelProjSurfaces());

  connect(m_d->controller,SIGNAL(shownSCTProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags)),
	  m_d->projsurfhelper_sct,SLOT(setSurfaces(InDetProjFlags::InDetProjPartsFlags)));
  m_d->projsurfhelper_sct->setSurfaces(m_d->controller->shownSCTProjSurfaces());

  connect(m_d->controller,SIGNAL(shownTRTProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags)),
	  m_d->projsurfhelper_trt,SLOT(setSurfaces(InDetProjFlags::InDetProjPartsFlags)));
  m_d->projsurfhelper_trt->setSurfaces(m_d->controller->shownTRTProjSurfaces());

  connect(m_d->controller,SIGNAL(applicablePixelProjPartsChanged(InDetProjFlags::InDetProjPartsFlags)),this,SLOT(possiblyEmit_ApplicableProjectionsChanged()));
  connect(m_d->controller,SIGNAL(applicableSCTProjPartsChanged(InDetProjFlags::InDetProjPartsFlags)),this,SLOT(possiblyEmit_ApplicableProjectionsChanged()));
  connect(m_d->controller,SIGNAL(applicableTRTProjPartsChanged(InDetProjFlags::InDetProjPartsFlags)),this,SLOT(possiblyEmit_ApplicableProjectionsChanged()));

  possiblyEmit_ApplicableProjectionsChanged();//fixme; late enough??? Problem with new delayed init: until guides are enabled, proper projections won't be known!!
  //Thus, the channel should call this slot explicitly after making all connections!!!
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::systemuncreate()
{
  delete m_d->coordinateaxes; m_d->coordinateaxes = 0;
  delete m_d->floor; m_d->floor = 0;
  delete m_d->cartesiangrid; m_d->cartesiangrid = 0;
  delete m_d->cylindricalgrid; m_d->cylindricalgrid = 0;
  delete m_d->letters; m_d->letters = 0;
  delete m_d->etacone1; m_d->etacone1 = 0;
  delete m_d->etacone2; m_d->etacone2 = 0;
  delete m_d->etacone3; m_d->etacone3 = 0;
  delete m_d->projsurfhelper_pixel; m_d->projsurfhelper_pixel = 0;
  delete m_d->projsurfhelper_sct; m_d->projsurfhelper_sct = 0;
  delete m_d->projsurfhelper_trt; m_d->projsurfhelper_trt = 0;
  delete m_d->trackingVolumes; m_d->trackingVolumes = 0;

  m_d->controller=0;
}


//_____________________________________________________________________________________
QByteArray VP1GuideLineSystem::saveState() {

  ensureBuildController();

  VP1Serialise serialise(4/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());
  serialise.save(m_d->controller->saveSettings());
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
  m_d->controller->restoreSettings(state.restoreByteArray());
  state.disableUnrestoredChecks();//We do the testing in the controller
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::setIDDetTypesUsingProjections( InDetProjFlags::DetTypeFlags f )
{
  ensureBuildController();
  if (!m_d->controller)
    return;//if receiving signals after uncreate
  messageVerbose("Signal received in setIDDetTypesUsingProjections("+str(f)+") slot (from "
		 +QString(sender()?sender()->objectName():"NULL sender")+")");
  m_d->sender2iddettypeswithprojs[sender()] = f;
  InDetProjFlags::DetTypeFlags used(InDetProjFlags::NoDet);
  std::map<QObject*,InDetProjFlags::DetTypeFlags>::iterator it, itE = m_d->sender2iddettypeswithprojs.end();
  for (it=m_d->sender2iddettypeswithprojs.begin();it!=itE;++it)
    used |= it->second;
  m_d->controller->setInDetDetTypesUsingProjections(used);
}

//_____________________________________________________________________________________
void VP1GuideLineSystem::possiblyEmit_ApplicableProjectionsChanged()
{
  ensureBuildController();
  if (!m_d->controller)
    return;//if receiving signals after uncreate
  InDetProjFlags::InDetProjPartsFlags new_pixel = m_d->controller->applicablePixelProjParts();
  InDetProjFlags::InDetProjPartsFlags new_sct = m_d->controller->applicableSCTProjParts();
  InDetProjFlags::InDetProjPartsFlags new_trt = m_d->controller->applicableTRTProjParts();
  if (m_d->lastemit_pixel==new_pixel&&m_d->lastemit_sct==new_sct&&m_d->lastemit_trt==new_trt)
    return;
  m_d->lastemit_pixel=new_pixel;
  m_d->lastemit_sct=new_sct;
  m_d->lastemit_trt=new_trt;
  if (verbose())
    messageVerbose("Emitting applicableProjectionsChanged("+str(m_d->lastemit_pixel)+", "+str(m_d->lastemit_sct)+", "+str(m_d->lastemit_trt)+")");
  emit applicableProjectionsChanged( m_d->lastemit_pixel,m_d->lastemit_sct,m_d->lastemit_trt );
}
