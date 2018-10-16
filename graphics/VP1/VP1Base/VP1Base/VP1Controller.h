/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Controller                       //
//                                                            //
//  Description: Common base class for VP1 controllers.       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: August 2008                              //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1CONTROLLER_H
#define VP1CONTROLLER_H

#include "VP1Base/VP1HelperClassBase.h"
#include "VP1Base/VP1Interval.h"

#include <QWidget>

class QPushButton;
class QAbstractButton;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QSpinBox;
class QDoubleSpinBox;
class QRadioButton;
class QSlider;
class QScrollArea;
class QButtonGroup;
class SbColor4f;
class VP1CollectionWidget;
class VP1ColorSelectButton;
class VP1Serialise;
class VP1Deserialise;
class SoMaterial;
class VP1MaterialButton;
class VP1CustomTourEditor;

class VP1Controller : public QWidget, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1Controller(IVP1System * sys,const QString& classname);
  virtual ~VP1Controller();

  void initLastVars(); //call at end of derived controller (after all addUpdateSlot calls & connection setups)

  QByteArray saveSettings() const;
  void restoreSettings(QByteArray);

  // Need to be able to pass signals from customtoureditor to
  static void setCustomTourEditor(VP1CustomTourEditor* editor) { m_customTourEditor=editor;}
  static VP1CustomTourEditor* customTourEditor() {return m_customTourEditor;}


protected:

  template <class T>
  bool changed( T&lastval, const T& newval );

  template <class T>
  static QString toString( const T& par );
  static QString toString( const bool& par );
  static QString toString( const QList<VP1Interval>& l );
  static QString toString( SbColor4f );

  template <class T>
  void initDialog(T& theUI, QPushButton* launchbutton, QAbstractButton* enabledButton = 0);
  void initDialog(QWidget * dialog, QPushButton* launchbutton, QPushButton* closebutton, QAbstractButton* enabledButton = 0);

  void setupCollWidgetInScrollArea(QScrollArea * scrollarea, VP1CollectionWidget* collWidget);

  virtual void actualRestoreSettings(VP1Deserialise&) = 0;
  virtual int currentSettingsVersion() const = 0;
  virtual void actualSaveSettings(VP1Serialise&) const = 0;

  const char * addUpdateSlot(const char * slot);//simply returns the argument slot for convenience
  void connectToLastUpdateSlot(QObject* sender,const char * signal);
  void connectToLastUpdateSlot(QCheckBox*);
  void connectToLastUpdateSlot(QComboBox*);
  void connectToLastUpdateSlot(QGroupBox*);
  void connectToLastUpdateSlot(QSpinBox*);
  void connectToLastUpdateSlot(QDoubleSpinBox*);
  void connectToLastUpdateSlot(QRadioButton*);
  void connectToLastUpdateSlot(QButtonGroup*);
  void connectToLastUpdateSlot(QSlider*);
  void connectToLastUpdateSlot(VP1ColorSelectButton*);

  bool initVarsMode() const { return m_initVarsMode; }

  //Convenient way to setup radio button groups (when multiple groups inside same group box):
  QButtonGroup * defineRadioButtonGroup( QRadioButton * rb0, QRadioButton * rb1,QRadioButton * rb2 = 0,
					 QRadioButton * rb3 = 0,QRadioButton * rb4 = 0,QRadioButton * rb5 = 0,
					 QRadioButton * rb6 = 0,QRadioButton * rb7 = 0,QRadioButton * rb8 = 0,
					 QRadioButton * rb9 = 0 );
  QButtonGroup * defineRadioButtonGroup( QList<QRadioButton *>);

  //convenience:
  SoMaterial * getMaterial(VP1MaterialButton*) const;
  SoMaterial * fallBackMaterial() const;


  protected slots:
  void testForChanges() { dummyUpdateTrigger(); }

  public slots:
  void toggleDialogState(QObject* widget=0);
  void enabledButtonStateChanged();
  void collWidgetContentChanged();


  signals:
  void dummyUpdateTrigger();


private:
  static VP1CustomTourEditor* m_customTourEditor;

  class Imp;
  Imp * m_d;
  bool m_initVarsMode;

};

///////////////
//  INLINES  //
///////////////

template <class T>
QString VP1Controller::toString( const T& par )
{
  return VP1String::str(par);
}

template <class T>
void VP1Controller::initDialog(T& theUI, QPushButton* launchbutton,QAbstractButton* enabledButton) {
  QWidget * w = new QWidget(0,Qt::WindowStaysOnTopHint);
  theUI.setupUi(w);
  initDialog(w, launchbutton, theUI.pushButton_close,enabledButton);
}

template <class T>
bool VP1Controller::changed( T&lastval, const T& newval )
{
  if (initVarsMode()) {
    lastval=newval;
    return false;
  }
  if (lastval==newval)
    return false;
  lastval = newval;
  return true;
}


#endif
