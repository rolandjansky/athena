/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackStatHelper.cxx
// Authors: Sven Vahsen
///////////////////////////////////////////////////////////////////

// Private utility class used by IDTrackStat to do track and hit counting
// Since I'm likely to changes the interface as needed,  
// I would not recommend anyone to use this class outside of IDTrackStat
//
// to-do-list
//   o  remove TrackCollection argument to addEvent. Is this really needed for the truthmatching?
//   o  add DR cut to the fake track cut
//

#include "InDetRecStatistics/TrackStatHelper.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "AtlasHepMC/GenVertex.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>
#include <cstring>
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "CLHEP/Geometry/Point3D.h"

namespace Trk {

  class RoT_Extractor {
  public:
    static void extract(std::vector<const RIO_OnTrack*>& rots, const std::vector<const MeasurementBase*>& measurements);
  };

}


// summary types for which statistics are gathered.
// should be synchronised with ETrackSummaryTypes
const Trk::SummaryType InDet::TrackStatHelper::s_summaryTypes[kNSummaryTypes] = {
   Trk::numberOfInnermostPixelLayerHits,
   Trk::numberOfInnermostPixelLayerSharedHits,
   Trk::numberOfInnermostPixelLayerOutliers,
   Trk::numberOfPixelHits,
   Trk::numberOfPixelSharedHits,
   Trk::numberOfPixelHoles,
   Trk::numberOfGangedPixels,
   Trk::numberOfSCTHits,
   Trk::numberOfSCTSharedHits,
   Trk::numberOfSCTHoles,
   Trk::numberOfSCTDoubleHoles,
   Trk::numberOfTRTHits,
   Trk::numberOfTRTOutliers,
   Trk::numberOfTRTHighThresholdHits,
   Trk::numberOfTRTHighThresholdOutliers,
   Trk::numberOfOutliersOnTrack,
   Trk::numberOfDBMHits
};

// Table column labels. should be synchronised with ETrackSummaryTypes
const char * const InDet::TrackStatHelper::s_summaryTypeName[kNSummaryTypes] = {
   "blay",
   "shrd",
   "outl",
   "pix",
   "shrd",
   "hole",
   "gang",
   "SCT",
   "shrd",
   "hole",
   "DHole",
   "TRT",
   "outl",
   "TRHi",
   "outl",
   "alloutl",
   "DBM"};

std::string InDet::TrackStatHelper::getSummaryTypeHeader() {
   std::stringstream out;
   for (unsigned int stype_i=0; stype_i < kNSummaryTypes; ++stype_i ) {
      if (0 != strcmp("DBM",s_summaryTypeName[stype_i])){
         out << std::setw(std::max(6,static_cast<int>(strlen(s_summaryTypeName[stype_i])+1))) << s_summaryTypeName[stype_i];
      }
   }
   return out.str();
}


static const std::string track_types_string[InDet::N_TRACKTYPES]= {"all",
								   "primary",
								   "secondary",
								   "truncated",
								   "lowPb1",
								   "lowPb2",
								   "noLink", 
								   "matched",
								   "matchedPrimary",
                                                                   "matchedSecondary",
                                                                   "multipleMatches"
								   };//LT added 06.21
 
InDet::TrackStatHelper::TrackStatHelper(std::string TrackCollectionKey, std::string TrackTruthCollectionKey, bool careAboutTruth):
  m_TrackCollectionKey       (TrackCollectionKey),
  m_TrackTruthCollectionKey  (TrackTruthCollectionKey),
  m_truthMissing             (false),
  m_careAboutTruth           (careAboutTruth)
{
  this->reset();
}


void InDet::TrackStatHelper::SetCuts(struct cuts ct)
{
  m_cuts = ct;

}

void InDet::TrackStatHelper::addEvent(const TrackCollection              * recTracks, 
				      std::vector <const Trk::Track *>   & rec, 
                                      Trk::PRDtoTrackMap *prd_to_track_map,
				      std::vector <std::pair<HepMC::GenParticle *,int> > & gen, 
				      const TrackTruthCollection         * truthMap, 
				      const AtlasDetectorID              * const idHelper, 
				      const PixelID                      * pixelID, 
				      const SCT_ID                       * sctID,
				      const Trk::IExtendedTrackSummaryTool* trkSummaryTool,
				      bool                               useTrackSummary,
				      unsigned int                       * inTimeStart,
				      unsigned int                       * inTimeEnd) const
{

  recoToTruthMap rttMap;
  
  recoToTruthMap::const_iterator imap;
  
  m_events ++;
  
  float Eta = 0;
  InDet::eta_region Region = ETA_ALL;
  int Author = 0;
  int recoClassification = 0;
  bool truth_missing=false;

  TracksCounter       tracks;
  HitsCounter         hits;
  TrackSummaryCounter trackSummarySum;
  bool                author_found [Trk::TrackInfo::NumberOfTrackFitters];
  std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo>   reco_info;
  std::bitset<Trk::TrackInfo::NumberOfTrackProperties> pattern_properties;
  for (unsigned int i=0; i < Trk::TrackInfo::NumberOfTrackFitters; ++i) { author_found[i]=false; }

  // ------------ reconstructed tracks -----------------
  for (const Trk::Track* track : rec) {

    const Trk::TrackParameters* para = track->trackParameters()->front();    
    recoClassification = -999;
    int thisEventIndex = -999;
    
    Author = track->info().trackFitter();
    if (Author > 0 && Author < Trk::TrackInfo::NumberOfTrackFitters){
      author_found[Author]  = true;
      reco_info |= track->info().patternRecognition();
      pattern_properties |= track->info().properties();
    }
    else {
      //FIX author_problem++;
    }
    if (!para) {
      Region =  ETA_UNKNOWN;
    }
    else {
      Eta = fabs(para->eta());
      if (Eta < m_cuts.maxEtaBarrel) Region = ETA_BARREL;
      else if  (Eta < m_cuts.maxEtaTransition) Region = ETA_TRANSITION;
      else if  (Eta < m_cuts.maxEtaEndcap) Region = ETA_ENDCAP;
      else if ((Eta > m_cuts.minEtaDBM) && (Eta < m_cuts.maxEtaDBM)) Region = ETA_DBM;
      else Region = ETA_OUTSIDE;
    }
    
    if ( !(Region==ETA_BARREL || Region==ETA_TRANSITION || Region==ETA_ENDCAP || Region==ETA_DBM) )
      {
	continue; // Only want to tally tracks that are within the detector volume.
      }

    TrackTruthCollection::const_iterator found;
    TrackTruth trtruth;
    double     trprob = 0;  

    if (!truthMap) {
      // no truthmap
      truth_missing=true;
    }
    else {
      ElementLink<TrackCollection> tracklink;
      tracklink.setElement(const_cast<Trk::Track*>(track));
      tracklink.setStorableObject(*recTracks);
      const ElementLink<TrackCollection> tracklink2=tracklink;
      
      found = truthMap->find(tracklink2);
      if(!(found == truthMap->end())){
	trtruth=found->second;
	HepMcParticleLink hmpl = trtruth.particleLink();
	thisEventIndex = hmpl.eventIndex();
	rttMap.insert(std::pair<HepMcParticleLink,float>(hmpl,trtruth.probability()));
	// ME: remember prob
	trprob = trtruth.probability();
      }
      
    }
    
    
    if(!PassTrackCuts(para)) continue;
    
    tracks.m_counter[kTracks_rec][TRACK_ALL][Region]++;
    tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_ALL]++;
    // signal only tracks for the denominator in signal efficiencies
    if (thisEventIndex==0)
      {
	tracks.m_counter[kTracks_rec][TRACK_ALL_SIGNAL][Region]++;
	tracks.m_counter[kTracks_rec][TRACK_ALL_SIGNAL][ETA_ALL]++;
      }
    
    // process track summary
    
    std::unique_ptr<Trk::TrackSummary> cleanup;
    const Trk::TrackSummary* summary = track->trackSummary();
    
    if (useTrackSummary) {
       if (!track->trackSummary()) {
          cleanup = trkSummaryTool->summary(*track,prd_to_track_map);
          summary=cleanup.get();
       }

       setSummaryStat(TRACK_ALL, Region, summary, trackSummarySum);
    }
      
    // check if current reco track has more than a specified fraction of hits
    // from a generated track. [If not, it will be counted as a fake
    // track in the statistics report.]	
      
    if (!truthMap) {
      truth_missing=true;
    }
    else {// no truthmap
	
      if (found == truthMap->end()) {
	// m_truthMissing=true;
	// no truth might happen with new truth !
	tracks.m_counter[kTracks_rec][TRACK_NOHEPMCPARTICLELINK][Region]++;
	tracks.m_counter[kTracks_rec][TRACK_NOHEPMCPARTICLELINK][ETA_ALL]++;
        setSummaryStat(TRACK_NOHEPMCPARTICLELINK, Region, summary, trackSummarySum);
      }
      else{   // no link  
	  
	// ME : change logic, secondaries from G4 processes are truncated, so Barcode 0 is possible 
	const HepMcParticleLink HMPL=trtruth.particleLink();
	if (! HMPL.isValid()) {
	  tracks.m_counter[kTracks_rec][TRACK_NOHEPMCPARTICLELINK][Region]++;
	  tracks.m_counter[kTracks_rec][TRACK_NOHEPMCPARTICLELINK][ETA_ALL]++;
          setSummaryStat(TRACK_NOHEPMCPARTICLELINK, Region, summary, trackSummarySum);
	}
	else {
	  //classify track as coming from primary, secondary or truncated gen particle
	  //	    if(HMPL.isValid()){
	  const HepMC::GenParticle *particle = HMPL.cptr();
	  recoClassification = ClassifyParticle(particle, trprob);
	    
	  if (trprob < m_cuts.fakeTrackCut)
	    {
	      tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB][Region]++;
	      tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB][ETA_ALL]++;
	      if (thisEventIndex==0)
		{
		  tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB_SIGNAL][Region]++;
		  tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB_SIGNAL][ETA_ALL]++;
		}
              setSummaryStat(TRACK_LOWTRUTHPROB, Region, summary, trackSummarySum);
	    }
	  if (trprob < m_cuts.fakeTrackCut2) {
	    tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB2][Region]++;
	    tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB2][ETA_ALL]++;
	    if (thisEventIndex==0)
	      {
		tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB2_SIGNAL][Region]++;
		tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB2_SIGNAL][ETA_ALL]++;
	      }
            setSummaryStat(TRACK_LOWTRUTHPROB2, Region, summary, trackSummarySum);
	  }
	} // end else on !isVadid
      }
    }
      
    // ------------------ hits on reconstructed tracks ---------------
      
    EHitsCounter part_type = ( recoClassification==TRACK_PRIMARY
                               ? kHits_pri
                               : ( recoClassification==TRACK_SECONDARY
                                   ? kHits_sec
                                   : kNHitsCounter));
    for (const Trk::TrackStateOnSurface* hit : *track->trackStateOnSurfaces()) {
      if(hit){ 
	const Trk::MeasurementBase* mesh =hit->measurementOnTrack();
	if (mesh) {
	  const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(mesh);
	  if (!rio) {
	    // try CompetingROT:
	    const Trk::CompetingRIOsOnTrack* comprot = dynamic_cast<const 
	      Trk::CompetingRIOsOnTrack*>(mesh);
	    if (comprot) {
	      rio = &comprot->rioOnTrack(comprot->indexOfMaxAssignProb());
	    }
	  }
	    
	    
	  if (rio) {
	    // skip outliers
	    if (!hit->type(Trk::TrackStateOnSurface::Measurement)) 
	      continue;
	      
	    hits.m_counter[kHits_rec][HIT_ALL][Region]++;
	    hits.m_counter[kHits_rec][HIT_ALL][ETA_ALL]++;
	    if(part_type<kNHitsCounter){
	      hits.m_counter[part_type][HIT_ALL][Region]++;
	      hits.m_counter[part_type][HIT_ALL][ETA_ALL]++;
	    }
	      
            Identifier id = rio->identify();
            int HitDet    = HIT_UNKNOWN;
            int HitLayer  = N_HITTYPES;
            bool part_type_for_all=true;
            if (idHelper->is_trt(id)){
               HitDet=HIT_TRT_ALL;
            }
            else if (idHelper->is_sct(id)){
              HitDet=HIT_SCT_ALL;
              if (sctID) {
                switch (sctID->layer_disk(id)) {
                case 0: HitLayer = HIT_SCT1; break;
                case 1: HitLayer = HIT_SCT2; break;
                case 2: HitLayer = HIT_SCT3; break;
                case 3: HitLayer = HIT_SCT4; break;
                case 4: case 5: case 6: case 7: case 8: HitLayer = HIT_SCT5TO9; break;
                default: HitLayer = HIT_UNKNOWN; break;
                }
              }
            }
            else if (idHelper->is_pixel(id)){
              part_type_for_all=false;
              if (pixelID && pixelID->is_dbm(id)) {
                 HitDet = HIT_DBM_ALL;
                 switch (pixelID->layer_disk(id)) {
                 case 0:  HitLayer = HIT_DBM1; break;
                 case 1:  HitLayer = HIT_DBM2; break;
                 case 2:  HitLayer = HIT_DBM3; break;
                 default: HitLayer = HIT_UNKNOWN;
                 }
              }
              else {
                HitDet = HIT_PIXEL_ALL;
                if (pixelID) {
                  switch (pixelID->layer_disk(id)) {
                  case 0:  HitLayer = HIT_PIX1; break;
                  case 1:  HitLayer = HIT_PIX2; break;
                  case 2:  HitLayer = HIT_PIX3; break;
                  default: HitLayer = HIT_UNKNOWN;
                  }
                }
              }
            }

            hits.m_counter[kHits_rec][HitDet][Region]++;
            hits.m_counter[kHits_rec][HitDet][ETA_ALL]++;
            if(part_type_for_all && part_type<kNHitsCounter){
               hits.m_counter[part_type][HitDet][Region]++;
               hits.m_counter[part_type][HitDet][ETA_ALL]++;
            }

            if (HitLayer<N_HITTYPES) {
               hits.m_counter[kHits_rec][HitLayer][Region]++;
               hits.m_counter[kHits_rec][HitLayer][ETA_ALL]++;
               if(part_type<kNHitsCounter){
                  hits.m_counter[part_type][HitLayer][Region]++;
                  hits.m_counter[part_type][HitLayer][ETA_ALL]++;
               }
            }
          }
	}
      }
    }
  }
  
  // ------------------------- generated tracks, including pileup -----------------------------
  
  Eta = 0;
  Region = ETA_ALL;
  int classification=-999;
  for (std::vector <std::pair<HepMC::GenParticle *,int> >::const_iterator truth = gen.begin(); truth != gen.end();  ++truth) {
    classification=-999; 
    bool inTimePileup = truth->second == 0
      || (truth->second >= (int)*inTimeStart && truth->second <= (int)*inTimeEnd);
    
    HepMC::GenParticle * const particle = truth->first;
    
    //determine eta region
    Eta = fabs(particle->momentum().pseudoRapidity());
    if (Eta < m_cuts.maxEtaBarrel) Region = ETA_BARREL;
    else if  (Eta < m_cuts.maxEtaTransition) Region = ETA_TRANSITION;
    else if  (Eta < m_cuts.maxEtaEndcap) Region = ETA_ENDCAP;
    else if ((Eta > m_cuts.minEtaDBM) && (Eta < m_cuts.maxEtaDBM)) Region = ETA_DBM;
    else Region = ETA_OUTSIDE;
    if ( !(Region==ETA_BARREL || Region==ETA_TRANSITION || Region==ETA_ENDCAP || Region==ETA_DBM) )
      {
	continue; // Only want to tally tracks that are within the detector volume.
      }
    tracks.m_counter[kTracks_gen][TRACK_ALL][Region]++;
    tracks.m_counter[kTracks_gen][TRACK_ALL][ETA_ALL] ++;
    if (inTimePileup) tracks.m_counter[kTracks_gen][TRACK_ALL_SIGNAL][ETA_ALL]++; // "SIGNAL" is misnomer here
    if (inTimePileup) tracks.m_counter[kTracks_gen][TRACK_ALL_SIGNAL][Region]++;

    //classify gen as primary, secondary or truncated
    classification = ClassifyParticle(particle,1.);
    
    if(classification==TRACK_PRIMARY){
      tracks.m_counter[kTracks_gen][TRACK_PRIMARY][ETA_ALL] ++;
      tracks.m_counter[kTracks_gen][TRACK_PRIMARY][Region]++;
    }
    if(classification==TRACK_SECONDARY){
      tracks.m_counter[kTracks_gen][TRACK_SECONDARY][ETA_ALL] ++;
      tracks.m_counter[kTracks_gen][TRACK_SECONDARY][Region]++;
    }
    
    //see if gen track has at least 1 matching reco track with high enough probability
    bool matched = false;
    int nmatched = 0;
    
    HepMcParticleLink hmpl2(particle,truth->second);
    recoToTruthMap::iterator rttIter=rttMap.find(hmpl2);
    if(rttIter != rttMap.end()){
      for(imap = rttMap.lower_bound(hmpl2); imap !=rttMap.upper_bound(hmpl2); ++imap){
	if(imap->second > m_cuts.matchTrackCut){
	  matched = true;
	  nmatched++;
	}
      }
    }
    if (matched) {
      tracks.m_counter[kTracks_gen][TRACK_MATCHED][Region]++;
      tracks.m_counter[kTracks_gen][TRACK_MATCHED][ETA_ALL]++;
      if (inTimePileup)
	{
	  tracks.m_counter[kTracks_gen][TRACK_MATCHED_SIGNAL][Region]++;
	  tracks.m_counter[kTracks_gen][TRACK_MATCHED_SIGNAL][ETA_ALL]++;
	}
      if(nmatched > 1){
	tracks.m_counter[kTracks_gen][TRACK_MULTMATCH][Region] += nmatched-1;
	tracks.m_counter[kTracks_gen][TRACK_MULTMATCH][ETA_ALL]+= nmatched-1;
      }
      if(classification==TRACK_PRIMARY){
	tracks.m_counter[kTracks_gen][TRACK_MATCHED_PRIMARY][Region]++;
	tracks.m_counter[kTracks_gen][TRACK_MATCHED_PRIMARY][ETA_ALL]++;
	if(nmatched > 1){
	  tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_PRIMARY][Region]  += nmatched-1;
	  tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_PRIMARY][ETA_ALL] += nmatched-1;
	}
      }
      if(classification==TRACK_SECONDARY){
	tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][Region]++;
	tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][ETA_ALL]++;
	if(nmatched > 1){
	  tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_SECONDARY][Region]  += nmatched-1;
	  tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_SECONDARY][ETA_ALL] += nmatched-1;
	}
      }
    }	
  }
  
  // ------------------------- generated tracks, just signal event -----------------------------
  // For efficiencies.
  
  Eta = 0;
  Region = ETA_ALL;
  classification=-999;
  
  for (std::vector <std::pair<HepMC::GenParticle *,int> >::const_iterator truth = gen.begin(); truth != gen.end();  ++truth) 
    {
      if (truth->second != 0) // only signal event GenParticles
	continue;
      
      classification=-999; 
      
      HepMC::GenParticle * const particle = truth->first;
      
      //determine eta region
      Eta = fabs(particle->momentum().pseudoRapidity());
      if (Eta < m_cuts.maxEtaBarrel) Region = ETA_BARREL;
      else if  (Eta < m_cuts.maxEtaTransition) Region = ETA_TRANSITION;
      else if  (Eta < m_cuts.maxEtaEndcap) Region = ETA_ENDCAP;
      else if ((Eta > m_cuts.minEtaDBM) && (Eta < m_cuts.maxEtaDBM)) Region = ETA_DBM;
      else Region = ETA_OUTSIDE;
      if ( !(Region==ETA_BARREL || Region==ETA_TRANSITION || Region==ETA_ENDCAP || Region==ETA_DBM) )
	{
	  continue; // Only want to tally tracks that are within the detector volume.
	}
      tracks.m_counter[kTracks_gen_signal][TRACK_ALL][ETA_ALL] ++;
      tracks.m_counter[kTracks_gen_signal][TRACK_ALL][Region]++;
      
      //classify gen partilce as primary, secondary or truncated
      classification = ClassifyParticle(particle,1.);
      
      if(classification==TRACK_PRIMARY){
	tracks.m_counter[kTracks_gen_signal][TRACK_PRIMARY][ETA_ALL] ++;
	tracks.m_counter[kTracks_gen_signal][TRACK_PRIMARY][Region]++;
      }
      if(classification==TRACK_SECONDARY){
	tracks.m_counter[kTracks_gen_signal][TRACK_SECONDARY][ETA_ALL] ++;
	tracks.m_counter[kTracks_gen_signal][TRACK_SECONDARY][Region]++;
      }
      
      //see if gen track has at least 1 matching reco track with high enough probability
      bool matched = false;
      int nmatched = 0;
      
      HepMcParticleLink hmpl2(particle,truth->second);
      recoToTruthMap::iterator rttIter=rttMap.find(hmpl2);
      if(rttIter != rttMap.end()){
	for(imap = rttMap.lower_bound(hmpl2); imap !=rttMap.upper_bound(hmpl2); ++imap){
	  if(imap->second > m_cuts.matchTrackCut){
	    matched = true;
	    nmatched++;
	  }
	}
      }
      if (matched) {
	tracks.m_counter[kTracks_gen_signal][TRACK_MATCHED][Region]++;
	tracks.m_counter[kTracks_gen_signal][TRACK_MATCHED][ETA_ALL]++;
	if(nmatched > 1){
	  tracks.m_counter[kTracks_gen_signal][TRACK_MULTMATCH][Region] += nmatched-1;
	  tracks.m_counter[kTracks_gen_signal][TRACK_MULTMATCH][ETA_ALL]+= nmatched-1;
	}
	if(classification==TRACK_PRIMARY){
	  tracks.m_counter[kTracks_gen_signal][TRACK_MATCHED_PRIMARY][Region]++;
	  tracks.m_counter[kTracks_gen_signal][TRACK_MATCHED_PRIMARY][ETA_ALL]++;
	  if(nmatched > 1){
	    tracks.m_counter[kTracks_gen_signal][TRACK_MULTMATCH_PRIMARY][Region]  += nmatched-1;
	    tracks.m_counter[kTracks_gen_signal][TRACK_MULTMATCH_PRIMARY][ETA_ALL] += nmatched-1;
	  }
	}
	if(classification==TRACK_SECONDARY){
	  tracks.m_counter[kTracks_gen_signal][TRACK_MATCHED_SECONDARY][Region]++;
	  tracks.m_counter[kTracks_gen_signal][TRACK_MATCHED_SECONDARY][ETA_ALL]++;
	  if(nmatched > 1){
	    tracks.m_counter[kTracks_gen_signal][TRACK_MULTMATCH_SECONDARY][Region]  += nmatched-1;
	    tracks.m_counter[kTracks_gen_signal][TRACK_MULTMATCH_SECONDARY][ETA_ALL] += nmatched-1;
	  }
	}
      }	
    }  

  if (truth_missing) {
     m_truthMissing = truth_missing;
  }
  m_tracks          += tracks;
  m_hits            += hits;
  m_trackSummarySum += trackSummarySum;
  for (unsigned int i=0; i < Trk::TrackInfo::NumberOfTrackFitters; ++i) {
     if (author_found[i]) {
        m_author_found[i] = author_found[i];
     }
  }
  {
     std::lock_guard<std::mutex> lock(m_authorMutex);
     m_recoInfo          |= reco_info;
     m_patternProperties |= pattern_properties;
  }
}

void InDet::TrackStatHelper::reset(){
  m_events = 0;
  m_hits.reset();
  m_tracks.reset();
  m_trackSummarySum.reset();

  for (int i=0; i<Trk::TrackInfo::NumberOfTrackFitters; i++) {
    m_author_found[i] = false;
  }
  m_truthMissing=false;
}


void InDet::TrackStatHelper::print(MsgStream &out) const {

  out << " Printing Statistics for " << key();

  out << "TrackCollection \"" << key() << "\" " << "\n" ;

  if (m_events > 0) {
    out << "(TrackAuthors:";
    std::vector<std::string> author_string;
    author_string.reserve(Trk::TrackInfo::NumberOfTrackFitters);
    for (int i=0; i<Trk::TrackInfo::NumberOfTrackFitters; ++i) {
       author_string.push_back( Trk::TrackInfo(static_cast<Trk::TrackInfo::TrackFitter>(i),
                                               Trk::undefined,
                                               m_patternProperties,
                                               m_recoInfo).dumpInfo() );
    }
    for (int i=0; i<Trk::TrackInfo::NumberOfTrackFitters; i++){
      if (m_author_found[i]){
	out << " " << author_string[i];
      }
    }
    out << " )" << std::endl
	      << "TrackTruthCollection \"" << Truthkey() << "\"" << std::endl;
 
    if (m_truthMissing)
      {
	if (m_careAboutTruth)
	  out << " WARNING: TrackTruth missing for part of this TrackCollection, no efficiencies or fakerates included.!" << std::endl;
	else
	  out << " INFO: Intentionally no TrackTruth for this TrackCollection, no efficiencies or fakerates included." << std::endl;
      }
    if (!m_truthMissing)
      {
	out << " \t\t\t  ................................tracks................................" << std::endl;
	out << " \t\t\tn/event\tSEff.pr\tSEff.sd\tSLowP1\tSLowP2\tLowP1\tLowP2\tnoLink\tmultM\t" << std::endl;
	out << " total     "; printRegion1(out,ETA_ALL);
	out << " in barrel "; printRegion1(out,ETA_BARREL);
	out << " in trans. "; printRegion1(out,ETA_TRANSITION);
	out << " in endcap "; printRegion1(out,ETA_ENDCAP);
	out << " in forwa."; printRegion1(out,ETA_DBM);

	      }
    else // no truth
      {
	out << "\t" << "tracks, n/event:" << std::endl;
	out << "\t\t" << "total" << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
	  std::setw(7) << std::setprecision(2) << "\t" << m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_ALL]/(float) m_events << std::endl << std::setprecision(-1);
	out << "\t\t" << "in barrel" << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
	  std::setw(7) << std::setprecision(2) << "\t" << m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_BARREL]/(float) m_events << std::endl << std::setprecision(-1);
	out << "\t\t" << "in trans." << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
	  std::setw(7) << std::setprecision(2) << "\t" << m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_TRANSITION]/(float) m_events << std::endl << std::setprecision(-1);
	out << "\t\t" << "in endcap" << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
	  std::setw(7) << std::setprecision(2) << "\t" << m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_ENDCAP]/(float) m_events << std::endl << std::setprecision(-1);
	out << "\t\t" << "in forwa." << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
          std::setw(7) << std::setprecision(2) << "\t" << m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_DBM]/(float) m_events << std::endl << std::setprecision(-1);
      }
    out << " \t\t\t ....................................hits/track............................" << std::endl;
    out << " \t\t\t\ttotal\tPIX1\tPIX2\tPIX3\tSCT1\tSCT2\tSCT3\tSCT4\tSCT5to9\tStraws" << std::endl;
    out << " total     "; printRegion2(out,ETA_ALL, (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_ALL]);
    out << " in barrel "; printRegion2(out,ETA_BARREL, (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_BARREL]);
    out << " in trans. "; printRegion2(out,ETA_TRANSITION, (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_TRANSITION] );
    out << " in endcap "; printRegion2(out,ETA_ENDCAP, (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_ENDCAP] );
    out << " in forwa. "; printRegion2(out,ETA_DBM, (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][ETA_DBM] );

  }
  else
    out << ": NO EVENTS PROCESSED! " << std::endl;
}

void InDet::TrackStatHelper::printRegion1(MsgStream &out, enum eta_region region) const {
  out  <<  std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(7) << std::setprecision(2)
	     << "\t" << m_tracks.m_counter[kTracks_rec][TRACK_ALL][region]/(float) m_events;
 
  if (m_tracks.m_counter[kTracks_gen_signal][TRACK_PRIMARY][region]) {
    out << "\t" <<  std::setprecision(4)
        << m_tracks.m_counter[kTracks_gen_signal][TRACK_MATCHED_PRIMARY][region]/ (float) m_tracks.m_counter[kTracks_gen_signal][TRACK_PRIMARY][region]     // track efficiency wrt. gen primary in signal
        <<  std::setprecision(2);
  }
  else {
    out  << "\t" << "n/a" ;
  }
  if (m_tracks.m_counter[kTracks_gen_signal][TRACK_SECONDARY][region]) {
    out << "\t" <<  std::setprecision(4)
        << m_tracks.m_counter[kTracks_gen_signal][TRACK_MATCHED_SECONDARY][region]/ (float) m_tracks.m_counter[kTracks_gen_signal][TRACK_SECONDARY][region]     // track efficiency wrt. gen secondary in signal
        <<  std::setprecision(2);
  }
  else {
    out  << "\t" << "n/a" ;
  }
  
  if (m_tracks.m_counter[kTracks_rec][TRACK_ALL_SIGNAL][region]) {
    out << "\t" << 100*(m_tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB_SIGNAL][region]/ (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL_SIGNAL][region]) << "%"  // track fake rate for signal only
        << "\t" << 100*(m_tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB2_SIGNAL][region]/ (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL_SIGNAL][region]) << "%";  // track fake rate for signal only
  }
  else {
    out  << "\t" << "n/a\tn/a" ;
  }
  
  if (m_tracks.m_counter[kTracks_rec][TRACK_ALL][region]) {
    out << "\t" << 100*(m_tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB][region]/ (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][region]) << "%"  // track fake rate
        << "\t" << 100*(m_tracks.m_counter[kTracks_rec][TRACK_LOWTRUTHPROB2][region]/ (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][region]) << "%"  // track fake rate
        << "\t" << 100*(m_tracks.m_counter[kTracks_rec][TRACK_NOHEPMCPARTICLELINK][region]/ (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][region])  << "%" // rate of tracks without HepMcParticleLink
        << "\t" << 100*(m_tracks.m_counter[kTracks_gen][TRACK_MULTMATCH][region]/ (float) m_tracks.m_counter[kTracks_rec][TRACK_ALL][region])  << "%"; // rate of tracks matched to the same truth track

  }
  else{
    out  << "\tn/a\tn/a\tn/a";
  }
  out << std::endl << std::setprecision(-1);
}

void InDet::TrackStatHelper::printRegion2(MsgStream &out, enum eta_region region, float denominator) const {
  out  <<  std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(7) << std::setprecision(2);
  if (denominator > 0) {     
    out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(4) << std::setprecision(1)
	      <<"    \t\t" << float(m_hits.m_counter[kHits_rec][HIT_ALL    ][region]/denominator) << std::setprecision(2)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_PIX1   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_PIX2   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_PIX3   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_SCT1   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_SCT2   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_SCT3   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_SCT4   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_SCT5TO9][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_rec][HIT_TRT_ALL][region]/denominator)
	      << std::endl << std::setprecision(-1);
  }
  else {
    out << " Unable to calculate: Denominator=0." << std::endl << std::setprecision(-1);
  } 
}

bool InDet::TrackStatHelper::printTrackSummaryRegion(MsgStream &out,
                                                     enum track_types track_type ,
						     enum eta_region eta_region ) const
{
  long ex_denom =  m_trackSummarySum.m_counter[kNTrackSummaryOK][track_type][eta_region][kNumberOfPixelHits]
                 + m_trackSummarySum.m_counter[kNTrackSummaryBAD][track_type][eta_region][kNumberOfPixelHits];
  if (ex_denom > 0)
    {
      out << std::setiosflags(std::ios::fixed | std::ios::showpoint)
          << std::setw(8) << track_types_string[track_type]
          << std::setw(25) << key();
      
      if (m_events)
	out << std::setw(6)  << std::setprecision(2) << m_tracks.m_counter[kTracks_rec][track_type][eta_region]/(float) m_events;
      else
	out << std::setw(6) << "n/a";
      for (unsigned int stype_i=0; stype_i< kNSummaryTypes; ++stype_i) {
         if (0 != strcmp("DBM",s_summaryTypeName[stype_i])){
            out << std::setw(std::max(6,static_cast<int>(strlen(s_summaryTypeName[stype_i])+1)))
                << std::setprecision(2); printTrackSummaryAverage(out, track_type , eta_region, stype_i );
         }
      }
      out << std::endl << std::setprecision(-1);
      return true; // yes, printed output
    }
  else
    return false; // no, didn't print output
}

void InDet::TrackStatHelper::printTrackSummaryAverage(MsgStream &out,
                                                      enum track_types track_type ,
                                                      enum eta_region eta_region,
                                                      int summary_type ) const
{
  assert( summary_type < kNSummaryTypes);
  long denom =  m_trackSummarySum.m_counter[kNTrackSummaryOK][track_type][eta_region][summary_type]
              + m_trackSummarySum.m_counter[kNTrackSummaryBAD][track_type][eta_region][summary_type];

  if (denom > 0) {
    out << float ( m_trackSummarySum.m_counter[kTrackSummarySum][track_type][eta_region][summary_type]/ (float) denom);
  }
  else {
    out << "n/a";
  }
}

void InDet::TrackStatHelper::printSecondary(MsgStream &out) const {
  out << "TrackCollection \"" << key() << "\" " << std::endl;
  if (m_events > 0) {
    out << "(TrackAuthors:";
    std::vector<std::string> author_string;
    author_string.reserve(Trk::TrackInfo::NumberOfTrackFitters);
    for (int i=0; i<Trk::TrackInfo::NumberOfTrackFitters; i++) {
       author_string.push_back( Trk::TrackInfo(static_cast<Trk::TrackInfo::TrackFitter>(i),
                                               Trk::undefined,
                                               m_patternProperties,
                                               m_recoInfo).dumpInfo() );
    }
    for (int i=0; i<Trk::TrackInfo::NumberOfTrackFitters; i++){
      if (m_author_found[i]){
	out << " " << author_string[i];
      }
    }
    out << " )" << std::endl
	      << "TrackTruthCollection \"" << Truthkey() << "\"" << std::endl;
 
    if (m_truthMissing)
      {
	if (m_careAboutTruth)
	  out << " WARNING: TrackTruth missing for part of this TrackCollection --> No secondaries information printed!" << std::endl;
	else
	  out << " INFO: Intentionally no TrackTruth for this TrackCollection. (No secondaries information printed.)" << std::endl;
      }
    if (!m_truthMissing)
      {
	out << " \t\t\t\t      ......................truth mached tracks statistics....................." << std::endl;
	out << " \t\t\t\tn/event\teff.\ttotal\tPIX1\tPIX2\tPIX3\tSCT1\tSCT2\tSCT3\tSCT4\tSCT5to9\tStraws" << std::endl;
	
	
	out << " total secondaries    ";
	printRegionSecondary(out, ETA_ALL,        (float) (m_tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][ETA_ALL]       +m_tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_SECONDARY][ETA_ALL]) );
	out << " secondaries in barrel ";
	printRegionSecondary(out, ETA_BARREL,     (float) (m_tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][ETA_BARREL]    +m_tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_SECONDARY][ETA_BARREL]) );
	out << " secondaries in trans. ";
	printRegionSecondary(out, ETA_TRANSITION, (float) (m_tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][ETA_TRANSITION]+m_tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_SECONDARY][ETA_TRANSITION]) );
	out << " secondaries in endcap ";
	printRegionSecondary(out, ETA_ENDCAP,     (float) (m_tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][ETA_ENDCAP]    +m_tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_SECONDARY][ETA_ENDCAP]) );
	out << " secondaries in forwa. ";
        printRegionSecondary(out, ETA_DBM,     (float) (m_tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][ETA_DBM]    +m_tracks.m_counter[kTracks_gen][TRACK_MULTMATCH_SECONDARY][ETA_DBM]) );
      }
  }
  else out << ": NO EVENTS PROCESSED! " << std::endl;
}

void InDet::TrackStatHelper::printRegionSecondary(MsgStream &out,
                                                  enum eta_region region,
                                                  float denominator) const {
  out  <<  std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(7) << std::setprecision(2)
	     << "\t" << m_tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][region]/(float) m_events;                                              // tracks / event
  if (m_tracks.m_counter[kTracks_gen][TRACK_SECONDARY][region]) {
    out << "\t" << m_tracks.m_counter[kTracks_gen][TRACK_MATCHED_SECONDARY][region]/ (float) m_tracks.m_counter[kTracks_gen][TRACK_SECONDARY][region];                 // track efficiency
  }
  else {
    out << "\t" << "n/a" ;
  }
 
  if (denominator > 0) {     
    out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(4) << std::setprecision(1)
	      <<"   " << float(m_hits.m_counter[kHits_sec][HIT_ALL    ][region]/denominator) << std::setprecision(2)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_PIX1   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_PIX2   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_PIX3   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_SCT1   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_SCT2   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_SCT3   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_SCT4   ][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_SCT5TO9][region]/denominator)
	      << "\t" << float(m_hits.m_counter[kHits_sec][HIT_TRT_ALL][region]/denominator)
	      << std::endl << std::setprecision(-1);
  }
  else {
    out << " Unable to calculate: Denominator=0." << std::endl << std::setprecision(-1);
  } 
}



bool InDet::TrackStatHelper::PassTrackCuts(const Trk::TrackParameters *para) const {
  bool passed = false;
  if(para->pT() >  m_cuts.minPt && fabs(para->eta()) < m_cuts.maxEtaDBM)passed = true;


  return passed;

}

int InDet::TrackStatHelper::ClassifyParticle( const HepMC::GenParticle *particle, const double prob) const {

  int partClass=-999;

  // ME: only classify matched tracks
  if (prob <= m_cuts.matchTrackCut) return partClass;  

  //classify as primary, secondary or truncated
  bool	primary=false;
  bool	secondary=false;
  bool	truncated=false;

  if (particle->production_vertex()){

    // primary vertex inside innermost layer?
    HepGeom::Point3D<double>	startVertex(particle->production_vertex()->position().x(),
			    particle->production_vertex()->position().y(),
			    particle->production_vertex()->position().z());
    if ( fabs(startVertex.perp()) < m_cuts.maxRStartPrimary 
	 && fabs(startVertex.z()) < m_cuts.maxZStartPrimary) { 
      if (particle->end_vertex() == 0) {  
	primary=true;
      }
      else {
	HepGeom::Point3D<double> endVertex(particle->end_vertex()->position().x(), 
			     particle->end_vertex()->position().y(),
			     particle->end_vertex()->position().z());
	if (  endVertex.perp()         > m_cuts.minREndPrimary 
	      || fabs(startVertex.z()) > m_cuts.minZEndPrimary){
	  primary=true;
	}
	else { 
	  truncated = true;
	}
      }
    }
    
    else if ( startVertex.perp() <  m_cuts.maxRStartSecondary && fabs(startVertex.z()) <  m_cuts.maxZStartSecondary) {
      if (particle->end_vertex() == 0) {  
	secondary=true;
      }
      else {
	HepGeom::Point3D<double> endVertex(particle->end_vertex()->position().x(),
			     particle->end_vertex()->position().y(),
			     particle->end_vertex()->position().z());

	if (endVertex.perp()       	> m_cuts.minREndSecondary
	    || fabs(endVertex.z())	> m_cuts.minZEndSecondary) {
	  secondary=true;
	}
      }
    }
  }//end classification

  if(secondary)partClass=TRACK_SECONDARY;
  if(primary)partClass=TRACK_PRIMARY;
  if(truncated)partClass=TRACK_TRUNCATED;

  return partClass;
}
