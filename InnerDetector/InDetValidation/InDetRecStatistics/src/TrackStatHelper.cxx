/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "HepMC/GenVertex.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

namespace Trk {

  class RoT_Extractor {
  public:
    static void extract(std::vector<const RIO_OnTrack*>& rots, const std::vector<const MeasurementBase*>& measurements);
  };

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
				      std::vector <std::pair<HepMC::GenParticle *,int> > & gen, 
				      const TrackTruthCollection         * truthMap, 
				      const AtlasDetectorID              * const idHelper, 
				      const PixelID                      * pixelID, 
				      const SCT_ID                       * sctID,
				      Trk::ITrackSummaryTool             * trkSummaryTool,
				      bool                               useTrackSummary,
				      unsigned int                       * inTimeStart,
				      unsigned int                       * inTimeEnd)
{

  m_rttMap.clear();
  
  recoToTruthMap::const_iterator imap;
  
  m_events ++;
  
  float Eta = 0;
  int Region = 0;
  int Author = 0;
  int recoClassification = 0;

  // ------------ reconstructed tracks -----------------
  for (const Trk::Track* track : rec) {

    const Trk::TrackParameters* para = track->trackParameters()->front();    
    recoClassification = -999;
    int thisEventIndex = -999;
    
    Author = track->info().trackFitter();
    if (Author > 0 && Author < Trk::TrackInfo::NumberOfTrackFitters && !m_author_found[Author]){
      m_author_found[Author]  = true;
      m_author_string[Author] = track->info().dumpInfo();
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
      m_truthMissing=true;
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
	m_rttMap.insert(std::pair<HepMcParticleLink,float>(hmpl,trtruth.probability()));
	// ME: remember prob
	trprob = trtruth.probability();
      }
      
    }
    
    
    if(!PassTrackCuts(para)) continue;
    
    m_tracks_rec[TRACK_ALL][Region]++;
    m_tracks_rec[TRACK_ALL][ETA_ALL]++;
    // signal only tracks for the denominator in signal efficiencies
    if (thisEventIndex==0)
      {
	m_tracks_rec[TRACK_ALL_SIGNAL][Region]++;
	m_tracks_rec[TRACK_ALL_SIGNAL][ETA_ALL]++;
      }
    
    // process track summary
    
    const Trk::TrackSummary* summary = NULL;
    
    if (useTrackSummary) {
      summary = trkSummaryTool->createSummary(*track);
	
      if (summary)
	{
	  for (int stype=0; stype < Trk::numberOfTrackSummaryTypes; stype++) {
	    int value = summary->get(static_cast<Trk::SummaryType>(stype));

	    //value is -1 if undefined
	    if (value>0) { 
	      m_TrackSummarySum [TRACK_ALL][Region][stype] += value; 
	      m_n_TrackSummaryOK[TRACK_ALL][Region][stype] ++; 
	    }
	    else {
	      m_n_TrackSummaryBAD [TRACK_ALL][Region][stype] ++; 
	    }
	  }
	}
    }
      
    // check if current reco track has more than a specified fraction of hits
    // from a generated track. [If not, it will be counted as a fake
    // track in the statistics report.]	
      
    if (!truthMap) {
      m_truthMissing=true;
    }
    else {// no truthmap
	
      if (found == truthMap->end()) {
	// m_truthMissing=true;
	// no truth might happen with new truth !
	m_tracks_rec[TRACK_NOHEPMCPARTICLELINK][Region]++;
	m_tracks_rec[TRACK_NOHEPMCPARTICLELINK][ETA_ALL]++;
	if (summary) {
	  for (int type=0; type < Trk::numberOfTrackSummaryTypes; type++) {
	    int value = summary->get(static_cast<Trk::SummaryType>(type));
	    if (value>0) { 
	      m_TrackSummarySum [TRACK_NOHEPMCPARTICLELINK][Region][type] += value; 
	      m_n_TrackSummaryOK[TRACK_NOHEPMCPARTICLELINK][Region][type] ++; 
	    }
	    else {
	      m_n_TrackSummaryBAD [TRACK_NOHEPMCPARTICLELINK][Region][type] ++; 
	    }
	  }
	}
      }
      else{   // no link  
	  
	// ME : change logic, secondaries from G4 processes are truncated, so Barcode 0 is possible 
	const HepMcParticleLink HMPL=trtruth.particleLink();
	if (! HMPL.isValid()) {
	  m_tracks_rec[TRACK_NOHEPMCPARTICLELINK][Region]++;
	  m_tracks_rec[TRACK_NOHEPMCPARTICLELINK][ETA_ALL]++;
	  if (summary) {
	    for (int type=0; type < Trk::numberOfTrackSummaryTypes; type++) {
	      int value = summary->get(static_cast<Trk::SummaryType>(type));
	      if (value>0) { 
		m_TrackSummarySum [TRACK_NOHEPMCPARTICLELINK][Region][type] += value; 
		m_n_TrackSummaryOK[TRACK_NOHEPMCPARTICLELINK][Region][type] ++; 
	      }
	      else {
		m_n_TrackSummaryBAD [TRACK_NOHEPMCPARTICLELINK][Region][type] ++; 
	      }
	    }
	  }
	}
	else {
	  //classify track as coming from primary, secondary or truncated gen particle
	  //	    if(HMPL.isValid()){
	  const HepMC::GenParticle *particle = HMPL.cptr();
	  recoClassification = ClassifyParticle(particle, trprob);
	    
	  if (trprob < m_cuts.fakeTrackCut)
	    {
	      m_tracks_rec[TRACK_LOWTRUTHPROB][Region]++;
	      m_tracks_rec[TRACK_LOWTRUTHPROB][ETA_ALL]++;
	      if (thisEventIndex==0)
		{
		  m_tracks_rec[TRACK_LOWTRUTHPROB_SIGNAL][Region]++;
		  m_tracks_rec[TRACK_LOWTRUTHPROB_SIGNAL][ETA_ALL]++;
		}
	      if (summary) {
		for (int type=0; type < Trk::numberOfTrackSummaryTypes; type++) {
		  int value = summary->get(static_cast<Trk::SummaryType>(type));
		  if (value>0) { 
		    m_TrackSummarySum [TRACK_LOWTRUTHPROB][Region][type] += value; 
		    m_n_TrackSummaryOK [TRACK_LOWTRUTHPROB][Region][type] ++; 
		  }
		  else {
		    m_n_TrackSummaryBAD [TRACK_LOWTRUTHPROB][Region][type] ++; 
		  }
		}
	      }
	    }
	  if (trprob < m_cuts.fakeTrackCut2) {
	    m_tracks_rec[TRACK_LOWTRUTHPROB2][Region]++;
	    m_tracks_rec[TRACK_LOWTRUTHPROB2][ETA_ALL]++;
	    if (thisEventIndex==0)
	      {
		m_tracks_rec[TRACK_LOWTRUTHPROB2_SIGNAL][Region]++;
		m_tracks_rec[TRACK_LOWTRUTHPROB2_SIGNAL][ETA_ALL]++;
	      }
	    if (summary) {
	      for (int type=0; type < Trk::numberOfTrackSummaryTypes; type++) {
		int value = summary->get(static_cast<Trk::SummaryType>(type));
		if (value>0) { 
		  m_TrackSummarySum [TRACK_LOWTRUTHPROB2][Region][type] += value; 
		  m_n_TrackSummaryOK[TRACK_LOWTRUTHPROB2][Region][type] ++; 
		}
		else {
		  m_n_TrackSummaryBAD [TRACK_LOWTRUTHPROB2][Region][type] ++; 
		}
	      }
	    }
	  }
	} // end else on !isVadid
      }
    }
      
    if (summary) delete summary;
    // ------------------ hits on reconstructed tracks ---------------
      
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
	      
	    m_hits_rec[HIT_ALL][Region]++;
	    m_hits_rec[HIT_ALL][ETA_ALL]++;
	      
	    if(recoClassification==TRACK_PRIMARY){
	      m_hits_pri[HIT_ALL][Region]++;
	      m_hits_pri[HIT_ALL][ETA_ALL]++;
	    }
	    if(recoClassification==TRACK_SECONDARY){
	      m_hits_sec[HIT_ALL][Region]++;
	      m_hits_sec[HIT_ALL][ETA_ALL]++;
	    }
	      
	      
	    Identifier id = rio->identify();
	    if (idHelper->is_trt(id)){	      
	      m_hits_rec[HIT_TRT_ALL][Region]++;
	      m_hits_rec[HIT_TRT_ALL][ETA_ALL]++;
	      if(recoClassification==TRACK_PRIMARY){
		m_hits_pri[HIT_TRT_ALL][Region]++;
		m_hits_pri[HIT_TRT_ALL][ETA_ALL]++;
	      }
	      if(recoClassification==TRACK_SECONDARY){
		m_hits_sec[HIT_TRT_ALL][Region]++;
		m_hits_sec[HIT_TRT_ALL][ETA_ALL]++;
	      }	
	    }

	    else if (idHelper->is_sct(id)){
	      m_hits_rec[HIT_SCT_ALL][Region]++;
	      m_hits_rec[HIT_SCT_ALL][ETA_ALL]++;
		 
	      if(recoClassification==TRACK_PRIMARY){
		m_hits_pri[HIT_SCT_ALL][Region]++;
		m_hits_pri[HIT_SCT_ALL][ETA_ALL]++;
	      }
	      if(recoClassification==TRACK_SECONDARY){
		m_hits_sec[HIT_SCT_ALL][Region]++;
		m_hits_sec[HIT_SCT_ALL][ETA_ALL]++;
	      }
	      int HitLayer = HIT_UNKNOWN;
	      if (sctID) {
		switch (sctID->layer_disk(id)) {
		case 0: HitLayer = HIT_SCT1; break;
		case 1: HitLayer = HIT_SCT2; break;
		case 2: HitLayer = HIT_SCT3; break;
		case 3: HitLayer = HIT_SCT4; break;
		case 4: case 5: case 6: case 7: case 8: HitLayer = HIT_SCT5TO9; break;
		default: HitLayer = HIT_UNKNOWN;
		}
	      }
	      m_hits_rec[HitLayer][Region]++;
	      m_hits_rec[HitLayer][ETA_ALL]++;
		 
	      if(recoClassification==TRACK_PRIMARY){
		m_hits_pri[HitLayer][Region]++;
		m_hits_pri[HitLayer][ETA_ALL]++;
	      }
	      if(recoClassification==TRACK_SECONDARY){
		m_hits_sec[HitLayer][Region]++;
		m_hits_sec[HitLayer][ETA_ALL]++;
	      }

	    }
	    else if (idHelper->is_pixel(id)){
              int HitLayer = HIT_UNKNOWN;
	      if (pixelID && pixelID->is_dbm(id)) {
              	m_hits_rec[HIT_DBM_ALL][Region]++;
               	m_hits_rec[HIT_DBM_ALL][ETA_ALL]++;                
               	switch (pixelID->layer_disk(id)) {
               	case 0:  HitLayer = HIT_DBM1; break;
               	case 1:  HitLayer = HIT_DBM2; break;
               	case 2:  HitLayer = HIT_DBM3; break;
               	default: HitLayer = HIT_UNKNOWN;
		}
	      }
	      else {
		m_hits_rec[HIT_PIXEL_ALL][Region]++;
		m_hits_rec[HIT_PIXEL_ALL][ETA_ALL]++;

		if (pixelID) {
		  switch (pixelID->layer_disk(id)) {
		  case 0:  HitLayer = HIT_PIX1; break;
		  case 1:  HitLayer = HIT_PIX2; break;
		  case 2:  HitLayer = HIT_PIX3; break;
		  default: HitLayer = HIT_UNKNOWN;
		  }
		}
	      }
	      m_hits_rec[HitLayer][Region]++;
	      m_hits_rec[HitLayer][ETA_ALL]++;

	      if(recoClassification==TRACK_PRIMARY){
		m_hits_pri[HitLayer][Region]++;
		m_hits_pri[HitLayer][ETA_ALL]++;
	      }
	      if(recoClassification==TRACK_SECONDARY){
		m_hits_sec[HitLayer][Region]++;
		m_hits_sec[HitLayer][ETA_ALL]++;
	      }


	    }
	    else {
	      m_hits_rec[HIT_UNKNOWN][Region]++;
	      m_hits_rec[HIT_UNKNOWN][ETA_ALL]++;

	      if(recoClassification==TRACK_PRIMARY){
		m_hits_pri[HIT_UNKNOWN][Region]++;
		m_hits_pri[HIT_UNKNOWN][ETA_ALL]++;
	      }
	      if(recoClassification==TRACK_SECONDARY){
		m_hits_sec[HIT_UNKNOWN][Region]++;
		m_hits_sec[HIT_UNKNOWN][ETA_ALL]++;
	      }
		  
	    }
	  }
	}
      }
    }
  }
  
  // ------------------------- generated tracks, including pileup -----------------------------
  
  Eta = 0;
  Region = 0;
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
    m_tracks_gen[TRACK_ALL][Region]++;
    m_tracks_gen[TRACK_ALL][ETA_ALL] ++;
    if (inTimePileup) m_tracks_gen[TRACK_ALL_SIGNAL][ETA_ALL]++; // "SIGNAL" is misnomer here
    if (inTimePileup) m_tracks_gen[TRACK_ALL_SIGNAL][Region]++;

    //classify gen as primary, secondary or truncated
    classification = ClassifyParticle(particle,1.);
    
    if(classification==TRACK_PRIMARY){
      m_tracks_gen[TRACK_PRIMARY][ETA_ALL] ++;
      m_tracks_gen[TRACK_PRIMARY][Region]++;
    }
    if(classification==TRACK_SECONDARY){
      m_tracks_gen[TRACK_SECONDARY][ETA_ALL] ++;
      m_tracks_gen[TRACK_SECONDARY][Region]++;
    }
    
    //see if gen track has at least 1 matching reco track with high enough probability
    bool matched = false;
    int nmatched = 0;
    
    HepMcParticleLink hmpl2(particle,truth->second);
    recoToTruthMap::iterator rttIter=m_rttMap.find(hmpl2);
    if(rttIter != m_rttMap.end()){
      for(imap = m_rttMap.lower_bound(hmpl2); imap !=m_rttMap.upper_bound(hmpl2); ++imap){
	if(imap->second > m_cuts.matchTrackCut){
	  matched = true;
	  nmatched++;
	}
      }
    }
    if (matched) {
      m_tracks_gen[TRACK_MATCHED][Region]++;
      m_tracks_gen[TRACK_MATCHED][ETA_ALL]++;
      if (inTimePileup)
	{
	  m_tracks_gen[TRACK_MATCHED_SIGNAL][Region]++;
	  m_tracks_gen[TRACK_MATCHED_SIGNAL][ETA_ALL]++;
	}
      if(nmatched > 1){
	m_tracks_gen[TRACK_MULTMATCH][Region] += nmatched-1;
	m_tracks_gen[TRACK_MULTMATCH][ETA_ALL]+= nmatched-1;
      }
      if(classification==TRACK_PRIMARY){
	m_tracks_gen[TRACK_MATCHED_PRIMARY][Region]++;
	m_tracks_gen[TRACK_MATCHED_PRIMARY][ETA_ALL]++;
	if(nmatched > 1){
	  m_tracks_gen[TRACK_MULTMATCH_PRIMARY][Region]  += nmatched-1;
	  m_tracks_gen[TRACK_MULTMATCH_PRIMARY][ETA_ALL] += nmatched-1;
	}
      }
      if(classification==TRACK_SECONDARY){
	m_tracks_gen[TRACK_MATCHED_SECONDARY][Region]++;
	m_tracks_gen[TRACK_MATCHED_SECONDARY][ETA_ALL]++;
	if(nmatched > 1){
	  m_tracks_gen[TRACK_MULTMATCH_SECONDARY][Region]  += nmatched-1;
	  m_tracks_gen[TRACK_MULTMATCH_SECONDARY][ETA_ALL] += nmatched-1;
	}
      }
    }	
  }
  
  // ------------------------- generated tracks, just signal event -----------------------------
  // For efficiencies.
  
  Eta = 0;
  Region = 0;
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
      m_tracks_gen_signal[TRACK_ALL][ETA_ALL] ++;
      m_tracks_gen_signal[TRACK_ALL][Region]++;
      
      //classify gen partilce as primary, secondary or truncated
      classification = ClassifyParticle(particle,1.);
      
      if(classification==TRACK_PRIMARY){
	m_tracks_gen_signal[TRACK_PRIMARY][ETA_ALL] ++;
	m_tracks_gen_signal[TRACK_PRIMARY][Region]++;
      }
      if(classification==TRACK_SECONDARY){
	m_tracks_gen_signal[TRACK_SECONDARY][ETA_ALL] ++;
	m_tracks_gen_signal[TRACK_SECONDARY][Region]++;
      }
      
      //see if gen track has at least 1 matching reco track with high enough probability
      bool matched = false;
      int nmatched = 0;
      
      HepMcParticleLink hmpl2(particle,truth->second);
      recoToTruthMap::iterator rttIter=m_rttMap.find(hmpl2);
      if(rttIter != m_rttMap.end()){
	for(imap = m_rttMap.lower_bound(hmpl2); imap !=m_rttMap.upper_bound(hmpl2); ++imap){
	  if(imap->second > m_cuts.matchTrackCut){
	    matched = true;
	    nmatched++;
	  }
	}
      }
      if (matched) {
	m_tracks_gen_signal[TRACK_MATCHED][Region]++;
	m_tracks_gen_signal[TRACK_MATCHED][ETA_ALL]++;
	if(nmatched > 1){
	  m_tracks_gen_signal[TRACK_MULTMATCH][Region] += nmatched-1;
	  m_tracks_gen_signal[TRACK_MULTMATCH][ETA_ALL]+= nmatched-1;
	}
	if(classification==TRACK_PRIMARY){
	  m_tracks_gen_signal[TRACK_MATCHED_PRIMARY][Region]++;
	  m_tracks_gen_signal[TRACK_MATCHED_PRIMARY][ETA_ALL]++;
	  if(nmatched > 1){
	    m_tracks_gen_signal[TRACK_MULTMATCH_PRIMARY][Region]  += nmatched-1;
	    m_tracks_gen_signal[TRACK_MULTMATCH_PRIMARY][ETA_ALL] += nmatched-1;
	  }
	}
	if(classification==TRACK_SECONDARY){
	  m_tracks_gen_signal[TRACK_MATCHED_SECONDARY][Region]++;
	  m_tracks_gen_signal[TRACK_MATCHED_SECONDARY][ETA_ALL]++;
	  if(nmatched > 1){
	    m_tracks_gen_signal[TRACK_MULTMATCH_SECONDARY][Region]  += nmatched-1;
	    m_tracks_gen_signal[TRACK_MULTMATCH_SECONDARY][ETA_ALL] += nmatched-1;
	  }
	}
      }	
    }  
}

void InDet::TrackStatHelper::reset(){
  m_events = 0;

  for (int i=0; i<N_TRACKTYPES; i++) {
    for (int j=0; j<N_ETAREGIONS; j++){
      m_tracks_rec[i][j]=0;
      m_tracks_gen[i][j]=0;
      m_tracks_gen_signal[i][j]=0;
      for (int type=0; type < Trk::numberOfTrackSummaryTypes; type++) {
	m_TrackSummarySum   [i][j][type] = 0;
	m_n_TrackSummaryOK  [i][j][type] = 0;
	m_n_TrackSummaryBAD [i][j][type] = 0;
      }
    }
  }

  for (int i=0; i<N_HITTYPES; i++) {
    for (int j=0; j<N_ETAREGIONS; j++) {
      m_hits_rec[i][j]=0;
      m_hits_pri[i][j]=0;
      m_hits_sec[i][j]=0;
      
    }
  }


  
  for (int i=0; i<Trk::TrackInfo::NumberOfTrackFitters; i++)
    m_author_found[i] = false;

  m_truthMissing=false;
}


void InDet::TrackStatHelper::print(){

  std::cout << " Printing Statistics for " << key();

  std::cout << "TrackCollection \"" << key() << "\" " << std::endl ;

  if (m_events > 0) {
    std::cout << "(TrackAuthors:";
    for (int i=0; i<Trk::TrackInfo::NumberOfTrackFitters; i++){
      if (m_author_found[i]){
	std::cout << " " << m_author_string[i];
      }
    }
    std::cout << " )" << std::endl
	      << "TrackTruthCollection \"" << Truthkey() << "\"" << std::endl;
 
    if (m_truthMissing)
      {
	if (m_careAboutTruth)
	  std::cout << " WARNING: TrackTruth missing for part of this TrackCollection, no efficiencies or fakerates included.!" << std::endl; 
	else
	  std::cout << " INFO: Intentionally no TrackTruth for this TrackCollection, no efficiencies or fakerates included." << std::endl;
      }
    if (!m_truthMissing)
      {
	//std::cout << " \t\t\t  ........................................tracks..........................................." << std::endl;
	//std::cout << " \t\t\tn/event\tSEff\tS+Eff\tEff.\tSEff.pr\tSEff.sd\tSLowP1\tSLowP2\tLowP1\tLowP2\tnoLink\tmultM\t" << std::endl;
	std::cout << " \t\t\t  ................................tracks................................" << std::endl;
	std::cout << " \t\t\tn/event\tSEff.pr\tSEff.sd\tSLowP1\tSLowP2\tLowP1\tLowP2\tnoLink\tmultM\t" << std::endl;
	std::cout << " total     "; printRegion1(ETA_ALL);
	std::cout << " in barrel "; printRegion1(ETA_BARREL);
	std::cout << " in trans. "; printRegion1(ETA_TRANSITION);
	std::cout << " in endcap "; printRegion1(ETA_ENDCAP);
	std::cout << " in DBM    "; printRegion1(ETA_DBM);

	      }
    else // no truth
      {
	std::cout << "\t" << "tracks, n/event:" << std::endl;
	std::cout << "\t\t" << "total" << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
	  std::setw(7) << std::setprecision(2) << "\t" << m_tracks_rec[TRACK_ALL][ETA_ALL]/(float) m_events << std::endl << std::setprecision(-1);
	std::cout << "\t\t" << "in barrel" << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
	  std::setw(7) << std::setprecision(2) << "\t" << m_tracks_rec[TRACK_ALL][ETA_BARREL]/(float) m_events << std::endl << std::setprecision(-1);
	std::cout << "\t\t" << "in trans." << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
	  std::setw(7) << std::setprecision(2) << "\t" << m_tracks_rec[TRACK_ALL][ETA_TRANSITION]/(float) m_events << std::endl << std::setprecision(-1);
	std::cout << "\t\t" << "in endcap" << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
	  std::setw(7) << std::setprecision(2) << "\t" << m_tracks_rec[TRACK_ALL][ETA_ENDCAP]/(float) m_events << std::endl << std::setprecision(-1);
	std::cout << "\t\t" << "in DBM" << std::setiosflags(std::ios::fixed | std::ios::showpoint) <<
          std::setw(7) << std::setprecision(2) << "\t" << m_tracks_rec[TRACK_ALL][ETA_DBM]/(float) m_events << std::endl << std::setprecision(-1);
      }
    std::cout << " \t\t\t ....................................hits/track............................" << std::endl;
    std::cout << " \t\t\t\ttotal\tPIX1\tPIX2\tPIX3\tSCT1\tSCT2\tSCT3\tSCT4\tSCT5to9\tStraws\tDBM\tDBM1\tDBM2\tDBM3" << std::endl;
    std::cout << " total     "; printRegion2(ETA_ALL, (float) m_tracks_rec[TRACK_ALL][ETA_ALL]);
    std::cout << " in barrel "; printRegion2(ETA_BARREL, (float) m_tracks_rec[TRACK_ALL][ETA_BARREL]);
    std::cout << " in trans. "; printRegion2(ETA_TRANSITION, (float) m_tracks_rec[TRACK_ALL][ETA_TRANSITION] );
    std::cout << " in endcap "; printRegion2(ETA_ENDCAP, (float) m_tracks_rec[TRACK_ALL][ETA_ENDCAP] );
    std::cout << " in DBM    "; printRegion2(ETA_DBM, (float) m_tracks_rec[TRACK_ALL][ETA_DBM] );

  }
  else
    std::cout << ": NO EVENTS PROCESSED! " << std::endl;
}

void InDet::TrackStatHelper::printRegion1(enum eta_region region){
  std::cout  <<  std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(7) << std::setprecision(2)
	     << "\t" << m_tracks_rec[TRACK_ALL][region]/(float) m_events;
  /*
  // tracks / event
  if (m_tracks_gen_signal[TRACK_ALL][region]) {
    std::cout << "\t" << m_tracks_gen_signal[TRACK_MATCHED][region]/ (float) m_tracks_gen_signal[TRACK_ALL][region];                 // track efficiency for signal
  }
  else {
    std::cout  << "\t" << "n/a" ;
  }
  if (m_tracks_gen[TRACK_ALL_SIGNAL][region]) {
    std::cout << "\t" << m_tracks_gen[TRACK_MATCHED_SIGNAL][region]/ (float) m_tracks_gen[TRACK_ALL_SIGNAL][region];                 // track efficiency for signal + in time pileup
  }
  else {
    std::cout  << "\t" << "n/a" ;
  }
  if (m_tracks_gen[TRACK_ALL][region]) {
    std::cout << "\t" << m_tracks_gen[TRACK_MATCHED][region]/ (float) m_tracks_gen[TRACK_ALL][region];                 // track efficiency
  }
  else {
    std::cout  << "\t" << "n/a" ;
  }
  */
  if (m_tracks_gen_signal[TRACK_PRIMARY][region]) {
    std::cout << "\t" <<  std::setprecision(4) 
	      << m_tracks_gen_signal[TRACK_MATCHED_PRIMARY][region]/ (float) m_tracks_gen_signal[TRACK_PRIMARY][region]     // track efficiency wrt. gen primary in signal
	      <<  std::setprecision(2); 
  }
  else {
    std::cout  << "\t" << "n/a" ;
  }
  if (m_tracks_gen_signal[TRACK_SECONDARY][region]) {
    std::cout << "\t" <<  std::setprecision(4) 
	      << m_tracks_gen_signal[TRACK_MATCHED_SECONDARY][region]/ (float) m_tracks_gen_signal[TRACK_SECONDARY][region]     // track efficiency wrt. gen secondary in signal
	      <<  std::setprecision(2);
  }
  else {
    std::cout  << "\t" << "n/a" ;
  }
  
  if (m_tracks_rec[TRACK_ALL_SIGNAL][region]) {	
    std::cout << "\t" << 100*(m_tracks_rec[TRACK_LOWTRUTHPROB_SIGNAL][region]/ (float) m_tracks_rec[TRACK_ALL_SIGNAL][region]) << "%"  // track fake rate for signal only
	      << "\t" << 100*(m_tracks_rec[TRACK_LOWTRUTHPROB2_SIGNAL][region]/ (float) m_tracks_rec[TRACK_ALL_SIGNAL][region]) << "%";  // track fake rate for signal only
  }
  else {
    std::cout  << "\t" << "n/a\tn/a" ;
  }
  
  if (m_tracks_rec[TRACK_ALL][region]) {	
    std::cout << "\t" << 100*(m_tracks_rec[TRACK_LOWTRUTHPROB][region]/ (float) m_tracks_rec[TRACK_ALL][region]) << "%"  // track fake rate 
	      << "\t" << 100*(m_tracks_rec[TRACK_LOWTRUTHPROB2][region]/ (float) m_tracks_rec[TRACK_ALL][region]) << "%"  // track fake rate 
	      << "\t" << 100*(m_tracks_rec[TRACK_NOHEPMCPARTICLELINK][region]/ (float) m_tracks_rec[TRACK_ALL][region])  << "%" // rate of tracks without HepMcParticleLink
	      << "\t" << 100*(m_tracks_gen[TRACK_MULTMATCH][region]/ (float) m_tracks_rec[TRACK_ALL][region])  << "%"; // rate of tracks matched to the same truth track
    
  }
  else{
    std::cout  << "\tn/a\tn/a\tn/a";
  }
  std::cout << std::endl << std::setprecision(-1);
}

void InDet::TrackStatHelper::printRegion2(enum eta_region region, float denominator){
  std::cout  <<  std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(7) << std::setprecision(2);
  if (denominator > 0) {     
    std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(4) << std::setprecision(1)
	      <<"    \t\t" << float(m_hits_rec[HIT_ALL    ][region]/denominator) << std::setprecision(2) 
	      << "\t" << float(m_hits_rec[HIT_PIX1   ][region]/denominator)
	      << "\t" << float(m_hits_rec[HIT_PIX2   ][region]/denominator)
	      << "\t" << float(m_hits_rec[HIT_PIX3   ][region]/denominator)
	      << "\t" << float(m_hits_rec[HIT_SCT1   ][region]/denominator)
	      << "\t" << float(m_hits_rec[HIT_SCT2   ][region]/denominator)
	      << "\t" << float(m_hits_rec[HIT_SCT3   ][region]/denominator)
	      << "\t" << float(m_hits_rec[HIT_SCT4   ][region]/denominator)
	      << "\t" << float(m_hits_rec[HIT_SCT5TO9][region]/denominator)
	      << "\t" << float(m_hits_rec[HIT_TRT_ALL][region]/denominator)
              << "\t" << float(m_hits_rec[HIT_DBM_ALL][region]/denominator)
              << "\t" << float(m_hits_rec[HIT_DBM1][region]/denominator)
              << "\t" << float(m_hits_rec[HIT_DBM2][region]/denominator)
              << "\t" << float(m_hits_rec[HIT_DBM3][region]/denominator)

	      << std::endl << std::setprecision(-1);
  }
  else {
    std::cout << " Unable to calculate: Denominator=0." << std::endl << std::setprecision(-1);
  } 
}

bool InDet::TrackStatHelper::printTrackSummaryRegion(enum track_types track_type ,
						     enum eta_region eta_region )
{
  long ex_denom =  m_n_TrackSummaryOK[track_type][eta_region][Trk::numberOfPixelHits] + m_n_TrackSummaryBAD[track_type][eta_region][Trk::numberOfPixelHits];
  if (ex_denom > 0)
    {
      std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint) 
		<< std::setw(8) << track_types_string[track_type] 
		<< std::setw(25) << key();
      
      if (m_events)
	std::cout << std::setw(6)  << std::setprecision(2) << m_tracks_rec[track_type][eta_region]/(float) m_events;
      else
	std::cout << std::setw(6) << "n/a";
      
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfInnermostPixelLayerHits );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfInnermostPixelLayerSharedHits );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfInnermostPixelLayerOutliers);
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfPixelHits  );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfPixelSharedHits );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfPixelHoles   );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfGangedPixels  );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfSCTHits );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfSCTSharedHits );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfSCTHoles );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfSCTDoubleHoles);
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfTRTHits   );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfTRTOutliers );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfTRTHighThresholdHits  );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfTRTHighThresholdOutliers );
      std::cout << std::setw(9)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfOutliersOnTrack );
      std::cout << std::setw(6)  << std::setprecision(2); printTrackSummaryAverage( track_type , eta_region, Trk::numberOfDBMHits );
      std::cout << std::endl << std::setprecision(-1);
      return true; // yes, printed output
    }
  else
    return false; // no, didn't print output
}

  void InDet::TrackStatHelper::printTrackSummaryAverage(enum track_types track_type ,
						      enum eta_region eta_region, int summary_type )
{
  long denom =  m_n_TrackSummaryOK[track_type][eta_region][summary_type] + m_n_TrackSummaryBAD[track_type][eta_region][summary_type];

  if (denom > 0) {
    std::cout << float ( m_TrackSummarySum[track_type][eta_region][summary_type]/ (float) denom);
  }
  else {
    std::cout << "n/a";
  }
}

void InDet::TrackStatHelper::printSecondary(){
  std::cout << "TrackCollection \"" << key() << "\" " << std::endl;
  if (m_events > 0) {
    std::cout << "(TrackAuthors:";
    for (int i=0; i<Trk::TrackInfo::NumberOfTrackFitters; i++){
      if (m_author_found[i]){
	std::cout << " " << m_author_string[i];
      }
    }
    std::cout << " )" << std::endl
	      << "TrackTruthCollection \"" << Truthkey() << "\"" << std::endl;
 
    if (m_truthMissing)
      {
	if (m_careAboutTruth)
	  std::cout << " WARNING: TrackTruth missing for part of this TrackCollection --> No secondaries information printed!" << std::endl; 
	else
	  std::cout << " INFO: Intentionally no TrackTruth for this TrackCollection. (No secondaries information printed.)" << std::endl;
      }
    if (!m_truthMissing)
      {
	std::cout << " \t\t\t\t      ......................truth mached tracks statistics....................." << std::endl;
	std::cout << " \t\t\t\tn/event\teff.\ttotal\tPIX1\tPIX2\tPIX3\tSCT1\tSCT2\tSCT3\tSCT4\tSCT5to9\tStraws\tDBMall\tDBM1\tDBM2\tDBM3" << std::endl;
	
	
	std::cout << " total secondaries    ";
	printRegionSecondary( ETA_ALL,        (float) (m_tracks_gen[TRACK_MATCHED_SECONDARY][ETA_ALL]       +m_tracks_gen[TRACK_MULTMATCH_SECONDARY][ETA_ALL]) );
	std::cout << " secondaries in barrel ";
	printRegionSecondary( ETA_BARREL,     (float) (m_tracks_gen[TRACK_MATCHED_SECONDARY][ETA_BARREL]    +m_tracks_gen[TRACK_MULTMATCH_SECONDARY][ETA_BARREL]) );
	std::cout << " secondaries in trans. ";
	printRegionSecondary( ETA_TRANSITION, (float) (m_tracks_gen[TRACK_MATCHED_SECONDARY][ETA_TRANSITION]+m_tracks_gen[TRACK_MULTMATCH_SECONDARY][ETA_TRANSITION]) );
	std::cout << " secondaries in endcap ";
	printRegionSecondary( ETA_ENDCAP,     (float) (m_tracks_gen[TRACK_MATCHED_SECONDARY][ETA_ENDCAP]    +m_tracks_gen[TRACK_MULTMATCH_SECONDARY][ETA_ENDCAP]) );
	std::cout << " secondaries in DBM ";
        printRegionSecondary( ETA_DBM,     (float) (m_tracks_gen[TRACK_MATCHED_SECONDARY][ETA_DBM]    +m_tracks_gen[TRACK_MULTMATCH_SECONDARY][ETA_DBM]) );
      }
  }
  else std::cout << ": NO EVENTS PROCESSED! " << std::endl;
}

void InDet::TrackStatHelper::printRegionSecondary(enum eta_region region, float denominator){
  std::cout  <<  std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(7) << std::setprecision(2)
	     << "\t" << m_tracks_gen[TRACK_MATCHED_SECONDARY][region]/(float) m_events;                                              // tracks / event
  if (m_tracks_gen[TRACK_SECONDARY][region]) {
    std::cout << "\t" << m_tracks_gen[TRACK_MATCHED_SECONDARY][region]/ (float) m_tracks_gen[TRACK_SECONDARY][region];                 // track efficiency
  }
  else {
    std::cout << "\t" << "n/a" ;
  }
  /*
    if (m_tracks_gen[TRACK_PRIMARY][region]) {
    buffer << "\t" <<  std::setprecision(4) 
    << m_tracks_gen[TRACK_MATCHED_PRIMARY][region]/ (float) m_tracks_gen[TRACK_PRIMARY][region]     // track efficiency wrt. gen primary
    <<  std::setprecision(2);
    }
    else {
    buffer  << "\t" << "n/a" ;
    }
    if (m_tracks_gen[TRACK_SECONDARY][region]) {
    buffer << "\t" <<  std::setprecision(4) 
    << m_tracks_gen[TRACK_MATCHED_SECONDARY][region]/ (float) m_tracks_gen[TRACK_SECONDARY][region]     // track efficiency wrt. gen secondary
    <<  std::setprecision(2);
    }
    else {
    buffer  << "\t" << "n/a" ;
    }
      

    if (m_tracks_rec[TRACK_ALL][region]) {	
    buffer << "\t" << 100*(m_tracks_rec[TRACK_LOWTRUTHPROB][region]/ (float) m_tracks_rec[TRACK_ALL][region]) << "%"  // track fake rate 
    << "\t" << 100*(m_tracks_rec[TRACK_LOWTRUTHPROB2][region]/ (float) m_tracks_rec[TRACK_ALL][region]) << "%"  // track fake rate 
    << "\t" << 100*(m_tracks_rec[TRACK_NOHEPMCPARTICLELINK][region]/ (float) m_tracks_rec[TRACK_ALL][region])  << "%" // rate of tracks without HepMcParticleLink
    << "\t" << 100*(m_tracks_gen[TRACK_MULTMATCH][region]/ (float) m_tracks_rec[TRACK_ALL][region])  << "%"; // rate of tracks matched to the same truth track

    }
    else{
    buffer  << "\tn/a\tn/a\tn/a";
    }
  */
  if (denominator > 0) {     
    std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(4) << std::setprecision(1)
	      <<"   " << float(m_hits_sec[HIT_ALL    ][region]/denominator) << std::setprecision(2) 
	      << "\t" << float(m_hits_sec[HIT_PIX1   ][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_PIX2   ][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_PIX3   ][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_SCT1   ][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_SCT2   ][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_SCT3   ][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_SCT4   ][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_SCT5TO9][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_TRT_ALL][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_DBM_ALL][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_DBM1][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_DBM2][region]/denominator)
	      << "\t" << float(m_hits_sec[HIT_DBM3][region]/denominator)
	      << std::endl << std::setprecision(-1);
  }
  else {
    std::cout << " Unable to calculate: Denominator=0." << std::endl << std::setprecision(-1);
  } 
}



bool InDet::TrackStatHelper::PassTrackCuts(const Trk::TrackParameters *para){
  bool passed = false;
  if(para->pT() >  m_cuts.minPt && fabs(para->eta()) < m_cuts.maxEtaDBM)passed = true;


  return passed;

}

int InDet::TrackStatHelper::ClassifyParticle( const HepMC::GenParticle *particle, const double prob){

  int partClass=-999;

  // ME: only classify matched tracks
  if (prob <= m_cuts.matchTrackCut) return partClass;  

  //classify as primary, secondary or truncated
  bool	primary=false;
  bool	secondary=false;
  bool	truncated=false;

  if (particle->production_vertex()){

    // primary vertex inside innermost layer?
    HepGeom::Point3D<double>	startVertex(particle->production_vertex()->point3d().x(),
			    particle->production_vertex()->point3d().y(),
			    particle->production_vertex()->point3d().z());
    if ( fabs(startVertex.perp()) < m_cuts.maxRStartPrimary 
	 && fabs(startVertex.z()) < m_cuts.maxZStartPrimary) { 
      if (particle->end_vertex() == 0) {  
	primary=true;
      }
      else {
	HepGeom::Point3D<double> endVertex(particle->end_vertex()->point3d().x(), 
			     particle->end_vertex()->point3d().y(),
			     particle->end_vertex()->point3d().z());
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
	HepGeom::Point3D<double> endVertex(particle->end_vertex()->point3d().x(),
			     particle->end_vertex()->point3d().y(),
			     particle->end_vertex()->point3d().z());

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
