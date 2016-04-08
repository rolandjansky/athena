/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Particle/TrackParticle.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrackParticleTPCnv/Particle/TrackParticleCnv_p2.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CxxUtils/make_unique.h"
#include <cmath>

void  TrackParticleCnv_p2::persToTrans(const Rec::TrackParticle_p2 * persObj, Rec::TrackParticle * transObj, MsgStream &log)
{
    ElementLink<TrackCollection> trackLink;
    m_elementLinkTrackCollectionConverter.persToTrans(&persObj->m_originalTrack,&trackLink,log);

    Trk::TrackParticleOrigin origin =
      static_cast<Trk::TrackParticleOrigin> (persObj->bitsAndPieces[0]);

    ElementLink<VxContainer> vxCandidate;
    m_elementLinkVxContainerConverter.persToTrans(&persObj->m_elVxCandidate,&vxCandidate,log);

    auto trkSummary = std::unique_ptr<Trk::TrackSummary>
      (createTransFromPStore( &m_trackSummaryCnv, persObj->m_trackSummary, log));

    std::vector<const Trk::TrackParameters*> parameters;
    m_vecParametersBaseCnv.persToTrans( &persObj->m_trackParameters, &parameters, log );

    auto fq = CxxUtils::make_unique<Trk::FitQuality>
      (persObj->m_fitQuality_m_chiSquared,
       persObj->m_fitQuality_m_numberDoF);

    //----- Track Info
    Trk::TrackInfo::TrackFitter fitter  = static_cast<Trk::TrackInfo::TrackFitter>            (persObj->bitsAndPieces[1]);
    Trk::ParticleHypothesis hypothesis  = static_cast<Trk::ParticleHypothesis>                (persObj->bitsAndPieces[2]);
    std::bitset<Trk::TrackInfo::NumberOfTrackProperties> properties(persObj->bitsAndPieces[3]);

    std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo> patternRecognition(persObj->bitsAndPieces[4]);
    // TODO - is there a better way to do the below, avoiding loop?
    for (unsigned int i = 32;i<Trk::TrackInfo::NumberOfTrackRecoInfo;++i) {
      unsigned int mask = (1<<(i-32));
      patternRecognition[i] = (persObj->bitsAndPieces[5] & mask );
    }
    Trk::TrackInfo              info(fitter,
                                     hypothesis,
                                     properties,
                                     patternRecognition);
    
    //----- Kinematics
    const Amg::Vector3D& mom = parameters.back()->momentum();
    double mpi = Trk::ParticleMasses().mass[Trk::pion];
    P4PxPyPzE fmom (mom[Trk::px],
                    mom[Trk::py],
                    mom[Trk::pz],
                    std::sqrt ((long double)mom.mag2() + (long double)mpi*mpi));


    *transObj = Rec::TrackParticle (trackLink,
                                    origin,
                                    vxCandidate,
                                    std::move(trkSummary),
                                    std::move(parameters),
                                    std::move(fq),
                                    info,
                                    fmom);

    //----- Barcode
    AthenaBarCodeImpl abc;
    m_AthenaBarCodeCnv.persToTrans(&persObj->m_abc, &abc, log);
    transObj->setAthenaBarCode (abc.getAthenaBarCode());
}


void  TrackParticleCnv_p2::transToPers(const Rec::TrackParticle * transObj, Rec::TrackParticle_p2 * persObj, MsgStream &log){
    
    AthenaBarCodeImpl abc;
    abc.setAthenaBarCode (transObj->getAthenaBarCode());
    m_AthenaBarCodeCnv.transToPers(&abc, &persObj->m_abc, log);

    m_elementLinkTrackCollectionConverter.transToPers(transObj->trackElementLink(),&persObj->m_originalTrack,log);
    m_elementLinkVxContainerConverter.transToPers(&transObj->reconstructedVertexLink(),&persObj->m_elVxCandidate,log);
    m_vecParametersBaseCnv.transToPers( &(transObj->trackParameters()), &persObj->m_trackParameters, log );
    persObj->m_trackSummary = toPersistent( &m_trackSummaryCnv, transObj->trackSummary(), log );
    
    if (transObj->fitQuality()){
      persObj->m_fitQuality_m_chiSquared=transObj->fitQuality()->chiSquared();
      persObj->m_fitQuality_m_numberDoF =transObj->fitQuality()->numberDoF();
    }

    const Trk::TrackInfo& info = transObj->info();
    
    persObj->bitsAndPieces[0] = static_cast<unsigned int>(transObj->particleOriginType());
    persObj->bitsAndPieces[1] = static_cast<unsigned int>(info.trackFitter());
    persObj->bitsAndPieces[2] = static_cast<unsigned int>(info.particleHypothesis());
    persObj->bitsAndPieces[3] = info.properties().to_ulong();
    if (info.patternRecognition().size()<32) {
      persObj->bitsAndPieces[4]  = info.patternRecognition().to_ulong();
    } else {
        // more than 32 bits so have to do it the hard way.
        unsigned int i = 0;
        unsigned int size=info.patternRecognition().size();
        persObj->bitsAndPieces[4]=0;
        persObj->bitsAndPieces[5]=0;
        for (;i<32;++i)       persObj->bitsAndPieces[4]  |= ((info.patternRecognition()[i]) << i);
        for (i=32;i<size;++i) persObj->bitsAndPieces[5]  |= ((info.patternRecognition()[i]) << (i-32));
    }
}
