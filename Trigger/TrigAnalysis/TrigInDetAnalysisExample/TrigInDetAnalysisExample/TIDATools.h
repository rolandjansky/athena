/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDATools.h
 **
 **     @brief   useful tool for the TrigInDetAnalysis class code
 ** 
 **     @author  mark sutton
 **     @date    Tue 16 May 2017 09:28:55 CEST 
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef TrigInDetAnalysisExample_TIDATools_H
#define TrigInDetAnalysisExample_TIDATools_H


#include <vector>

template<typename T>
void HighestPTOnly( std::vector<T*>& tracks ) { 

  if ( tracks.size()>1 ) {
    
    std::vector<T*> tmp_tracks; 
    
    int ih = 0;
    
    for ( unsigned i=1 ; i<tracks.size() ; i++ ) { 
      if ( std::fabs(tracks[i]->pT())>std::fabs(tracks[ih]->pT()) ) ih = i;
    }
    
    tmp_tracks.push_back( tracks[ih] );
    
    tracks = tmp_tracks;
  }
}



template<typename T>
void FilterPT( std::vector<T*>& tracks, double pt ) { 

  std::vector<T*> tmp_tracks; 
  
  tmp_tracks.reserve( tracks.size() );
  
  for ( unsigned i=0 ; i<tracks.size() ; i++ ) { 
    if ( std::fabs(tracks[i]->pT())>=pt ) tmp_tracks.push_back( tracks[i] );
  }

  if ( tmp_tracks.size()<tracks.size() ) tracks = tmp_tracks;

}

 

#endif  // TrigInDetAnalysisExample_TIDATools_H
