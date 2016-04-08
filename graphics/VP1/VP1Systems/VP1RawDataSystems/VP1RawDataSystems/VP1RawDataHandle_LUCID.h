/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataHandle_LUCID              //
//                                                            //
//  Description: Handle base for LUCID Digits (and RDO's at   //
//               some point).                                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAHANDLE_LUCID_H
#define VP1RAWDATAHANDLE_LUCID_H

#include "VP1RawDataSystems/VP1RawDataHandleBase.h"

class VP1RawDataHandle_LUCID : public VP1RawDataHandleBase {
public:

  VP1RawDataHandle_LUCID(VP1RawDataCollBase*);
  virtual ~VP1RawDataHandle_LUCID();

  //Reimplement the following 5 + QStringList clicked(bool verbose) const;
  virtual unsigned short getTubeID() const = 0;
  virtual unsigned short getQDC() const = 0;
  virtual unsigned short getTDC() const = 0;
  virtual bool isSideA() const = 0;
  virtual bool isSideC() const = 0;

  //Use this in clicked(..) implementations to get the 5 basic properties above printed:
  QStringList commonClicked(const QString& prefix = "") const;

protected:
  class Imp;
  SoNode * buildShape();
  SoTransform * buildTransform();
};

#endif
