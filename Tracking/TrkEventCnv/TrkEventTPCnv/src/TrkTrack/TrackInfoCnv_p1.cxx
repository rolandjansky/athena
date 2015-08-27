/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackInfoCnv_p1.cxx
//
//-----------------------------------------------------------------------------
#define private public
#include "TrkTrack/TrackInfo.h"
#undef private

#include "TrkEventTPCnv/TrkTrack/TrackInfoCnv_p1.h"

void TrackInfoCnv_p1::persToTrans( const Trk::TrackInfo_p1 * persObj, Trk::TrackInfo * transObj, MsgStream& )
{
  transObj->m_fitter              = static_cast<Trk::TrackInfo::TrackFitter>(persObj->m_fitter);
  transObj->m_particleHypo        = static_cast<Trk::ParticleHypothesis>(persObj->m_particleHypo);

  transObj->m_properties 	  = std::bitset<Trk::TrackInfo::NumberOfTrackProperties>(persObj->m_properties);
 
 // set first 32 bits
  transObj->m_patternRecognition  = std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>(persObj->m_patternRecognition);
  
  
  for (unsigned int i = 32;i<Trk::TrackInfo::NumberOfTrackRecoInfo;++i)
  { 
    unsigned int mask = (1<<(i-32));
    transObj->m_patternRecognition[i] = (persObj->m_extPatternRecognition & mask );  
  }   
      
 // std::cout<<" The bitset is read back "<<std::endl;
 // if(transObj->patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum) ) 
 //   std::cout<<"This is low Pt !"<<std::endl;
 // std::cout<<transObj->dumpBitsPatternRecoInfo()<<std::endl;;   
      
}//end  of pers to trans method


void TrackInfoCnv_p1::transToPers( const Trk::TrackInfo * transObj, Trk::TrackInfo_p1 * persObj, MsgStream& /**log*/)
{
//  std::cout<<"Writing object out "<<std::endl;
//  if(transObj->patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum) ) 
//    std::cout<<"This is low Pt !"<<std::endl;
  
  persObj->m_fitter              = static_cast<unsigned int>(transObj->m_fitter);
  persObj->m_particleHypo        = static_cast<unsigned int>(transObj->m_particleHypo);
  persObj->m_properties          = transObj->m_properties.to_ulong();

  if (transObj->m_patternRecognition.size()<32) 
  {
   persObj->m_patternRecognition  = transObj->m_patternRecognition.to_ulong();
  }else{
      
// more 32 bits so have to do it the hard way.
   unsigned int i = 0;
   unsigned int size=transObj->m_patternRecognition.size();
 
 //  std::cout<<transObj->dumpBitsPatternRecoInfo()<<std::endl;;
 
   for (;i<32;++i)       persObj->m_patternRecognition    |= ((transObj->m_patternRecognition[i]) << i);
   for (i=32;i<size;++i) persObj->m_extPatternRecognition |= ((transObj->m_patternRecognition[i]) << (i-32));

// CHECK!
   // set first 32 bits
    // std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo> test  = std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>(persObj->m_patternRecognition);
    // 
    // 
    // for (unsigned int i = 32;i<Trk::TrackInfo::NumberOfTrackRecoInfo;++i)
    // { 
    //   unsigned int mask = (1<<(i-32));
    //   test[i] = (persObj->m_extPatternRecognition & mask );  
    // }
    // 
    // if (test!=transObj->m_patternRecognition){
    //   log<<MSG::WARNING<<"TIC Trackinfos don't match! "<<endreq;
    //   log<<MSG::WARNING<<test.to_string<char, std::char_traits<char>,std::allocator<char> >()<<endreq;
    //   log<<MSG::WARNING<<transObj->m_patternRecognition.to_string<char, std::char_traits<char>,std::allocator<char> >()<<endreq;
    // }  

 
  }
}//end of trans to pers method
