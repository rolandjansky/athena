/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERIGEE_CNV_P1_H
#define PERIGEE_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   PerigeeCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//#include "TrkEventPrimitives/LocalPosition.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p1.h"

#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"
//#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"


class MsgStream;

class PerigeeCnv_p1
   : public  T_AthenaPoolTPPolyCnvBase< Trk::TrackParameters, Trk::Perigee, Trk::Perigee_p1>
{
public:
  PerigeeCnv_p1() : m_perigeeSurfaceCnv(0),/* m_localPosCnv(0),*/ m_trackParametersCnv(0) {}


  void persToTrans( const Trk::Perigee_p1*, Trk::Perigee*, MsgStream& );
  void transToPers( const Trk::Perigee*, Trk::Perigee_p1*, MsgStream& );

  PerigeeSurfaceCnv_p1* m_perigeeSurfaceCnv; // always this type
//  ITPConverterFor<Trk::LocalPosition>	* m_localPosCnv; // not polymorphic I think ...
//  LocalPositionCnv_p1 * m_localPosCnv;
  TrackParametersCnv_p1* m_trackParametersCnv; // base class always the same
}; 


#endif // PERIGEE_CNV_P1_H





