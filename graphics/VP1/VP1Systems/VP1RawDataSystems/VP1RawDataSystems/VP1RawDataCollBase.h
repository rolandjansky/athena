/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataCollBase                  //
//                                                            //
//  Description: ...                                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATACOLLBASE_H
#define VP1RAWDATACOLLBASE_H

#include "VP1Base/VP1StdCollection.h"
#include "VP1RawDataSystems/VP1RawDataFlags.h"
#include "VP1Base/VP1Interval.h"
#include <vector>

class VP1RawDataHandleBase;
class VP1RawDataCommonData;
class VP1ExtraSepLayerHelper;

class VP1RawDataCollBase : public VP1StdCollection {

  Q_OBJECT

public:

  VP1RawDataCollBase(VP1RawDataCommonData*,const QString& key);
  virtual ~VP1RawDataCollBase();
  virtual void init(VP1MaterialButtonBase* mat=0);

  VP1RawDataCommonData * common() const { return m_common; }
  VP1ExtraSepLayerHelper * sepHelper() const;

  virtual bool usesInDetPartsCuts() { return false; }//Override if InDet

public slots:
  void setAllowedEta(const VP1Interval&);
  void setAllowedPhi(const QList<VP1Interval>&);
  void setEnabledInDetParts(VP1RawDataFlags::InDetPartsFlags);

//Nb: RawDataHandleBase needs eta() and phi() method.

protected:
  QString provideText() const;

//For use in load() reimplementations:
  void addHandle(VP1RawDataHandleBase*);//Return pointer to the handle. Base class (this) will own it.

  virtual bool cut(VP1RawDataHandleBase*) = 0;//Return true if should be shown (based on various cuts not including eta and phi), false otherwise.

  //For rechecking cut status:
  void recheckCutStatus(VP1RawDataHandleBase*);
  void recheckCutStatusOfAllHandles();
  void recheckCutStatusOfAllVisibleHandles();
  void recheckCutStatusOfAllNotVisibleHandles();

  std::vector<VP1RawDataHandleBase*>& getHandles();

private slots:
  void collVisibilityChanged(bool);

private:
  VP1RawDataCommonData * m_common;
  class Imp;
  Imp * d;

};

#endif
