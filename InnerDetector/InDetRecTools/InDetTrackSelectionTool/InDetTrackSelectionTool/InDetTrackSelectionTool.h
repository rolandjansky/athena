// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSELECTIONTOOL_INDETTRACKSELECTIONTOOL_H
#define INDETTRACKSELECTIONTOOL_INDETTRACKSELECTIONTOOL_H

// Local include(s):
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

// Framework include(s):
#include "AsgTools/AsgTool.h"
#ifndef XAOD_ANALYSIS
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#endif

#include <map>
#include <limits>

namespace InDet {

  // forward declaration of helper classes
  class TrackAccessor;
  class TrackCut;

   /// Implementation of the track selector tool
   ///
  class InDetTrackSelectionTool :
    public virtual IInDetTrackSelectionTool,
    public asg::AsgTool {
    
    friend class TrackCut;

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( InDetTrackSelectionTool,
		     IAsgSelectionTool,
		     InDet::IInDetTrackSelectionTool )
    
  public:
    /// Constructor for standalone usage
    InDetTrackSelectionTool( const std::string& name, const std::string& cutLevel = "" );

    // The default destructor is OK but it must be defined in the
    // implementation file in order to forward declare with unique_ptr
    ~InDetTrackSelectionTool();

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{
    
    /// Function initialising the tool
    virtual StatusCode initialize() override;
    /// Function finalizing the tool
    virtual StatusCode finalize() override;
    
    /// @}
    
    /// @name Function(s) implementing the IAsgSelectionTool interface
    /// @{
    
    /// Get an object describing the "selection steps" of the tool
    virtual const Root::TAccept& getTAccept() const override;
    
    /// Get the decision using a generic IParticle pointer
    virtual const Root::TAccept& accept( const xAOD::IParticle* ) const override;
    
    /// @}
    
    /// @name Function(s) implementing the IInDetTrackSelectionTool interface
    /// @{
    
    /// Get the decision for a specific track object
    virtual const Root::TAccept& accept( const xAOD::TrackParticle& track,
					 const xAOD::Vertex* vertex = nullptr ) const override;

#ifndef XAOD_ANALYSIS
    virtual const Root::TAccept& accept( const Trk::Track& track,
					 const Trk::Vertex* vertex = nullptr ) const override;
#endif
    
    /// @}

    virtual void setCutLevel( InDet::CutLevel level, Bool_t overwrite = true ) override
      __attribute__ ((deprecated("For consistency with the athena interface, the cut level is best set through the \"CutLevel\" property.")));

  private:
    bool m_isInitialized = false; // flag whether or not the tool has been initialized, to check erroneous use cases.
    mutable bool m_warnInit = false; // flag to keep track of whether we have warned about a lack of initialization

    // this is the setCutLevel function that actually does the work, so that it doesn't warn if called in athena.
    void setCutLevelPrivate( InDet::CutLevel level, Bool_t overwrite = true );

    std::unordered_map< std::string, std::shared_ptr<TrackAccessor> > m_trackAccessors; //!< list of the accessors that need to be run for each track

    // first element is cut family, second is the set of cuts
    std::map< std::string, std::vector< std::unique_ptr<TrackCut> > > m_trackCuts; //!< First element is the name of the cut family, second element is the set of cuts

    mutable ULong64_t m_numTracksProcessed = 0; //!< a counter of the number of tracks proccessed
    mutable ULong64_t m_numTracksPassed = 0; //!< a counter of the number of tracks that passed all cuts
    mutable std::vector<ULong64_t> m_numTracksPassedCuts; //!< tracks the number of tracks that passed each cut family

    constexpr static Double_t LOCAL_MAX_DOUBLE = 1.0e16;
    constexpr static Int_t LOCAL_MAX_INT = std::numeric_limits<Int_t>::max();

    Double_t m_minPt = -1.; //!< Minimum p_T of tracks
    Double_t m_maxAbsEta = LOCAL_MAX_DOUBLE; //!< Maximum magnitude of pseudorapidity
    Double_t m_maxZ0SinTheta = LOCAL_MAX_DOUBLE; //!< Maximum |z0*sin(theta)| of tracks
    Double_t m_maxZ0 = LOCAL_MAX_DOUBLE; //!< Maximum |z0| of tracks
    Double_t m_maxD0 = LOCAL_MAX_DOUBLE; //!< Maximum |d0| of tracks
    Double_t m_maxSigmaD0 = LOCAL_MAX_DOUBLE; //!< Maximum sigma of d0
    Double_t m_maxSigmaZ0 = LOCAL_MAX_DOUBLE; //!< Maximum sigma of z0
    Double_t m_maxSigmaZ0SinTheta = LOCAL_MAX_DOUBLE; //!< Maximum sigma of z0*sin(theta)
    Double_t m_maxD0overSigmaD0 = LOCAL_MAX_DOUBLE; //!< Maximum |d0|/sigma_d0 of tracks
    Double_t m_maxZ0overSigmaZ0 = LOCAL_MAX_DOUBLE; //!< Maximum |z0|/sigma_z0 of tracks
    Double_t m_maxZ0SinThetaoverSigmaZ0SinTheta = LOCAL_MAX_DOUBLE; //!< Maximum |z0*sin(theta)|/sigma_z0sinTheta
    Int_t m_minNInnermostLayerHits = -1; //!< Minimum number of innermost pixel layer hits
    Int_t m_minNNextToInnermostLayerHits = -1; //!< Minimum number of next to innermost pixel layer hits
    Int_t m_minNBothInnermostLayersHits = -1; //!< Minimum number of two innermost pixel layer hits
    Int_t m_maxNInnermostLayerSharedHits = LOCAL_MAX_INT; //!<  Maximum number of shared innermost pixel layer hits
    Int_t m_useMinBiasInnermostLayersCut = 0; //!< Use the Minimum-Bias definition of the IBL/BL cut
    Int_t m_minNSiHits = -1; //!< Minimum number of silicon hits (pixel + SCT)
    Int_t m_maxNSiSharedHits = LOCAL_MAX_INT; //!< Maximum number of silicon sensors shared with other track
    Int_t m_minNSiHitsIfSiSharedHits = -1; //!< Minimum number of silicon hits if the track has shared hits
    Int_t m_maxNSiHoles = LOCAL_MAX_INT; //!< Maximum number of silicon sensors without a hit
    Int_t m_minNPixelHits = -1; //!< Minimum number of pixel hits
    Int_t m_maxNPixelSharedHits = LOCAL_MAX_INT; //!< Maximum number of pixels shared with other track
    Int_t m_maxNPixelHoles = LOCAL_MAX_INT; //!< Maximum number of pixel layers without a hit
    Double_t m_minEtaForStrictNSiHitsCut = LOCAL_MAX_DOUBLE; //!< Eta cutoff above which a tighter cut on NSiHits applies
    Int_t m_minNSiHitsAboveEtaCutoff = -1; //!< Tighter cut on NSiHits above a certain eta
    Int_t m_maxNSiSharedModules = LOCAL_MAX_INT; //!< Reject if more than N of 1 shared pixel hit + 2 shared SCT hits.
    Bool_t m_useEtaDependentMaxChiSq = false; //!< Flag whether we use the eta-dependent chi^2/dof cut
    Double_t m_minP = -1.; //!< Minimum p = p_T/cos(theta)
    Int_t m_minNSiHitsPhysical = -1; //!< Minimum number of physical (pixel + SCT) hits (no dead sensors)
    Int_t m_minNPixelHitsPhysical = -1; //!< Minimum number of physical pixel hits (no dead sensors)
    Int_t m_minNSctHitsPhysical = -1; //!< Minimum number of physical SCT hits (no dead sensors)
    Int_t m_minNSctHits = -1; //!< Minimum number of SCT hits (plus dead sensors)
    Int_t m_maxNSctSharedHits = LOCAL_MAX_INT; //!< Maximum number of SCT sensors shared with another track
    Int_t m_maxNSctHoles = LOCAL_MAX_INT; //!< Maximum number of holes in SCT
    Int_t m_maxNSctDoubleHoles = LOCAL_MAX_INT; //!< Maximum number of double holes in SCT
    Double_t m_maxTrtEtaAcceptance = LOCAL_MAX_DOUBLE; //!< Pseudorapidity below which TRT hit cuts will not be applied
    Double_t m_maxEtaForTrtHitCuts = -1.; //!< Pseudorapidity above which TRT hit cuts will not be applied
    Int_t m_minNTrtHits = -1; //!< Minimum number of TRT hits
    Int_t m_minNTrtHitsPlusOutliers = -1; //!< Minimum number of TRT hits plus outliers
    Int_t m_minNTrtHighThresholdHits = -1; //!< Minimum number of high E TRT hits
    Int_t m_minNTrtHighThresholdHitsPlusOutliers = -1; //!< Minimum number of high E TRT hits including outliers
    Double_t m_maxTrtHighEFraction = LOCAL_MAX_DOUBLE; //!< Maximum fraction of TRT hits that are high threshold
    Double_t m_maxTrtHighEFractionWithOutliers = LOCAL_MAX_DOUBLE; //!< Maximum fraction of TRT hits that are high threshold, including outliers
    Double_t m_maxTrtOutlierFraction = LOCAL_MAX_DOUBLE; //!< Maximum fraction of TRT outliers over TRT hits + outliers
    Double_t m_maxChiSq = LOCAL_MAX_DOUBLE; //!< Maximum fit chi squared
    Double_t m_maxChiSqperNdf = LOCAL_MAX_DOUBLE; //!< Maximum chi squared per degree of freedom
    Double_t m_minProb = -1.; //!< Minimum fit probability
    Double_t m_minPtForProbCut = LOCAL_MAX_DOUBLE; //!< Pt above which a Prob(chiSq, ndf) cut is applied
    Double_t m_minProbAbovePtCutoff = -1.; //!< Minimum probability above Pt cutoff
    Int_t m_minNUsedHitsdEdx = -1; //!< Minimum number of dEdx hits used
    Int_t m_minNOverflowHitsdEdx = -1; //!< Minimum number of IBL overflow hits for dEdx
    Bool_t m_eProbHTonlyForXe = false; //!< Flag whether to only check eProbabilityHT if all TRT hits are Xenon hits
    Double_t m_minEProbabilityHT = -1.; //!< Minimum eProbabiltyHT
    Int_t m_useExperimentalInnermostLayersCut = false; //!< use (NPixHoles == 0) but allow 1 hole if a BLayer hit is expected but not present
#ifndef XAOD_ANALYSIS
    Int_t m_minNSiHitsMod = -1; //!< Minimum number of Si hits, with pixel hits counting twice
    Int_t m_minNSiHitsModTop = -1; //!< Min number of Si hits on top half (pixel counting twice)
    Int_t m_minNSiHitsModBottom = -1; //!< Min number of Si hits on bottom half (pixel counting twice)
#endif
    std::vector< Double_t > m_vecEtaCutoffsForSiHitsCut; //!< Above each of these eta levels a new SCT hits + pix hits will be applied
    std::vector< Int_t > m_vecMinNSiHitsAboveEta; //!< the minimum Si hits above each eta level
    std::vector< Double_t > m_vecEtaCutoffsForPtCut; //!< Above each of these eta levels a new pT cut will be applied                                                                           
    std::vector< Double_t > m_vecMinPtAboveEta; //!< the minimum transverse momentum above each eta level

    std::vector< Double_t > m_vecPtCutoffsForSctHitsCut; //!< Above each of these pT levels a new SCT hits + dead sensors will be applied
    std::vector< Int_t > m_vecMinNSctHitsAbovePt; //!< the minimum SCT hits above each pt level

    /// Object used to store the last decision
    mutable Root::TAccept m_accept; //!< Object that stores detailed selection information

    // to set to a pre-defined cut level in Athena, we need to save the cut level
    // as a string so we can do a soft set in initialize()
    std::string m_cutLevel; //!< The string version of the cut level so that it can be set via jobOptions

    // we need a map from strings (for use in Athena) to the CutLevel enum
    static const std::unordered_map<std::string, CutLevel> s_mapCutLevel;
   
#ifndef XAOD_ANALYSIS
    Bool_t m_initTrkTools = false; //!< Whether to initialize the Trk::Track tools
    Bool_t m_trackSumToolAvailable = false; //!< Whether the summary tool is available    
    ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool; //!< Track summary tool
    ToolHandle<Trk::IExtrapolator> m_extrapolator; //!< Extrapolator tool

#endif // XAOD_ANALYSIS

  }; // class InDetTrackSelectionTool

} // namespace InDet

#endif // INDETTRACKSELECTIONTOOL_INDETTRACKSELECTIONTOOL_H
