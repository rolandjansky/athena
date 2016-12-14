/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONTRACKSCORINGTOOL_H
#define MUONTRACKSCORINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include <vector>

namespace Trk {
  class Track;
  class ITrackSummaryTool;
  class TrackSummary;
}

class MsgStream;

namespace Muon {

  class MuonEDMPrinterTool;


  /**Concrete implementation of the ITrackScoringTool pABC*/
  class MuonTrackScoringTool : virtual public Trk::ITrackScoringTool, public AthAlgTool
  {
    
  public:
    MuonTrackScoringTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonTrackScoringTool ();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize  () override;
    /** create a score based on how good the passed track is*/
    virtual
    Trk::TrackScore score( const Trk::Track& track, const bool suppressHoleSearch ) const override;
    
    /** create a score based on how good the passed TrackSummary is*/
    virtual
    Trk::TrackScore simpleScore( const Trk::Track& track, const Trk::TrackSummary& trackSummary ) const override;
    
  private:
    
    /**\todo make this const, once createSummary method is const*/
    ToolHandle<Trk::ITrackSummaryTool> m_trkSummaryTool;
    ToolHandle<MuonEDMPrinterTool>     m_printer;
    
    /**holds the scores assigned to each Trk::SummaryType from the track's Trk::TrackSummary*/
    std::vector<Trk::TrackScore> m_summaryTypeScore;

  };
  

}
#endif 
