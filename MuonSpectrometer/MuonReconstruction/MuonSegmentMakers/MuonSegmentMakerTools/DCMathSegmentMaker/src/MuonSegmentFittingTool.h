/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTFITTINGTOOL_H
#define MUONSEGMENTFITTINGTOOL_H

#include "MuonRecToolInterfaces/IMuonSegmentFittingTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IPropagator.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include <string>
#include <vector>

namespace Trk {
  class PlaneSurface;
  class LocalDirection;
  class MeasurementBase;
}

namespace Muon {
  class MuonSegment;
}

namespace Muon {

  /** 
      @class MuonSegmentFittingTool

      Implementation of an IMuonSegmentFittingTool. 

  */
  class MuonSegmentFittingTool :  virtual public IMuonSegmentFittingTool, public AthAlgTool   {
  public:
    MuonSegmentFittingTool(const std::string&,const std::string&,const IInterface*);
    
    virtual ~MuonSegmentFittingTool()=default;
    
    virtual StatusCode initialize();

    /** fit segment parameters + hits producing a track */
    Trk::Track* fit( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir, const Trk::PlaneSurface& surf, 
		     const std::vector<const Trk::MeasurementBase*>& rioVec ) const;

    /** fit segment parameters + hits producing a track */
    Trk::Track* fit( const MuonSegment& segment ) const;

    /** update the parameters of the segment using the track information */
    void updateSegmentParameters( const Trk::Track& track, const Trk::PlaneSurface& surf, Amg::Vector2D& segLocPos, 
				  Trk::LocalDirection& segLocDir, Amg::MatrixX& locerr ) const;
    

    ToolHandle<Trk::IPropagator>              m_slPropagator;           //<! propagator, always use straightline
    Trk::MagneticFieldProperties              m_magFieldProperties;     //<! pmagnetic field properties
    ToolHandle<Trk::ITrackFitter>             m_slTrackFitter;          //<! fitter, always use straightline
    ToolHandle<Trk::ITrackFitter>             m_curvedTrackFitter;      //<! fitter, curved tracks
    ToolHandle<Muon::IMuonTrackCleaner>       m_trackCleaner;

    bool m_updatePrecisionCoordinate; //<! flag to select update of precision coordinate in fit
  };
  
}
#endif 

