/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MeasuredAtaSurfaceCnv_p1_H
#define MeasuredAtaSurfaceCnv_p1_H

//-----------------------------------------------------------------------------
//
// file:   MeasuredAtaSurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "TrkParameters/TrackParameters.h"

#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurface_p1.h"

#include "AtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"



class MsgStream;

template< class SURFACE_CNV, class ATA_SURFACE >
class MeasuredAtaSurfaceCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::TrackParameters, ATA_SURFACE, Trk::MeasuredAtaSurface_p1>
{
public:
  MeasuredAtaSurfaceCnv_p1() : m_errorMxCnv(0) {}
  virtual ~MeasuredAtaSurfaceCnv_p1() {}
  
  void persToTrans( const Trk::MeasuredAtaSurface_p1 *, ATA_SURFACE *, MsgStream & );
  void transToPers( const ATA_SURFACE *, Trk::MeasuredAtaSurface_p1 *, MsgStream & );

  virtual void  initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
     m_surfaceCnv.setTopConverter(topCnv, 0);
  }
  
private:
  ErrorMatrixCnv_p1     *m_errorMxCnv;  
  SURFACE_CNV           m_surfaceCnv;
};

// No more 'measured' parameters since rel19 - now all parameters can be measured 
class MeasuredAtaDiscCnv_p1
   : public MeasuredAtaSurfaceCnv_p1< AtaDiscCnv_p1, Trk::AtaDisc > {};

class MeasuredAtaCylinderCnv_p1
   : public MeasuredAtaSurfaceCnv_p1< AtaCylinderCnv_p1, Trk::AtaCylinder > {};

class MeasuredAtaPlaneCnv_p1
   : public MeasuredAtaSurfaceCnv_p1< AtaPlaneCnv_p1, Trk::AtaPlane > {};

class MeasuredAtaStraightLineCnv_p1
   : public MeasuredAtaSurfaceCnv_p1< AtaStraightLineCnv_p1, Trk::AtaStraightLine > {};

class MeasuredCurvilinearParametersCnv_p1
   : public MeasuredAtaSurfaceCnv_p1< CurvilinearParametersCnv_p1, Trk::CurvilinearParameters > {}; 


#endif
