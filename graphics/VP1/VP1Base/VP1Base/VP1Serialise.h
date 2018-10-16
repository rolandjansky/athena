/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Serialise                        //
//                                                            //
//  Description: Helper class for serialisation.              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1PERSISTIFY_H
#define VP1PERSISTIFY_H

#include "VP1Base/VP1HelperClassBase.h"
#include <typeinfo>
#include <QDataStream>
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
class QWidget;
class QObject;

class VP1Serialise : public VP1HelperClassBase {
public:

  VP1Serialise(qint32 version, IVP1System * sys = 0);//sys!=0 for messages in gui
  virtual ~VP1Serialise();

  QByteArray result();
  qint32 version() const;

  void ignoreWidget(const QWidget*);//Ignores widget and all children.
  void widgetHandled(const QWidget*);//Indicates that the widget should be considered as handled.
  void warnUnsaved(const QObject*);//Any (stateful) widget children whose state was not saved result in a warning!
  void disableUnsavedChecks();//In the rare case where you really do not want to call the previous method, disable warnings by calling this.

  //Fixme: pointers->const ??

  //Bools:
  void save(bool);
  void save(QCheckBox*);
  void save(QGroupBox*);//Checkable groupboxes only

  //Strings:
  void save(const QString&);
  void save(QComboBox*);
  void save(QLineEdit*);
  void saveByTitle(QToolBox*);//Better if number of pages is not constant.

  //Doubles:
  void save(const double&);
  void save(QDoubleSpinBox*, const double& unit = 1.0 );

  //Ints:
  void save(qint32);
  void save(QSpinBox*);
  void save(QSlider*);
  void save(QToolBox*);
  //fixme tabwidget
  void save( QRadioButton * rb0,//Add all radio buttons in a given group here.
 	     QRadioButton * rb1,//We save an integer indicating which is checked.
 	     QRadioButton * rb2 = 0,//Any NULL values will be taken to indicate "unchecked".
 	     QRadioButton * rb3 = 0,
 	     QRadioButton * rb4 = 0,
 	     QRadioButton * rb5 = 0,
 	     QRadioButton * rb6 = 0,
 	     QRadioButton * rb7 = 0,
 	     QRadioButton * rb8 = 0,
 	     QRadioButton * rb9 = 0 );

  //ByteArrays:
  void save(const QByteArray&);
  void save(const VP1MaterialButton*);//Persistifies values of first handled material (saves empty byte array if none handled)
  void save(SoMaterial*);
  void save(const VP1CollectionWidget*);
//  void save(const JetCollectionSettingsButton* jcb);
  void save(const VP1CollectionSettingsButtonBase* jcb);
  void save(const VP1EtaPhiCutWidget*);
  void save(const VP1DrawOptionsWidget*);
  void save(const PhiSectionWidget*);//Used to be QPair<int,QList<int> >

  //QColor's:
  void save(const QColor&);
  void save(VP1ColorSelectButton*);

  //Custom (with datastream):
  template <class T>
  void save(const T& t);
  template <class T>
  void save( T* t);//To keep previous template from swallowing const pointers constness.

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
void VP1Serialise::save(const T& t) {//Fallback template method
  if (verbose())
    messageVerbose("Saving "+QString(typeid(T).name())+" via datastream operator");
  *(stream()) << t;
}

template <class T>
void VP1Serialise::save( T* t)
{
  save((const T*)t);
}

#endif
