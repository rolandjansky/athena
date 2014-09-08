/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MEASURED_PERIGEE_CNV_P2_H
#define MEASURED_PERIGEE_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigee_p1.h"

#include "PerigeeCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

class MsgStream;
namespace Trk {
  using  MeasuredPerigee =Perigee;
}
class MeasuredPerigeeCnv_p2 : public  T_AthenaPoolTPPolyCnvBase< Trk::TrackParameters, Trk::MeasuredPerigee, Trk::MeasuredPerigee_p1>
{
public:
  MeasuredPerigeeCnv_p2() : m_perigeeConverter(0), m_emConverter(0) {}

  void persToTrans( const Trk::MeasuredPerigee_p1 *persObj,  Trk::MeasuredPerigee    *transObj, MsgStream &log );
  void transToPers( const Trk::MeasuredPerigee    *transObj, Trk::MeasuredPerigee_p1 *persObj,  MsgStream &log );

private:
    PerigeeCnv_p2      *m_perigeeConverter;
    ITPConverterFor<Trk::ErrorMatrix> *m_emConverter;
};

#endif
