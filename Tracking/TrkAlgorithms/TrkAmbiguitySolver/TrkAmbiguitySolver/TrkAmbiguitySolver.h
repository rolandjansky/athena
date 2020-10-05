/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
TrkAmbiguitySolver Algorithm 
***************************************************************************/


#ifndef TRKAMBIGUITYSOLVER_H
#define TRKAMBIGUITYSOLVER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include <string>

namespace Trk 
{
  class ITrackAmbiguityProcessorTool;
  class ITrackAmbiguityScoreProcessorTool;

  /**Algorithm does ambiguity processing
     This algorithm uses the TrkAmbiguityProcessorTool AlgTool to resolve ambiguities in the passed tracks.
     
     Actually at the moment, this functionality is disabled - all TrkAmbiguitySolver does is to load Trk::Tracks (from
     a location passed by jobOptions) and save them into (by default) "Tracks"*/

  class TrkAmbiguitySolver : public AthReentrantAlgorithm
    {

    public:
      TrkAmbiguitySolver(const std::string& name, ISvcLocator* pSvcLocator);
      ~TrkAmbiguitySolver(void);
      
      StatusCode	      initialize(void) override;
      StatusCode	      execute(const EventContext& ctx) const override;
      StatusCode	      finalize(void) override;

    private:
      //!<where to find tracks (set in jobOptions and can be multiple collections
      SG::ReadHandleKey<TracksScores> m_scoredTracksKey;
      SG::WriteHandleKey<TrackCollection> m_resolvedTracksKey;//!<where to save the resolved tracks
      ToolHandle<ITrackAmbiguityProcessorTool> m_ambiTool;

      /**Number of tracks input. Useful for debugging*/
       mutable std::atomic<long int>          m_trackInCount;
      /**Number of tracks passing Ambi solving. Useful for debugging*/
       mutable std::atomic<long int>          m_trackOutCount;
      
    };

}

#endif 




