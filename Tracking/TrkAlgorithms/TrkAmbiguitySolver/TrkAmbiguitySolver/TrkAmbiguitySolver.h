/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
TrkAmbiguitySolver Algorithm 
***************************************************************************/


#ifndef TRKAMBIGUITYSOLVER_H
#define TRKAMBIGUITYSOLVER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include <string>

namespace Trk 
{
  class ITrackAmbiguityProcessorTool;

  /**Algorithm does ambiguity processing
     This algorithm uses the TrkAmbiguityProcessorTool AlgTool to resolve ambiguities in the passed tracks.
     
     Actually at the moment, this functionality is disabled - all TrkAmbiguitySolver does is to load Trk::Tracks (from
     a location passed by jobOptions) and save them into (by default) "Tracks"*/

  class TrkAmbiguitySolver : public AthAlgorithm
    {

    public:
      TrkAmbiguitySolver(const std::string& name, ISvcLocator* pSvcLocator);
      ~TrkAmbiguitySolver(void);
      
      StatusCode	      initialize(void);
      StatusCode	      execute(void);
      StatusCode	      finalize(void);

    private:
      //!<where to find tracks (set in jobOptions and can be multiple collections)  
      std::vector<std::string>   m_tracksLocation;
      std::string       m_resolvedTracksLocation;//!<where to save the resolved tracks
      std::string       m_ambiProcessorName;
      std::string       m_ambiProcessorInstance;
        
      /** decides whether ambi processing actually occurs 
	  (if not, the tracks are just resaved). 
	  Default=false.*/
      bool              m_resolveTracks; 
      ConstDataVector<TrackCollection>   m_oldTracks;
      TrackCollection * m_tracks;
      
      /**Number of tracks input. Useful for debugging*/
      long int          m_trackInCount; 
      /**Number of tracks passing Ambi solving. Useful for debugging*/
      long int          m_trackOutCount; 
      
      /** responsible for actual amiguity processing*/                  
      ToolHandle<ITrackAmbiguityProcessorTool> m_ambiTool;

      /** get tracks */
      StatusCode        getTracks();

      /** Save the processed tracks. If ambiguity process has not been called then they will 
	  still be the original converted tracks */
      void saveTracks() const;
      
      /** Eventually this will use the TrkAmbiguityProcessorTool to resolve the track ambiguities.
	  At the moment it just saves a new vector of converted tracks. */
      void resolveTracks();
};

}

#endif 




