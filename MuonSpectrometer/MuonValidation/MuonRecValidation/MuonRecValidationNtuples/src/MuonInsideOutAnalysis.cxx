/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecValidationNtuples/MuonInsideOutAnalysis.h"
#include "MuonRecValidationNtuples/MuonInsideOutValidationNtuple.h"
#include "MuonRecValidationNtuples/MuonBetaCalculationUtils.h"


#include "TTree.h"
#include <iostream>

namespace Muon {

  MuonInsideOutAnalysis::MuonInsideOutAnalysis( TDirectory* dir ) {
    plots.book(dir,"");
  }

  void MuonInsideOutAnalysis::analyse( TTree& tree ){
    

    // create ntuple and initialize it
    Muon::MuonInsideOutValidationNtuple ntuple;
    ntuple.init("",&tree,false);
    tree.SetMakeClass(1);
    // loop over tree
    Long64_t nentries = tree.GetEntriesFast();
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
      TrackMap track_seg_map;
      for( unsigned int i=0;i<ntuple.segmentBlock.track.trkid->size();++i){
        track_seg_map[ (*ntuple.segmentBlock.track.trkid)[i] ].push_back(i);
      }

      TrackMap track_hough_map;
      for( unsigned int i=0;i<ntuple.houghBlock.track.trkid->size();++i){
        track_hough_map[ (*ntuple.houghBlock.track.trkid)[i] ].push_back(i);
      }

      TrackMap track_hit_map;
      for( unsigned int i=0;i<ntuple.hitBlock.track.trkid->size();++i){
        track_hit_map[ (*ntuple.hitBlock.track.trkid)[i] ].push_back(i);
      }

      std::cout << " time measurements in the event " << ntuple.timeBlock.track.trkid->size() << std::endl;
      TrackMap track_time_map;
      for( unsigned int i=0;i<ntuple.timeBlock.track.trkid->size();++i){
        std::cout << " adding time measurement to track " << (*ntuple.timeBlock.track.trkid)[i] << " index " << i << std::endl;
        track_time_map[ (*ntuple.timeBlock.track.trkid)[i] ].push_back(i);
      }
      
      // now run the analysis per track
      // loop over tracks
      TrackPlots* trackPlots = 0;
      for( unsigned int i=0;i<ntuple.trackParticleBlock.pt->size();++i){
        
        // this loop is ther to deal with the following cases:
        // 1) true muon + segments from the same muon in the MS -> our signal
        // 2) true muon + segments from other particles -> background
        // 3) ID track not a muon -> background
        for( int ii=0;ii<3;++ii ){
          MatchingStrategy matchingStrategy = static_cast<MatchingStrategy>(ii);
          
          // first select track type, first deal with the case the track is a stau
          if( std::abs((*ntuple.trackParticleBlock.truth.pdg)[i]) == 1000015 ) {
            if( matchingStrategy == SameTrack )         trackPlots = &plots.muon;
            else if( matchingStrategy == NotSameTrack ) trackPlots = &plots.rest;
            else continue;
          }else{
            // track is not a muon
            if( matchingStrategy == All ) trackPlots = &plots.rest;
            else continue;
          }
          
          // count the number of true segments/reconstructed segments/layers with hough maxima
          unsigned int ntruth = 0;
          unsigned int nseg = 0;
          unsigned int nseg1 = 0;
          unsigned int nhough = 0;

          // process segments
          TrackMap::iterator pos = track_seg_map.find(i);
          if( pos != track_seg_map.end() ) {
            nseg  = handleSegments(ntuple,i,pos->second,0, matchingStrategy, trackPlots->segments);
            nseg1 = handleSegments(ntuple,i,pos->second,1, matchingStrategy, trackPlots->segments1);
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
            handleTimes(ntuple,i,pos->second, matchingStrategy, trackPlots->rpcTimes, trackPlots->segmentTimes);
          }

          
          // fill general track block only once per track
          if( matchingStrategy == SameTrack || matchingStrategy == All ) {
            trackPlots->fill(ntuple.trackParticleBlock,i);
            trackPlots->fill(ntruth,nseg,nseg1,nhough);

            pos = track_time_map.find(i);
            if( pos != track_time_map.end() ) {
              
              float betaTrack =  1.;
              if( std::abs((*ntuple.trackParticleBlock.truth.pdg)[i]) == 1000015 ){
                MuonBetaCalculationUtils betaUtils(431000.);
                betaTrack = betaUtils.calculateBetaTrack((*ntuple.trackParticleBlock.p)[i]);
              } 

              TimePointBetaFit::FitResult betaFitResult = fitBeta( ntuple, pos->second,-1 );
              if( betaFitResult.status != 0 ) trackPlots->betaAll.fill( betaFitResult.beta, betaTrack, betaFitResult.chi2, betaFitResult.ndof );

              betaFitResult = fitBeta( ntuple, pos->second, 2 );
              if( betaFitResult.status != 0 ) trackPlots->betaType0.fill( betaFitResult.beta, betaTrack, betaFitResult.chi2, betaFitResult.ndof );

              betaFitResult = fitBeta( ntuple, pos->second, 1 );
              if( betaFitResult.status != 0 ) trackPlots->betaType1.fill( betaFitResult.beta, betaTrack, betaFitResult.chi2, betaFitResult.ndof );
            }
          }
        }
      }     
    }
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
      if( !testStrategy(matchingStrategy,sameTrack) ) continue;
      
      segmentPlots.fill( ntuple.segmentBlock, *it, p );

      ++nseg;
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
                                           const std::vector<int>& indexVec, MuonInsideOutAnalysis::MatchingStrategy matchingStrategy,
                                           TimePlots& plotsType0, TimePlots& plotsType1 ) {

    int barcode_trk = (*ntuple.trackParticleBlock.truth.barcode)[trkIndex];
    float p = (*ntuple.trackParticleBlock.p)[trkIndex]*0.001;

    // loop over hits
    for( std::vector<int>::const_iterator it=indexVec.begin();it!=indexVec.end();++it ){

      if( *it < 0 || *it >= ntuple.timeBlock.type->size() ) {
        std::cout << " WARNING, index out of range: " << (*it) << " max " << ntuple.timeBlock.type->size() << std::endl;
        continue;
      }
      // check that we have the correct type
      TimePlots& plots = (*ntuple.timeBlock.type)[*it] == 2 ? plotsType0 : plotsType1;

      // select stagesame track, only same barcode for now
      //bool sameTrack = (*ntuple.timeBlock.truth.barcode)[*it] == barcode_trk;
      //if( !testStrategy(matchingStrategy,sameTrack) ) continue;
      plots.fill( ntuple.timeBlock, *it, p, (*ntuple.trackParticleBlock.truth.pdg)[trkIndex] );
    }
  }

  TimePointBetaFit::FitResult MuonInsideOutAnalysis::fitBeta( MuonInsideOutValidationNtuple& ntuple, 
                                                              const std::vector<int>& indexVec, int type ) {

    TimePointBetaFit fitter;
    fitter.setDebugLevel(10);
    TimePointBetaFit::HitVec hits;

    // loop over hits
    for( std::vector<int>::const_iterator it=indexVec.begin();it!=indexVec.end();++it ){

      // get hits 
      if( type == -1 || (*ntuple.timeBlock.type)[*it] == type ) hits.push_back( TimePointBetaFit::Hit((*ntuple.timeBlock.d)[*it], (*ntuple.timeBlock.time)[*it], (*ntuple.timeBlock.err)[*it] ) );
    }
    
    /// fit beta 
    TimePointBetaFit::FitResult result = fitter.fitWithOutlierLogic(hits);
    
    float chi2ndof = result.chi2/result.ndof;
    if( hits.size() > 1 && result.status != 0 && chi2ndof>5 ){
      MuonBetaCalculationUtils betaUtils(0.);
      std::cout << " Beta " << result.beta << " of fit for type " << type << " with large chi2 " << chi2ndof << " hits " << hits.size() << std::endl;
      for( auto& hit : hits ){
        std::cout << "  hit: dist "<< std::setw(7) << (int)hit.distance 
                  << " time " << std::setprecision(3) << std::setw(6) << hit.time - betaUtils.calculateTof(1.,hit.distance)
                  << " beta " << std::setw(6) << betaUtils.calculateTof(hit.time,hit.distance) 
                  << " error " << std::setw(6) << hit.error << " residual " << std::setw(6) << hit.residual << " pull " << std::setw(6) << hit.residual/hit.error << std::endl;
      }
    }
    
    return result;
  }

}
