/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SURFACE_CNV_P2_H
#define SURFACE_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   SurfaceCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h" 

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkDistortedSurfaces/SaggedLineSurface.h"
#include "TrkEventTPCnv/TrkSurfaces/Surface_p2.h"

class MsgStream;

template< class SURFACE >
class SurfaceCnv_p2
   : public T_AthenaPoolTPPolyCnvBase<Trk::Surface, SURFACE, Trk::Surface_p2>
{
public:
  SurfaceCnv_p2() : m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"){}
        
  void persToTrans( const Trk::Surface_p2*, SURFACE*, MsgStream& );
  void transToPers( const SURFACE*, Trk::Surface_p2*, MsgStream& );
  
  SURFACE* createTransient( const Trk::Surface_p2 * persObj, MsgStream& log);

  void setCnvToolName (const std::string& name)
  { m_eventCnvTool.setTypeAndName(name); }

private:
  ToolHandle<Trk::IEventCnvSuperTool>        m_eventCnvTool;    
};

class ConeSurfaceCnv_p2
   : public SurfaceCnv_p2< Trk::ConeSurface> {};
class CylinderSurfaceCnv_p2
   : public SurfaceCnv_p2< Trk::CylinderSurface> {};
class DiscSurfaceCnv_p2
   : public SurfaceCnv_p2< Trk::DiscSurface> {};
class PerigeeSurfaceCnv_p2
   : public SurfaceCnv_p2< Trk::PerigeeSurface> {};
class PlaneSurfaceCnv_p2
   : public SurfaceCnv_p2< Trk::PlaneSurface> {};
class StraightLineSurfaceCnv_p2
   : public SurfaceCnv_p2< Trk::StraightLineSurface> {};
class SaggedLineSurfaceCnv_p2
  : public SurfaceCnv_p2< Trk::SaggedLineSurface> {};


#endif // SURFACE_CNV_p2_H
