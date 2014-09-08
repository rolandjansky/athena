/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXTRACKATVERTEX_CNV_P2_H
#define VXTRACKATVERTEX_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkEventTPCnv/VxVertex/VxTrackAtVertex_p2.h"
 
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "DataModel/ElementLink.h" 
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"


 class MsgStream;
 
 class VxTrackAtVertexCnv_p2 : public T_AthenaPoolTPCnvBase< Trk::VxTrackAtVertex, Trk::VxTrackAtVertex_p2>
 {                            
  public:
  
   VxTrackAtVertexCnv_p2(): m_paramsCnv(0){} // : m_fqCnv(0) {}

/**  
 * Conversion methods 
 */
    void  persToTrans(const Trk::VxTrackAtVertex_p2 * persObj, Trk::VxTrackAtVertex * transObj, MsgStream &log);
  
    void  transToPers(const Trk::VxTrackAtVertex * transObj, Trk::VxTrackAtVertex_p2 * persObj, MsgStream &log); 

  private:
  
    // don't know why this one needs to be ITPConv but RecVertex does the same (and it works ...)
    // ITPConverterFor<Trk::FitQuality>         *m_fqCnv;
    ITPConverterFor<Trk::TrackParameters> * m_paramsCnv;
    ElementLinkCnv_p3<ElementLink< TrackCollection > >  m_elementLinkConverterForTrack;
    ElementLinkCnv_p3<ElementLink< Trk::TrackParticleBaseCollection > >  m_elementLinkConverterForTrackParticle;
 };
 
#endif 
