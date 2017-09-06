/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataCommonData                //
//                                                            //
//  Description: Common pointers, data, node-to-object maps   //
//               etc. for the raw data system                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATACOMMONDATA_H
#define VP1RAWDATACOMMONDATA_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>

class VP1RawDataHandleBase;
class VP1RawDataSysController;
class IVP13DSystem;
class HitsSoNodeManager;
class SoTransform;
class SoGroup;

class VP1RawDataCommonData : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1RawDataCommonData(IVP13DSystem * sys,VP1RawDataSysController*);
  virtual ~VP1RawDataCommonData();

  VP1RawDataSysController * controller() { return m_controller; }
  IVP13DSystem * system() { return m_system; }
  HitsSoNodeManager * nodeManager() const { return m_nodeManager; }

  void registerTransformAndHandle(SoTransform*,VP1RawDataHandleBase*);
  void clearEventData();//clears transform-to-handle map.
  VP1RawDataHandleBase* getHandle(SoTransform*);

  void updateVolumeOutlines(SoGroup*);//updates children under the group recursively.

private slots:
  void setShowVolumeOutLines(bool);

private:

  VP1RawDataSysController * m_controller;
  IVP13DSystem * m_system;
  HitsSoNodeManager * m_nodeManager;

  class Imp;
  Imp * d;

};

#endif
