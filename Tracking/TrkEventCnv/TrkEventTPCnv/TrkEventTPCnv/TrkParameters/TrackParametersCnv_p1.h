/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParametersCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef TRACK_PARAMETERS_CNV_P1_H
#define TRACK_PARAMETERS_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"

class MsgStream;

class TrackParametersCnv_p1
   : public T_AthenaPoolTPAbstractPolyCnvBase<Trk::TrackParameters, Trk::TrackParameters, Trk::TrackParameters_p1>
{
public:
  TrackParametersCnv_p1() {}
  
  void persToTrans( const Trk :: TrackParameters_p1 *persObj,
		    Trk :: TrackParameters    *transObj,
		    MsgStream &log );
  
  void transToPers( const Trk::TrackParameters    *transObj,
		    Trk::TrackParameters_p1 *persObj,
		    MsgStream &log );
};

#endif // TRACK_PARAMETERS_CNV_P1_H
