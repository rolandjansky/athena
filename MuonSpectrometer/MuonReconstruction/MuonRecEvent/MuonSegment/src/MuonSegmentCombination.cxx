/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkSegment/Segment.h"

namespace Muon {

  MuonSegmentCombination::MuonSegmentCombination()
  {
    nGood[0]=4; nGood[1]=4;
  }
    
  MuonSegmentCombination::~MuonSegmentCombination()
  {
    clear();
  }

  MuonSegmentCombination::MuonSegmentCombination( const MuonSegmentCombination& segc )
  {
    copy( segc );
  }

  MuonSegmentCombination& MuonSegmentCombination::operator=( const MuonSegmentCombination& segc )
  {
    if( this != &segc ){
      clear();
      copy( segc );
    }
    return *this;
  }

  void MuonSegmentCombination::copy( const MuonSegmentCombination& segc )
  {
    nGood[0]=segc.getNGoodCscLayers(0);
    nGood[1]=segc.getNGoodCscLayers(1);
    unsigned int nst( segc.numberOfStations() );
    for( unsigned int i=0;i<nst;++i ){
      SegmentVec::const_iterator vit = segc.stationSegments( i )->begin();
      SegmentVec::const_iterator vit_end = segc.stationSegments( i )->end();
      SegmentVec* newsegs = new SegmentVec();
      for( ; vit!=vit_end; ++vit ) newsegs->push_back( (*vit)->clone() );
      m_segmentsPerStation.push_back(newsegs);
    }
  }

  void MuonSegmentCombination::clear()
  {
    SegmentVecVec::iterator it = m_segmentsPerStation.begin();
    SegmentVecVec::iterator it_end = m_segmentsPerStation.end();

    for( ; it!=it_end; ++it ){
      SegmentVec::iterator vit = (*it)->begin();
      SegmentVec::iterator vit_end = (*it)->end();
      for( ; vit!=vit_end; ++vit ) delete *vit;
      delete *it;
    }
    m_segmentsPerStation.clear();
  } 

}
