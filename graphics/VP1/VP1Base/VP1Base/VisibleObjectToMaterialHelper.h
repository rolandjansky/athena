/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VisibleObjectToMaterialHelper       //
//                                                            //
//  Description: Helper class for those systems that must     //
//               emit signals with lists of physics objects   //
//               pointers and associated materials            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VISIBLEOBJECTTOMATERIALHELPER_H
#define VISIBLEOBJECTTOMATERIALHELPER_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>
#include <map>
class SoMaterial;

//We use a _Base class since we can't have templates and signals in
//the same class (limitation of Qt's moc):
class VisibleObjectToMaterialHelper_Base : public QObject {
  Q_OBJECT
public:
  VisibleObjectToMaterialHelper_Base(QObject * p) : QObject(p) {}
  virtual ~VisibleObjectToMaterialHelper_Base() {}
signals:
  void visibleObjectsChanged();
private slots:
  virtual void emitSignal() = 0;
};


template <class objectT>
class VisibleObjectToMaterialHelper : public VisibleObjectToMaterialHelper_Base, public VP1HelperClassBase {
public:

  inline VisibleObjectToMaterialHelper(QObject * parent = 0, IVP1System*sys=0);
  inline virtual ~VisibleObjectToMaterialHelper();

  inline void setState( const objectT*, SoMaterial *mat );//Call with null material if the object is not visible.
  inline void setNoVisibleObjects();//call from systemerase()

  inline const std::map<const objectT*,SoMaterial*>& visibleObjects() { return m_visobjects; }

private:
  inline void emitSignal();
  inline void scheduleUpdate();
  std::map<const objectT*,SoMaterial*> m_visobjects;
  bool m_hasscheduledupdate;
};

#include "VisibleObjectToMaterialHelper.icc"

#endif
