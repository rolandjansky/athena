/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "Particle/TrackParticle.h"
#include "TrkParticleBase/TrackParticleBase.h"
#undef private
#undef protected
#include "TrackParticleTPCnv/Particle/TrackParticleCnv_p2.h"
#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

void  TrackParticleCnv_p2::persToTrans(const Rec::TrackParticle_p2 * persObj, Rec::TrackParticle * transObj, MsgStream &log){
    m_elementLinkTrackCollectionConverter.persToTrans(&persObj->m_originalTrack,&transObj->m_originalTrack,log);
    m_elementLinkVxContainerConverter.persToTrans(&persObj->m_elVxCandidate,&transObj->m_elVxCandidate,log);
    m_vecParametersBaseCnv.persToTrans( &persObj->m_trackParameters, &(transObj->m_trackParameters), log );
    transObj->m_trackSummary            = createTransFromPStore( &m_trackSummaryCnv, persObj->m_trackSummary, log);
    
    //----- Fit quality
    Trk::FitQuality *tq = new Trk::FitQuality();
    tq->m_chiSquared = persObj->m_fitQuality_m_chiSquared;
    tq->m_numberDoF  = persObj->m_fitQuality_m_numberDoF ;
    transObj->m_fitQuality=tq;

    //----- Track Info
    transObj->m_trackParticleOrigin     = static_cast<Trk::TrackParticleOrigin>               (persObj->bitsAndPieces[0]);
    Trk::TrackInfo::TrackFitter fitter  = static_cast<Trk::TrackInfo::TrackFitter>            (persObj->bitsAndPieces[1]);
    Trk::ParticleHypothesis hypothesis  = static_cast<Trk::ParticleHypothesis>                (persObj->bitsAndPieces[2]);
    Trk::TrackInfo              info(fitter, hypothesis);
    transObj->m_trackInfo = info;
    transObj->m_trackInfo.m_properties 	= std::bitset<Trk::TrackInfo::NumberOfTrackProperties>(persObj->bitsAndPieces[3]);

    transObj->m_trackInfo.m_patternRecognition  = std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>(persObj->bitsAndPieces[4]);
    // TODO - is there a better way to do the below, avoiding loop?
    for (unsigned int i = 32;i<Trk::TrackInfo::NumberOfTrackRecoInfo;++i) {
        unsigned int mask = (1<<(i-32));
        transObj->m_trackInfo.m_patternRecognition[i] = (persObj->bitsAndPieces[5] & mask );
    }
    
    //----- Kinematics
    const Amg::Vector3D& mom = transObj->definingParameters().momentum();

    transObj->setPx (mom[Trk::px]);
    transObj->setPy (mom[Trk::py]);
    transObj->setPz (mom[Trk::pz]);
    double mpi = Trk::ParticleMasses().mass[Trk::pion];
    transObj->setE (std::sqrt ((long double)mom.mag2() + (long double)mpi*mpi));

    //----- Barcode
    m_AthenaBarCodeCnv.persToTrans(&persObj->m_abc, &transObj->m_abc, log);
}

void  TrackParticleCnv_p2::transToPers(const Rec::TrackParticle * transObj, Rec::TrackParticle_p2 * persObj, MsgStream &log){
    
    m_AthenaBarCodeCnv.transToPers(&transObj->m_abc, &persObj->m_abc, log);

    m_elementLinkTrackCollectionConverter.transToPers(&transObj->m_originalTrack,&persObj->m_originalTrack,log);
    m_elementLinkVxContainerConverter.transToPers(&transObj->m_elVxCandidate,&persObj->m_elVxCandidate,log);
    m_vecParametersBaseCnv.transToPers( &(transObj->m_trackParameters), &persObj->m_trackParameters, log );
    persObj->m_trackSummary = toPersistent( &m_trackSummaryCnv, transObj->m_trackSummary, log );
    
    if (transObj->m_fitQuality){
      persObj->m_fitQuality_m_chiSquared=transObj->m_fitQuality->m_chiSquared;
      persObj->m_fitQuality_m_numberDoF =transObj->m_fitQuality->m_numberDoF;
    }

    const Trk::TrackInfo& info = transObj->m_trackInfo;
    
    persObj->bitsAndPieces[0] = static_cast<unsigned int>(transObj->m_trackParticleOrigin);
    persObj->bitsAndPieces[1] = static_cast<unsigned int>(info.m_fitter);
    persObj->bitsAndPieces[2] = static_cast<unsigned int>(info.m_particleHypo);
    persObj->bitsAndPieces[3] = info.m_properties.to_ulong();
    if (info.m_patternRecognition.size()<32) {
        persObj->bitsAndPieces[4]  = info.m_patternRecognition.to_ulong();
    } else {
        // more than 32 bits so have to do it the hard way.
        unsigned int i = 0;
        unsigned int size=info.m_patternRecognition.size();
        persObj->bitsAndPieces[4]=0;
        persObj->bitsAndPieces[5]=0;
        for (;i<32;++i)       persObj->bitsAndPieces[4]  |= ((transObj->m_trackInfo.m_patternRecognition[i]) << i);
        for (i=32;i<size;++i) persObj->bitsAndPieces[5]  |= ((transObj->m_trackInfo.m_patternRecognition[i]) << (i-32));
    }
}
