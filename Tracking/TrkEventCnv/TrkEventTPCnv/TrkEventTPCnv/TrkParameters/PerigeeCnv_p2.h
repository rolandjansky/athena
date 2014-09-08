/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERIGEE_CNV_P2_H
#define PERIGEE_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParametersBase/ParametersBase.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p2.h"

#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"


class MsgStream;

class PerigeeCnv_p2 : public  T_AthenaPoolTPPolyCnvBase< Trk::TrackParameters, Trk::Perigee, Trk::Perigee_p2>
{
public:
  PerigeeCnv_p2() : m_perigeeSurfaceCnv(0),/* m_localPosCnv(0),*/ m_trackParametersCnv(0) {}

  void persToTrans( const Trk::Perigee_p2*, Trk::Perigee*, MsgStream& );
  void transToPers( const Trk::Perigee*, Trk::Perigee_p2*, MsgStream& );

  PerigeeSurfaceCnv_p1* m_perigeeSurfaceCnv; // always this type
  TrackParametersCnv_p1* m_trackParametersCnv; // base class always the same
}; 


#endif





