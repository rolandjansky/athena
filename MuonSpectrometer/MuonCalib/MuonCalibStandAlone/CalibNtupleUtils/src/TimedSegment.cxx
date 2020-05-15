/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleUtils/TimedSegment.h"
#include "CalibNtupleUtils/TimedMdtCalibHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibStl/DeleteObject.h"
#include <algorithm>

namespace MuonCalib {
  
  TimedSegment::TimedSegment( const MuonCalibSegment* segment ) 
    : m_segment(0), m_segment_original(0), m_time(0.), m_sigma_time(0.) {
      TimedHitVec newVec(segment->mdtHitsOnTrack(),0);
      m_tHitVec = newVec ;
      std::vector<double> dNewVec(segment->mdtHitsOnTrack(),0);
      m_timesHit = dNewVec ;
      m_sigmaTimesHit = dNewVec;
      m_radiiHit = dNewVec;
      std::vector<unsigned int> uiNewVec(segment->mdtHitsOnTrack(),0);
      m_mlHit = uiNewVec;
    
      m_segment = new MuonCalibSegment( *segment );
      m_segment_original = new MuonCalibSegment( *segment );
      
      MuonCalibSegment::MdtHitCit mdt_it     = segment->mdtCloseHitsBegin();
      MuonCalibSegment::MdtHitCit mdt_it_end = segment->mdtCloseHitsEnd();
      for( ; mdt_it!=mdt_it_end;++mdt_it){
	m_tHitVec.push_back( new TimedMdtCalibHit(**mdt_it) );
      }
      
    }
  
  TimedSegment::~TimedSegment() {
    std::for_each( hitsBegin(), hitsEnd(), DeleteObject()  ) ; 
    m_tHitVec.clear();
    m_timesHit.clear();
    m_sigmaTimesHit.clear();
    m_radiiHit.clear();
    m_mlHit.clear();
    if(m_segment) delete m_segment;
    if(m_segment_original) delete m_segment_original;
  } 
  
  TimedSegment::TimedSegment( const TimedSegment& tseg ) 
    : m_segment(0) ,  m_segment_original(0) 
    , m_time(tseg.time()) ,  m_sigma_time(tseg.sigmaTime()){
      
      m_segment = new MuonCalibSegment( *tseg.segment() );
      m_segment_original = new MuonCalibSegment( *tseg.originalSegment() );

      THitVecCit it = tseg.hitsBegin() ;
      THitVecCit it_end = tseg.hitsEnd() ;
      unsigned int i = 0;
      for( ; it!= it_end ; ++it ){
	m_tHitVec.push_back( new TimedMdtCalibHit(**it) ); 
	setTimeHit(i, tseg.timeHit(i) ) ;
	setSigmaTimeHit(i, tseg.sigmaTimeHit(i) );      
	++i;
      }    
    }

  TimedSegment& TimedSegment::operator=( const TimedSegment& rhs ){
    if(this!=&rhs) {
      std::for_each( hitsBegin(), hitsEnd(), DeleteObject() ) ;
      m_tHitVec.clear();
      m_timesHit.clear();
      m_sigmaTimesHit.clear();
      TimedHitVec temp_tHitVec ;
      THitVecCit it = rhs.hitsBegin() ;
      THitVecCit it_end = rhs.hitsEnd() ;
      unsigned int i=0;
      for( ; it!=it_end ; ++it ){
	temp_tHitVec.push_back( new TimedMdtCalibHit(**it) ) ;
	setTimeHit(i, rhs.timeHit(i) ) ;
	setSigmaTimeHit(i, rhs.sigmaTimeHit(i) );      
	++i;
      }
      m_tHitVec = temp_tHitVec ;
      m_time = rhs.time();
      m_sigma_time = rhs.sigmaTime();
      m_radiiHit = rhs.m_radiiHit;
      m_mlHit = rhs.m_mlHit;
      if(m_segment) delete m_segment;
      m_segment = new MuonCalibSegment( *rhs.segment() );
      if(m_segment_original) delete m_segment_original;
      m_segment_original = new MuonCalibSegment( *rhs.originalSegment() );
    }
    
    return (*this) ;
  }
  
  void TimedSegment::update(){
    MuonCalibSegment* newSeg(m_segment_original);
        
    MuonCalibSegment::MdtHitCit it     = newSeg->mdtCloseHitsBegin();
    MuonCalibSegment::MdtHitCit it_end = newSeg->mdtCloseHitsEnd();
    unsigned int i=0;
    for( ; it!=it_end;++it){
      double radius = (*it)->driftRadius();
      double sigma_r = (*it)->sigmaDriftRadius();
      (*it)->setDriftRadius( radius + radiusHit(i), sigma_r );
      ++i;
    }
    m_segment = newSeg;

  }

}
