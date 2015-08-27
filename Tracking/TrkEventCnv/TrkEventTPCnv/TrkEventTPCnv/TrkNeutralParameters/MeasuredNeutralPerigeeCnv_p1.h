/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MEASURED_NEUTRAL_PERIGEE_CNV_P1_H
#define MEASURED_NEUTRAL_PERIGEE_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MeasuredNeutralPerigeeCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigee_p1.h"

#include "NeutralPerigeeCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

class MsgStream;

class MeasuredNeutralPerigeeCnv_p1
   : public  T_AthenaPoolTPPolyCnvBase< Trk::NeutralParameters, Trk::NeutralPerigee, Trk::MeasuredPerigee_p1>
{
public:
  MeasuredNeutralPerigeeCnv_p1() : m_perigeeConverter(0), m_emConverter(0) {}

  void persToTrans( const Trk::MeasuredPerigee_p1 *persObj,
		    Trk::NeutralPerigee    *transObj,
		    MsgStream                 &log );
  void transToPers( const Trk::NeutralPerigee    *transObj,
		    Trk::MeasuredPerigee_p1 *persObj,
		    MsgStream                 &log );

private:
  NeutralPerigeeCnv_p1		*m_perigeeConverter;
  ErrorMatrixCnv_p1 	*m_emConverter;
};

#endif // NEUTRAL_MEASURED_PERIGEE_CNV_P1_H
