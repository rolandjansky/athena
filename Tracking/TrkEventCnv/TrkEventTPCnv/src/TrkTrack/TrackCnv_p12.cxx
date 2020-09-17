/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/Track.h"
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p12.h"

//including old enumerate schema
#include "TrkEventTPCnv/TrkTrack/TrackInfoEnum_old.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"

static const FitQualityCnv_p1 fitQualityCnv;

void TrackCnv_p12::persToTrans( const Trk::Track_p12 *persObj, Trk::Track *transObj, MsgStream &log ){
    auto fitQuality = std::make_unique<Trk::FitQuality>();
    fitQualityCnv.persToTrans(&persObj->m_fitQuality,  fitQuality.get(), log);
    delete transObj->m_fitQuality;
    transObj->m_fitQuality = fitQuality.release();
    
    transObj->m_trackStateVector = m_trackStateVectorCnv.createTransient( &persObj->m_trackState, log );
    
//forwarding the TrackInfo from old to new version
    Trk::TrackInfo::TrackFitter  fitter = Trk::TrackInfo::Unknown;
    std::vector< unsigned int>   properties;
    std::vector< unsigned int>   patternReco;
    
    switch(static_cast<TrackAuthor_old>(persObj->m_author))
    {
     case unknown_old:
       
     case iPat_old:
       fitter = Trk::TrackInfo::iPatTrackFitter;
       break;
     case iPatBremFit_old:
       fitter = Trk::TrackInfo::iPatTrackFitter;
       properties.push_back(Trk::TrackInfo::BremFit);
       break;
     case xKalman_old: 
       fitter = Trk::TrackInfo::xKalman;
       break;
     case xKalmanBremFit_old:
       fitter = Trk::TrackInfo::xKalman;
       properties.push_back(Trk::TrackInfo::BremFit);
       break;
     case iPatLegacyCnv_old:
       fitter = Trk::TrackInfo::iPatTrackFitter;
       patternReco.push_back(Trk::TrackInfo::iPatLegacyCnv);
       break;
     case iPatLegacyCnvBremFit_old:
       fitter = Trk::TrackInfo::iPatTrackFitter;
       patternReco.push_back(Trk::TrackInfo::iPatLegacyCnv);
       properties.push_back(Trk::TrackInfo::BremFit);
       break;
     case MooreLegacyCnv_old:
       patternReco.push_back(Trk::TrackInfo::MooreLegacyCnv);
       break;
     case MooreLegacyCnvBremFit_old:
       properties.push_back(Trk::TrackInfo::BremFit);
       break; 
     case Muonboy_old:
       patternReco.push_back(Trk::TrackInfo::Muonboy);
       break;
     case xKalmanLegacyCnv_old:
       fitter = Trk::TrackInfo::xKalman;
       patternReco.push_back(Trk::TrackInfo::xKalmanLegacyCnv);
       break;
     case xKalmanLegacyCnvBremFit_old:
       fitter = Trk::TrackInfo::xKalman;
       patternReco.push_back(Trk::TrackInfo::xKalmanLegacyCnv);
       properties.push_back(Trk::TrackInfo::BremFit);
       break;
     case KalmanFitter_old:
       fitter = Trk::TrackInfo::KalmanFitter;
       break;
     case MuidComb_old:     
       patternReco.push_back(Trk::TrackInfo::MuidComb);
       break;
     case MuidStandAlone_old:
       patternReco.push_back(Trk::TrackInfo::MuidStandAlone);
       break;
     case MuidLowPt_old:
       patternReco.push_back(Trk::TrackInfo::MuidStandAlone);
       properties.push_back(Trk::TrackInfo::LowPtTrack);
       break;
     case STACO_old: 
       patternReco.push_back(Trk::TrackInfo::STACO);
       break;
     case StacoLowPt_old: 
       patternReco.push_back(Trk::TrackInfo::STACO);
       properties.push_back(Trk::TrackInfo::LowPtTrack);
       break;
     case TrigIDSCAN:
       patternReco.push_back(Trk::TrackInfo::TrigIDSCAN);
       break;
     case TrigSiTrack_old: 
       patternReco.push_back(Trk::TrackInfo::TrigSiTrack);
       break;
     case TrigTRTxK_old:
       patternReco.push_back(Trk::TrackInfo::TrigTRTxK);
       break;
     case TrigTRTLUT_old:
       patternReco.push_back(Trk::TrackInfo::TrigTRTLUT);
       break;          
     case GaussianSumFilter_old:
       fitter = Trk::TrackInfo::GaussianSumFilter;
       break;             
     case SiSPSeededTrackFinder_old:
       patternReco.push_back(Trk::TrackInfo::SiSPSeededFinder);
       break;            
     case GlobalChi2Fitter_old:
       fitter = Trk::TrackInfo::GlobalChi2Fitter;
       break;          
     case GlobalChi2FitterBremFit_old: 
       fitter = Trk::TrackInfo::GlobalChi2Fitter;
       properties.push_back(Trk::TrackInfo::BremFit);
       break;      
     case DistributedKalmanFilter_old:
       fitter = Trk::TrackInfo::DistributedKalmanFilter;
       break;         
     case DeterministicAnnealingFilter_old:
       fitter = Trk::TrackInfo::DeterministicAnnealingFilter; 
       break;      
     case KalmanDNAFitter_old:
       fitter = Trk::TrackInfo::KalmanDNAFitter;
       break;
     case TRT_SeededTrackFinder_old:
       patternReco.push_back(Trk::TrackInfo::TRTSeededTrackFinder);
       break;
     case TRT_StandAlone_old:
       patternReco.push_back(Trk::TrackInfo::TRTStandalone);             
       break;
     case NumberOfTrackAuthors_old:
       break;
    }//end of switch author method
      
    Trk::TrackInfo info(fitter , static_cast<Trk::ParticleHypothesis>(persObj->m_particleHypo));

//setting   properties 
    unsigned int pro_size = properties.size();
    unsigned int pat_size = patternReco.size();
    if(pro_size) for(unsigned int i = 0;i<pro_size;++i) 
      info.setTrackProperties(static_cast<Trk::TrackInfo::TrackProperties>(properties[i]));
    
    if(pat_size) for(unsigned int i = 0;i<pat_size;++i) 
      info.setPatternRecognitionInfo(static_cast<Trk::TrackInfo::TrackPatternRecoInfo>(patternReco[i]));  
      
    transObj->m_trackInfo = info;
}



void TrackCnv_p12::transToPers( const Trk::Track *, Trk::Track_p12 *, MsgStream & ){
  throw std::runtime_error("TrackCnv_p12::transToPers is deprecated!");   
}
