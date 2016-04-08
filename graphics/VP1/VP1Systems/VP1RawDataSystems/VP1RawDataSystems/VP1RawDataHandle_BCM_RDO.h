/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataHandle_BCM_RDO            //
//                                                            //
//  Description: Handle for BCM RDO's                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009.                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAHANDLE_BCM_RDO_H
#define VP1RAWDATAHANDLE_BCM_RDO_H

#include "VP1RawDataSystems/VP1RawDataHandleBase.h"
class BCM_RawData;

class VP1RawDataHandle_BCM_RDO : public VP1RawDataHandleBase {
public:

  VP1RawDataHandle_BCM_RDO(VP1RawDataCollBase*,int moduleID, QList<const BCM_RawData*>);
  virtual ~VP1RawDataHandle_BCM_RDO();

  QStringList clicked(bool verbose) const;

  int moduleID() const { return m_moduleID; }
  QString moduleName() const;
  QString moduleNick() const;
  QString modulePosDescription() const;

  int numberOfHighAttenuationHits() const { return m_nHighAttenuationHits; }
  int nHits() const { return m_data.count(); }

  static bool isHighAttenuationChannel(int channelID) { return channelID < 8; }

protected:
  class Imp;
  SoNode * buildShape();
  SoTransform * buildTransform();
  int m_moduleID;
  int m_nHighAttenuationHits;
  QList<const BCM_RawData*> m_data;
};

#endif
