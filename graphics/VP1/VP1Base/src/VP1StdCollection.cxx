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
  : VP1Collection(sys,helperClassName), m_d(new Imp)
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

  m_d->theswitch = new SoSwitch;
  m_d->theswitch->ref();
  m_d->theswitch->setName(("StdCollSwitch"+safetext).toStdString().c_str());

  // get a default material for the collection
  m_d->material = new SoMaterial;
  m_d->material->setName(("StdCollMat"+safetext).toStdString().c_str());
  m_d->material->ref();
  assignDefaultMaterial(m_d->material);

  // the material button hosting the default material
  button==0?m_d->matButton = new VP1MaterialButton:m_d->matButton=button; // Use button if something has been passed in...
  m_d->matButton->setObjectName("matButtonColl_"+safetext);
  m_d->matButton->setToolTip(matButtonToolTip());
  m_d->matButton->setMaterial(m_d->material);
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(possibleChangeMatTranspOrBrightness()));

  // the collection checkbox
  m_d->checkBox = new QCheckBox();
  m_d->checkBox->setText(m_d->checkBox->fontMetrics().elidedText(text(), Qt::ElideRight, 140 ));
  m_d->checkBox->setObjectName("checkBoxColl_"+safetext);
  m_d->checkBox->setToolTip(checkBoxToolTip());
  connect(m_d->checkBox,SIGNAL(toggled(bool)),this,SLOT(setVisible(bool)));

  m_d->collsep = new SoSeparator;
  m_d->collsep->ref();
  m_d->collsep->setName(("StdCollSep"+safetext).toStdString().c_str());

  possibleChangeMatTranspOrBrightness();
}

//____________________________________________________________________
VP1StdCollection::~VP1StdCollection()
{
  delete m_d->checkBox;
  delete m_d->matButton;
  foreach (QWidget*w,m_d->extraWidgets)
    delete w;
  m_d->material->unref();
  m_d->collsep->unref();
  m_d->theswitch->unref();
  delete m_d;
}

//____________________________________________________________________
QString VP1StdCollection::text() const
{
  if (!m_d->textProvided) {
    m_d->textProvided = true;
    m_d->text = provideText();
  }
  return m_d->text;
}

//____________________________________________________________________
bool VP1StdCollection::visible() const
{
  return m_d->visible;
}

//____________________________________________________________________
bool VP1StdCollection::isLoaded() const
{
  return m_d->loaded;
}

//____________________________________________________________________
bool VP1StdCollection::problemsLoading() const
{
  return m_d->problemsloading;
}

//____________________________________________________________________
void VP1StdCollection::setVisible(bool b)
{
  if (m_d->visible==b||problemsLoading())
    return;
  m_d->visible=b;
  messageVerbose("Visible state ("+text()+") => "+str(m_d->visible));

  //Possibly load:
  if (m_d->visible&&!m_d->loaded) {
    m_d->loaded = true;
    m_d->theswitch->whichChild = SO_SWITCH_NONE;
    bool ok = load();
    if (!ok) {
      m_d->problemsloading = true;
      m_d->visible = false;
      m_d->checkBox->blockSignals(true);
      m_d->checkBox->setChecked(false);
      m_d->checkBox->setToolTip("Problems encountered during attempt to load this collection");
      foreach (QWidget*w,widgetsForGuiRow())
	w->setEnabled(false);
      message("Problems loading "+text());
      return;
    }
    m_d->theswitch->addChild(m_d->material);
    m_d->theswitch->addChild(m_d->collsep);
  }

  Q_ASSERT(!problemsLoading());

  //Update checkbox and switch:
  if (m_d->checkBox->isChecked()!=m_d->visible) {
    bool save = m_d->checkBox->blockSignals(true);
    m_d->checkBox->setChecked(m_d->visible);
    if (!save)
      m_d->checkBox->blockSignals(false);
  }
  if ((m_d->theswitch->whichChild.getValue()==SO_SWITCH_ALL) != m_d->visible)
    m_d->theswitch->whichChild = ( m_d->visible ? SO_SWITCH_ALL : SO_SWITCH_NONE );

  visibilityChanged(m_d->visible);
}


//____________________________________________________________________
SoSwitch * VP1StdCollection::collSwitch() const
{
  if (!m_d->theswitch)
    message("ERROR: collSwitch() called before init()");
  return m_d->theswitch;
}

//____________________________________________________________________
SoSeparator * VP1StdCollection::collSep() const
{
  if (!m_d->collsep)
    message("ERROR: collSep() called before init()");
  return m_d->collsep;
}

//____________________________________________________________________
SoMaterial * VP1StdCollection::material() const
{
  if (!m_d->material)
    message("ERROR: material() called before init()");
  return m_d->material;
}

//____________________________________________________________________
void VP1StdCollection::largeChangesBegin()
{
  if (!m_d->collsep)
    message("ERROR: largeChangesBegin() called before init()");
  m_d->collsep->enableNotify(false);
  ++(m_d->largechangescount_sep);
  m_d->theswitch->enableNotify(false);
  ++(m_d->largechangescount_switch);
}

//____________________________________________________________________
void VP1StdCollection::largeChangesEnd()
{
  if (!m_d->collsep)
    message("ERROR: largeChangesEnd() called before init()");
  if (m_d->largechangescount_sep>0) {
    if (--(m_d->largechangescount_sep)==0) {
      m_d->collsep->enableNotify(true);
      m_d->collsep->touch();
    }
  }
  if (m_d->largechangescount_switch>0) {
    if (--(m_d->largechangescount_switch)==0) {
      m_d->theswitch->enableNotify(true);
      m_d->theswitch->touch();
    }
  }
}

//____________________________________________________________________
QList<QWidget*> VP1StdCollection::provideWidgetsForGuiRow() const
{
  m_d->extraWidgets = provideExtraWidgetsForGuiRow();

  QList<QWidget*> l;
  l << m_d->checkBox;
  l << m_d->matButton;
  l << m_d->extraWidgets;
  return l;
}

//____________________________________________________________________
QByteArray VP1StdCollection::persistifiableState() const
{
	messageDebug("VP1StdCollection::persistifiableState()");

  if (!m_d->material) {
    message("ERROR: persistifiableState() called before init()");
    return QByteArray();
  }
  VP1Serialise serialise(1/*version*/);
  serialise.disableUnsavedChecks();
  serialise.save(m_d->visible);
  Q_ASSERT(m_d->material&&"Did you forget to call init() on this VP1StdCollection?");
  serialise.save(m_d->material);
  serialise.save(extraWidgetsState());//version 1+
  return serialise.result();
}

//____________________________________________________________________
void VP1StdCollection::setState(const QByteArray&state)
{
  if (!m_d->material) {
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

  VP1QtInventorUtils::deserialiseSoMaterial(matState,m_d->material);
  m_d->matButton->copyValuesFromMaterial(m_d->material);
  // m_d->material->restoreFromState(matState);
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);
}

//____________________________________________________________________
qint32 VP1StdCollection::collTypeID() const
{
  if (!m_d->idProvided) {
    m_d->idProvided = true;
    m_d->id = provideCollTypeID();
  }
  return m_d->id;
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
  return m_d->lastMatTransparency;
}

//____________________________________________________________________
double VP1StdCollection::collMaterialBrightness() const
{
  return m_d->lastMatBrightness;
}

//____________________________________________________________________
void VP1StdCollection::possibleChangeMatTranspOrBrightness()
{
  double t = m_d->matButton->lastAppliedTransparency();
  double b = m_d->matButton->lastAppliedBrightness();
  if ( m_d->lastMatTransparency == t && m_d->lastMatBrightness == b )
    return;
  m_d->lastMatTransparency = t;
  m_d->lastMatBrightness = b;
  collMaterialTransparencyAndBrightnessChanged();
}
