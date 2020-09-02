/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// StatBox.h
// Authors: Sven Vahsen
///////////////////////////////////////////////////////////////////

// Private utility class used by IDTrackStat to do track counting
// Since this StatBox is likely to change to without notification 
// as needed,  I would not recommend anyone else to use it

#ifndef INDETRECSTATISTICS_TrackStatHelper_H
#define INDETRECSTATISTICS_TrackStatHelper_H

#include "TrkTrack/TrackInfo.h"
#include "TrkTrackSummary/TrackSummary.h" // needed by Trk::numberOfTrackSummaryTypes
#include "TrkTrack/TrackCollection.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrack/TrackInfo.h"

#include <vector>
#include <string>
#include <map>
#include <atomic>

// forward declarations:
class PixelID;
class SCT_ID;
class AtlasDetectorID;
class Track;

namespace InDet {

  enum track_types {TRACK_ALL, 
		    TRACK_PRIMARY, 
		    TRACK_SECONDARY, //LT 06.21
		    TRACK_TRUNCATED,
		    TRACK_LOWTRUTHPROB, 
		    TRACK_LOWTRUTHPROB2, 
		    TRACK_NOHEPMCPARTICLELINK, 
		    TRACK_MATCHED,
		    TRACK_MATCHED_PRIMARY, 
		    TRACK_MATCHED_SECONDARY, // LT 06.21
                    TRACK_MULTMATCH,
		    TRACK_MULTMATCH_PRIMARY, 
		    TRACK_MULTMATCH_SECONDARY, // LT 06.21
		    TRACK_ALL_SIGNAL,
		    TRACK_LOWTRUTHPROB_SIGNAL, 
		    TRACK_LOWTRUTHPROB2_SIGNAL, 
		    TRACK_MATCHED_SIGNAL,
		    N_TRACKTYPES};//!< track categories ex. all, primary tracks, etc

 

  enum hit_types   {HIT_ALL, 
		    HIT_PIXEL_ALL, 
		    HIT_PIX1, 
		    HIT_PIX2, 
		    HIT_PIX3, 
		    HIT_SCT_ALL, 
		    HIT_SCT1,
		    HIT_SCT2, 
		    HIT_SCT3, 
		    HIT_SCT4,  
		    HIT_SCT5TO9, 
		    HIT_TRT_ALL,
		    HIT_DBM_ALL,
		    HIT_DBM1,
		    HIT_DBM2,
		    HIT_DBM3,
		    HIT_UNKNOWN, 
		    N_HITTYPES};//!< hit type, all, subsystem, subsystem layer

  enum eta_region  {ETA_ALL, 
		    ETA_BARREL, 
		    ETA_TRANSITION, 
		    ETA_ENDCAP, 
		    ETA_OUTSIDE,
		    ETA_DBM,
		    ETA_UNKNOWN,
		    N_ETAREGIONS}; //!< eta region

  struct cuts {
    float maxEtaBarrel;
    float maxEtaTransition;//!< Maxiumu eta for transition region
    float maxEtaEndcap;//!< Maximum eta for endcap
    float minEtaDBM;
    float maxEtaDBM;
    float fakeTrackCut;//!< fraction of hits per track that come from single matched truth track.  tracks with less than this fraction are considered fakes or low probability
    float fakeTrackCut2;//!< 2nd value for fraction of hits per track that come from single matched truth track.  tracks with less than this fraction are considered fakes or low probability
    float matchTrackCut;//!< Truth  probability has to be greater than this for track to be considered matched    
    float maxRStartPrimary;
    float maxRStartSecondary;
    float maxZStartPrimary;
    float maxZStartSecondary;
    float minREndPrimary;
    float minREndSecondary;
    float minZEndPrimary;
    float minZEndSecondary;
    float minPt;
    cuts(){
      maxEtaBarrel    =0;
      maxEtaTransition=0;
      maxEtaEndcap    =0;
      minEtaDBM = 3.1;
      maxEtaDBM=3.5;
      fakeTrackCut=0;
      fakeTrackCut2     =0;
      matchTrackCut     =0;
      maxRStartPrimary  =  25.0;
      maxRStartSecondary= 360.0;
      maxZStartPrimary  = 200.0;
      maxZStartSecondary=2000.0;
      minREndPrimary    = 400.0;
      minREndSecondary  =1000.0; 
      minZEndPrimary    =2300.0;
      minZEndSecondary  =3200.0;
      minPt = 1000;
    }



  };


  class TrackStatHelper{
       /**
	  @class TrackStatHelper
	  This class is used to print out the statistics used in InDetRecStatisticsAlg to compare multiple track finding algorithms. It additionally prints out the track summary information from the TrackSummaryTool
	  @author lauren.tompkin@cern.ch, sven.vahsen@cern.ch
       */

      public:
    /** Constructor.  Takes strings of TrackCollectionKey and Track TruthCollectionKey. Assume that
	truth collections will be used, but let user specify if they intentionally will not be.*/
    TrackStatHelper     (std::string, std::string, bool careAboutTruth = true);
    /** Sets the cuts such as the eta regions (barrel, transition,endcap) and the hit fraction fake cuts and the track matching cut*/
    void     SetCuts(struct cuts);
    /** Adds hit, track and matching information for each event.  Called at each event*/
    void     addEvent   (const TrackCollection *, 
			       std::vector<const Trk::Track *> &, 
                               Trk::PRDtoTrackMap *prd_to_track_map,
			       std::vector <std::pair<HepMC::GenParticle *,int> > &,   
			       const TrackTruthCollection *, 
			       const AtlasDetectorID * const, 
			       const PixelID *, 
			       const SCT_ID *,
			       const Trk::IExtendedTrackSummaryTool *,
			       bool,
			       unsigned int *,
                               unsigned int *) const;
    /** Resets the track collection information, called in the constructor*/
    void     reset      ();
    /** Prints all of the statistics information,  calls printRegion, printTrackSummaryRegion, etc for detailed statistics*/
    void     print      (MsgStream &out) const;
    /** Prints ntracks per event,efficiencies,fake rates, and general hit information for given eta region*/ 
    void     printRegion1(MsgStream &out, enum eta_region) const;
    void     printRegion2(MsgStream &out, enum eta_region, float denominator) const;
    /** Prints all of the statistics information,  calls printRegion, printTrackSummaryRegion, etc for detailed statistics*/
    void     printSecondary      (MsgStream &out) const;
    /** Prints ntracks per event,efficiencies,fake rates, and general hit information for given eta region*/ 
    void     printRegionSecondary(MsgStream &out, enum eta_region, float denominator) const;


    /** Sets up detailed statistics part of table, calls printTrackSummaryRegion*/
    bool     printTrackSummaryRegion (MsgStream &out, enum track_types, enum eta_region) const;
    /**Prints information from TrackSummaryTool*/
    void     printTrackSummaryAverage(MsgStream &out, enum track_types, enum eta_region, int summary_type) const;
    /** defines 'good' reco tracks*/
    bool     PassTrackCuts(const Trk::TrackParameters *para) const;
    /** classifies gen particle as primary, secondary or truncated */
    int      ClassifyParticle( const HepMC::GenParticle *particle, const double prob) const;


    static std::string getSummaryTypeHeader();

    /** Returns TrackCollection Key */
    const std::string &key() const { return m_TrackCollectionKey; }
    /** Returns Truth TrackCollection Key */
    const std::string &Truthkey() const { return m_TrackTruthCollectionKey; }

  private:

    std::string  m_TrackCollectionKey;//!< StoreGate Track Collection Key
    std::string  m_TrackTruthCollectionKey;  //!< StoreGate Track Truth Collection Key

    mutable std::atomic<bool>  m_author_found ATLAS_THREAD_SAFE [Trk::TrackInfo::NumberOfTrackFitters]; //!<Number of tracking authors found
    mutable std::atomic<long>  m_events ATLAS_THREAD_SAFE ;//!< Number of events

    template <int N_Categories, int N_Types, int N_Regions, typename T_Int=long>
    struct Counter {
       Counter() { reset(); }
       void reset() {
          for (unsigned int cat_i=0; cat_i < N_Categories; ++cat_i ) {
             for (unsigned int type_i=0; type_i < N_Types; ++type_i) {
                for (unsigned int eta_i=0; eta_i < N_Regions; ++eta_i) {
                   m_counter[cat_i][type_i][eta_i]=0;
                }
             }
          }
       }
       template <typename T_IntB>
       Counter &operator +=(const Counter<N_Categories,N_Types,N_Regions,T_IntB> &a) {
          for (unsigned int cat_i=0; cat_i < N_Categories; ++cat_i ) {
             for (unsigned int type_i=0; type_i < N_Types; ++type_i) {
                for (unsigned int eta_i=0; eta_i < N_Regions; ++eta_i) {
                   m_counter[cat_i][type_i][eta_i] += a.m_counter[cat_i][type_i][eta_i];
                }
             }
          }
          return *this;
       }
       T_Int m_counter[N_Categories][N_Types][N_Regions];
    };

    template <int N_Categories, int N_Types, int N_Regions, int N_SubCategories, typename T_Int=long>
    struct Counter4D {
       Counter4D() { reset(); }
       void reset() {
          for (unsigned int cat_i=0; cat_i < N_Categories; ++cat_i ) {
             for (unsigned int type_i=0; type_i < N_Types; ++type_i) {
                for (unsigned int eta_i=0; eta_i < N_Regions; ++eta_i) {
                   for (unsigned int sub_i=0; sub_i < N_SubCategories; ++sub_i) {
                      m_counter[cat_i][type_i][eta_i][sub_i]=0;
                   }
                }
             }
          }
       }
       template <typename T_IntB>
       Counter4D &operator +=(const Counter4D<N_Categories,N_Types,N_Regions,N_SubCategories,T_IntB> &a) {
          for (unsigned int cat_i=0; cat_i < N_Categories; ++cat_i ) {
             for (unsigned int type_i=0; type_i < N_Types; ++type_i) {
                for (unsigned int eta_i=0; eta_i < N_Regions; ++eta_i) {
                   for (unsigned int sub_i=0; sub_i < N_SubCategories; ++sub_i) {
                      m_counter[cat_i][type_i][eta_i][sub_i] += a.m_counter[cat_i][type_i][eta_i][sub_i];
                   }
                }
             }
          }
          return *this;
       }
       T_Int m_counter[N_Categories][N_Types][N_Regions][N_SubCategories];
    };

    enum ETrackCounter {kTracks_rec,        //!< number of reconstructed tracks for a given type and eta region
                        kTracks_gen,        //!< number of generated tracks for a given type and eta region, looping over genevents to include possible pileup
                        kTracks_gen_signal, //!< number of generated tracks for a given type and eta region, just from first genevent
                        kNTracksCounter};

    using TracksCounter = Counter<kNTracksCounter,N_TRACKTYPES,N_ETAREGIONS, int>;
    using TracksCounterAtomic = Counter<kNTracksCounter,N_TRACKTYPES,N_ETAREGIONS, std::atomic<long> >;
    mutable TracksCounterAtomic m_tracks ATLAS_THREAD_SAFE;

    enum EHitsCounter {
       kHits_rec,     //!< number of reconstructed hits for a given type and eta region
       kHits_pri,     //!< number of hits from primary tracks for a given type and eta region
       kHits_sec,     //!< number of hits from secondary tracks for a given type and eta region
       kNHitsCounter
    };
    using HitsCounter = Counter<kNHitsCounter,N_HITTYPES, N_ETAREGIONS, int>;
    using HitsCounterAtomic = Counter<kNHitsCounter,N_HITTYPES, N_ETAREGIONS, std::atomic<long> >;
    mutable HitsCounterAtomic m_hits ATLAS_THREAD_SAFE;
    enum ETrackSummaryCounter {
       kTrackSummarySum,     //!< Track Summary Values for each track type, region and summary type
       kNTrackSummaryOK,     //!< Number of tracks with track summary OK for given type,eta,summary type.
       kNTrackSummaryBAD,    //!< Number of tracks with track summary bad for given type,eta,summary type.
       kNTrackSummaryCounter
    };

    // The ETrackSummaryTypes should be synchronised with the arrays summaryTypes and
    // summaryTypeName i.e. matching order and number of elements. Of these enums only
    // two are used: kNSummaryTypes which defines the summary statistics array sizes;
    // kNumberOfPixelHits, which is used to get counts for some denominators.
    enum ETrackSummaryTypes {
        kNumberOfInnermostPixelLayerHits,
        kNumberOfInnermostPixelLayerSharedHits,
        kNumberOfInnermostPixelLayerOutliers,
        kNumberOfPixelHits,
        kNumberOfPixelSharedHits,
        kNumberOfPixelHoles,
        kNumberOfGangedPixels,
        kNumberOfSCTHits,
        kNumberOfSCTSharedHits,
        kNumberOfSCTHoles,
        kNumberOfSCTDoubleHoles,
        kNumberOfTRTHits,
        kNumberOfTRTOutliers,
        kNumberOfTRTHighThresholdHits,
        kNumberOfTRTHighThresholdOutliers,
        kNumberOfOutliersOnTrack,
        kNumberOfDBMHits,
        kNSummaryTypes
    };
    static const Trk::SummaryType       s_summaryTypes                      [kNSummaryTypes]; //!< summary types for which statistics
                                                                                              //!  are gathered
    static const char            *const s_summaryTypeName ATLAS_THREAD_SAFE [kNSummaryTypes]; //!< table column labels for summary
                                                                                              //!  type statistics.

    using TrackSummaryCounter = Counter4D<kNTrackSummaryCounter,
                                          N_TRACKTYPES,
                                          N_ETAREGIONS,
                                          kNSummaryTypes,
                                          int>;
    using TrackSummaryCounterAtomic = Counter4D<kNTrackSummaryCounter,
                                                N_TRACKTYPES,
                                                N_ETAREGIONS,
                                                kNSummaryTypes,
                                                std::atomic<long> >;
    mutable TrackSummaryCounterAtomic m_trackSummarySum ATLAS_THREAD_SAFE;


    void setSummaryStat(track_types track_i,
                        eta_region region_i,
                        const Trk::TrackSummary *summary,
                        TrackSummaryCounter &trackSummarySum) const
    {
       if (summary) {
          for (int stype=0; stype < kNSummaryTypes; stype++) {
             int value = summary->get(s_summaryTypes[stype]);
             //value is -1 if undefined
             if (value>0) {
                trackSummarySum.m_counter[kTrackSummarySum][track_i][region_i][stype] += value;
                trackSummarySum.m_counter[kNTrackSummaryOK][track_i][region_i][stype] ++;
             }
             else {
                trackSummarySum.m_counter[kNTrackSummaryBAD][track_i][region_i][stype] ++;
             }
          }
       }
    }

    mutable std::atomic<bool>  m_truthMissing ATLAS_THREAD_SAFE; //!< Flag for if track truth is missing
    bool  m_careAboutTruth;
    struct cuts m_cuts;

    mutable std::mutex m_authorMutex ATLAS_THREAD_SAFE;
    mutable std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>   m_recoInfo ATLAS_THREAD_SAFE;
    mutable std::bitset<Trk::TrackInfo::NumberOfTrackProperties> m_patternProperties ATLAS_THREAD_SAFE;

    typedef std::multimap<HepMcParticleLink,float> recoToTruthMap;//!< map containing reco track and matched truth track barcode
  };



} // close of namespace

#endif // INDETRECSTATISTICS_TrackStatHelper_H
