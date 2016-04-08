/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1MissingEtHandle                  //
//                                                            //
//  Description: Handle for missing Et.                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//  Updated: July 2010 by Giorgi Gvaberidze(ggvaberi@cern.ch) //
////////////////////////////////////////////////////////////////

#ifndef VP1MISSINGETHANDLE_H
#define VP1MISSINGETHANDLE_H

#include "VP1Base/VP1StdCollection.h"

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>

class SoPath;
class MissingET;

//updated: for prevent loading zero length system
//must be changed in future, for set more correct value.
#define MISSINGET_ZERO 0.0001
#define MISSINGET_ZEROLENGTH(x) (x < MISSINGET_ZERO)?(true):(false)

class VP1MissingEtHandle : public VP1StdCollection {
public:

  static QList<VP1StdCollection*> createCollections(IVP1System*sys);

  VP1MissingEtHandle(IVP1System*sys,const QString& key);
  virtual ~VP1MissingEtHandle();

  virtual QStringList clicked(SoPath*) const;
protected:
  QString key() const;
  virtual QString provideText() const;
  virtual QString provideSection() const;
  virtual QString provideSectionToolTip() const;
  virtual QString checkBoxToolTip() const { return "MissingET object with key "+key(); }
  void assignDefaultMaterial(SoMaterial*) const;
  virtual bool load();
  void addArrowToCollSep(const MissingET*);
  void truncateToCollSep(SoPath*) const;
  QStringList baseInfo(const MissingET*) const;
  virtual QColor baseCol() const;
  virtual qint32 provideCollTypeID() const { return 2; }
  virtual QByteArray providePersistifiableID() const;

private:
  class Imp;
  Imp*  d;

  //updated: added for pivot
  SoGroup         *base, *arrow, *dash;
  double           length, phi;

public:
  static double thickness;
  static double scale;
  static bool   shape;

  static void   useThickness(double);
  static void   useScale(double);
  static void   useShape(bool);
public:
  void refresh();
  //-------------------
};

#endif
