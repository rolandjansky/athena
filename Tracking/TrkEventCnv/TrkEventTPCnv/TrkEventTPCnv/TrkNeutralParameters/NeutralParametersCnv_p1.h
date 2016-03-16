/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   NeutralParametersCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef NEUTRAL_PARAMETERS_CNV_P1_H
#define NEUTRAL_PARAMETERS_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"

class MsgStream;

class NeutralParametersCnv_p1
   : public T_AthenaPoolTPAbstractPolyCnvBase<Trk::NeutralParameters, Trk::NeutralParameters, Trk::TrackParameters_p1>
{
public:
  NeutralParametersCnv_p1() {}
  
  void persToTrans( const Trk::TrackParameters_p1 *persObj,
		    Trk::NeutralParameters    *transObj,
		    MsgStream &log );
  
  void transToPers( const Trk::NeutralParameters    *transObj,
		    Trk::TrackParameters_p1 *persObj,
		    MsgStream &log );
};

#endif // NEUTRAL_PARAMETERS_CNV_P1_H
