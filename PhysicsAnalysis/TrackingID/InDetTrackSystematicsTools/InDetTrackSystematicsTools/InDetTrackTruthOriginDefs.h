// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_INDETTRACKTRUTHORIGINDEFS_H
#define INDETTRACKSYSTEMATICSTOOLS_INDETTRACKTRUTHORIGINDEFS_H

/**
   Definition of track truth origin types
 */
namespace InDet {

  namespace TrkOrigin {

    enum OriginType {
      // fake
      Fake=0,
      // pileup
      Pileup,
      // secondaries
      KshortDecay,
      LambdaDecay,
      GammaConversion,
      OtherDecay,
      HadronicInteraction,
      OtherSecondary,
      // B/D decays
      BHadronDecay,
      DHadronDecay,
      // fragmentation
      Fragmentation,
      // unknown?
      OtherOrigin
    };

    /** Helper functions */
      
    /** fake */
    inline bool isFake(int origin) {
      if (origin & (0x1 << Fake)) return true;
      return false;
    }
    
    /** from pileup */
    inline bool isPileup(int origin) {
      if (origin & (0x1 << Pileup)) return true;
      return false;
    }
    
    /** from V0 */
    inline bool isV0Decay(int origin) {
      if (origin & (0x1 << KshortDecay)) return true;
      if (origin & (0x1 << LambdaDecay)) return true;
      return false;
    }
    
    /** from conversions */
    inline bool isGammaConversion(int origin) {
      if (origin & (0x1 << GammaConversion)) return true;
      return false;
    }
    
    /** from hadronic interactions */
    inline bool isHadronicInteraction(int origin) {
      if (origin & (0x1 << HadronicInteraction)) return true;
      return false;
    }

    /** from long living particle decays or gamma conversions or hadronic interactions and anything else with barcode > 200000 */ 
    inline bool isSecondary(int origin) {
      if (origin & (0x1 << KshortDecay)) return true;
      if (origin & (0x1 << LambdaDecay)) return true;
      if (origin & (0x1 << GammaConversion)) return true;
      if (origin & (0x1 << HadronicInteraction)) return true;
      if (origin & (0x1 << OtherDecay)) return true;
      if (origin & (0x1 << OtherSecondary)) return true;
      return false;
    }

    /** from B decay chain */
    inline bool isFromB(int origin) {
      if (origin & (0x1 << BHadronDecay)) return true;
      return false;
    }

    /** from D decay chain*/
    inline bool isFromD(int origin) {
      if (origin & (0x1 << DHadronDecay)) return true;
      return false;
    }

    /** from D decay chain excluding D from B */
    inline bool isFromDNotFromB(int origin) {
      if (isFromB(origin)) return false;
      if (origin & (0x1 << DHadronDecay)) return true;
      return false;
    }

    /** from from B-->D decay chain */
    inline bool isFromDfromB(int origin) {
      if (isFromB(origin) && isFromD(origin)) return true;
      return false;
    }

    /** from D but not from B-->D */
    inline bool isFragmentation(int origin) {
      if (origin & (0x1 << Fragmentation)) return true;
      return false;
    }

    /** not fake, not pileup, not secondaries */
    inline bool isPrimary(int origin) {
      if (isFake(origin)) return false;
      if (isPileup(origin)) return false;
      if (isSecondary(origin)) return false;
      if (origin & (0x1 << OtherOrigin)) return false;
      return true;
    }

  } // namespace TrkOrigin 

} // namespace InDet

#endif
