/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLE_CNV_P3_REC_H
#define TRACKPARTICLE_CNV_P3_REC_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/AthenaBarCodeCnv_p1.h"

#include "Particle/TrackParticle.h"
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p2.h"

#include "TrackParticleTPCnv/Particle/TrackParticle_p3.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TrkEventTPCnv/TrkTrackSummary/TrackSummaryCnv_p2.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"

#include "TrkParameters/TrackParameters.h"
#include <vector>

class MsgStream;

class TrackParticleCnv_p3: 
public T_AthenaPoolTPCnvBase<Rec::TrackParticle, Rec::TrackParticle_p3> {
public:
    TrackParticleCnv_p3() : m_trackSummaryCnv(0), m_parameterCnv(0) {}  
    virtual void persToTrans( const Rec :: TrackParticle_p3 *persObj,  Rec :: TrackParticle    *transObj, MsgStream &log );
    virtual void transToPers( const Rec :: TrackParticle    *transObj, Rec :: TrackParticle_p3 *persObj,  MsgStream &log );

    virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase * /**topCnv*/ ) {
      // Because we want to use a single convertor for all parameter types, we need to force the retrieval
      // of a convertor for TrackParameters. Can't just instantiate the right one (I think) because it needs
      // to be the exact convertor which belongs to the TL object, so that it has right place to store
      // ObjRefs.
      ITPConverter* dummy = topConverter ()->converterForType( typeid(Trk::TrackParameters));    
      if (!m_parameterCnv)  m_parameterCnv = dynamic_cast<TrackParametersCnv_p2*>(dummy); //FIXME - doesn't work for some reason...
      // std::cout<<"TrackParticleCnv_p3::initPrivateConverters - have set m_parameterCnv to "<<m_parameterCnv<<std::endl;
    }

private:

    // typedef T_AthenaPoolTPPolyVectorCnv< std::vector<const Trk::ParametersBase*>, std::vector<TPObjRef>, TrackParametersCnv_p2 > vecParametersBaseCnv_p2;

    TrackSummaryCnv_p2*                                 m_trackSummaryCnv;
    ElementLinkCnv_p3<ElementLink< TrackCollection > >  m_elementLinkTrackCollectionConverter;
    ElementLinkCnv_p3<ElementLink< VxContainer > >      m_elementLinkVxContainerConverter;

    TrackParametersCnv_p2* m_parameterCnv;
    AthenaBarCodeCnv_p1 m_AthenaBarCodeCnv;
};

#endif
