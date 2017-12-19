/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecValidationNtuples/MuonInsideOutAnalysis.h"
#include "MuonRecValidationNtuples/MuonInsideOutValidationNtuple.h"
#include "MuGirlStau/MuonBetaCalculationUtils.h"


#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include <iostream>

namespace Muon {

  MuonInsideOutAnalysis::MuonInsideOutAnalysis( TDirectory* dir ) {
    m_plots.book(dir,"");
  }

  void MuonInsideOutAnalysis::analyse( TTree& tree ){

    // create ntuple and initialize it
    Muon::MuonInsideOutValidationNtuple ntuple;
    ntuple.init("",&tree,false);
    //tree.SetMakeClass(1);
    // loop over tree
    Long64_t nentries = tree.GetEntries();
    std::cout << " Opening tree " << nentries << std::endl;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      
      // load tree and get entries
      Long64_t centry = tree.LoadTree(jentry);
      if (centry < 0) return;
      tree.GetEntry(jentry);
      
      
      if( jentry%1000 == 0 ){
        std::cout << " new event " << jentry << std::endl;
        std::cout << " tracks " << ntuple.trackParticleBlock.pt->size() << " segments " << ntuple.segmentBlock.quality->size() << std::endl;
      }
      
      // to simplify the analysis we in the beginning look up the indicies of objects corresponding to a given track identifier
      // fill look up tables
      typedef std::map<int,std::vector<int> > TrackMap;
      auto fillTrackMap = []( const std::vector<int>& trkids, TrackMap& trackMap ){
        for( unsigned int i=0;i<trkids.size();++i){
          trackMap[ trkids[i] ].push_back(i);
        }
      };

      TrackMap track_seg_map;
      fillTrackMap( *ntuple.segmentBlock.track.trkid, track_seg_map );

      TrackMap track_hough_map;
      fillTrackMap( *ntuple.houghBlock.track.trkid, track_hough_map );

      TrackMap track_hit_map;
      fillTrackMap( *ntuple.hitBlock.track.trkid, track_hit_map );

      TrackMap track_time_map;
      fillTrackMap( *ntuple.timeBlock.track.trkid, track_time_map );

      TrackMap track_candidate_map;
      fillTrackMap( *ntuple.candidateBlock.track.trkid, track_candidate_map );
      
      // now run the analysis per track
      // loop over tracks
      TrackPlots* trackPlots = 0;
      for( unsigned int i=0;i<ntuple.trackParticleBlock.pt->size();++i){

        //if( (*ntuple.trackParticleBlock.pt)[i] < 50000. ) continue;
        float eta = (*ntuple.trackParticleBlock.eta)[i];
        bool isBarrel = std::abs(eta)<1.05;
        float betaTruth = (*ntuple.trackParticleBlock.truth.beta)[i];
        
        // this loop is ther to deal with the following cases:
        // 1) true muon + segments from the same muon in the MS -> our signal
        // 2) true muon + segments from other particles -> background
        // 3) ID track not a muon -> background
        for( int ii=0;ii<3;++ii ){
          MatchingStrategy matchingStrategy = static_cast<MatchingStrategy>(ii);
          
          // first select track type, first deal with the case the track is a stau
          if( std::abs((*ntuple.trackParticleBlock.truth.pdg)[i]) == 1000015 ) {
            if( matchingStrategy == SameTrack )         trackPlots = &m_plots.stau;
            else if( matchingStrategy == NotSameTrack ) trackPlots = &m_plots.rest;
            else continue;
          }else if( std::abs((*ntuple.trackParticleBlock.truth.pdg)[i]) == 13 ) {
            if( matchingStrategy == SameTrack )         trackPlots = &m_plots.muon;
            else if( matchingStrategy == NotSameTrack ) trackPlots = &m_plots.rest;
            else continue;
          }else{
            // track is not a muon
            if( matchingStrategy == All ) trackPlots = &m_plots.rest;
            else continue;
          }
          
          // count the number of true segments/reconstructed segments/layers with hough maxima
          unsigned int ntruth = 0;
          unsigned int nseg = 0;
          unsigned int nseg1 = 0;
          unsigned int nseg2 = 0;
          unsigned int nseg3 = 0;
          unsigned int nhough = 0;

          // process segments
          TrackMap::iterator pos = track_seg_map.find(i);
          if( pos != track_seg_map.end() ) {
            nseg  = handleSegments(ntuple,i,pos->second,0, matchingStrategy, trackPlots->segments);
            nseg1 = handleSegments(ntuple,i,pos->second,1, matchingStrategy, trackPlots->segments1);
            nseg2 = handleSegments(ntuple,i,pos->second,2, matchingStrategy, trackPlots->segments2);
            nseg3 = handleSegments(ntuple,i,pos->second,3, matchingStrategy, trackPlots->segments3);
          }

          // process hough
          pos = track_hough_map.find(i);
          if( pos != track_hough_map.end() ) {
            nhough = handleHough(ntuple,i,pos->second, matchingStrategy, trackPlots->hough);
          }
          
          // process hits
          pos = track_hit_map.find(i);
          if( pos != track_hit_map.end() ) {
            ntruth = handleHits(ntuple,i,pos->second, matchingStrategy, trackPlots->hits);
          }

          // process times
          pos = track_time_map.find(i);
          if( pos != track_time_map.end() ) {
            handleTimes(ntuple,i,pos->second, matchingStrategy, *trackPlots);
          }

        
          // fill general track block only once per track
          if( matchingStrategy == SameTrack || matchingStrategy == All ) {
            trackPlots->fill(ntuple.trackParticleBlock,i);
            trackPlots->fill(ntruth,nseg,nseg1,nseg2,nseg3,nhough);

            pos = track_candidate_map.find(i);
            // bool justOnCandidate = true;
            // loop over stages and fill plots 
            for( unsigned int si=0;si<trackPlots->candidateStages.size();++si ){
              StageSummaryPlots& plots = trackPlots->candidateStages[si];
              std::pair<unsigned int,unsigned int> candidateCounts(0,0);
              if( pos != track_candidate_map.end() ) candidateCounts = handleCandidates(ntuple,i,pos->second, plots, si );
              if( candidateCounts.first == 0 )  {
                plots.etaMissed->Fill(eta);
                plots.etaBetaMissed->Fill(eta,betaTruth);
                plots.betaMissed->Fill(betaTruth);
              }
              if( candidateCounts.second == 0 ) {
                plots.etaMissedCombined->Fill(eta);
                plots.betaMissedCombined->Fill(betaTruth);
              }
              plots.ncandidates->Fill(candidateCounts.first);
              plots.ncombinedCandidates->Fill(candidateCounts.second);
              // if( candidateCounts.first > 1 ) justOnCandidate = false;
            }
            if( nseg3 < 2 ) continue;

            pos = track_time_map.find(i);
            if( pos != track_time_map.end() ) {
              
              BetaFitRegionPlots& betaRegionPlots = isBarrel ? trackPlots->barrel : trackPlots->endcap;
              
              // loop over input types, RPC + MDT segment or RPC + MDTT
              for( unsigned int co=0;co<2;++co ){

                std::set<int> selection = {1};
                TimePointBetaFit::FitResult betaFitResult = fitBeta( ntuple, pos->second,selection);
                if( betaFitResult.status != 0 ) betaRegionPlots.mdt.fill( betaFitResult.beta, betaTruth, betaFitResult.chi2, betaFitResult.ndof );
                if( isBarrel ){

                  selection = {2};
                  betaFitResult = fitBeta( ntuple, pos->second,selection );
                  if( betaFitResult.status != 0 ) betaRegionPlots.rpc.fill( betaFitResult.beta, betaTruth, betaFitResult.chi2, betaFitResult.ndof );

                  selection = {12};
                  betaFitResult = fitBeta( ntuple, pos->second,selection );
                  if( betaFitResult.status != 0 ) betaRegionPlots.rpct.fill( betaFitResult.beta, betaTruth, betaFitResult.chi2, betaFitResult.ndof );

                  selection = {1,2};
                  betaFitResult = fitBeta( ntuple, pos->second,selection );
                  if( betaFitResult.status != 0 ) betaRegionPlots.all.fill( betaFitResult.beta, betaTruth, betaFitResult.chi2, betaFitResult.ndof );

                  selection = {10,12};
                  betaFitResult = fitBeta( ntuple, pos->second,selection );
                  if( betaFitResult.status != 0 ) {
                    betaRegionPlots.allt.fill( betaFitResult.beta, betaTruth, betaFitResult.chi2, betaFitResult.ndof );
                    if( betaFitResult.ndof > 9 ) betaRegionPlots.allt_good.fill( betaFitResult.beta, betaTruth, betaFitResult.chi2, betaFitResult.ndof );
                  }
                }

                selection = {10};
                betaFitResult = fitBeta( ntuple, pos->second,selection );
                if( betaFitResult.status != 0 ) {
                  betaRegionPlots.mdtt.fill( betaFitResult.beta, betaTruth, betaFitResult.chi2, betaFitResult.ndof );
                  if( betaFitResult.ndof > 9 ) betaRegionPlots.mdtt_good.fill( betaFitResult.beta, betaTruth, betaFitResult.chi2, betaFitResult.ndof );
                }
              }
            }
          }
        }
      }     
    }
  }

  std::pair<unsigned int,unsigned int> MuonInsideOutAnalysis::handleCandidates( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                                                                                const std::vector<int>& indexVec, 
                                                                                StageSummaryPlots& plots, int stage ){

    CandidatePlots& allCandidates = plots.allCandidates; 
    CandidatePlots& tagCandidates = plots.tagCandidates;
    CandidatePlots& combinedCandidates = plots.combinedCandidates;

    float betaTruth = (*ntuple.trackParticleBlock.truth.beta)[trkIndex];

    // loop over hits
    int bestTagCandidateIndex = -1; 
    float bestRes = 1e99;
    int bestCombinedCandidateIndex = -1; 
    float bestCombinedRes = 1e99;
    unsigned int ncandidates = 0;
    unsigned int ncombinedCandidates = 0;
    for( std::vector<int>::const_iterator it=indexVec.begin();it!=indexVec.end();++it ){

      if( *it < 0 || *it >= (int)ntuple.candidateBlock.ntimes->size() ) {
        std::cout << " WARNING, index out of range: " << (*it) << " max " << ntuple.candidateBlock.ntimes->size() << std::endl;
        continue;
      }
      if( (*ntuple.candidateBlock.stage)[*it] != stage ) continue;
      bool hasTrack = (*ntuple.candidateBlock.nprec)[*it] > 0;
      float beta = (*ntuple.candidateBlock.beta)[*it];
      float res = std::abs(beta-betaTruth);
      ++ncandidates;

      allCandidates.fill(ntuple.trackParticleBlock,trkIndex);
      allCandidates.betaCandidates.fill( beta, betaTruth, (*ntuple.candidateBlock.chi2ndof)[*it], (*ntuple.candidateBlock.ntimes)[*it] );

      if( hasTrack ){
        if( res < bestCombinedRes ){
          bestCombinedRes = res;
          bestCombinedCandidateIndex = *it;
        }
        combinedCandidates.fill(ntuple.trackParticleBlock,trkIndex);
        combinedCandidates.betaCandidates.fill( beta, betaTruth, (*ntuple.candidateBlock.chi2ndof)[*it], (*ntuple.candidateBlock.ntimes)[*it] );
        ++ncombinedCandidates;
      }else{
        if( res < bestRes ){
          bestRes = res;
          bestTagCandidateIndex = *it;
        }
        tagCandidates.fill(ntuple.trackParticleBlock,trkIndex);
        tagCandidates.betaCandidates.fill( beta, betaTruth, (*ntuple.candidateBlock.chi2ndof)[*it], (*ntuple.candidateBlock.ntimes)[*it] );
      }

    }
    if( bestCombinedCandidateIndex != -1 ){
      combinedCandidates.betaBestCandidate.fill( (*ntuple.candidateBlock.beta)[bestCombinedCandidateIndex], betaTruth, 
                                                 (*ntuple.candidateBlock.chi2ndof)[bestCombinedCandidateIndex], 
                                                 (*ntuple.candidateBlock.ntimes)[bestCombinedCandidateIndex] );
    }
    if( bestTagCandidateIndex != -1 ){
      tagCandidates.betaBestCandidate.fill( (*ntuple.candidateBlock.beta)[bestTagCandidateIndex], betaTruth, (*ntuple.candidateBlock.chi2ndof)[bestTagCandidateIndex], 
                                            (*ntuple.candidateBlock.ntimes)[bestTagCandidateIndex] );
    }
    int bestCandidateIndex = bestCombinedCandidateIndex != -1 ? bestCombinedCandidateIndex : bestTagCandidateIndex;
    if( bestCandidateIndex != -1 ){
      allCandidates.betaBestCandidate.fill( (*ntuple.candidateBlock.beta)[bestCandidateIndex], betaTruth, (*ntuple.candidateBlock.chi2ndof)[bestCandidateIndex], 
                                            (*ntuple.candidateBlock.ntimes)[bestCandidateIndex] );
    }
    return std::make_pair(ncandidates,ncombinedCandidates);
  }

  unsigned int MuonInsideOutAnalysis::handleSegments( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                                                      const std::vector<int>& indexVec, int stage, 
                                                      MuonInsideOutAnalysis::MatchingStrategy matchingStrategy,
                                                      SegmentPlots& segmentPlots ) {

    unsigned int nseg = 0;
    int barcode_trk = (*ntuple.trackParticleBlock.truth.barcode)[trkIndex];
    float p = (*ntuple.trackParticleBlock.p)[trkIndex]*0.001;
    
    for( std::vector<int>::const_iterator it=indexVec.begin();it!=indexVec.end();++it ){

      bool sameTrack = (*ntuple.segmentBlock.truth.barcode)[*it] == barcode_trk;
      // std::cout << " seg: stage " << (*ntuple.segmentBlock.stage)[*it] << " " << stage
      //           << " bc " << (*ntuple.segmentBlock.truth.barcode)[*it] << " " <<  barcode_trk 
      //           << " str " << matchingStrategy << " ac " << testStrategy(matchingStrategy,sameTrack) << std::endl;
      // select stage + same track only same barcode for now
      if( (*ntuple.segmentBlock.stage)[*it] != stage ) continue;
      ++nseg;
      if( !testStrategy(matchingStrategy,sameTrack) ) continue;
      
      segmentPlots.fill( ntuple.segmentBlock, *it, p, (*ntuple.trackParticleBlock.truth.beta)[trkIndex] );

    }
    // std::cout << " nsegments " << nseg << std::endl;
    return nseg;
  }

  unsigned int MuonInsideOutAnalysis::handleHough( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                                                   const std::vector<int>& indexVec, MuonInsideOutAnalysis::MatchingStrategy matchingStrategy,
                                                   HoughPlots& plots ) {

    unsigned int nhough = 0;
    int barcode_trk = (*ntuple.trackParticleBlock.truth.barcode)[trkIndex];
    float p = (*ntuple.trackParticleBlock.p)[trkIndex]*0.001;
    //std::cout << " handleHough " << indexVec.size() << std::endl;
    for( std::vector<int>::const_iterator it=indexVec.begin();it!=indexVec.end();++it ){
      
      // same track
      bool sameTrack = (*ntuple.houghBlock.truth.barcode)[*it] == barcode_trk;
      if( !testStrategy(matchingStrategy,sameTrack) ) continue;
      
      plots.fill( ntuple.houghBlock, *it, p );

      ++nhough;
    }

    return nhough;
  }

  unsigned int MuonInsideOutAnalysis::handleHits( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                                                  const std::vector<int>& indexVec, MuonInsideOutAnalysis::MatchingStrategy matchingStrategy, 
                                                  HitPlots& plots ) {

    int barcode_trk = (*ntuple.trackParticleBlock.truth.barcode)[trkIndex];
    float p = (*ntuple.trackParticleBlock.p)[trkIndex]*0.001;
    
    // count the number of hits per layer and use that to get the number of 'truth' segments
    std::map<int,int> countsPerLayer;

    for( std::vector<int>::const_iterator it=indexVec.begin();it!=indexVec.end();++it ){
      
      // select stagesame track, only same barcode for now
      bool sameTrack = (*ntuple.hitBlock.truth.barcode)[*it] == barcode_trk;
      if( !testStrategy(matchingStrategy,sameTrack) ) continue;
      ++countsPerLayer[(*ntuple.hitBlock.id.chIndex)[*it]];
      plots.fill( ntuple.hitBlock, *it, p );

    }
    // count layers, require at least 3 hits
    unsigned int nlayers = 0;
    for( std::map<int,int>::iterator it=countsPerLayer.begin();it!=countsPerLayer.end();++it ){
      if( it->second>2 )++nlayers;
    }
    return nlayers;
  }

  bool MuonInsideOutAnalysis::testStrategy( MuonInsideOutAnalysis::MatchingStrategy matchingStrategy, bool sameTrack ) const {
    if( matchingStrategy == SameTrack && !sameTrack )   return false;
    if( matchingStrategy == NotSameTrack && sameTrack ) return false;
    return true;
  }


  void MuonInsideOutAnalysis::handleTimes( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                                           const std::vector<int>& indexVec, MuonInsideOutAnalysis::MatchingStrategy /*matchingStrategy*/,
                                           TrackPlots& trackPlots ) {

    //int barcode_trk = (*ntuple.trackParticleBlock.truth.barcode)[trkIndex];
    //float p = (*ntuple.trackParticleBlock.p)[trkIndex]*0.001;
    float eta = (*ntuple.trackParticleBlock.eta)[trkIndex];
    bool isBarrel = std::abs(eta)<1.05;

    // loop over hits
    for( std::vector<int>::const_iterator it=indexVec.begin();it!=indexVec.end();++it ){

      if( *it < 0 || *it >= (int)ntuple.timeBlock.type->size() ) {
        std::cout << " WARNING, index out of range: " << (*it) << " max " << ntuple.timeBlock.type->size() << std::endl;
        continue;
      }

      int type = (*ntuple.timeBlock.type)[*it];
      // barrel endcap split 
      if( !isBarrel && (type == 1 || type == 10) ) type += 1000;

      // check that we have the correct type
      TimePlots* plots = trackPlots.getTimePlots(type);
      if( !plots ) continue;
      plots->fill( ntuple.timeBlock, *it, (*ntuple.trackParticleBlock.truth.beta)[trkIndex] );
    }
  }

  TimePointBetaFit::FitResult MuonInsideOutAnalysis::fitBeta( MuonInsideOutValidationNtuple& ntuple, 
                                                              const std::vector<int>& indexVec, std::set<int> types ) {

    TimePointBetaFit fitter;
    std::map<int,TimePointBetaFit::HitVec> hitsPerCandidate;

    // loop over hits
    for( std::vector<int>::const_iterator it=indexVec.begin();it!=indexVec.end();++it ){

      if( *it < 0 || *it >= (int)ntuple.timeBlock.type->size() ) {
        std::cout << " WARNING, index out of range: " << (*it) << " max " << ntuple.timeBlock.type->size() << std::endl;
        continue;
      }

      // get hits 
      int type = (*ntuple.timeBlock.type)[*it];
      if( !types.count(type) ) continue;
      float time = (*ntuple.timeBlock.time)[*it];
      float error = (*ntuple.timeBlock.err)[*it];
      int candidateIndex =  (*ntuple.timeBlock.stage)[*it];
      //if( type == 1 || type == 10 ) time -= 1.5;
      //if( type == 12 ) error *= 0.3;
      //if( type == 10 ) time -= (*ntuple.timeBlock.timeCor)[*it];

      hitsPerCandidate[candidateIndex].push_back( TimePointBetaFit::Hit((*ntuple.timeBlock.d)[*it], time, error ) );
    }
    
    std::map<int,TimePointBetaFit::HitVec>::iterator cit = hitsPerCandidate.begin();
    std::map<int,TimePointBetaFit::HitVec>::iterator cit_end = hitsPerCandidate.end();
    if( hitsPerCandidate.size() > 1 ){
      std::cout << " multple candidates for track " << hitsPerCandidate.size() << std::endl;
      fitter.setDebugLevel(10);
    }

    TimePointBetaFit::FitResult result;
    for( ;cit!=cit_end;++cit ){
      /// fit beta 
      TimePointBetaFit::HitVec& hits = cit->second;
      TimePointBetaFit::FitResult candidateResult = fitter.fitWithOutlierLogic(hits);
    
      float chi2ndof = candidateResult.chi2/candidateResult.ndof;
      if( hits.size() > 1 && candidateResult.status != 0 && chi2ndof>5 ){
        MuonBetaCalculationUtils betaUtils(0.);
        std::cout << " Beta " << candidateResult.beta << " of fit for types ";
        for( auto type : types ) std::cout << " " << type;
        std::cout << " with large chi2 " << chi2ndof << " hits " << hits.size() << std::endl;
        for( auto& hit : hits ){
          const char* text = hit.useInFit ? "    hit " : " outlier ";
          std::cout << text << ": dist "<< std::setw(7) << (int)hit.distance 
                    << " time " << std::setprecision(3) << std::setw(6) << hit.time - betaUtils.calculateTof(1.,hit.distance)
                    << " beta " << std::setw(6) << betaUtils.calculateBeta(hit.time,hit.distance) 
                    << " error " << std::setw(6) << hit.error << " residual " << std::setw(6) << hit.residual << " pull " << std::setw(6) << hit.residual/hit.error << std::endl;
        }
      }
      if( candidateResult.status != 0 ){
        if( result.status == 0 || result.ndof < candidateResult.ndof ) result = candidateResult;
      }
    }
    if( hitsPerCandidate.size() > 1 ){
      fitter.setDebugLevel(0);
    }
    return result;
  }

}
