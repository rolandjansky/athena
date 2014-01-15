/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLE_CNV_P2_REC_H
#define TRACKPARTICLE_CNV_P2_REC_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/AthenaBarCodeCnv_p1.h"

#include "TrkParticleBase/TrackParticleBase.h"
#include "Particle/TrackParticle.h"

#include "TrackParticleTPCnv/Particle/TrackParticle_p2.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TrkEventTPCnv/TrkTrackSummary/TrackSummaryCnv_p2.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"

#include "TrkParameters/TrackParameters.h"
#include <vector>

class MsgStream;

class TrackParticleCnv_p2: 
public T_AthenaPoolTPCnvBase<Rec::TrackParticle, Rec::TrackParticle_p2> {
public:
    TrackParticleCnv_p2() : m_trackSummaryCnv(0) {}  
    virtual void persToTrans( const Rec :: TrackParticle_p2 *persObj,  Rec :: TrackParticle    *transObj, MsgStream &log );
    virtual void transToPers( const Rec :: TrackParticle    *transObj, Rec :: TrackParticle_p2 *persObj,  MsgStream &log );

    virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
        m_vecParametersBaseCnv.setTopConverter( topCnv, 0 );
    }

private:

    typedef T_AthenaPoolTPPolyVectorCnv< std::vector<const Trk::TrackParameters*>, std::vector<TPObjRef>, ITPConverterFor<Trk::TrackParameters> > vecParametersBaseCnv_p1;

    TrackSummaryCnv_p2         *m_trackSummaryCnv;
    ElementLinkCnv_p3<ElementLink< TrackCollection > >  m_elementLinkTrackCollectionConverter;
    ElementLinkCnv_p3<ElementLink< VxContainer > >  m_elementLinkVxContainerConverter;

    vecParametersBaseCnv_p1     m_vecParametersBaseCnv;
    AthenaBarCodeCnv_p1 m_AthenaBarCodeCnv;
};

#endif
