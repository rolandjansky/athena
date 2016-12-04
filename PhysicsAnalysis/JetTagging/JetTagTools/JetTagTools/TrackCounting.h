/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_TRACKCOUNTING_H
#define JETTAGTOOLS_TRACKCOUNTING_H
 
/******************************************************
    @class TrackCounting
    
    b-tagging AlgTool based on counting tracks with an 
    impact parameter significence exceeding a given
    threshold

    @author Thomas Goepfert <thomas.goepfert@cern.ch>
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"

#include <vector>

namespace Reco { class ITrackToVertex; }
namespace Trk  { class VxCandidate; class ITrackToVertexIPEstimator; }
//class Jet;

namespace Analysis { 

  class TrackCountingInfo;
  class TrackSelector;
  class TrackGrade;
  class TrackGradePartition;
  class ITrackGradeFactory;
  class GradedTrack;
  class SVForIPTool;
          
  class TrackCounting : public AthAlgTool , virtual public ITagTool
  {
    public:
      TrackCounting(const std::string&,const std::string&,const IInterface*);
      
      /** Implementations of the methods defined in the abstract base class */
      virtual ~TrackCounting();
      StatusCode initialize();
      StatusCode finalize();
      void finalizeHistos(){};      

      /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
	  be part of the JetTag IParticle interface implementation. The trouble with 
	  ElementLink and persistency has to be solved for that. Revisit ... */
      void setOrigin(const Trk::VxCandidate* priVtx);
      
      /** called by BJetBuilder */
      void tagJet(xAOD::Jet& jetToTag);    

    private:      

      /** TrackToVertex tool */
      ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;
      
      /** Track selection cuts for TrackCounting */
      ToolHandle< TrackSelector > m_trackSelectorTool;
     
      /** The jet of TrackParticles to be tagged. */
      std::vector<GradedTrack> m_tracksInJet;
       
      /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
      // this pointer does not need to be deleted in the destructor (because it
      // points to something in storegate)
      const Trk::VxCandidate* m_priVtx = 0;

      /** track classification. */
      std::vector<std::string> m_trackGradePartitionsDefinition;
      std::vector<TrackGradePartition*> m_trackGradePartitions;

      /** possibility to exclude Bad Tracks found with SVTagger and 
	  possibility to use the sign of the impact parameter with SVTagger/JetFitter */
      bool m_RejectBadTracks;
      bool m_SignWithSvx;

      bool m_flipIP;

      /** write tagging info to info class */
      bool m_writeInfo;

      std::string m_truthMatchingName; // name of truthMatchingTool instance to get TruthInfo
      double m_purificationDeltaR;     // skip light jets with heavy flavor in this cone

      std::vector<std::string> m_jetCollectionList;

      //GP: use Tool for getting the secondary vertex information
      std::string m_secVxFinderNameForV0Removal;
      std::string m_secVxFinderNameForIPSign;
      ToolHandle< SVForIPTool > m_SVForIPTool;
      
      /** ToolHandle for the ITrackGradeFactory tool */
      ToolHandle< ITrackGradeFactory > m_trackGradeFactory;

      /** Tool for the estimation of the IPs to the Vertex */
      ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
      /** option to unbias IP estimation (remove track from vertex) */
      bool m_unbiasIPEstimation;
      
      /** sort algorithm for vectors */
      template <class T> static bool m_greater(T a, T b){
	return a > b;
      }

  }; // End class
  inline void TrackCounting::setOrigin(const Trk::VxCandidate* priVtx) { m_priVtx=priVtx; }
  
} // End namespace 

#endif
