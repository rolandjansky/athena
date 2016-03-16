/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticleBaseCnv_p1.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef TRACKPARTICLEBASE_CNV_P1_TRK_H
#define TRACKPARTICLEBASE_CNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkEventTPCnv/TrkParticleBase/TrackParticleBase_p1.h"

#include "DataModelAthenaPool/ElementLinkCnv_p2.h"

#include "TrkEventTPCnv/TrkTrackSummary/TrackSummaryCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"

#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"

#include "TrkParameters/TrackParameters.h"
#include <vector>

class MsgStream;

/** Convertor for Trk::TrackParticleBase.

Used in e.g. Reconstruction/TrackParticleTPCnv's TrackParticleCnv_p1.*/
class TrackParticleBaseCnv_p1: 
     public T_AthenaPoolTPCnvBase< Trk::TrackParticleBase, Trk::TrackParticleBase_p1 >
{
    public:
        TrackParticleBaseCnv_p1() : m_trackSummaryCnv(0), m_fitQualityCnv(0) {}
        virtual void persToTrans( const Trk :: TrackParticleBase_p1 *persObj,
                                        Trk :: TrackParticleBase    *transObj,
                                        MsgStream                        &log );
        virtual void transToPers( const Trk :: TrackParticleBase    *transObj,
                                        Trk :: TrackParticleBase_p1 *persObj,
                                        MsgStream                        &log );
        virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
           m_vecParametersBaseCnv.setTopConverter( topCnv, 0 );
        }

    private:
        typedef T_AthenaPoolTPPolyVectorCnv< std::vector<const Trk::TrackParameters*>,
                                          std::vector<TPObjRef>,
                                          ITPConverterFor<Trk::TrackParameters> > vecParametersBaseCnv_p1;

        TrackSummaryCnv_p1         *m_trackSummaryCnv;
        FitQualityCnv_p1           *m_fitQualityCnv;
        ElementLinkCnv_p2<ElementLink< TrackCollection > >  m_elementLinkTrackCollectionConverter;
        ElementLinkCnv_p2<ElementLink< VxContainer > >  m_elementLinkVxContainerConverter;
        vecParametersBaseCnv_p1     m_vecParametersBaseCnv;
};

#endif // TRACKPARTICLEBASE_CNV_P1_TRK_H
