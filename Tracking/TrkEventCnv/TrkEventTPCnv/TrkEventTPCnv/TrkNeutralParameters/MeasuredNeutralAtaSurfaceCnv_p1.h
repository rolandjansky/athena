/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MeasuredNeutralAtaSurfaceCnv_p1_H
#define MeasuredNeutralAtaSurfaceCnv_p1_H

//-----------------------------------------------------------------------------
//
// file:   MeasuredAtaSurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "TrkParameters/TrackParameters.h"

#include "TrkNeutralParameters/NeutralParameters.h"


#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurface_p1.h"

#include "NeutralAtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"



class MsgStream;

template< class SURFACE_CNV, class ATA_SURFACE >
class MeasuredNeutralAtaSurfaceCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::NeutralParameters, ATA_SURFACE, Trk::MeasuredAtaSurface_p1>
{
public:
  MeasuredNeutralAtaSurfaceCnv_p1() : m_errorMxCnv(0) {}
  virtual ~MeasuredNeutralAtaSurfaceCnv_p1() {}
  
  void persToTrans( const Trk::MeasuredAtaSurface_p1 *, ATA_SURFACE *, MsgStream & );
  void transToPers( const ATA_SURFACE *, Trk::MeasuredAtaSurface_p1 *, MsgStream & );

  virtual void	initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
     m_surfaceCnv.setTopConverter(topCnv, 0);
  }
  
private:
  ErrorMatrixCnv_p1 	*m_errorMxCnv;  
  SURFACE_CNV 		m_surfaceCnv;
};


class MeasuredNeutralAtaDiscCnv_p1
   : public MeasuredNeutralAtaSurfaceCnv_p1< NeutralAtaDiscCnv_p1, Trk::NeutralAtaDisc > {};

class MeasuredNeutralAtaCylinderCnv_p1
   : public MeasuredNeutralAtaSurfaceCnv_p1< NeutralAtaCylinderCnv_p1, Trk::NeutralAtaCylinder > {};

class MeasuredNeutralAtaPlaneCnv_p1
   : public MeasuredNeutralAtaSurfaceCnv_p1< NeutralAtaPlaneCnv_p1, Trk::NeutralAtaPlane > {};

class MeasuredNeutralAtaStraightLineCnv_p1
   : public MeasuredNeutralAtaSurfaceCnv_p1< NeutralAtaStraightLineCnv_p1, Trk::NeutralAtaStraightLine > {};

#endif
