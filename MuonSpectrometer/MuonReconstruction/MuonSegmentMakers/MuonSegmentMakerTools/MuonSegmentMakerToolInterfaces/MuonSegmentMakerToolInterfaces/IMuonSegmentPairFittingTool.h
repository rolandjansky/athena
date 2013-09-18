/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTPAITFITTER_H
#define MUON_IMUONSEGMENTPAITFITTER_H

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"


static const InterfaceID IID_IMuonSegmentPairFittingTool("Muon::IMuonSegmentPairFittingTool",1,0);


namespace Muon {
  
  class MuonSegment;

  /**
     @brief tool to fit pairs of segments

  */
  class IMuonSegmentPairFittingTool : virtual public IAlgTool {
  public:
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_IMuonSegmentPairFittingTool; }


    virtual void trackParameters( const Muon::MuonSegment& seg ,double &theta,double &curvature,int &imeth ) = 0;
    virtual void extrapolateSegment( const Muon::MuonSegment& seg , const Muon::MuonSegment& sege, Amg::VectorX& Res, Amg::VectorX& Pull ) = 0;

    virtual void fit2Segments( const Muon::MuonSegment& seg , const Muon::MuonSegment& sege, Amg::VectorX& Res, Amg::VectorX& Pull ) = 0;
    virtual void fit2SegmentsC( const Muon::MuonSegment& seg , const Muon::MuonSegment& sege, Amg::VectorX& Res, Amg::VectorX& Pull ) = 0;
    virtual void fit2SegmentsSL( const Muon::MuonSegment& seg , const Muon::MuonSegment& sege, Amg::VectorX& Res, Amg::VectorX& Pull ) = 0;

  };

}

#endif
