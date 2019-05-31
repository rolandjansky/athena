/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
TrkAmbiguitySolver Algorithm 
***************************************************************************/


#ifndef TRKAMBIGUITYSOLVER_H
#define TRKAMBIGUITYSOLVER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>

namespace Trk 
{
  /**Algorithm does ambiguity processing
     This algorithm uses the TrkAmbiguityProcessorTool AlgTool to resolve ambiguities in the passed tracks.
     
     Actually at the moment, this functionality is disabled - all TrkAmbiguitySolver does is to load Trk::Tracks (from
     a location passed by jobOptions) and save them into (by default) "Tracks"*/

  class TrkAmbiguitySolver : public AthAlgorithm
    {

    public:
      TrkAmbiguitySolver(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~TrkAmbiguitySolver(void) = default;
      
      virtual StatusCode initialize(void) override;
      virtual StatusCode execute(void) override;
      virtual StatusCode finalize(void) override;

    private:
      //!<where to find tracks (set in jobOptions and can be multiple collections)  
      SG::ReadHandleKeyArray<TrackCollection> m_tracksLocation;
      SG::WriteHandleKey<TrackCollection> m_resolvedTracksLocation{this, "TrackOutput", "Tracks", "where to save the resolved tracks"};
        
      /** decides whether ambi processing actually occurs 
	  (if not, the tracks are just resaved). 
	  Default=false.*/
      BooleanProperty m_resolveTracks{this, "ResolveTracks", true};

      
      /**Number of tracks input. Useful for debugging*/
      long int m_trackInCount{0};
      /**Number of tracks passing Ambi solving. Useful for debugging*/
      long int m_trackOutCount{0};

      /** responsible for actual amiguity processing*/                  
      ToolHandle<ITrackAmbiguityProcessorTool> m_ambiTool{this, "AmbiguityProcessor", "Trk::SimpleAmbiguityProcessorTool/TrkAmbiguityProcessor"};

      /** get tracks */
      StatusCode getTracks(ConstDataVector<TrackCollection>& oldTracks);

      /** Save the processed tracks. If ambiguity process has not been called then they will 
	  still be the original converted tracks */
      
      /** Eventually this will use the TrkAmbiguityProcessorTool to resolve the track ambiguities.
	  At the moment it just saves a new vector of converted tracks. */
      void resolveTracks(std::unique_ptr<TrackCollection>& tracks, ConstDataVector<TrackCollection>& oldTracks);
};

}

#endif
