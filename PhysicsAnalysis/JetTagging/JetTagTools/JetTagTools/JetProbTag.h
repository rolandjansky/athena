/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_JETPROBTAG_H
#define JETTAGTOOLS_JETPROBTAG_H

/******************************************************
  @class JetProbTag
  Implementation of the ALEPH tagger

  @author devivie@lal.in2p3.fr
  @author vacavant@cppm.in2p3.fr
  @author zaidan@cppm.in2p3.fr
  @author lapoire@cppm.in2p3.fr
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandle.h"
#include <vector>
#include "JetTagTools/ITagTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

class StoreGateSvc;
//namespace xAOD  { class TrackParticle; class TrackParticleContainer; }
namespace Reco { class ITrackToVertex; }
namespace Trk  {
  class VxCandidate;
  class ITrackToVertexIPEstimator;
}
//class Jet;

namespace Analysis
{

  class GradedTrack;
  class TrackGradePartition;
  class TrackSelector;
  class TrackGradePartition;
  class SVForIPTool;
  class ITrackGradeFactory;
  class CalibrationBroker;
  class HistoHelperRoot;

  class JetProbTag : public AthAlgTool , virtual public ITagTool
  {
    public:
      JetProbTag(const std::string&,const std::string&,const IInterface*);
      
    /**
    Implementations of the methods defined in the abstract base class
     */
      virtual ~JetProbTag();
      StatusCode initialize();
      StatusCode finalize();
      
      /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
      be part of the JetTag IParticle interface implementation. The trouble with 
      ElementLink and persistency has to be solved for that. Revisit ... */
      void setOrigin(const Trk::VxCandidate* priVtx);
      
      void tagJet(xAOD::Jet& jetToTag);
    
    private:      

      /** This switch is needed to indicate what to do. The algorithm can be run to produce
      reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
      (m_runModus=1) where already made reference histograms are read.*/ 
      std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)
 
      std::string  m_inputType;
    
      /** TrackToVertex tool */
      ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;
      
      /** Track selection cuts for JetProbTagging */
      ToolHandle< TrackSelector > m_trackSelectorTool;
      
      std::string m_originalTPCollectionName;
      SG::ReadHandle<xAOD::TrackParticleContainer> m_originalTPCollection;

      bool m_writeInfoBase; // writes a basic info for each tagger with Pb, Pu (IPInfoBase)
      std::string m_infoPlusName; // key to store the IPInfoPlus
      std::vector<std::string> m_jetCollectionList;
      std::vector<std::string> m_jetWithInfoPlus;

      /** The jet of TrackParticles to be tagged. */
      std::vector<GradedTrack> m_tracksInJet;
      
      /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
      // this pointer does not need to be deleted in the destructor (because it
      // points to something in storegate)
      const Trk::VxCandidate* m_priVtx = 0;

      /** specify the tag type (1D or 2D) */
      std::string m_impactParameterView;
      
      /** track classification. */
      std::vector<std::string> m_trackGradePartitionsDefinition;
      std::vector<std::string> m_trackGradesToIgnore;
      std::vector<TrackGradePartition*> m_trackGradePartitions;

      bool m_RejectBadTracks;
      bool m_SignWithSvx;
      bool m_negIP;
      bool m_posIP;
      bool m_flipIP;

      /** parameters for resolution function: */
      double m_getTrackProb(double significance, std::string trkName);
      bool m_useHistograms;
      double m_getTrackProbFromHistograms(double significance, std::string trkName);
      std::vector<double> m_fitParams;
      std::string m_fitExpression;
      double m_getTrackProbFromFit(double significance, std::string trkName);

      double m_factorial(int n);
      
      /** pointer to the calibration tool: */
      ToolHandle<CalibrationBroker> m_calibrationTool;

      void finalizeHistos();

      /** use Tool for getting the secondary vertex information */
      std::string m_secVxFinderNameForV0Removal;
      std::string m_secVxFinderNameForIPSign;
      ToolHandle< SVForIPTool > m_SVForIPTool;

      /** ToolHandle for the ITrackGradeFactory tool */
      ToolHandle< ITrackGradeFactory > m_trackGradeFactory;

      /** for reference mode: */
      HistoHelperRoot* m_histoHelper;
      bool m_checkOverflows;
      double m_jetPtMinRef;
      double m_isolationDeltaR;
      bool m_useTrueFlavour; // if true, make uses of truth and cuts below
      std::string m_referenceType;
      std::string m_truthMatchingName;
      double m_purificationDeltaR;

      /** Tool for the estimation of the IPs to the Vertex */
      ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
      /** option to unbias IP estimation (remove track from vertex) */
      bool m_unbiasIPEstimation;

    double m_iAccuracy;
    double m_fullIntegral;
      bool m_printWarning; //// print warning one time

  }; // End class
  inline void JetProbTag::setOrigin(const Trk::VxCandidate* priVtx) { m_priVtx=priVtx; }
} // End namespace 

#endif
