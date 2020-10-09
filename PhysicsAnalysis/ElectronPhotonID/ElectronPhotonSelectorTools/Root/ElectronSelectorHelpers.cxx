/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AsgMessaging/AsgMessaging.h"

// ==================================================================
std::size_t ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(const xAOD::TrackParticle *tp){

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronSelectorHelpers");

  if(!tp){
    msg.msg(MSG::WARNING) << "No track particle found!" << endmsg;
    return 0;
  }

  uint8_t nPixHits(0);
  uint8_t nPixDead(0);

  bool allFound = true;
  allFound = allFound && tp->summaryValue(nPixHits, xAOD::numberOfPixelHits);
  allFound = allFound && tp->summaryValue(nPixDead, xAOD::numberOfPixelDeadSensors);
  if(!allFound){
    msg.msg(MSG::WARNING) << "Failed to load all pixel variables" << endmsg;
  }

  return nPixHits + nPixDead;
}
// ==================================================================
std::size_t ElectronSelectorHelpers::numberOfSCTHitsAndDeadSensors(const xAOD::TrackParticle *tp){

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronSelectorHelpers");

  if(!tp){
    msg.msg(MSG::WARNING) << "No track particle found!" << endmsg;
    return 0;
  }

  uint8_t nSCTHits(0);
  uint8_t nSCTDead(0);

  bool allFound = true;
  allFound = allFound && tp->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
  allFound = allFound && tp->summaryValue(nSCTDead, xAOD::numberOfSCTDeadSensors);
  if(!allFound){
    msg.msg(MSG::WARNING) << "Failed to load all SCT variables" << endmsg;
  }

  return nSCTHits + nSCTDead;
}
// ==================================================================
std::size_t ElectronSelectorHelpers::numberOfSiliconHitsAndDeadSensors(const xAOD::TrackParticle *tp){
  return numberOfPixelHitsAndDeadSensors(tp) + numberOfSCTHitsAndDeadSensors(tp);
}
// ==================================================================
bool ElectronSelectorHelpers::passBLayerRequirement(const xAOD::TrackParticle *tp){

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronSelectorHelpers");

  if(!tp){
    msg.msg(MSG::WARNING) << "No track particle found!" << endmsg;
    return 0;
  }

  uint8_t expectInnermostLayer(0);
  uint8_t nInnermostLayerHits(0);
  uint8_t nInnermostLayerOutliers(0);
  uint8_t expectNextToInnermostLayer(0);
  uint8_t nNextToInnermostLayerHits(0);
  uint8_t nNextToInnermostLayerOutliers(0);

  bool allFound = true;
  allFound = allFound && tp->summaryValue(expectInnermostLayer, xAOD::expectBLayerHit);
  allFound = allFound && tp->summaryValue(nInnermostLayerHits, xAOD::numberOfBLayerHits);
  allFound = allFound && tp->summaryValue(nInnermostLayerOutliers, xAOD::numberOfBLayerOutliers);
  allFound = allFound && tp->summaryValue(expectNextToInnermostLayer, xAOD::expectNextToInnermostPixelLayerHit);
  allFound = allFound && tp->summaryValue(nNextToInnermostLayerHits, xAOD::numberOfNextToInnermostPixelLayerHits);
  allFound = allFound && tp->summaryValue(nNextToInnermostLayerOutliers, xAOD::numberOfNextToInnermostPixelLayerOutliers);
  if(!allFound){
    msg.msg(MSG::WARNING) << "Failed to load all BLayer variables" << endmsg;
  }

  // If innermost and next to innermost layer are both masked off, then the
  // selectors should PASS the BL cut, so the default here is true
  bool passBLReq = true;

  if(expectInnermostLayer){
    passBLReq = ( nInnermostLayerHits + nInnermostLayerOutliers > 0 );
  }
  else if(expectNextToInnermostLayer){
    passBLReq = ( nNextToInnermostLayerHits + nNextToInnermostLayerOutliers > 0 );
  }

  return passBLReq;
}


bool ElectronSelectorHelpers::passAmbiguity(xAOD::AmbiguityTool::AmbiguityType type, const uint8_t criterion){

  // helper to check if ambiguity type is one of several that are stored in a bitmask
  //
  // Christos Rel 22 new ambi.
  //
  // The issue is that the "criterion" encodes what should
  // pass only.
  // If it was not stored in the GroupData and need to keep compatibility
  // this would have been trivial but ....
  //
  // We want reject types
  //  ambiguousVertexEoverPBetterThanTrackEoverP=6,
  //  photon=7,
  //
  // What pass is 0b11111 (31) e.g we do not specify explicitly the bit
  // to cut on. Which of course now breaks.
  // The magic 31 comes from config files which will be updated
  // at some stage for new tunes.
  // And the code below pass for types 0,1,2,3,4 (the 5 bits sets)
  // 2^0 + 2^1  + 2^2 +2^3 +2^4 = 1+2+4+8+16 = 31
  //
  // For now solve ad-hoc and keep compatibility with existing config
  // tooling in GroupArea/PHYS/Derivation etc by adding 32
  //
  //
  return (32+criterion) & 0x1<<type;

}
