/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DenseEnvironmentsAmbiguityScoreProcessorTool_H
#define DenseEnvironmentsAmbiguityScoreProcessorTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkToolInterfaces/ITrackAmbiguityScoreProcessorTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "TrkValInterfaces/ITrkObserverTool.h"


//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackSeedMap.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include <map>
#include <set>
#include <vector>
#include <functional>
#include <ostream>


namespace InDet{
  class IPixelClusterSplitProbTool;
  class PixelCluster;
  class SCT_Cluster;
}

namespace Trk {

  class ITrackScoringTool;

  class DenseEnvironmentsAmbiguityScoreProcessorTool: public AthAlgTool, 
                                                      virtual public ITrackAmbiguityScoreProcessorTool
    {
    public:
      
      // public types
      typedef std::multimap< TrackScore, const Track* > TrackScoreMap;
      typedef std::set<const PrepRawData*> PrdSignature;
      typedef std::set<PrdSignature> PrdSignatureSet;
      
      // default methods
      DenseEnvironmentsAmbiguityScoreProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~DenseEnvironmentsAmbiguityScoreProcessorTool();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      virtual void process(std::vector<const Track*>* tracks,
                           TracksScores* trackScoreTrackMap) const override;

      /** statistics output to be called by algorithm during finalize. */
      void statistics() override;
    private:
      class TrackStat {
      public:
         TrackStat(const std::vector<float> &eta_bounds) : m_etabounds(&eta_bounds){}

         enum EStatType {
            kNtracks,
            kNcandidates,
            kNcandScoreZero,
            kNcandDouble,
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
         void dumpStatType(std::ostream &out, const std::string &head, EStatType type, unsigned short iw=9) const;
         void dump(std::ostream &out) const;

         const std::vector<float>  &etaBounds() const { return *m_etabounds; }  //!< eta intervals for internal monitoring

      private:
         std::array<Counter,kNGlobalStatTypes>                      m_globalCounter;
         std::array<std::array<Counter, kNStatRegions>,kNStatTypes> m_counter;

         const std::vector<float>  *m_etabounds;           //!< eta intervals for internal monitoring
      };
      
      /**Add passed TrackCollection, and Trk::PrepRawData from tracks to caches
         @param tracks the TrackCollection is looped over, 
         and each Trk::Track is added to the various caches. 
         The Trk::PrepRawData from each Trk::Track are added to the IPRD_AssociationTool*/
      void addNewTracks(std::vector<const Track*>* tracks,
                        TracksScores* trackScoreTrackMap) const;

      /**  Find SiS Tracks that share hits in the track score map*/
      void overlappingTracks(const TracksScores* scoreTrackMap,
                             InDet::PixelGangedClusterAmbiguities *splitClusterMap,
                             Trk::PRDtoTrackMap &prd_to_track_map) const;

      /**  Update pixel split information based using the fitted track*/
      void updatePixelSplitInformationForCluster(const std::pair<const InDet::PixelCluster* const,
                                                                 const Trk::TrackParameters*> & clusterTrkPara,
                                                 InDet::PixelGangedClusterAmbiguities *splitClusterMap) const;

               
      /**Scoring tool
         This tool is used to 'score' the tracks, i.e. to quantify what a good track is.
         @todo The actual tool that is used should be configured through job options*/
      ToolHandle<ITrackScoringTool> m_scoringTool;
      
      ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
         {this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };

      ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoToolNotGanged
         {this, "AssociationToolNotGanged", "Trk::PRDtoTrackMapTool" };  // @TODO why are ganged pixels ignored ?
       
      /** selection tool - here the decision which hits remain on a track and
          which are removed are made */
      ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

      /** recalculate split prob tool **/
      ToolHandle<InDet::IPixelClusterSplitProbTool> m_splitProbTool; 

      /** cluster split information generated by previous instances*/
      SG::ReadHandleKey<InDet::PixelGangedClusterAmbiguities>  m_splitClusterMapKey_last;
      
      /** cluster split information which m_splitClusterMap_last will be merged into*/  
      SG::WriteHandleKey<InDet::PixelGangedClusterAmbiguities>  m_splitClusterMapKey;

       /** key for the PRDtoTrackMap to be used by the ambiguity process to resolve the ambiguities.**/
      SG::WriteHandleKey<Trk::PRDtoTrackMap>  m_assoMapName
         {this,"AssociationMapName",""};  ///< the key given to the newly created association map

      /**NN split sprob cut for 2 particle clusters */      
      float m_sharedProbCut;

      /**NN split sprob cut for 3 particle clusters */      
      float m_sharedProbCut2;

      std::vector<float>     m_etabounds;           //!< eta intervals for internal monitoring
      mutable std::mutex m_statMutex;
      mutable TrackStat  m_stat ATLAS_THREAD_SAFE;
  };
} //end ns

void Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::TrackStat::increment_by_eta(EStatType type,
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

void Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::TrackStat::dumpStatType(std::ostream &out,
                                                                           const std::string &head,
                                                                           EStatType type,
                                                                           unsigned short iw) const {
   out << head << std::setiosflags(std::ios::dec);
   for (unsigned region_i=0; region_i<kNStatRegions; ++region_i) {
      out << std::setw(iw) << m_counter[type][region_i].value();
   }
   out << std::endl;
}

#endif // TrackAmbiguityProcessorTool_H
