/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKRECORDSTREAMER_P0_H
#define TRACKRECORDSTREAMER_P0_H

#include "AthenaPoolServices/T_AthenaRootConverter.h"
#include "G4SimTPCnv/TrackRecord_p0.h"
#include "TrackRecord/TrackRecord.h"

//#include <iostream>

class TrackRecordStreamer_p0
   : public T_AthenaRootConverter<TrackRecord, TrackRecord_p0 >
{
public:
  TrackRecordStreamer_p0() {
     SetClassNameAndChecksum("TrackRecord",  275360258U);
  }          

  void CopyOldToNew(const TrackRecord_p0 &old_obj, TrackRecord &new_obj )
  {

     TrackRecord_p0 a(old_obj);
     new_obj=TrackRecord();
     new_obj.SetPDGCode(a.GetPDGCode());
     new_obj.SetEnergy(a.GetEnergy());
     new_obj.SetMomentum(a.GetMomentum());
     new_obj.SetPosition(a.GetPosition());
     new_obj.SetTime(0.);
     new_obj.SetBarCode(0);
     new_obj.SetVolName("None"); 
  }
  
};



#endif
