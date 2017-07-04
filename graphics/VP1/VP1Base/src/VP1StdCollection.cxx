/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1StdCollection                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1StdCollection.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1QtInventorUtils.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>

#include <QCheckBox>

#include <iostream>
//____________________________________________________________________
class VP1StdCollection::Imp {
public:
  Imp() : theswitch(0),
	  collsep(0),
	  checkBox(0),
	  material(0),
	  largechangescount_sep(0),
	  largechangescount_switch(0),
	  matButton(0),
	  visible(false),
	  loaded(false),
	  problemsloading(false),
	  textProvided(false),
	  id(-1),
	  idProvided(false),
	  lastMatTransparency(0),
	  lastMatBrightness(0) {}
  SoSwitch*  theswitch;
  SoSeparator*  collsep;
  QCheckBox * checkBox;
  SoMaterial *  material;
  int largechangescount_sep;
  int largechangescount_switch;
  VP1MaterialButtonBase* matButton;
  bool visible;
  bool loaded;
  bool problemsloading;
  bool textProvided;
  qint32 id;
  bool idProvided;
  QString text;
  QList<QWidget*> extraWidgets;
  double lastMatTransparency;
  double lastMatBrightness;
};

//____________________________________________________________________
VP1StdCollection::VP1StdCollection(IVP1System* sys,const QString& helperClassName)
  : VP1Collection(sys,helperClassName), d(new Imp)
{
}

//____________________________________________________________________
void VP1StdCollection::init(VP1MaterialButtonBase* button)
{
  //The object names should not contain all sorts of funky chars (mat button style sheets wont work for instance):
  QString safetext(text());
  safetext.replace(' ','_');
  safetext.replace('[','_');
  safetext.replace(']','_');
  safetext.replace('/','_');
  safetext.replace('.','_');
  safetext.replace(',','_');
  safetext.replace('<','_');
  safetext.replace('>','_');
  safetext.replace('&','_');

  d->theswitch = new SoSwitch;
  d->theswitch->ref();
  d->theswitch->setName(("StdCollSwitch"+safetext).toStdString().c_str());

  // get a default material for the collection
  d->material = new SoMaterial;
  d->material->setName(("StdCollMat"+safetext).toStdString().c_str());
  d->material->ref();
  assignDefaultMaterial(d->material);

  // the material button hosting the default material
  button==0?d->matButton = new VP1MaterialButton:d->matButton=button; // Use button if something has been passed in...
  d->matButton->setObjectName("matButtonColl_"+safetext);
  d->matButton->setToolTip(matButtonToolTip());
  d->matButton->setMaterial(d->material);
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(possibleChangeMatTranspOrBrightness()));

  // the collection checkbox
  d->checkBox = new QCheckBox();
  d->checkBox->setText(d->checkBox->fontMetrics().elidedText(text(), Qt::ElideRight, 140 ));
  d->checkBox->setObjectName("checkBoxColl_"+safetext);
  d->checkBox->setToolTip(checkBoxToolTip());
  connect(d->checkBox,SIGNAL(toggled(bool)),this,SLOT(setVisible(bool)));

  d->collsep = new SoSeparator;
  d->collsep->ref();
  d->collsep->setName(("StdCollSep"+safetext).toStdString().c_str());

  possibleChangeMatTranspOrBrightness();
}

//____________________________________________________________________
VP1StdCollection::~VP1StdCollection()
{
  delete d->checkBox;
  delete d->matButton;
  foreach (QWidget*w,d->extraWidgets)
    delete w;
  d->material->unref();
  d->collsep->unref();
  d->theswitch->unref();
  delete d;
}

//____________________________________________________________________
QString VP1StdCollection::text() const
{
  if (!d->textProvided) {
    d->textProvided = true;
    d->text = provideText();
  }
  return d->text;
}

//____________________________________________________________________
bool VP1StdCollection::visible() const
{
  return d->visible;
}

//____________________________________________________________________
bool VP1StdCollection::isLoaded() const
{
  return d->loaded;
}

//____________________________________________________________________
bool VP1StdCollection::problemsLoading() const
{
  return d->problemsloading;
}

//____________________________________________________________________
void VP1StdCollection::setVisible(bool b)
{
  if (d->visible==b||problemsLoading())
    return;
  d->visible=b;
  messageVerbose("Visible state ("+text()+") => "+str(d->visible));

  //Possibly load:
  if (d->visible&&!d->loaded) {
    d->loaded = true;
    d->theswitch->whichChild = SO_SWITCH_NONE;
    bool ok = load();
    if (!ok) {
      d->problemsloading = true;
      d->visible = false;
      d->checkBox->blockSignals(true);
      d->checkBox->setChecked(false);
      d->checkBox->setToolTip("Problems encountered during attempt to load this collection");
      foreach (QWidget*w,widgetsForGuiRow())
	w->setEnabled(false);
      message("Problems loading "+text());
      return;
    }
    d->theswitch->addChild(d->material);
    d->theswitch->addChild(d->collsep);
  }

  Q_ASSERT(!problemsLoading());

  //Update checkbox and switch:
  if (d->checkBox->isChecked()!=d->visible) {
    bool save = d->checkBox->blockSignals(true);
    d->checkBox->setChecked(d->visible);
    if (!save)
      d->checkBox->blockSignals(false);
  }
  if ((d->theswitch->whichChild.getValue()==SO_SWITCH_ALL) != d->visible)
    d->theswitch->whichChild = ( d->visible ? SO_SWITCH_ALL : SO_SWITCH_NONE );

  visibilityChanged(d->visible);
}


//____________________________________________________________________
SoSwitch * VP1StdCollection::collSwitch() const
{
  if (!d->theswitch)
    message("ERROR: collSwitch() called before init()");
  return d->theswitch;
}

//____________________________________________________________________
SoSeparator * VP1StdCollection::collSep() const
{
  if (!d->collsep)
    message("ERROR: collSep() called before init()");
  return d->collsep;
}

//____________________________________________________________________
SoMaterial * VP1StdCollection::material() const
{
  if (!d->material)
    message("ERROR: material() called before init()");
  return d->material;
}

//____________________________________________________________________
void VP1StdCollection::largeChangesBegin()
{
  if (!d->collsep)
    message("ERROR: largeChangesBegin() called before init()");
  d->collsep->enableNotify(false);
  ++(d->largechangescount_sep);
  d->theswitch->enableNotify(false);
  ++(d->largechangescount_switch);
}

//____________________________________________________________________
void VP1StdCollection::largeChangesEnd()
{
  if (!d->collsep)
    message("ERROR: largeChangesEnd() called before init()");
  if (d->largechangescount_sep>0) {
    if (--(d->largechangescount_sep)==0) {
      d->collsep->enableNotify(true);
      d->collsep->touch();
    }
  }
  if (d->largechangescount_switch>0) {
    if (--(d->largechangescount_switch)==0) {
      d->theswitch->enableNotify(true);
      d->theswitch->touch();
    }
  }
}

//____________________________________________________________________
QList<QWidget*> VP1StdCollection::provideWidgetsForGuiRow() const
{
  d->extraWidgets = provideExtraWidgetsForGuiRow();

  QList<QWidget*> l;
  l << d->checkBox;
  l << d->matButton;
  l << d->extraWidgets;
  return l;
}

//____________________________________________________________________
QByteArray VP1StdCollection::persistifiableState() const
{
	messageDebug("VP1StdCollection::persistifiableState()");

  if (!d->material) {
    message("ERROR: persistifiableState() called before init()");
    return QByteArray();
  }
  VP1Serialise serialise(1/*version*/);
  serialise.disableUnsavedChecks();
  serialise.save(d->visible);
  Q_ASSERT(d->material&&"Did you forget to call init() on this VP1StdCollection?");
  serialise.save(d->material);
  serialise.save(extraWidgetsState());//version 1+
  return serialise.result();
}

//____________________________________________________________________
void VP1StdCollection::setState(const QByteArray&state)
{
  if (!d->material) {
    message("ERROR: setState(..) called before init()");
    return;
  }
  VP1Deserialise des(state);
  des.disableUnrestoredChecks();
  if (des.version()!=0&&des.version()!=1) {
    messageDebug("Warning: Ignoring state with wrong version");
    return;
  }
  bool vis = des.restoreBool();

  QByteArray matState = des.restoreByteArray();
  QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();

  VP1QtInventorUtils::deserialiseSoMaterial(matState,d->material);
  d->matButton->copyValuesFromMaterial(d->material);
  // d->material->restoreFromState(matState);
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);
}

//____________________________________________________________________
qint32 VP1StdCollection::collTypeID() const
{
  if (!d->idProvided) {
    d->idProvided = true;
    d->id = provideCollTypeID();
  }
  return d->id;
}

//____________________________________________________________________
QByteArray VP1StdCollection::providePersistifiableID() const
{
  VP1Serialise serialise(-10/*version*/);//we use negative versions here (0 and -10 already used),
                                         //to allow derived classes to use positive numbers.
  serialise.disableUnsavedChecks();
  serialise.save(collTypeID());
  serialise.save(text());
  return serialise.result();
}

//____________________________________________________________________
double VP1StdCollection::collMaterialTransparency() const
{
  return d->lastMatTransparency;
}

//____________________________________________________________________
double VP1StdCollection::collMaterialBrightness() const
{
  return d->lastMatBrightness;
}

//____________________________________________________________________
void VP1StdCollection::possibleChangeMatTranspOrBrightness()
{
  double t = d->matButton->lastAppliedTransparency();
  double b = d->matButton->lastAppliedBrightness();
  if ( d->lastMatTransparency == t && d->lastMatBrightness == b )
    return;
  d->lastMatTransparency = t;
  d->lastMatBrightness = b;
  collMaterialTransparencyAndBrightnessChanged();
}
