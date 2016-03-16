/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkSegment/TrackSegment.h"
#include "TrkEventTPCnv/TrkSegment/TrackSegmentCnv_p1.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"

void TrackSegmentCnv_p1::persToTrans( const Trk::TrackSegment_p1 *persObj, Trk::TrackSegment *transObj, MsgStream &log )
{
//    std::cout << "TrackSegmentCnv_p1::persToTrans(...) start" << std::endl;
   fillTransFromPStore( &m_segmentCnv, persObj->m_segment, transObj, log );
 //  transObj->m_associatedSurface = createTransFromPStore( (ITPConverterFor<Trk::Surface>**)0, persObj->m_associatedSurface, log );
   ITPConverterFor<Trk::Surface>* surfaceCnv=0;
   const Trk::Surface* temp = this->createTransFromPStore( &surfaceCnv, persObj->m_associatedSurface, log );
   
   DetElementSurfaceCnv_p1* detElCnv = dynamic_cast<DetElementSurfaceCnv_p1*>(surfaceCnv); 
   if (detElCnv) {
       // have a DetElementSurface
       temp = detElCnv->createSurface(log);
   }   
   transObj->m_associatedSurface = temp;
//    std::cout << "TrackSegmentCnv_p1::persToTrans(...) end" << std::endl;
}


void TrackSegmentCnv_p1::transToPers( const Trk::TrackSegment *transObj, Trk::TrackSegment_p1 *persObj, MsgStream &log )
{
   persObj->m_segment = baseToPersistent( &m_segmentCnv, transObj, log );   
   if (transObj->m_associatedSurface->isFree() ) // if this is a free surface, write it out 'as is'
       persObj->m_associatedSurface = toPersistent((ITPConverterFor<Trk::Surface>**)0, transObj->m_associatedSurface, log);
   else { // else, make it into a DetElementSurface, to allow the correct convertor to be called
       Trk::DetElementSurface surf( *(transObj->m_associatedSurface) );
       persObj->m_associatedSurface = toPersistent((ITPConverterFor<Trk::Surface>**)0, &surf, log );
   }
}
