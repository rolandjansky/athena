/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_DETELEMENTSURFACECNV_P1_H
#define TRK_DETELEMENTSURFACECNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   DetElementSurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurface_p1.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h" // for some reason cant be forward declared (template business)
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

// #include "TrkSurfaces/StraightLineSurface.h"
// #include "TrkSurfaces/DiscSurface.h"
// #include "TrkSurfaces/PlaneSurface.h"
// #include "TrkSurfaces/CylinderSurface.h"

// template< class SURFACE >
// class DetElementSurfaceCnv_p1
//    : public T_AthenaPoolTPAbstractPolyCnvBase< Trk::Surface, Trk::DetElementSurface<SURFACE>, Trk::DetElementSurface_p1 >
// {
// public:
//     DetElementSurfaceCnv_p1() {}
//     //Trk::DetElementSurface<SURFACE> * createTransient( const Trk::DetElementSurface_p1 *persObj, MsgStream & log );
//     void persToTrans( const Trk::DetElementSurface_p1 *persObj, Trk::DetElementSurface<SURFACE> *transObj, MsgStream &log ) {} 
//     void transToPers( const Trk::DetElementSurface<SURFACE> *transObj, Trk::DetElementSurface_p1 *persObj, MsgStream &log );
// };
// 
// class DetElementStraightLineSurfaceCnv_p1 : public DetElementSurfaceCnv_p1<Trk::StraightLineSurface> {};
// class DetElementDiscSurfaceCnv_p1 : public DetElementSurfaceCnv_p1<Trk::DiscSurface>();
// class DetElementPlaneSurfaceCnv_p1 : public DetElementSurfaceCnv_p1<Trk::PlaneSurface>();
// class DetElementCylinderSurfaceCnv_p1 : public DetElementSurfaceCnv_p1<Trk::CylinderSurface>();

class DetElementSurfaceCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::Surface, Trk::DetElementSurface, Trk::DetElementSurface_p1 >
{
public:
    DetElementSurfaceCnv_p1() : m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"){}
    virtual Trk::DetElementSurface * createTransient( const Trk::DetElementSurface_p1 *persObj, MsgStream & log ); // DO NOT USE
    const Trk::Surface* createSurface( MsgStream & log ); // DO USE!!!
    virtual void persToTrans( const Trk::DetElementSurface_p1 *persObj, Trk::DetElementSurface *transObj, MsgStream &log ); 
    virtual void transToPers( const Trk::DetElementSurface *transObj, Trk::DetElementSurface_p1 *persObj, MsgStream &log );
    //virtual Trk::Surface* virt_createTransFromPStore( unsigned index, MsgStream &log ){}
private:
    ToolHandle<Trk::IEventCnvSuperTool>        m_eventCnvTool;
    Identifier                                 m_id; //!< Used in a horrible hack, in createSurface()
};


#endif 
