/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include <vector>
#include <string>
#include <map>

// forward declarations:
class PixelID;
class SCT_ID;
class AtlasDetectorID;
class Track;
class TrackTruthCollection;

namespace Trk
{
 class ITrackSummaryTool;
}



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
			       std::vector <std::pair<HepMC::GenParticle *,int> > &,   
			       const TrackTruthCollection *, 
			       const AtlasDetectorID * const, 
			       const PixelID *, 
			       const SCT_ID *,
			       Trk::ITrackSummaryTool *,
			       bool,
			       unsigned int *,
		       	       unsigned int *);
    /** Resets the track collection information, called in the constructor*/
    void     reset      ();
    /** Prints all of the statistics information,  calls printRegion, printTrackSummaryRegion, etc for detailed statistics*/
    void     print      ();
    /** Prints ntracks per event,efficiencies,fake rates, and general hit information for given eta region*/ 
    void     printRegion1(enum eta_region);
    void     printRegion2(enum eta_region, float denominator);
    /** Prints all of the statistics information,  calls printRegion, printTrackSummaryRegion, etc for detailed statistics*/
    void     printSecondary      ();
    /** Prints ntracks per event,efficiencies,fake rates, and general hit information for given eta region*/ 
    void     printRegionSecondary(enum eta_region, float denominator);


    /** Sets up detailed statistics part of table, calls printTrackSummaryRegion*/
    bool     printTrackSummaryRegion (enum track_types, enum eta_region);
    /**Prints information from TrackSummaryTool*/
    void     printTrackSummaryAverage(enum track_types, enum eta_region, int summary_type);
    /** defines 'good' reco tracks*/
    bool      PassTrackCuts(const Trk::TrackParameters *para);
    /** classifies gen particle as primary, secondary or truncated */
    int      ClassifyParticle( const HepMC::GenParticle *particle, const double prob);


    /** Returns TrackCollection Key */
    const std::string &key() const { return m_TrackCollectionKey; }
    /** Returns Truth TrackCollection Key */
    const std::string &Truthkey() const { return m_TrackTruthCollectionKey; }

  private:

    std::string  m_TrackCollectionKey;//!< StoreGate Track Collection Key
    std::string  m_TrackTruthCollectionKey;  //!< StoreGate Track Truth Collection Key
    bool         m_author_found [Trk::TrackInfo::NumberOfTrackFitters]; //!<Number of tracking authors found 
    std::string  m_author_string[Trk::TrackInfo::NumberOfTrackFitters];//!< Names of tracking authors
    long  m_events;//!< Number of events
    long  m_tracks_rec[N_TRACKTYPES][N_ETAREGIONS];//!< number of reconstructed tracks for a given type and eta region
    long  m_tracks_gen[N_TRACKTYPES][N_ETAREGIONS];//!< number of generated tracks for a given type and eta region, looping over genevents to include possible pileup
    long  m_tracks_gen_signal[N_TRACKTYPES][N_ETAREGIONS];//!< number of generated tracks for a given type and eta region, just from first genevent
    long  m_hits_rec  [N_HITTYPES]  [N_ETAREGIONS];//!< number of reconstructed hits for a given type and eta region
    long  m_hits_pri  [N_HITTYPES]  [N_ETAREGIONS];//!< number of hits from primary tracks for a given type and eta region
    long  m_hits_sec  [N_HITTYPES]  [N_ETAREGIONS];//!< number of hits from secondary tracks for a given type and eta region
    long  m_TrackSummarySum   [N_TRACKTYPES][N_ETAREGIONS][Trk::numberOfTrackSummaryTypes];//!< Track Summary Values for each track type, region and summary type
    long  m_n_TrackSummaryOK  [N_TRACKTYPES][N_ETAREGIONS][Trk::numberOfTrackSummaryTypes];//!< Number of tracks with track summary OK for given type,eta,summary type.
    long  m_n_TrackSummaryBAD [N_TRACKTYPES][N_ETAREGIONS][Trk::numberOfTrackSummaryTypes];//!< Number of tracks with track summary bad for given type,eta,summary type.
    bool  m_truthMissing; //!< Flag for if track truth is missing
    bool  m_careAboutTruth;
    struct cuts m_cuts;
    
    typedef std::multimap<HepMcParticleLink,float> recoToTruthMap;//!< map containing reco track and matched truth track barcode
    recoToTruthMap m_rttMap;
  };
    


} // close of namespace

#endif // INDETRECSTATISTICS_TrackStatHelper_H
