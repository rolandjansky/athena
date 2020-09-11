/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AmbiguityProcessorBase_h
#define AmbiguityProcessorBase_h

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "AmbiCounter.icc"
#include "GaudiKernel/ToolHandle.h"

#include "TrackPtr.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkEventUtils/ClusterSplitProbabilityContainer.h"

#include <vector>
#include <map> //multimap
#include <memory> //unique_ptr
#include <mutex>  //mutex



namespace Trk {
  //fwd declare
  class ITrackScoringTool;
  class Track;
  class PRDtoTrackMap;

  //base class for SimpleAmbiguityProcessorTool and DenseEnvironmentsAmbiguityProcessorTool
  class AmbiguityProcessorBase : public AthAlgTool, virtual public ITrackAmbiguityProcessorTool {
  public:
    enum class CounterIndex {
      kNcandidates,
      kNcandScoreZero,
      kNcandDouble,
      kNscoreOk,
      kNscoreZeroBremRefit,
      kNscoreZeroBremRefitFailed,
      kNscoreZeroBremRefitScoreZero,
      kNscoreZero,
      kNaccepted,
      kNsubTrack,
      kNnoSubTrack,
      kNacceptedBrem,
      kNbremFits,
      kNfits,
      kNrecoveryBremFits,
      kNgoodFits,
      kNfailedFits,
      kNCounter
    };
    using Counter = AmbiCounter<CounterIndex>;
    using TrackScoreMap = std::multimap< TrackScore, TrackPtr > ;

    // default methods
    AmbiguityProcessorBase(const std::string&,const std::string&,const IInterface*);
    virtual ~AmbiguityProcessorBase () = default;
  protected:
    // methods
    // should try a Brem refit? based on track properties
    bool
    shouldTryBremRecovery(const Trk::Track & track) const;
    
    // should try a Brem refit? based on track properties and previously obtained track parameters
    bool
    shouldTryBremRecovery(const Trk::Track & track, const TrackParameters * pPar) const;
    
    // do a brem refit; implemented in the derived classes
    virtual std::unique_ptr<Trk::Track>
    doBremRefit(const Trk::Track & track) const = 0;
    
    /** refit track */
    Track * 
    refitTrack( const Trk::Track* track,Trk::PRDtoTrackMap &prdToTrackMap, Counter &stat) const;
                       
    //refit PRD
    virtual Trk::Track* 
    refitPrds( const Trk::Track* track, Trk::PRDtoTrackMap &prdToTrackMap,Counter &stat) const = 0;
    
    //refit ROTs
    virtual Trk::Track* 
    refitRots( const Trk::Track* track, Counter &stat) const;
    
    //generic normal fit
    virtual std::unique_ptr<Trk::Track>
    fit(const Track &track, bool flag, Trk::ParticleHypothesis hypo) const = 0;
    
    void
    addTrack(Trk::Track* in_track, const bool fitted,
             TrackScoreMap &trackScoreTrackMap,
             Trk::PRDtoTrackMap &prdToTrackMap,
             std::vector<std::unique_ptr<const Trk::Track> >& trackDustbin,
             Counter &stat) const;
                                                 
    const TrackParameters *
    getTrackParameters(const Trk::Track* track) const;

    /** Initialize read and write handles for ClusterSplitProbabilityContainers.
     * If a write handle key is specified for the new ClusterSplitProbabilityContainer, read handles
     * for this key are "renounced" in all child tools.
     */
    StatusCode
    initializeClusterSplitProbContainer();

    // special pointer type for the ClusterSplitProbabilityContainerPtr to allow to use a single "pointer" for
    // ClusterSplitProbabilityContainer which need to be deleted and those owned by storegate
    using UniqueClusterSplitProbabilityContainerPtr = std::unique_ptr<Trk::ClusterSplitProbabilityContainer,void(*)(Trk::ClusterSplitProbabilityContainer*) >;

    /** Create a new cluster splitting probability container and (optionally) record it in storegate
     * The new container may be populated from an already existing container, and might be stored in StoreGate.
     * The ownersip, which might be either storegate or the calling scope, is taken into account by the
     * UniqueClusterSplitProbabilityContainerPtr and must not be touched i.e. unique_ptr::release must not be called.
     */
    AmbiguityProcessorBase::UniqueClusterSplitProbabilityContainerPtr
    createAndRecordClusterSplitProbContainer(const EventContext& ctx) const;

    //variables accessible to derived classes
    std::vector<float>     m_etaBounds;           //!< eta intervals for internal monitoring
    mutable std::mutex m_statMutex;
    mutable Counter  m_stat ATLAS_THREAD_SAFE;
    /**Scoring tool
       This tool is used to 'score' the tracks, i.e. to quantify what a good track is.
       @todo The actual tool that is used should be configured through job options*/
    ToolHandle<ITrackScoringTool> m_scoringTool;
    ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool{this, "TrackSummaryTool", "InDetTrackSummaryToolNoHoleSearch"};

  private:
    // the handles should not be used directly instead the methods initializeClusterSplitProbContainer
    // and createAndRecordClusterSplitProbContainer should be used.
    SG::ReadHandleKey<Trk::ClusterSplitProbabilityContainer> m_clusterSplitProbContainerIn
        {this, "InputClusterSplitProbabilityName", "",""};
    SG::WriteHandleKey<Trk::ClusterSplitProbabilityContainer> m_clusterSplitProbContainerOut
       {this, "OutputClusterSplitProbabilityName", "",""};

  protected:
     /** brem recovery mode with brem fit ? */
    bool  m_tryBremFit{};
    bool  m_caloSeededBrem{};
    float m_pTminBrem{1000.};
    bool m_suppressHoleSearch{};
    // for track refit
    bool m_suppressTrackFit{};
    bool m_refitPrds{};
    /** by default tracks at input get refitted */
    bool m_forceRefit{true};
    
    /** read in as an integer and convert to particle hypothesis */
    /** reference: /TrkEventPrimitives/ParticleHypothesis.h **/
    int m_matEffects{3};//pion
    Trk::ParticleHypothesis m_particleHypothesis{undefined};   
  };
}//namespace
#endif

