/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackCnv_p4.cxx
//
//-----------------------------------------------------------------------------
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p4.h"
#include "TrkTrack/TrackStateOnSurface.h"

namespace {
   unsigned int keepTSOS(const Trk::TrackStateOnSurface *tsos) {
      return (tsos &&
		   (   !tsos->type(Trk::TrackStateOnSurface::PartialPersistification)
		    || tsos->type(Trk::TrackStateOnSurface::PersistifyTrackParameters)
		    || tsos->type(Trk::TrackStateOnSurface::PersistifyMeasurement)) );
   }
}

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrackCnv_p4::persToTrans( const Trk::Track_p4 *persObj,
             Trk::Track    *transObj,
             MsgStream       &log )
{
  using namespace Trk;
   transObj->info().m_fitter              = static_cast<Trk::TrackInfo::TrackFitter>(persObj->m_fitter);
   transObj->info().m_particleHypo        = static_cast<Trk::ParticleHypothesis>(persObj->m_particleHypo);

   transObj->info().m_properties 	  = std::bitset<Trk::TrackInfo::NumberOfTrackProperties>(persObj->m_properties);

  // set first 32 bits
   transObj->info().m_patternRecognition  = std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>(persObj->m_patternRecognition);
   for (unsigned int i = 32;i<Trk::TrackInfo::NumberOfTrackRecoInfo;++i){ 
     unsigned int mask = (1<<(i-32));
     transObj->info().m_patternRecognition[i] = (persObj->m_extPatternRecognition & mask );  
   }
   
   // Should always be a FQ so let's just go ahead and make it...
  transObj->m_fitQuality   = std::make_unique<FitQuality>(persObj->m_chiSquared, persObj->m_numberDoF);

  //ensure we delete the ptr
  std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>> sink(
    m_trackStateVectorCnv.createTransient(&persObj->m_trackState, log));
  //move copy
  transObj->m_trackStateVector = std::move(*sink);
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrackCnv_p4::transToPers( const Trk::Track    *transObj,
             Trk::Track_p4 *persObj,
             MsgStream       &log )
{
  // std::cout<<"TrackCnv_p4::transToPers"<<std::endl;
  
  persObj->m_fitter              = static_cast<unsigned int>(transObj->info().m_fitter);
  persObj->m_particleHypo        = static_cast<unsigned int>(transObj->info().m_particleHypo);
  persObj->m_properties          = transObj->info().m_properties.to_ulong();

  if (transObj->info().m_patternRecognition.size()<32) {
    persObj->m_patternRecognition  = transObj->info().m_patternRecognition.to_ulong();
  }else{
  // more 32 bits so have to do it the hard way.
    unsigned int i = 0;
    unsigned int size=transObj->info().m_patternRecognition.size();
    for (;i<32;++i)       persObj->m_patternRecognition    |= ((transObj->info().m_patternRecognition[i]) << i);
    for (i=32;i<size;++i) persObj->m_extPatternRecognition |= ((transObj->info().m_patternRecognition[i]) << (i-32));
  }

  assert(transObj->fitQuality());
  if (transObj->m_fitQuality){
    persObj->m_chiSquared = transObj->m_fitQuality->chiSquared();
    persObj->m_numberDoF  = transObj->m_fitQuality->numberDoF();
  } else {
    log<<MSG::WARNING<<"No FitQuality on track at ["<<transObj<<"]"<<" with info="<<transObj->info().dumpInfo()<<endmsg;
  }

  if (!transObj->m_trackStateVector.empty()) {
    unsigned int n_elms = 0;
    {
      for (const Trk::TrackStateOnSurface* tsos :
           (transObj->m_trackStateVector)) {
        if (keepTSOS(tsos))
          ++n_elms;
      }
    }

    if (n_elms != transObj->m_trackStateVector.size()) {
      DataVector<const Trk::TrackStateOnSurface> pers_tsos(SG::VIEW_ELEMENTS);
      pers_tsos.reserve(n_elms);
      {
        for (const Trk::TrackStateOnSurface* tsos :
             (transObj->m_trackStateVector)) {
          if (keepTSOS(tsos)) {
            pers_tsos.push_back(const_cast<Trk::TrackStateOnSurface*>(tsos));
          }
        }
      }
      m_trackStateVectorCnv.transToPers(
        &pers_tsos, &persObj->m_trackState, log);
    } else {
      m_trackStateVectorCnv.transToPers(
        &transObj->m_trackStateVector, &persObj->m_trackState, log);
    }
  } else {
    m_trackStateVectorCnv.transToPers( &transObj->m_trackStateVector, &persObj->m_trackState, log );
  }
}
