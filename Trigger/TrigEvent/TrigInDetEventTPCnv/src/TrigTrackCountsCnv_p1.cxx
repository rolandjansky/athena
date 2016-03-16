/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigTrackCounts.h"
#include "TrigInDetEventTPCnv/TrigTrackCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p1.h"

void TrigTrackCountsCnv_p1::persToTrans( const TrigTrackCounts_p1*, 
                                         TrigTrackCounts*, 
					 MsgStream &log)
{
   log << MSG::DEBUG << "TrigTrackCountsCnv_p1::persToTrans called " << endreq;

   // No longer meaningful following EDM change.
}


void TrigTrackCountsCnv_p1::transToPers(const TrigTrackCounts*, 
	 			        TrigTrackCounts_p1*, 
				        MsgStream &log)
{
  log << MSG::DEBUG << "TrigTrackCountsCnv_p1::transToPers called " << endreq;

  // No longer meaningful following EDM change.
}
