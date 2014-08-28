/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PAUegammaTruth.h, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////
//
// This class contains truth information related to PAU::egamma
// objects
//

#ifndef PHOTONANALYSISUTILS_PAUEGAMMATRUTH_H
#define PHOTONANALYSISUTILS_PAUEGAMMATRUTH_H

#include "PAUnamespace.h"

#include <vector>
class TruthParticle ;
namespace HepMC { class GenParticle ; } 

namespace PAU {

  //-------------------------------------------------------------------------------------
  // class PAU::egammaTruth:
  class egammaTruth {

  public: // methods
    egammaTruth() :
      matchedTruthParticle(0)
      , isPrimaryParticle(false) 
      , truthMothers(0)
      , truthChildren(0) 
      , isTruthConv(0)
      , XTruthConv(UNKNOWN)
      , YTruthConv(UNKNOWN)
      , ZTruthConv(UNKNOWN)
      , RTruthConv(UNKNOWN)
      , leadingTruthParton(0)
      , matchedTrackGenParticle(0)
      {} ;
      
    ~egammaTruth() {} ;
      
  public: // data members

    const TruthParticle*                   matchedTruthParticle   ;
    bool                                   isPrimaryParticle      ;
    std::vector<const TruthParticle*>      truthMothers           ;
    std::vector<const TruthParticle*>      truthChildren          ;
    int                                    isTruthConv            ;
    float                                  XTruthConv             ;
    float                                  YTruthConv             ;
    float                                  ZTruthConv             ;
    float                                  RTruthConv             ;
    const TruthParticle*                   leadingTruthParton     ;
    int                                    pdgMother              ;

    int                                    iRecPh                 ;
    float                                  dR                     ;

    bool                                   isHardProc             ;
    bool                                   isBrem                 ;

    // kinematics
    float                                  zvertex                ;
    float                                  eta                    ;
    float                                  phi                    ;
    float                                  pt                     ;
    float                                  e                      ;

    /// vector of genParticles from track-truth-matching
    std::vector<const HepMC::GenParticle*> matchedTrackGenParticle; 

    // parton isolation
    float
      partonIsolation15_UE  ,
      partonIsolation20_UE  ,
      partonIsolation25_UE  ,
      partonIsolation30_UE  ,
      partonIsolation35_UE  ,
      partonIsolation40_UE  ,
      partonIsolation45_UE  ,
      partonIsolation60_UE  ,
      partonIsolation15  ,
      partonIsolation20  ,
      partonIsolation25  ,
      partonIsolation30  ,
      partonIsolation35  ,
      partonIsolation40  ,
      partonIsolation45  ,
      partonIsolation60  ;

    float 
      particleIsolation40,
      particleIsolation40_ED_corrected,
      particleIsolation40_ED_corrected_reco,
      particleIsolation40_ED_corrected_new,
      particleIsolation40_ED_corrected_new_reco;

    float 
      particleIsolation30,
      particleIsolation30_ED_corrected,
      particleIsolation30_ED_corrected_reco,
      particleIsolation30_ED_corrected_new,
      particleIsolation30_ED_corrected_new_reco;

    float 
      particleIsolation20,
      particleIsolation20_ED_corrected,
      particleIsolation20_ED_corrected_reco,
      particleIsolation20_ED_corrected_new,
      particleIsolation20_ED_corrected_new_reco;

    
  } ;

}

#endif // PHOTONANALYSISUTILS_PAUEGAMMATRUTH_H
