/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataColl_TRT_RDO              //
//                                                            //
//  Description: Collection of SCT RDO's                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATACOLL_TRT_RDO_H
#define VP1RAWDATACOLL_TRT_RDO_H

#include "VP1RawDataSystems/VP1RawDataCollBase.h"
#include "VP1RawDataSystems/VP1RawDataFlags.h"

class VP1RawDataColl_TRT_RDO : public VP1RawDataCollBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);

  VP1RawDataColl_TRT_RDO(VP1RawDataCommonData*,const QString& key);
  virtual ~VP1RawDataColl_TRT_RDO();

  bool cut(VP1RawDataHandleBase*);

  bool usesInDetPartsCuts() { return true; }//To get InDet parts cuts

  bool useSpecialHTMat();

public slots:
  void setRequireHT(bool);
  void setAllowedToT(const VP1Interval&);
  void setUseSpecialTRTHTMaterial(bool);

protected:
  void assignDefaultMaterial(SoMaterial*) const;
  bool load();
  qint32 provideCollTypeID() const { return 1; }
  QString provideSection() const { return "Inner Detector"; }

private:

  class Imp;
  Imp * d;

};

#endif
