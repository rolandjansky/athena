/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LocalSegmentResolver.h
//   Header file for class MdtSegment
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_LOCALSEGMENTRESOLVER_H
#define MUONCALIB_LOCALSEGMENTRESOLVER_H


// stl includes
#include <vector>

#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MdtCalibHitBase;

namespace MuonCalib {

/**@class LocalSegmentResolver
   Class calculates the local position and direction of the segment using the hits on track.

   @author Niels.Van.Eldik@cern.ch
*/

  class LocalSegmentResolver {
  public:
    /** constructor */
    LocalSegmentResolver();

    /** resolve local position and direction of the track segment */
    bool resolve(MuonCalibSegment* seg) const;

    /** set print level */
    void setPrintLevel(int level) { m_printLevel = level; }

  private:
    typedef MuonCalibSegment::MdtHitVec HitVec;
    typedef std::pair<Amg::Vector3D,Amg::Vector3D > Line;
    typedef std::vector<Line>                 LineVec;
  private:
    LineVec getLines( const MdtCalibHitBase& firstHit, const MdtCalibHitBase& lastHit ) const;

    int bestLine( const HitVec& hits, const LineVec& localTracks ) const;
  
    /** print level */
    int m_printLevel;
  };

}

#endif
