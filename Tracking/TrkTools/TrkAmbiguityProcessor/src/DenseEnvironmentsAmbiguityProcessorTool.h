/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DenseEnvironmentsAmbiguityProcessorTool_H
#define DenseEnvironmentsAmbiguityProcessorTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "TrkAmbiguityProcessor/dRMap.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"

//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackSeedMap.h"
#include "TrkParameters/TrackParameters.h"
//
#include "AmbiCounter.icc"
//
#include <map>
#include <vector>

#include "TrackPtr.h"


class AtlasDetectorID;
class PixelID;

namespace InDet{
  class IPixelClusterSplitProbTool;
  class PixelCluster;
  class SCT_Cluster;
}

namespace Trk {
  class ITrackScoringTool;
  class ITruthToTrack;
  class IExtrapolator;
  //
  class DenseEnvironmentsAmbiguityProcessorTool : public AthAlgTool, 
        virtual public ITrackAmbiguityProcessorTool{
  public:
    enum class EStatType {
      kNtracks,
      kNinvalid,
      kNcandidates,
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
  using TrackStat = AmbiCounter<EStatType>;
  // default methods
  DenseEnvironmentsAmbiguityProcessorTool(const std::string&,const std::string&,const IInterface*);
  virtual ~DenseEnvironmentsAmbiguityProcessorTool ();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize  () override;
  void dumpStat(MsgStream &out) const;


  /**Returns a processed TrackCollection from the passed 'tracks'
  @param tracks collection of tracks which will have ambiguities resolved. Will not be 
  modified.
  The tracks will be refitted if no fitQuality is given at input.
  @return new collections of tracks, with ambiguities resolved. Ownership is passed on 
  (i.e. client handles deletion)*/
  virtual TrackCollection*  process(const TracksScores *trackScoreTrackMap) const override;

  virtual TrackCollection*  process(const TrackCollection*,Trk::PRDtoTrackMap *) const override {return nullptr;};

  /** statistics output to be called by algorithm during finalize. */
  virtual void statistics() override;
  
  private:
    //transfer ownership
    void addTrack(Track* track, const bool fitted,
                  std::multimap<float, TrackPtr > &scoreTrackFitflagMap,
                  const Trk::PRDtoTrackMap &prd_to_track_map,
                  std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks,
                  TrackStat &stat) const;

    void solveTracks(const TracksScores& trackScoreTrackMap,
                     Trk::PRDtoTrackMap &prd_to_track_map,
                     TrackCollection &finalTracks,
                     std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks,
                     TrackStat &stat) const;

    /** refit track */
    Track* refitTrack( const Trk::Track* track, Trk::PRDtoTrackMap &prd_to_track_map,
    TrackStat &stat) const;

    /** refit PRDs */
    Track* refitPrds( const Track* track, Trk::PRDtoTrackMap &prd_to_track_map,
    TrackStat &stat) const;

    /** refit ROTs corresponding to PRDs*/
    //TODO or Q: new created track, why const
    Track* refitRots( const Track* track, TrackStat &stat) const;

    /** stores the minimal dist(trk,trk) for covariance correction*/
    void storeTrkDistanceMapdR(TrackCollection& tracks, std::vector<Trk::Track*> &refit_tracks_out );
    
    /** refit Tracks that are in the region of interest and removes inner hits that are wrongly assigned*/
    void removeInnerHits(std::vector<const Trk::MeasurementBase*>& measurements) const;
    
    Trk::Track* refitTracksFromB(const Trk::Track* track,double fitQualityOriginal) const;
   
    /** see if we are in the region of interest for B tracks*/
    bool decideIfInHighPtBROI(const Trk::Track*) const;

    /** Check if the cluster is compatible with a hadronic cluster*/
    bool isHadCaloCompatible(const Trk::TrackParameters& Tp) const;

    /** Load the clusters to see if they are compatibles with ROI*/
    void reloadHadROIs() const;
         

    Trk::Track *fit(const std::vector<const Trk::PrepRawData*> &raw,
          const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const;
          
    Trk::Track *fit(const std::vector<const Trk::MeasurementBase*> &measurements,
          const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const;
          
    template<typename... Args>
    Trk::Track *fit(const Track &track, Args... args) const;
    
    bool checkTrack(const Trk::Track *) const;
  
    // private data members

    /** brem recovery mode with brem fit ? */
    bool  m_tryBremFit;
    bool  m_caloSeededBrem;
    float m_pTminBrem;

    /** by default refit tracks from PRD */
    bool m_refitPrds;
    
    /** rescale pixel PRD covariances */
    bool m_applydRcorrection;

    /** suppress Hole Search */ 
    bool m_suppressHoleSearch;

    /** suppress Track Fit */ 
    bool m_suppressTrackFit;

    /** by default tracks at input get refitted */
    bool m_forceRefit;

    /** variables to decide if we are in a ROI */
    bool m_useHClusSeed;
    float m_minPtBjetROI;
    float m_phiWidth;
    float m_etaWidth;
    SG::ReadHandleKey<CaloClusterROI_Collection> m_inputHadClusterContainerName;

    mutable std::vector<double>   m_hadF;
    mutable std::vector<double>   m_hadE;
    mutable std::vector<double>   m_hadR;
    mutable std::vector<double>   m_hadZ;

    /** control material effects (0=non-interacting, 1=pion, 2=electron, 3=muon, 4=pion) read in as an integer 
    read in as an integer and convert to particle hypothesis */
    int m_matEffects;
    Trk::ParticleHypothesis m_particleHypothesis;   
    
    /**Scoring tool
       This tool is used to 'score' the tracks, i.e. to quantify what a good track is.
       @todo The actual tool that is used should be configured through job options*/
    ToolHandle<ITrackScoringTool> m_scoringTool;
    
    /** refitting tool - used to refit tracks once shared hits are removed. 
        Refitting tool used is configured via jobOptions.*/
    ToolHandleArray<ITrackFitter> m_fitterTool;
    /** extrapolator tool - used to refit tracks once shared hits are removed. 
        Extrapolator tool used is configured via jobOptions.*/
    ToolHandle<IExtrapolator> m_extrapolatorTool;

    ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
       {this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };

    ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool
      {this, "TrackSummaryTool", "InDetTrackSummaryToolNoHoleSearch"};

    /** key for the PRDtoTrackMap to filled by the ambiguity score processor.**/
    SG::ReadHandleKey<Trk::PRDtoTrackMap>  m_assoMapName
       {this,"AssociationMapName",""};  ///< the key given to the newly created association map

    /** selection tool - here the decision which hits remain on a track and
        which are removed are made */
    ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

    /**These allow us to retrieve the helpers*/
    const PixelID* m_pixelId;
    const AtlasDetectorID* m_idHelper;
    std::vector<float>     m_etaBounds;           //!< eta intervals for internal monitoring

    SG::WriteHandleKey<InDet::DRMap>                    m_dRMap;      //!< the actual dR map         

    mutable std::mutex m_statMutex;
    mutable TrackStat  m_stat ATLAS_THREAD_SAFE;

    bool m_rejectInvalidTracks;
  };
  
  inline Trk::Track *
  DenseEnvironmentsAmbiguityProcessorTool::fit(const std::vector<const Trk::PrepRawData*> &raw,
                                                           const TrackParameters &param, bool flag,
                                                           Trk::ParticleHypothesis hypo) const {
     Trk::Track *newTrack=nullptr;
     for ( const ToolHandle<ITrackFitter> &thisFitter : m_fitterTool) {
          delete newTrack;
          newTrack=nullptr;
          newTrack =  thisFitter->fit(raw, param, flag,hypo);
          if (Trk::DenseEnvironmentsAmbiguityProcessorTool::checkTrack(newTrack)) {
                      return newTrack;
          }
          ATH_MSG_WARNING( "The track fitter, " <<  thisFitter->name() << ", produced a track with an invalid covariance matrix." );
     }
     ATH_MSG_WARNING( "None of the " <<  m_fitterTool.size() << " track fitter(s) produced a track with a valid covariance matrix." );
     if (m_rejectInvalidTracks) {
         delete newTrack;
         newTrack=nullptr;
     }
     return newTrack;
  }

  inline Trk::Track *
  DenseEnvironmentsAmbiguityProcessorTool::fit(const std::vector<const Trk::MeasurementBase*> &measurements,
                                                           const TrackParameters &param,
                                                           bool flag,
                                                           Trk::ParticleHypothesis hypo) const{
    Trk::Track *newTrack=nullptr;
    for ( const ToolHandle<ITrackFitter> &thisFitter : m_fitterTool) {
      delete newTrack;
      newTrack = nullptr;
      newTrack = thisFitter->fit(measurements, param, flag, hypo);
      if (Trk::DenseEnvironmentsAmbiguityProcessorTool::checkTrack(newTrack)) {
        return newTrack;
      }
      ATH_MSG_WARNING( "The track fitter, " <<  thisFitter->name() << ", produced a track with an invalid covariance matrix." );
    }
    ATH_MSG_WARNING( "None of the " <<  m_fitterTool.size() << " track fitter(s) produced a track with a valid covariance matrix." );
    if (m_rejectInvalidTracks) {
      delete newTrack;
      newTrack = nullptr;
    }
    return newTrack;
  }

  template<typename... Args>
  inline Trk::Track *
  DenseEnvironmentsAmbiguityProcessorTool::fit(const Track &track, Args... args) const{
    Trk::Track *newTrack=nullptr;
    for ( const ToolHandle<ITrackFitter> &thisFitter : m_fitterTool) {
      delete newTrack;
      newTrack=nullptr;
      newTrack =  thisFitter->fit(track,args...);
      if (Trk::DenseEnvironmentsAmbiguityProcessorTool::checkTrack(newTrack)) {
         return newTrack;
      }
      ATH_MSG_WARNING( "The track fitter, " <<  thisFitter->name() << ", produced a track with an invalid covariance matrix." );
      //TODO: potential memory leakage 
    }
    ATH_MSG_WARNING( "None of the " <<  m_fitterTool.size() << " track fitter(s) produced a track with a valid covariance matrix." );
    if (m_rejectInvalidTracks) {
      delete newTrack;
      newTrack=nullptr;
    }
    return newTrack;
  }
  
} //end ns


#endif // TrackAmbiguityProcessorTool_H

