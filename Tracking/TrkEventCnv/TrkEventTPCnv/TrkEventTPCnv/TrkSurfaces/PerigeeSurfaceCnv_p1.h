/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PerigeeSURFACE_CNV_P1_H
#define PerigeeSURFACE_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   PerigeeSurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/PerigeeSurface.h"

#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"


namespace Trk {
   typedef Surface_p1	PerigeeSurface_p1;
}


class MsgStream;

class PerigeeSurfaceCnv_p1
   : public T_AthenaPoolTPPolyCnvBase<Trk::Surface, Trk::PerigeeSurface, Trk::PerigeeSurface_p1>
{
public:
  PerigeeSurfaceCnv_p1() {}
        
  void persToTrans( const Trk::PerigeeSurface_p1*, Trk::PerigeeSurface*, MsgStream& );
  void transToPers( const Trk::PerigeeSurface*, Trk::PerigeeSurface_p1*, MsgStream& );


  // special case: this converter does not need to be initialized
  // becase it does not use it's own persistent storage
  // it is only used as a conversion tool
  // that is possible because  PerigeeSurface_p1 is a typedef
  SurfaceCnv_p1		m_surfaceCnv;

};


inline
void PerigeeSurfaceCnv_p1::persToTrans( const Trk::PerigeeSurface_p1 *persObj, Trk::PerigeeSurface *transObj, MsgStream &log ) {
   m_surfaceCnv.persToTrans( persObj, transObj, log );
}

inline
void PerigeeSurfaceCnv_p1::transToPers( const Trk::PerigeeSurface* transObj, Trk::PerigeeSurface_p1 *persObj, MsgStream &log ) {
   m_surfaceCnv.transToPers( transObj, persObj, log );
}

#endif 
