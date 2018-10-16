/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1TrackSanity                    //
//                                                            //
//  Authors: Edward Moyse (Edward.Moyse@cern.ch)              //
//           Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch) //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/VP1TrackSanity.h"
#include "VP1Utils/VP1DetInfo.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"

//____________________________________________________________________
class VP1TrackSanity::Imp {
public:
};

//____________________________________________________________________
VP1TrackSanity::VP1TrackSanity(IVP1System*sys)
  : VP1HelperClassBase(sys,"VP1TrackSanity"), m_d(new Imp)
{
  messageVerbose("constructor");
  if (sys)
    VP1DetInfo::ensureInit(sys);
}

//____________________________________________________________________
VP1TrackSanity::~VP1TrackSanity()
{
  delete m_d;
}

//____________________________________________________________________
bool VP1TrackSanity::isSafe(const Trk::TrackStateOnSurface* tsos ) const
{
  if (!tsos)
    return false;
  //Fixme: More!
  return true;
}

//____________________________________________________________________
bool VP1TrackSanity::isSafe(const Trk::TrackParameters* pars ) const
{
  if (!pars)
    return false;
  if (!(&pars->associatedSurface()))
    return false;
  //Fixme: More!
  return true;
}

//____________________________________________________________________
bool VP1TrackSanity::isSafe(const Trk::MeasurementBase* meas ) const
{
  if (!meas)
    return false;
  //TK: The following is mainly guess work - we need master Ed to implement really useful stuff!
  if (!&meas->associatedSurface())
    return false;
  if (!meas->associatedSurface().associatedDetectorElement())
    return false;
  if (!meas->associatedSurface().associatedDetectorElementIdentifier().is_valid())
    return false;
  const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
  if (rio) {
    if (VP1DetInfo::isUnsafe(rio->identify()))
      return false;
    if ( !rio->detectorElement() )
      return false;
  }
  return true;
}

//____________________________________________________________________
bool VP1TrackSanity::isSafe(const Trk::RIO_OnTrack* rio ) const
{
  if (!rio)
    return false;
  //TK: The following is mainly guess work - we need master Ed to implement really useful stuff!
  if (VP1DetInfo::isUnsafe(rio->identify()))
    return false;

  if (!rio->detectorElement())
    return false;
  return true;
}

//____________________________________________________________________
bool VP1TrackSanity::isSafe( const Trk::PrepRawData* prd ) const
{
  if (!prd)
    return false;
  if (VP1DetInfo::isUnsafe(prd->identify()))
    return false;
  return true;
}
