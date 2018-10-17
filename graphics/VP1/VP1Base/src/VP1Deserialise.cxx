/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1Deserialise                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/PhiSectionWidget.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1Base/VP1CollectionWidget.h"
#include "VP1Base/VP1EtaPhiCutWidget.h"
#include "VP1Base/VP1DrawOptionsWidget.h"
#include "VP1Base/VP1CollectionSettingsButtonBase.h"
//#include "VP1AODSystems/JetCollectionSettingsButton.h"

#include <Inventor/nodes/SoMaterial.h>

#include <QSet>
#include <QBuffer>
#include <QByteArray>
#include <QDataStream>

#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QSlider>
#include <QToolBox>
#include <QRadioButton>

//____________________________________________________________________
class VP1Deserialise::Imp {
public:
  Imp(VP1Deserialise* tc) : theclass(tc), buffer(0), state(0), checkedUnused(false), version(-1), widgetNeedingUnblock(0) {}
  VP1Deserialise * theclass;
  QByteArray byteArray;
  QBuffer * buffer;
  QDataStream * state;
  bool checkedUnused;
  qint32 version;

  QSet<QWidget*> handledWidgets;
  QSet<QWidget*> ignoredWidgets;

  QWidget * widgetNeedingUnblock;
  void block(QWidget*w) {
    assert(!widgetNeedingUnblock);
    if (w&&!w->signalsBlocked()) {
      w->blockSignals(true);
      widgetNeedingUnblock=w;
    }
  }
  void unblock() {
    if (widgetNeedingUnblock) {
      widgetNeedingUnblock->blockSignals(false);
      widgetNeedingUnblock = 0;
    }
  }

  void handle(QWidget*w) {
    if (!w)
      return;
    if (handledWidgets.contains(w))
      theclass->message("ERROR: Handled widget more than once: Type="
			+QString(w->metaObject()->className())
			+", name="+w->objectName());
    handledWidgets.insert(w);
  }
  void handle(const QWidget*w) { handle((QWidget*)(w)); }
  bool expectsPersistification(QWidget*w);

  static unsigned numberOfInstantiations;
};

unsigned VP1Deserialise::Imp::numberOfInstantiations = 0;

//____________________________________________________________________
unsigned VP1Deserialise::numberOfInstantiations()
{
  return Imp::numberOfInstantiations;
}

//____________________________________________________________________
void VP1Deserialise::decrementNumberOfInstantiations()
{
  --(Imp::numberOfInstantiations);
}

//____________________________________________________________________
VP1Deserialise::VP1Deserialise(const QByteArray & ba, IVP1System * sys)
  : VP1HelperClassBase(sys,"VP1Deserialise"), m_d(new Imp(this))
{
  ++(Imp::numberOfInstantiations);

  m_d->byteArray = ba;
  m_d->buffer = new QBuffer(&(m_d->byteArray));
  if (!m_d->buffer->open(QIODevice::ReadOnly)) {
    delete m_d->buffer;
    m_d->buffer = 0;
    m_d->version = -1;
    //Fixme: ensure that other methods do not do anything (or are not called!)
    //Ensure that we are at end of stream when finished - and print versions (data+preferred).
  } else {
    m_d->state = new QDataStream(m_d->buffer);
    m_d->version = restoreInt();
  }
}

//____________________________________________________________________
VP1Deserialise::~VP1Deserialise()
{
  if (!m_d->checkedUnused)
    message("WARNING: warnUnrestored(..) was never called!");
  if (!atEnd())
    message("Destructor WARNING: Buffer not at end!");
  m_d->buffer->close();
  delete m_d->state;
  m_d->state = 0;
  delete m_d->buffer;
  m_d->buffer = 0;
  delete m_d;
}

//____________________________________________________________________
bool VP1Deserialise::atEnd() const
{
  return m_d->buffer->atEnd();
}

//____________________________________________________________________
qint32 VP1Deserialise::version() const
{
  return m_d->version;
}

//____________________________________________________________________
QDataStream * VP1Deserialise::stream()
{
  return m_d->state;
}

//____________________________________________________________________
void VP1Deserialise::restore( SoMaterial* m )
{
  messageDebug("VP1Deserialise::restore(SoMaterial)");
  QByteArray ba(restoreByteArray());
  if (!m||ba.isEmpty())
    return;
  VP1QtInventorUtils::deserialiseSoMaterial(ba,m);
}

//____________________________________________________________________
void VP1Deserialise::restore(VP1MaterialButton* mb)
{
  messageDebug("VP1Deserialise::restore(VP1MaterialButton)");
  m_d->handle(mb);
  SoMaterial * m(0);
  QList<SoMaterial*> mats = mb ? mb->handledMaterials() : QList<SoMaterial*>();
  bool tempmat = mats.isEmpty() || !mats.at(0);
  m = tempmat ? new SoMaterial : mats.at(0);
  m->ref();
  restore(m);
  if (mb) {
    m_d->block(mb);
    mb->copyValuesFromMaterial(m);
    m_d->unblock();
  }
  if (tempmat)
    m->unref();
  else
    m->unrefNoDelete();
}

//____________________________________________________________________
QColor VP1Deserialise::restoreColor()
{
  QColor c;
  (*(m_d->state)) >> c;
  messageDebug("VP1Deserialise::restore(QColor) - name: " + c.name());
  if (verbose())
    messageVerbose("Restoring color "+str(c));
  return c;
}

//____________________________________________________________________
void VP1Deserialise::restore(VP1ColorSelectButton*cb)
{
  messageDebug("VP1Deserialise::restore(VP1ColorSelectButton) - name: " + cb->objectName());
  m_d->handle(cb);
  QColor c = restoreColor();
  if (c.isValid()&&cb&&cb->color()!=c) {
    m_d->block(cb);
    cb->setColor(c);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(PhiSectionWidget* phi)
{
  messageDebug("VP1Deserialise::restore(PhiSectionWidget) - name: " + phi->objectName());
  m_d->handle(phi);
  QByteArray ba(restoreByteArray());
  if (phi&&ba != QByteArray()) {
    m_d->block(phi);
    phi->setState(ba);
    m_d->unblock();
  }
}

//____________________________________________________________________
QByteArray VP1Deserialise::restoreByteArray()
{
  QByteArray ba;
  (*(m_d->state)) >> ba;
  if (verbose())
    messageVerbose("Restoring byte array (length = "+QString::number(ba.count())+")");
  return ba;
}

//____________________________________________________________________
void VP1Deserialise::restore(QCheckBox *cb )
{
  messageDebug("VP1Deserialise::restore(QCheckBox) - name: " + cb->objectName());

  m_d->handle(cb);

  bool b = restoreBool();

  messageDebug("bool b: "+QString::number(b));
  messageDebug("bool cb: "+QString::number( cb->isChecked() ));

  if (cb->isChecked()!=b) {
    m_d->block(cb);
    messageDebug("setting - checked: "+QString::number( b ));
    cb->setChecked(b);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(QGroupBox*gb)
{
  messageDebug("VP1Deserialise::restore(QGroupBox) - name: " + gb->objectName());
  if (!gb->isCheckable())
    message("WARNING: Asked to handled GroupBox which is not checkable: "+gb->objectName());
  m_d->handle(gb);
  bool b = restoreBool();
  if (gb->isChecked()!=b) {
    m_d->block(gb);
    gb->setChecked(b);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(QComboBox *cb)
{
  messageDebug("VP1Deserialise::restore(QComboBox) - name: " + cb->objectName());
  m_d->handle(cb);
  QString t = restoreString();
  if (t.isEmpty())
    return;
  int i = cb->findText(t);
  if (i<0||i>cb->count()-1)
    return;
  if (cb->currentIndex()!=i) {
    m_d->block(cb);
    cb->setCurrentIndex(i);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(QLineEdit* le)
{
  messageDebug("VP1Deserialise::restore(QLineEdit) - name: " + le->objectName());
  m_d->handle(le);
  QString s = restoreString();
  if (s!=le->text()) {
    m_d->block(le);
    le->setText(s);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(QDoubleSpinBox *sb, const double& unit )
{
  messageDebug("VP1Deserialise::restore(QDoubleSpinBox) - name: " + sb->objectName());
  m_d->handle(sb);
  double dbl = (unit == 1.0 ? restoreDouble() : restoreDouble()/unit);
  dbl = std:: max(std::min(dbl,sb->maximum()),sb->minimum());
  if (sb->value()!=dbl) {
    m_d->block(sb);
    sb->setValue(dbl);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(QSpinBox *sb)
{
  messageDebug("VP1Deserialise::restore(QSpinBox) - name: " + sb->objectName());
  m_d->handle(sb);
  qint32 i = restoreInt();
  i = std:: max(std::min(i,sb->maximum()),sb->minimum());
  if (sb->value()!=i) {
    m_d->block(sb);
    sb->setValue(i);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(QSlider *s)
{
  messageDebug("VP1Deserialise::restore(QSlider) - name: " + s->objectName());
  m_d->handle(s);
  qint32 i = restoreInt();
  i = std:: max(std::min(i,s->maximum()),s->minimum());
  if (s->value()!=i) {
    m_d->block(s);
    s->setValue(i);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(QToolBox *tb)
{
  messageDebug("VP1Deserialise::restore(QToolBox) - name: " + tb->objectName());
  m_d->handle(tb);
  qint32 i = restoreInt();
  if (i>=0&&i<tb->count()&&i!=tb->currentIndex()) {
    m_d->block(tb);
    tb->setCurrentIndex(i);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::restoreByTitle(QToolBox *tb)
{
  messageDebug("VP1Deserialise::restore(QToolBox) - name: " + tb->objectName());
  m_d->handle(tb);
  QString s = restoreString();
  int itarget (-1);
  for (int i = 0; i < tb->count(); ++i) {
    if (tb->itemText(i)==s) {
      itarget = i;
      break;
    }
  }
  if (itarget>0&&itarget<tb->count()&&itarget!=tb->currentIndex()) {
    m_d->block(tb);
    tb->setCurrentIndex(itarget);
    m_d->unblock();
  }
}


//____________________________________________________________________
QString VP1Deserialise::restoreString()
{
  QString t;
  (*(m_d->state)) >> t;
  if (verbose())
    messageVerbose("Restoring string "+t);
  return t;
}
//____________________________________________________________________
bool VP1Deserialise::restoreBool()
{
  bool b;
  (*(m_d->state)) >> b;
  if (verbose())
    messageVerbose("Restoring bool "+str(b));
  return b;
}
//____________________________________________________________________
double VP1Deserialise::restoreDouble()
{
  double dbl;
  (*(m_d->state)) >> dbl;
  if (verbose())
    messageVerbose("Restoring double "+str(dbl));
  return dbl;
}

//____________________________________________________________________
qint32 VP1Deserialise::restoreInt()
{
  qint32 i;
  (*(m_d->state)) >> i;
  if (verbose())
    messageVerbose("Restoring int "+str(i));
  return i;
}

//____________________________________________________________________
void VP1Deserialise::restore( QRadioButton * rb0,
			      QRadioButton * rb1,
			      QRadioButton * rb2,
			      QRadioButton * rb3,
			      QRadioButton * rb4,
			      QRadioButton * rb5,
			      QRadioButton * rb6,
			      QRadioButton * rb7,
			      QRadioButton * rb8,
			      QRadioButton * rb9 )
{
  qint32 ichecked = restoreInt();
  QList<QRadioButton *> l;
  l << rb0 << rb1 << rb2 << rb3 << rb4 << rb5 << rb6 << rb7 << rb8 << rb9;
  for (qint32 i = 0; i < l.count(); ++i) {
    if (l.at(i)) {
    	messageDebug("VP1Deserialise::restore(QRadioButton) - name: " + l.at(i)->objectName());
      m_d->handle(l.at(i));
    }
  }
  //We only change any state if we have a pointer to the one needing to be checked:
  if (ichecked<0||ichecked>=l.count()||!l.at(ichecked))
    return;
  for (qint32 i = 0; i < l.count(); ++i) {
    QRadioButton * rb = l.at(i);
    if (rb&&rb->isChecked()!=(i==ichecked)) {
      m_d->block(rb);
      rb->setChecked(i==ichecked);
      m_d->unblock();
    }
  }
}

//____________________________________________________________________
void VP1Deserialise::restore(VP1CollectionWidget*cw)
{
    	messageDebug("VP1Deserialise::restore(VP1CollectionWidget) - name: " + cw->objectName());
    	messageDebug("VP1Deserialise::restore(VP1CollectionWidget)- start...");
  m_d->handle(cw);
  ignoreWidget(cw);//To ignore all children of the collection widget.
  QByteArray ba(restoreByteArray());
  QBuffer buffer(&ba);
  buffer.open(QIODevice::ReadOnly);
  QDataStream state(&buffer);
  qint32 version;
  state >> version;
  if (version!=0)
    return;//We ignore wrong versions silently here.
  VP1CollStates cwstates;
  state >> cwstates;
  buffer.close();
  m_d->block(cw);
  cw->addStateInfo(cwstates,true/*overwrite existing*/);
  m_d->unblock();
  messageDebug("VP1Deserialise::restore(VP1CollectionWidget)- end.");
}

//____________________________________________________________________
void VP1Deserialise::restore(VP1CollectionSettingsButtonBase*w)
{
    	messageDebug("VP1Deserialise::restore(VP1CollectionSettingsButtonBase) - name: " + w->objectName());
    	messageDebug("VP1Deserialise::restore(VP1CollectionSettingsButtonBase)- start...");
  m_d->handle(w);
  ignoreWidget(w);//To ignore all children of the widget.

  QByteArray ba = restoreByteArray();
  if (w&&ba!=QByteArray()) {
    m_d->block(w);
    w->restoreFromState(ba);
    m_d->unblock();
  }
  messageDebug("VP1Deserialise::restore(VP1CollectionSettingsButtonBase)- end.");
}
////____________________________________________________________________
//void VP1Deserialise::restore(JetCollectionSettingsButton*w)
//{
//  m_d->handle(w);
//  ignoreWidget(w);//To ignore all children of the widget.
//
//  QByteArray ba = restoreByteArray();
//  if (w&&ba!=QByteArray()) {
//    m_d->block(w);
//    w->restoreFromState(ba);
//    m_d->unblock();
//  }
//}



//____________________________________________________________________
void VP1Deserialise::restore(VP1EtaPhiCutWidget*w)
{
    	messageDebug("VP1Deserialise::restore(VP1EtaPhiCutWidget) - name: " + w->objectName());
  m_d->handle(w);
  ignoreWidget(w);//To ignore all children of the widget.

  QByteArray ba = restoreByteArray();
  if (w&&ba!=QByteArray()) {
    m_d->block(w);
    w->restoreFromState(ba);
    m_d->unblock();
  }
}




//____________________________________________________________________
void VP1Deserialise::restore(VP1DrawOptionsWidget*w)
{
    	messageDebug("VP1Deserialise::restore(VP1DrawOptionsWidget) - name: " + w->objectName());
  m_d->handle(w);
  ignoreWidget(w);//To ignore all children of the widget.

  QByteArray ba = restoreByteArray();
  if (w&&ba!=QByteArray()) {
    m_d->block(w);
    w->applyState(ba);
    m_d->unblock();
  }
}

//____________________________________________________________________
void VP1Deserialise::ignoreBool()
{
  bool b;
  (*(m_d->state)) >> b;
  if (verbose())
    messageVerbose("Ignoring bool "+str(b));
}

//____________________________________________________________________
void VP1Deserialise::ignoreInt()
{
  qint32 i;
  (*(m_d->state)) >> i;
  if (verbose())
    messageVerbose("Ignoring int "+str(i));
}

//____________________________________________________________________
void VP1Deserialise::ignoreDouble()
{
  double dbl;
  (*(m_d->state)) >> dbl;
  if (verbose())
    messageVerbose("Ignoring double "+str(dbl));
}

//____________________________________________________________________
void VP1Deserialise::ignoreString()
{
  QString t;
  (*(m_d->state)) >> t;
  if (verbose())
    messageVerbose("Ignoring string "+t);
}

//____________________________________________________________________
void VP1Deserialise::ignoreByteArray()
{
  QByteArray ba;
  (*(m_d->state)) >> ba;
  if (verbose())
    messageVerbose("Ignoring byte array (length = "+QString::number(ba.count())+")");
}

//____________________________________________________________________
void VP1Deserialise::ignoreObsoletePhiSectionWidgetState()
{
  QPair<int,QList<int> > state;
  (*(m_d->state)) >> state;
  if (verbose())
    messageVerbose("Ignoring obsolete phi section widget state");
}

//____________________________________________________________________
void VP1Deserialise::ignoreWidget(QWidget*w)
{
    	messageDebug("VP1Deserialise::ignoreWidget(QWidget) - name: " + w->objectName());
  if (w)
    m_d->ignoredWidgets.insert(w);
}

//____________________________________________________________________
void VP1Deserialise::widgetHandled(QWidget*w)
{
  m_d->handle(w);
}

//____________________________________________________________________
bool VP1Deserialise::Imp::expectsPersistification(QWidget*w)
{
  //NB: Same code as in VP1Serialise::Imp::expectsPersistification
  if (!w)
    return false;

  //Fixme: Something faster than string based? Or only do this in verbose mode?

  if (w->inherits("QGroupBox"))
    return static_cast<QGroupBox*>(w)->isCheckable();

  return w->inherits("QCheckBox")
    || w->inherits("QRadioButton")
    || w->inherits("QComboBox")
    || w->inherits("QAbstractSpinBox")
    || w->inherits("QSlider")
    || w->inherits("QToolBox")
    || w->inherits("PhiSectionWidget")
    || w->inherits("VP1EtaPhiCutWidget")
    || w->inherits("VP1DrawOptionsWidget")
    || w->inherits("QLineEdit")
    || w->inherits("VP1ColorSelectButton")
    || w->inherits("VP1MaterialButton");
}

//____________________________________________________________________
void VP1Deserialise::disableUnrestoredChecks()
{
  m_d->checkedUnused = true;
}

//____________________________________________________________________
void VP1Deserialise::warnUnrestored(QObject* object)
{
  //NB: Same code as in VP1Serialise::warnUnsaved

  if (!m_d->checkedUnused)
    m_d->checkedUnused = true;

  if (!object)
    return;

  if (object->isWidgetType()&&m_d->ignoredWidgets.contains(static_cast<QWidget*>(object)))
    return;

  if (object->isWidgetType()&&!object->objectName().startsWith("qt_")) {
    QWidget * wid = static_cast<QWidget*>(object);
    if (!m_d->handledWidgets.contains(wid)&&m_d->expectsPersistification(wid)) {
      QString s("WARNING Unrestored widget of type: "+QString(wid->metaObject()->className())+" and object name = "+wid->objectName());
      if (verbose())
	message(s);
      else
	messageDebug(s);
    }
  }
  //Call recursively on all "children":
  foreach(QObject* o, object->children())
    warnUnrestored(static_cast<QWidget*>(o));
}
