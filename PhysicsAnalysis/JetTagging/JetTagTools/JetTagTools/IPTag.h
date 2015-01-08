/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_IPTAG_H
#define JETTAGTOOLS_IPTAG_H

/******************************************************
    @class IPTag
    b-jet tagger based on 2D or 3D impact parameter
    @author CPPM Marseille
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include <vector>

//namespace xAOD  { class TrackParticle; class TrackParticleContainer; }
namespace Reco { class ITrackToVertex; }
namespace Trk  { class ITrackToVertexIPEstimator; }
//class Jet;

namespace Analysis { 

  class IPInfo;
  class NewLikelihoodTool;
  class HistoHelperRoot;
  class TrackSelector;
  class TrackGrade;
  class GradedTrack;
  class TrackGradePartition;
  class SVForIPTool;
  class ITrackGradeFactory;
  

  class IPTag : public AthAlgTool , virtual public ITagTool {
   
  public:
    IPTag(const std::string&,const std::string&,const IInterface*);
      
    /**
       Implementations of the methods defined in the abstract base class
    */
    virtual ~IPTag();
    StatusCode initialize();
    StatusCode finalize();
      
    /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
	be part of the JetTag IParticle interface implementation. The trouble with 
	ElementLink and persistency has to be solved for that. Revisit ... */
    void setOrigin(const xAOD::Vertex* priVtx);
      
    StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging * BTag);    

    void finalizeHistos() {};
    
  private:      

    /** This switch is needed to indicate what to do. The algorithm can be run to produce
	reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
	(m_runModus=1) where already made reference histograms are read.*/ 
    std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)

    /** base name string for persistification in xaod */
    std::string m_xAODBaseName;
      
    /** Histogram Helper Class */
    HistoHelperRoot* m_histoHelper;

    /** TrackToVertex tool */
    ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;
      
    /** Track selection cuts for IPTagging */
    ToolHandle< TrackSelector > m_trackSelectorTool;
      
    /** Pointer to the likelihood tool. */
    ToolHandle< NewLikelihoodTool > m_likelihoodTool;
 
    /** The jet of TrackParticles to be tagged. */
    std::vector<GradedTrack> m_tracksInJet;
      
    /** Name of the track-to-jet association in the BTagging object */
    std::string m_trackAssociationName;

    /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
    // this pointer does not need to be deleted in the destructor (because it
    // points to something in storegate)
    const xAOD::Vertex* m_priVtx;

    /** List of the variables to be used in the likelihood */
    std::vector<std::string> m_useVariables;
     
    /** specify the tag type (1D or 2D) */
    std::string m_impactParameterView;

    /** track classification. */
    std::vector<std::string> m_trackGradePartitionsDefinition;
    std::vector<TrackGradePartition*> m_trackGradePartitions;

    bool m_flipIP;   // reverse impact parameter signs for negative tag calibration method
    bool m_flipZIP;  // reverse Z impact parameter signs for negative tag calibration method
    bool m_usePosIP; // use tracks with positive impact parameter for tagging
    bool m_useNegIP; // use tracks with negative impact parameter for tagging
    bool m_useZIPSignForPosNeg; // use Z impact parameter sign as well to select Pos and Neg tracks
    bool m_use2DSignForIP3D; // force to use 2D IP sign even for IP3D IP
    bool m_useD0SignForZ0; // force to use transverse IP sign for Z impact parameter for IP3D

    /** JBdV */ 
    bool m_RejectBadTracks;
    bool m_SignWithSvx;

    /** information to persistify: */
    std::string m_originalTPCollectionName;
    const xAOD::TrackParticleContainer* m_originalTPCollection;

    bool m_writeInfoBase; // writes a basic info for each tagger with Pb, Pu (IPInfoBase)
    bool m_writeInfoPlus; // writes a detailed info 
    std::string m_infoPlusName; // key to store the IPInfoPlus

    /** calculate individual track contribution to the three likelihoods: */
    void trackWeight(std::string jetAuthor, TrackGrade grade, double sa0, double sz0,
                     double & twb, double & twu, double & twc);

    /** for reference mode: */
    std::string m_referenceType; // label to use for reference mode
    std::string m_truthMatchingName; // name of truthMatchingTool instance to get TruthInfo
    bool m_checkOverflows; // if true put the overflows in the first/last bins
    double m_purificationDeltaR; // skip light jets with heavy flavor in this cone
    double m_jetPtMinRef; // min cut on jet pT for reference

    std::vector<std::string> m_jetCollectionList, m_jetWithInfoPlus; // 

    std::vector<std::string> m_hypotheses; // hypotheses: b | u

    bool m_doForcedCalib;
    std::string m_ForcedCalibName;

    // for debugging:
    int m_nbjet;
    int m_ncjet;
    int m_nljet;

    bool m_useCHypo;

    //GP: use Tool for getting the secondary vertex information
    std::string m_secVxFinderNameForV0Removal;
    std::string m_secVxFinderNameForIPSign;
    std::string m_secVxFinderName;
    ToolHandle< SVForIPTool > m_SVForIPTool;

    /** ToolHandle for the ITrackGradeFactory tool */
    ToolHandle< ITrackGradeFactory > m_trackGradeFactory;

    /** GP: Tool for the estimation of the IPs to the Vertex */
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
    /** option to unbias IP estimation (remove track from vertex) */
    bool m_unbiasIPEstimation;
    

  }; // End class

  inline void IPTag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }

} // End namespace 

#endif
