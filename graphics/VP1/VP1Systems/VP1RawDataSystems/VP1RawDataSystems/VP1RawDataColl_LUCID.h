/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataColl_LUCID                //
//                                                            //
//  Description: Collection of BCM Digits (and RDO's at some  //
//               point)                                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATACOLL_LUCID_H
#define VP1RAWDATACOLL_LUCID_H

#include "VP1RawDataSystems/VP1RawDataCollBase.h"
#include "VP1RawDataSystems/VP1RawDataFlags.h"

class VP1RawDataColl_LUCID : public VP1RawDataCollBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);

  VP1RawDataColl_LUCID(VP1RawDataCommonData*,const QString& key);
  virtual ~VP1RawDataColl_LUCID();

  bool cut(VP1RawDataHandleBase*);

public slots:
  void setMinQDC(unsigned);

protected:
  void assignDefaultMaterial(SoMaterial*) const;
  bool load();
  qint32 provideCollTypeID() const { return 1; }
  QString provideSection() const { return "Luminosity Monitors"; }

private:

  class Imp;
  Imp * d;

};

#endif
