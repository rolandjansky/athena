/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef INDETCOSMICSCORINGTOOL_H
#define INDETCOSMICSCORINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include <vector>
#include <string>

namespace Trk {
  class IExtrapolator;
  class Track;
  class TrackSummary;
}


namespace InDet {


/**Concrete implementation of the ITrackScoringTool pABC*/
class InDetCosmicScoringTool : virtual public Trk::ITrackScoringTool, public AthAlgTool
{

public:
  InDetCosmicScoringTool(const std::string&,const std::string&,const IInterface*);
  virtual ~InDetCosmicScoringTool ();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize  () override;
  /** create a score based on how good the passed track is*/
  virtual
  Trk::TrackScore score( const Trk::Track& track, const bool suppressHoleSearch ) const override;
  
  /** create a score based on how good the passed TrackSummary is*/
  virtual
  Trk::TrackScore simpleScore( const Trk::Track& track, const Trk::TrackSummary& trackSum ) const override;
  
 private:
  
  /**\todo make this const, once createSummary method is const*/
  ToolHandle<Trk::ITrackSummaryTool> m_trkSummaryTool;

  int m_nWeightedClustersMin; 
  int m_minTRTHits;

};


}
#endif 
