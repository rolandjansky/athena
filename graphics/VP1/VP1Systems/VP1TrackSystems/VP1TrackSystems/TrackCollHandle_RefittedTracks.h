/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackCollHandle_RefittedTracks      //
//                                                            //
//  Description: Specialisation of TrkTracks which contains   //
//               Fatras truth information                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKCOLLHANDLE_REFITTEDTRACKS_H
#define TRACKCOLLHANDLE_REFITTEDTRACKS_H

#include "VP1TrackSystems/TrackCollHandle_TrkTrack.h"
#include <QList>

class TrackCollHandle_RefittedTracks : public TrackCollHandle_TrkTrack {

  Q_OBJECT

public:

  TrackCollHandle_RefittedTracks( TrackSysCommonData *,
				  const QString& name,
				  QList<const Trk::Track*> fittedtracks );
  virtual ~TrackCollHandle_RefittedTracks();

protected:
  virtual bool load();
  virtual QColor defaultColor() const { return QColor::fromRgbF(0.0f, 1.0f, 1.0f); }

private:

  class Imp;
  Imp * m_d;

};

#endif
