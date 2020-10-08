/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSIMPLEHOLESEARCH_H
#define MUONCALIBSIMPLEHOLESEARCH_H

#include "MuonCalibExtraUtils/MuonCalibSimpleGeometry.h"
#include "MuonCalibExtraUtils/MuonCalibSLPropagator.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CxxUtils/checker_macros.h"

#include <vector>

namespace MuonCalib {


  class ATLAS_NOT_THREAD_SAFE MuonCalibSimpleHoleSearch {
  public:
    struct Result {
      Result() {}
      Result( MuonFixedId id, const Amg::Vector3D& pos ) : stationLayerId(id),intersectPosition(pos) {}
      MuonFixedId         stationLayerId;
      Amg::Vector3D intersectPosition;
    };
    typedef std::vector<Result> ResultVec;
    typedef ResultVec::iterator ResultIt;
  public:
    MuonCalibSimpleHoleSearch() { init(); }
    
    ResultVec intersectsWithGeometry( const Amg::Vector3D& parPos, const Amg::Vector3D& parDir, double tolerance = 1e9 );
    


  private:
    /** search for intersect with the give surface */
    void searchForSurfaceIntersect( const Amg::Vector3D& parPos, const Amg::Vector3D& parDir, 
				    const MuonCalibSurface& surf, const MuonFixedId& stLayerId,
				    ResultVec& intersects, double tolerance = 1e9 ) const;

    /** init static members */
    void init();
    
    static MuonCalibSimpleGeometry m_geometry; 
    static MuonCalibSLPropagator   m_propagator; 
    static MuonFixedIdManipulator  m_idManipulator; 
    typedef std::vector< std::pair<const MuonCalibSurface*,MuonFixedId> > SurfaceIdVec;
    typedef SurfaceIdVec::iterator SurfaceIdIt;
    static  SurfaceIdVec m_surfaceIdList;
  };

}



#endif
