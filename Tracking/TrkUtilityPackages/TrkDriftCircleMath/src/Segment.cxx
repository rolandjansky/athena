/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/SegmentCandidate.h"

#include "TrkDriftCircleMath/DCOnTrack.h"


namespace TrkDriftCircleMath {


std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::Segment& seg )
{
  os << seg.line() << " chi2 " << seg.chi2() << " ndof " << seg.ndof() << " t0 shift " << seg.t0Shift() << std::endl 
     << "   hot " << seg.hitsOnTrack()
     << " deltas " << seg.deltas() 
     << " hoo " << seg. hitsOutOfTime() 
     << " cl " << seg.closeHits()
     << " ml1 " << seg.hitsMl1() << " ml2 " << seg.hitsMl2() 
     << " cls " << seg.clusterLayers() 
     << " empty " << seg.emptyTubes().size()  
     << " cross ml1 " << seg.crossedTubesMl1() << " ml2 " << seg.crossedTubesMl2() << std::endl;

  //   TrkDriftCircleMath::ResidualWithLine resLine( seg.line() );  

  TrkDriftCircleMath::DCOnTrackCit it = seg.dcs().begin();
  TrkDriftCircleMath::DCOnTrackCit it_end = seg.dcs().end();
  
  for( ; it!=it_end; ++it ){
    os << *it << std::endl;
  }
  return os;
}

MsgStream& operator<< (MsgStream & os, const TrkDriftCircleMath::Segment& seg)
{
  os << seg.line() << " chi2 " << seg.chi2() << " ndof " << seg.ndof() << " t0 shift " << seg.t0Shift() 
     << "\n   hot " << seg.hitsOnTrack()
     << " deltas " << seg.deltas() 
     << " hoo " << seg. hitsOutOfTime() 
     << " cl " << seg.closeHits()
     << " ml1 " << seg.hitsMl1() << " ml2 " << seg.hitsMl2() 
     << " cls " << seg.clusterLayers() 
     << " empty " << seg.emptyTubes().size()  
     << " cross ml1 " << seg.crossedTubesMl1() << " ml2 " << seg.crossedTubesMl2() << endmsg;
    
  TrkDriftCircleMath::DCOnTrackCit it       = seg.dcs().begin();
  TrkDriftCircleMath::DCOnTrackCit it_end   = seg.dcs().end();
    
  for (; it != it_end; ++it) {
    os << *it << endmsg;
  }
    
  return os;
}


} // namespace TrkDriftCircleMath
