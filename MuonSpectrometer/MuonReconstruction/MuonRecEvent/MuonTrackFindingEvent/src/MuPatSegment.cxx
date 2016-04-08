/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackFindingEvent/MuPatSegment.h"
#include <iostream>
#define DUMP_MCTB_OBJECT_POINTERS
namespace Muon {

  // Static members
  unsigned int MuPatSegment::s_maxNumberOfInstantiations = 0;
  unsigned int MuPatSegment::s_numberOfInstantiations = 0;
  unsigned int MuPatSegment::s_numberOfCopies = 0;

  // Static functions
  void MuPatSegment::resetMaxNumberOfInstantiations() {
    s_maxNumberOfInstantiations = 0;
  }

  void MuPatSegment::resetNumberOfCopies() {
    s_numberOfCopies = 0;
  }

  MuPatSegment::MuPatSegment() : MuPatCandidateBase(), quality(0.0),segQuality(0),segment(0),segPars(0),segmentIndex(-1),usedInFit(0),
				 chIndex(MuonStationIndex::ChUnknown),stIndex(MuonStationIndex::StUnknown),isEndcap(false),isMdt(false),mboyInfo(0)
				       
  {
#ifdef MCTB_OBJECT_COUNTERS
    addInstance();
#endif
#ifdef MCTB_OBJECT_POINTERS
    std::cout << " new MuPatSegment() " << this << " , instances " << s_numberOfInstantiations << std::endl;
#endif
  }
  
  MuPatSegment::~MuPatSegment() {
    delete segPars;
    if (mboyInfo) delete mboyInfo;
#ifdef MCTB_OBJECT_COUNTERS
    removeInstance();
#endif
#ifdef MCTB_OBJECT_POINTERS
    std::cout << " dtor MuPatSegment() " << this << " , instances " << s_numberOfInstantiations << std::endl;
#endif
  }

  MuPatSegment::MuPatSegment( const MuPatSegment& info ) : 
    MuPatCandidateBase(info) {
    quality = info.quality;
    segQuality = info.segQuality;
    segment = info.segment;
    segPars =  info.segPars ? info.segPars->clone() : 0;
    segmentIndex = info.segmentIndex;
    usedInFit = info.usedInFit;
    chid = info.chid;
    name = info.name;
    chIndex = info.chIndex;
    stIndex = info.stIndex;
    isEndcap = info.isEndcap;
    isMdt = info.isMdt;
      if (!info.mboyInfo){
        mboyInfo = 0;
      } else {
        mboyInfo = new MboyInfo();
        mboyInfo->m_ISC0    = info.mboyInfo->m_ISC0    ;
        mboyInfo->m_FFDRAT  = info.mboyInfo->m_FFDRAT  ;
        mboyInfo->m_GGDRAT  = info.mboyInfo->m_GGDRAT  ;
        mboyInfo->m_EZDRAT  = info.mboyInfo->m_EZDRAT  ;
        mboyInfo->m_JFDRAT  = info.mboyInfo->m_JFDRAT  ;
        mboyInfo->m_IWDRAT1 = info.mboyInfo->m_IWDRAT1 ;
        mboyInfo->m_ZZDRAT1 = info.mboyInfo->m_ZZDRAT1 ;
        mboyInfo->m_TTDRAT1 = info.mboyInfo->m_TTDRAT1 ;
        mboyInfo->m_SSDRAT1 = info.mboyInfo->m_SSDRAT1 ;
        mboyInfo->m_IWDRAT2 = info.mboyInfo->m_IWDRAT2 ;
        mboyInfo->m_ZZDRAT2 = info.mboyInfo->m_ZZDRAT2 ;
        mboyInfo->m_TTDRAT2 = info.mboyInfo->m_TTDRAT2 ;
        mboyInfo->m_SSDRAT2 = info.mboyInfo->m_SSDRAT2 ;
      }
#ifdef MCTB_OBJECT_COUNTERS
    addInstance();
    ++s_numberOfCopies;
#endif
#ifdef MCTB_OBJECT_POINTERS
    std::cout << " ctor MuPatSegment " << this << " , instances " << s_numberOfInstantiations << std::endl;
#endif
  }

  MuPatSegment& MuPatSegment::operator=( const MuPatSegment& info ) { 
    if( &info != this ){
      MuPatCandidateBase::operator=(info);
      quality = info.quality;
      segQuality = info.segQuality;
      segment = info.segment;
      delete segPars;
      segPars =  info.segPars ? info.segPars->clone() : 0;
      segmentIndex = info.segmentIndex;
      usedInFit = info.usedInFit;
      chid = info.chid;
      name = info.name;
      chIndex = info.chIndex;
      stIndex = info.stIndex;
      isEndcap = info.isEndcap;
      isMdt = info.isMdt;
      if(mboyInfo) delete mboyInfo;
      if (!info.mboyInfo){
        mboyInfo = 0;
      } else {
        mboyInfo = new MboyInfo();
        mboyInfo->m_ISC0    = info.mboyInfo->m_ISC0    ;
        mboyInfo->m_FFDRAT  = info.mboyInfo->m_FFDRAT  ;
        mboyInfo->m_GGDRAT  = info.mboyInfo->m_GGDRAT  ;
        mboyInfo->m_EZDRAT  = info.mboyInfo->m_EZDRAT  ;
        mboyInfo->m_JFDRAT  = info.mboyInfo->m_JFDRAT  ;
        mboyInfo->m_IWDRAT1 = info.mboyInfo->m_IWDRAT1 ;
        mboyInfo->m_ZZDRAT1 = info.mboyInfo->m_ZZDRAT1 ;
        mboyInfo->m_TTDRAT1 = info.mboyInfo->m_TTDRAT1 ;
        mboyInfo->m_SSDRAT1 = info.mboyInfo->m_SSDRAT1 ;
        mboyInfo->m_IWDRAT2 = info.mboyInfo->m_IWDRAT2 ;
        mboyInfo->m_ZZDRAT2 = info.mboyInfo->m_ZZDRAT2 ;
        mboyInfo->m_TTDRAT2 = info.mboyInfo->m_TTDRAT2 ;
        mboyInfo->m_SSDRAT2 = info.mboyInfo->m_SSDRAT2 ;
      }
#ifdef MCTB_OBJECT_COUNTERS
      ++s_numberOfCopies;
#endif
    }
    return *this; 
  }


  void MuPatSegment::addTrack( MuPatTrack* track ) {
#ifdef MCTB_OBJECT_POINTERS
    std::cout << " addTrack " << this << " track " << track;
    if( m_associatedTracks.count(track) ) std::cout << " DOUBLE assocation!! " << std::endl;
    std::cout << std::endl;
#endif
    m_associatedTracks.insert(track);
  }

  void MuPatSegment::removeTrack( MuPatTrack* track ) {
    // look up track
    std::set<MuPatTrack*>::iterator pos = m_associatedTracks.find(track);
    if( pos != m_associatedTracks.end() ){
      // if found remove it from list
      m_associatedTracks.erase(pos);
#ifdef MCTB_OBJECT_POINTERS
      std::cout << " removeTrack " << this << " track " << track << std::endl;
#endif
    }else{
#ifdef MCTB_OBJECT_POINTERS
      std::cout << " removeTrack: TRACK NOT FOUND " << this << " track " << track << std::endl;
#endif
    }
  }

} // namespace Muon
