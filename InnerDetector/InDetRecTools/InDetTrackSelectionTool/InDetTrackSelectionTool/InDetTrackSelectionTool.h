// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSELECTIONTOOL_INDETTRACKSELECTIONTOOL_H
#define INDETTRACKSELECTIONTOOL_INDETTRACKSELECTIONTOOL_H

// Local include(s):
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "InDetTrackSelectionTool/InDetTrackCut.h"

// Framework include(s):
#include "AsgTools/AsgTool.h"
#ifndef XAOD_STANDALONE
#include "GaudiKernel/ToolHandle.h"
#ifndef XAOD_ANALYSIS
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#endif
#endif

#include <map>

namespace InDet {

   /// Implementation of the track selector tool
   ///
  class InDetTrackSelectionTool :
    public virtual IInDetTrackSelectionTool,
    public asg::AsgTool {
    
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( InDetTrackSelectionTool,
		     IAsgSelectionTool,
		     InDet::IInDetTrackSelectionTool )
    
  public:
    /// Constructor for standalone usage
    InDetTrackSelectionTool( const std::string& name );

    /// Destructor to clean up memory
    // don't need it with smart pointers
    //    ~InDetTrackSelectionTool();

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{
    
    /// Function initialising the tool
    virtual StatusCode initialize();
    /// Function finalizing the tool
    virtual StatusCode finalize();
    
    /// @}
    
    /// @name Function(s) implementing the IAsgSelectionTool interface
    /// @{
    
    /// Get an object describing the "selection steps" of the tool
    virtual const Root::TAccept& getTAccept() const;
    
    /// Get the decision using a generic IParticle pointer
    virtual const Root::TAccept& accept( const xAOD::IParticle* ) const;
    
    /// @}
    
    /// @name Function(s) implementing the IInDetTrackSelectionTool interface
    /// @{
    
    /// Get the decision for a specific track object
    virtual const Root::TAccept& accept( const xAOD::TrackParticle& track,
					 const xAOD::Vertex* vertex = 0 ) const;

#ifndef XAOD_ANALYSIS
    virtual const Root::TAccept& accept( const Trk::Track& track,
					 const Trk::Vertex* vertex = 0 ) const;
#endif
    
    /// @}

    virtual void setCutLevel( InDet::CutLevel level, Bool_t overwrite = true );
    
  protected:
    // the TrackCut needs to be able to add accessors to the tool in initialize()
    friend StatusCode TrackCut::initialize();

    // TODO: make the accessor an enum instead of string. should only affect initialize(). (?)
    std::unordered_map< std::string, std::shared_ptr<TrackAccessor> > m_trackAccessors; //!< list of the accessors that need to be run for each track

    // first element is cut family, second is the set of cuts
    std::map< std::string, std::vector< std::unique_ptr<TrackCut> > > m_trackCuts; //!< First element is the name of the cut family, second element is the set of cuts

    Double_t m_minPt; //!< Minimum p_T of tracks
    Double_t m_maxAbsEta; //!< Maximum magnitude of pseudorapidity
    Double_t m_maxZ0SinTheta; //!< Maximum |z0*sin(theta)| of tracks
    Double_t m_maxZ0; //!< Maximum |z0| of tracks
    Double_t m_maxD0; //!< Maximum |d0| of tracks
    Double_t m_maxSigmaD0; //!< Maximum sigma of d0
    Double_t m_maxSigmaZ0; //!< Maximum sigma of z0
    Double_t m_maxSigmaZ0SinTheta; //!< Maximum sigma of z0*sin(theta)
    Double_t m_maxD0overSigmaD0; //!< Maximum |d0|/sigma_d0 of tracks
    Double_t m_maxZ0overSigmaZ0; //!< Maximum |z0|/sigma_z0 of tracks
    Double_t m_maxZ0SinThetaoverSigmaZ0SinTheta; //!< Maximum |z0*sin(theta)|/sigma_z0sinTheta
    //Int_t m_minNBLayersHits; //!< Minimum number of B layer hits, including IBL (layers 0 and 1)
    //Int_t m_maxNBLayersSharedHits; //!< Maximum number of shared B Layer hits
    Int_t m_minNInnermostLayerHits; //!< Minimum number of innermost pixel layer hits
    Int_t m_minNNextToInnermostLayerHits; //!< Minimum number of next to innermost pixel layer hits
    Int_t m_minNBothInnermostLayersHits; //!< Minimum number of two innermost pixel layer hits
    Int_t m_maxNInnermostLayerSharedHits; //!<  Maximum number of shared innermost pixel layer hits
    Int_t m_minNSiHits; //!< Minimum number of silicon hits (pixel + SCT)
    Int_t m_maxNSiSharedHits; //!< Maximum number of silicon sensors shared with other track
    Int_t m_minNSiHitsIfSiSharedHits; //!< Minimum number of silicon hits if the track has shared hits
    Int_t m_maxNSiHoles; //!< Maximum number of silicon sensors without a hit
    Int_t m_minNPixelHits; //!< Minimum number of pixel hits
    Int_t m_maxNPixelSharedHits; //!< Maximum number of pixels shared with other track
    Int_t m_maxNPixelHoles; //!< Maximum number of pixel layers without a hit
    Double_t m_minEtaForStrictNSiHitsCut; //!< Eta cutoff above which a tighter cut on NSiHits applies
    Int_t m_minNSiHitsAboveEtaCutoff; //!< Tighter cut on NSiHits above a certain eta
    Bool_t m_maxOneSharedModule; //!< Flag whether to reject if more than one of 1 shared pixel hit or 2 shared SCT hits.
    Bool_t m_useEtaDependentMaxChiSq; //!< Flag whether we use the eta-dependent chi^2/dof cut
    Double_t m_minP; //!< Minimum p = p_T/cos(theta)
    Int_t m_minNSiHitsPhysical; //!< Minimum number of physical (pixel + SCT) hits (no dead sensors)
    Int_t m_minNPixelHitsPhysical; //!< Minimum number of physical hits (no dead sensors)
    Int_t m_minNSctHits; //!< Minimum number of SCT hits (plus dead sensors)
    Int_t m_maxNSctSharedHits; //!< Maximum number of SCT sensors shared with another track
    Int_t m_maxNSctHoles; //!< Maximum number of holes in SCT
    Int_t m_maxNSctDoubleHoles; //!< Maximum number of double holes in SCT
    Double_t m_maxTrtEtaAcceptance; //!< Pseudorapidity below which TRT hit cuts will not be applied
    Int_t m_minNTrtHits; //!< Minimum number of TRT hits
    Int_t m_minNTrtHitsPlusOutliers; //!< Minimum number of TRT hits plus outliers
    Int_t m_minNTrtHighThresholdHits; //!< Minimum number of high E TRT hits
    Int_t m_minNTrtHighThresholdHitsPlusOutliers; //!< Minimum number of high E TRT hits including outliers
    Double_t m_maxTrtHighEFraction; //!< Maximum fraction of TRT hits that are high threshold
    Double_t m_maxTrtHighEFractionWithOutliers; //!< Maximum fraction of TRT hits that are high threshold, including outliers
    Double_t m_maxChiSq; //!< Maximum fit chi squared
    Double_t m_maxChiSqperNdf; //!< Maximum chi squared per degree of freedom
    Double_t m_minProb; //!< Minimum fit probability


    /// Object used to store the last decision
    mutable Root::TAccept m_accept; //!< Object that stores detailed selection information

    // to set to a pre-defined cut level in Athena, we need to save the cut level
    // as a string so we can do a soft set in initialize()
    std::string m_cutLevel; //!< The string version of the cut level so that it can be set via jobOptions

    // we need a map from strings (for use in Athena) to the CutLevel enum
    static const std::unordered_map<std::string, CutLevel> s_mapCutLevel;
   
#ifndef XAOD_ANALYSIS
    Bool_t m_initTrkTools; //!< Whether to initialize the Trk::Track tools

    ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool; //!< Track summary tool
    Bool_t m_trackSumToolAvailable; //!< Whether the summary tool is available
    
    ToolHandle<Trk::IExtrapolator> m_extrapolator; //!< Extrapolator tool
#endif // XAOD_ANALYSIS

  }; // class InDetTrackSelectionTool

} // namespace InDet

#endif // INDETTRACKSELECTIONTOOL_INDETTRACKSELECTIONTOOL_H
