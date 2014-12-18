// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPRIMITIVES_ISOLATIONCORRECTIONHELPER_H
#define XAODPRIMITIVES_ISOLATIONCORRECTIONHELPER_H

#include <bitset>
#include <vector>

namespace xAOD {

  namespace Iso {

    /// helper to encode and decode the isolation corrections
    class IsolationCaloCorrectionBitsetHelper {
    public:
      /// decode the bitset into the individual corrections
      static void decode( const IsolationCaloCorrectionBitset& bitset, std::vector<IsolationCaloCorrection>& results ) {
        results.clear();
        for( int i=0;i<static_cast<int>(IsolationCaloCorrection::numIsolationCaloCorrections);++i ){
          if( bitset.test(i) ) results.push_back( static_cast<IsolationCaloCorrection>(i) );
        }
      }
      
      /// encode the bitset given a vector of corrections
      static void encode( const std::vector<IsolationCaloCorrection>& values, IsolationCaloCorrectionBitset& bitset ) {
        bitset.reset();
        for( auto value : values ){
          bitset.set(static_cast<int>(value));
        }
      }      
    };
    
    
  
  
    /// helper to encode and decode the isolation corrections
    class IsolationTrackCorrectionBitsetHelper {
    public:
      /// decode the bitset into the individual corrections
      static void decode( const IsolationTrackCorrectionBitset& bitset, std::vector<IsolationTrackCorrection>& results ) {
        results.clear();
        for( int i=0;i<static_cast<int>(IsolationTrackCorrection::numIsolationTrackCorrections);++i ){
          if( bitset.test(i) ) results.push_back( static_cast<IsolationTrackCorrection>(i) );
        }
      }

      /// encode the bitset given a vector of corrections
      static void encode( const std::vector<IsolationTrackCorrection>& values, IsolationTrackCorrectionBitset& bitset ) {
        bitset.reset();
        for( auto value : values ){
          bitset.set(static_cast<int>(value));
        }
      }      
    };
    
  }
}    
    
#endif
