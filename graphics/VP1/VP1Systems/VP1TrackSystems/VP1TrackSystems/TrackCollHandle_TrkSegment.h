/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackCollHandle_TrkSegment          //
//                                                            //
//  Description: fixme                                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKCOLLHANDLE_TRKSEGMENT_H
#define TRACKCOLLHANDLE_TRKSEGMENT_H

#include "VP1TrackSystems/TrackCollHandleBase.h"
#include <QStringList>

class TrackHandle_TrkSegment;

class TrackCollHandle_TrkSegment : public TrackCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  TrackCollHandle_TrkSegment(TrackSysCommonData *,
			     const QString& name);
  virtual ~TrackCollHandle_TrkSegment();

  void setupSettingsFromControllerSpecific(TrackSystemController*);

  void visibleStateUpdated(TrackHandle_TrkSegment*);//Must be called whenever visibility/material of handle changes.
  bool mightHaveSubSystemHitInfo() const { return true; }

protected:
  bool load();
  bool cut(TrackHandleBase*);
  virtual QColor defaultColor() const { return QColor::fromRgbF(0.75f, 0.39f, 1.0f ); }
  virtual bool allowColourByMomentum() const { return false; }
  virtual bool allowColourByCharge() const { return false; }

private:

  class Imp;
  Imp * m_d;

};

#endif
