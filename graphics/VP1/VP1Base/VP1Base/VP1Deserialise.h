/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Deserialise                      //
//                                                            //
//  Description: ...                                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1DESERIALISE_H
#define VP1DESERIALISE_H

#include "VP1Base/VP1HelperClassBase.h"
#include <typeinfo>
class VP1EtaPhiCutWidget;
class VP1DrawOptionsWidget;
class PhiSectionWidget;
class VP1MaterialButton;
class VP1ColorSelectButton;
class VP1CollectionWidget;
//class JetCollectionSettingsButton;
class VP1CollectionSettingsButtonBase;
class SoMaterial;
class QCheckBox;
class QGroupBox;
class QComboBox;
class QLineEdit;
class QDoubleSpinBox;
class QSpinBox;
class QSlider;
class QToolBox;
class QRadioButton;
class QObject;
class QWidget;

class VP1Deserialise : public VP1HelperClassBase {
public:

  VP1Deserialise(const QByteArray &, IVP1System * sys = 0);//sys!=0 for messages in gui
  virtual ~VP1Deserialise();

  qint32 version() const;//Negative version indicates an error.

  void ignoreWidget(QWidget*);//Ignores widget and all children.
  void widgetHandled(QWidget*);//Indicates that the widget should be considered as handled.
  void warnUnrestored(QObject*);//Any (stateful) widget children whose state was not saved result in a warning!
  void disableUnrestoredChecks();//In the rare case where you really do not want to call the previous method, disable warnings by calling this.

  bool atEnd() const;

  //Bools:
  bool restoreBool();
  void restore(QCheckBox *sb);
  void restore(QGroupBox*);//Checkable groupboxes only

  //Strings:
  QString restoreString();
  void restore(QComboBox *sb);
  void restore(QLineEdit*);
  void restoreByTitle(QToolBox*);//Better if number of pages is not constant.

  //Doubles:
  double restoreDouble();
  void restore(QDoubleSpinBox *sb, const double& unit = 1.0 );

  //Ints:
  qint32 restoreInt();
  void restore(QSpinBox *sb);
  void restore(QSlider*);
  void restore(QToolBox *tb);
  void restore( QRadioButton * rb0,//Add all radio buttons in a given group here.
		QRadioButton * rb1,//We save an integer indicating which is checked.
		QRadioButton * rb2 = 0,
		QRadioButton * rb3 = 0,
		QRadioButton * rb4 = 0,
		QRadioButton * rb5 = 0,
		QRadioButton * rb6 = 0,
		QRadioButton * rb7 = 0,
		QRadioButton * rb8 = 0,
		QRadioButton * rb9 = 0 );

  //ByteArrays:
  QByteArray restoreByteArray();
  void restore(VP1MaterialButton*);
  void restore(SoMaterial*);
  void restore(VP1CollectionWidget*);
//  void restore(JetCollectionSettingsButton*w);
  void restore(VP1CollectionSettingsButtonBase*w);
  void restore(VP1EtaPhiCutWidget*);
  void restore(VP1DrawOptionsWidget*);
  void restore(PhiSectionWidget*);

  //QColor's:
  QColor restoreColor();
  void restore(VP1ColorSelectButton*);

  //Custom (with datastream):
  template <class T>
  T restore();


  //When ensuring backwards compatibility, older version()'s might
  //require calls to one of the following to ignore parts of the data
  //in the stream:
  void ignoreBool();
  void ignoreInt();
  void ignoreDouble();
  void ignoreString();
  void ignoreByteArray();
  void ignoreColor();
  void ignoreObsoletePhiSectionWidgetState();//QPair<int,QList<int> >

  static unsigned numberOfInstantiations();//Number of times the constructor was invoked.
  static void decrementNumberOfInstantiations();//Only ever call this from code in VP1Base!!

private:

  class Imp;
  Imp * m_d;
  QDataStream * stream();

};

///////////////
//  INLINES  //
///////////////

template <class T>
T VP1Deserialise::restore() {//Fallback template method
  if (verbose())
    messageVerbose("Restoring "+QString(typeid(T).name())+" via datastream operator");
  T t;
  *(stream()) >> t;
  return t;
}

#endif
