/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRALPERIGEE_CNV_P1_H
#define NEUTRALPERIGEE_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   NeutralPerigeeCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParameters/TrackParameters.h"

//#include "TrkEventPrimitives/LocalPosition.h"

#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p1.h"

#include "TrkEventTPCnv/TrkNeutralParameters/NeutralParametersCnv_p1.h"

#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"

class MsgStream;

class NeutralPerigeeCnv_p1
   : public  T_AthenaPoolTPPolyCnvBase< Trk::NeutralParameters, Trk::NeutralPerigee, Trk::Perigee_p1>
{
public:
  NeutralPerigeeCnv_p1() : m_perigeeSurfaceCnv(0), /*m_localPosCnv(0),*/ m_trackParametersCnv(0) {}


  void persToTrans( const Trk::Perigee_p1*, Trk::NeutralPerigee*, MsgStream& );
  void transToPers( const Trk::NeutralPerigee*, Trk::Perigee_p1*, MsgStream& );

  PerigeeSurfaceCnv_p1* m_perigeeSurfaceCnv; // always this type
//  ITPConverterFor<Trk::LocalPosition>	*m_localPosCnv; // not polymorphic I think ...
  NeutralParametersCnv_p1* m_trackParametersCnv; // base class always the same
}; 


#endif // PERIGEE_CNV_P1_H





