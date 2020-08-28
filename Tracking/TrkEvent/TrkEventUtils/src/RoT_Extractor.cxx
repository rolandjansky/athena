/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RoT_Extractor.cxx,               (c) ATLAS Detector software  //
///////////////////////////////////////////////////////////////////

#include "TrkEventUtils/RoT_Extractor.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

void Trk::RoT_Extractor::extract(std::vector<const RIO_OnTrack*>& rots, const std::vector<const MeasurementBase*>& measurements, bool convertCompRots)
{
  using namespace std;
  rots.reserve( rots.size() + measurements.size() ); 
  vector<const MeasurementBase*>::const_iterator it    = measurements.begin();
  vector<const MeasurementBase*>::const_iterator itEnd = measurements.end();
  for (; it!=itEnd ; ++it)
  {
    const Trk::RIO_OnTrack* rot = dynamic_cast<const RIO_OnTrack*>(*it);
    if ( convertCompRots && nullptr == rot) {
      const Trk::CompetingRIOsOnTrack *comprot =
        dynamic_cast<const Trk::CompetingRIOsOnTrack*>(*it);
      if (comprot) {
        rot = &comprot->rioOnTrack(comprot->indexOfMaxAssignProb());
}
    }
    if ( nullptr != rot ) { rots.push_back(rot);
}
  }
  }

void Trk::RoT_Extractor::extract(const Trk::RIO_OnTrack*& rot,
                                 const Trk::MeasurementBase* meas)
{
  rot = dynamic_cast<const RIO_OnTrack*>(meas);
  if (rot==nullptr) {
    const Trk::CompetingRIOsOnTrack *comprot =
      dynamic_cast<const Trk::CompetingRIOsOnTrack*>(meas);
    if (comprot) {
      rot = &comprot->rioOnTrack(comprot->indexOfMaxAssignProb());
}
  }
  }

