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

#include <map>
#include <set>
#include <vector>
#include <ostream>

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
      virtual TrackCollection*  process(const TracksScores *trackScoreTrackMap) const override;

      virtual TrackCollection*  process(const TrackCollection*,Trk::PRDtoTrackMap *) const override {return nullptr;};

      /** statistics output to be called by algorithm during finalize. */
      virtual void statistics() override;
    private:
      class TrackStat {
      public:
         TrackStat(const std::vector<float> &eta_bounds) : m_etabounds(&eta_bounds){}

         enum EStatType {
            kNtracks,
            kNinvalid,
            kNcandidates,
            // kNcandScoreZero,
            // kNcandDouble,
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
            kNStatTypes
         };
         enum EGlobalStatType {
            kNevents,
            kNInvalidTracks,
            kNTracksWithoutParam,
            kNGlobalStatTypes,
         };
         /** internal monitoring: categories for counting different types of extension results*/
         enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3, iForwrd = 4, kNStatRegions=5};

         class Counter {
         public:
            Counter &operator+=(const Counter &a) {
               m_counter += a.m_counter;
               return *this;
            }
            Counter &operator++()   { ++m_counter; return *this;}
            operator int() const    { return m_counter; }
            int value()    const    { return m_counter; }
         private:
            unsigned int m_counter=0;
         };

         void newEvent() {
            ++m_globalCounter[kNevents];
         }

         TrackStat &operator+=(const TrackStat &a) {
            for (unsigned int i=0; i<kNGlobalStatTypes; ++i) {
               m_globalCounter[i]+= a.m_globalCounter[i];
            }
            for (unsigned int i=0; i<kNStatTypes; ++i) {
               for (unsigned int region_i=0; region_i< kNStatRegions; ++region_i) {
                  m_counter[i][region_i] += a.m_counter[i][region_i];
               }
            }
            return *this;
         }

         /** helper for monitoring and validation: does success/failure counting */
         void increment_by_eta(EStatType type, const Track* track, bool updateAll=true);
         void dumpStatType(MsgStream &out, const std::string &head, EStatType type, unsigned short iw=9) const;
         void dump(MsgStream &out, bool try_brem_fit) const;

         const std::vector<float>  &etaBounds() const { return *m_etabounds; }  //!< eta intervals for internal monitoring

      private:
         std::array<Counter,kNGlobalStatTypes>                      m_globalCounter;
         std::array<std::array<Counter, kNStatRegions>,kNStatTypes> m_counter;

         const std::vector<float>  *m_etabounds;           //!< eta intervals for internal monitoring
      };

      //transfer ownership
      void addTrack(Track* track, const bool fitted,
                    std::multimap<float, TrackPtr > &scoreTrackFitflagMap,
                    const Trk::PRDtoTrackMap &prd_to_track_map,
                    std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks,
                    Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const;

      void solveTracks(const TracksScores& trackScoreTrackMap,
                       Trk::PRDtoTrackMap &prd_to_track_map,
                       TrackCollection &finalTracks,
                       std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks,
                       Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const;

      /** refit track */
      Track* refitTrack( const Trk::Track* track, Trk::PRDtoTrackMap &prd_to_track_map,
                         Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const;

      /** refit PRDs */
      Track* refitPrds( const Track* track, Trk::PRDtoTrackMap &prd_to_track_map,
                        Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const;

      /** refit ROTs corresponding to PRDs*/
      //TODO or Q: new created track, why const
      Track* refitRots( const Track* track, Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const;

      /** stores the minimal dist(trk,trk) for covariance correction*/
      void storeTrkDistanceMapdR(TrackCollection& tracks,
                                 std::vector<Trk::Track*> &refit_tracks_out );
      
      /** refit Tracks that are in the region of interest and removes inner hits that are wrongly assigned*/
      void removeInnerHits(std::vector<const Trk::MeasurementBase*>& measurements) const;
      Trk::Track* refitTracksFromB(const Trk::Track* track,double fitQualityOriginal) const;
     
      /** see if we are in the region of interest for B tracks*/
      bool decideIfInHighPtBROI(const Trk::Track*) const;

      /** Check if the cluster is compatible with a hadronic cluster*/
      bool isHadCaloCompatible(const Trk::TrackParameters& Tp) const;

      /** Load the clusters to see if they are compatibles with ROI*/
      void reloadHadROIs() const;
           
 
      Trk::Track *fit(std::vector<const Trk::PrepRawData*> &raw,
      			const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const;
      Trk::Track *fit(std::vector<const Trk::MeasurementBase*> &measurements,
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
      std::vector<float>     m_etabounds;           //!< eta intervals for internal monitoring

      SG::WriteHandleKey<InDet::DRMap>                    m_dRMap;      //!< the actual dR map         

      mutable std::mutex m_statMutex;
      mutable TrackStat  m_stat ATLAS_THREAD_SAFE;

      bool m_rejectInvalidTracks;
  };
      inline
      Trk::Track *DenseEnvironmentsAmbiguityProcessorTool::fit(std::vector<const Trk::PrepRawData*> &raw,
                                                               const TrackParameters &param, bool flag,
                                                               Trk::ParticleHypothesis hypo) const {
         Trk::Track *new_track=nullptr;
         for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
              delete new_track;
              new_track=nullptr;
              new_track =  a_fitter->fit(raw, param, flag,hypo);
              if (Trk::DenseEnvironmentsAmbiguityProcessorTool::checkTrack(new_track)) {
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
      Trk::Track *DenseEnvironmentsAmbiguityProcessorTool::fit(std::vector<const Trk::MeasurementBase*> &measurements,
                                                               const TrackParameters &param,
                                                               bool flag,
                                                               Trk::ParticleHypothesis hypo) const
      {
        Trk::Track *new_track=nullptr;
        for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
           delete new_track;
           new_track=nullptr;
           new_track =  a_fitter->fit(measurements, param, flag, hypo);
           if (Trk::DenseEnvironmentsAmbiguityProcessorTool::checkTrack(new_track)) {
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
            if (Trk::DenseEnvironmentsAmbiguityProcessorTool::checkTrack(new_track)) {
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

void Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat::increment_by_eta(EStatType type,
                                                                               const Track* track,
                                                                               bool updateAll) {
   std::array<Counter,kNStatRegions> &Ntracks = m_counter[type];
   if (updateAll) ++Ntracks[iAll];

   // test
   if (!track) {
      ++m_globalCounter[kNInvalidTracks];
   }
   // use first parameter
   if (!track->trackParameters()) {
      ++m_globalCounter[kNTracksWithoutParam];
   }
   else {
      double eta = track->trackParameters()->front()->eta();
      for (unsigned int region_i=1; region_i< kNStatRegions; ++region_i) {
         if (std::abs(eta)      < (*m_etabounds)[region_i-1]) {
            ++Ntracks[region_i];
            break;
         }
      }
   }
}

void Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat::dumpStatType(MsgStream &out,
                                                                           const std::string &head,
                                                                           EStatType type,
                                                                           unsigned short iw) const {
   out << head << std::setiosflags(std::ios::dec);
   for (unsigned region_i=0; region_i<kNStatRegions; ++region_i) {
      out << std::setw(iw) << m_counter[type][region_i].value();
   }
   out << "\n";
}

#endif // TrackAmbiguityProcessorTool_H

