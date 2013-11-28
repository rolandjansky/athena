/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTFITTINGTOOL_H
#define MUONSEGMENTFITTINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "MuonRecToolInterfaces/IMuonSegmentFittingTool.h"
#include "TrkGeometry/MagneticFieldProperties.h"

#include <vector>

namespace Trk {
  class PlaneSurface;
  class IPropagator;
  class ITrackFitter;
  class LocalDirection;
  class MeasurementBase;
}


namespace Muon {
  class IMuonTrackCleaner;
  class MuonIdHelperTool;
  class MuonSegment;
}


class MsgStream;

namespace Muon {

  /** 
      @class MuonSegmentFittingTool

      Implementation of an IMuonSegmentFittingTool. 

  */
  class MuonSegmentFittingTool :  virtual public IMuonSegmentFittingTool, public AthAlgTool   {
  public:
    /** default AlgTool constructor */
    MuonSegmentFittingTool(const std::string&,const std::string&,const IInterface*);
    
    /** destructor */
    virtual ~MuonSegmentFittingTool ();
    
    /** initialize method, method taken from bass-class AlgTool */
    virtual StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    virtual StatusCode finalize();

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
    ToolHandle<MuonIdHelperTool>              m_idHelperTool;   //<! Id helper tool

    bool m_updatePrecisionCoordinate; //<! flag to select update of precision coordinate in fit
  };
  
}
#endif 

