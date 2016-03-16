/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticleBaseCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkEventTPCnv/TrkParticleBase/TrackParticleBaseCnv_p1.h"

void  TrackParticleBaseCnv_p1::persToTrans(const Trk::TrackParticleBase_p1 * persObj, Trk::TrackParticleBase * transObj, MsgStream &log)
{
 m_elementLinkTrackCollectionConverter.resetForCnv(persObj->m_originalTrackNames);
 m_elementLinkTrackCollectionConverter.persToTrans(&persObj->m_originalTrack,&transObj->m_originalTrack,log);
 m_elementLinkVxContainerConverter.resetForCnv(persObj->m_elVxCandidateNames);
 m_elementLinkVxContainerConverter.persToTrans(&persObj->m_elVxCandidate,&transObj->m_elVxCandidate,log);
 
 m_vecParametersBaseCnv.persToTrans( &persObj->m_trackParameters, &(transObj->m_trackParameters), log );
 
 transObj->m_trackParticleOrigin = static_cast<Trk::TrackParticleOrigin>(persObj->m_trackParticleOrigin);
 transObj->m_trackSummary    = createTransFromPStore( &m_trackSummaryCnv, persObj->m_trackSummary, log);
 transObj->m_fitQuality      = createTransFromPStore( &m_fitQualityCnv, persObj->m_fitQuality, log);

//reading the persistent TrackInfo if any

 Trk::TrackInfo::TrackFitter fitter       = static_cast<Trk::TrackInfo::TrackFitter>(persObj->m_fitter);
 Trk::ParticleHypothesis     hypothesis = static_cast<Trk::ParticleHypothesis>(persObj->m_particleHypo);
 Trk::TrackInfo              info(fitter, hypothesis);
 transObj->m_trackInfo = info;
  
 transObj->m_trackInfo.m_properties 	     = std::bitset<Trk::TrackInfo::NumberOfTrackProperties>(persObj->m_properties);
// transObj->m_trackInfo.m_patternRecognition  = std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>(persObj->m_patternRecognition);
 
  transObj->m_trackInfo.m_patternRecognition  = std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>(persObj->m_patternRecognition);
  for (unsigned int i = 32;i<Trk::TrackInfo::NumberOfTrackRecoInfo;++i) {
      unsigned int mask = (1<<(i-32));
      transObj->m_trackInfo.m_patternRecognition[i] = (persObj->m_extPatternRecognition & mask );
  }
}


void  TrackParticleBaseCnv_p1::transToPers(const Trk::TrackParticleBase * transObj, Trk::TrackParticleBase_p1 * persObj, MsgStream &log)
{
 m_elementLinkTrackCollectionConverter.resetForCnv(persObj->m_originalTrackNames);
 m_elementLinkTrackCollectionConverter.transToPers(&transObj->m_originalTrack,&persObj->m_originalTrack,log);
 m_elementLinkVxContainerConverter.resetForCnv(persObj->m_elVxCandidateNames);
 m_elementLinkVxContainerConverter.transToPers(&transObj->m_elVxCandidate,&persObj->m_elVxCandidate,log);
 
 m_vecParametersBaseCnv.transToPers( &(transObj->m_trackParameters), &persObj->m_trackParameters, log );
 
 persObj->m_trackParticleOrigin = static_cast<unsigned int>(transObj->m_trackParticleOrigin);
 persObj->m_trackSummary = toPersistent( &m_trackSummaryCnv, transObj->m_trackSummary, log );
 persObj->m_fitQuality = toPersistent( &m_fitQualityCnv, transObj->m_fitQuality, log );
 
 Trk::TrackInfo info = transObj->m_trackInfo;
 persObj->m_fitter             = static_cast<unsigned int>(info.m_fitter);
 persObj->m_particleHypo       = static_cast<unsigned int>(info.m_particleHypo);
 persObj->m_properties         = info.m_properties.to_ulong();
// persObj->m_patternRecognition = info.m_patternRecognition.to_ulong();
   if (info.m_patternRecognition.size()<32) {
      persObj->m_patternRecognition  = info.m_patternRecognition.to_ulong();
  } else {
      // more 32 bits so have to do it the hard way.
      unsigned int i = 0;
      unsigned int size=info.m_patternRecognition.size();
      persObj->m_patternRecognition=0;
      persObj->m_extPatternRecognition=0;
      for (;i<32;++i)       persObj->m_patternRecognition    |= ((transObj->m_trackInfo.m_patternRecognition[i]) << i);
      for (i=32;i<size;++i) persObj->m_extPatternRecognition |= ((transObj->m_trackInfo.m_patternRecognition[i]) << (i-32));
  }
  
  // std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo> test  = std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>(persObj->m_patternRecognition);
  // for (unsigned int i = 32;i<Trk::TrackInfo::NumberOfTrackRecoInfo;++i)
  // { 
  //   unsigned int mask = (1<<(i-32));
  //   test[i] = (persObj->m_extPatternRecognition & mask );  
  // }
  // 
  // // Leave test in for the moment.
  // // TODO - remove.
  // if (test!=transObj->m_trackInfo.m_patternRecognition){
  //   log<<MSG::WARNING<<"TPBC Trackinfos don't match! "<<endreq;
  //   std::cout<<"Trans:     "<<transObj->m_trackInfo.m_patternRecognition.to_string<char, std::char_traits<char>,std::allocator<char> >()<<std::endl;
  //   std::cout<<"Read back: "<<test.to_string<char, std::char_traits<char>,std::allocator<char> >()<<std::endl;
  //   std::cout<<"persObj->m_patternRecognition:    "<<std::hex<<persObj->m_patternRecognition<<std::endl;
  //   std::cout<<"persObj->m_extPatternRecognition: "<<persObj->m_extPatternRecognition<<std::dec<<std::endl;
  //   std::cout<<"transObj->m_trackInfo.m_patternRecognition[i]: "<<std::endl;
  //   for (unsigned int i=0;i<info.m_patternRecognition.size();++i)       std::cout<<" "<<(transObj->m_trackInfo.m_patternRecognition[i]);
  //   for (unsigned int i=0;i<info.m_patternRecognition.size();++i)       std::cout<<" "<<i;
  //   std::cout<<std::endl;
  //   
  // }
  
}
