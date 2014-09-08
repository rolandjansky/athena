/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINFO_CNV_P1_H
#define TRACKINFO_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TrackInfoCnv_p1.h
//  DEPRECATED - NO LONGER USED AS OF TRACKCNV_P4
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkTrack/TrackInfo.h"
#include "TrkEventTPCnv/TrkTrack/TrackInfo_p1.h"



class MsgStream;

class TrackInfoCnv_p1
   : public T_AthenaPoolTPCnvBase<Trk::TrackInfo, Trk::TrackInfo_p1> 
{
     public:

    	 TrackInfoCnv_p1(){}

    	 virtual void persToTrans( const Trk::TrackInfo_p1 *persObj, 
    					 Trk::TrackInfo    *transObj, 
    					 MsgStream  &log);
    					 
    	 virtual void transToPers( const Trk::TrackInfo    *transObj, 
    					 Trk::TrackInfo_p1 *persObj, 
    					 MsgStream &log);  
};  
  
  
#endif // TRACKINFO_CNV_P1_H
