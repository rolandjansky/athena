/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CaloClusterCollection            //
//                                                            //
//  Description: VP1Collection for Calo Clusters              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2007                                //
//                                                            //
////////////////////////////////////////////////////////////////


#ifndef VP1CALOCLUSTERCOLLECTION_H
#define VP1CALOCLUSTERCOLLECTION_H

#include "VP1Base/VP1StdCollection.h"
#include "VP1Base/VP1Interval.h"
#include <QStringList>
#include <QPair>
class CaloClusterSysController;
class SoPath;

class VP1CaloClusterCollection : public VP1StdCollection {

  Q_OBJECT

public:

  static QList<VP1CaloClusterCollection*> createCollections(IVP1System*,CaloClusterSysController*);
  static double crackEta();

  VP1CaloClusterCollection(const QString& key, IVP1System * sys,CaloClusterSysController*);
  ~VP1CaloClusterCollection();

  QStringList infoOnClicked(SoPath* pickedPath);//Provides info + pops appropriately

  double highestVisibleClusterEnergy() const;
  void largeChangesBegin();
  void largeChangesEnd();

protected:
  QString provideText() const;
  void assignDefaultMaterial(SoMaterial*) const;
  bool load();
  qint32 provideCollTypeID() const { return 0; }
  QString provideSection() const;
signals:
  void highestVisibleClusterEnergyChanged();
private slots:
  void setConsiderTransverseEnergies(bool);
  void setScale(const QPair<bool,double>&);
  void setAllowedEnergies(const VP1Interval&);
  void setAllowedEta(const VP1Interval&);
  void setAllowedPhi(const QList<VP1Interval>&);
  void recheckHighestVisibleClusterEnergy();
  void setShowVolumeOutLines(bool);
private:
  class Imp;
  Imp * d;

};

#endif
