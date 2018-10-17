/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1TrackingVolumes                 //
//                                                            //
//  Author: Edward.Moyse@cern.ch  //
//  Initial version: March 2009                              //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/VP1TrackingVolumes.h"
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <iostream>
//____________________________________________________________________
class VP1TrackingVolumes::Imp {
public:
	Imp(VP1TrackingVolumes * tc,
		SoMaterial * materialID,
		SoMaterial * materialCalo,
		SoMaterial * materialMS,
		SoSeparator * attachsep,
    bool showID, bool showCalo, bool showMS);  
	VP1TrackingVolumes * theclass;
	SoSeparator * attachSep;
	SoMaterial * materialID;
	SoMaterial * materialCalo;
	SoMaterial * materialMS;

  bool shown;
  bool shownID;
  bool shownCalo;
  bool shownMS;

  SoSeparator * sep;
  SoSeparator * sepID;
  SoSeparator * sepCalo;
  SoSeparator * sepMS;
  SoCylinder* innerDetector;
  SoCylinder* calos;
  SoCylinder* muonSpectrometer;

  void updateFields();
  void ensureInit3DObjects();
};

//____________________________________________________________________
VP1TrackingVolumes::VP1TrackingVolumes(		SoMaterial * materialID,SoMaterial * materialCalo,SoMaterial * materialMS,
             bool showID, bool showCalo, bool showMS,
				     SoSeparator * attachsep,
				     IVP1System * sys,QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1TrackingVolumes"), m_d(new Imp(this,materialID, materialCalo, materialMS, attachsep, showID, showCalo, showMS))
{
}

//____________________________________________________________________
VP1TrackingVolumes::~VP1TrackingVolumes()
{

  setShown(false);
  if (m_d->sepID)
    m_d->sepID->unref();
  if (m_d->sepCalo)
    m_d->sepCalo->unref();
  if (m_d->sepMS)
    m_d->sepMS->unref();

  if (m_d->sep)
    m_d->sep->unref();
  std::cout<<"m_d->attachSep"<<std::endl;

  m_d->attachSep->unref();
  delete m_d;
}

//____________________________________________________________________
VP1TrackingVolumes::Imp::Imp(VP1TrackingVolumes *tc, SoMaterial * materialID,SoMaterial * materialCalo,SoMaterial * materialMS, SoSeparator * as, bool showID, bool showCalo, bool showMS)
  : theclass(tc), attachSep(as), 
  materialID(materialID), materialCalo(materialCalo), materialMS(materialMS),  
  shown(false), shownID(showID), shownCalo(showCalo), shownMS(showMS),
  sep(0),sepID(0),sepCalo(0),sepMS(0),innerDetector(0),calos(0),muonSpectrometer(0)
{
  attachSep->ref();
}

//____________________________________________________________________
void VP1TrackingVolumes::Imp::ensureInit3DObjects()
{
  if (sep && sepID && sepCalo && sepMS)
    return;
  theclass->messageVerbose("Building 3D objects");
  
  sep = new SoSeparator; 
  sep->ref();
  SoRotationXYZ * rotz = new SoRotationXYZ;
  rotz->axis.setValue(SoRotationXYZ::X);
  rotz->angle.setValue(0.5*M_PI);
  sep->addChild(rotz);
  
  sepID = new SoSeparator; 
  sepID->ref();
  sepID->addChild(materialID);
  innerDetector = new SoCylinder; 
  innerDetector->radius.setValue(1100.0);
  innerDetector->height.setValue( 6400.0 );
  sepID->addChild(innerDetector);

  sepCalo = new SoSeparator; 
  sepCalo->ref();
  sepCalo->addChild(materialCalo);  
  calos = new SoCylinder; 
  calos->radius.setValue(4250.0);
  calos->height.setValue( 6779.0*2.0 );
  sepCalo->addChild(calos);

  sepMS = new SoSeparator; 
  sepMS->ref();
  sepMS->addChild(materialMS);  
  muonSpectrometer = new SoCylinder; 
  muonSpectrometer->radius.setValue(15000.0);
  muonSpectrometer->height.setValue( 21000.0*2.0 );
  sepMS->addChild(muonSpectrometer);


//  innerDetector->parts.setValue(SoCylinder::SIDES|SoCylinder::BOTTOM);
}

//____________________________________________________________________
void VP1TrackingVolumes::Imp::updateFields()
{  
  ensureInit3DObjects();
  theclass->messageVerbose("Updating fields");

  const bool save = sep->enableNotify(false);

  if (shownID ){
    if (sep->findChild(sepID)<0)
      sep->addChild(sepID);
  } else {
      sep->removeChild(sepID);
  }
  
  if (shownCalo ){
    if (sep->findChild(sepCalo)<0)
      sep->addChild(sepCalo);
  } else {
    sep->removeChild(sepCalo);
  }
  
  if (shownMS ){
    if (sep->findChild(sepMS)<0)
      sep->addChild(sepMS);
  } else {
    sep->removeChild(sepMS);
  }

  if (save) {
    sep->enableNotify(true);
    sep->touch();
  }
}

//____________________________________________________________________
void VP1TrackingVolumes::setShown(bool b)
{
  messageVerbose("Signal received: setShown("+str(b)+")");
  if (m_d->shown==b)
    return;
  m_d->shown=b;
  if (m_d->shown) {
    m_d->updateFields();
    if (m_d->attachSep->findChild(m_d->sep)<0)
      m_d->attachSep->addChild(m_d->sep);
  } else {
    m_d->attachSep->removeChild(m_d->sep);
  }
}

//____________________________________________________________________
void VP1TrackingVolumes::setShownID(bool b)
{
  messageVerbose("Signal received: setShownID("+str(b)+")");
  if (m_d->shownID==b)
    return;
  m_d->shownID=b;
  if (m_d->shown)
    m_d->updateFields();
}

//____________________________________________________________________
void VP1TrackingVolumes::setShownCalo(bool b)
{
  messageVerbose("Signal received: setShownCalo("+str(b)+")");
  if (m_d->shownCalo==b)
    return;
  m_d->shownCalo=b;
  if (m_d->shown)
    m_d->updateFields();
}

//____________________________________________________________________
void VP1TrackingVolumes::setShownMS(bool b)
{
  messageVerbose("Signal received: setShownMS("+str(b)+")");
  if (m_d->shownMS==b)
    return;
  m_d->shownMS=b;
  if (m_d->shown)
    m_d->updateFields();
}
