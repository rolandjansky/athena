/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2BEAMSPOT_IT2VERTEXBEAMSPOTTOOL_H
#define TRIGT2BEAMSPOT_IT2VERTEXBEAMSPOTTOOL_H
/// Local tools
#include "../src/T2Track.h"
#include "../src/T2Vertex.h"
//Athena tools
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "TrkTrack/TrackCollection.h"
#include <string>
#include <vector>
#include "GaudiKernel/IAlgTool.h"

class TrigVertexCollection;

/// Event statistics
// When this changes, adjust T2VertexBeamSpotMonitoring.py: EventStatistics histogram
enum Statistics
{ 
   allInput      = 0,
   hasTE         = 1,
   hasROI        = 2,
   hasTracks     = 3,
   hasSeedTrack  = 4,
   enoughTracks  = 5,
   hasCluster    = 6,
   hasVertex     = 7,
   hasGoodVertex = 8,
   numStatistics /// Leave this entry at the end
};



namespace PESA {

   class IT2VertexBeamSpotTool : virtual public IAlgTool {
      public:
         DeclareInterfaceID (IT2VertexBeamSpotTool, 1, 0);

         virtual bool isGoodTrack     ( const T2Track & track  ) const = 0;


         virtual bool isGoodVertex    ( const T2Vertex& vertex ) const = 0;


         virtual  bool isGoodVertexBCID( const T2Vertex& vertex ) const =0;


         virtual  bool isHighPTTrack( unsigned &nTracks ) const =0;


         virtual  bool notEnoughTracks(unsigned &nTracks) const =0;


         virtual void selectTracks( const TrackCollection*  trackCollection,
               ConstDataVector<TrackCollection>& mySelectedTrackCollection, std::vector<unsigned> &trackCounter ) = 0;

         virtual void reconstructVertices( ConstDataVector<TrackCollection>& mySelectedTrackCollection,
               TrigVertexCollection& myVertexCollection,
               DataVector< TrigVertexCollection >&  mySplitVertexCollections) = 0;

         virtual void reconstructSplitVertices( ConstDataVector<TrackCollection>& mySelectedTrackCollection,
               DataVector< TrigVertexCollection >& mySplitVertexCollections ) = 0;


         virtual void resetMonitoredVariables() = 0;

         //Monitoring variables, need to be reset every event in resetMonitored variables
         std::vector<bool>     m_eventStageFlag;
         std::vector<unsigned> m_eventStage;

         //Event info variables 
         unsigned int m_EventID = 0;

         //This might not have to be a member of the tool at all
         //But need this in the tool as it is used in both tool and the alg
         virtual bool eventStage( Statistics stage ) = 0;

         //Keep the track of how many vertices passed the selection
         unsigned int m_NvtxPass;

         std::string m_vertexCollName;


         bool m_passNpvTrigCuts;

         std::vector<T2Vertex> m_vertex;


   };

} // end namespace

#endif
