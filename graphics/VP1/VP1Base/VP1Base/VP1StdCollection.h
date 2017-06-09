/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1StdCollection                    //
//                                                            //
//  Description: Implements most of the VP1Collection code    //
//               for the most typical use-case of a checkbox  //
//               controlling a switch + load on the first     //
//               time, as well as a material button with the  //
//               material.                                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1STDCOLLECTION_H
#define VP1STDCOLLECTION_H

#include "VP1Base/VP1Collection.h"
#include <QObject>
class SoSwitch;
class SoSeparator;
class SoMaterial;
class VP1MaterialButtonBase;

class VP1StdCollection : public QObject, public VP1Collection {

  Q_OBJECT

public:

  VP1StdCollection(IVP1System*,const QString& helperClassName);

  virtual void init(VP1MaterialButtonBase* button=0);//Must always be called directly after construction! (since it needs virtual methods!).
                      //Reimplementations must start with a call to this base implementation.
  virtual ~VP1StdCollection();

  bool visible() const;
  bool isLoaded() const;
  bool problemsLoading() const;

  QString text() const;//Will be used on checkBox (possibly elided if too long - thus important that tooltip includes the full text()).

  //Attach this somewhere in your scenegraph:
  //Put your objects somewhere below this:
  SoSwitch * collSwitch() const;//!<Add this somewhere in your scenegraph (do not add any children here!)
  SoSeparator * collSep() const;//!<All 3D objects from this coll. belongs under here.
  SoMaterial * material() const;

  //So e.g. random materials can inherit transparency and brightness
  double collMaterialTransparency() const;
  double collMaterialBrightness() const;

  //sterilises collSep+collSwitch (if coll. has objects not under
  //collSep - reimplement and call this base implementation in
  //addition to custom sterilisations):
  virtual void largeChangesBegin();
  virtual void largeChangesEnd();

  virtual QByteArray persistifiableState() const;//!<Provide default implementation based on widget list + version
  virtual void setState(const QByteArray&);//!<Provide default implementation based on widget list + version

  //Node clicked stuff?

public slots:
  void setVisible(bool);//will also update checkbox, etc.
  void possibleChangeMatTranspOrBrightness();
protected:
  QList<QWidget*> provideWidgetsForGuiRow() const;
  virtual QString provideText() const = 0;//Will only be called once.
  virtual QString checkBoxToolTip() const { return "Collection "+text(); }
  virtual QString matButtonToolTip() const { return "Edit the material of "+text(); }
  virtual void assignDefaultMaterial(SoMaterial*) const = 0;
  virtual void collMaterialTransparencyAndBrightnessChanged(){}
  virtual bool load() = 0;
  //Is called the first time the collection is made visible: Add 3D
  //objects under the collSep(). Return false in case of problems.
  //You probably want to use systemBase() to construct an
  //VP1SGAccessHelper, etc. And do not forget calls to
  //systemBase()->updateGUI().

  //In case the collection needs more widgets, override these three:
  virtual QList<QWidget*> provideExtraWidgetsForGuiRow() const { return QList<QWidget*>(); }
  virtual QByteArray extraWidgetsState() const { return QByteArray(); }
  virtual void setExtraWidgetsState(const QByteArray&) {}

  virtual qint32 provideCollTypeID() const { return -1; }
  virtual QByteArray providePersistifiableID() const;//Implementation concatenating collTypeID() and text()
public:
  qint32 collTypeID() const;

signals:
  void visibilityChanged(bool);
private:
  VP1StdCollection( const VP1StdCollection & );
  VP1StdCollection & operator= ( const VP1StdCollection & );
  class Imp;
  Imp * d;
};

#endif
