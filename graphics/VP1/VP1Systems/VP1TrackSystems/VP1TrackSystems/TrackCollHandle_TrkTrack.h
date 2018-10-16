/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackCollHandle_TrkTrack            //
//                                                            //
//  Description: Collection handle for Trk::Tracks.           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKCOLLHANDLE_TRKTRACK_H
#define TRACKCOLLHANDLE_TRKTRACK_H

#include "VP1TrackSystems/TrackCollHandleBase.h"
#include "VP1TrackSystems/TrackCommonFlags.h"
#include <QStringList>
#include <iostream>

class TrackHandle_TrkTrack;
namespace Trk { class Track; }
class TrackCollHandle_TrkTrack : public TrackCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  TrackCollHandle_TrkTrack(TrackSysCommonData *,
			   const QString& name,
			   TrackType::Type type = TrackType::TrkTrack);
  virtual ~TrackCollHandle_TrkTrack();

  //NB: All overridden methods should be virtual since
  //TrackCollHandle_FatrasTruthTracks inherits from this.
  virtual void setupSettingsFromControllerSpecific(TrackSystemController*);
  virtual bool allowColourByVertex() const { return true;}
  
  void visibleStateUpdated(TrackHandle_TrkTrack*);//Must be called whenever visibility/material of handle changes.
  bool mightHaveSubSystemHitInfo() const { return true; }

protected:
  virtual bool load();
  virtual bool cut(TrackHandleBase*);
  virtual QColor defaultColor() const { return QColor::fromRgbF(1.0f, 0.666667f, 0.0f); }

private:

  class Imp;
  Imp * m_d;

};

#endif
