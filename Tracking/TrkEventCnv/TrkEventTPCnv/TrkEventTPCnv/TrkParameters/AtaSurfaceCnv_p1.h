/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AtaSurfaceCnv_p1_H
#define AtaSurfaceCnv_p1_H

//-----------------------------------------------------------------------------
//
// file:   AtaSurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParameters/TrackParameters.h"
#include "TrackParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/AtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h" // for some reason cant be forward declared (template business)

class MsgStream;

template< class SURFACE_CNV, class ATA_SURFACE >
class AtaSurfaceCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::TrackParameters, ATA_SURFACE, Trk::AtaSurface_p1>
{
public:
  AtaSurfaceCnv_p1(): m_paramCnv(0), m_surfaceCnv(0), m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool")  {}
  
  void persToTrans( const Trk::AtaSurface_p1 *, ATA_SURFACE *, MsgStream & );
  void transToPers( const ATA_SURFACE *, Trk::AtaSurface_p1 *, MsgStream & );

private:
  TrackParametersCnv_p1 *m_paramCnv; // base class
  SURFACE_CNV           *m_surfaceCnv; // always same type (hence no ITPConverterFor)
  ToolHandle<Trk::IEventCnvSuperTool>        m_eventCnvTool;
};


// Here we create instances of the various convertors, where the code is in AtaSurfaceCnv.icc 
// (with a specialisation for AtaStraightLine, which actually behaves like PerigeeCnv)
class AtaDiscCnv_p1 : public  AtaSurfaceCnv_p1< DiscSurfaceCnv_p1, Trk::AtaDisc > {};
class AtaCylinderCnv_p1 : public  AtaSurfaceCnv_p1< CylinderSurfaceCnv_p1, Trk::AtaCylinder > {};
class AtaPlaneCnv_p1 : public  AtaSurfaceCnv_p1< PlaneSurfaceCnv_p1, Trk::AtaPlane > {};
class AtaStraightLineCnv_p1 : public  AtaSurfaceCnv_p1< StraightLineSurfaceCnv_p1, Trk::AtaStraightLine > {};
class CurvilinearParametersCnv_p1 : public  AtaSurfaceCnv_p1< PlaneSurfaceCnv_p1, Trk::CurvilinearParameters > {};

#endif












