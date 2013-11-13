/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RIO_OnTrackMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKRIO_ONTRACK_RIO_ONTRACKMAPS_H
#define TRKRIO_ONTRACK_RIO_ONTRACKMAPS_H 

#include "CLIDSvc/CLASS_DEF.h"
#include <map>


namespace Trk{
  class PrepRawData;
  class RIO_OnTrack;
  typedef std::multimap<Trk::PrepRawData*, RIO_OnTrack*> RIOtoROT_Multimap;
}

CLASS_DEF( Trk::RIOtoROT_Multimap , 160822029 , 1 )

#endif // TRKRIO_ONTRACK_RIO_ONTRACKMAPS_H
