/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataColl_BCM_RDO              //
//                                                            //
//  Description: Collection of BCM RDO's                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATACOLL_BCM_RDO_H
#define VP1RAWDATACOLL_BCM_RDO_H

#include "VP1RawDataSystems/VP1RawDataCollBase.h"
#include "VP1RawDataSystems/VP1RawDataFlags.h"

class VP1RawDataColl_BCM_RDO : public VP1RawDataCollBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);

  VP1RawDataColl_BCM_RDO(VP1RawDataCommonData*,const QString& key);
  virtual ~VP1RawDataColl_BCM_RDO();

  bool cut(VP1RawDataHandleBase*);

  //Convenience (also used by handles):
  int moduleID(int channelID) const { return channelID % 8; }

  bool useSpecialBCMHighAttMaterial() const;
public slots:
  void setUseSpecialBCMHighAttMaterial(bool);

protected:
  void assignDefaultMaterial(SoMaterial*) const;
  bool load();
  qint32 provideCollTypeID() const { return 1; }
  QString provideSection() const { return "Beam Monitors"; }

private:

  class Imp;
  Imp * d;

};

#endif
