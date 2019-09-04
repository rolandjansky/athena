/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TrkValInterfaces/ITrkObserverTool.h"
#include "TrkAmbiguityProcessor/dRMap.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"


//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackSeedMap.h"
#include "TrkParameters/TrackParameters.h"

#include <map>
#include <set>
#include <vector>
#include <functional>


class AtlasDetectorID;
class PixelID;

namespace InDet{
  class IPixelClusterSplitProbTool;
  class PixelCluster;
  class SCT_Cluster;
}

namespace Trk {

  class ITrackScoringTool;
  class IPRD_AssociationTool;
  class ITruthToTrack;
  class IExtrapolator;

  class DenseEnvironmentsAmbiguityProcessorTool : public AthAlgTool, 
                                                  virtual public ITrackAmbiguityProcessorTool
    {
    public:
    
      // default methods
      DenseEnvironmentsAmbiguityProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~DenseEnvironmentsAmbiguityProcessorTool ();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      /**Returns a processed TrackCollection from the passed 'tracks'
     @param tracks collection of tracks which will have ambiguities resolved. Will not be 
     modified.
     The tracks will be refitted if no fitQuality is given at input.
     @return new collections of tracks, with ambiguities resolved. Ownership is passed on 
     (i.e. client handles deletion)*/
      virtual TrackCollection*  process(TracksScores* trackScoreTrackMap) override;
      
      virtual TrackCollection*  process(const TrackCollection*) override {return nullptr;}; 
      /** statistics output */
      virtual void statistics() override;

    private:
      
      //transfer ownership
      void addTrack(const Track* track, const bool fitted,
                    std::multimap<float, std::pair<const Track*, bool>>* scoreTrackFitflagMap,
                    std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks);

      void solveTracks(TracksScores* trackScoreTrackMap,
                       TrackCollection* finalTracks,
                       std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks);
    
      /** refit track */
      Track* refitTrack( const Trk::Track* track);
      
      /** refit PRDs */
      Track* refitPrds( const Track* track);

      /** refit ROTs corresponding to PRDs*/
      //TODO or Q: new created track, why const
      Track* refitRots( const Track* track);

      /** stores the minimal dist(trk,trk) for covariance correction*/
      void storeTrkDistanceMapdR(TrackCollection& tracks,
                                 std::vector<Trk::Track*> &refit_tracks_out );
      
      /** refit Tracks that are in the region of interest and removes inner hits that are wrongly assigned*/
      void removeInnerHits(std::vector<const Trk::MeasurementBase*>& measurements) const;
      Trk::Track* refitTracksFromB(const Trk::Track* track,double fitQualityOriginal) const;
     
      /** see if we are in the region of interest for B tracks*/
      bool decideIfInHighPtBROI(const Trk::Track*);

      /** Check if the cluster is compatible with a hadronic cluster*/
      bool isHadCaloCompatible(const Trk::TrackParameters& Tp) const;

      /** Load the clusters to see if they are compatibles with ROI*/
      void reloadHadROIs();
           
 
      Trk::Track *fit(std::vector<const Trk::PrepRawData*> &raw,
      			const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const;
      Trk::Track *fit(std::vector<const Trk::MeasurementBase*> &measurements,
      			const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const;
      template<typename... Args>
      Trk::Track *fit(const Track &track, Args... args) const;
      bool _checkTrack(const Trk::Track *) const;
    
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

      std::vector<double>   m_hadF;
      std::vector<double>   m_hadE;
      std::vector<double>   m_hadR;
      std::vector<double>   m_hadZ;

      /** control material effects (0=non-interacting, 1=pion, 2=electron, 3=muon, 4=pion) read in as an integer 
      read in as an integer and convert to particle hypothesis */
      int m_matEffects;
      Trk::ParticleHypothesis m_particleHypothesis;   
      
      /**Scoring tool
         This tool is used to 'score' the tracks, i.e. to quantify what a good track is.
         @todo The actual tool that is used should be configured through job options*/
      ToolHandle<ITrackScoringTool> m_scoringTool;

	  /**Observer tool
         This tool is used to observe the tracks and their 'score' */
      ToolHandle<ITrkObserverTool> m_observerTool;
      
      /** refitting tool - used to refit tracks once shared hits are removed. 
          Refitting tool used is configured via jobOptions.*/
      ToolHandleArray<ITrackFitter> m_fitterTool;
      /** extrapolator tool - used to refit tracks once shared hits are removed. 
          Extrapolator tool used is configured via jobOptions.*/
      ToolHandle<IExtrapolator> m_extrapolatorTool;

      /** selection tool - here the decision which hits remain on a track and
          which are removed are made */
      ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

      
      /**These allow us to retrieve the helpers*/
      const PixelID* m_pixelId;
      const AtlasDetectorID* m_idHelper;
      
      /** internal monitoring: categories for counting different types of extension results*/
      enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3};
      std::vector<float>  m_etabounds;           //!< eta intervals for internal monitoring

      bool m_monitorTracks; // to track observeration/monitoring (default is false)

      SG::WriteHandleKey<InDet::DRMap>                    m_dRMap;      //!< the actual dR map         

      bool m_rejectInvalidTracks;
  };
      inline
      Trk::Track *DenseEnvironmentsAmbiguityProcessorTool::fit(std::vector<const Trk::PrepRawData*> &raw,
                                                           const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const {
         Trk::Track *new_track=nullptr;
         for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
              delete new_track;
              new_track=nullptr;
              new_track =  a_fitter->fit(raw, param, flag,hypo);
              if (Trk::DenseEnvironmentsAmbiguityProcessorTool::_checkTrack(new_track)) {
                          return new_track;
              }
              ATH_MSG_WARNING( "The track fitter, " <<  a_fitter->name() << ", produced a track with an invalid covariance matrix." );
         }
         ATH_MSG_WARNING( "None of the " <<  m_fitterTool.size() << " track fitter(s) produced a track with a valid covariance matrix." );
         if (m_rejectInvalidTracks) {
             delete new_track;
             new_track=nullptr;
         }
         return new_track;
      }

      inline
      Trk::Track *DenseEnvironmentsAmbiguityProcessorTool::fit(std::vector<const Trk::MeasurementBase*> &measurements, const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const
      {
        Trk::Track *new_track=nullptr;
        for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
           delete new_track;
           new_track=nullptr;
           new_track =  a_fitter->fit(measurements, param, flag, hypo);
           if (Trk::DenseEnvironmentsAmbiguityProcessorTool::_checkTrack(new_track)) {
              return new_track;
           }
           ATH_MSG_WARNING( "The track fitter, " <<  a_fitter->name() << ", produced a track with an invalid covariance matrix." );
        }
        ATH_MSG_WARNING( "None of the " <<  m_fitterTool.size() << " track fitter(s) produced a track with a valid covariance matrix." );
        if (m_rejectInvalidTracks) {
          delete new_track;
          new_track=nullptr;
        }
        return new_track;
      }

      template<typename... Args>
      inline
      Trk::Track *DenseEnvironmentsAmbiguityProcessorTool::fit(const Track &track, Args... args) const
      {
         Trk::Track *new_track=nullptr;
         for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
            delete new_track;
            new_track=nullptr;
            new_track =  a_fitter->fit(track,args...);
            if (Trk::DenseEnvironmentsAmbiguityProcessorTool::_checkTrack(new_track)) {
               return new_track;
            }
            ATH_MSG_WARNING( "The track fitter, " <<  a_fitter->name() << ", produced a track with an invalid covariance matrix." );
            //TODO: potential memory leakage 
         }
        ATH_MSG_WARNING( "None of the " <<  m_fitterTool.size() << " track fitter(s) produced a track with a valid covariance matrix." );
        if (m_rejectInvalidTracks) {
          delete new_track;
          new_track=nullptr;
        }
        return new_track;
      }
} //end ns


#endif // TrackAmbiguityProcessorTool_H

