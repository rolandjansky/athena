/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1TrackSanity                      //
//                                                            //
//  Description: Helper class for ensuring track parameters,  //
//               measurements, etc. are constructed properly  //
//               before we start to ask them about their      //
//               global positions, etc.                       //
//                                                            //
//  Authors: Edward Moyse (Edward.Moyse@cern.ch)              //
//           Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch) //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1TRACKSANITY_H
#define VP1TRACKSANITY_H

#include "VP1Base/VP1HelperClassBase.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class TrackStateOnSurface;
  class MeasurementBase;
  class RIO_OnTrack;
  class PrepRawData;
}

class VP1TrackSanity : public VP1HelperClassBase {
public:

  VP1TrackSanity(IVP1System * sys=0);//sys!=0 for messages in gui.
  virtual ~VP1TrackSanity();

  bool isSafe(const Trk::TrackStateOnSurface* ) const;
  bool isSafe(const Trk::TrackParameters* ) const;
  bool isSafe(const Trk::MeasurementBase* ) const;
  bool isSafe(const Trk::RIO_OnTrack* ) const;
  bool isSafe(const Trk::PrepRawData* ) const;

  //TK: OK, maybe we can't call all of these "isSafe", if more than
  //one of the arguments inherit from the same baseclass (not sure if
  //they do apart from measurementbase/rioontrack).
private:

  class Imp;
  Imp * m_d;

};

#endif
