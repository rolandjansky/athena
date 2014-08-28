/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PAUgammaTruth.h, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////
//
// This class contains truth information related to PAU::gamma
// objects
//

#ifndef PHOTONANALYSISUTILS_PAUGAMMATRUTH_H
#define PHOTONANALYSISUTILS_PAUGAMMATRUTH_H

#include <vector>
class TruthParticle ;
namespace HepMC { class GenParticle ; } 

namespace PAU {

  //-------------------------------------------------------------------------------------
  // class PAU::gammaTruth:
  class gammaTruth {
  public: // methods
    gammaTruth() :
      matchedTruthParticle(0)
      , isPrimaryParticle(false) 
      , truthMothers(0), truthChildren(0) 
      , isTruthConv(false), XtruthConv(UNKNOWN), YtruthConv(UNKNOWN), ZtruthConv(UNKNOWN), RtruthConv(UNKNOWN)
      , leadingTruthParton(0)
      , matchedTrackGenParticle(0)
      {} ;
    ~gammaTruth() {} ;
  public: // data members
    const TruthParticle* matchedTruthParticle ;
    bool isPrimaryParticle ;
    std::vector<const TruthParticle*> truthMothers, truthChildren ;
    bool isTruthConv ;
    float XtruthConv, YtruthConv, ZtruthConv, RtruthConv ;
    const TruthParticle* leadingTruthParton ;
    std::vector<const HepMC::GenParticle*> matchedTrackGenParticle; /// vector of genParticles from track-truth-matching
  } ;

}

#endif // PHOTONANALYSISUTILS_PAUGAMMATRUTH_H
