/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_TIMINGTOOL_H
#define RPC_TIMINGTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkSpaceTimePoint/SpaceTimePoint.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "TrkSegment/SegmentCollection.h"

#include "TrkToolInterfaces/ITrackTimingTool.h"

//class AtlasDetectorID;            //!< to identify measurements

/** @class RPC_TimingTool
    
    RPC_TimingTool obtains RPC time measurements from tracks and segments.

    @author MCP projects
*/

#define FLAG -88888.0

namespace Muon{

  class RpcPrepData;
  class RpcClusterOnTrack;
  //  class MuonEDMHelperTool;
  
  class RPC_TimingTool : virtual public Trk::ITrackTimingTool, public AthAlgTool{
  public:
    RPC_TimingTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor **/
    virtual ~RPC_TimingTool();

    /** standard initialization method **/
    virtual StatusCode initialize();
    
    /** standard finalization method **/
    virtual StatusCode finalize();

    /** Returns a list of SpaceTimePoints from RPC hits on a track.
    */
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const Trk::Track& trk,
                 const Trk::SegmentCollection* = NULL) const;

    /** unsupported interface ? */
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const Trk::TrackParticleBase& trk) const;

    /** Returns a list of SpaceTimePoints corresponding to the segments given */
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(std::vector<const Trk::Segment*> segments) const;

  private:

    //    bool matchSegmentTrk(const Trk::Track& origTrk, const Trk::Segment*& seg) const ;

    //double getError(const Muon::RpcPrepData& seg)   const ; 
    double getError(const Muon::RpcClusterOnTrack*) const ; 

    //const AtlasDetectorID* m_idHelper;
    const DataHandle<RpcIdHelper> m_idHelper;
    //    ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;

  };
}

#endif
