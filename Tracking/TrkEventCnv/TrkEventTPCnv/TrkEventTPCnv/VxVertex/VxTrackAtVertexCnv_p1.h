/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXTRACKATVERTEX_CNV_P1_H
#define VXTRACKATVERTEX_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxVertex/VxTrackAtVertex.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertex_p1.h"
 
#include "DataModelAthenaPool/ElementLinkCnv_p2.h"

#include "DataModel/ElementLink.h" 
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
 
/**
 * Converter for Transient/Persistent separation
 * of the VxVertex/VxTrackAtVertexVertex class.
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * July 2006
 * January 2007 modified for the new TP split schema
 */

 class MsgStream;
 
 class VxTrackAtVertexCnv_p1 : public T_AthenaPoolTPCnvBase< Trk::VxTrackAtVertex, Trk::VxTrackAtVertex_p1>
 {                            
  public:
  
   VxTrackAtVertexCnv_p1() : m_fqCnv(0) {}

/**  
 * Conversion methods 
 */
    void  persToTrans(const Trk::VxTrackAtVertex_p1 * persObj, Trk::VxTrackAtVertex * transObj, MsgStream &log);
  
    void  transToPers(const Trk::VxTrackAtVertex * transObj, Trk::VxTrackAtVertex_p1 * persObj, MsgStream &log); 

  private:
  
    // don't know why this one needs to be ITPConv but RecVertex does the same (and it works ...)
    ITPConverterFor<Trk::FitQuality>        	*m_fqCnv;
    ElementLinkCnv_p2<ElementLink< TrackCollection > >  m_elementLinkConverterForTrack;
    ElementLinkCnv_p2<ElementLink< Trk::TrackParticleBaseCollection > >  m_elementLinkConverterForTrackParticle;
  
  
 };//end of class definitions
 
#endif //VXTRACKATVERTEX_CNV_P1_H
